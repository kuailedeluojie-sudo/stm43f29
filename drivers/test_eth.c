/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-03-12     MO       the first version
 */
/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-19     SummerGift   first version
 * 2018-12-25     zylx         fix some bugs
 * 2019-06-10     SummerGift   optimize PHY state detection process
 * 2019-09-03     xiaofan      optimize link change detection process
 */
#if 1
#include "board.h"
#include <rtdbg.h>
#include <netif/ethernetif.h>
#include "lwipopts.h"
#include "test_eth.h"

#include <rtthread.h>
#include <board.h>
#include <rtdevice.h>
/*
* Emac driver uses CubeMX tool to generate emac and phy's configuration,
* the configuration files can be found in CubeMX_Config folder.
*/

/* debug option */
//#define ETH_RX_DUMP
//#define ETH_TX_DUMP
//#define DRV_DEBUG
#define LOG_TAG             "drv.emac"

#define ETH_TIMEOUT_SWRESET               500U  
#define ETH_TIMEOUT_LINKED_STATE          5000U
#define ETH_TIMEOUT_AUTONEGO_COMPLETED    5000U
#define MAX_ADDR_LEN 6
static void ETH_MACDMAConfig(ETH_HandleTypeDef *heth, uint32_t err);
static void ETH_MACAddressConfig(ETH_HandleTypeDef *heth, uint32_t MacAddr, uint8_t *Addr);
static void ETH_MACReceptionEnable(ETH_HandleTypeDef *heth);
static void ETH_MACTransmissionEnable(ETH_HandleTypeDef *heth);
static void ETH_DMATransmissionEnable(ETH_HandleTypeDef *heth);
static void ETH_DMAReceptionEnable(ETH_HandleTypeDef *heth);

static void ETH_FlushTransmitFIFO(ETH_HandleTypeDef *heth);

struct rt_stm32_eth
{
    /* inherit from ethernet device */
    struct eth_device parent;
#ifndef PHY_USING_INTERRUPT_MODE
    rt_timer_t poll_link_timer;
#endif

    /* interface address info, hw address */
    rt_uint8_t  dev_addr[MAX_ADDR_LEN];
    /* ETH_Speed */
    uint32_t    ETH_Speed;
    /* ETH_Duplex_Mode */
    uint32_t    ETH_Mode;
};

static ETH_DMADescTypeDef *DMARxDscrTab, *DMATxDscrTab;
static rt_uint8_t *Rx_Buff, *Tx_Buff;
static  ETH_HandleTypeDef EthHandle;
static struct rt_stm32_eth stm32_eth_device;

#if defined(ETH_RX_DUMP) || defined(ETH_TX_DUMP)
#define __is_print(ch) ((unsigned int)((ch) - ' ') < 127u - ' ')
static void dump_hex(const rt_uint8_t *ptr, rt_size_t buflen)
{
    unsigned char *buf = (unsigned char *)ptr;
    int i, j;

    for (i = 0; i < buflen; i += 16)
    {
        rt_kprintf("%08X: ", i);

        for (j = 0; j < 16; j++)
            if (i + j < buflen)
                rt_kprintf("%02X ", buf[i + j]);
            else
                rt_kprintf("   ");
        rt_kprintf(" ");

        for (j = 0; j < 16; j++)
            if (i + j < buflen)
                rt_kprintf("%c", __is_print(buf[i + j]) ? buf[i + j] : '.');
        rt_kprintf("\n");
    }
}
#endif
#if 1
/**
  * @brief  Enables the DMA reception.
  * @param  heth pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module 
  * @retval None
  */
static void ETH_DMAReceptionEnable(ETH_HandleTypeDef *heth)
{  
  /* Enable the DMA reception */
  (heth->Instance)->DMAOMR |= ETH_DMAOMR_SR;  
}
/**
  * @brief  Configures the selected MAC address.
  * @param  heth pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  MacAddr The MAC address to configure
  *          This parameter can be one of the following values:
  *             @arg ETH_MAC_Address0: MAC Address0 
  *             @arg ETH_MAC_Address1: MAC Address1 
  *             @arg ETH_MAC_Address2: MAC Address2
  *             @arg ETH_MAC_Address3: MAC Address3
  * @param  Addr Pointer to MAC address buffer data (6 bytes)
  * @retval HAL status
  */
static void ETH_MACAddressConfig(ETH_HandleTypeDef *heth, uint32_t MacAddr, uint8_t *Addr)
{
  uint32_t tmpreg1;
  
  /* Prevent unused argument(s) compilation warning */
  UNUSED(heth);

  /* Check the parameters */
  assert_param(IS_ETH_MAC_ADDRESS0123(MacAddr));
  
  /* Calculate the selected MAC address high register */
  tmpreg1 = ((uint32_t)Addr[5U] << 8U) | (uint32_t)Addr[4U];
  /* Load the selected MAC address high register */
  (*(__IO uint32_t *)((uint32_t)(ETH_MAC_ADDR_HBASE + MacAddr))) = tmpreg1;
  /* Calculate the selected MAC address low register */
  tmpreg1 = ((uint32_t)Addr[3U] << 24U) | ((uint32_t)Addr[2U] << 16U) | ((uint32_t)Addr[1U] << 8U) | Addr[0U];
  
  /* Load the selected MAC address low register */
  (*(__IO uint32_t *)((uint32_t)(ETH_MAC_ADDR_LBASE + MacAddr))) = tmpreg1;
}

/**
  * @brief  This function provides delay (in milliseconds) based on CPU cycles method.
  * @param  mdelay specifies the delay time length, in milliseconds.
  * @retval None
  */
static void ETH_Delay(uint32_t mdelay)
{
  __IO uint32_t Delay = mdelay * (SystemCoreClock / 8U / 1000U);
  do 
  {
    __NOP();
  } 
  while (Delay --);
}


/**
  * @brief  Tx Transfer completed callbacks.
  * @param  heth pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @retval None
  */
__weak void HAL_ETH_TxCpltCallback(ETH_HandleTypeDef *heth)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(heth);
  /* NOTE : This function Should not be modified, when the callback is needed,
  the HAL_ETH_TxCpltCallback could be implemented in the user file
  */ 
}



/**
  * @brief  Enables the DMA transmission.
  * @param  heth pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module   
  * @retval None
  */
static void ETH_DMATransmissionEnable(ETH_HandleTypeDef *heth)
{
  /* Enable the DMA transmission */
  (heth->Instance)->DMAOMR |= ETH_DMAOMR_ST;  
}

/**
  * @brief  Enables the MAC reception.
  * @param  heth pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module   
  * @retval None
  */
static void ETH_MACReceptionEnable(ETH_HandleTypeDef *heth)
{ 
  __IO uint32_t tmpreg1 = 0U;
  
  /* Enable the MAC reception */
  (heth->Instance)->MACCR |= ETH_MACCR_RE;
  
  /* Wait until the write operation will be taken into account:
     at least four TX_CLK/RX_CLK clock cycles */
  tmpreg1 = (heth->Instance)->MACCR;
  ETH_Delay(ETH_REG_WRITE_DELAY);
  (heth->Instance)->MACCR = tmpreg1;
}
/**
  * @brief  Clears the ETHERNET transmit FIFO.
  * @param  heth pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @retval None
  */
static void ETH_FlushTransmitFIFO(ETH_HandleTypeDef *heth)
{
  __IO uint32_t tmpreg1 = 0U;
  
  /* Set the Flush Transmit FIFO bit */
  (heth->Instance)->DMAOMR |= ETH_DMAOMR_FTF;
  
  /* Wait until the write operation will be taken into account:
     at least four TX_CLK/RX_CLK clock cycles */
  tmpreg1 = (heth->Instance)->DMAOMR;
  ETH_Delay(ETH_REG_WRITE_DELAY);
  (heth->Instance)->DMAOMR = tmpreg1;
}


/**
  * @brief  Enables the MAC transmission.
  * @param  heth pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module  
  * @retval None
  */
static void ETH_MACTransmissionEnable(ETH_HandleTypeDef *heth)
{ 
  __IO uint32_t tmpreg1 = 0U;
  
  /* Enable the MAC transmission */
  (heth->Instance)->MACCR |= ETH_MACCR_TE;
  
  /* Wait until the write operation will be taken into account:
     at least four TX_CLK/RX_CLK clock cycles */
  tmpreg1 = (heth->Instance)->MACCR;
  ETH_Delay(ETH_REG_WRITE_DELAY);
  (heth->Instance)->MACCR = tmpreg1;
}


/**
  * @brief  Configures Ethernet MAC and DMA with default parameters.
  * @param  heth pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  err Ethernet Init error
  * @retval HAL status
  */
static void ETH_MACDMAConfig(ETH_HandleTypeDef *heth, uint32_t err)
{
  ETH_MACInitTypeDef macinit;
  ETH_DMAInitTypeDef dmainit;
  uint32_t tmpreg1 = 0U;
  
  if (err != ETH_SUCCESS) /* Auto-negotiation failed */
  {
    /* Set Ethernet duplex mode to Full-duplex */
    (heth->Init).DuplexMode = ETH_MODE_FULLDUPLEX;
    
    /* Set Ethernet speed to 100M */
    (heth->Init).Speed = ETH_SPEED_100M;
  }
  
  /* Ethernet MAC default initialization **************************************/
  macinit.Watchdog = ETH_WATCHDOG_ENABLE;
  macinit.Jabber = ETH_JABBER_ENABLE;
  macinit.InterFrameGap = ETH_INTERFRAMEGAP_96BIT;
  macinit.CarrierSense = ETH_CARRIERSENCE_ENABLE;
  macinit.ReceiveOwn = ETH_RECEIVEOWN_ENABLE;
  macinit.LoopbackMode = ETH_LOOPBACKMODE_DISABLE;
  if(heth->Init.ChecksumMode == ETH_CHECKSUM_BY_HARDWARE)
  {
    macinit.ChecksumOffload = ETH_CHECKSUMOFFLAOD_ENABLE;
  }
  else
  {
    macinit.ChecksumOffload = ETH_CHECKSUMOFFLAOD_DISABLE;
  }
  macinit.RetryTransmission = ETH_RETRYTRANSMISSION_DISABLE;
  macinit.AutomaticPadCRCStrip = ETH_AUTOMATICPADCRCSTRIP_DISABLE;
  macinit.BackOffLimit = ETH_BACKOFFLIMIT_10;
  macinit.DeferralCheck = ETH_DEFFERRALCHECK_DISABLE;
  macinit.ReceiveAll = ETH_RECEIVEAll_DISABLE;
  macinit.SourceAddrFilter = ETH_SOURCEADDRFILTER_DISABLE;
  macinit.PassControlFrames = ETH_PASSCONTROLFRAMES_BLOCKALL;
  macinit.BroadcastFramesReception = ETH_BROADCASTFRAMESRECEPTION_ENABLE;
  macinit.DestinationAddrFilter = ETH_DESTINATIONADDRFILTER_NORMAL;
  macinit.PromiscuousMode = ETH_PROMISCUOUS_MODE_DISABLE;
  macinit.MulticastFramesFilter = ETH_MULTICASTFRAMESFILTER_PERFECT;
  macinit.UnicastFramesFilter = ETH_UNICASTFRAMESFILTER_PERFECT;
  macinit.HashTableHigh = 0x0U;
  macinit.HashTableLow = 0x0U;
  macinit.PauseTime = 0x0U;
  macinit.ZeroQuantaPause = ETH_ZEROQUANTAPAUSE_DISABLE;
  macinit.PauseLowThreshold = ETH_PAUSELOWTHRESHOLD_MINUS4;
  macinit.UnicastPauseFrameDetect = ETH_UNICASTPAUSEFRAMEDETECT_DISABLE;
  macinit.ReceiveFlowControl = ETH_RECEIVEFLOWCONTROL_DISABLE;
  macinit.TransmitFlowControl = ETH_TRANSMITFLOWCONTROL_DISABLE;
  macinit.VLANTagComparison = ETH_VLANTAGCOMPARISON_16BIT;
  macinit.VLANTagIdentifier = 0x0U;
  
  /*------------------------ ETHERNET MACCR Configuration --------------------*/
  /* Get the ETHERNET MACCR value */
  tmpreg1 = (heth->Instance)->MACCR;
  /* Clear WD, PCE, PS, TE and RE bits */
  tmpreg1 &= ETH_MACCR_CLEAR_MASK;
  /* Set the WD bit according to ETH Watchdog value */
  /* Set the JD: bit according to ETH Jabber value */
  /* Set the IFG bit according to ETH InterFrameGap value */
  /* Set the DCRS bit according to ETH CarrierSense value */
  /* Set the FES bit according to ETH Speed value */ 
  /* Set the DO bit according to ETH ReceiveOwn value */ 
  /* Set the LM bit according to ETH LoopbackMode value */
  /* Set the DM bit according to ETH Mode value */ 
  /* Set the IPCO bit according to ETH ChecksumOffload value */
  /* Set the DR bit according to ETH RetryTransmission value */
  /* Set the ACS bit according to ETH AutomaticPadCRCStrip value */
  /* Set the BL bit according to ETH BackOffLimit value */
  /* Set the DC bit according to ETH DeferralCheck value */
  tmpreg1 |= (uint32_t)(macinit.Watchdog | 
                       macinit.Jabber | 
                       macinit.InterFrameGap |
                       macinit.CarrierSense |
                       (heth->Init).Speed | 
                       macinit.ReceiveOwn |
                       macinit.LoopbackMode |
                       (heth->Init).DuplexMode | 
                       macinit.ChecksumOffload |    
                       macinit.RetryTransmission | 
                       macinit.AutomaticPadCRCStrip | 
                       macinit.BackOffLimit | 
                       macinit.DeferralCheck);
  
  /* Write to ETHERNET MACCR */
  (heth->Instance)->MACCR = (uint32_t)tmpreg1;
  
  /* Wait until the write operation will be taken into account:
     at least four TX_CLK/RX_CLK clock cycles */
  tmpreg1 = (heth->Instance)->MACCR;
  HAL_Delay(ETH_REG_WRITE_DELAY);
  (heth->Instance)->MACCR = tmpreg1; 
  
  /*----------------------- ETHERNET MACFFR Configuration --------------------*/ 
  /* Set the RA bit according to ETH ReceiveAll value */
  /* Set the SAF and SAIF bits according to ETH SourceAddrFilter value */
  /* Set the PCF bit according to ETH PassControlFrames value */
  /* Set the DBF bit according to ETH BroadcastFramesReception value */
  /* Set the DAIF bit according to ETH DestinationAddrFilter value */
  /* Set the PR bit according to ETH PromiscuousMode value */
  /* Set the PM, HMC and HPF bits according to ETH MulticastFramesFilter value */
  /* Set the HUC and HPF bits according to ETH UnicastFramesFilter value */
  /* Write to ETHERNET MACFFR */  
  (heth->Instance)->MACFFR = (uint32_t)(macinit.ReceiveAll | 
                                        macinit.SourceAddrFilter |
                                        macinit.PassControlFrames |
                                        macinit.BroadcastFramesReception | 
                                        macinit.DestinationAddrFilter |
                                        macinit.PromiscuousMode |
                                        macinit.MulticastFramesFilter |
                                        macinit.UnicastFramesFilter);
   
   /* Wait until the write operation will be taken into account:
      at least four TX_CLK/RX_CLK clock cycles */
   tmpreg1 = (heth->Instance)->MACFFR;
   HAL_Delay(ETH_REG_WRITE_DELAY);
   (heth->Instance)->MACFFR = tmpreg1;
   
   /*--------------- ETHERNET MACHTHR and MACHTLR Configuration --------------*/
   /* Write to ETHERNET MACHTHR */
   (heth->Instance)->MACHTHR = (uint32_t)macinit.HashTableHigh;
   
   /* Write to ETHERNET MACHTLR */
   (heth->Instance)->MACHTLR = (uint32_t)macinit.HashTableLow;
   /*----------------------- ETHERNET MACFCR Configuration -------------------*/
   
   /* Get the ETHERNET MACFCR value */  
   tmpreg1 = (heth->Instance)->MACFCR;
   /* Clear xx bits */
   tmpreg1 &= ETH_MACFCR_CLEAR_MASK;
   
   /* Set the PT bit according to ETH PauseTime value */
   /* Set the DZPQ bit according to ETH ZeroQuantaPause value */
   /* Set the PLT bit according to ETH PauseLowThreshold value */
   /* Set the UP bit according to ETH UnicastPauseFrameDetect value */
   /* Set the RFE bit according to ETH ReceiveFlowControl value */
   /* Set the TFE bit according to ETH TransmitFlowControl value */ 
   tmpreg1 |= (uint32_t)((macinit.PauseTime << 16U) | 
                        macinit.ZeroQuantaPause |
                        macinit.PauseLowThreshold |
                        macinit.UnicastPauseFrameDetect | 
                        macinit.ReceiveFlowControl |
                        macinit.TransmitFlowControl); 
   
   /* Write to ETHERNET MACFCR */
   (heth->Instance)->MACFCR = (uint32_t)tmpreg1;
   
   /* Wait until the write operation will be taken into account:
   at least four TX_CLK/RX_CLK clock cycles */
   tmpreg1 = (heth->Instance)->MACFCR;
   HAL_Delay(ETH_REG_WRITE_DELAY);
   (heth->Instance)->MACFCR = tmpreg1;
   
   /*----------------------- ETHERNET MACVLANTR Configuration ----------------*/
   /* Set the ETV bit according to ETH VLANTagComparison value */
   /* Set the VL bit according to ETH VLANTagIdentifier value */  
   (heth->Instance)->MACVLANTR = (uint32_t)(macinit.VLANTagComparison | 
                                            macinit.VLANTagIdentifier);
    
    /* Wait until the write operation will be taken into account:
       at least four TX_CLK/RX_CLK clock cycles */
    tmpreg1 = (heth->Instance)->MACVLANTR;
    HAL_Delay(ETH_REG_WRITE_DELAY);
    (heth->Instance)->MACVLANTR = tmpreg1;
    
    /* Ethernet DMA default initialization ************************************/
    dmainit.DropTCPIPChecksumErrorFrame = ETH_DROPTCPIPCHECKSUMERRORFRAME_ENABLE;
    dmainit.ReceiveStoreForward = ETH_RECEIVESTOREFORWARD_ENABLE;
    dmainit.FlushReceivedFrame = ETH_FLUSHRECEIVEDFRAME_ENABLE;
    dmainit.TransmitStoreForward = ETH_TRANSMITSTOREFORWARD_ENABLE;  
    dmainit.TransmitThresholdControl = ETH_TRANSMITTHRESHOLDCONTROL_64BYTES;
    dmainit.ForwardErrorFrames = ETH_FORWARDERRORFRAMES_DISABLE;
    dmainit.ForwardUndersizedGoodFrames = ETH_FORWARDUNDERSIZEDGOODFRAMES_DISABLE;
    dmainit.ReceiveThresholdControl = ETH_RECEIVEDTHRESHOLDCONTROL_64BYTES;
    dmainit.SecondFrameOperate = ETH_SECONDFRAMEOPERARTE_ENABLE;
    dmainit.AddressAlignedBeats = ETH_ADDRESSALIGNEDBEATS_ENABLE;
    dmainit.FixedBurst = ETH_FIXEDBURST_ENABLE;
    dmainit.RxDMABurstLength = ETH_RXDMABURSTLENGTH_32BEAT;
    dmainit.TxDMABurstLength = ETH_TXDMABURSTLENGTH_32BEAT;
    dmainit.EnhancedDescriptorFormat = ETH_DMAENHANCEDDESCRIPTOR_ENABLE;
    dmainit.DescriptorSkipLength = 0x0U;
    dmainit.DMAArbitration = ETH_DMAARBITRATION_ROUNDROBIN_RXTX_1_1;
    
    /* Get the ETHERNET DMAOMR value */
    tmpreg1 = (heth->Instance)->DMAOMR;
    /* Clear xx bits */
    tmpreg1 &= ETH_DMAOMR_CLEAR_MASK;
    
    /* Set the DT bit according to ETH DropTCPIPChecksumErrorFrame value */
    /* Set the RSF bit according to ETH ReceiveStoreForward value */
    /* Set the DFF bit according to ETH FlushReceivedFrame value */
    /* Set the TSF bit according to ETH TransmitStoreForward value */
    /* Set the TTC bit according to ETH TransmitThresholdControl value */
    /* Set the FEF bit according to ETH ForwardErrorFrames value */
    /* Set the FUF bit according to ETH ForwardUndersizedGoodFrames value */
    /* Set the RTC bit according to ETH ReceiveThresholdControl value */
    /* Set the OSF bit according to ETH SecondFrameOperate value */
    tmpreg1 |= (uint32_t)(dmainit.DropTCPIPChecksumErrorFrame | 
                         dmainit.ReceiveStoreForward |
                         dmainit.FlushReceivedFrame |
                         dmainit.TransmitStoreForward | 
                         dmainit.TransmitThresholdControl |
                         dmainit.ForwardErrorFrames |
                         dmainit.ForwardUndersizedGoodFrames |
                         dmainit.ReceiveThresholdControl |
                         dmainit.SecondFrameOperate);
    
    /* Write to ETHERNET DMAOMR */
    (heth->Instance)->DMAOMR = (uint32_t)tmpreg1;
    
    /* Wait until the write operation will be taken into account:
       at least four TX_CLK/RX_CLK clock cycles */
    tmpreg1 = (heth->Instance)->DMAOMR;
    HAL_Delay(ETH_REG_WRITE_DELAY);
    (heth->Instance)->DMAOMR = tmpreg1;
    
    /*----------------------- ETHERNET DMABMR Configuration ------------------*/
    /* Set the AAL bit according to ETH AddressAlignedBeats value */
    /* Set the FB bit according to ETH FixedBurst value */
    /* Set the RPBL and 4*PBL bits according to ETH RxDMABurstLength value */
    /* Set the PBL and 4*PBL bits according to ETH TxDMABurstLength value */
    /* Set the Enhanced DMA descriptors bit according to ETH EnhancedDescriptorFormat value*/
    /* Set the DSL bit according to ETH DesciptorSkipLength value */
    /* Set the PR and DA bits according to ETH DMAArbitration value */
    (heth->Instance)->DMABMR = (uint32_t)(dmainit.AddressAlignedBeats | 
                                          dmainit.FixedBurst |
                                          dmainit.RxDMABurstLength |    /* !! if 4xPBL is selected for Tx or Rx it is applied for the other */
                                          dmainit.TxDMABurstLength |
                                          dmainit.EnhancedDescriptorFormat |
                                          (dmainit.DescriptorSkipLength << 2U) |
                                          dmainit.DMAArbitration |
                                          ETH_DMABMR_USP); /* Enable use of separate PBL for Rx and Tx */
     
     /* Wait until the write operation will be taken into account:
        at least four TX_CLK/RX_CLK clock cycles */
     tmpreg1 = (heth->Instance)->DMABMR;
     HAL_Delay(ETH_REG_WRITE_DELAY);
     (heth->Instance)->DMABMR = tmpreg1;

     if((heth->Init).RxMode == ETH_RXINTERRUPT_MODE)
     {
       /* Enable the Ethernet Rx Interrupt */
       __HAL_ETH_DMA_ENABLE_IT((heth), ETH_DMA_IT_NIS | ETH_DMA_IT_R);
     }

     /* Initialize MAC address in ethernet MAC */ 
     ETH_MACAddressConfig(heth, ETH_MAC_ADDRESS0, heth->Init.MACAddr);
}

/**
  * @brief  Initializes the ETH MSP.
  * @param  heth pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @retval None
  */
__weak void HAL_ETH_MspInit(ETH_HandleTypeDef *heth)
{
  /* Prevent unused argument(s) compilation warning */
 // UNUSED(heth);
  /* NOTE : This function Should not be modified, when the callback is needed,
  the HAL_ETH_MspInit could be implemented in the user file
  */
    GPIO_InitTypeDef GPIO_InitStruct = {0};
      if(heth->Instance==ETH)
      {
      /* USER CODE BEGIN ETH_MspInit 0 */

      /* USER CODE END ETH_MspInit 0 */
        /* Peripheral clock enable */
        __HAL_RCC_ETH_CLK_ENABLE();

        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOB_CLK_ENABLE();
        /**ETH GPIO Configuration
        PC1     ------> ETH_MDC
        PA1     ------> ETH_REF_CLK
        PA2     ------> ETH_MDIO
        PA7     ------> ETH_CRS_DV
        PC4     ------> ETH_RXD0
        PC5     ------> ETH_RXD1
        PB11     ------> ETH_TX_EN
        PB12     ------> ETH_TXD0
        PB13     ------> ETH_TXD1
        */
        GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_7;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

      /* USER CODE BEGIN ETH_MspInit 1 */

      /* USER CODE END ETH_MspInit 1 */
      }


}


/**
  * @brief  DeInitializes ETH MSP.
  * @param  heth pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @retval None
  */
__weak void HAL_ETH_MspDeInit(ETH_HandleTypeDef *heth)
{
  /* Prevent unused argument(s) compilation warning */
 // UNUSED(heth);
  /* NOTE : This function Should not be modified, when the callback is needed,
  the HAL_ETH_MspDeInit could be implemented in the user file
  */
    if(heth->Instance==ETH)
      {
      /* USER CODE BEGIN ETH_MspDeInit 0 */

      /* USER CODE END ETH_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_ETH_CLK_DISABLE();

        /**ETH GPIO Configuration
        PC1     ------> ETH_MDC
        PA1     ------> ETH_REF_CLK
        PA2     ------> ETH_MDIO
        PA7     ------> ETH_CRS_DV
        PC4     ------> ETH_RXD0
        PC5     ------> ETH_RXD1
        PB11     ------> ETH_TX_EN
        PB12     ------> ETH_TXD0
        PB13     ------> ETH_TXD1
        */
        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5);

        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_7);

        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13);

      /* USER CODE BEGIN ETH_MspDeInit 1 */

      /* USER CODE END ETH_MspDeInit 1 */
      }


}

/**
  * @brief  Initializes the Ethernet MAC and DMA according to default
  *         parameters.
  * @param  heth pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETH_Init(ETH_HandleTypeDef *heth)
{
  uint32_t tmpreg1 = 0U, phyreg = 0U;
  uint32_t hclk = 60000000U;
  uint32_t tickstart = 0U;
  uint32_t err = ETH_SUCCESS;
  
  /* Check the ETH peripheral state */
  if(heth == NULL)
  {
    return HAL_ERROR;
  }
  
  /* Check parameters */
  assert_param(IS_ETH_AUTONEGOTIATION(heth->Init.AutoNegotiation));
  assert_param(IS_ETH_RX_MODE(heth->Init.RxMode));
  assert_param(IS_ETH_CHECKSUM_MODE(heth->Init.ChecksumMode));
  assert_param(IS_ETH_MEDIA_INTERFACE(heth->Init.MediaInterface));  
  
  if(heth->State == HAL_ETH_STATE_RESET)
  {
    /* Allocate lock resource and initialize it */
    heth->Lock = HAL_UNLOCKED;
#if (USE_HAL_ETH_REGISTER_CALLBACKS == 1)
    ETH_InitCallbacksToDefault(heth);

    if(heth->MspInitCallback == NULL)
    {
      /* Init the low level hardware : GPIO, CLOCK, NVIC. */
      heth->MspInitCallback = HAL_ETH_MspInit;
    }
    heth->MspInitCallback(heth);

#else
    /* Init the low level hardware : GPIO, CLOCK, NVIC. */
    HAL_ETH_MspInit(heth);
#endif /* USE_HAL_ETH_REGISTER_CALLBACKS */
  }
  
  /* Enable SYSCFG Clock */
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  
  /* Select MII or RMII Mode*/
  SYSCFG->PMC &= ~(SYSCFG_PMC_MII_RMII_SEL);
  SYSCFG->PMC |= (uint32_t)heth->Init.MediaInterface;
  
  /* Ethernet Software reset */
  /* Set the SWR bit: resets all MAC subsystem internal registers and logic */
  /* After reset all the registers holds their respective reset values */
  (heth->Instance)->DMABMR |= ETH_DMABMR_SR;
  
  /* Get tick */
  tickstart = HAL_GetTick();
  
  /* Wait for software reset */
  while (((heth->Instance)->DMABMR & ETH_DMABMR_SR) != (uint32_t)RESET)
  {
    /* Check for the Timeout */
    if((HAL_GetTick() - tickstart ) > ETH_TIMEOUT_SWRESET)
    {     
      heth->State= HAL_ETH_STATE_TIMEOUT;
  
      /* Process Unlocked */
      __HAL_UNLOCK(heth);
    
      /* Note: The SWR is not performed if the ETH_RX_CLK or the ETH_TX_CLK are  
         not available, please check your external PHY or the IO configuration */
      return HAL_TIMEOUT;
    }
  }
  
  /*-------------------------------- MAC Initialization ----------------------*/
  /* Get the ETHERNET MACMIIAR value */
  tmpreg1 = (heth->Instance)->MACMIIAR;
  /* Clear CSR Clock Range CR[2:0] bits */
  tmpreg1 &= ETH_MACMIIAR_CR_MASK;
  
  /* Get hclk frequency value */
  hclk = HAL_RCC_GetHCLKFreq();
  
  /* Set CR bits depending on hclk value */
  if((hclk >= 20000000U)&&(hclk < 35000000U))
  {
    /* CSR Clock Range between 20-35 MHz */
    tmpreg1 |= (uint32_t)ETH_MACMIIAR_CR_Div16;
  }
  else if((hclk >= 35000000U)&&(hclk < 60000000U))
  {
    /* CSR Clock Range between 35-60 MHz */ 
    tmpreg1 |= (uint32_t)ETH_MACMIIAR_CR_Div26;
  }  
  else if((hclk >= 60000000U)&&(hclk < 100000000U))
  {
    /* CSR Clock Range between 60-100 MHz */ 
    tmpreg1 |= (uint32_t)ETH_MACMIIAR_CR_Div42;
  }  
  else if((hclk >= 100000000U)&&(hclk < 150000000U))
  {
    /* CSR Clock Range between 100-150 MHz */ 
    tmpreg1 |= (uint32_t)ETH_MACMIIAR_CR_Div62;
  }
  else /* ((hclk >= 150000000)&&(hclk <= 183000000)) */
  {
    /* CSR Clock Range between 150-183 MHz */ 
    tmpreg1 |= (uint32_t)ETH_MACMIIAR_CR_Div102;    
  }
  
  /* Write to ETHERNET MAC MIIAR: Configure the ETHERNET CSR Clock Range */
  (heth->Instance)->MACMIIAR = (uint32_t)tmpreg1;
  
  /*-------------------- PHY initialization and configuration ----------------*/
  /* Put the PHY in reset mode */
  if((HAL_ETH_WritePHYRegister(heth, PHY_BCR, PHY_RESET)) != HAL_OK)
  {
    /* In case of write timeout */
    err = ETH_ERROR;
    
    /* Config MAC and DMA */
    ETH_MACDMAConfig(heth, err);
    
    /* Set the ETH peripheral state to READY */
    heth->State = HAL_ETH_STATE_READY;
    
    /* Return HAL_ERROR */
    return HAL_ERROR;
  }
  
  /* Delay to assure PHY reset */
  HAL_Delay(PHY_RESET_DELAY);
  
  if((heth->Init).AutoNegotiation != ETH_AUTONEGOTIATION_DISABLE)
  {
    /* Get tick */
    tickstart = HAL_GetTick();
    
    /* We wait for linked status */
    do
    {
      HAL_ETH_ReadPHYRegister(heth, PHY_BSR, &phyreg);
      
      /* Check for the Timeout */
      if((HAL_GetTick() - tickstart ) > ETH_TIMEOUT_LINKED_STATE)
      {
        /* In case of write timeout */
        err = ETH_ERROR;
      
        /* Config MAC and DMA */
        ETH_MACDMAConfig(heth, err);
        
        heth->State= HAL_ETH_STATE_READY;
  
        /* Process Unlocked */
        __HAL_UNLOCK(heth);
    
        return HAL_TIMEOUT;
      }
    } while (((phyreg & PHY_LINKED_STATUS) != PHY_LINKED_STATUS));

    
    /* Enable Auto-Negotiation */
    if((HAL_ETH_WritePHYRegister(heth, PHY_BCR, PHY_AUTONEGOTIATION)) != HAL_OK)
    {
      /* In case of write timeout */
      err = ETH_ERROR;
      
      /* Config MAC and DMA */
      ETH_MACDMAConfig(heth, err);
      
      /* Set the ETH peripheral state to READY */
      heth->State = HAL_ETH_STATE_READY;
      
      /* Return HAL_ERROR */
      return HAL_ERROR;   
    }
    
    /* Get tick */
    tickstart = HAL_GetTick();
    
    /* Wait until the auto-negotiation will be completed */
    do
    {
      HAL_ETH_ReadPHYRegister(heth, PHY_BSR, &phyreg);
      
      /* Check for the Timeout */
      if((HAL_GetTick() - tickstart ) > ETH_TIMEOUT_AUTONEGO_COMPLETED)
      {
        /* In case of write timeout */
        err = ETH_ERROR;
      
        /* Config MAC and DMA */
        ETH_MACDMAConfig(heth, err);
        
        heth->State= HAL_ETH_STATE_READY;
  
        /* Process Unlocked */
        __HAL_UNLOCK(heth);
    
        return HAL_TIMEOUT;
      }
      
    } while (((phyreg & PHY_AUTONEGO_COMPLETE) != PHY_AUTONEGO_COMPLETE));
    
    /* Read the result of the auto-negotiation */
    if((HAL_ETH_ReadPHYRegister(heth, PHY_SR, &phyreg)) != HAL_OK)
    {
      /* In case of write timeout */
      err = ETH_ERROR;
      
      /* Config MAC and DMA */
      ETH_MACDMAConfig(heth, err);
      
      /* Set the ETH peripheral state to READY */
      heth->State = HAL_ETH_STATE_READY;
      
      /* Return HAL_ERROR */
      return HAL_ERROR;   
    }
    
    /* Configure the MAC with the Duplex Mode fixed by the auto-negotiation process */
    if((phyreg & PHY_DUPLEX_STATUS) != (uint32_t)RESET)
    {
      /* Set Ethernet duplex mode to Full-duplex following the auto-negotiation */
      (heth->Init).DuplexMode = ETH_MODE_FULLDUPLEX;  
    }
    else
    {
      /* Set Ethernet duplex mode to Half-duplex following the auto-negotiation */
      (heth->Init).DuplexMode = ETH_MODE_HALFDUPLEX;           
    }
    /* Configure the MAC with the speed fixed by the auto-negotiation process */
    if((phyreg & PHY_SPEED_STATUS) == PHY_SPEED_STATUS)
    {  
      /* Set Ethernet speed to 10M following the auto-negotiation */
      (heth->Init).Speed = ETH_SPEED_10M; 
    }
    else
    {   
      /* Set Ethernet speed to 100M following the auto-negotiation */ 
      (heth->Init).Speed = ETH_SPEED_100M;
    }
  }
  else /* AutoNegotiation Disable */
  {
    /* Check parameters */
    assert_param(IS_ETH_SPEED(heth->Init.Speed));
    assert_param(IS_ETH_DUPLEX_MODE(heth->Init.DuplexMode));
    
    /* Set MAC Speed and Duplex Mode */
    if(HAL_ETH_WritePHYRegister(heth, PHY_BCR, ((uint16_t)((heth->Init).DuplexMode >> 3U) |
                                                (uint16_t)((heth->Init).Speed >> 1U))) != HAL_OK)
    {
      /* In case of write timeout */
      err = ETH_ERROR;
      
      /* Config MAC and DMA */
      ETH_MACDMAConfig(heth, err);
      
      /* Set the ETH peripheral state to READY */
      heth->State = HAL_ETH_STATE_READY;
      
      /* Return HAL_ERROR */
      return HAL_ERROR;
    }  
    
    /* Delay to assure PHY configuration */
    HAL_Delay(PHY_CONFIG_DELAY);
  }
  
  /* Config MAC and DMA */
  ETH_MACDMAConfig(heth, err);
  
  /* Set ETH HAL State to Ready */
  heth->State= HAL_ETH_STATE_READY;
  
  /* Return function status */
  return HAL_OK;
}


/**
  * @brief  Initializes the DMA Tx descriptors in chain mode.
  * @param  heth pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module  
  * @param  DMATxDescTab Pointer to the first Tx desc list 
  * @param  TxBuff Pointer to the first TxBuffer list
  * @param  TxBuffCount Number of the used Tx desc in the list
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETH_DMATxDescListInit(ETH_HandleTypeDef *heth, ETH_DMADescTypeDef *DMATxDescTab, uint8_t *TxBuff, uint32_t TxBuffCount)
{
  uint32_t i = 0U;
  ETH_DMADescTypeDef *dmatxdesc;
  
  /* Process Locked */
  __HAL_LOCK(heth);
  
  /* Set the ETH peripheral state to BUSY */
  heth->State = HAL_ETH_STATE_BUSY;
  
  /* Set the DMATxDescToSet pointer with the first one of the DMATxDescTab list */
  heth->TxDesc = DMATxDescTab;
  
  /* Fill each DMATxDesc descriptor with the right values */   
  for(i=0U; i < TxBuffCount; i++)
  {
    /* Get the pointer on the ith member of the Tx Desc list */
    dmatxdesc = DMATxDescTab + i;
    
    /* Set Second Address Chained bit */
    dmatxdesc->Status = ETH_DMATXDESC_TCH;  
    
    /* Set Buffer1 address pointer */
    dmatxdesc->Buffer1Addr = (uint32_t)(&TxBuff[i*ETH_TX_BUF_SIZE]);
    
    if ((heth->Init).ChecksumMode == ETH_CHECKSUM_BY_HARDWARE)
    {
      /* Set the DMA Tx descriptors checksum insertion */
      dmatxdesc->Status |= ETH_DMATXDESC_CHECKSUMTCPUDPICMPFULL;
    }
    
    /* Initialize the next descriptor with the Next Descriptor Polling Enable */
    if(i < (TxBuffCount-1U))
    {
      /* Set next descriptor address register with next descriptor base address */
      dmatxdesc->Buffer2NextDescAddr = (uint32_t)(DMATxDescTab+i+1U);
    }
    else
    {
      /* For last descriptor, set next descriptor address register equal to the first descriptor base address */ 
      dmatxdesc->Buffer2NextDescAddr = (uint32_t) DMATxDescTab;  
    }
  }
  
  /* Set Transmit Descriptor List Address Register */
  (heth->Instance)->DMATDLAR = (uint32_t) DMATxDescTab;
  
  /* Set ETH HAL State to Ready */
  heth->State= HAL_ETH_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(heth);
  
  /* Return function status */
  return HAL_OK;
}



/**
  * @brief  Initializes the DMA Rx descriptors in chain mode.
  * @param  heth pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module  
  * @param  DMARxDescTab Pointer to the first Rx desc list 
  * @param  RxBuff Pointer to the first RxBuffer list
  * @param  RxBuffCount Number of the used Rx desc in the list
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETH_DMARxDescListInit(ETH_HandleTypeDef *heth, ETH_DMADescTypeDef *DMARxDescTab, uint8_t *RxBuff, uint32_t RxBuffCount)
{
  uint32_t i = 0U;
  ETH_DMADescTypeDef *DMARxDesc;
  
  /* Process Locked */
  __HAL_LOCK(heth);
  
  /* Set the ETH peripheral state to BUSY */
  heth->State = HAL_ETH_STATE_BUSY;
  
  /* Set the Ethernet RxDesc pointer with the first one of the DMARxDescTab list */
  heth->RxDesc = DMARxDescTab; 
  
  /* Fill each DMARxDesc descriptor with the right values */
  for(i=0U; i < RxBuffCount; i++)
  {
    /* Get the pointer on the ith member of the Rx Desc list */
    DMARxDesc = DMARxDescTab+i;
    
    /* Set Own bit of the Rx descriptor Status */
    DMARxDesc->Status = ETH_DMARXDESC_OWN;
    
    /* Set Buffer1 size and Second Address Chained bit */
    DMARxDesc->ControlBufferSize = ETH_DMARXDESC_RCH | ETH_RX_BUF_SIZE;  
    
    /* Set Buffer1 address pointer */
    DMARxDesc->Buffer1Addr = (uint32_t)(&RxBuff[i*ETH_RX_BUF_SIZE]);
    
    if((heth->Init).RxMode == ETH_RXINTERRUPT_MODE)
    {
      /* Enable Ethernet DMA Rx Descriptor interrupt */
      DMARxDesc->ControlBufferSize &= ~ETH_DMARXDESC_DIC;
    }
    
    /* Initialize the next descriptor with the Next Descriptor Polling Enable */
    if(i < (RxBuffCount-1U))
    {
      /* Set next descriptor address register with next descriptor base address */
      DMARxDesc->Buffer2NextDescAddr = (uint32_t)(DMARxDescTab+i+1U); 
    }
    else
    {
      /* For last descriptor, set next descriptor address register equal to the first descriptor base address */ 
      DMARxDesc->Buffer2NextDescAddr = (uint32_t)(DMARxDescTab); 
    }
  }
  
  /* Set Receive Descriptor List Address Register */
  (heth->Instance)->DMARDLAR = (uint32_t) DMARxDescTab;
  
  /* Set ETH HAL State to Ready */
  heth->State= HAL_ETH_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(heth);
  
  /* Return function status */
  return HAL_OK;
}


 /**
  * @brief  Enables Ethernet MAC and DMA reception/transmission 
  * @param  heth pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETH_Start(ETH_HandleTypeDef *heth)
{  
  /* Process Locked */
  __HAL_LOCK(heth);
  
  /* Set the ETH peripheral state to BUSY */
  heth->State = HAL_ETH_STATE_BUSY;
  
  /* Enable transmit state machine of the MAC for transmission on the MII */
  ETH_MACTransmissionEnable(heth);
  
  /* Enable receive state machine of the MAC for reception from the MII */
  ETH_MACReceptionEnable(heth);
  
  /* Flush Transmit FIFO */
  ETH_FlushTransmitFIFO(heth);
  
  /* Start DMA transmission */
  ETH_DMATransmissionEnable(heth);
  
  /* Start DMA reception */
  ETH_DMAReceptionEnable(heth);
  
  /* Set the ETH state to READY*/
  heth->State= HAL_ETH_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(heth);
  
  /* Return function status */
  return HAL_OK;
}



/**
  * @brief  Sends an Ethernet frame. 
  * @param  heth pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @param  FrameLength Amount of data to be sent
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETH_TransmitFrame(ETH_HandleTypeDef *heth, uint32_t FrameLength)
{
  uint32_t bufcount = 0U, size = 0U, i = 0U;
  
  /* Process Locked */
  __HAL_LOCK(heth);
  
  /* Set the ETH peripheral state to BUSY */
  heth->State = HAL_ETH_STATE_BUSY;
  
  if (FrameLength == 0U) 
  {
    /* Set ETH HAL state to READY */
    heth->State = HAL_ETH_STATE_READY;
    
    /* Process Unlocked */
    __HAL_UNLOCK(heth);
    
    return  HAL_ERROR;                                    
  }  
  
  /* Check if the descriptor is owned by the ETHERNET DMA (when set) or CPU (when reset) */
  if(((heth->TxDesc)->Status & ETH_DMATXDESC_OWN) != (uint32_t)RESET)
  {  
    /* OWN bit set */
    heth->State = HAL_ETH_STATE_BUSY_TX;
    
    /* Process Unlocked */
    __HAL_UNLOCK(heth);
    
    return HAL_ERROR;
  }
  
  /* Get the number of needed Tx buffers for the current frame */
  if (FrameLength > ETH_TX_BUF_SIZE)
  {
    bufcount = FrameLength/ETH_TX_BUF_SIZE;
    if (FrameLength % ETH_TX_BUF_SIZE) 
    {
      bufcount++;
    }
  }
  else 
  {  
    bufcount = 1U;
  }
  if (bufcount == 1U)
  {
    /* Set LAST and FIRST segment */
    heth->TxDesc->Status |=ETH_DMATXDESC_FS|ETH_DMATXDESC_LS;
    /* Set frame size */
    heth->TxDesc->ControlBufferSize = (FrameLength & ETH_DMATXDESC_TBS1);
    /* Set Own bit of the Tx descriptor Status: gives the buffer back to ETHERNET DMA */
    heth->TxDesc->Status |= ETH_DMATXDESC_OWN;
    /* Point to next descriptor */
    heth->TxDesc= (ETH_DMADescTypeDef *)(heth->TxDesc->Buffer2NextDescAddr);
  }
  else
  {
    for (i=0U; i< bufcount; i++)
    {
      /* Clear FIRST and LAST segment bits */
      heth->TxDesc->Status &= ~(ETH_DMATXDESC_FS | ETH_DMATXDESC_LS);
      
      if (i == 0U) 
      {
        /* Setting the first segment bit */
        heth->TxDesc->Status |= ETH_DMATXDESC_FS;  
      }
      
      /* Program size */
      heth->TxDesc->ControlBufferSize = (ETH_TX_BUF_SIZE & ETH_DMATXDESC_TBS1);
      
      if (i == (bufcount-1U))
      {
        /* Setting the last segment bit */
        heth->TxDesc->Status |= ETH_DMATXDESC_LS;
        size = FrameLength - (bufcount-1U)*ETH_TX_BUF_SIZE;
        heth->TxDesc->ControlBufferSize = (size & ETH_DMATXDESC_TBS1);
      }
      
      /* Set Own bit of the Tx descriptor Status: gives the buffer back to ETHERNET DMA */
      heth->TxDesc->Status |= ETH_DMATXDESC_OWN;
      /* point to next descriptor */
      heth->TxDesc = (ETH_DMADescTypeDef *)(heth->TxDesc->Buffer2NextDescAddr);
    }
  }
  
  /* When Tx Buffer unavailable flag is set: clear it and resume transmission */
  if (((heth->Instance)->DMASR & ETH_DMASR_TBUS) != (uint32_t)RESET)
  {
    /* Clear TBUS ETHERNET DMA flag */
    (heth->Instance)->DMASR = ETH_DMASR_TBUS;
    /* Resume DMA transmission*/
    (heth->Instance)->DMATPDR = 0U;
  }
  
  /* Set ETH HAL State to Ready */
  heth->State = HAL_ETH_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(heth);
  
  /* Return function status */
  return HAL_OK;
}


/**
  * @brief  Gets the Received frame in interrupt mode. 
  * @param  heth pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETH_GetReceivedFrame_IT(ETH_HandleTypeDef *heth)
{
  uint32_t descriptorscancounter = 0U;
  
  /* Process Locked */
  __HAL_LOCK(heth);
  
  /* Set ETH HAL State to BUSY */
  heth->State = HAL_ETH_STATE_BUSY;
  
  /* Scan descriptors owned by CPU */
  while (((heth->RxDesc->Status & ETH_DMARXDESC_OWN) == (uint32_t)RESET) && (descriptorscancounter < ETH_RXBUFNB))
  {
    /* Just for security */
    descriptorscancounter++;
    
    /* Check if first segment in frame */
    /* ((heth->RxDesc->Status & ETH_DMARXDESC_FS) != (uint32_t)RESET) && ((heth->RxDesc->Status & ETH_DMARXDESC_LS) == (uint32_t)RESET)) */  
    if((heth->RxDesc->Status & (ETH_DMARXDESC_FS | ETH_DMARXDESC_LS)) == (uint32_t)ETH_DMARXDESC_FS)
    { 
      heth->RxFrameInfos.FSRxDesc = heth->RxDesc;
      heth->RxFrameInfos.SegCount = 1U;   
      /* Point to next descriptor */
      heth->RxDesc = (ETH_DMADescTypeDef*) (heth->RxDesc->Buffer2NextDescAddr);
    }
    /* Check if intermediate segment */
    /* ((heth->RxDesc->Status & ETH_DMARXDESC_LS) == (uint32_t)RESET)&& ((heth->RxDesc->Status & ETH_DMARXDESC_FS) == (uint32_t)RESET)) */
    else if ((heth->RxDesc->Status & (ETH_DMARXDESC_LS | ETH_DMARXDESC_FS)) == (uint32_t)RESET)
    {
      /* Increment segment count */
      (heth->RxFrameInfos.SegCount)++;
      /* Point to next descriptor */
      heth->RxDesc = (ETH_DMADescTypeDef*)(heth->RxDesc->Buffer2NextDescAddr);
    }
    /* Should be last segment */
    else
    { 
      /* Last segment */
      heth->RxFrameInfos.LSRxDesc = heth->RxDesc;
      
      /* Increment segment count */
      (heth->RxFrameInfos.SegCount)++;
      
      /* Check if last segment is first segment: one segment contains the frame */
      if ((heth->RxFrameInfos.SegCount) == 1U)
      {
        heth->RxFrameInfos.FSRxDesc = heth->RxDesc;
      }
      
      /* Get the Frame Length of the received packet: substruct 4 bytes of the CRC */
      heth->RxFrameInfos.length = (((heth->RxDesc)->Status & ETH_DMARXDESC_FL) >> ETH_DMARXDESC_FRAMELENGTHSHIFT) - 4U;
      
      /* Get the address of the buffer start address */ 
      heth->RxFrameInfos.buffer =((heth->RxFrameInfos).FSRxDesc)->Buffer1Addr;
      
      /* Point to next descriptor */      
      heth->RxDesc = (ETH_DMADescTypeDef*) (heth->RxDesc->Buffer2NextDescAddr);
      
      /* Set HAL State to Ready */
      heth->State = HAL_ETH_STATE_READY;
      
      /* Process Unlocked */
      __HAL_UNLOCK(heth);
  
      /* Return function status */
      return HAL_OK;
    }
  }

  /* Set HAL State to Ready */
  heth->State = HAL_ETH_STATE_READY;
  
  /* Process Unlocked */
  __HAL_UNLOCK(heth);
  
  /* Return function status */
  return HAL_ERROR;
}


/**
  * @brief  This function handles ETH interrupt request.
  * @param  heth pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @retval HAL status
  */
void HAL_ETH_IRQHandler(ETH_HandleTypeDef *heth)
{
  /* Frame received */
  if (__HAL_ETH_DMA_GET_FLAG(heth, ETH_DMA_FLAG_R)) 
  {
#if (USE_HAL_ETH_REGISTER_CALLBACKS == 1)
    /*Call registered Receive complete callback*/
    heth->RxCpltCallback(heth);
#else
    /* Receive complete callback */
    HAL_ETH_RxCpltCallback(heth);
#endif /* USE_HAL_ETH_REGISTER_CALLBACKS */

     /* Clear the Eth DMA Rx IT pending bits */
    __HAL_ETH_DMA_CLEAR_IT(heth, ETH_DMA_IT_R);

    /* Set HAL State to Ready */
    heth->State = HAL_ETH_STATE_READY;
    
    /* Process Unlocked */
    __HAL_UNLOCK(heth);

  }
  /* Frame transmitted */
  else if (__HAL_ETH_DMA_GET_FLAG(heth, ETH_DMA_FLAG_T)) 
  {
#if (USE_HAL_ETH_REGISTER_CALLBACKS == 1)
    /*  Call resgistered Transfer complete callback*/
    heth->TxCpltCallback(heth);
#else
    /* Transfer complete callback */
    HAL_ETH_TxCpltCallback(heth);
#endif /* USE_HAL_ETH_REGISTER_CALLBACKS */

    /* Clear the Eth DMA Tx IT pending bits */
    __HAL_ETH_DMA_CLEAR_IT(heth, ETH_DMA_IT_T);

    /* Set HAL State to Ready */
    heth->State = HAL_ETH_STATE_READY;
    
    /* Process Unlocked */
    __HAL_UNLOCK(heth);
  }
  
  /* Clear the interrupt flags */
  __HAL_ETH_DMA_CLEAR_IT(heth, ETH_DMA_IT_NIS);
  
  /* ETH DMA Error */
  if(__HAL_ETH_DMA_GET_FLAG(heth, ETH_DMA_FLAG_AIS))
  {
#if (USE_HAL_ETH_REGISTER_CALLBACKS == 1)
    heth->DMAErrorCallback(heth);
#else
    /* Ethernet Error callback */
    HAL_ETH_ErrorCallback(heth);
#endif /* USE_HAL_ETH_REGISTER_CALLBACKS */

    /* Clear the interrupt flags */
    __HAL_ETH_DMA_CLEAR_IT(heth, ETH_DMA_FLAG_AIS);
  
    /* Set HAL State to Ready */
    heth->State = HAL_ETH_STATE_READY;
    
    /* Process Unlocked */
    __HAL_UNLOCK(heth);
  }
}

/**
  * @brief  Reads a PHY register
  * @param  heth pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module                  
  * @param PHYReg PHY register address, is the index of one of the 32 PHY register. 
  *                This parameter can be one of the following values: 
  *                   PHY_BCR: Transceiver Basic Control Register, 
  *                   PHY_BSR: Transceiver Basic Status Register.   
  *                   More PHY register could be read depending on the used PHY
  * @param RegValue PHY register value                  
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETH_ReadPHYRegister(ETH_HandleTypeDef *heth, uint16_t PHYReg, uint32_t *RegValue)
{
  uint32_t tmpreg1 = 0U;     
  uint32_t tickstart = 0U;
  
  /* Check parameters */
  assert_param(IS_ETH_PHY_ADDRESS(heth->Init.PhyAddress));
  
  /* Check the ETH peripheral state */
  if(heth->State == HAL_ETH_STATE_BUSY_RD)
  {
    return HAL_BUSY;
  }
  /* Set ETH HAL State to BUSY_RD */
  heth->State = HAL_ETH_STATE_BUSY_RD;
  
  /* Get the ETHERNET MACMIIAR value */
  tmpreg1 = heth->Instance->MACMIIAR;
  
  /* Keep only the CSR Clock Range CR[2:0] bits value */
  tmpreg1 &= ~ETH_MACMIIAR_CR_MASK;
  
  /* Prepare the MII address register value */
  tmpreg1 |=(((uint32_t)heth->Init.PhyAddress << 11U) & ETH_MACMIIAR_PA); /* Set the PHY device address   */
  tmpreg1 |=(((uint32_t)PHYReg<<6U) & ETH_MACMIIAR_MR);                   /* Set the PHY register address */
  tmpreg1 &= ~ETH_MACMIIAR_MW;                                            /* Set the read mode            */
  tmpreg1 |= ETH_MACMIIAR_MB;                                             /* Set the MII Busy bit         */
  
  /* Write the result value into the MII Address register */
  heth->Instance->MACMIIAR = tmpreg1;
  
  /* Get tick */
  tickstart = HAL_GetTick();
  
  /* Check for the Busy flag */
  while((tmpreg1 & ETH_MACMIIAR_MB) == ETH_MACMIIAR_MB)
  {
    /* Check for the Timeout */
    if((HAL_GetTick() - tickstart ) > PHY_READ_TO)
    {
      heth->State= HAL_ETH_STATE_READY;
  
      /* Process Unlocked */
      __HAL_UNLOCK(heth);
    
      return HAL_TIMEOUT;
    }
    
    tmpreg1 = heth->Instance->MACMIIAR;
  }
  
  /* Get MACMIIDR value */
  *RegValue = (uint16_t)(heth->Instance->MACMIIDR);
  
  /* Set ETH HAL State to READY */
  heth->State = HAL_ETH_STATE_READY;
  
  /* Return function status */
  return HAL_OK;
}



/**
  * @brief  Writes to a PHY register.
  * @param  heth pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module  
  * @param  PHYReg PHY register address, is the index of one of the 32 PHY register. 
  *          This parameter can be one of the following values: 
  *             PHY_BCR: Transceiver Control Register.  
  *             More PHY register could be written depending on the used PHY
  * @param  RegValue the value to write
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETH_WritePHYRegister(ETH_HandleTypeDef *heth, uint16_t PHYReg, uint32_t RegValue)
{
  uint32_t tmpreg1 = 0U;
  uint32_t tickstart = 0U;
  
  /* Check parameters */
  assert_param(IS_ETH_PHY_ADDRESS(heth->Init.PhyAddress));
  
  /* Check the ETH peripheral state */
  if(heth->State == HAL_ETH_STATE_BUSY_WR)
  {
    return HAL_BUSY;
  }
  /* Set ETH HAL State to BUSY_WR */
  heth->State = HAL_ETH_STATE_BUSY_WR;
  
  /* Get the ETHERNET MACMIIAR value */
  tmpreg1 = heth->Instance->MACMIIAR;
  
  /* Keep only the CSR Clock Range CR[2:0] bits value */
  tmpreg1 &= ~ETH_MACMIIAR_CR_MASK;
  
  /* Prepare the MII register address value */
  tmpreg1 |=(((uint32_t)heth->Init.PhyAddress<<11U) & ETH_MACMIIAR_PA); /* Set the PHY device address */
  tmpreg1 |=(((uint32_t)PHYReg<<6U) & ETH_MACMIIAR_MR);                 /* Set the PHY register address */
  tmpreg1 |= ETH_MACMIIAR_MW;                                           /* Set the write mode */
  tmpreg1 |= ETH_MACMIIAR_MB;                                           /* Set the MII Busy bit */
  
  /* Give the value to the MII data register */
  heth->Instance->MACMIIDR = (uint16_t)RegValue;
  
  /* Write the result value into the MII Address register */
  heth->Instance->MACMIIAR = tmpreg1;
  
  /* Get tick */
  tickstart = HAL_GetTick();
  
  /* Check for the Busy flag */
  while((tmpreg1 & ETH_MACMIIAR_MB) == ETH_MACMIIAR_MB)
  {
    /* Check for the Timeout */
    if((HAL_GetTick() - tickstart ) > PHY_WRITE_TO)
    {
      heth->State= HAL_ETH_STATE_READY;
  
      /* Process Unlocked */
      __HAL_UNLOCK(heth);
    
      return HAL_TIMEOUT;
    }
    
    tmpreg1 = heth->Instance->MACMIIAR;
  }
  
  /* Set ETH HAL State to READY */
  heth->State = HAL_ETH_STATE_READY;
  
  /* Return function status */
  return HAL_OK; 
}



/**
  * @brief  De-Initializes the ETH peripheral. 
  * @param  heth pointer to a ETH_HandleTypeDef structure that contains
  *         the configuration information for ETHERNET module
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_ETH_DeInit(ETH_HandleTypeDef *heth)
{
  /* Set the ETH peripheral state to BUSY */
  heth->State = HAL_ETH_STATE_BUSY;
  
#if (USE_HAL_ETH_REGISTER_CALLBACKS == 1)
  if(heth->MspDeInitCallback == NULL)
  {
    heth->MspDeInitCallback = HAL_ETH_MspDeInit;
  }
  /* De-Init the low level hardware : GPIO, CLOCK, NVIC. */
  heth->MspDeInitCallback(heth);
#else
  /* De-Init the low level hardware : GPIO, CLOCK, NVIC. */
  HAL_ETH_MspDeInit(heth);
#endif
  
  /* Set ETH HAL state to Disabled */
  heth->State= HAL_ETH_STATE_RESET;

  /* Release Lock */
  __HAL_UNLOCK(heth);

  /* Return function status */
  return HAL_OK;
}


#endif 

extern void phy_reset(void);

void phy_reset(void)
{

}
/* EMAC initialization function */
static rt_err_t rt_stm32_eth_init(rt_device_t dev)
{
    __HAL_RCC_ETH_CLK_ENABLE();

    phy_reset();

    /* ETHERNET Configuration */
    EthHandle.Instance = ETH;
    EthHandle.Init.MACAddr = (rt_uint8_t *)&stm32_eth_device.dev_addr[0];
    EthHandle.Init.AutoNegotiation = ETH_AUTONEGOTIATION_DISABLE;
    EthHandle.Init.Speed = ETH_SPEED_100M;
    EthHandle.Init.DuplexMode = ETH_MODE_FULLDUPLEX;
    EthHandle.Init.MediaInterface = ETH_MEDIA_INTERFACE_RMII;
    EthHandle.Init.RxMode = ETH_RXINTERRUPT_MODE;
#ifdef RT_LWIP_USING_HW_CHECKSUM
    EthHandle.Init.ChecksumMode = ETH_CHECKSUM_BY_HARDWARE;
#else
    EthHandle.Init.ChecksumMode = ETH_CHECKSUM_BY_SOFTWARE;
#endif

    HAL_ETH_DeInit(&EthHandle);

    /* configure ethernet peripheral (GPIOs, clocks, MAC, DMA) */
    if (HAL_ETH_Init(&EthHandle) != HAL_OK)
    {
        LOG_E("eth hardware init failed");
    }
    else
    {
        LOG_D("eth hardware init success");
    }

    /* Initialize Tx Descriptors list: Chain Mode */
    HAL_ETH_DMATxDescListInit(&EthHandle, DMATxDscrTab, Tx_Buff, ETH_TXBUFNB);

    /* Initialize Rx Descriptors list: Chain Mode  */
    HAL_ETH_DMARxDescListInit(&EthHandle, DMARxDscrTab, Rx_Buff, ETH_RXBUFNB);

    /* ETH interrupt Init */
    HAL_NVIC_SetPriority(ETH_IRQn, 0x07, 0);
    HAL_NVIC_EnableIRQ(ETH_IRQn);

    /* Enable MAC and DMA transmission and reception */
    if (HAL_ETH_Start(&EthHandle) == HAL_OK)
    {
        LOG_D("emac hardware start");
    }
    else
    {
        LOG_E("emac hardware start faild");
        return -RT_ERROR;
    }

    return RT_EOK;
}

static rt_err_t rt_stm32_eth_open(rt_device_t dev, rt_uint16_t oflag)
{
    LOG_D("emac open");
    return RT_EOK;
}

static rt_err_t rt_stm32_eth_close(rt_device_t dev)
{
    LOG_D("emac close");
    return RT_EOK;
}

static rt_size_t rt_stm32_eth_read(rt_device_t dev, rt_off_t pos, void *buffer, rt_size_t size)
{
    LOG_D("emac read");
    rt_set_errno(-RT_ENOSYS);
    return 0;
}

static rt_size_t rt_stm32_eth_write(rt_device_t dev, rt_off_t pos, const void *buffer, rt_size_t size)
{
    LOG_D("emac write");
    rt_set_errno(-RT_ENOSYS);
    return 0;
}

static rt_err_t rt_stm32_eth_control(rt_device_t dev, int cmd, void *args)
{
    switch (cmd)
    {
    case NIOCTL_GADDR:
        /* get mac address */
        if (args) rt_memcpy(args, stm32_eth_device.dev_addr, 6);
        else return -RT_ERROR;
        break;

    default :
        break;
    }

    return RT_EOK;
}

/* ethernet device interface */
/* transmit data*/
rt_err_t rt_stm32_eth_tx(rt_device_t dev, struct pbuf *p)
{
    rt_err_t ret = RT_ERROR;
    HAL_StatusTypeDef state;
    struct pbuf *q;
    uint8_t *buffer = (uint8_t *)(EthHandle.TxDesc->Buffer1Addr);
    __IO ETH_DMADescTypeDef *DmaTxDesc;
    uint32_t framelength = 0;
    uint32_t bufferoffset = 0;
    uint32_t byteslefttocopy = 0;
    uint32_t payloadoffset = 0;

    DmaTxDesc = EthHandle.TxDesc;
    bufferoffset = 0;

    /* copy frame from pbufs to driver buffers */
    for (q = p; q != NULL; q = q->next)
    {
        /* Is this buffer available? If not, goto error */
        if ((DmaTxDesc->Status & ETH_DMATXDESC_OWN) != (uint32_t)RESET)
        {
            LOG_D("buffer not valid");
            ret = ERR_USE;
            goto error;
        }

        /* Get bytes in current lwIP buffer */
        byteslefttocopy = q->len;
        payloadoffset = 0;

        /* Check if the length of data to copy is bigger than Tx buffer size*/
        while ((byteslefttocopy + bufferoffset) > ETH_TX_BUF_SIZE)
        {
            /* Copy data to Tx buffer*/
            memcpy((uint8_t *)((uint8_t *)buffer + bufferoffset), (uint8_t *)((uint8_t *)q->payload + payloadoffset), (ETH_TX_BUF_SIZE - bufferoffset));

            /* Point to next descriptor */
            DmaTxDesc = (ETH_DMADescTypeDef *)(DmaTxDesc->Buffer2NextDescAddr);

            /* Check if the buffer is available */
            if ((DmaTxDesc->Status & ETH_DMATXDESC_OWN) != (uint32_t)RESET)
            {
                LOG_E("dma tx desc buffer is not valid");
                ret = ERR_USE;
                goto error;
            }

            buffer = (uint8_t *)(DmaTxDesc->Buffer1Addr);

            byteslefttocopy = byteslefttocopy - (ETH_TX_BUF_SIZE - bufferoffset);
            payloadoffset = payloadoffset + (ETH_TX_BUF_SIZE - bufferoffset);
            framelength = framelength + (ETH_TX_BUF_SIZE - bufferoffset);
            bufferoffset = 0;
        }

        /* Copy the remaining bytes */
        memcpy((uint8_t *)((uint8_t *)buffer + bufferoffset), (uint8_t *)((uint8_t *)q->payload + payloadoffset), byteslefttocopy);
        bufferoffset = bufferoffset + byteslefttocopy;
        framelength = framelength + byteslefttocopy;
    }

#ifdef ETH_TX_DUMP
    dump_hex(buffer, p->tot_len);
#endif

    /* Prepare transmit descriptors to give to DMA */
    /* TODO Optimize data send speed*/
    LOG_D("transmit frame length :%d", framelength);

    /* wait for unlocked */
    while (EthHandle.Lock == HAL_LOCKED);

    state = HAL_ETH_TransmitFrame(&EthHandle, framelength);
    if (state != HAL_OK)
    {
        LOG_E("eth transmit frame faild: %d", state);
    }

    ret = ERR_OK;

error:

    /* When Transmit Underflow flag is set, clear it and issue a Transmit Poll Demand to resume transmission */
    if ((EthHandle.Instance->DMASR & ETH_DMASR_TUS) != (uint32_t)RESET)
    {
        /* Clear TUS ETHERNET DMA flag */
        EthHandle.Instance->DMASR = ETH_DMASR_TUS;

        /* Resume DMA transmission*/
        EthHandle.Instance->DMATPDR = 0;
    }

    return ret;
}

/* receive data*/
struct pbuf *rt_stm32_eth_rx(rt_device_t dev)
{

    struct pbuf *p = NULL;
    struct pbuf *q = NULL;
    HAL_StatusTypeDef state;
    uint16_t len = 0;
    uint8_t *buffer;
    __IO ETH_DMADescTypeDef *dmarxdesc;
    uint32_t bufferoffset = 0;
    uint32_t payloadoffset = 0;
    uint32_t byteslefttocopy = 0;
    uint32_t i = 0;

    /* Get received frame */
    state = HAL_ETH_GetReceivedFrame_IT(&EthHandle);
    if (state != HAL_OK)
    {
        LOG_D("receive frame faild");
        return NULL;
    }

    /* Obtain the size of the packet and put it into the "len" variable. */
    len = EthHandle.RxFrameInfos.length;
    buffer = (uint8_t *)EthHandle.RxFrameInfos.buffer;

    LOG_D("receive frame len : %d", len);

    if (len > 0)
    {
        /* We allocate a pbuf chain of pbufs from the Lwip buffer pool */
        p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
    }

#ifdef ETH_RX_DUMP
    dump_hex(buffer, p->tot_len);
#endif

    if (p != NULL)
    {
        dmarxdesc = EthHandle.RxFrameInfos.FSRxDesc;
        bufferoffset = 0;
        for (q = p; q != NULL; q = q->next)
        {
            byteslefttocopy = q->len;
            payloadoffset = 0;

            /* Check if the length of bytes to copy in current pbuf is bigger than Rx buffer size*/
            while ((byteslefttocopy + bufferoffset) > ETH_RX_BUF_SIZE)
            {
                /* Copy data to pbuf */
                memcpy((uint8_t *)((uint8_t *)q->payload + payloadoffset), (uint8_t *)((uint8_t *)buffer + bufferoffset), (ETH_RX_BUF_SIZE - bufferoffset));

                /* Point to next descriptor */
                dmarxdesc = (ETH_DMADescTypeDef *)(dmarxdesc->Buffer2NextDescAddr);
                buffer = (uint8_t *)(dmarxdesc->Buffer1Addr);

                byteslefttocopy = byteslefttocopy - (ETH_RX_BUF_SIZE - bufferoffset);
                payloadoffset = payloadoffset + (ETH_RX_BUF_SIZE - bufferoffset);
                bufferoffset = 0;
            }
            /* Copy remaining data in pbuf */
            memcpy((uint8_t *)((uint8_t *)q->payload + payloadoffset), (uint8_t *)((uint8_t *)buffer + bufferoffset), byteslefttocopy);
            bufferoffset = bufferoffset + byteslefttocopy;
        }
    }

    /* Release descriptors to DMA */
    /* Point to first descriptor */
    dmarxdesc = EthHandle.RxFrameInfos.FSRxDesc;
    /* Set Own bit in Rx descriptors: gives the buffers back to DMA */
    for (i = 0; i < EthHandle.RxFrameInfos.SegCount; i++)
    {
        dmarxdesc->Status |= ETH_DMARXDESC_OWN;
        dmarxdesc = (ETH_DMADescTypeDef *)(dmarxdesc->Buffer2NextDescAddr);
    }

    /* Clear Segment_Count */
    EthHandle.RxFrameInfos.SegCount = 0;

    /* When Rx Buffer unavailable flag is set: clear it and resume reception */
    if ((EthHandle.Instance->DMASR & ETH_DMASR_RBUS) != (uint32_t)RESET)
    {
        /* Clear RBUS ETHERNET DMA flag */
        EthHandle.Instance->DMASR = ETH_DMASR_RBUS;
        /* Resume DMA reception */
        EthHandle.Instance->DMARPDR = 0;
    }

    return p;
}

/* interrupt service routine */
void ETH_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    HAL_ETH_IRQHandler(&EthHandle);

    /* leave interrupt */
    rt_interrupt_leave();
}

void HAL_ETH_RxCpltCallback(ETH_HandleTypeDef *heth)
{
    rt_err_t result;
    result = eth_device_ready(&(stm32_eth_device.parent));
    if (result != RT_EOK)
        LOG_I("RxCpltCallback err = %d", result);
}

void HAL_ETH_ErrorCallback(ETH_HandleTypeDef *heth)
{
    LOG_E("eth err");
}

enum {
    PHY_LINK        = (1 << 0),
    PHY_100M        = (1 << 1),
    PHY_FULL_DUPLEX = (1 << 2),
};

static void phy_linkchange()
{
    static rt_uint8_t phy_speed = 0;
    rt_uint8_t phy_speed_new = 0;
    rt_uint32_t status;

    HAL_ETH_ReadPHYRegister(&EthHandle, PHY_BASIC_STATUS_REG, (uint32_t *)&status);
    LOG_D("phy basic status reg is 0x%X", status);

    if (status & (PHY_AUTONEGO_COMPLETE_MASK | PHY_LINKED_STATUS_MASK))
    {
        rt_uint32_t SR = 0;

        phy_speed_new |= PHY_LINK;

        HAL_ETH_ReadPHYRegister(&EthHandle, PHY_Status_REG, (uint32_t *)&SR);
        LOG_D("phy control status reg is 0x%X", SR);

        if (PHY_Status_SPEED_100M(SR))
        {
            phy_speed_new |= PHY_100M;
        }

        if (PHY_Status_FULL_DUPLEX(SR))
        {
            phy_speed_new |= PHY_FULL_DUPLEX;
        }
    }

    if (phy_speed != phy_speed_new)
    {
        phy_speed = phy_speed_new;
        if (phy_speed & PHY_LINK)
        {
            LOG_D("link up");
            if (phy_speed & PHY_100M)
            {
                LOG_D("100Mbps");
                stm32_eth_device.ETH_Speed = ETH_SPEED_100M;
            }
            else
            {
                stm32_eth_device.ETH_Speed = ETH_SPEED_10M;
                LOG_D("10Mbps");
            }

            if (phy_speed & PHY_FULL_DUPLEX)
            {
                LOG_D("full-duplex");
                stm32_eth_device.ETH_Mode = ETH_MODE_FULLDUPLEX;
            }
            else
            {
                LOG_D("half-duplex");
                stm32_eth_device.ETH_Mode = ETH_MODE_HALFDUPLEX;
            }

            /* send link up. */
            eth_device_linkchange(&stm32_eth_device.parent, RT_TRUE);
        }
        else
        {
            LOG_I("link down");
            eth_device_linkchange(&stm32_eth_device.parent, RT_FALSE);
        }
    }
}

#ifdef PHY_USING_INTERRUPT_MODE
static void eth_phy_isr(void *args)
{
    rt_uint32_t status = 0;

    HAL_ETH_ReadPHYRegister(&EthHandle, PHY_INTERRUPT_FLAG_REG, (uint32_t *)&status);
    LOG_D("phy interrupt status reg is 0x%X", status);

    phy_linkchange();
}
#endif /* PHY_USING_INTERRUPT_MODE */

static void phy_monitor_thread_entry(void *parameter)
{
    uint8_t phy_addr = 0xFF;
    uint8_t detected_count = 0;

    while(phy_addr == 0xFF)
    {
        /* phy search */
        rt_uint32_t i, temp;
        for (i = 0; i <= 0x1F; i++)
        {
            EthHandle.Init.PhyAddress = i;
            HAL_ETH_ReadPHYRegister(&EthHandle, PHY_ID1_REG, (uint32_t *)&temp);

            if (temp != 0xFFFF && temp != 0x00)
            {
                phy_addr = i;
                break;
            }
        }

        detected_count++;
        rt_thread_mdelay(1000);

        if (detected_count > 10)
        {
            LOG_E("No PHY device was detected, please check hardware!");
        }
    }

    LOG_D("Found a phy, address:0x%02X", phy_addr);

    /* RESET PHY */
    LOG_D("RESET PHY!");
    HAL_ETH_WritePHYRegister(&EthHandle, PHY_BASIC_CONTROL_REG, PHY_RESET_MASK);
    rt_thread_mdelay(2000);
    HAL_ETH_WritePHYRegister(&EthHandle, PHY_BASIC_CONTROL_REG, PHY_AUTO_NEGOTIATION_MASK);

    phy_linkchange();
#ifdef PHY_USING_INTERRUPT_MODE
    /* configuration intterrupt pin */
    rt_pin_mode(PHY_INT_PIN, PIN_MODE_INPUT_PULLUP);
    rt_pin_attach_irq(PHY_INT_PIN, PIN_IRQ_MODE_FALLING, eth_phy_isr, (void *)"callbackargs");
    rt_pin_irq_enable(PHY_INT_PIN, PIN_IRQ_ENABLE);

    /* enable phy interrupt */
    HAL_ETH_WritePHYRegister(&EthHandle, PHY_INTERRUPT_MASK_REG, PHY_INT_MASK);
#if defined(PHY_INTERRUPT_CTRL_REG)
    HAL_ETH_WritePHYRegister(&EthHandle, PHY_INTERRUPT_CTRL_REG, PHY_INTERRUPT_EN);
#endif
#else /* PHY_USING_INTERRUPT_MODE */
    stm32_eth_device.poll_link_timer = rt_timer_create("phylnk", (void (*)(void*))phy_linkchange,
                                        NULL, RT_TICK_PER_SECOND, RT_TIMER_FLAG_PERIODIC);
    if (!stm32_eth_device.poll_link_timer || rt_timer_start(stm32_eth_device.poll_link_timer) != RT_EOK)
    {
        LOG_E("Start link change detection timer failed");
    }
#endif /* PHY_USING_INTERRUPT_MODE */
}
#define ETH_MDIO                 GET_PIN(A, 2)
#define ETH_MDC                  GET_PIN(C, 1)
#define ETH_RMII_REF_CLK         GET_PIN(A, 1)
#define ETH_RMII_CRS_DV          GET_PIN(A, 7)
#define ETH_RMII_RXD0            GET_PIN(C, 4)
#define ETH_RMII_RXD1            GET_PIN(C, 5)
#define ETH_RMII_TX_EN           GET_PIN(G, 11)
#define ETH_RMII_TXD0            GET_PIN(G, 13)
#define ETH_RMII_TXD1            GET_PIN(G, 14)
#define ETH_RMII1_TX_EN           GET_PIN(B, 11)
#define ETH_RMII1_TXD0            GET_PIN(B, 12)
#define ETH_RMII1_TXD1            GET_PIN(B, 13)

/*
 * GPIO Configuration for ETH
 */
static void ETH_GPIO_Configuration(void)
{
    rt_pin_mode(ETH_MDIO, GPIO_AF11_ETH);
    rt_pin_mode(ETH_MDC, GPIO_AF11_ETH);
    rt_pin_mode(ETH_RMII_REF_CLK, GPIO_AF11_ETH);
    rt_pin_mode(ETH_RMII_CRS_DV, GPIO_AF11_ETH);
    rt_pin_mode(ETH_RMII_RXD0, GPIO_AF11_ETH);
    rt_pin_mode(ETH_RMII_RXD1, GPIO_AF11_ETH);
    rt_pin_mode(ETH_RMII_TX_EN, GPIO_AF11_ETH);
    rt_pin_mode(ETH_RMII_TXD0, GPIO_AF11_ETH);
    rt_pin_mode(ETH_RMII_TXD1, GPIO_AF11_ETH);
    rt_pin_mode(ETH_RMII1_TX_EN, GPIO_AF11_ETH);
    rt_pin_mode(ETH_RMII1_TXD0, GPIO_AF11_ETH);
    rt_pin_mode(ETH_RMII1_TXD1, GPIO_AF11_ETH);

}



/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
#if 0
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
#endif
  /*Configure GPIO pin : PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF0_MCO;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

/* Register the EMAC device */
static int rt_hw_stm32_eth_init(void)
{
    rt_err_t state = RT_EOK;
    /* Initialize all configured peripherals */
        MX_GPIO_Init();

    /* Prepare receive and send buffers */
    Rx_Buff = (rt_uint8_t *)rt_calloc(ETH_RXBUFNB, ETH_MAX_PACKET_SIZE);
    if (Rx_Buff == RT_NULL)
    {
        LOG_E("No memory");
        state = -RT_ENOMEM;
        goto __exit;
    }



    Tx_Buff = (rt_uint8_t *)rt_calloc(ETH_TXBUFNB, ETH_MAX_PACKET_SIZE);
    if (Tx_Buff == RT_NULL)
    {
        LOG_E("No memory");
        state = -RT_ENOMEM;
        goto __exit;
    }

    DMARxDscrTab = (ETH_DMADescTypeDef *)rt_calloc(ETH_RXBUFNB, sizeof(ETH_DMADescTypeDef));
    if (DMARxDscrTab == RT_NULL)
    {
        LOG_E("No memory");
        state = -RT_ENOMEM;
        goto __exit;
    }

    DMATxDscrTab = (ETH_DMADescTypeDef *)rt_calloc(ETH_TXBUFNB, sizeof(ETH_DMADescTypeDef));
    if (DMATxDscrTab == RT_NULL)
    {
        LOG_E("No memory");
        state = -RT_ENOMEM;
        goto __exit;
    }
    ETH_GPIO_Configuration();
    stm32_eth_device.ETH_Speed = ETH_SPEED_100M;
    stm32_eth_device.ETH_Mode  = ETH_MODE_FULLDUPLEX;

    /* OUI 00-80-E1 STMICROELECTRONICS. */
    stm32_eth_device.dev_addr[0] = 0x00;
    stm32_eth_device.dev_addr[1] = 0x80;
    stm32_eth_device.dev_addr[2] = 0xE1;
    /* generate MAC addr from 96bit unique ID (only for test). */
    stm32_eth_device.dev_addr[3] = *(rt_uint8_t *)(UID_BASE + 4);
    stm32_eth_device.dev_addr[4] = *(rt_uint8_t *)(UID_BASE + 2);
    stm32_eth_device.dev_addr[5] = *(rt_uint8_t *)(UID_BASE + 0);

    stm32_eth_device.parent.parent.init       = rt_stm32_eth_init;
    stm32_eth_device.parent.parent.open       = rt_stm32_eth_open;
    stm32_eth_device.parent.parent.close      = rt_stm32_eth_close;
    stm32_eth_device.parent.parent.read       = rt_stm32_eth_read;
    stm32_eth_device.parent.parent.write      = rt_stm32_eth_write;
    stm32_eth_device.parent.parent.control    = rt_stm32_eth_control;
    stm32_eth_device.parent.parent.user_data  = RT_NULL;

    stm32_eth_device.parent.eth_rx     = rt_stm32_eth_rx;
    stm32_eth_device.parent.eth_tx     = rt_stm32_eth_tx;

    /* register eth device */
    state = eth_device_init(&(stm32_eth_device.parent), "e0");
    if (RT_EOK == state)
    {
        LOG_D("emac device init success");
    }
    else
    {
        LOG_E("emac device init faild: %d", state);
        state = -RT_ERROR;
        goto __exit;
    }

    /* start phy monitor */
    rt_thread_t tid;
    tid = rt_thread_create("phy",
                           phy_monitor_thread_entry,
                           RT_NULL,
                           1024,
                           RT_THREAD_PRIORITY_MAX - 2,
                           2);
    if (tid != RT_NULL)
    {
        rt_thread_startup(tid);
    }
    else
    {
        state = -RT_ERROR;
    }
__exit:
    if (state != RT_EOK)
    {
        if (Rx_Buff)
        {
            rt_free(Rx_Buff);
        }

        if (Tx_Buff)
        {
            rt_free(Tx_Buff);
        }

        if (DMARxDscrTab)
        {
            rt_free(DMARxDscrTab);
        }

        if (DMATxDscrTab)
        {
            rt_free(DMATxDscrTab);
        }
    }

    return state;
}
INIT_DEVICE_EXPORT(rt_hw_stm32_eth_init);
#endif

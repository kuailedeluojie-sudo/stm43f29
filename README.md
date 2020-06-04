# stm43f29
是基于stm32f429的程序包，完善的SPI和FLASH通信驱动，IIC和EPROM驱动，文件系统，DP83848驱动

## 引脚说明
	* SPI驱动引脚 
	SPI3 GPIO Configuration <br>
    PB3     ------> SPI3_SCK <br>
    PB4     ------> SPI3_MISO <br>
    PB5     ------> SPI3_MOSI <br>
    PB15    ------> SPI3_NSS  <Br>
	
	* 网卡引脚
	Configure ETH_MDIO                 PA2 <br>
	Configure ETH_MDC                  PC1 <br>
	Configure ETH_RMII_REF_CLK         PA1 <br>
	Configure ETH_RMII_CRS_DV          PA7 <br>
	Configure ETH_RMII_RXD0            PC4 <br>
	Configure ETH_RMII_RXD1            PC5 <br>
	Configure ETH_RMII_TX_EN           PG11 <br>
	Configure ETH_RMII_TXD0            PG13 <br>
	Configure ETH_RMII_TXD1            PG14 <br>
	Configure ETH_RMII1_TX_EN          PB11 <br>
	Configure ETH_RMII1_TXD0           PB12 <br>
	Configure ETH_RMII1_TXD1           PB13 <br>
	Configure 	  				       PA8 <br>
	

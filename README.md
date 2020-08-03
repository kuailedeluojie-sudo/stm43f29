# stm43f29
是基于stm32f429的程序包，完善的SPI和FLASH通信驱动，IIC和EPROM驱动，文件系统，DP83848驱动

## 引脚说明
	* SPI驱动引脚 
	SPI3 GPIO Configuration 
    PB3     ------> SPI3_SCK 
    PB4     ------> SPI3_MISO 
    PB5     ------> SPI3_MOSI 
    PB15    ------> SPI3_NSS  
	
	* 网卡引脚
	Configure ETH_MDIO                 PA2 
	Configure ETH_MDC                  PC1 
	Configure ETH_RMII_REF_CLK         PA1 
	Configure ETH_RMII_CRS_DV          PA7 
	Configure ETH_RMII_RXD0            PC4 
	Configure ETH_RMII_RXD1            PC5 
	Configure ETH_RMII_TX_EN           PG11 
	Configure ETH_RMII_TXD0            PG13 
	Configure ETH_RMII_TXD1            PG14 
	Configure ETH_RMII1_TX_EN          PB11 
	Configure ETH_RMII1_TXD0           PB12 
	Configure ETH_RMII1_TXD1           PB13 
	Configure GPIO pin : PA8 
	
	* IIC引脚
	PA8
	PC9
	
## 升级编辑器后的一些更改
	 #define RT_DFS_ELM_MAX_SECTOR_SIZE 4096 外部falsh的大小
#### bootloader的一些设置
	 VECT_TAB_OFFSET 修改成软件的起始位置
	 STM32_FLASH_START_ADRESS flash的起始位置
	 link.lds中设置起始位置
#### 打印信息串口的设置
	 Settings 中更多设置-->内核-->内核设备对象-->rt_kprintf控制台
/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-01-08     tyustli      the first version
 *
 */

#ifndef __DRV_SPI_H_
#define __DRV_SPI_H_

#include "board.h"

rt_err_t rt_hw_spi_bus_register(SPI_TypeDef *spi_num, char *bus_name);
rt_err_t rt_hw_spi_device_attach(const char *bus_name, const char *device_name, GPIO_TypeDef* cs_gpiox, uint16_t cs_gpio_pin);

struct stm32_hw_spi_cs
{
    GPIO_TypeDef* GPIOx;
    uint16_t GPIO_Pin;
};

struct stm32_spi_config
{
    SPI_TypeDef *Instance;
    char *bus_name;
};

struct stm32_spi_device
{
    rt_uint32_t pin;
    char *bus_name;
    char *device_name;
};

/* stm32 spi dirver class */
struct stm32_spi
{
    SPI_HandleTypeDef handle;
    struct stm32_spi_config *config;
    struct rt_spi_configuration *cfg;
    struct rt_spi_bus spi_bus;
};

#endif /*__DRV_SPI_H_ */

/*************** end of file *********************/

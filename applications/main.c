/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-09-09     RT-Thread    first version
 */

#include <rtthread.h>
#include <board.h>
#include <rtdevice.h>
#include <sfud.h>
#include "update.h"
#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>
#include "drv_soft_i2c.h"
//2020.03.27文件系统
#include <dfs_fs.h>
#include <dfs_file.h>
//2020.6.4  VTOR  VECT_TAB_OFFSET bootloader的重点

/* PLEASE DEFINE the LED0 pin for your board, such as: PC8 */
#define LED0_PIN    GET_PIN(C, 8)


int main(void)
{
    int count = 1;
    /* set LED0 pin mode to output */
    rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);
    tcp_update_init();
    rt_hw_i2c_init("i2c3", GET_PIN(A,8), GET_PIN(C,9));
    //devfs_init();

 //   dfs_mkfs("elm","flash0"); //格式化外部flash，挂载文件系统


        if (dfs_mount("flash0", "/", "elm", 0, 0) == 0)
        {
            rt_kprintf("file system initialization done!\n");
        }
    while (count++)
    {
        /* set LED0 pin level to high or low */
        rt_pin_write(LED0_PIN, count % 2);
     //   LOG_D("Hello RT-Thread!");

        rt_thread_mdelay(1000);
    }

    return RT_EOK;
}

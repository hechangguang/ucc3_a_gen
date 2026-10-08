/* 
 * gpio_v100.h -- gpio v100 驱动接口头文件,不对用户开放
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __TDL_GPIO_V100_H__
#define __TDL_GPIO_V100_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <base/ts_type.h>
#include <cpl/ts_cpl.h>
#include <uapi/chip_def.h>
#include <uapi/gpio_v100.h>

#define GPIO_BANK_MAX_IONUM     (32)

/* ioctl */
typedef enum ts_gpio_ioctl_en {
    IOC_NR_GPIO_INIT = 0,
    IOC_NR_GPIO_DEINIT = 1,

    IOC_NR_GPIO_BUTT,
} ts_gpio_ioctl_en_t;

/* ioctl 传递结构体 */
typedef struct ts_gpio_ioctl_s {
    /* 管脚序号, 多 PIN 可以通过 GPIO 定义或上，如: TS_GPIO_BANK0_10 | TS_GPIO_BANK0_20 
      不能跨 BANK */
    ts_u32 u32_pin_no;

    /* 读写的值 */
    ts_u32 u32_value;
} ts_gpio_ioctl_s_t;


#define TS_IOCTL_IOC_NR_GPIO_INIT       TS_CPL_IOW(TS_IOC_MAGIC_GPIO, IOC_NR_GPIO_INIT, ts_u32)
#define TS_IOCTL_IOC_NR_GPIO_DEINIT     TS_CPL_IOW(TS_IOC_MAGIC_GPIO, IOC_NR_GPIO_DEINIT, ts_u32)


/* register offset */
#define GPIO_TG_OFFSET_DR                   (0x0000)
#define GPIO_TG_OFFSET_DDR                  (0x0004)
#define GPIO_TG_OFFSET_CTL                  (0x0008)
#define GPIO_TG_OFFSET_INTEN                (0x0030)
#define GPIO_TG_OFFSET_INTMASK              (0x0034)
#define GPIO_TG_OFFSET_INTTYPE_LEVEL        (0x0038)
#define GPIO_TG_OFFSET_INT_POLARITY         (0x003C)
#define GPIO_TG_OFFSET_INTSTATUS            (0x0040)
#define GPIO_TG_OFFSET_RAW_INTSTATUS        (0x0044)
#define GPIO_TG_OFFSET_DEBOUNCE             (0x0048)
#define GPIO_TG_OFFSET_EOI                  (0x004c)
#define GPIO_TG_OFFSET_EXT_PORTA            (0x0050)
#define GPIO_TG_OFFSET_LS_SYNC              (0x0060)
#define GPIO_TG_OFFSET_ID_CODE              (0x0064)
#define GPIO_TG_OFFSET_INT_BOTHEDEG         (0x0068)

/* 单 GPIO 设备 */
typedef struct gpio_dev_s {
    /* 保存寄存器地址 */
    ts_u32 u32_regbase;

    /* 中断号 */
    ts_s32 s32_irq_no;

    /* 中断函数回调 */
    ts_gpio_irqcb_t afp_cb[GPIO_BANK_MAX_IONUM];
} gpio_dev_s_t;


/* 多组 GPIO 设备组合 */
typedef struct gpio_grp_s {
    gpio_dev_s_t ast_dev[TS_GPIO_MAX_GRP];

    /* 实际设备数，根据设备树扫描确定，但不能大于 TS_GPIO_MAX_BANK */
    ts_u32 u32_bank_num;
} gpio_grp_s_t;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __TDL_GPIO_V100_H__ */

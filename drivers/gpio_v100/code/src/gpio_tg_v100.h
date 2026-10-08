/* 
 * gpio_tg_v100.h -- gpio_tg_v100 驱动实现头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __GPIO_TG_V100_H__
#define __GPIO_TG_V100_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <config.h>
#include <base/ts_type.h>
#include <base/ts_errno.h>

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

#define GPIO_TG_OFFSET_VER_ID_CODE          (0x006C)
#define GPIO_TG_OFFSET_CONFIG_REG2          (0x0070)
#define GPIO_TG_OFFSET_CONFIG_REG1          (0x0074)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __GPIO_TG_V100_H__ */

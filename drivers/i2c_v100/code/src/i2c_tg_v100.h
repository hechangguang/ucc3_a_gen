/* 
 * i2c_tg_v100.h -- i2c_tg_v100 驱动实现头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __I2C_TG_V100_H__
#define __I2C_TG_V100_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <config.h>
#include <base/ts_type.h>
#include <base/ts_errno.h>

/* register offset */
#define I2C_TG_OFFSET_CTRL                  (0x0000)
#define I2C_TG_OFFSET_TARGET                (0x0004)
#define I2C_TG_OFFSET_SAR                   (0x0008)
#define I2C_TG_OFFSET_DAT_CMD               (0x0010)
#define I2C_TG_OFFSET_SCLCNT_H              (0x001C)
#define I2C_TG_OFFSET_SCLCNT_L              (0x0020)

#define I2C_TG_OFFSET_INTR_STAT             (0x002C)
#define I2C_TG_OFFSET_INTR_MASK             (0x0030)
#define I2C_TG_OFFSET_RAW_STAT              (0x0034)

#define I2C_TG_OFFSET_TL_RX                 (0x0038)
#define I2C_TG_OFFSET_TL_TX                 (0x003C)

#define I2C_TG_OFFSET_CLR_INTR              (0x0040)
#define I2C_TG_OFFSET_CLR_RX_UNDER          (0x0044)
#define I2C_TG_OFFSET_CLR_RX_OVER           (0x0048)
#define I2C_TG_OFFSET_CLR_TX_OVER           (0x004C)
#define I2C_TG_OFFSET_CLR_RD_REQ            (0x0050)
#define I2C_TG_OFFSET_CLR_TX_ABRT           (0x0054)
#define I2C_TG_OFFSET_CLR_RX_DONE           (0x0058)
#define I2C_TG_OFFSET_CLR_ACT               (0x005C)
#define I2C_TG_OFFSET_CLR_STOP_DET          (0x0060)
#define I2C_TG_OFFSET_CLR_START_DET         (0x0064)
#define I2C_TG_OFFSET_CLR_GEN_CALL          (0x0068)

#define I2C_TG_OFFSET_ENABLE                (0x006C)
#define I2C_TG_OFFSET_STATUS                (0x0070)

#define I2C_TG_OFFSET_FLR_TX                (0x0074)
#define I2C_TG_OFFSET_FLR_RX                (0x0078)

#define I2C_TG_OFFSET_SDA_HOLD              (0x007C)

#define I2C_TG_OFFSET_ABRT_SRC              (0x0080)

#define I2C_TG_OFFSET_SLV_DATA_NACK_ONLY    (0x0084)

#define I2C_TG_OFFSET_SDA_SETUP             (0x0094)
#define I2C_TG_OFFSET_ENABLE_STATUS         (0x009C)

#define I2C_TG_OFFSET_PARAM1                (0x00F4)
#define I2C_TG_OFFSET_VERSION               (0x00F8)
#define I2C_TG_OFFSET_TYPE                  (0x00FC)


/* for I2C_TG_OFFSET_DAT_CMD */
#define I2C_BIT_CMD_READ                    (1 << 8)
#define I2C_BIT_CMD_STOP                    (1 << 9)


/* for I2C_TG_OFFSET_RAW_STAT */
#define I2C_BIT_RAW_STAT_RX_DONE            (1 << 7)

/* I2C_TG_OFFSET_ENABLE */
#define I2C_BIT_DEV_DISENABLE               (0)
#define I2C_BIT_DEV_ENABLE                  (1 << 0)

/* I2C_TG_OFFSET_STATUS */
#define I2C_BIT_DEV_ACT         (0x01)
#define I2C_BIT_DEV_IDLE        (0x00)
#define I2C_BIT_DEV_TFNF        (0x02)
#define I2C_BIT_DEV_BUSY        (0x05)

/* interrupt bit map */
#define I2C_BIT_INTRPT_RX_OVER      (1)
#define I2C_BIT_INTRPT_RX_FULL      (2)
#define I2C_BIT_INTRPT_RD_REQ       (5)
#define I2C_BIT_INTRPT_STOP         (9)

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __I2C_TG_V100_H__ */

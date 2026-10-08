/*
 * chip_def.h -- tdk 芯片定义头文件, 此文件为 T10x 定制
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023-2025 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __UAPI_CHIP_DEF_H__
#define __UAPI_CHIP_DEF_H__

#include <base/ts_type.h>
#include <base/ts_comm.h>

#include <kapi/k_console_v100.h>
#include <uapi/iomux.h>

/* === soc 定义 === */
#define TS_CACHE_LINE_SIZE          (32)
#define TS_HW_ALIGN_SIZE            (TS_CACHE_LINE_SIZE)

#define TS_UART_MAX_NUM             (5)
#define TS_I2C_MAX_NUM              (4)
#define TS_GPIO_MAX_GRP             (2)
#define TS_PWM_MAX_NUM              (4)
#define TS_QDR_MAX_NUM              (2)

#define TS_USB_EP_MAX_NUM           (4)

/* SCU 寄存器指定 */
#define TS_SUC_REGBASE          (0x00160000)
#define TS_SUC_MUX_OFFSET       (0x0040)

#define TS_SUC_CFG_E4_OFFSET    (0x0090)
#define TS_SUC_CFG_E8_OFFSET    (0x00B0)
#define TS_SUC_CFG_PD_OFFSET    (0x0130)
#define TS_SUC_CFG_PU_OFFSET    (0x0150)
#define TS_SUC_CFG_SMT_OFFSET   (0x01B0)
#define TS_SUC_CFG_SR_OFFSET    (0x01D0)

/* SFC 定义 */
#define TS_SFC_MAX_NUM              (2)
#define TS_SFC_INSIDE_DEV           (0)
#define TS_SFC_OUTSIDE_DEV          (1)

/* GPIO 定义 */
#define TS_GPIO_BANK0_0         (1 << 0)
#define TS_GPIO_BANK0_1         (1 << 1)
#define TS_GPIO_BANK0_2         (1 << 2)
#define TS_GPIO_BANK0_3         (1 << 3)
#define TS_GPIO_BANK0_4         (1 << 4)
#define TS_GPIO_BANK0_5         (1 << 5)
#define TS_GPIO_BANK0_6         (1 << 6)
#define TS_GPIO_BANK0_7         (1 << 7)
#define TS_GPIO_BANK0_8         (1 << 8)
#define TS_GPIO_BANK0_9         (1 << 9)
#define TS_GPIO_BANK0_10        (1 << 10)
#define TS_GPIO_BANK0_11        (1 << 11)
#define TS_GPIO_BANK0_12        (1 << 12)
#define TS_GPIO_BANK0_13        (1 << 13)
#define TS_GPIO_BANK0_14        (1 << 14)
#define TS_GPIO_BANK0_15        (1 << 15)
#define TS_GPIO_BANK0_16        (1 << 16)
#define TS_GPIO_BANK0_17        (1 << 17)
#define TS_GPIO_BANK0_18        (1 << 18)
#define TS_GPIO_BANK0_19        (1 << 19)
#define TS_GPIO_BANK0_20        (1 << 20)
#define TS_GPIO_BANK0_21        (1 << 21)
#define TS_GPIO_BANK0_22        (1 << 22)
#define TS_GPIO_BANK0_23        (1 << 23)
#define TS_GPIO_BANK0_24        (1 << 24)
#define TS_GPIO_BANK0_25        (1 << 25)
#define TS_GPIO_BANK0_26        (1 << 26)
#define TS_GPIO_BANK0_27        (1 << 27)
#define TS_GPIO_BANK0_28        (1 << 28)
#define TS_GPIO_BANK0_29        (1 << 29)
#define TS_GPIO_BANK0_30        (1 << 30)
#define TS_GPIO_BANK0_31        (1 << 31)

#define TS_GPIO_BANK1_32        (1 << 0)
#define TS_GPIO_BANK1_33        (1 << 1)
#define TS_GPIO_BANK1_34        (1 << 2)
#define TS_GPIO_BANK1_35        (1 << 3)
#define TS_GPIO_BANK1_36        (1 << 4)
#define TS_GPIO_BANK1_37        (1 << 5)
#define TS_GPIO_BANK1_38        (1 << 6)
#define TS_GPIO_BANK1_39        (1 << 7)
#define TS_GPIO_BANK1_40        (1 << 8)
#define TS_GPIO_BANK1_41        (1 << 9)
#define TS_GPIO_BANK1_42        (1 << 10)
#define TS_GPIO_BANK1_43        (1 << 11)
#define TS_GPIO_BANK1_44        (1 << 12)
#define TS_GPIO_BANK1_45        (1 << 13)
#define TS_GPIO_BANK1_46        (1 << 14)
#define TS_GPIO_BANK1_47        (1 << 15)
#define TS_GPIO_BANK1_48        (1 << 16)
#define TS_GPIO_BANK1_49        (1 << 17)
#define TS_GPIO_BANK1_50        (1 << 18)
#define TS_GPIO_BANK1_51        (1 << 19)
#define TS_GPIO_BANK1_52        (1 << 20)
#define TS_GPIO_BANK1_53        (1 << 21)
#define TS_GPIO_BANK1_54        (1 << 22)
#define TS_GPIO_BANK1_55        (1 << 23)
#define TS_GPIO_BANK1_56        (1 << 24)
#define TS_GPIO_BANK1_57        (1 << 25)
#define TS_GPIO_BANK1_58        (1 << 26)
#define TS_GPIO_BANK1_59        (1 << 27)
#define TS_GPIO_BANK1_60        (1 << 28)
#define TS_GPIO_BANK1_61        (1 << 29)
#define TS_GPIO_BANK1_62        (1 << 30)
#define TS_GPIO_BANK1_63        (1 << 31)


/* === viu 定义 === */
#define TS_VIU_MDEV0                TS_MDEV_SET(TS_IOC_MAGIC_VIU, 0, 0, 0)
#define TS_VIU_MAX_GRP_NUM          (1)
#define TS_VIU_MAX_CHN_NUM          (1)

/* === isp 定义 === */
#define TS_ISP_MAX_DEV_NUM          (1)

#define TS_ISP_MDEV0                TS_MDEV_SET(TS_IOC_MAGIC_ISP, 0, 0, 0)
#if (TS_ISP_MAX_DEV_NUM > 1)
#define TS_ISP_MDEV1                TS_MDEV_SET(TS_IOC_MAGIC_ISP, 0, 1, 0)
#endif

/* === aisp 定义 === */
#define TS_AISP_MAX_DEV_NUM          (1)

#define TS_AISP_MDEV0                TS_MDEV_SET(TS_IOC_MAGIC_AISP, 0, 0, 0)
#if (TS_AISP_MAX_DEV_NUM > 1)
#define TS_AISP_MDEV1                TS_MDEV_SET(TS_IOC_MAGIC_AISP, 0, 1, 0)
#endif

/* === osd 定义 === */
#define TS_OSD_MAX_GRP_NUM          (1)
#define TS_OSD_MAX_CHN_NUM          (2)

/* 通道定义 */
#define TS_OSD_DISP_MDEV0           TS_MDEV_SET(TS_IOC_MAGIC_OSD, 0, 0, 0)
#define TS_OSD_CVBS_MDEV0           TS_MDEV_SET(TS_IOC_MAGIC_OSD, 0, 1, 0)

/* 所有通道 */
#define TS_OSD_ALL_MDEV0            TS_MDEV_SET(TS_IOC_MAGIC_OSD, 0, TS_OSD_MAX_CHN_NUM, 0)

/* === vpo 定义 === */
#define TS_VPO_GRP0                 (0)
#define TS_VPO_MDEV0                TS_MDEV_SET(TS_IOC_MAGIC_VPO, TS_VPO_GRP0, 0, 0)
#define TS_VPO_MDEV1                TS_MDEV_SET(TS_IOC_MAGIC_VPO, TS_VPO_GRP0, 1, 0)
#define TS_VPO_MDEV2                TS_MDEV_SET(TS_IOC_MAGIC_VPO, TS_VPO_GRP0, 2, 0)
#define TS_VPO_MDEV3                TS_MDEV_SET(TS_IOC_MAGIC_VPO, TS_VPO_GRP0, 3, 0)
#define TS_VPO_MDEV4                TS_MDEV_SET(TS_IOC_MAGIC_VPO, TS_VPO_GRP0, 4, 0)
#define TS_VPO_MDEV5                TS_MDEV_SET(TS_IOC_MAGIC_VPO, TS_VPO_GRP0, 5, 0)
#define TS_VPO_MDEV6                TS_MDEV_SET(TS_IOC_MAGIC_VPO, TS_VPO_GRP0, 6, 0)

/* 别名定义 */
#define TS_VPO_CVBS_MDEV            TS_VPO_MDEV0
#define TS_VPO_LCD_MDEV             TS_VPO_MDEV1
#define TS_VPO_BTENC_MDEV           TS_VPO_MDEV2
#define TS_VPO_MIPI_CSI_MDEV        TS_VPO_MDEV3
#define TS_VPO_MIPI_DSI_MDEV        TS_VPO_MDEV4
#define TS_VPO_MEM_MDEV             TS_VPO_MDEV5
#define TS_VPO_NULL_MDEV            TS_VPO_MDEV6 

#define TS_VPO_MAX_GRP_NUM          (1)
#define TS_VPO_MAX_CHN_NUM          (7)


#endif /* __UAPI_CHIP_DEF_H__ */

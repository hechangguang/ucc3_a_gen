/* 
 * k_dmac_v100.h -- dmac v100 驱动接口头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023-2025 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __K_DMAC_V100_H__
#define __K_DMAC_V100_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <config.h>
#include <base/ts_type.h>
#include <base/ts_errno.h>

#include <uapi/dmac_v100.h>
#include <cpl/ts_cpl.h>

#define DMAC_TG_CHAN_OFFSET_BASE                    (0x0100)

#define DMAC_TG_OFFSET_ID                           (0x0000)
#define DMAC_TG_OFFSET_VERSION                      (0x0008)
#define	DMAC_TG_OFFSET_CFG                          (0x0010)
#define	DMAC_TG_OFFSET_CHEN                         (0x0018)
#define	DMAC_TG_OFFSET_INT_STATUS                   (0x0030)
#define	DMAC_TG_OFFSET_COMM_INT_CLEAR               (0x0038)
#define	DMAC_TG_OFFSET_COMM_INT_STATUS_ENABLE       (0x0040)
#define	DMAC_TG_OFFSET_COMM_INT_SIGNAL_ENABLE       (0x0048)
#define	DMAC_TG_OFFSET_COMM_INT_STATUS              (0x0050)
#define	DMAC_TG_OFFSET_RESET                        (0x0058)

#define	DMAC_TG_OFFSET_LOWPWR_CFG                   (0x0060)
#define	DMAC_TG_OFFSET_COMM_ECC_CTL_STATUS          (0x0078)

#define DMAC_CHN_OFFSET_SAR                         (0x0000)
#define DMAC_CHN_OFFSET_DAR                         (0x0008)
#define DMAC_CHN_OFFSET_BLK_TS                      (0x0010)
#define DMAC_CHN_OFFSET_CTL                         (0x0018)
#define DMAC_CHN_OFFSET_CFG                         (0x0020)
#define DMAC_CHN_OFFSET_LLP                         (0x0028)
#define DMAC_CHN_OFFSET_STATUS                      (0x0030)
#define DMAC_CHN_OFFSET_SWHS_SRC                    (0x0038)
#define DMAC_CHN_OFFSET_SWHS_DST                    (0x0040)
#define DMAC_CHN_OFFSET_BLK_TFR_RESUME_REQ          (0x0048)
#define DMAC_CHN_OFFSET_AXI_ID                      (0x0050)
#define DMAC_CHN_OFFSET_AXI_QOS                     (0x0058)

#define DMAC_CHN_OFFSET_SSTAT                       (0x0060)
#define DMAC_CHN_OFFSET_DSTAT                       (0x0068)
#define DMAC_CHN_OFFSET_SSTATAR                     (0x0070)
#define DMAC_CHN_OFFSET_DSTATAR                     (0x0078)

#define DMAC_CHN_OFFSET_INT_STATUS_ENABLE           (0x0080)
#define DMAC_CHN_OFFSET_INT_STATUS                  (0x0088)
#define DMAC_CHN_OFFSET_INT_SIGNAL_ENABLE           (0x0090)
#define DMAC_CHN_OFFSET_INT_CLEAR                   (0x0098)

/* bitmap */
#define DMAC_BIT_CFG_INT_EN         (1)
#define DMAC_BIT_CFG_DMAC_EN        (0)

#define DMAC_BIT_RESET              (0)

/* channel */
#define DMAC_BIT_CHN_EN(chn)        (TS_BIT(chn))
#define DMAC_BIT_CHN_EN_WE(chn)     (TS_BIT(chn + 8))

/* CHx_CTL LOW bits */
#define DMAC_BIT_CHN_CTL_SMS            (0)
#define DMAC_BIT_CHN_CTL_DMS            (2)
#define DMAC_BIT_CHN_CTL_SINC           (4)
#define DMAC_BIT_CHN_CTL_DINC           (6)
#define DMAC_BIT_CHN_CTL_SRC_TR_WIDTH   (8)
#define DMAC_BIT_CHN_CTL_DST_TR_WIDTH   (11)
#define DMAC_BIT_CHN_CTL_SRC_MSIZE      (14)
#define DMAC_BIT_CHN_CTL_DST_MSIZE      (18)
#define DMAC_BIT_CHN_CTL_AR_CACHE       (22)
#define DMAC_BIT_CHN_CTL_AW_CACHE       (26)

/* CHx_CTL HI bits */
#define DMAC_BIT_CHN_CTL_ARLEN_EN       (6)
#define DMAC_BIT_CHN_CTL_ARLEN          (7)
#define DMAC_BIT_CHN_CTL_AWLEN_EN       (15)
#define DMAC_BIT_CHN_CTL_AWLEN          (16)
#define DMAC_BIT_CHN_CTL_IOC_BLKTFR     (26)

#define DMAC_TR_WIDTH_BITS_8            (0)
#define DMAC_TR_WIDTH_BITS_16           (1)
#define DMAC_TR_WIDTH_BITS_32           (2)
#define DMAC_TR_WIDTH_BITS_64           (3)
#define DMAC_TR_WIDTH_BITS_128          (4)
#define DMAC_TR_WIDTH_BITS_256          (5)
#define DMAC_TR_WIDTH_BITS_512          (6)

#define DMAC_MSZIE_DATA_ITEM_1          (0)
#define DMAC_MSZIE_DATA_ITEM_4          (1)
#define DMAC_MSZIE_DATA_ITEM_8          (2)
#define DMAC_MSZIE_DATA_ITEM_16         (3)
#define DMAC_MSZIE_DATA_ITEM_32         (4)
#define DMAC_MSZIE_DATA_ITEM_64         (5)
#define DMAC_MSZIE_DATA_ITEM_128        (6)
#define DMAC_MSZIE_DATA_ITEM_256        (7)
#define DMAC_MSZIE_DATA_ITEM_512        (8)
#define DMAC_MSZIE_DATA_ITEM_1024       (9)

#define DMAC_SADDR_INC                  (0)
#define DMAC_SADDR_FIXED                TS_BIT(4)

#define DMAC_DADDR_INC                  (0)
#define DMAC_DADDR_FIXED                TS_BIT(6)

/* cfg */
#define DMAC_BIT_CHN_TT_FC              (0)

/* interrupt flag */
#define DMAC_BIT_CHN_TFR_DONE           TS_BIT(1)

#define DMAC_CHN_IDLE                   (0)
#define DMAC_CHN_BUSY                   (1)

/* ioctl */
enum {
    /* DMAC设备初始化 */
    IOC_NR_DMAC_INIT = 0,

    /* 设备去初始化 */
    IOC_NR_DMAC_DEINIT,

    IOC_NR_DMA_BUTT
};

#define TS_IOCTL_IOC_NR_DMAC_INIT               TS_CPL_IOR(TS_IOC_MAGIC_DMA, IOC_NR_DMAC_INIT, ts_u32)
#define TS_IOCTL_IOC_NR_DMAC_DEINIT             TS_CPL_IOW(TS_IOC_MAGIC_DMA, IOC_NR_DMAC_DEINIT, ts_u32)



typedef struct dmac_dev_s {
    /* 寄存器基地址 */
    ts_u32 u32_regbase;

    /* 统一互斥 */
    ts_cpl_mutex_s_t st_mutex;

    /* 是否带 ecc */
    ts_u08 u08_support_ecc;

    /* 总线 port 口数量 */
    ts_u08 u08_port_num;

    /* 通道数 */
    ts_u16 u16_chn_num;

    /* 总线位宽 */
    ts_u16 u16_bus_width;

    /* burst 长度 */
    ts_u16 u16_burst_len;

    /* 实际 dmac 私有变量 */
    ts_void *pv_priv;
} dmac_dev_s_t;

typedef struct dmac_chn_s {
    /* 通道相关寄存器组基于基地址的偏移量 */
    ts_u32 u32_reg_addr;

    /* 通道 ID */
    ts_u16 u16_id;

    /* 此通道是否被占用 */
    ts_u16 u16_busy;

    /* 保存中断状态 */
    ts_u32 u32_int_status;

    /* 等待 */
    ts_cpl_wait_s_t st_wait;

    /* 保存设备指针 */
    ts_void *pv_priv;
} dmac_chn_s_t;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __K_DMAC_V100_H__ */

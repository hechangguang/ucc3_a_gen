/* 
 * dmac_v100.h -- dmac v100 API 头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __API_DMAC_V100_H__
#define __API_DMAC_V100_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <base/ts_type.h>
#include <base/ts_errno.h>

#include <cpl/ts_cpl.h>


#define TS_DMAC_XFER_ADDR_NORMAL        (0)
#define TS_DMAC_XFER_ADDR_SINC_ONLY     (1)
#define TS_DMAC_XFER_ADDR_DINC_ONLY     (2)
#define TS_DMAC_XFER_ADDR_NO_CHANGE     (3)

#define TS_DMAC_XFER_TTFC_M2M_DMAC      (0)
#define TS_DMAC_XFER_TTFC_M2P_DMAC      (1)
#define TS_DMAC_XFER_TTFC_P2M_DMAC      (2)
#define TS_DMAC_XFER_TTFC_P2P_DMAC      (3)
#define TS_DMAC_XFER_TTFC_P2M_SRC       (4)
#define TS_DMAC_XFER_TTFC_P2P_SRC       (5)
#define TS_DMAC_XFER_TTFC_M2P_DST       (6)
#define TS_DMAC_XFER_TTFC_P2P_DST       (7)


typedef struct ts_dma_attr_s {
    /* 通道句柄 */
    ts_void *pv_hdl;

    /* 传输的源地址 */
    ts_dma_addr_t da_src_addr;

    /* 传输的目标地址 */
    ts_dma_addr_t da_dst_addr;

    /* 传输的大小 */
    ts_u32 u32_size;

    /* 传输的位宽 */
    ts_u32 u32_width;

    /* 传输地址处理类型 */
    ts_u32 u32_addr_type;

    /* 传输类型和流程控制 */
    ts_u32 u32_tt_fc;
} ts_dma_attr_s_t;

/* function list */



/* 
 * 函数名 :  ts_lib_dma_memcpy
 * 描  述 :  DMAC 按照 32 bit 为单位进行传输, 
 * 输  入 :  参数               描述
 *           pv_dst             DMA 目标地址
 *           pv_src             DMA 源地址
 *           u32_count          DMA 拷贝个数， byte 为单位
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           = pv_dst           成功
 *           ts_null            失败
 */
ts_void *ts_lib_dma_memcpy(ts_void *pv_dst, const ts_void *pv_src, ts_u32 u32_count);


/* 
 * 函数名 :  ts_lib_dma_memcpy64
 * 描  述 :  DMAC 按照 64 bit 为单位进行传输, 
 * 输  入 :  参数               描述
 *           pv_dst             DMA 目标地址
 *           pv_src             DMA 源地址
 *           u32_count          DMA 拷贝个数， byte 为单位
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           = pv_dst           成功
 *           ts_null            失败
 */
ts_void *ts_lib_dma_memcpy64(ts_void *pv_dst, const ts_void *pv_src, ts_u32 u32_count);


/* 
 * 函数名 :  ts_lib_dma_memcpy16
 * 描  述 :  DMAC 按照 16 bit 为单位进行传输, 
 * 输  入 :  参数               描述
 *           pv_dst             DMA 目标地址
 *           pv_src             DMA 源地址
 *           u32_count          DMA 拷贝个数， byte 为单位
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           = pv_dst           成功
 *           ts_null            失败
 */
ts_void *ts_lib_dma_memcpy16(ts_void *pv_dst, const ts_void *pv_src, ts_u32 u32_count);


/* 
 * 函数名 :  ts_lib_dma_memcpy8
 * 描  述 :  DMAC 按照 8 bit 为单位进行传输, 
 * 输  入 :  参数               描述
 *           pv_dst             DMA 目标地址
 *           pv_src             DMA 源地址
 *           u32_count          DMA 拷贝个数， byte 为单位
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           = pv_dst           成功
 *           ts_null            失败
 */
ts_void *ts_lib_dma_memcpy8(ts_void *pv_dst, const ts_void *pv_src, ts_u32 u32_count);


/* 
 * 函数名 :  ts_lib_dmac_deinit
 * 描  述 :  DMAC 库去初始化
 * 输  入 :  参数           描述
 *           无             无
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          去初始化成功
 */
ts_s32 ts_lib_dmac_deinit(ts_void);


/* 
 * 函数名 :  ts_lib_dmac_init
 * 描  述 :  初始化 DMAC 库
 * 输  入 :  参数           描述
 *           无             无
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          初始化成功
 *           TS_ERR        初始化失败
 */
extern ts_s32 ts_lib_dmac_init(ts_void);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __API_DMAC_V100_H__ */

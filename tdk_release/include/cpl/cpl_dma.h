/* 
 * cpl_dma.h -- dma 内存部分 cpl 头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2022-2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __CPL_DMA_H__
#define __CPL_DMA_H__

#include "cpl/cpl_device.h"

typedef enum ts_cpl_dma_data_direct_en {
    TS_CPL_DMA_BIDIRECTIONAL = 0,
    TS_CPL_DMA_TO_DEVICE,
    TS_CPL_DMA_FROM_DEVICE,
    TS_CPL_DMA_NONE,

    TS_CPL_DMA_BUTT
} ts_cpl_dma_data_direct_en_t;

extern ts_void *ts_cpl_dma_alloc_coherent(ts_cpl_dev_s_t *, ts_size_t, ts_dma_addr_t *, ts_u32);
extern ts_void ts_cpl_dma_free_coherent(ts_cpl_dev_s_t *, ts_size_t, ts_void *, ts_dma_addr_t);
extern ts_dma_addr_t ts_cpl_dma_map_single(ts_cpl_dev_s_t *, ts_void *, ts_size_t, ts_cpl_dma_data_direct_en_t);
extern ts_void ts_cpl_dma_unmap_single(ts_cpl_dev_s_t *, ts_dma_addr_t, ts_size_t, ts_cpl_dma_data_direct_en_t);
extern ts_s32 ts_cpl_dma_set_mask_and_coherent(ts_cpl_dev_s_t *, ts_dma_addr_t);

#endif /* __CPL_DMA_H__ */

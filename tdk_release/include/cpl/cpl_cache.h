/* 
 * cpl_cache.h -- cpl cache 实现头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __CPL_CACHE_H__
#define __CPL_CACHE_H__

extern ts_u32 ts_cpl_get_dcache_linesize(ts_void);

extern ts_void ts_cpl_invalid_icache_all(ts_void);
extern ts_void ts_cpl_flush_dcache_all(ts_void);

/* exec invalid dcache */
extern ts_void ts_cpl_sync_dma_for_cpu(ts_void *pv_k_virt, ts_phys_addr_t pa_addr, ts_u32 u32_length);

/* exec flush dache(flush and invalid) */
extern ts_void ts_cpl_sync_dma_for_device(ts_void *pv_k_virt, ts_phys_addr_t pa_addr, ts_u32 u32_length);

#endif /* __CPL_CACHE_H__ */

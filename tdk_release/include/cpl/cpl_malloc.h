/*
 * cpl_malloc.h -- cpl 内存管理抽象层头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __TS_CPL_MALLOC_H__
#define __TS_CPL_MALLOC_H__

#define TS_CPL_GFP_KERNEL   (0)
#define TS_CPL_GFP_ATOMIC   (1)
#define TS_CPL_GFP_DMA      (3)

typedef unsigned int        ts_gfp_t;

extern ts_void *ts_cpl_kmalloc(ts_size_t sz_size, ts_u32 u32_flags);
extern ts_void *ts_cpl_kzalloc(ts_size_t sz_size, ts_u32 u32_flags);
extern ts_void ts_cpl_kfree(const ts_void *pv_addr);

extern ts_void *ts_cpl_kmalloc_align(ts_size_t sz_size, ts_u32 u32_flags, ts_u32 u32_align);

extern ts_void *ts_cpl_vmalloc(ts_size_t sz_size);
extern ts_void ts_cpl_vfree(const ts_void *pv_addr);

#endif /* __TS_CPL_MALLOC_H__ */

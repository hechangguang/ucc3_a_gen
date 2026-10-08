/*
 * cache.h -- tsp cache 实现头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __CACHE_H__
#define __CACHE_H__

#include <base/ts_type.h>

ts_u32 cache_get_linesize(ts_void);

ts_void icache_invalid_all(ts_void);
ts_void icache_invalid_area(ts_void *pv_phys_addr, ts_u32 u32_phys_size);

ts_void dcache_flush_all(ts_void);
ts_void dcache_invalid_area(ts_void *pv_phys_addr, ts_u32 u32_phys_size);
ts_void dcache_flush_area(ts_void *pv_phys_addr, ts_u32 u32_phys_size);

#endif /* __CACHE_H__ */

/*
 * slab.h -- tsp 实现内存管理类 Linux 接口头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */

#ifndef __LINUX_SLAB_H__
#define __LINUX_SLAB_H__

#include <config.h>
#include <base/ts_type.h>
#include "linux/gfp.h"


#ifdef CONFIG_FREERTOS
extern void *pvPortMalloc( size_t xWantedSize );
extern void vPortFree( void *pv );
extern void prvHeapInit( size_t uxHeapBase, size_t uxHeapSize );
#endif

#ifdef CONFIG_RT_THREAD
#include <rthw.h>
#include <rtthread.h>

extern rt_smem_t rt_smem_init(const char *name, void *begin_addr, rt_size_t size);
extern void *rt_smem_alloc(rt_smem_t m, rt_size_t size);
extern void *rt_smem_realloc(rt_smem_t m, void *rmem, rt_size_t newsize);
extern void rt_smem_free(void *rmem);
#endif

#define SLAB_NODE(p)            (((*(ts_u32 *)(((ts_u32)p) - 4)) & 0x7FFFFFFF) - 8)

void kfree(const void *);
void *kmalloc(size_t size, gfp_t flags);
ts_void *kzalloc(size_t size, gfp_t flags);
ts_void *kmm_realloc(FAR ts_void *oldmem, size_t newsize);
ts_void *kmm_zalloc(size_t size);

ts_void slab_init(ts_u32 u32_heap_base, ts_u32 u32_heap_size);

#endif    /* __LINUX_SLAB_H__ */

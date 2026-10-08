/* 
 * cpl_addr.h -- tdk 跨平台地址相关抽象层头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2022-2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __CPL_ADDR_H__
#define __CPL_ADDR_H__

#include <base/ts_type.h>
#include <base/ts_compiler.h>
#include <cpl/ts_cpl.h>

/* 通用联合体， 用于统一寄存器读写 */
typedef union ts_cpl_reg_comm_un {
    struct  {
        __IOM ts_u32 rev00_31:32; //[0..31]
    };
    __IOM ts_u32 u32_val; 
} ts_cpl_reg_comm_un_t;

ts_void *ts_cpl_ioremap(ts_phys_addr_t phys_addr, ts_size_t sz_size);
ts_void *ts_cpl_ioremap_nocache(ts_phys_addr_t phys_addr, ts_size_t sz_size);
ts_void *ts_cpl_ioremap_wc(ts_phys_addr_t phys_addr, ts_size_t sz_size);
ts_void *ts_cpl_ioremap_cached(ts_phys_addr_t phys_addr, ts_size_t sz_size);
ts_void ts_cpl_iounmap(ts_void *pv_addr);

ts_size_t ts_cpl_copy_from_user(ts_void *pv_to, const ts_void *pv_from, ts_size_t sz_num);
ts_size_t ts_cpl_copy_to_user(ts_void *pv_to, const ts_void *pv_from, ts_size_t sz_num);

ts_s32 ts_cpl_access_ok(ts_s32 s32_type, const ts_void *pv_addr, ts_size_t sz_size);

ts_void *ts_cpl_kmap_cached(ts_phys_addr_t phys_addr, ts_size_t sz_size);
ts_void *ts_cpl_kmap_noncached(ts_phys_addr_t phys_addr, ts_size_t sz_size);
ts_void *ts_cpl_kmap_writecombine(ts_phys_addr_t phys_addr, ts_size_t sz_size);
ts_s32 ts_cpl_unkmap_kernel(ts_void *pv_vaddr);

#define ts_cpl_readl(addr)              *(volatile ts_u32 *)(addr)
#define ts_cpl_writel(value, addr)      *(volatile ts_u32 *)(addr) = value
#define ts_cpl_reg(addr)				(*(volatile ts_u32 *)(addr))
#define REG(x)                  		(*(volatile unsigned int *)(x))

ts_u32 ts_cpl_readl_rmb(volatile ts_void *pv_addr);
ts_void ts_cpl_writel_wmb(ts_u32 u32_value, volatile ts_void *pv_addr);

#endif /* __CPL_ADDR_H__ */

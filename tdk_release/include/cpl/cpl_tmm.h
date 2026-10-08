/*
 * cpl_tmm.h -- cpl 内存管理抽象层头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __TS_CPL_TMM_H__
#define __TS_CPL_TMM_H__

#include <base/ts_type.h>

/* tmm  系统内存相关 */
typedef struct tmm_sys_info_s {
    ts_u32 u32_start_addr;
    ts_u32 u32_end_addr;
    ts_u32 u32_total_size;
} tmm_sys_info_s_t;

typedef struct tmm_sys_mem_s {
    /* TEXT 段 */
    tmm_sys_info_s_t st_text;

    /* DATA 段 */
    tmm_sys_info_s_t st_data;

    /* BSS 段 */
    tmm_sys_info_s_t st_bss;

    /* 指令紧耦合 */
    tmm_sys_info_s_t st_itcm;

    /* 数据紧耦合 */
    tmm_sys_info_s_t st_dtcm;

    /* 堆信息 */
    tmm_sys_info_s_t st_heap;
} tmm_sys_mem_s_t;

/* function list */
extern ts_s32 ts_cpl_tmm_init(ts_u32 u32_max_zone, ts_u32 u32_mode, ts_void *pv_mngt);
extern ts_void ts_cpl_tmm_deinit(ts_void);
extern ts_void ts_cpl_tmm_set_debug(ts_u32 u32_on);
extern ts_void ts_cpl_tmm_info(ts_void);

extern ts_void *ts_cpl_tmm_add_zone(ts_u32 u32_base_addr, ts_u32 u32_size, ts_u32 u32_align_size, ts_u32 u32_max_node, ts_void *pv_node_mngt);
extern ts_void *ts_cpl_tmm_get_zone(ts_u32 u32_idx);
extern ts_s32 ts_cpl_tmm_get_zone_size(ts_void *pv_zone);
extern ts_s32 ts_cpl_tmm_reset_zone(ts_void *pv_zone);
extern ts_void *ts_cpl_tmm_malloc(ts_void *pv_zone, ts_u32 u32_size, ts_u32 u32_align_size, const ts_char *pc_name);
extern ts_void ts_cpl_tmm_free(ts_void *pv_node);

extern ts_void ts_cpl_tmm_get_sys_mem(tmm_sys_mem_s_t *pst_mem);
extern ts_void *ts_cpl_tmm_dma_map(ts_void *pv_addr);

extern ts_u32 ts_cpl_tmm_get_mngt_size(ts_void);
extern ts_u32 ts_cpl_tmm_get_node_size(ts_void);

#endif /* __TS_CPL_MALLOC_H__ */


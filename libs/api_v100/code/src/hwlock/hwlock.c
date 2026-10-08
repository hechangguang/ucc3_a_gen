/* 
 * hwlock.c -- 硬件锁实现
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#include <config.h>
#include <base/ts_type.h>
#include <base/ts_errno.h>
#include <cpl/ts_cpl.h>
#include <uapi/hwlock_v100.h>

typedef struct hwlock_ctx_s {
    ts_u32 u32_regbase;
    ts_u32 u32_locks;
    ts_u32 u32_max;
    ts_cpl_mutex_s_t st_mutex;
} hwlock_ctx_s_t;

static hwlock_ctx_s_t sst_hwl_ctx;

#define HWLOCK_IS_UNLOCK        (1)
#define HWLOCK_IS_LOCK          (0)


ts_s32 ts_lib_hwlock_init(ts_u32 u32_regbase, ts_u32 u32_max_num)
{
    ts_s32 s32_ret;

    /* 只在用户层实现， Linux 需要将寄存器通过 mmap 映射到用户层 */
    sst_hwl_ctx.u32_regbase = u32_regbase;
    sst_hwl_ctx.u32_locks = 0;
    sst_hwl_ctx.u32_max = u32_max_num;

    s32_ret = ts_cpl_mutex_init((ts_cpl_mutex_s_t *)&sst_hwl_ctx.st_mutex);

    return s32_ret;
}


ts_void ts_lib_hwlock_deinit(ts_void)
{
    sst_hwl_ctx.u32_regbase = 0;
    sst_hwl_ctx.u32_locks = 0;
    sst_hwl_ctx.u32_max = 0;

    ts_cpl_mutex_destory((ts_cpl_mutex_s_t *)&sst_hwl_ctx.st_mutex);
}


ts_void *ts_lib_hwlock_get_free(ts_void)
{
    ts_u32 i;
    ts_u32 *pu32_lock;

    pu32_lock = (ts_u32 *)sst_hwl_ctx.u32_regbase;

    ts_cpl_mutex_lock((ts_cpl_mutex_s_t *)&sst_hwl_ctx.st_mutex);
    for (i = 0; i < sst_hwl_ctx.u32_max; i++) {
        if (0 == (sst_hwl_ctx.u32_locks & (1 << i))) {
            sst_hwl_ctx.u32_locks |= 1 << i;

            ts_cpl_mutex_unlock((ts_cpl_mutex_s_t *)&sst_hwl_ctx.st_mutex);
            ts_lib_hwlock_unlock(pu32_lock);

            return pu32_lock;
        }

        pu32_lock++;
    }

    ts_cpl_mutex_unlock((ts_cpl_mutex_s_t *)&sst_hwl_ctx.st_mutex);

    return ts_null;
}


ts_void ts_lib_hwlock_release(ts_void *pv_lock)
{
    ts_u32 u32_cur_lock, u32_idx;

    u32_cur_lock = (ts_u32)pv_lock;
    u32_idx = u32_cur_lock - sst_hwl_ctx.u32_regbase;
    u32_idx = u32_idx >> 2;

    ts_cpl_mutex_lock((ts_cpl_mutex_s_t *)&sst_hwl_ctx.st_mutex);
    sst_hwl_ctx.u32_locks &= ~(1 << u32_idx);
    ts_cpl_mutex_unlock((ts_cpl_mutex_s_t *)&sst_hwl_ctx.st_mutex);
}


ts_s32 ts_lib_hwlock_lock(ts_void *pv_lock, ts_u32 u32_tmout_ms)
{
    ts_u32 u32_lock;
    ts_u32 u32_now;

    u32_now = 0;
    while (1) {
        u32_lock = ts_cpl_readl(pv_lock);
        if (HWLOCK_IS_UNLOCK == u32_lock)
            return TS_OK;
        
        if (u32_now < u32_tmout_ms) {
            ts_cpl_msleep(10);
            u32_now += 10;
        } else {
            return TS_ERR;
        }
    }
}


ts_void ts_lib_hwlock_unlock(ts_void *pv_lock)
{
    ts_cpl_writel(HWLOCK_IS_UNLOCK, pv_lock);
}


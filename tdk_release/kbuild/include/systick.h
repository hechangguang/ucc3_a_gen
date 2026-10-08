/*
 * systick.h -- tsp 系统 tick ，用于时钟
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2022-2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __SYSTICK_H__
#define __SYSTICK_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <base/ts_type.h>

typedef struct systick_data_s {
    ts_u32 u32_lo;
    ts_u32 u32_hi;
} systick_data_s_t;


ts_void systick_stop(ts_void);
ts_void systick_start(ts_void);
ts_void systick_clean(ts_void);
ts_void systick_sip_clean(ts_void);
ts_void systick_sip_set(ts_void);
ts_void systick_set_hz(ts_u32 u32_hz);
ts_void systick_reload(ts_void);
ts_void systick_get_now(systick_data_s_t *pst_now);
ts_u64 systick_get_now64(ts_void);
ts_s32 systick_init(ts_void);

ts_void systick_delay_us(ts_u32 u32_us);
ts_u32 systick_ms2tick(ts_u32 u32_ms);
ts_u32 systick_tick2ms(ts_u32 u32_tick);


ts_void daytick_init(ts_void);
ts_void daytick_get_now(systick_data_s_t *pst_daytick);
ts_void daytick_stop(ts_void);
ts_void daytick_start(ts_void);
ts_u32 daytick_mktime(const ts_u32 year0, const ts_u32 mon0, const ts_u32 day, const ts_u32 hour,
            const ts_u32 min, const ts_u32 sec);

extern ts_u32 gu32_base_time;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __SYSTICK_H__ */

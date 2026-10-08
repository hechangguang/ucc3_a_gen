/* 
 * cpl_time.c -- cpl 时间相关实现头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __CPL_TIME_H__
#define __CPL_TIME_H__

#include <base/ts_type.h>

typedef struct ts_cpl_timeval_s {
	ts_u32 tv_sec;
	ts_u32 tv_usec;
} ts_cpl_timeval_s_t;

typedef struct ts_cpl_rtctime_s {
	ts_s32 tm_sec;
	ts_s32 tm_min;
	ts_s32 tm_hour;
	ts_s32 tm_mday;
	ts_s32 tm_mon;
	ts_s32 tm_year;
	ts_s32 tm_wday;
	ts_s32 tm_yday;
	ts_s32 tm_isdst;
} ts_cpl_rtctime_s_t;

extern ts_u32 ts_cpl_ms_to_tick(ts_u32 u32_msecs);
extern ts_u64 ts_cpl_ms_to_tick64(ts_u32 u32_msecs);
extern ts_u32 ts_cpl_tick_to_ms(ts_u32 u32_tick);
extern ts_u32 ts_cpl_tick64_to_ms(ts_u64 u64_tick);

/* 不能保证延时时间达到需要的时间 */
extern ts_void ts_cpl_msleep(ts_u32 u32_msecs);

/* 保证延时的时间大于等于需要的时间 */
extern ts_void ts_cpl_fix_msleep(ts_u32 u32_msecs);

extern ts_void ts_cpl_udelay(ts_u32 u32_usecs);
extern ts_void ts_cpl_mdelay(ts_u32 u32_msecs);

extern ts_u32 ts_cpl_get_tickcount(ts_void);
extern ts_u64 ts_cpl_sched_clock(ts_void);
extern ts_void ts_cpl_getjiffies(ts_u64 *pu64_pjiffies);
extern ts_void ts_cpl_getjiffies32(ts_u32 *pu32_pjiffies);
extern ts_s32 ts_cpl_time_before(ts_ulong ulg_time1, ts_ulong ulg_time2);

/* 普通通过 jiffies 延迟用 */
extern ts_u32 ts_cpl_get_now(ts_void);
extern ts_s32 ts_cpl_chk_timeout(ts_u32 u32_start, ts_u32 u32_time_out);

extern ts_void ts_cpl_gettimeofday(ts_cpl_timeval_s_t *pst_tv);
extern ts_u32 ts_cpl_get_timedif(ts_cpl_timeval_s_t *pst_start, ts_cpl_timeval_s_t *pst_end);

extern ts_void ts_cpl_rtc_time_to_tm(ts_ulong ulg_time, ts_cpl_rtctime_s_t *pv_tm);
extern ts_void ts_cpl_rtc_tm_to_time(ts_cpl_rtctime_s_t *pst_tm, ts_ulong *pulg_time);
extern ts_s32 ts_cpl_rtc_valid_tm(ts_cpl_rtctime_s_t *pst_tm);


#endif /* __CPL_TIME_H__ */

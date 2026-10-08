/* 
 * cpl_schedule.h -- cpl 调度实现头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __CPL_SCHEDULE_H__
#define __CPL_SCHEDULE_H__

#include <base/ts_type.h>

ts_void ts_cpl_yield(ts_void);
ts_void ts_cpl_cond_resched(ts_void);

#endif /* __CPL_SCHEDULE_H__ */

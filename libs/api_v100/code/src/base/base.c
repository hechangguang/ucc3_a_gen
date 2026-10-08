/* 
 * base.c -- OPEN TDK 基础功能实现
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#include <uapi/base_v100.h>

ts_s32 gs32_uapi_log_lvl = TS_LOG_WARNING; 

ts_u32 ts_lib_base_get_version(ts_void)
{
	return 100;
}

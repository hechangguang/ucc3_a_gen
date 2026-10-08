/* 
 * log_v100.h -- log v100 驱动接口头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __API_LOG_V100_H__
#define __API_LOG_V100_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <base/ts_type.h>
ts_s32 ts_lib_log2flash_init(ts_void);

ts_s32 ts_lib_log2flash_clear(ts_void);

ts_s32 ts_lib_log2flash_flush(ts_void);

ts_s32 ts_lib_log2flash_read(ts_void);

ts_s32 ts_lib_log2flash_write(ts_u08 *pu08_buf, ts_u32 u32_len);

ts_s32 ts_lib_log2flash_en(ts_u32 u32_en);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __API_LOG_V100_H__ */


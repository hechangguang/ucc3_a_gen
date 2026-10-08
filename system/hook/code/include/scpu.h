/*
 * scpu.h -- 从核头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __SCPU_H__
#define __SCPU_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */
#include <config.h>
#include <base/ts_type.h>


ts_void dbg_drv_init(ts_void);
ts_void dbg_set_baudrate(ts_u32 u32_clk, ts_u32 u32_baudrate);
ts_void dbg_put_char(ts_u08 u08_val);
ts_void dbg_put_str(ts_char *pc_str);

ts_s32 scpu_load(ts_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __SCPU_H__ */

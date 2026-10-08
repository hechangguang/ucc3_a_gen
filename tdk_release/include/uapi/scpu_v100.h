/* 
 * scpu_v100.h -- scpu v100 API 头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */


#ifndef __API_SCPU_V100_H__
#define __API_SCPU_V100_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <base/ts_type.h>
#include <base/ts_errno.h>
#include <base/ts_log.h>
#include <cpl/cpl_ioctl.h>

/*
 * 函数名 :  ts_lib_scpu_set_param
 * 描  述 :  设置scpu共享内存地址和长度
 * 输  入 :  参数                   描述
 *           u32_addr           共享内存地址
 *           u32_size           共享内存长度
 * 输  出 :  参数                   描述
 *           无                     无
 * 返  回 :  返回值                 描述
 *           无                     无
 */
ts_void ts_lib_scpu_set_param(ts_u32 u32_addr, ts_u32 u32_size);


/*
 * 函数名 :  scpu_start
 * 描  述 :  开启scpu
 * 输  入 :  参数           描述
 *           无             无
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          scpu开启成功
 *          TS_ERR          scpu开启失败
 */
ts_s32 ts_lib_scpu_start(ts_void);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __API_SCPU_V100_H__ */
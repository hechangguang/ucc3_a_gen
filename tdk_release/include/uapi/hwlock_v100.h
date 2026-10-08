/* 
 * hwlock_v100.h -- hwlock_v100 用户层硬件锁头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __UAPI_HWLOCK_V100_H__
#define __UAPI_HWLOCK_V100_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <base/ts_type.h>

/* 
 * 函数名 :  ts_lib_hwlock_init
 * 描  述 :  用户层硬件锁初始化
 * 输  入 :  参数               描述
 *           u32_regbase        硬件锁寄存器基地址
 *           u32_max_num        最大锁个数
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           TS_OK              初始化成功
 *           TS_ERR             初始化失败
 */
ts_s32 ts_lib_hwlock_init(ts_u32 u32_regbase, ts_u32 u32_max_num);


/* 
 * 函数名 :  ts_lib_hwlock_deinit
 * 描  述 :  用户层硬件锁去初始化
 * 输  入 :  参数               描述
 *           无                 无
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           无                 无
 */
ts_void ts_lib_hwlock_deinit(ts_void);


/* 
 * 函数名 :  ts_lib_hwlock_get_free
 * 描  述 :  申请一个锁
 * 输  入 :  参数               描述
 *           无                 无
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           ts_null            获取失败
 *           非 ts_null         获取成功
 */
ts_void *ts_lib_hwlock_get_free(ts_void);


/* 
 * 函数名 :  ts_lib_hwlock_release
 * 描  述 :  释放一个锁
 * 输  入 :  参数               描述
 *           pv_lock            硬件锁句柄
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           无                 无
 */
ts_void ts_lib_hwlock_release(ts_void *pv_lock);


/* 
 * 函数名 :  ts_lib_hwlock_lock
 * 描  述 :  加锁
 * 输  入 :  参数               描述
 *           pv_lock           硬件锁句柄
 *           u32_tmout_ms      尝试加锁超时
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           TS_OK              加锁成功
 *           TS_ERR             加锁失败
 */
ts_s32 ts_lib_hwlock_lock(ts_void *pv_lock, ts_u32 u32_tmout_ms);


/* 
 * 函数名 :  ts_lib_hwlock_unlock
 * 描  述 :  去锁
 * 输  入 :  参数               描述
 *           pv_lock           硬件锁句柄
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           无                 无
 */
ts_void ts_lib_hwlock_unlock(ts_void *pv_lock);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __UAPI_HWLOCK_V100_H__ */


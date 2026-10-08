/* 
 * timer_v100.h -- timer_v100 上层用户调用头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __API_TIMER_V100_H__
#define __API_TIMER_V100_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <base/ts_type.h>
#include <base/ts_errno.h>
#include <base/ts_log.h>

#include <cpl/cpl_ioctl.h>

/* 定时器结束定时回调函数定义 */
typedef ts_void (*ts_hwt_cb_fp_t)(ts_void *);

/* 
 * 函数名 :  ts_lib_timer_get_free
 * 描  述 :  获取空闲子设备句柄
 * 输  入 :  参数               描述 
 *           无                 无
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           TS_EBUSY           获取失败，暂无空闲设备
 *           >= 0               获取成功，返回空闲子设备句柄
 */
extern ts_s32 ts_lib_timer_get_free(void);

/* 
 * 函数名 :  ts_lib_timer_release
 * 描  述 :  释放占用的子设备
 * 输  入 :  参数               描述 
 *           s32_handle         子设备句柄
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           TS_OK              释放成功
 *           TS_ENOINIT         定时器未初始化
 *           TS_EINVAL          参数错误
 */
extern ts_s32 ts_lib_timer_release(ts_s32 s32_handle);

/* 
 * 函数名 :  ts_lib_timer_stop
 * 描  述 :  定时器停止定时
 * 输  入 :  参数               描述 
 *           s32_handle         子设备句柄
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           TS_OK              释放成功
 *           TS_ENOINIT         定时器未初始化
 *           TS_EINVAL          参数错误
 */
extern ts_s32 ts_lib_timer_stop(ts_s32 s32_handle);

/* 
 * 函数名 :  ts_lib_timer_start
 * 描  述 :  启动定时器
 * 输  入 :  参数               描述
 *           s32_handle         子设备句柄
 *           u32_dly_us         延时时间,单位us
 *           fp_cb              定时器结束后回调
 *           pv_cb_param        回调函数参数
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           TS_OK              启动成功
 *           TS_ERR             失败
 *           TS_ETIMEDOUT       超时
 *           TS_ENOINIT         未初始化
 *           TS_EINVAL          参数错误
 */
extern ts_s32 ts_lib_timer_start(ts_s32 s32_handle, ts_u32 u32_dly_us);


/* 
 * 函数名 :  ts_lib_timer_set_cb
 * 描  述 :  定时器设置回调
 * 输  入 :  参数           描述
 *           s32_handle      子设备句柄
 *           fp_cb           定时器结束后回调
 *           pv_param        回调函数参数
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          成功
 *           TS_ENOINIT     未初始化
 *           TS_EINVAL      参数错误
 */
extern ts_s32 ts_lib_timer_set_callback(ts_s32 s32_handle, ts_hwt_cb_fp_t fp_cb, ts_void *pv_param);

/* 
 * 函数名 :  ts_lib_timer_deinit
 * 描  述 :  timer 库去初始化
 * 输  入 :  参数           描述
 *           无             无
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          去初始化成功
 */
extern ts_s32 ts_lib_timer_deinit(ts_void);


/* 
 * 函数名 :  ts_lib_timer_init
 * 描  述 :  初始化 TIMER 库
 * 输  入 :  参数           描述
 *           无             无
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          初始化成功
 *           TS_ERR        初始化失败
 */
extern ts_s32 ts_lib_timer_init(ts_void);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __API_TIMER_V100_H__ */

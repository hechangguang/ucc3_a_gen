/* 
 * adc_v100.h -- adc v100 API 头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2025 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __UAPI_ADC_V100_H__
#define __UAPI_ADC_V100_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <base/ts_type.h>
#include <base/ts_errno.h>
#include <base/ts_log.h>

#define TDK_ADC_MAX_NUM         (4)


/* 
 * 函数名 :  ts_lib_adc_init
 * 描  述 :  初始化 ADC 库
 * 输  入 :  参数           描述
 *          u32_sample_clk  采样时钟，最大 12MHZ
 *          u32_refv         参考电压，放大 100 倍，即 3.3v=330,1.8v=180
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          初始化成功
 *           TS_ERR         初始化失败
 */
ts_s32 ts_lib_adc_init(ts_u32 u32_sample_clk, ts_u32 u32_refv);


/* 
 * 函数名 :  ts_lib_adc_deinit
 * 描  述 :  ADC 库去初始化
 * 输  入 :  参数           描述
 *           无             无
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          去初始化成功
 */
ts_s32 ts_lib_adc_deinit(ts_void);


/* 
 * 函数名 :  ts_lib_adc_read
 * 描  述 :  读取一次 ADC 采样值
 * 输  入 :  参数           描述
 *          u32_ch_num      通道号
 *          u32_sample_cnt  采样次数，如果此配置大于 1， 则会进行多次采样，取均值。
 *          u32_dly_ms      多次采样模式，每次采样的间隔时间，单位为 ms，内部采用 ts_cpl_msleep 实现延时
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           -TS_ENOINIT    ADC 库未初始化
 *          -TS_EINVALCHNID 错误通道 ID 号
 *          TS_ERR          采样错误
 *           >= 0           采样值
 */
ts_s32 ts_lib_adc_read(ts_u32 u32_ch_num, ts_u32 u32_sample_cnt, ts_u32 u32_dly_ms);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __UAPI_ADC_V100_H__ */


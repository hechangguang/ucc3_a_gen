/* 
 * wdt_v100.h -- wdt v100 API 头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __API_WDT_V100_H__
#define __API_WDT_V100_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <base/ts_type.h>

/* 中断回调函数定义 */
typedef ts_void (*ts_wdt_irqcb_fp_t) (ts_void *);

/* wdt 工作模式 */
typedef enum ts_wdt_mode_en {
    /* 第一次喂狗超时即产生全局复位 */
    TS_WDT_MODE_RST_AT_ONCE = 0,

    /* 第一次喂狗超时只产生中断，第二次喂狗超时产生全局复位 */
    TS_WDT_MODE_RST_FOR_SECOND,

    TS_WDT_MODE_BUTT
} ts_wdt_mode_en_t;

/* 看门狗回调属性 */
typedef struct ts_wdt_irqcb_attr_s {
    /* wdt 中断回调函数 */
    ts_wdt_irqcb_fp_t fp_irqcb;

    /* wdt 中断回调函数的私有参数 */
    void *pv_priv;
} ts_wdt_irqcb_attr_s_t;


/* 复位状态 */
#define TS_WDT_POR          (0)
#define TS_WDT_RESET        (1)

/* --- function list --- */

/* 
 * 函数名 :  ts_lib_wdt_set_timeout
 * 描  述 :  重新设置 wdt 计数器的值,不带关闭和启动看门狗动作
 * 输  入 :  参数           描述
 *          u32_timeout_ms 计数时间(单位 ms,最小值为 200 ms, 最大为 178000 ms)
 *          u32_restart    0: 不重启看门狗;  >0 重启看门狗
 * 输  出 :  参数           描述
 *          无             无
 * 返  回 :  返回值         描述
 *          TS_OK          设置成功
 */
extern ts_s32 ts_lib_wdt_set_timeout(ts_u32 u32_timeout_ms, ts_u32 u32_restart);


/* 
 * 函数名 :  ts_lib_wdt_set_mode
 * 描  述 :  设置 wdt 的工作模式
 * 输  入 :  参数           描述
 *          en_mode        wdt 的工作模式
 * 输  出 :  参数           描述
 *          无             无
 * 返  回 :  返回值         描述
 *          -TS_ENOTSUP     此版本不支持
 */
extern ts_s32 ts_lib_wdt_set_mode(ts_wdt_mode_en_t en_mode);


/* 
 * 函数名 :  ts_lib_wdt_feeding
 * 描  述 :  刷新 wdt (喂狗)
 * 输  入 :  参数           描述
 *          s32_handle     设备句柄
 * 输  出 :  参数           描述
 *          无             无
 * 返  回 :  返回值         描述
 *          TS_OK          喂狗成功
 */
extern ts_s32 ts_lib_wdt_feeding(ts_void);


/* 
 * 函数名 :  ts_lib_wdt_set_callback
 * 描  述 :  设置 wdt 的中断回调函数(只支持 RTOS 使用)
 * 输  入 :  参数           描述
 *          fp_irqcb       wdt 的中断回调函数
 *          pv_priv        中断回调函数的私有参数
 * 输  出 :  参数           描述
 *          无             无
 * 返  回 :  返回值         描述
 *          -TS_ENOTSUP     此版本不支持
 */
extern ts_s32 ts_lib_wdt_set_callback(ts_wdt_irqcb_fp_t fp_irqcb, void *pv_priv);


/* 
 * 函数名 :  ts_lib_wdt_get_reset_status
 * 描  述 :  获取此次复位是否由看门狗导致
 * 输  入 :  参数           描述
 *          无             无
 * 输  出 :  参数           描述
 *          无             无
 * 返  回 :  返回值         描述
 *          TS_WDT_POR      由 POR 或者复位管脚导致复位
 *          TS_WDT_RESET    由看门狗导致复位
 */
extern ts_s32 ts_lib_wdt_get_reset_status(ts_void);


/* 
 * 函数名 :  ts_lib_wdt_start
 * 描  述 :  启动 wdt 计数器
 * 输  入 :  参数           描述
 *          u32_timeout_ms 计数时间(单位 ms,最小值为 100 ms, 最大为 178000 ms)
 * 输  出 :  参数           描述
 *          无             无
 * 返  回 :  返回值         描述
 *          TS_OK          启动成功
 */
extern ts_s32 ts_lib_wdt_start(ts_u32 u32_timeout_ms);


/* 
 * 函数名 :  ts_lib_wdt_stop
 * 描  述 :  停止 wdt 计数器
 * 输  入 :  参数           描述
 *          无             无
 * 输  出 :  参数           描述
 *          无             无
 * 返  回 :  返回值         描述
 *          TS_OK          停止成功
 */
extern ts_s32 ts_lib_wdt_stop(ts_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __API_WDT_V100_H__ */

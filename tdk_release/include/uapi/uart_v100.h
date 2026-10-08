/*
 * uart_v100.h -- tdk uart 驱动头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2024 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __UART_V100_H__
#define __UART_V100_H__

#include <config.h>
#include <base/ts_type.h>
#include <cpl/ts_cpl.h>

typedef ts_bool (*ts_hbp_irqcb_fp_t)(ts_u08);

/* 协议解析属性配置 */
typedef struct uart_proto_s {
    /* 协议标志，默认为 0xA5, HIK 0xF0 */
    ts_u32 u32_flg;

    /* 是否使能协议解析 */
    ts_u32 u32_en;

    /* 协议解析回调函数接口 */
    ts_hbp_irqcb_fp_t fp_cb;
} uart_proto_s_t;


/* 1 个停止位 */
#define UART_STOP_1BIT      (0)

/* 1.5 个停止位，仅在数据位为 5bit 的情况下生效 */
#define UART_STOP_1_5BIT    (1)

/* 2 个停止位 */
#define UART_STOP_2BIT      (2)

/* 无校验 */
#define UART_PARITY_NONE    (0)

/* 奇数校验 */
#define UART_PARITY_ODD     (1)

/* 偶数校验 */
#define UART_PARITY_EVEN    (2)


/* 串口属性配置 */
typedef struct uart_attr_s {
    /* 波特率 */
    ts_u32 u32_baudr;

    /* 数据位, 5..8 */
    ts_u08 u08_data_bit;

    /* 停止位,可配置范围: UART_STOP_1BIT, UART_STOP_1_5BIT, UART_STOP_2BIT */
    ts_u08 u08_stop_bit;

    /* 校验位类型,可配置范围: UART_PARITY_NONE, UART_PARITY_ODD, UART_PARITY_EVEN */
    ts_u08 u08_parity_type;

    /* 保留 */
    ts_u08 u08_rev;
} uart_attr_s_t;


/* 
 * 函数名 :  ts_lib_uart_hbp_get_attr
 * 描  述 :  获取串口hbp解析功能属性
 * 输  入 :  参数           描述
 *           s32_hdev       设备句柄
 * 输  出 :  参数           描述
 *           pst_hbp_attr    获取到的属性
 * 返  回 :  返回值         描述
 *           TS_OK          读取成功
 *           TS_ENOTSUP     命令不支持
 *           TS_EINVAL      参数错误
 *           TS_ERR         其他错误
 */
extern ts_s32 ts_lib_uart_hbp_get_attr(ts_s32 s32_hdev, uart_proto_s_t *pst_hbp_attr);


/* 
 * 函数名 :  ts_lib_uart_hikp_get_attr
 * 描  述 :  获取串口hik proto解析功能属性
 * 输  入 :  参数           描述
 *           s32_hdev       设备句柄
 * 输  出 :  参数           描述
 *           pst_hikp_attr    获取到的属性
 * 返  回 :  返回值         描述
 *           TS_OK          读取成功
 *           TS_ENOTSUP     命令不支持
 *           TS_EINVAL      参数错误
 *           TS_ERR         其他错误
 */
extern ts_s32 ts_lib_uart_hikp_get_attr(ts_s32 s32_hdev, uart_proto_s_t *pst_hikp_attr);


/* 
 * 函数名 :  ts_lib_uart_hbp_set_attr
 * 描  述 :  设置串口hbp解析功能属性
 * 输  入 :  参数           描述
*           s32_hdev       设备句柄
 *          fp_cb           hbp解析回调函数
 *          u32_flg         帧头
 *          u32_en          hbp解析功能使能，1使能，0关闭
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          读取成功
 *           TS_ENOTSUP     命令不支持
 *           TS_EINVAL      参数错误
 *           TS_ERR         其他错误
 */
extern ts_s32 ts_lib_uart_hbp_set_attr(ts_s32 s32_hdev, ts_hbp_irqcb_fp_t fp_cb, ts_u32 u32_flg, ts_u32 u32_en);


/* 
 * 函数名 :  ts_lib_uart_hikp_set_attr
 * 描  述 :  设置串口hik proto解析功能属性
 * 输  入 :  参数           描述
*           s32_hdev       设备句柄
 *          fp_cb           hik proto解析回调函数
 *          u32_flg         帧头
 *          u32_en          hik proto解析功能使能，1使能，0关闭
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          读取成功
 *           TS_ENOTSUP     命令不支持
 *           TS_EINVAL      参数错误
 *           TS_ERR         其他错误
 */
extern ts_s32 ts_lib_uart_hikp_set_attr(ts_s32 s32_hdev, ts_hbp_irqcb_fp_t fp_cb, ts_u32 u32_flg, ts_u32 u32_en);


/* 
 * 函数名 :  ts_lib_uart_hikp_task
 * 描  述 :  开启 hik 协议守护线程
 * 输  入 :  参数           描述
 *          s32_hdev        hikp 对接串口设备句柄
 *          u32_prio        线程优先级
 *          u32_stack_size  线程栈
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          启动成功
 *           TS_EINVAL      无效入参
 */
ts_s32 ts_lib_uart_hikp_task(ts_s32 s32_hdev, ts_u32 u32_prio, ts_u32 u32_stack_size);



/* 
 * 函数名 :  ts_lib_uart_hikp_en
 * 描  述 :  hik协议解析使能设置
 * 输  入 :  参数           描述
 *          s32_hdev        hikp 对接串口设备句柄
 *			u32_en			使能
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK         	设置成功
 *           TS_ERR         设置失败
 */
ts_s32 ts_lib_uart_hikp_en(ts_s32 s32_hdev, ts_u32 u32_en);


/* 
 * 函数名 :  ts_lib_uart_set_buadr
 * 描  述 :  设置串口的波特率
 * 输  入 :  参数           描述
 *          s32_hdev        串口设备句柄
 *          u32_baudrate    波特率，范围从 1200 到 7500000
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          设置成功
 *           -TS_EINVAL     无效入参
 */
ts_s32 ts_lib_uart_set_buadr(ts_s32 s32_hdev, ts_u32 u32_baudrate);


/* 
 * 函数名 :  ts_lib_uart_get_attr
 * 描  述 :  获取串口当前属性
 * 输  入 :  参数           描述
 *          s32_hdev        串口设备句柄
 *          pst_attr        属性缓存，由用户申请
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          获取成功
 *           -TS_EINVAL     无效入参
 */
ts_s32 ts_lib_uart_get_attr(ts_s32 s32_hdev, uart_attr_s_t *pst_attr);


/* 
 * 函数名 :  ts_lib_uart_set_attr
 * 描  述 :  设置串口属性
 * 输  入 :  参数           描述
 *          s32_hdev        串口设备句柄
 *          pst_attr        待设置的串口属性句柄
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          设置成功
 *           -TS_EINVAL     无效入参
 */
ts_s32 ts_lib_uart_set_attr(ts_s32 s32_hdev, const uart_attr_s_t *pst_attr);


#endif /* __UART_V100_H__ */

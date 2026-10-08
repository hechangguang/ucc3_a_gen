/*
 * uart_v100.h -- tdk uart 驱动头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2024 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __K_UART_V100_H__
#define __K_UART_V100_H__

#include <config.h>
#include <base/ts_type.h>
#include <kapi/k_console_v100.h>
#include <uapi/uart_v100.h>
#include <cpl/ts_cpl.h>

typedef struct uart_dev_s uart_dev_s_t;

/* ioctl */
typedef enum ts_uart_ioctl_en {
    IOC_NR_UART_HBP_SET_ATTR = 0,
    IOC_NR_UART_HBP_GET_ATTR = 1,
    IOC_NR_UART_HBP_SET_EN = 2,

    IOC_NR_UART_HIK_SET_ATTR = 3,
    IOC_NR_UART_HIK_GET_ATTR = 4,
    IOC_NR_UART_HIK_SET_EN = 5,

    /* 设置波特率 */
    IOC_NR_UART_SET_BAUDR = 0x10,

    /* 获取和设置串口属性 */
    IOC_NR_UART_GET_ATTR = 0x11,
    IOC_NR_UART_SET_ATTR = 0x12,

    IOC_NR_GPIO_BUTT,
} ts_uart_ioctl_en_t;

#define TS_IOCTRL_IOC_NR_UART_SET_HBP_ATTR              TS_CPL_IOW(TS_IOC_MAGIC_UART, IOC_NR_UART_HBP_SET_ATTR, uart_proto_s_t)
#define TS_IOCTRL_IOC_NR_UART_GET_HBP_ATTR              TS_CPL_IOR(TS_IOC_MAGIC_UART, IOC_NR_UART_HBP_GET_ATTR, uart_proto_s_t)
#define TS_IOCTRL_IOC_NR_UART_SET_HBP_EN                TS_CPL_IOW(TS_IOC_MAGIC_UART, IOC_NR_UART_HBP_SET_EN, ts_u32)
#define TS_IOCTRL_IOC_NR_UART_SET_HIK_ATTR              TS_CPL_IOW(TS_IOC_MAGIC_UART, IOC_NR_UART_HIK_SET_ATTR, uart_proto_s_t)
#define TS_IOCTRL_IOC_NR_UART_GET_HIK_ATTR              TS_CPL_IOR(TS_IOC_MAGIC_UART, IOC_NR_UART_HIK_GET_ATTR, uart_proto_s_t)
#define TS_IOCTRL_IOC_NR_UART_SET_HIK_EN                TS_CPL_IOW(TS_IOC_MAGIC_UART, IOC_NR_UART_HIK_SET_EN, ts_u32)

#define TS_IOCTRL_IOC_NR_UART_SET_BAUDR             TS_CPL_IOW(TS_IOC_MAGIC_UART, IOC_NR_UART_SET_BAUDR, ts_u32)

#define TS_IOCTRL_IOC_NR_UART_GET_DEV_ATTR          TS_CPL_IOW(TS_IOC_MAGIC_UART, IOC_NR_UART_GET_ATTR, ts_u32)
#define TS_IOCTRL_IOC_NR_UART_SET_DEV_ATTR          TS_CPL_IOW(TS_IOC_MAGIC_UART, IOC_NR_UART_SET_ATTR, ts_u32)

/* UART 缓冲 */
typedef struct uart_buf_s {
    /* pc_buf 和 u32_szie 有一个为 0， 则不使用缓冲管理 */
    ts_char *pc_buf;
    ts_u32 u32_szie;

    /* 缓冲同步信号,使用定时休眠来替代同步信号 */
    //ts_cpl_semphr_s_t st_sema;

    /* 缓冲读写索引 */
    volatile ts_u32 u32_wr_idx;
    volatile ts_u32 u32_rd_idx;
} uart_buf_s_t;


typedef struct uart_ops_s {
    /* 串口设置 */
    ts_void (*setup)(uart_dev_s_t *);

    /* 关闭串口，但不释放资源 */
    ts_void (*close)(uart_dev_s_t *);

    /* 发送一个字符 */
    ts_s32 (*putc)(uart_dev_s_t *, ts_u08);

    /* 接收一个字符 */
    ts_u08 (*getc)(uart_dev_s_t *);

    /* 检测是否接收到至少一个字符 */
    ts_u32 (*chkc)(uart_dev_s_t *);

    /* 设置传输速度，即波特率 */
    ts_s32 (*set_speed)(uart_dev_s_t *, ts_u32);

    /* 中断接口 */
    ts_cpl_irqreturn_en_t (*hirq)(uart_dev_s_t *);
} uart_ops_s_t;


typedef struct uart_dev_s {
    /* UART 设备属性 */
    ts_s32 s32_inited;
    ts_u32 u32_regbase;
    ts_s32 s32_irq_no;

    /* 波特率 */
    ts_u32 u32_speed;

    /* 接收超时时间, 单位微秒 */
    ts_u32 u32_rx_tmout_ms;

    /* 发送直接输出，接收使用缓冲 */
    uart_buf_s_t st_rx_buf;

    /* 串口驱动内部接口 */
    uart_ops_s_t *pst_ops;

    /* hbp协议*/
    uart_proto_s_t st_hbp;

    /* hik协议 */
    uart_proto_s_t st_hikp;

    /* 内部流程控制 */
    ts_u32 u32_proto_step;

    /* 保存属性配置 */
    uart_attr_s_t st_attr;
} uart_dev_s_t;


ts_s32 tdl_uart_init(uart_dev_s_t *pst_uart_dev, ts_u32 u32_rx_size);
ts_s32 uart_set_attr(uart_dev_s_t *pst_dev, uart_attr_s_t *pst_attr);


#endif /* __K_UART_V100_H__ */


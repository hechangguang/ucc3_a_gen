/* 
 * uart.c -- tg uart 驱动实现
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023-2025 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#include <config.h>
#include <base/ts_type.h>
#include <base/ts_comm.h>
#include <cpl/ts_cpl.h>
#include <kapi/k_console_v100.h>
#include <kapi/k_uart_v100.h>

#include "uart.h"

#define UART_MAX_LOOPS              (32)
#define UART_MAX_TMOUT              (0x200000)

static ts_void uart_set_divisor(uart_dev_s_t *pst_dev, ts_u32 u32_clk, ts_u32 u32_baudrate)
{
    uart_reg_s_t *pst_reg;
    ts_u32 u32_div, u32_decimal;
    lcr_un_t un_lcr;
    dll_un_t un_dll;
    dlh_un_t un_dlh;

    pst_reg = (uart_reg_s_t *)pst_dev->u32_regbase;

    u32_div = (u32_clk << 1) / (u32_baudrate << 4);
    u32_decimal = u32_div & 0x01;
    u32_div = u32_div >> 1;
    u32_div += u32_decimal;

    /* DLAB = 1 */
    un_lcr.u32_val = pst_reg->un_lcr.u32_val;
    un_lcr.dlab = 1;
    pst_reg->un_lcr.u32_val = un_lcr.u32_val;

    un_dll.u32_val = 0;
    un_dll.dll = u32_div & 0xFF;
    pst_reg->un_dll.u32_val = un_dll.u32_val;

    un_dlh.u32_val = 0;
    un_dlh.dlh = (u32_div >> 8) & 0xFF;
    pst_reg->un_dlh.u32_val = un_dlh.u32_val;

    /* DLAB = 0 */
    un_lcr.dlab = 0;
    pst_reg->un_lcr.u32_val = un_lcr.u32_val;
}


static ts_s32 uart_putc(uart_dev_s_t *pst_dev, ts_u08 u08_char)
{
    ts_u32 i;
    uart_reg_s_t *pst_reg;
    usr_un_t un_usr;
    thr_un_t un_thr;

    pst_reg = (uart_reg_s_t *)pst_dev->u32_regbase;
    i = 0;
    while (1) {
        un_usr.u32_val = pst_reg->un_usr.u32_val;
        if (un_usr.tfnf > 0)
            break;
        
        if (i >= UART_MAX_TMOUT)
            return -TS_ETIMEDOUT;

        i++;
    }

    un_thr.u32_val = 0;
    un_thr.thr = u08_char;
    pst_reg->un_thr.u32_val = un_thr.u32_val;

    return TS_OK;
}


static ts_u08 uart_getc(uart_dev_s_t *pst_dev)
{
    uart_buf_s_t *pst_buf;
    ts_u08 u08_val;

    pst_buf = (uart_buf_s_t *)&pst_dev->st_rx_buf;

    /* 仅供单线程使用，不锁, 直到获取数据后退出, 可配合 uart_chkc 使用进行超时退出 */
    while (1) {
        if (pst_buf->u32_rd_idx != pst_buf->u32_wr_idx)
            break;

        ts_cpl_msleep(10);
    }

    u08_val = (ts_u08)pst_buf->pc_buf[pst_buf->u32_rd_idx];
    pst_buf->u32_rd_idx++;
    if (pst_buf->u32_rd_idx >= pst_buf->u32_szie)
        pst_buf->u32_rd_idx = 0;

    return u08_val;
}


static ts_u32 uart_chkc(uart_dev_s_t *pst_dev)
{
    uart_buf_s_t *pst_buf;

    pst_buf = (uart_buf_s_t *)&pst_dev->st_rx_buf;
    if (pst_buf->u32_rd_idx != pst_buf->u32_wr_idx)
        return 1;
    else
        return 0;
}


/* 设置波特率 */
static ts_s32 uart_set_speed(uart_dev_s_t *pst_dev, ts_u32 u32_speed)
{
    uart_set_divisor(pst_dev, 24000000, u32_speed);

    pst_dev->st_attr.u32_baudr = u32_speed;

    return TS_OK;
}


ts_s32 uart_set_attr(uart_dev_s_t *pst_dev, uart_attr_s_t *pst_attr)
{
    uart_reg_s_t *pst_reg;
    lcr_un_t un_lcr;

    if (pst_dev->st_attr.u32_baudr != pst_attr->u32_baudr)
        uart_set_speed(pst_dev, pst_attr->u32_baudr);

    pst_reg = (uart_reg_s_t *)pst_dev->u32_regbase;
    un_lcr.u32_val = 0;
    un_lcr.wls = pst_attr->u08_data_bit - 5;
    if (UART_STOP_1BIT == pst_attr->u08_stop_bit)
        un_lcr.stb = 0;
    else
        un_lcr.stb = 1;
    
    if (pst_attr->u08_parity_type > UART_PARITY_NONE) {
        un_lcr.pen = 1;
        if (UART_PARITY_ODD == pst_attr->u08_parity_type)
            un_lcr.eps = 0;
        else
            un_lcr.eps = 1;
    } else {
        un_lcr.pen = 0;
    }

    pst_reg->un_lcr.u32_val = un_lcr.u32_val;

    pst_dev->st_attr.u08_data_bit = pst_attr->u08_data_bit;
    pst_dev->st_attr.u08_stop_bit = pst_attr->u08_stop_bit;
    pst_dev->st_attr.u08_parity_type = pst_attr->u08_parity_type;

    return TS_OK;
}


static ts_void uart_setup(uart_dev_s_t *pst_dev)
{
    uart_reg_s_t *pst_reg;
    fcr_un_t un_fcr;
    lcr_un_t un_lcr;
    ier_un_t un_ier;

    pst_reg = (uart_reg_s_t *)pst_dev->u32_regbase;
    if (0 == pst_dev->s32_inited) {
        un_lcr.u32_val = 0;
        un_lcr.wls = UART_LCR_WLS_8BIT;
        pst_reg->un_lcr.u32_val = un_lcr.u32_val;

        pst_reg->un_ier.u32_val = 0;

        uart_set_divisor(pst_dev, 24000000, pst_dev->u32_speed);

        /* Clear fifos */
        un_fcr.u32_val = 0;
        un_fcr.rx_rst = 1;
        un_fcr.tx_rst = 1;
        un_fcr.fifo_en = 1;
        un_fcr.rx_trigger = UART_FCR_RXTRIGGER_1;
        pst_reg->un_fcr.u32_val = un_fcr.u32_val;

        /* 申请内存 */
        pst_dev->st_rx_buf.pc_buf = ts_cpl_kmalloc(pst_dev->st_rx_buf.u32_szie, TS_CPL_GFP_KERNEL);
        pst_dev->st_rx_buf.u32_rd_idx = 0;
        pst_dev->st_rx_buf.u32_wr_idx = 0;

        pst_dev->s32_inited = 1;
    }

    /* 开启发送接收中断 */
    un_ier.u32_val = 0;
    if ((pst_dev->st_rx_buf.pc_buf != ts_null) || (pst_dev->st_rx_buf.u32_szie > 4))
        un_ier.erbfi = 1;

    pst_reg->un_ier.u32_val = un_ier.u32_val;
}

static ts_void uart_close(uart_dev_s_t *pst_dev)
{
    uart_reg_s_t *pst_reg;

    /* 关闭中断 */
    pst_reg = (uart_reg_s_t *)pst_dev->u32_regbase;
    pst_reg->un_ier.u32_val = 0;

    /* 锁中断? */
    pst_dev->st_rx_buf.u32_rd_idx = 0;
    pst_dev->st_rx_buf.u32_wr_idx = 0;
}


static ts_cpl_irqreturn_en_t uart_hirq(uart_dev_s_t *pst_dev)
{
    ts_u32 i;
    uart_proto_s_t *pst_hbp;
    uart_proto_s_t *pst_hikp;
    uart_reg_s_t *pst_reg;
    iir_un_t un_iir;
    rbr_un_t un_rbr;
    uart_buf_s_t *pst_buf;
    ts_bool b_ret;

    pst_hbp = (uart_proto_s_t *)&pst_dev->st_hbp;
    pst_hikp = (uart_proto_s_t *)&pst_dev->st_hikp;
    pst_reg = (uart_reg_s_t *)pst_dev->u32_regbase;
    pst_buf = (uart_buf_s_t *)&pst_dev->st_rx_buf;

    for (i = 0; i < UART_MAX_LOOPS; i++) {
        un_iir.u32_val = pst_reg->un_iir.u32_val;
        if (UART_IIR_RECV_DATA == un_iir.intrpt_id) {
            /* rx */
            un_rbr.u32_val = pst_reg->un_rbr.u32_val;
            if (((ts_null == pst_hbp->fp_cb) || (0 == pst_hbp->u32_en)) && \
                ((ts_null == pst_hikp->fp_cb) || (0 == pst_hikp->u32_en))) {
                goto _normal_rev;
            } else {
                if (1 == pst_dev->u32_proto_step) {
                    goto _hbp_rev;
                } else if (2 == pst_dev->u32_proto_step) {
                    goto _hik_rev;
                } else {
                    if (un_rbr.rbr == pst_hbp->u32_flg)  {
                        pst_dev->u32_proto_step = 1;

                        goto _hbp_rev;
                    } else if (un_rbr.rbr == pst_hikp->u32_flg) {
                        pst_dev->u32_proto_step = 2;

                        goto _hik_rev;
                    } else {
                        goto _normal_rev;
                    }
                }
            }

_normal_rev:
            pst_buf->pc_buf[pst_buf->u32_wr_idx] = (ts_char)un_rbr.rbr;
            pst_buf->u32_wr_idx++;
            if (pst_buf->u32_wr_idx >= pst_buf->u32_szie)
                pst_buf->u32_wr_idx = 0;

            continue;
_hbp_rev:
            b_ret = pst_hbp->fp_cb(un_rbr.rbr);
            if (ts_true == b_ret)
                pst_dev->u32_proto_step = 0;

            continue;
_hik_rev:
            b_ret = pst_hikp->fp_cb(un_rbr.rbr);
            if (ts_true == b_ret)
                pst_dev->u32_proto_step = 0;

            continue;
        } else if (UART_IIR_THR_EMPTY == un_iir.intrpt_id) {
            /* tx */
        } else {
            break;
        }
    }

    return TS_CPL_IRQ_HANDLED;
}



static uart_ops_s_t sst_ops = {
    .setup = uart_setup,
    .close = uart_close,
    .putc = uart_putc,
    .getc = uart_getc,
    .chkc = uart_chkc,
    .set_speed = uart_set_speed,
    .hirq = uart_hirq
};

ts_s32 tdl_uart_init(uart_dev_s_t *pst_uart_dev, ts_u32 u32_rx_size)
{
    pst_uart_dev->s32_inited = 0;
    pst_uart_dev->u32_speed = 115200;
    pst_uart_dev->st_rx_buf.u32_szie = u32_rx_size;

    pst_uart_dev->pst_ops = (uart_ops_s_t *)&sst_ops;

    pst_uart_dev->st_hbp.fp_cb = ts_null;
    pst_uart_dev->st_hbp.u32_en = 0;
    pst_uart_dev->st_hbp.u32_flg = 0xA5;

    pst_uart_dev->st_hikp.fp_cb = ts_null;
    pst_uart_dev->st_hikp.u32_en = 0;
    pst_uart_dev->st_hikp.u32_flg = 0xF0;

    pst_uart_dev->u32_proto_step = 0;

    /* 默认配置 */
    pst_uart_dev->st_attr.u32_baudr = 115200;
    pst_uart_dev->st_attr.u08_data_bit = 8;
    pst_uart_dev->st_attr.u08_stop_bit = 1;
    pst_uart_dev->st_attr.u08_parity_type = UART_PARITY_NONE;
    pst_uart_dev->st_attr.u08_rev = 0;

    return TS_OK;
}


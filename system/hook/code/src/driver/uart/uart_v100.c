/*
 * uart_v100.c -- uart tg v100 串口实现代码
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2025 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#include <config.h>
#include <base/ts_type.h>
#include <base/ts_errno.h>
#include <io.h>
#include <bootup.h>
#include <bootup_api.h>
#include "uart_v100.h"

static ts_u32 uart_set_baudrate(ts_u32 u32_regbase, ts_u32 u32_baudrate)
{
    ts_u32 u32_baudrate_div, u32_val, u32_clk;

    u32_clk = soc_get_osc_clk();

    /* 计算分频 */
    u32_baudrate_div = (u32_clk << 1) / (u32_baudrate << 4);
    u32_val = u32_baudrate_div & 0x01;
    u32_baudrate_div = u32_baudrate_div >> 1;
    u32_baudrate_div += u32_val;
    if ((u32_baudrate_div > UART_MAX_DL) || (0 == u32_baudrate_div))
         return 0;

    u32_val = readl(u32_regbase + UART_REG_OFFSET_LCR);
    u32_val |= UART_ENABLE_DIVISOR | UART_ENABLE_8BIT;
    writel(u32_val, (u32_regbase + UART_REG_OFFSET_LCR));

    writel((u32_baudrate_div & 0xFF), (u32_regbase + UART_REG_OFFSET_DLL));
    writel(((u32_baudrate_div >> 8) & 0xFF), (u32_regbase + UART_REG_OFFSET_DLH));

    u32_val &= ~UART_ENABLE_DIVISOR;
    writel(u32_val, (u32_regbase + UART_REG_OFFSET_LCR));

    return u32_baudrate_div;
}


ts_s32 hook_uart_read(ts_u32 u32_dev_idx, ts_u08 *pu08_buf, ts_u32 u32_size, ts_u32 u32_timeout_us)
{
    ts_u32 i, u32_lsr, u32_regbase;
    ts_u08 *pu08_ptr;
    dly_context_s_t st_dly;

    u32_regbase = soc_get_uart_regbase(u32_dev_idx);
    if (0 == u32_regbase)
        return -TS_ENOTSUP;

    pu08_ptr = pu08_buf;
    for (i = 0; i < u32_size; i++) {
        dly_lib_init_ctx(&st_dly, u32_timeout_us);
        while (1) {
            u32_lsr = readl(u32_regbase + UART_REG_OFFSET_LSR);
            if ((u32_lsr & (1 << UART_LSR_DR)) > 0)
                break;

            if (ts_true == dly_lib_timeout(&st_dly))
                return TS_ETIMEDOUT;
        }

        *pu08_ptr++ = (ts_u08)readl(u32_regbase + UART_REG_OFFSET_RBR);
    }

    return TS_OK;
}


ts_s32 hook_uart_write(ts_u32 u32_dev_idx, ts_u08 *pu08_buf, ts_u32 u32_size, ts_u32 u32_timeout_us)
{
    ts_u32 i, u32_status, u32_regbase;
    ts_u08 u08_val;
    dly_context_s_t st_dly;

    u32_regbase = soc_get_uart_regbase(u32_dev_idx);

    if (0 == u32_regbase)
        return -TS_ENOTSUP;

    for (i = 0; i < u32_size; i++) {
        dly_lib_init_ctx(&st_dly, u32_timeout_us);
        u08_val = pu08_buf[i];
        while (1) {
            u32_status = readl(u32_regbase + UART_REG_OFFSET_USR);
            if ((u32_status & (1 << UART_USR_TFNF)) > 0) {
                writel(u08_val, (u32_regbase + UART_REG_OFFSET_THR));

                break;
            }

            if (ts_true == dly_lib_timeout(&st_dly))
                return -TS_ETIMEDOUT;
        }
    }

    return TS_OK;
}


ts_void hook_uart_putc(ts_u32 u32_dev_idx, ts_u08 u08_val)
{
    ts_u32 i, u32_status, u32_regbase;

    u32_regbase = soc_get_uart_regbase(u32_dev_idx);
    if (0 == u32_regbase)
        return;

    for (i = 0; i < UART_MAX_TIMEOUT; i++) {
        u32_status = readl(u32_regbase + UART_REG_OFFSET_USR);
        if ((u32_status & (1 << UART_USR_TFNF)) > 0) {
            writel(u08_val, (u32_regbase + UART_REG_OFFSET_THR));

            break;
        }
    }
}


ts_void hook_uart_puts(ts_u32 u32_dev_idx, ts_char *pc_str)
{
    ts_u32 i;
    ts_char *pc_ptr;
    ts_char ch;

    i = 0;
    pc_ptr = pc_str;
    while (1) {
        if (i > 100)
            break;

        ch = *pc_ptr++;
        if ('\0' == ch)
            break;
        
        if ('\n' == ch)
            hook_uart_putc(u32_dev_idx, '\r');

        hook_uart_putc(u32_dev_idx, ch);
        i++;
    }
}


ts_s32 hook_uart_init(ts_u32 u32_dev_idx, ts_u32 u32_baudrate)
{
    ts_u32 u32_regbase, u32_val;

    u32_regbase = soc_get_uart_regbase(u32_dev_idx);
    if (0 == u32_regbase)
        return -TS_ENOTSUP;

    uart_set_baudrate(u32_regbase, u32_baudrate);

    u32_val = (1 << UART_FCR_FIFOE) | (1 << UART_FCR_RFIFOR) | (1 << UART_FCR_XFIFOR);
    writel(u32_val, (u32_regbase + UART_REG_OFFSET_FCR));

    writel(0, (u32_regbase + UART_REG_OFFSET_IER));

    soc_uart_set_mux(u32_dev_idx);

    return TS_OK;
}

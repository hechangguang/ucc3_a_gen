/*
 * uart_null.c -- uart 空实现代码，可以此作为模版编写
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


ts_s32 hook_uart_read(ts_u32 u32_dev_idx, ts_u08 *pu08_buf, ts_u32 u32_size)
{
    return -TS_ENOTSUP;
}


ts_s32 hook_uart_write(ts_u32 u32_dev_idx, ts_u08 *pu08_buf, ts_u32 u32_size)
{
    return -TS_ENOTSUP;
}


ts_s32 hook_uart_init(ts_u32 u32_dev_idx, ts_u32 u32_baudrate, ts_u32 u32_timeout_us)
{
    return -TS_ENOTSUP;
}

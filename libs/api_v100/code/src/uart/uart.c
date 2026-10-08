/* 
 * uart.c -- OPEN TDK 对 uart 驱动的封装，实现 hbp属性设置和获取 的功能
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */

#include <base/ts_type.h>
#include <base/ts_errno.h>
#include <cpl/ts_cpl.h>
#include <uapi/base_v100.h>
#include <uapi/uart_v100.h>
#include <kapi/k_uart_v100.h>
#include <command.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>


ts_s32 ts_lib_uart_hbp_set_attr(ts_s32 s32_hdev, ts_hbp_irqcb_fp_t fp_cb, ts_u32 u32_flg, ts_u32 u32_en)
{
    uart_proto_s_t st_attr;

    st_attr.fp_cb = fp_cb;
    st_attr.u32_flg = u32_flg;
    st_attr.u32_en = u32_en;

    return ioctl(s32_hdev, TS_IOCTRL_IOC_NR_UART_SET_HBP_ATTR, &st_attr);
}


ts_s32 ts_lib_uart_hbp_get_attr(ts_s32 s32_hdev, uart_proto_s_t *pst_hbp_attr)
{
    return ioctl(s32_hdev, TS_IOCTRL_IOC_NR_UART_GET_HBP_ATTR, pst_hbp_attr);
}


ts_s32 ts_lib_uart_set_buadr(ts_s32 s32_hdev, ts_u32 u32_baudrate)
{
    if ((u32_baudrate < 1200) || (u32_baudrate > 7500000)) {
        ts_uapi_log_err("baudr(%d) must in [1200..750000]\n", u32_baudrate);

        return -TS_EINVAL;
    }

    return (ts_s32)ioctl(s32_hdev, TS_IOCTRL_IOC_NR_UART_SET_BAUDR, u32_baudrate);
}


ts_s32 ts_lib_uart_get_attr(ts_s32 s32_hdev, uart_attr_s_t *pst_attr)
{
    if (ts_null == pst_attr) {
        ts_uapi_log_err("%s\n", TS_EINVAL_STR);

        return -TS_EINVAL;
    }
    
    return (ts_s32)ioctl(s32_hdev, TS_IOCTRL_IOC_NR_UART_GET_DEV_ATTR, (ts_u32)pst_attr);
}


ts_s32 ts_lib_uart_set_attr(ts_s32 s32_hdev, const uart_attr_s_t *pst_attr)
{
    if (ts_null == pst_attr) {
        ts_uapi_log_err("%s\n", TS_EINVAL_STR);

        return -TS_EINVAL;
    }

    if ((pst_attr->u08_data_bit < 5) || (pst_attr->u08_data_bit > 8)) {
        ts_uapi_log_err("%s,the data bit needs to be between 5..8\n", TS_EINVAL_STR);

        return -TS_EINVAL;
    }

    if (pst_attr->u08_stop_bit > UART_STOP_2BIT) {
        ts_uapi_log_err("%s,the stop bit needs to be 1,1.5,2\n", TS_EINVAL_STR);

        return -TS_EINVAL;
    }

    if ((UART_STOP_1_5BIT == pst_attr->u08_stop_bit) && (pst_attr->u08_data_bit != 5)) {
        ts_uapi_log_err("%s,the 1.5 stop bit is set, but the data bit is not 5\n", TS_EINVAL_STR);

        return -TS_EINVAL;
    }

    if (pst_attr->u08_parity_type > UART_PARITY_EVEN) {
        ts_uapi_log_err("%s,the parity needs to be none,odd,even\n", TS_EINVAL_STR);

        return -TS_EINVAL;
    }

    return (ts_s32)ioctl(s32_hdev, TS_IOCTRL_IOC_NR_UART_SET_DEV_ATTR, (ts_u32)pst_attr);
}


/*
* spis.c -- OPEN TDK 对 spislave 驱动的封装
* Created by : Top-Silicon Software Team
* Copyright : (C) 2023-2025 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
*
* This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
* may not be copied or distributed in any isomorphic form without the prior
* written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
*/
#include <base/ts_type.h>
#include <base/ts_errno.h>
#include <cpl/ts_cpl.h>
#include <uapi/spi_v100.h>
#include <uapi/base_v100.h>
#include <kapi/k_spimst_v100.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>


#define SPI_FIFO_MAX_DEP        (32)

ts_s32 ts_lib_spislv_set_attr(ts_s32 s32_hdl, ts_spi_attr_s_t *pst_attr)
{
    return (ts_s32)ioctl(s32_hdl, TS_SPI_IOCTL_SET_ATTR, (ts_long)pst_attr);
}


ts_s32 ts_lib_spislv_get_attr(ts_s32 s32_hdl, ts_spi_attr_s_t *pst_attr)
{
    return (ts_s32)ioctl(s32_hdl, TS_SPI_IOCTL_GET_ATTR, (ts_long)pst_attr);
}


/*
    1.避免发送错过时钟，驱动内部考虑fifo满的情况，减少函数调用
    2.接收，驱动内部直接将所有数据读回
    3.接收发送分开实现，避免接收发送互相影响错过时钟
*/
ts_s32 ts_lib_spislv_write(ts_s32 s32_hdl, ts_spi_msg_s_t *pst_msg)
{
    pst_msg->pu08_rx_buf = ts_null;
    pst_msg->u32_rx_size = 0;

    return ioctl(s32_hdl, TS_SPI_IOCTL_XFER, (ts_ulong)pst_msg);
}


ts_s32 ts_lib_spislv_read(ts_s32 s32_hdl, ts_spi_msg_s_t *pst_msg)
{
    pst_msg->pu08_cmd_buf = ts_null;
    pst_msg->u32_cmd_size = 0;
    pst_msg->pu08_tx_buf = ts_null;
    pst_msg->u32_tx_size = 0;

    return ioctl(s32_hdl, TS_SPI_IOCTL_XFER, (ts_ulong)pst_msg);
}



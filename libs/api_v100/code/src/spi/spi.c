/*
* spimst.c -- OPEN TDK 对 spimst 驱动的封装
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

ts_s32 ts_lib_spimst_set_attr(ts_s32 s32_hdl, ts_spi_attr_s_t *pst_attr)
{
    return (ts_s32)ioctl(s32_hdl, TS_SPI_IOCTL_SET_ATTR, (ts_long)pst_attr);
}


ts_s32 ts_lib_spimst_get_attr(ts_s32 s32_hdl, ts_spi_attr_s_t *pst_attr)
{
    return (ts_s32)ioctl(s32_hdl, TS_SPI_IOCTL_GET_ATTR, (ts_long)pst_attr);
}


ts_s32 ts_lib_spimst_xfer(ts_s32 s32_hdl, ts_spi_msg_s_t *pst_msg)
{
    ts_spi_msg_s_t st_use_msg;
    ts_u32 u32_max_fifo, u32_remain_cnt;
    ts_u32 u32_cmd_cnt, u32_tx_cnt, u32_rx_cnt;
    ts_u32 u32_cmd_pre_cnt, u32_tx_pre_cnt, u32_rx_pre_cnt;
    ts_u08 *pu08_cmd_ptr, *pu08_tx_ptr, *pu08_rx_ptr;
    ts_s32 s32_ret;


    u32_max_fifo = SPI_FIFO_MAX_DEP;
    u32_cmd_cnt = pst_msg->u32_cmd_size;
    u32_tx_cnt = pst_msg->u32_tx_size;
    u32_rx_cnt = pst_msg->u32_rx_size;

    pu08_cmd_ptr = pst_msg->pu08_cmd_buf;
    pu08_tx_ptr = pst_msg->pu08_tx_buf;
    pu08_rx_ptr = pst_msg->pu08_rx_buf;

    st_use_msg.u08_cs = pst_msg->u08_cs;
    st_use_msg.u08_dfs = pst_msg->u08_dfs;

    while(1) {
        u32_remain_cnt = u32_max_fifo;
        u32_cmd_pre_cnt = 0;
        u32_tx_pre_cnt = 0;
        u32_rx_pre_cnt = 0;

        if (u32_cmd_cnt <= 0 && u32_rx_cnt <= 0 && u32_tx_cnt <= 0)
            break;

        if (u32_cmd_cnt > 0) {
            if (u32_cmd_cnt > u32_remain_cnt)
                u32_cmd_pre_cnt = u32_remain_cnt;
            else
                u32_cmd_pre_cnt = u32_cmd_cnt;

            u32_remain_cnt -= u32_cmd_pre_cnt;
        }

        if (u32_tx_cnt > 0) {
            if (u32_tx_cnt > u32_remain_cnt)
                u32_tx_pre_cnt = u32_remain_cnt;
            else
                u32_tx_pre_cnt = u32_tx_cnt;
        }

        if (u32_rx_cnt > 0) {
            if (u32_rx_cnt > u32_max_fifo)
                u32_rx_pre_cnt = u32_max_fifo;
            else
                u32_rx_pre_cnt = u32_rx_cnt;
        }

        st_use_msg.pu08_cmd_buf = pu08_cmd_ptr;
        st_use_msg.u32_cmd_size = u32_cmd_pre_cnt;

        st_use_msg.pu08_tx_buf = pu08_tx_ptr;
        st_use_msg.u32_tx_size = u32_tx_pre_cnt;

        st_use_msg.pu08_rx_buf = pu08_rx_ptr;
        st_use_msg.u32_rx_size = u32_rx_pre_cnt;

        //printf("u32_cmd_pre_cnt %d u32_tx_pre_cnt %d u32_rx_pre_cnt %d\n", u32_cmd_pre_cnt, u32_tx_pre_cnt, u32_rx_pre_cnt);
        s32_ret = ioctl(s32_hdl, TS_SPI_IOCTL_XFER, (ts_ulong)&st_use_msg);
        if (s32_ret != TS_OK)
            return s32_ret;

        u32_cmd_cnt -= u32_cmd_pre_cnt;
        u32_tx_cnt -= u32_tx_pre_cnt;
        u32_rx_cnt -= u32_rx_pre_cnt;

        if (pst_msg->u08_dfs <= 8) {
            pu08_cmd_ptr += u32_cmd_pre_cnt;
            pu08_tx_ptr += u32_tx_pre_cnt;
            pu08_rx_ptr += u32_rx_pre_cnt;
        } else if (pst_msg->u08_dfs > 8 && pst_msg->u08_dfs <= 16) {
            pu08_cmd_ptr += (u32_cmd_pre_cnt * 2);
            pu08_tx_ptr += (u32_tx_pre_cnt * 2);
            pu08_rx_ptr += (u32_rx_pre_cnt * 2);
        } else if (pst_msg->u08_dfs > 16 && pst_msg->u08_dfs <= 32){
            pu08_cmd_ptr += (u32_cmd_pre_cnt * 4);
            pu08_tx_ptr += (u32_tx_pre_cnt * 4);
            pu08_rx_ptr += (u32_rx_pre_cnt * 4);
        }
    }

    return TS_OK;
}


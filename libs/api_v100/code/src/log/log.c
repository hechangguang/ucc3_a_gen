/*
 * log.c -- OPEN TDK 对 log 驱动的封装，实现 log 的功能,直接用户态实现
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
#include <uapi/base_v100.h>
#include <kapi/k_log_v100.h>
#include <kapi/k_sfc_v100.h>
#include <kapi/k_console_v100.h>
#include <uapi/sfc_v100.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <command.h>
#include <uapi/log_v100.h>


typedef struct log_ctx_s {

    /* log驱动句柄 */
    ts_s32 s32_dev_hdl;
} log_ctx_s_t;

static log_ctx_s_t sst_ctx = {0};

ts_s32 ts_lib_log2flash_init(ts_void)
{
    log_dev_s_t st_dev;
    ts_s32 s32_hdl;
    ts_s32 s32_dev_hdl;
    ts_u32 u32_type;
    ts_u32 u32_id;
    ts_sfc_dev_s_t *pst_dev;
    ts_sfc_ctx_s_t *pst_sfc_ctx;
    ts_u32 u32_grp_addr;
    ts_s32 s32_ret;

    s32_hdl = open("/dev/sfc", TS_CPL_RDWR);
    if (s32_hdl < 0) {
        ts_uapi_log_err("%s", TS_ENODEV_STR);

        return TS_ERR;
    }

    /* 获取驱动上下文 */
    s32_ret = (ts_s32)ioctl(s32_hdl, TS_SFC_IOCTL_INIT, (ts_long)&u32_grp_addr);
    if (s32_ret != TS_OK) {
        ts_uapi_log_err("Failed to get sfc context\n");

        return TS_ERR;
    }

    pst_sfc_ctx = (ts_sfc_ctx_s_t *)u32_grp_addr;
    pst_dev = (ts_sfc_dev_s_t *)&pst_sfc_ctx->ast_dev[0];

    s32_hdl = ts_lib_sfc_open("/mnt/base/log");
    if (s32_hdl <= 0) {
        ts_uapi_log_err("Failed to open /mnt/base/log\n");

        return TS_ERR;
    }

    u32_type = ts_lib_sfc_get_type();
    if (1 == u32_type) {
        /* nor */
        u32_id = tlib_sfc_read_id(pst_dev , 2);
        if (0x176085 == u32_id) {
            /* puya */
            st_dev.u32_page_size = 1024;
            st_dev.u32_erase_unit_size = 64 * 1024;
            st_dev.u32_slot_size = 512;
        } else {
            st_dev.u32_page_size = 256;
            st_dev.u32_erase_unit_size = 64 * 1024;
            st_dev.u32_slot_size = 256;
        }
    } else {
        /* nand */
        st_dev.u32_page_size = 2048;
        st_dev.u32_erase_unit_size = 128 * 1024;
        st_dev.u32_slot_size = 512;
    }

    st_dev.u32_region_size = 256 * 1024;
    st_dev.s32_hdl = s32_hdl;

    s32_dev_hdl = open("/dev/log", TS_CPL_RDWR);
    if (s32_dev_hdl < 0) {
        ts_uapi_log_err("Failed to open /dev/log interface\n");

        return TS_ERR;
    }

    sst_ctx.s32_dev_hdl = s32_dev_hdl;

    return ioctl(s32_dev_hdl, TS_IOCTRL_IOC_NR_LOG_INIT, (ts_ulong)&st_dev);
}


ts_s32 ts_lib_log2flash_clear(ts_void)
{
    return ioctl(sst_ctx.s32_dev_hdl, TS_IOCTRL_IOC_NR_LOG_CLEAR, 0);
}


ts_s32 ts_lib_log2flash_flush(ts_void)
{
    return ioctl(sst_ctx.s32_dev_hdl, TS_IOCTRL_IOC_NR_LOG_FLUSH, 0);
}


ts_s32 ts_lib_log2flash_read(ts_void)
{
    return ioctl(sst_ctx.s32_dev_hdl, TS_IOCTRL_IOC_NR_LOG_READ, 0);
}

ts_s32 ts_lib_log2flash_write(ts_u08 *pu08_buf, ts_u32 u32_len)
{
    return write(sst_ctx.s32_dev_hdl, pu08_buf, u32_len);
}

ts_s32 ts_lib_log2flash_en(ts_u32 u32_en)
{
    ts_s32 s32_hdl;

    if (u32_en) {
        /* 关闭log驱动 */
        close(sst_ctx.s32_dev_hdl);
        tsp_kcon_init("/dev/log");

        /* log驱动在kcon_init 重新open */
        sst_ctx.s32_dev_hdl = ts_h_stdin;
    } else {
        /* 原有log被close 重新open */
        tsp_kcon_init("/dev/ttyS0");
        s32_hdl = open("/dev/log", TS_CPL_RDWR);

        if (s32_hdl >= 0) {
            sst_ctx.s32_dev_hdl = s32_hdl;
        } else {
            ts_uapi_log_err("Failed to open the log interface when switching terminals\n");

            return TS_ERR;
        }
    }

    return TS_OK;
}


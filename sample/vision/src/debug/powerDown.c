/*
 * serial.c -- tdk isp 串口使用例子
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2024 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#include <base/ts_type.h>
#include <base/ts_errno.h>
#include <command.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <cpl/ts_cpl.h>
#include <uapi/vpo_v100.h>
#include <uapi/chip_def.h>
#include <uapi/viu_v100.h>
#include <uapi/isp_comm.h>
#include "api.h"
#include "snr_comm.h"

static ts_void pd_help(ts_void)
{
    vision_log_debug("pd usb/cvbs/vdet \n");
}

TS_DECLARE_CMD(pd)
{
    if (argc < 2)
    {
        pd_help();
    }
    else
    {
        ts_cpl_writel(0x0, 0x0013008c);
        ts_cpl_writel(0x0, 0x00130098);
        switch (argv[1][0])
        {
        case 'u':
            ts_cpl_writel(0xf81, 0x00160010);
            vision_log_debug("Powerdown usb.\n");
            break;
        case 'c':
            ts_cpl_writel(0x11, 0x0013002c);
            ts_cpl_writel(0x32aa8, 0x00160020);
            vision_log_debug("Powerdown cvbs.\n");
            break;
        case 'v':
            ts_cpl_writel(0x0, 0x00240000);
            ts_cpl_msleep(2000);
            for (ts_u32 i = 0; i < 2; i++)
            {
                ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 1); // 冻结为agc前
                ts_cpl_msleep(40);

                tdk_snrcfg_shutter_close();
                ts_tdk_isp_calc_nuc(TS_ISP_MDEV0);
                tdk_snrcfg_shutter_open();

                /*由于挡片后，图像整体亮度发生变化而导致图像闪烁，因此需要通过冻结变化过程，暂时使用offline实现， 后续通过vpo实现*/
                ts_tdk_isp_set_offline_enable(TS_ISP_MDEV0, 1);
                ts_isp_offline_frm_s_t st_frm;
                ts_s32 s32_ret = ts_tdk_isp_get_offline_frm(TS_ISP_MDEV0, &st_frm, 2000);
                if (s32_ret != TS_OK)
                {
                    vision_log_debug("ts_tdk_isp_get_offline_frm err !\n");
                    return 0;
                }
                ts_tdk_isp_send_offline_frm(TS_ISP_MDEV0, &st_frm);
                if (s32_ret != TS_OK)
                {
                    vision_log_debug("ts_tdk_isp_send_offline_frm err !\n");
                    return 0;
                }
                ts_tdk_isp_set_frame_freeze(TS_ISP_MDEV0, 0); // 解除冻结
                ts_cpl_msleep(40);
                ts_tdk_isp_write_reg(TS_ISP_MDEV0, 0xc, 0xffffffff); ////fw rst
                ts_cpl_msleep(40);
                ts_tdk_isp_set_offline_enable(TS_ISP_MDEV0, 0);
            }
            vision_log_debug("vdet select lower voltage.\n");
            break;
        default:
            vision_log_debug("Unsupport command.\n");
            break;
        }
    }
    return 0;
}

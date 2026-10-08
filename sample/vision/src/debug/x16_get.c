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


static ts_void x16Get_help(ts_void)
{
    printf("x16 x0 y0\n");
}

extern ts_s32 ts_tdk_viu_get_stats(ts_mdev_t h_viu, ts_u32 u32_type, ts_u16 u16_coor_x,ts_u16 u16_coor_y, ts_u32 *pu32_sum, ts_u32 *pu32_mean);

TS_DECLARE_CMD(x16)
{
    if (argc < 3){
        x16Get_help();
    }else{

        ts_u16 u16_coor_x = atoi(argv[1]);
        ts_u16 u16_coor_y = atoi(argv[2]);
        ts_u32 u32_sum = 0;

        ts_tdk_viu_get_stats(TS_VIU_MDEV0, TS_VIU_STATS_DOT, u16_coor_x, u16_coor_y, &u32_sum, ts_null);

        printf("[%d,%d] Value:%d\n",u16_coor_x,u16_coor_y,u32_sum);
    }
    return 0;
}

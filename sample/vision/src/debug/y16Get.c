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


static ts_void y16Get_help(ts_void)
{
    vision_log_debug("y16 s/c/k x0 y0 x1 y1 x2 y2\n");
}

extern ts_s32 ts_tdk_vpo_get_y16(ts_vpo_y16_attr_s_t *pst_attr);
TS_DECLARE_CMD(y16)
{
    ts_vpo_y16_attr_s_t ast_attr;

    if (argc < 8){
        y16Get_help();
    }else{
        ast_attr.ast_pos[0].u16_pos_x = atoi(argv[2]);
        ast_attr.ast_pos[0].u16_pos_y = atoi(argv[3]);
        ast_attr.ast_pos[1].u16_pos_x = atoi(argv[4]);
        ast_attr.ast_pos[1].u16_pos_y = atoi(argv[5]);
        ast_attr.ast_pos[2].u16_pos_x = atoi(argv[6]);
        ast_attr.ast_pos[2].u16_pos_y = atoi(argv[7]);

        vision_log_debug("Pos X0 %d\n",ast_attr.ast_pos[0].u16_pos_x);
        vision_log_debug("Pos Y0 %d\n",ast_attr.ast_pos[0].u16_pos_y);
        vision_log_debug("Pos X1 %d\n",ast_attr.ast_pos[1].u16_pos_x);
        vision_log_debug("Pos Y1 %d\n",ast_attr.ast_pos[1].u16_pos_y);
        vision_log_debug("Pos X2 %d\n",ast_attr.ast_pos[2].u16_pos_x);
        vision_log_debug("Pos Y2 %d\n",ast_attr.ast_pos[2].u16_pos_y);

        if('s' == argv[1][0]){
            ts_tdk_vpo_get_y16((ts_vpo_y16_attr_s_t *)&ast_attr);

            vision_log_debug("Y16[0] : %d\t",ast_attr.u16_y16_val[0]);
            vision_log_debug("Y16[1] : %d\t",ast_attr.u16_y16_val[1]);
            vision_log_debug("Y16[2] : %d\n",ast_attr.u16_y16_val[2]);
        }else if('c' == argv[1][0]){
            ts_u32 i = 0;
            while (i<100)
            {
                ts_tdk_vpo_get_y16((ts_vpo_y16_attr_s_t *)&ast_attr);
                vision_log_debug("Frame %d Y16[0] : %d\t",i,ast_attr.u16_y16_val[0]);
                vision_log_debug("Frame %d Y16[1] : %d\t",i,ast_attr.u16_y16_val[1]);
                vision_log_debug("Frame %d Y16[2] : %d\n",i,ast_attr.u16_y16_val[2]);
                i ++;
            }
        }else if('k' == argv[1][0]){
            ts_u32 i = 0;
            while (i<100)
            {
                ast_attr.ast_pos[0].u16_pos_x = i;
                ast_attr.ast_pos[0].u16_pos_y = i;
                ast_attr.ast_pos[1].u16_pos_x = 1 + i;
                ast_attr.ast_pos[1].u16_pos_y = 1 + i;
                ast_attr.ast_pos[2].u16_pos_x = 2 + i;
                ast_attr.ast_pos[2].u16_pos_y = 2 + i;
                ts_tdk_vpo_get_y16((ts_vpo_y16_attr_s_t *)&ast_attr);
                vision_log_debug("Frame %d Y16[0] : %d\t",i,ast_attr.u16_y16_val[0]);
                vision_log_debug("Frame %d Y16[1] : %d\t",i,ast_attr.u16_y16_val[1]);
                vision_log_debug("Frame %d Y16[2] : %d\n",i,ast_attr.u16_y16_val[2]);
                i ++;
            }
        }
    }

    return 0;
}

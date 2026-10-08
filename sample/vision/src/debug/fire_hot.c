/*
 * @Author: git config user.name && git config user.email
 * @Date: 2026-04-28 17:43:23
 * @LastEditors: git config user.name && git config user.email
 * @LastEditTime: 2026-09-07 14:42:04
 * @FilePath: /ucc3_a_gen/sample/vision/src/debug/fire_hot.c
 * @Description: 
 * 
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved. 
 */
#include <base/ts_type.h>
#include <base/ts_errno.h>
#include <cpl/ts_cpl.h>
#include <uapi/chip_def.h>
#include <uapi/isp_v100.h>
#include <command.h>
#include <stdio.h>
#include <api.h>

TS_DECLARE_CMD(fire_hot)
{
    // fire函数相关case
    if (!strcmp(argv[1], "case_fire")){
        ts_s32 s32_ret;
        ts_isp_fire_attr_s_t set_attr = {0};
        ts_isp_fire_attr_s_t get_attr = {0};

        // 设置自动分块使能和尺寸
        set_attr.u08_enable = 1;            /*使能，0 关闭， 1，开启*/  
        set_attr.u08_blk_size_w = 16;       /*横向块大小*/
        set_attr.u08_blk_size_h = 16;       /*纵向块大小*/

        // 设置手动分块区域
        for (int i = 0; i < ISP_FIRE_RECT_NUM; i++) {
            set_attr.ast_rect[i].b_enable = 1;                      /*手动分块使能，0 关闭， 1，开启*/  
            set_attr.ast_rect[i].u16_start_x = 10 * i;              /*横向起始位置*/
            set_attr.ast_rect[i].u16_start_y = 20 * i;              /*纵向起始位置*/
            set_attr.ast_rect[i].u16_end_x = 100 + 10 * i;          /*横向结束位置*/
            set_attr.ast_rect[i].u16_end_y = 200 + 20 * i;          /*纵向结束位置*/
        }

        // 设置坐标点
        for (int i = 0; i < ISP_FIRE_POS_NUM; i++) {
            set_attr.ast_pos[i].u16_pos_x = 50 * i;         /*横向坐标*/ 
            set_attr.ast_pos[i].u16_pos_y = 100 * i;        /*纵向坐标*/
        }

        // 调用 set 函数
        s32_ret = ts_tdk_isp_set_fire_attr(TS_ISP_MDEV0, &set_attr);
        if (s32_ret != 0) {
            printf("Set function failed! Error: %d\n", s32_ret);
        }

        // 调用 get 函数
        s32_ret = ts_tdk_isp_get_fire_attr(TS_ISP_MDEV0, &get_attr);
        if (s32_ret != 0) {
            printf("Get function failed! Error: %d\n", s32_ret);
        }

        //验证配置是否生效
        if (memcmp(&set_attr, &get_attr, sizeof(ts_isp_fire_attr_s_t)) != 0) {
            printf("Data mismatch between set and get!\n");
        }

        ts_isp_fire_statis_s_t fire_statis;
        memset(&fire_statis, 0, sizeof(ts_isp_fire_statis_s_t));
        

        // 测试获取数据
        s32_ret = ts_tdk_isp_get_fire_statis(TS_ISP_MDEV0, &fire_statis, 1000);
        if (s32_ret != 0) {
            printf("Error: Get fire statis failed! (ret=%d)\n", s32_ret);
        }else{
            printf(" Get fire statis success\n");
        }

        // 打印自动分块信息
        printf("\n[Auto Blocks] %dx%d blocks (block size: %dx%d):\n", 
            fire_statis.st_auto.u16_blk_num_x, 
            fire_statis.st_auto.u16_blk_num_y,
            fire_statis.st_auto.u16_blk_size_x,
            fire_statis.st_auto.u16_blk_size_y);
        
        // 打印所有自动分块数据内容
        // for (int y = 0; y < fire_statis.st_auto.u16_blk_num_y; y++) {
        //     for (int x = 0; x < fire_statis.st_auto.u16_blk_num_x; x++) {
        //         int idx = y * fire_statis.st_auto.u16_blk_num_x + x;
        //         ts_isp_fire_blk_info_s_t *blk = &fire_statis.st_auto.pst_blk_info[idx];
                
        //         printf("  Block(%d,%d):\n", x, y);
        //         printf("    Min: val=%u @(%u,%u)\n", blk->u16_min_val, blk->u16_min_pos_x, blk->u16_min_pos_y);
        //         printf("    Max: val=%u @(%u,%u)\n", blk->u16_max_val, blk->u16_max_pos_x, blk->u16_max_pos_y);
        //     }
        // }
        
        //打印整帧信息
        printf("\n[Frame Statistics]:\n");
        printf("  Min: val=%u @(%u,%u)\n", 
            fire_statis.st_frm_info.u16_min_val,
            fire_statis.st_frm_info.u16_min_pos_x,
            fire_statis.st_frm_info.u16_min_pos_y);
            
        printf("  Max: val=%u @(%u,%u)\n", 
            fire_statis.st_frm_info.u16_max_val,
            fire_statis.st_frm_info.u16_max_pos_x,
            fire_statis.st_frm_info.u16_max_pos_y);
            
        printf("  Mean: %u\n", fire_statis.st_frm_info.u16_mean);
        
        //打印手动区域信息
        printf("\n[Manual Areas]:\n");
        for (int i = 0; i < ISP_FIRE_RECT_NUM; i++) {
            ts_isp_fire_man_info_s_t *man_info = &fire_statis.ast_man_info[i];
            
            printf("  Area %d:\n", i);
            printf("    Area Mean: %u\n", man_info->u16_mean);
            
            printf("    Block Min: val=%u @(%u,%u)\n", 
                man_info->st_blk_info.u16_min_val,
                man_info->st_blk_info.u16_min_pos_x,
                man_info->st_blk_info.u16_min_pos_y);
                
            printf("    Block Max: val=%u @(%u,%u)\n", 
                man_info->st_blk_info.u16_max_val,
                man_info->st_blk_info.u16_max_pos_x,
                man_info->st_blk_info.u16_max_pos_y);
        }

        // 释放资源
        s32_ret = ts_tdk_isp_free_fire_statis(TS_ISP_MDEV0, &fire_statis);
        if (s32_ret != 0) {
            printf("Error: Free fire statis failed! (ret=%d)\n", s32_ret);
        }else{
            printf(" free fire statis success\n");
        }

    }


    // hot函数相关case
    if (!strcmp(argv[1], "case_hot")) {
        ts_s32 ret;
        ts_isp_hot_attr_s_t attr;       
        ts_isp_hot_statis_s_t statis;   

        // 1. 配置热点检测参数
        memset(&attr, 0, sizeof(attr));
        attr.u08_enable = 1;            
        attr.u08_samp_step = 0;         
        attr.u16_hot_thr = 16000;        
        attr.u16_edge_thr = 13000;        
        ret = ts_tdk_isp_set_hot_attr(TS_ISP_MDEV0, &attr);
        if (ret != 0) {
            printf("Failed to set hot attributes. Error code: %d\n", ret);
            return 0;
        }

        // 2. 验证是否设置成功
        ts_isp_hot_attr_s_t get_attr;
        memset(&get_attr, 0, sizeof(get_attr));
        ret = ts_tdk_isp_get_hot_attr(TS_ISP_MDEV0, &get_attr);
        if (ret != 0) {
            printf("Failed to get hot attributes. Error code: %d\n", ret);
            return 0;
        }
        printf("u08_enable = %d\n", get_attr.u08_enable);
        printf("u08_samp_step = %d\n", get_attr.u08_samp_step);
        printf("u16_hot_thr = %d\n", get_attr.u16_hot_thr);
        printf("u16_edge_thr = %d\n", get_attr.u16_edge_thr);

        // 3. 获取热点检测统计数据
        memset(&statis, 0, sizeof(statis));
        ret = ts_tdk_isp_get_hot_statis(TS_ISP_MDEV0, &statis, 1000);
        if (ret != 0) {
            printf("Failed to get hot statistics. Error code: %d\n", ret);
            return 0;
        }

        // 4. 打印统计信息
        printf("Hot spot count: %d\n", statis.u32_hot_cnt);
        printf("Edge point count: %d\n", statis.u32_edge_cnt);
        

        // 5. 释放统计数据资源
        ret = ts_tdk_isp_free_hot_statis(TS_ISP_MDEV0, &statis);
        if (ret != 0) {
            printf("Failed to free hot statistics. Error code: %d\n", ret);
        }
    }


    return 0;
}

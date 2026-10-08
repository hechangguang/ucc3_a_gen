/*
 * clk_tx10.h -- t10x 时钟驱动头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2022-2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __CLK_T10X_H__
#define __CLK_T10X_H__

#include <config.h>

typedef union clkm_pll_cfg0_un {
    struct  {
        ts_u32 prediv:6;        //[0..5]
        ts_u32 rev06_07:2;      //[6..7]
        ts_u32 fbdiv:12;        //[8..19]
        ts_u32 rev20_31:12;     //[20..31]
    };
    ts_u32 u32_val; 
} clkm_pll_cfg0_un_t;


typedef union clkm_pll_cfg2_un {
    struct  {
        ts_u32 postdiv0_0:4;        //[0..3]
        ts_u32 postdiv1_0:3;        //[4..6]
        ts_u32 rev07:1;             //[7]
        ts_u32 postdiv0_1:4;        //[8..11]
        ts_u32 postdiv1_1:3;        //[12..14]
        ts_u32 rev15_31:17;         //[15..31]
    };
    ts_u32 u32_val; 
} clkm_pll_cfg2_un_t;


/* pll 配置结构体 */
typedef struct clkm_pll_cfg_s {
    clkm_pll_cfg0_un_t un_cfg0;     /* 0x0000 */
    ts_u32 u32_rev1;                /* 0x0004 */
    clkm_pll_cfg2_un_t un_cfg2;     /* 0x0008 */
    ts_u32 u32_rev3;                /* 0x000C */
} clkm_pll_cfg_s_t;


#endif /* __CLK_T10X_H__ */


/*
 * clk_t10x.c -- t10x 时钟驱动，实现在启动阶段，完成 pll 的初始化，并配置正常工作时钟
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2022-2024 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#include <config.h>
#include <base/ts_type.h>
#include <base/ts_errno.h>
#include <io.h>
#include <bootup.h>
#include <lib/log.h>
#include "clk_t10x.h"


static ts_s32 clkm_calc_pll(ts_u32 u32_pll_mhz, ts_u32 u32_min_vco, ts_u32 *pu32_prediv,
                    ts_u32 *pu32_fbdiv, ts_u32 *pu32_div0, ts_u32 *pu32_div1)
{
    ts_u32 i, u32_div0, u32_div1, u32_vco, u32_fbdiv, u32_prediv;

    /* 预分频固定在 24 */
    u32_prediv = 24;

    /* div1 = 1..7; div0 = 1..15 */
    for (u32_div1 = 1; u32_div1 < 8; u32_div1++) {
        for (u32_div0 = 1; u32_div0 < 16; u32_div0++) {
            u32_vco = u32_pll_mhz * u32_div0 * u32_div1;
            if (u32_vco >= u32_min_vco) {
                *pu32_prediv = u32_prediv;
                *pu32_fbdiv = u32_vco;
                *pu32_div0 = u32_div0;
                *pu32_div1 = u32_div1;
            
                return TS_OK;
            }
        }
    }

    return TS_ERR;
}



static ts_void clkm_init_asic_release(ts_u32 u32_prc_clk, ts_u32 u32_vco_safe_en)
{
    ts_u32 u32_tmout, u32_lock;
    ts_u32 u32_div0, u32_div1, u32_prediv, u32_min_vco, u32_fbdiv;
    clkm_pll_cfg_s_t *pst_pll;
    clkm_pll_cfg0_un_t un_cfg0;
    clkm_pll_cfg2_un_t un_cfg2;

    //sys clk 460
    writel(0x00000010, 0x0013000c);

    if (0 == u32_vco_safe_en)
        u32_min_vco = 400;
    else
        u32_min_vco = 600;

    clkm_calc_pll(u32_prc_clk, u32_min_vco, (ts_u32 *)&u32_prediv, \
                    (ts_u32 *)&u32_fbdiv, (ts_u32 *)&u32_div0, (ts_u32 *)&u32_div1);

    pst_pll = (clkm_pll_cfg_s_t *)(0x00130010);

    un_cfg0.u32_val = pst_pll->un_cfg0.u32_val;
    un_cfg0.prediv = u32_prediv;
    un_cfg0.fbdiv = u32_fbdiv;
    pst_pll->un_cfg0.u32_val = un_cfg0.u32_val;

    un_cfg2.u32_val = pst_pll->un_cfg2.u32_val;
    un_cfg2.postdiv0_0 = u32_div0;
    un_cfg2.postdiv0_1 = u32_div1;
    pst_pll->un_cfg2.u32_val = un_cfg2.u32_val;

    writel(0x00000010, 0x0013001c);

    //pll_108 108
    if (0 == u32_vco_safe_en) {
        writel(0x0000d818, 0x00130020); // 24 * 54 / 6 = 216
        writel(0x00001112, 0x00130028); //216 / 2 = 108
    }

    writel(0x00000010, 0x0013002c);

    //viu clk 432
    writel(0x00000010, 0x0013003c);

    //isp clk 360
    writel(0x00000010, 0x0013004c);

    //lcd clk 432
    writel(0x00000010, 0x0013005c);

    u32_tmout = 0;
    while (1) {
        u32_lock = readl(0x00130064);
        if ((u32_lock & 0x7f) > 0)
            break;

        u32_tmout++;
        if (u32_tmout > 0x10000) {
            ts_log(TS_LOG_EMERG, "PLL lock timeout\n");

            break;
        }
    }
}


/* 时钟单位为 MHZ */
ts_void hook_clkm_init(ts_u32 u32_cpu_clk, ts_u32 u32_bus_clk, ts_u32 u32_ddr_clk, ts_u32 u32_vco_safe_en)
{
    ts_u32 u32_val;

    /* SYS_VCO = u32_cpu_clk  * 2 = (24M / 24) * x */

    if (0 == u32_vco_safe_en) {
        printf("vco safe close\n");
        u32_val = u32_cpu_clk << 1;
        writel(((u32_val << 8) | 24), 0x00130000);
        writel(0x00001111, 0x00130008);
    } else {
        u32_val = u32_cpu_clk << 2;
        writel(((u32_val << 8) | 24), 0x00130000);
        writel(0x00001112, 0x00130008);
    }

    clkm_init_asic_release(u32_ddr_clk, u32_vco_safe_en);

    //open all pll clock en
    writel(0x0000003f, 0x00130060);

    //axi clk en & div_num; u32_bus_clk 靠用户保证
    u32_val = u32_cpu_clk / u32_bus_clk;
    if (u32_val > 0)
        u32_val--;

    u32_val |= 0x80000000;
    writel(u32_val, 0x0013006c);

    //apb clk en & div_num
    u32_val = u32_cpu_clk / (u32_bus_clk >> 1);
    if (u32_val > 0)
        u32_val--;

    u32_val |= 0x80000000;
    writel(u32_val, 0x00130074);

    //switch cpu clock
    u32_val = readl(0x00130078);
    u32_val |= 0x1 << 2;
    writel(u32_val, 0x00130078);

    /* reset uart baudrate */
    //serial_set_baudrate(CONFIG_CLK_APB, 115200);
    writel(0x18CFC, 0x001300c4); //soc clk enable
    writel(0x306, 0x001300c8); //soc clk enable
    writel(0xffffffff, 0x001300cc); //soc clk enable
    writel(0x00000001, 0x001300d0); //clk monitor

    /* 重新设置 APB */
    clk_drv_set(TS_BOOTUP_CLKT_APB, CONFIG_CLK_APB);
}


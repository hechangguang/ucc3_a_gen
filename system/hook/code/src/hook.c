/*
 * bootup.c -- bootup 主文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2022-2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#include <config.h>
#include <base/ts_type.h>
#include <base/ts_errno.h>
#include <base/ts_log.h>
#include <bootup.h>
#include <ts_string.h>
#include <io.h>
#include <scpu.h>
#include <app/snand_flash.h>
#include <app/boot_param.h>
#include <app/hook_bootup.h>
#include <hbproto.h>
#include <lib/log.h>
#include <bootup_api.h>


//#define HOOK_IGNORE

extern ts_u32 __hook_bss_start[];
extern ts_u32 __hook_end[];

#ifdef CONFIG_SUPPORT_CMDL
extern ts_u32 __cmdl_start;
extern ts_u32 __cmdl_end;
#endif


extern const hbp_ops_s_t gst_hbp_serial;

static ts_void bu_clear_bss(ts_void)
{
    ts_u32 i, u32_cnt;
    ts_u32 u32_bss_start, u32_bss_end;
    ts_u32 *pu32_ptr;

    u32_bss_start = (ts_u32)&__hook_bss_start;
    u32_bss_end = (ts_u32)&__hook_end;
    u32_cnt = (u32_bss_end - u32_bss_start) >> 2;
    u32_cnt++;
    pu32_ptr = (ts_u32 *)u32_bss_start;
    for (i = 0; i < u32_cnt; i++)
        *pu32_ptr++ = 0;
}


static ts_void buh_set_extra_cmds(ts_void)
{
#ifdef CONFIG_SUPPORT_CMDL
    cmd_info_s_t *pst_extra_cmds;
    ts_u32 u32_cmd_cnt;

    u32_cmd_cnt = ((ts_u32)&__cmdl_end - (ts_u32)&__cmdl_start) >> 3;
    pst_extra_cmds = (cmd_info_s_t *)&__cmdl_start;

    cmdl_set_extra_list(pst_extra_cmds, u32_cmd_cnt);
#endif
}


static ts_void hook_test_ddr(ts_void)
{
    ts_u32 *pu32_ddr;
    ts_u32 i, u32_val;

    printf("ddr test:\n");

    pu32_ddr = (ts_u32 *)0x30000000;
    u32_val = 0x55AA55AA;
    for (i = 0; i < 10000; i++) {
        pu32_ddr[i] = u32_val;
        u32_val = ~u32_val;
    }

    printf("ddr test wr done\n");
    pu32_ddr = (ts_u32 *)0x30000000;
    u32_val = 0x55AA55AA;
    for (i = 0; i < 10000; i++) {
        if (pu32_ddr[i] != u32_val) {
            printf("ddr rd fail @%d, wr=%x, rd=%x\n", i, u32_val, pu32_ddr[i]);

            break;
        }

        u32_val = ~u32_val;
    }

    printf("test done\n");
}


ts_s32 HOOK_START_SECT bootup_hook(ts_void)
{
    ts_u32 u32_ddr_clk;
    ts_s32 s32_ret;

    /* 对 BSS 段清零 */
    bu_clear_bss();

    u32_ddr_clk = 250;

    /* 时钟模块初始化, CPU 100MHZ, bus 100MHZ, DDR 250MHZ, 用户请勿擅自修改,  u32_vco_safe_en = 0 情况下，不保证 PLL 稳定，风险自负 */
    //hook_clkm_init(160, 80, 250);
    //hook_clkm_init(100, 100, 250);
    //hook_clkm_init(114, 114, 250);
    //hook_clkm_init(228, 114, u32_ddr_clk, 1);
    hook_clkm_init(200, 100, u32_ddr_clk, 1);

    /* 需要重新配置 SFC 频率 */
    tdl_sfc_set_attr(4, 0);

    /* 芯片相关底层初始化 */
    soc_init();

    /* ddr 初始化, 双 DDR，每片 32MB, 时钟频率 250MHZ, 用户请勿擅自修改 */
#if 1
    hook_ddr_init(1, 1, BOOTUP_DDR_VOLUME_32MB, u32_ddr_clk);

    //hook_test_ddr();
#else
    /* for n2 */
    s32_ret = hook_ddr_init(1, 0, BOOTUP_DDR_VOLUME_32MB, 200);
    if (s32_ret != TS_OK) {
        ts_log(TS_LOG_ERR, "prc reinit\n");

        hook_ddr_init(1, 0, BOOTUP_DDR_VOLUME_4MB, 200);
    }

#endif

    /* 关闭看门狗 */
	wdt_stop();

    /* 注册 hook 命令列表，并等待用户选择, 注释增加启动速度 */
    //buh_set_extra_cmds();
    //cancel_lib_by_key(8);

    /* 跳转应用镜像，该函数不会返回 */
    hook_boot_app();

    return TS_OK;
}


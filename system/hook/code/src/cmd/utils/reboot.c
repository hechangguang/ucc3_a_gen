/* 
 * reboot.c -- 利用看门狗实现重启功能
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#include <config.h>
#include <base/ts_type.h>
#include <bootup.h>
#include <ts_string.h>

/* 
 * 函数名 :  cmdl_reboot
 * 描  述 :  利用看门狗实现重启功能
 * 输  入 :  参数               描述
 *          argc                输入参数个数
 *          argv                输入参数
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           无                 无
 */
static ts_void cmdl_reboot(int argc, char * argv[])
{
    ts_u32 i, u32_dly, u32_dly_cnt, u32_wdt_dly;

    if (2 == argc)
        u32_dly = ts_strtoul((const ts_char *)argv[1]);
    else
        u32_dly = 0;
    
    if (u32_dly < 2)
        u32_dly = 2;

    if (u32_dly > 10)
        u32_dly = 10;

    u32_wdt_dly = 500;

    wdt_stop();
    u32_dly_cnt = u32_dly * 1000;
    for (i = 0; i < u32_dly_cnt; i++)
        dly_lib_delay_us(1000);

    wdt_start(u32_wdt_dly);

    while(1);
}

CMDL_REGISTER(reboot, cmdl_reboot)

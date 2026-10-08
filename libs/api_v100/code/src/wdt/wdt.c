/* 
 * wdt.c -- OPEN TDK 对 wdt 驱动的封装，实现 wdt 的功能,直接用户态实现
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
#include <uapi/wdt_v100.h>

/* for T10x */
#define CONFIG_CLK_OSC          (24000000)
#define CONFIG_WDT_REGBASE      (0x00180000)

/* 通用寄存器配置 */
#define WDT_REG_ACCESS          (CONFIG_WDT_REGBASE + 0x0000)
#define WDT_REG_CTRL            (CONFIG_WDT_REGBASE + 0x0004)
#define WDT_REG_LOAD_DATA_H     (CONFIG_WDT_REGBASE + 0x000C)
#define WDT_REG_LOAD_DATA_L     (CONFIG_WDT_REGBASE + 0x0010)
#define WDT_REG_FEEDING         (CONFIG_WDT_REGBASE + 0x0014)

#define WDT_REG_RST_CLR         (CONFIG_WDT_REGBASE + 0x0028)
#define WDT_REG_RST_STATUS      (CONFIG_WDT_REGBASE + 0x002C)

/* for WDT_REG_ACCESS */
#define WDT_V100_SET_UNLOCK     (0x20221017)
#define WDT_V100_SET_LOCK       (0x0)

/* for WDT_REG_CTRL */
#define WDT_V100_ENABLE         (1)
#define WDT_V100_DISABLE        (0)

/* for feeding */
#define WDT_V100_FEED           (0x5a3c2d1e)

/* 控制看门狗超时的最大和最小时间 */
#define  WDT_MAX_TMOUT          (178000)
#define  WDT_MIN_TMOUT          (200)


ts_s32 ts_lib_wdt_stop(ts_void)
{
    ts_cpl_writel(WDT_V100_SET_UNLOCK, WDT_REG_ACCESS);
    ts_cpl_writel(WDT_V100_DISABLE, WDT_REG_CTRL);
    ts_cpl_writel(WDT_V100_SET_LOCK, WDT_REG_ACCESS);

    return TS_OK;
}


ts_s32 ts_lib_wdt_set_timeout(ts_u32 u32_timeout_ms, ts_u32 u32_restart)
{
    ts_u32 u32_dly_ms, u32_period;

    if (u32_restart > 0)
        ts_lib_wdt_stop();
    
    if (u32_timeout_ms < WDT_MIN_TMOUT)
        u32_dly_ms = WDT_MIN_TMOUT;
    else if (u32_timeout_ms > WDT_MAX_TMOUT)
        u32_dly_ms = WDT_MAX_TMOUT;
    else
        u32_dly_ms = u32_timeout_ms;

    u32_period = (CONFIG_CLK_OSC / 1000) * u32_dly_ms;

    ts_cpl_writel(WDT_V100_SET_UNLOCK, WDT_REG_ACCESS);
    ts_cpl_writel(0, WDT_REG_LOAD_DATA_H);
    ts_cpl_writel(u32_period, WDT_REG_LOAD_DATA_L);

    if (u32_restart > 0)
        ts_cpl_writel(WDT_V100_ENABLE, WDT_REG_CTRL);

    ts_cpl_writel(WDT_V100_SET_LOCK, WDT_REG_ACCESS);

    return TS_OK;
}


ts_s32 ts_lib_wdt_set_mode(ts_wdt_mode_en_t en_mode)
{
    ts_uapi_log_err("%s\n", TS_ENOTSUP_STR);

    return -TS_ENOTSUP;
}


ts_s32 ts_lib_wdt_feeding(ts_void)
{
    ts_cpl_writel(WDT_V100_SET_UNLOCK, WDT_REG_ACCESS);
    ts_cpl_writel(WDT_V100_FEED, WDT_REG_FEEDING);
    ts_cpl_writel(WDT_V100_SET_LOCK, WDT_REG_ACCESS);

    return TS_OK;
}


ts_s32 ts_lib_wdt_get_reset_status(ts_void)
{
    ts_u32 u32_val;

    u32_val = ts_cpl_readl(WDT_REG_RST_STATUS);
    ts_cpl_writel(0x01, WDT_REG_RST_CLR);
    if ((u32_val & 0x04) > 0)
        return TS_WDT_RESET;
    else
        return TS_WDT_POR;
}


ts_s32 ts_lib_wdt_set_callback(ts_wdt_irqcb_fp_t fp_irqcb, void *pv_priv)
{
    ts_uapi_log_err("%s\n", TS_ENOTSUP_STR);

    return -TS_ENOTSUP;
}


ts_s32 ts_lib_wdt_start(ts_u32 u32_timeout_ms)
{
    return ts_lib_wdt_set_timeout(u32_timeout_ms, 1);
}

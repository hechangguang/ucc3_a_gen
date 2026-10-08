/* 
 * rtc.c -- rtc 实现, 对接 spi-rtc 驱动，实现对 rtc 读写，日期转化，用于 RTOS
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#include <config.h>
#include <base/ts_type.h>
#include <base/ts_errno.h>

#include <cpl/ts_cpl.h>
#include <api/rtc_v100.h>

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

static ts_s32 s32_rtc_hdl = -1;

extern ts_u32 daytick_mktime(const ts_u32 year0, const ts_u32 mon0, const ts_u32 day, const ts_u32 hour,
            const ts_u32 min, const ts_u32 sec);

extern ts_void daytick_mkdate(const ts_u32 u32_now, ts_cpl_rtctime_s_t *pst_tm);

/* 
 * 函数名 :  ts_lib_rtc_open
 * 描  述 :  打开 rtc 设备
 * 输  入 :  参数               描述
 *           pc_dev_name        rtc 设备文件名, 不输入则为默认设备名
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           TS_OK              rtc 设备打开成功
 *           TS_ERR             rtc 设备打开失败
 */
ts_s32 ts_lib_rtc_open(ts_char *pc_dev_name)
{
    if (s32_rtc_hdl < 0) {
        if (ts_null == pc_dev_name)
            s32_rtc_hdl = open("/dev/tdk/rtc", O_RDWR);
        else
            s32_rtc_hdl = open(pc_dev_name, O_RDWR);
    }

    if (s32_rtc_hdl >= 0)
        return TS_OK;
    else
        return TS_ERR;
}


/* 
 * 函数名 :  ts_lib_rtc_close
 * 描  述 :  关闭 rtc 设备
 * 输  入 :  参数               描述
 *           无                 无
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           无                 无
 */
ts_void ts_lib_rtc_close(ts_void)
{
    if (s32_rtc_hdl >= 0) {
        close(s32_rtc_hdl);

        s32_rtc_hdl = -1;
    }
}


/* 
 * 函数名 :  ts_lib_rtc_read
 * 描  述 :  获取当前 rtc 计时
 * 输  入 :  参数               描述
 *           无                 无
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           TS_OK              获取成功
 *           TS_ERR             获取失败
 *           TS_EINVAL          无效参数
 */
ts_s32 ts_lib_rtc_get(ts_cpl_rtctime_s_t *pst_time)
{
    ts_long lg_now;

    if ((s32_rtc_hdl >= 0) && (pst_time != ts_null)) {
        lg_now = ioctl(s32_rtc_hdl, TS_RTC_IOCTL_GET_NOW, 0);

        daytick_mkdate((const ts_u32)lg_now, pst_time);

        return TS_OK;
    } else {
        return -TS_EINVAL;
    }
}


/* 
 * 函数名 :  ts_lib_rtc_set
 * 描  述 :  设置当前 rtc
 * 输  入 :  参数               描述
 *           pst_time           当前完整日期
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           TS_OK              设置成功
 *           TS_EINVAL          无效参数
 */
ts_s32 ts_lib_rtc_set(ts_cpl_rtctime_s_t *pst_time)
{
    ts_long lg_now;

    if ((s32_rtc_hdl >= 0) && (pst_time != ts_null)) {
        lg_now = (ts_long)daytick_mktime(pst_time->tm_year, pst_time->tm_mon, pst_time->tm_mday, \
                        pst_time->tm_hour, pst_time->tm_min, pst_time->tm_sec);

        ioctl(s32_rtc_hdl, TS_RTC_IOCTL_SET_NOW, lg_now);

        return TS_OK;
    } else {
        return -TS_EINVAL;
    }
}

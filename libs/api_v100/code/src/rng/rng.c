/* 
 * rng.c -- OPEN TDK 对 rng 驱动的封装，实现 rng 的功能
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#include <base/ts_type.h>
#include <base/ts_errno.h>

#include <cpl/ts_cpl.h>
#include <api/rng_v100.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

/* 
 * 函数名 :  ts_lib_rng_srand
 * 描  述 :  设置伪随机数种子
 * 输  入 :  参数           描述
 *          s32_handle     设备句柄
 *          u32_seed       伪随机数种子值
 * 输  出 :  参数           描述
 *          无              无
 * 返  回 :  返回值         描述
 *          TS_OK          设置成功
 *          TS_EINVAL      参数错误
 *          TS_ERR         生成失败
 */
ts_s32 ts_lib_rng_srand(ts_s32 s32_handle, ts_u32 u32_seed)
{
    ts_s32 s32_ret;

    if (s32_handle < 0)
        s32_ret = -TS_EINVAL;
    else
        s32_ret = (ts_s32)ioctl(s32_handle, TS_RNG_IOCTL_SRAND, (ts_long)u32_seed);

    return s32_ret;
}


/* 
 * 函数名 :  ts_lib_rng_pseudo_random
 * 描  述 :  生成伪随机数
 * 输  入 :  参数           描述
 *          s32_handle     设备句柄
 * 输  出 :  参数           描述
 *          pu32_prandom   指向生成的伪随机数的指针
 * 返  回 :  返回值         描述
 *          TS_OK          生成成功
 *          TS_EINVAL      参数错误
 *          TS_ERR         生成失败
 */
ts_s32 ts_lib_rng_pseudo_random(ts_s32 s32_handle, ts_u32 *pu32_prandom)
{
    ts_s32 s32_ret;

    if ((s32_handle < 0) || (ts_null == pu32_prandom))
        s32_ret = -TS_EINVAL;
    else
        s32_ret = (ts_s32)ioctl(s32_handle, TS_RNG_IOCTL_PSEUDO_RANDOM, (ts_long)pu32_prandom);

    return s32_ret;
}


/* 
 * 函数名 :  ts_lib_rng_pseudo_random_range
 * 描  述 :  生成指定范围内的伪随机数
 * 输  入 :  参数           描述
 *          s32_handle     设备句柄
 *          u32_min        生成的伪随机数的最小值
 *          u32_max        生成的伪随机数的最大值
 * 输  出 :  参数           描述
 *          pu32_prandom   指向生成的伪随机数的指针
 * 返  回 :  返回值         描述
 *          TS_OK          生成成功
 *          TS_EINVAL      参数错误
 *          TS_ERR         生成失败
 */
ts_s32 ts_lib_rng_pseudo_random_range(ts_s32 s32_handle, ts_u32 *pu32_prandom, ts_u32 u32_min, ts_u32 u32_max)
{
    ts_s32 s32_ret;
    ts_rng_arg_s_t st_rng_arg;

    if ((s32_handle < 0) || (u32_min >= u32_max) || (ts_null == pu32_prandom)) {
        s32_ret = -TS_EINVAL;
    } else {
        st_rng_arg.u32_min = u32_min;
        st_rng_arg.u32_max = u32_max;
        
        s32_ret = (ts_s32)ioctl(s32_handle, TS_RNG_IOCTL_PSEUDO_RANDOM_RANGE, (ts_long)&st_rng_arg);
    }

    if (TS_OK == s32_ret)
        *pu32_prandom = st_rng_arg.u32_random;

    return s32_ret;
}


/* 
 * 函数名 :  ts_lib_rng_random
 * 描  述 :  生成真随机数
 * 输  入 :  参数           描述
 *          s32_handle     设备句柄
 *          u32_timeout    生成真随机数超时值
 * 输  出 :  参数           描述
 *          pu32_random    指向生成的真随机数的指针
 * 返  回 :  返回值         描述
 *          TS_OK          生成成功
 *          TS_EINVAL      参数错误
 *          TS_ETIMEDOUT   超时错误
 *          TS_ERR         生成失败
 */
ts_s32 ts_lib_rng_random(ts_s32 s32_handle, ts_u32 *pu32_random, ts_u32 u32_timeout)
{
    ts_s32 s32_ret;
    ts_rng_arg_s_t st_rng_arg;

    if ((s32_handle < 0) || (ts_null == pu32_random)) {
        s32_ret = -TS_EINVAL;
    } else {
        if (0 == u32_timeout)
            st_rng_arg.u32_timeout = (~u32_timeout);
        else
            st_rng_arg.u32_timeout = u32_timeout;

        s32_ret = (ts_s32)ioctl(s32_handle, TS_RNG_IOCTL_RANDOM, (ts_long)&st_rng_arg);
    }

    if (TS_OK == s32_ret)
        *pu32_random = st_rng_arg.u32_random;

    return s32_ret;
}


/* 
 * 函数名 :  ts_lib_rng_random_range
 * 描  述 :  生成指定范围内的真随机数
 * 输  入 :  参数           描述
 *          s32_handle     设备句柄
 *          u32_timeout    生成真随机数超时值 
 *          u32_min        生成的真随机数的最小值
 *          u32_max        生成的真随机数的最大值
 * 输  出 :  参数           描述
 *          pu32_random    指向生成的真随机数的指针
 * 返  回 :  返回值         描述
 *          TS_OK          生成成功
 *          TS_EINVAL      参数错误
 *          TS_ETIMEDOUT   超时错误
 *          TS_ERR         生成失败
 */
ts_s32 ts_lib_rng_random_range(ts_s32 s32_handle, ts_u32 *pu32_random, ts_u32 u32_timeout,\
                                 ts_u32 u32_min, ts_u32 u32_max)
{
    ts_s32 s32_ret;
    ts_rng_arg_s_t st_rng_arg;

    if ((s32_handle < 0) || (u32_min >= u32_max) || (ts_null == pu32_random)) {
        s32_ret = -TS_EINVAL;
    } else {
        st_rng_arg.u32_min = u32_min;
        st_rng_arg.u32_max = u32_max;
        if (0 == u32_timeout)
            st_rng_arg.u32_timeout = (~u32_timeout);
        else
            st_rng_arg.u32_timeout = u32_timeout;
        
        s32_ret = (ts_s32)ioctl(s32_handle, TS_RNG_IOCTL_RANDOM_RANGE, (ts_long)&st_rng_arg);
    }

    if (TS_OK == s32_ret)
        *pu32_random = st_rng_arg.u32_random;

    return s32_ret;
}


/* 
 * 函数名 :  ts_lib_rng_open
 * 描  述 :  打开 rng 设备
 * 输  入 :  参数           描述
 *          pc_dev_name    设备文件名
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           >= 0           设备句柄
 *           < 0            打开设备失败
 */
ts_s32 ts_lib_rng_open(ts_char *pc_dev_name)
{
    return open(pc_dev_name, O_RDWR);
}


/* 
 * 函数名 :  ts_lib_rng_close
 * 描  述 :  关闭 rng 设备句柄
 * 输  入 :  参数           描述
 *          s32_handle     设备句柄
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           无             无
 */
ts_void ts_lib_rng_close(ts_s32 s32_handle)
{
    if (s32_handle >= 0)
        close(s32_handle);
}

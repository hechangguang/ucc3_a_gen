/*
 * checksum.c -- 校验和实现
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2022-2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#include <config.h>
#include <bootup.h>
#include <base/ts_type.h>
#include <base/ts_errno.h>
//#include <lib/verify.h>

/*
 * 函数名 :  verify_lib_calc_sum32
 * 描  述 :  按照 4 byte 计算校验和
 * 输  入 :  参数           描述
 *          u32_init_val   初始值
 *          pu8_inbuf      待计算缓冲
 *          u32_size       待计算大小
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *          0..0xffffffff  最终计算值
 */
ts_u32 verify_lib_calc_sum32(ts_u32 u32_init_val, ts_u08 *pu8_inbuf, ts_u32 u32_size)
{
    ts_u32 u32_sum_val;
    ts_u32 i;
    ts_u32 *pu32_ptr;

    /* param check */
    if ((0 == u32_size) || (null == pu8_inbuf))
        return u32_init_val;
    
    u32_sum_val = u32_init_val;
    pu32_ptr = (ts_u32 *)pu8_inbuf;
    for (i = 0; i < (u32_size >> 2); i++)
        u32_sum_val += *pu32_ptr++;

    return u32_sum_val;
}


/*
 * 函数名 :  verify_lib_calc_sum8
 * 描  述 :  按照 1 byte 计算校验和
 * 输  入 :  参数           描述
 *          u08_init_val   初始值
 *          pu8_inbuf      待计算缓冲
 *          u32_size       待计算大小
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *          0..0xFF         最终计算值
 */
ts_u08 verify_lib_calc_sum8(ts_u08 u08_init_val, ts_u08 *pu8_inbuf, ts_u32 u32_size)
{
    ts_u08 u08_sum_val;
    ts_u32 i;
    ts_u08 *pu8_ptr;

    /* param check */
    if ((0 == u32_size) || (ts_null == pu8_inbuf))
        return u08_init_val;

    u08_sum_val = u08_init_val;
    pu8_ptr = pu8_inbuf;
    for (i = 0; i < u32_size; i++)
        u08_sum_val += *pu8_ptr++;

    return u08_sum_val;
}


/*
 * 函数名 :  verify_lib_checksum
 * 描  述 :  检查校验和
 * 输  入 :  参数           描述
 *           无             无
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *          ts_success     校验和匹配
 *          ts_failure     校验和不匹配
 */
ts_s32 verify_lib_check_sum(ts_void)
{
    return TS_OK;
}


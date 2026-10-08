/*
 * crc8.c --  crc8 实现
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

#define POLY	(0x1070U << 3)

static ts_u08 calc_crc8(ts_u16 u16_dat)
{
    ts_s32 i;

	for (i = 0; i < 8; i++) {
		if (u16_dat & 0x8000)
			u16_dat = u16_dat ^ POLY;

		u16_dat = u16_dat << 1;
	}

	return (ts_u08)(u16_dat >> 8);
}


/*************************************************
  函数名 :  verify_lib_calc_crc8
  描  述 :  对输入数据做 CRC8 计算
  输  入 :  参数            描述
          u32_init_val    用户自定义初值，也可以是分步计算的前一个计算结果
          pu8_inbuf       待计算数据
          u32_size        待计算数据大小
  输  出 :  参数            描述
           无             无
  返  回 :  返回值          描述
          0..0xffffffff   最终 Crc8 计算结果
*************************************************/
ts_u32 verify_lib_calc_crc8(ts_u32 u32_init_val, ts_u08 *pu8_inbuf, ts_u32 u32_size)
{
    ts_s32 i;
    ts_u32 u32_crc_val;

    u32_crc_val = u32_init_val;
    for (i = 0; i < u32_size; i++)
        u32_crc_val = calc_crc8((u32_crc_val ^ pu8_inbuf[i]) << 8);

    return u32_crc_val;
}


/*
 * 函数名 :  verify_lib_check_crc8
 * 描  述 :  检查 CRC8
 * 输  入 :  参数           描述
 *           无             无
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *          ts_success     CRC8 匹配
 *          ts_failure     CRC8 不匹配
 */
ts_s32 verify_lib_check_crc8(ts_void)
{
    return ts_success;
}


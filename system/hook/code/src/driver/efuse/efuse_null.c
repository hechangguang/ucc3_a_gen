/*
 * efuse_null.c -- efuse  空实现实现代码
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2024 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#include <config.h>
#include <base/ts_type.h>
#include <base/ts_errno.h>
#include <io.h>

/*
每列 32 bit， 每行 64 bit
*/
ts_s32 efuse_hal_write_bit(ts_u32 u32_bit_offset)
{
	return TS_ERR;
}


ts_s32 efuse_hal_writel(ts_u32 addr, ts_u32 data)
{
	return TS_ERR;
}


ts_void efuse_read_pubkey(ts_void)
{
	/* do nothing */
}


ts_void efuse_init_pubkey(ts_void)
{
	/* do nothing */
}


/* 根据需要，如果 26MHZ 下无需重配置参数 */
ts_void efuse_reinit(ts_void)
{
	/* do nothing */
}

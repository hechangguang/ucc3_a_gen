/*
 * crc.h -- crc校验值计算函数
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2022-2025 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
*/

#ifndef __CRC_H__
#define __CRC_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#include <base/ts_type.h>

ts_u32 verify_lib_calc_crc8(ts_u32 u32_init_val, ts_u08 *pu8_inbuf, ts_u32 u32_size);

ts_u32 verify_lib_calc_crc32(ts_u32 u32_init_val, ts_u08 *pu8_inbuf, ts_u32 u32_size);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __CRC_H__ */
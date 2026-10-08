/* 
 * cpl_crc.h -- cpl crc 实现头文件,包括 32 和 8 位
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2022-2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __CPL_CRC_H__
#define __CPL_CRC_H__

#include <base/ts_type.h>


extern ts_u32 ts_cpl_crc32(ts_u32 u32_init_val, ts_u08 *pu8_inbuf, ts_u32 u32_size);
extern ts_u32 ts_cpl_crc8(ts_u32 u32_init_val, ts_u08 *pu8_inbuf, ts_u32 u32_size);
extern ts_u32 ts_cpl_crc16(ts_u32 u32_init_val, ts_u08 *pu8_inbuf, ts_u32 u32_size);

#endif /* __CPL_CRC_H__ */

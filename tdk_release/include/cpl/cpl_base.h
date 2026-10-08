/* 
 * cpl_base.h -- cpl base 实现头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2022-2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __CPL_BASE_H__
#define __CPL_BASE_H__

#include <base/ts_type.h>

extern ts_u32 ts_cpl_get_version(ts_void);
extern ts_u32 ts_cpl_get_lofft_size(ts_void);
extern ts_u32 ts_cpl_get_physt_size(ts_void);

extern ts_void ts_cpl_init(ts_void);

#endif /* __CPL_BASE_H__ */

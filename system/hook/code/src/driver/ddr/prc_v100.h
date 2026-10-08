/*
 * prc_v100.h -- prc_v100 驱动头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2022-2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __PRC_V100_H__
#define __PRC_V100_H__

#include <config.h>

#define PRC_OPT_TYPE_WR                 (0x02)
#define PRC_OPT_TYPE_RD                 (0x03)

#define PRC_REG_OFF_CA_L                (0x0008)
#define PRC_REG_OFF_CA_H                (0x000C)
#define PRC_REG_OFF_WDAT                (0x0010)
#define PRC_REG_OFF_RDAT                (0x0014)
#define PRC_REG_OFF_OPS                 (0x0018)

#define PRC_REG_OFF_DLL_LOCK_STATE      (0x0108)


#endif /* __PRC_V100_H__ */

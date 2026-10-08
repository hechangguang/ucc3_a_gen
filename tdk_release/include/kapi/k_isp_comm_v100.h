/* 
 * k_isp_comm_v100.h -- isp v100 驱动通用接口头文件，对用户可见
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023-2024 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __K_ISP_COMM_V100_H__
#define __K_ISP_COMM_V100_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <config.h>
#include <base/ts_type.h>
#include <base/ts_comm.h>
#include <base/ts_log.h>
#include <base/ts_errno.h>

#include <uapi/isp_v100.h>
#include <cpl/ts_cpl.h>

typedef struct ts_isp_dev_s {
    ts_u32 u32_regbase;

    /* 设备是否已经调用过配置函数 */
    ts_bool b_cfg;

    /* 设备当前状态 */
    ts_status_en_t en_status;
} ts_isp_dev_s_t;


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __K_ISP_COMM_V100_H__ */

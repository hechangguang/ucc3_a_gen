/* 
 * spi_v100.c -- spi_v100 实现头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __K_SPIMST_V100_H__
#define __K_SPIMST_V100_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <config.h>
#include <base/ts_type.h>
#include <base/ts_errno.h>

#include <uapi/spi_v100.h>
#include <cpl/ts_cpl.h>

typedef struct spi_dev_s {
    ts_u32 u32_register_base;
    ts_cpl_mutex_s_t st_mutex;

    ts_u08 u08_irq_no;
    ts_u08 u08_slv_cnt;
    ts_u16 u16_fifo_depth;

    /* 存放当前配置 */
    ts_spi_attr_s_t st_attr;
} spi_dev_s_t;


/*
 * 函数名 :  spi_tdl_xfer
 * 描  述 :  spimst驱动实现传输功能
 * 输  入 :  参数               描述
 *           pv_dev           spi-dev
 *           pst_msg            传输参数
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           TS_OK               正确传输
 *           TS_ERR             传输失败
 */
ts_s32 spi_tdl_xfer(spi_dev_s_t *pv_dev, ts_spi_msg_s_t *pst_msg);


/*
 * 函数名 :  spis_tdl_xfer
 * 描  述 :  spislv驱动实现传输功能
 * 输  入 :  参数               描述
 *           pst_dev           spis-dev
 *           pst_msg            传输参数
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           TS_OK               正确传输
 *           TS_ERR             传输失败
 */
ts_s32 spis_tdl_xfer(spi_dev_s_t *pst_dev, ts_spi_msg_s_t *pst_msg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __K_SPIMST_V100_H__ */

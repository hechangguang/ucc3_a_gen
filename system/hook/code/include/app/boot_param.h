/*
 * bootup_param.h -- 启动参数声明
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2022-2025 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
*/
#ifndef __BOOT_PARAM_H__
#define __BOOT_PARAM_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <base/ts_type.h>


/* 双启动参数在nand-flash中存放地址 */
#define BT_PARAM_A_ADDRESS (0x000A0000)
#define BT_PARAM_B_ADDRESS (0x000E0000)

/* 启动参数最大坏块跳过数 */
#define BT_PARAM_MAX_BAD_CNT (1)

/* 启动参数魔术字 */
#define BOOT_PARAM_MAGIC (0xE1D2C3B4)

typedef struct bt_param_s {
    /* 魔术字，必须为0xE1D2C3B4 */
    ts_u32 u32_magic;

    /* 当前启动分区 */
    ts_u32 u32_active_sys;

    /* 升级标志 */
    ts_u32 u32_upgr_flg;

    /* A分区健康标记 */
    ts_u32 u32_a_state;

    /* B分区健康标记 */
    ts_u32 u32_b_state;

    /* 启动参数crc值 */
    ts_u32 u32_crc;
    
    ts_u32 u32_rev[2];
} bt_param_s_t;

/* 镜像序号 */
#define A_IMG_INDEX                     (1)
#define B_IMG_INDEX                     (2)





/* 
 * 函数名 : set_bt_param
 * 描  述 : 向FLASH中写入
 * 输  入 : 参数            描述
 *          pst_boot_param    写入nand-flash的启动参数
 * 输  出 : 参数            描述
 *          无              无
 * 返  回 : 返回值         描述
 *          TS_OK         设置成功
 *          TS_ERR        设置失败
 */
ts_s32 set_bt_param(bt_param_s_t *pst_boot_param);


/* 
 * 函数名 : get_bt_param
 * 描  述 : 获取启动参数
 * 输  入 : 参数           描述
 *          无             无  
 * 输  出 : 参数           描述
 *          pst_param      读取启动参数
 * 返  回 : 返回值         描述
 *          TS_OK         读取成功
 *          TS_ERR        读取失败
 */
ts_s32 get_bt_param(bt_param_s_t *pst_param);



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /*__BOOT_PARAM_H__ */
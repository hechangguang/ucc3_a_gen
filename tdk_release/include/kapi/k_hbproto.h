/*
 * k_hbproto.h -- simple host bootup protocol 头文件， 用于主机与板端通讯命令字等定义
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2022-2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __K_HBPROTO_H__
#define __K_HBPROTO_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <base/ts_type.h>

/* 
 * 约定 
 * 使用数据量单位都使用 32bit 为单位
 */

/* --- 协议定义 ---  */
#define TS_HBP_INDEX_START      (0)
#define TS_HBP_INDEX_SUM        (1)
#define TS_HBP_INDEX_CMD        (2)
#define TS_HBP_INDEX_SIZE       (3)

#define TS_HBP_HEAD_MAX_SIZE    (4)
#define TS_HBP_TAIL_MAX_SIZE    (4)
#define TS_HBP_ATTR_MAX_SIZE    (8)


#define TS_HBP_PROTO_MAX_SIZE   (TS_HBP_HEAD_MAX_SIZE + TS_HBP_TAIL_MAX_SIZE + TS_HBP_ATTR_MAX_SIZE)
#define TS_HBP_DATA_MAX_SIZE    (2048)

#define TS_HBP_BUF_MAX_SIZE     (TS_HBP_PROTO_MAX_SIZE + TS_HBP_DATA_MAX_SIZE)

/* 协议头标志，为不可见字符 */
#define TS_HBP_START            (0x8F)


/* --- 命令字定义(大于或等于 128) ---  */

/* 回应类， >= 0x90 and <= 0x9f */  
#define TS_HBP_CMD_ACK_DONE         (0x90)
#define TS_HBP_CMD_ACK_BUSY         (0x91)
#define TS_HBP_CMD_ACK_ERROR        (0x92)
#define TS_HBP_CMD_ACK_UNSUP        (0x93)

/* 控制类， >= b0 and <= 0xcf */
#define TS_HBP_CMD_READ             (0xB0)
#define TS_HBP_CMD_WRITE            (0xB1)
#define TS_HBP_CMD_SAVE             (0xB2)
#define TS_HBP_CMD_ERASE            (0xB3)

#define TS_HBP_CMD_SET_SCRIPT       (0xC0)
#define TS_HBP_CMD_GET_SCRIPT       (0xC1)
#define TS_HBP_CMD_DO_CORRECTION    (0xC2)

/* 系统类, >= e0 and <= 0xef */ 
#define TS_HBP_CMD_REBOOT           (0xE0)

typedef struct hbp_head_s {
    ts_u08 u08_flag;
    ts_u08 u08_cmd;

    /* 1-byte 为单位 ， 最大 TS_HBP_DATA_MAX_SIZE, 用户不可见 */
    ts_u16 u16_size;
} hbp_head_s_t;


typedef struct hbp_tail_s {

    /* 开放，不用命令字有各自的属性或者配置, 参看各自的结构体 */
    ts_u08 au08_rev[3];

    /* 校验和取反 */
    ts_u08 u08_sum_n;
} hbp_tail_s_t;




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __K_HBPROTO_H__ */

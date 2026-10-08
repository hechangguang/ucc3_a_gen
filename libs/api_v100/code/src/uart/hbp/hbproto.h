/*
 * hbproto.h -- simple host bootup protocol 头文件， 用于主机与 bootup 通讯命令字等定义
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2022-2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __HBPROTO_H__
#define __HBPROTO_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <config.h>
#include <base/ts_type.h>

/* --- 协议定义 ---  */
#define TS_HBP_START            (0xA5)

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

/* --- 命令字定义 ---  */

/* 握手 */
#define TS_HBP_CMD_SAY_UP           (0x21)
#define TS_HBP_CMD_SAY_HI           (0x40)

/* 回应 */  
#define TS_HBP_CMD_ACK_DONE         (0x64)
#define TS_HBP_CMD_ACK_BUSY         (0x62)
#define TS_HBP_CMD_ACK_ERROR        (0x65)
#define TS_HBP_CMD_ACK_UNSUP        (0x75)  /* ACK Unsupport */

/* Read and Write data via internal buffer */
#define TS_HBP_CMD_READ             (0x52)
#define TS_HBP_CMD_WRITE            (0x57)
#define TS_HBP_CMD_SPIF_REG         (0x73)
#define TS_HBP_CMD_CRC              (0x43)
#define TS_HBP_CMD_ERASE            (0x45)

/* for sffs */
#define TS_HBP_CMD_SFFS_OPEN        (0x4F)
#define TS_HBP_CMD_SFFS_LIST        (0x4C)
#define TS_HBP_CMD_SFFS_FLUSH       (0x46)
#define TS_HBP_CMD_SFFS_CLOSE       (0x63)

/* Other */ 
#define TS_HBP_CMD_UPDATE_BP_DATA   (0x34)
#define TS_HBP_CMD_BP_COMPLETE_SIGN (0x33)
#define TS_HBP_CMD_GET_BP_ADDR      (0x32)
#define TS_HBP_CMD_GET_BP           (0x31)
#define TS_HBP_CMD_QUIT             (0x51)
#define TS_HBP_CMD_GOTO             (0x67)
#define TS_HBP_CMD_SET_CFG          (0x53)
#define TS_HBP_CMD_GET_CFG          (0x47)
#define TS_HBP_CMD_SET_PROTO_DEV    (0x44)

#define TS_HBP_CMD_SET_INDEX        (0x49)
#define TS_HBP_CMD_SET_INFO         (0x69)

#define TS_HBP_CMD_REBOOT           (0x42)  /* 使用看门狗复位 */

/* 帧内包类型定义 */
#define TS_HBP_FT_HEAD              (0)
#define TS_HBP_FT_ATTR              (1)
#define TS_HBP_FT_DATA              (2)
#define TS_HBP_FT_TAIL              (3)

/* 获取数据缓冲是否需要算上属性包 */
#define TS_HBP_IGNORE_ATTR          (0)
#define TS_HBP_NEED_ATTR            (1)

/* 协议方向 */
#define TS_HBP_DIR_READ             (0x00)
#define TS_HBP_DIR_SEND             (0x10)

/* 启动支持类型 */
#define BOOTUP_DEVTYPE_BUFFER       (0)
#define BOOTUP_DEVTYPE_RAM          (1)
#define BOOTUP_DEVTYPE_SPI_NOR      (2)
#define BOOTUP_DEVTYPE_SPI_NAND     (3)
#define BOOTUP_DEVTYPE_EMMC         (4)
#define BOOTUP_DEVTYPE_SDIO         (5)
#define BOOTUP_DEVTYPE_SDCARD       (6)
#define BOOTUP_DEVTYPE_PCI          (7)
#define BOOTUP_DEVTYPE_USB          (8)
#define BOOTUP_DEVTYPE_NET_TCP      (9)
#define BOOTUP_DEVTYPE_NET_UDP      (10)
#define BOOTUP_DEVTYPE_UART         (11)
#define BOOTUP_DEVTYPE_SPI          (12)
#define BOOTUP_DEVTYPE_SSBUS        (13)
#define BOOTUP_DEVTYPE_SFFS         (14)

#define BOOTUP_DEVTYPE_UNKONW       (-1)

typedef struct hbp_head_s {
    ts_u08 u08_flag;
    ts_u08 u08_cmd;

    /* 1-byte 为单位 ， 最大 TS_HBP_DATA_MAX_SIZE, 用户不可见 */
    ts_u16 u16_size;
} hbp_head_s_t;

typedef struct hbp_run_tail_s {
    union {
        struct  {
            ts_u32 u32_value : 24;
            ts_u32 u32_sum : 8;
        };
        ts_u32 u32_rev;
    };
} hbp_run_tail_s_t;


typedef struct hbp_index_tail_s {
    ts_u08 u08_type;
    ts_u08 u08_index;
    ts_u08 u08_rev;
    ts_u08 u08_sum;
} hbp_index_tail_s_t;


typedef struct hbp_caps_tail_s {
    ts_u08 u08_buf_size;
    ts_u08 u08_load;
    ts_u08 u08_dev;
    ts_u08 u08_sum;
} hbp_caps_tail_s_t;

typedef struct hbp_sffs_tail_s {
    union {
        struct  {
            ts_u32 u32_size : 24;
            ts_u32 u32_sum : 8;
        };
        ts_u32 u32_rev;
    };
} hbp_sffs_tail_s_t;


typedef struct hbp_tail_s {

    /* 开放，不用命令字有各自的属性或者配置, 参看各自的结构体 */
    ts_u08 au08_rev[3];

    /* 校验和取反 */
    ts_u08 u08_sum_n;
} hbp_tail_s_t;

/* 读写，擦除属性包 */
typedef struct hbp_rw_attr_s {
    ts_u32 u32_addr;

    union {
        struct  {
            ts_u32 u32_type : 8;    /* 读取的设备类型，对应是 BOOTUP_DEVTYPE_XXXX */
            ts_u32 u32_index : 4;   /* 设备索引，可以是 emmc 的 part , 也可以是 FLASH 的片选 */
            ts_u32 u32_bw : 4;      /* 读写的位宽，可以用此配置地址对齐， 使用定义 TS_BWXX */
            ts_u32 u32_size : 16;    /* 读写取个数，单位为 1byte */
        };
        ts_u32 u32_rev;
    };
} hbp_rw_attr_s_t;


typedef struct hbp_erase_attr_s {
    union {
        struct  {
            ts_u32 u32_type : 8;
            ts_u32 u32_addr : 24;
        };
        ts_u32 u32_rev;
    };

    ts_u32 u32_size;
} hbp_erase_attr_s_t;


typedef struct hbp_crc_buf_attr_s {
    ts_u32 u32_init_crc;

    union {
        ts_u32 u32_size;
        ts_u32 u32_calc_crc;
    };

} hbp_crc_buf_attr_s_t;


typedef struct hbp_run_attr_s {
    union {
        ts_u32 u32_addr;
        ts_u32 u32_wdt_ms;
    };

    ts_u32 u32_rev;
} hbp_run_attr_s_t;

typedef struct hbp_sfpi_w_arrt_s {
    ts_u08 au08_buf[6];
    ts_u08 u08_r_cnt;
    ts_u08 u08_w_cnt;
} hbp_sfpi_w_arrt_s_t;


typedef struct hbp_attr_s {
    union {
        hbp_rw_attr_s_t st_rw_attr;
        hbp_erase_attr_s_t st_erase_attr;
        hbp_crc_buf_attr_s_t st_crc_buf_attr;
        hbp_run_attr_s_t st_run_attr;
        hbp_sfpi_w_arrt_s_t st_spif_w_attr;
        ts_u32 au32_rev[2];
    };
} hbp_attr_s_t;

/* function list */


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __HBPROTO_H__ */

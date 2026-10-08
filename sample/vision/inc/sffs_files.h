/*
 * sffs_files.h -- 统一管理在 sfc-nand 上的文件名
 */
#ifndef __SFFS_FILES_H__
#define __SFFS_FILES_H__

#include <base/ts_type.h>
#include "api.h"

// 命令：sffs eall  可擦除文件系统
/* sffs 挂载根路径 */
#define SFFS_MNT_ROOT               "/mnt/"
#define SFFS_GEN_FULLNAME(file)    SFFS_MNT_ROOT file

#define FN_OCC_CFG                  SFFS_GEN_FULLNAME("base/fn_occ_cfg")
#define FN_OCC_FILE_SIZE            (1280 * 1024 * 4 + 4096)

#define FN_K_CFG                    SFFS_GEN_FULLNAME("base/fn_k_cfg")
#define FN_K_FILE_SIZE              (1280 * 1024 * 2 * 4 + 4096)

#define FN_B_CFG                    SFFS_GEN_FULLNAME("base/fn_B_cfg")
#define FN_B_FILE_SIZE              (1280 * 1024 * 2 * 8 + 4096)

#define FN_SFFC_CFG                 SFFS_GEN_FULLNAME("base/fn_sffc_cfg")
#define FN_SFFC_FILE_SIZE           (1280 * 1024 * 2 * 4 + 4096)

// #define FN_NPU_CFG                  SFFS_GEN_FULLNAME("base/fn_npu_cfg")
// #define FN_NPU_FILE_SIZE            (1024 * 1024 * 1)
#define FN_SR_CFG                   SFFS_GEN_FULLNAME("base/fn_sr_cfg")
#define FN_SR_FILE_SIZE             (1024 * 1024 * 1)

#define FN_AISP_CFG                 SFFS_GEN_FULLNAME("base/fn_aisp_cfg")
#define FN_AISP_FILE_SIZE           (1024 * 1024 * 1)


/* 保存在 super 区域的文件 */
#define FN_SUPER_TDK                SFFS_GEN_FULLNAME("cfg/tdk.bin")
#define FN_SUPER_ISP                SFFS_GEN_FULLNAME("cfg/isp.bin")
#define FN_SUPER_SNS                SFFS_GEN_FULLNAME("cfg/sns.bin")
#define FN_SUPER_USR                SFFS_GEN_FULLNAME("cfg/usr.bin")
#define FN_SUPER_CFG                SFFS_GEN_FULLNAME("cfg/config.bin")
#define FN_SUPER_LIFE               SFFS_GEN_FULLNAME("cfg/life.bin")
#define FN_SUPER_BP                 SFFS_GEN_FULLNAME("cfg/bp.bin")
#define FN_SUPER_EXT_BP             SFFS_GEN_FULLNAME("cfg/ext_bp.bin")


typedef struct usr_cfg_file_s {
    ts_char ac_name[32];
} usr_cfg_file_s_t;


/* #define FN_SUPER_CFG SFFS_GEN_FULLNAME("cfg/config.bin") 
此文件结构体
*/
#define TS_SCFG_MAGIC               (0x5a5aeeff)
#define TS_SCFG_INIT_USB_CLOSE      (0)
#define TS_SCFG_INIT_USB_LIBUSB     (1)
#define TS_SCFG_INIT_USB_UVC        (2)
typedef struct ts_scfg_s {
    /* 魔术字 */
    ts_u32 u32_magic;

    /* 版本号 */
    ts_u32 u32_version;

    /* smart 配置，最大 256 */
    ts_u32 u32_smart_size;
    ts_u32 au32_smart[256];

    /* vpo 配置 */
    ts_char ac_cfg[4][16];
    ts_u32 u32_cfg_num;

    /* 0: close; 1: libusb; 2: uvc */
    ts_u32 u32_usb_type;

    /* 挡片配置 */
    ts_s32 s32_max_gap;
    ts_s32 s32_cut_max_temp_diff;

    /* bt clk 配置:0,1 有效 */
    ts_u08 u08_bt_clk_cfg;

    /* bt data 配置:0,1 有效 */
    ts_u08 u08_bt_data_cfg;

    ts_u16 u16_rev;

    /* 自动挡片模式 */
    ts_s32 s32_max_gap_en;
    ts_s32 s32_cut_max_temp_diff_en;

    /* 机芯ID */
    ts_char ac_id[12];

    /* isp配置序号 */
    ts_s32 s32_isp_idx;

    /* 保留 */
    ts_u32 au32_rev[738];
} ts_scfg_s_t;


/* 区分各种 flash 配置
 * 0..7             8..15       16..23          24..31
 * outer_volume    outer_type   inner_volume    inner_type
 */
typedef ts_u32 ts_sffs_t;

#define TS_SFFS_GET_INNER_TYPE(hsffs)       (ts_u32)((hsffs <<  0) >> 24)
#define TS_SFFS_GET_INNER_VOL(hsffs)        (ts_u32)((hsffs <<  8) >> 24)
#define TS_SFFS_GET_OUTER_TYPE(hsffs)       (ts_u32)((hsffs << 16) >> 24)
#define TS_SFFS_GET_OUTER_VOL(hsffs)        (ts_u32)((hsffs << 24) >> 24)

#define SFFS_TYPE_NULL      (0)
#define SFFS_TYPE_SNAND     (1)
#define SFFS_TYPE_SNOR      (2)

#define TS_SFFS_SET(inner_type, inner_vol, outer_type, outer_vol)   ((((ts_u08)inner_type & 0xFF) << 24) | \
        (((ts_u08)inner_vol & 0xFF) << 16) | ((((ts_u08)outer_type & 0xFF) << 8) | ((ts_u08)outer_vol & 0xFF)))


/* inner: nand(128MB), outer: null */
#define SFFS_INAND_ONULL            TS_SFFS_SET(SFFS_TYPE_SNAND, 128, SFFS_TYPE_NULL, 0)

/* inner: nor(8MB), outer: null */
#define SFFS_INNOR_ONULL            TS_SFFS_SET(SFFS_TYPE_SNOR, 8, SFFS_TYPE_NULL, 0)

/* inner: nand(128MB), outer: snor(16MB) */
#define SFFS_INAND_ONOR_16MB        TS_SFFS_SET(SFFS_TYPE_SNAND, 128, SFFS_TYPE_SNOR, 16)

ts_void vision_sffs_init(ts_sffs_t hsffs);

#endif /* __SFFS_FILES_H__ */


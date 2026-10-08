/*
 * sffs_files.c -- vision 在 sfc-nand 上的文件列表，基于 sffs 文件系统
 */
#include <base/ts_type.h>
#include <base/ts_comm.h>
#include <base//ts_compiler.h>
#include <uapi/sfc_v100.h>
#include <uapi/gpio_v100.h>
#include <uapi/iomux.h>
#include <uapi/sfc_v100.h>
#include <uapi/spi_v100.h>
#include <cpl/ts_cpl.h>
#include <command.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "sffs_files.h"
#include "sffs_files_vot.h"
#include <api.h>


extern ts_void ts_lib_sfc_info(ts_u32 u32_index);
sfc_fs_file_s_t sst_inner_snand_files[] = {
    /* 文件名(pc_fname)     保持 0  文件最大大小(u32_size)      标志(u32_flag)                         读回调(read_cb) */
    {FN_OCC_CFG,            0,      FN_OCC_FILE_SIZE,          SFFS_FIXED_MAPPING | SFFS_EXCLUSIVE,    ts_null},
    {FN_K_CFG,              0,      FN_K_FILE_SIZE,            SFFS_FIXED_MAPPING | SFFS_EXCLUSIVE,    ts_null},
    {FN_B_CFG,              0,      FN_B_FILE_SIZE,            SFFS_FIXED_MAPPING | SFFS_EXCLUSIVE,    ts_null},
    {FN_SFFC_CFG,           0,      FN_SFFC_FILE_SIZE,         SFFS_FIXED_MAPPING | SFFS_EXCLUSIVE,    ts_null},
    {FN_SR_CFG,             0,      FN_SR_FILE_SIZE,           SFFS_FIXED_MAPPING | SFFS_EXCLUSIVE,    ts_null},
    {FN_AISP_CFG,           0,      FN_AISP_FILE_SIZE,         SFFS_FIXED_MAPPING | SFFS_EXCLUSIVE,    ts_null},
    
    {FN_APP_CFG,            0,      FN_APP_FILE_SIZE,          SFFS_FIXED_MAPPING | SFFS_EXCLUSIVE,    ts_null},
    {FN_APP_MENU2_CFG,      0,      FN_APP_MENU2_FILE_SIZE,    SFFS_FIXED_MAPPING | SFFS_EXCLUSIVE,    ts_null},
    {FN_VOT_ISP_CFG,        0,      FN_VOT_ISP_FILE_SIZE,      SFFS_FIXED_MAPPING | SFFS_EXCLUSIVE,    ts_null},
    {FN_VOT_NUC_KB_CFG,     0,      FN_VOT_NUC_KB_FILE_SIZE,   SFFS_FIXED_MAPPING | SFFS_EXCLUSIVE,    ts_null},
    {FN_MENU1_DEF_PARA_CFG, 0,      FN_MENU1_DEF_PARA_SIZE,    SFFS_FIXED_MAPPING | SFFS_EXCLUSIVE,    ts_null},
    {FN_MENU2_DEF_PARA_CFG, 0,      FN_MENU2_DEF_PARA_SIZE,    SFFS_FIXED_MAPPING | SFFS_EXCLUSIVE,    ts_null},
	{FN_MOTORSTORAGE_PARA_CFG, 0,   FN_MOTORSTORAGE_PARA_FILE_SIZE,   SFFS_FIXED_MAPPING | SFFS_EXCLUSIVE,    ts_null},
    {FN_VOT_TEMPPARA_CFG,   0,      FN_VOT_TEMPPARA_FILE_SIZE, SFFS_FIXED_MAPPING | SFFS_EXCLUSIVE,    ts_null},

    /* 小尺寸，双备份，写入频繁的配置文件, 所有文件加起来最大为 120KB */
    {FN_SUPER_TDK,         0,      512,                        SFFS_SUPER_SYNC | SFFS_IN_SUPER,         ts_null},
    {FN_SUPER_ISP,         0,      16 * 1024,                  SFFS_SUPER_SYNC | SFFS_IN_SUPER,         ts_null},
    {FN_SUPER_SNS,         0,      4 * 1024,                   SFFS_SUPER_SYNC | SFFS_IN_SUPER,         ts_null},
    {FN_SUPER_USR,         0,      8 * 1024,                   SFFS_SUPER_SYNC | SFFS_IN_SUPER,         ts_null},
    {FN_SUPER_CFG,         0,      9 * 1024,                   SFFS_SUPER_SYNC | SFFS_IN_SUPER,         ts_null},
    {FN_SUPER_LIFE,        0,      1 * 1024,                   SFFS_SUPER_SYNC | SFFS_IN_SUPER,         ts_null},
    {FN_SUPER_BP,          0,      12 * 1024,                  SFFS_SUPER_SYNC | SFFS_IN_SUPER,         ts_null},
    {FN_SUPER_EXT_BP,      0,      16 * 1024,                  SFFS_SUPER_SYNC | SFFS_IN_SUPER,         ts_null},
    {0}
};

sfc_fs_file_s_t sst_inner_snor_files[] = {
    /* 文件名(pc_fname)     保持 0  文件最大大小(u32_size)      标志(u32_flag)                         读回调(read_cb) */
    {FN_OCC_CFG,            0,      N6E_FN_OCC_FILE_SIZE,      SFFS_FIXED_MAPPING | SFFS_EXCLUSIVE,    ts_null}, //
    {FN_K_CFG,              0,      N6E_FN_K_FILE_SIZE,        SFFS_FIXED_MAPPING | SFFS_EXCLUSIVE,    ts_null},
    {FN_B_CFG,              0,      N6E_FN_B_FILE_SIZE,        SFFS_FIXED_MAPPING | SFFS_EXCLUSIVE,    ts_null},
    {FN_SFFC_CFG,           0,      N6E_FN_SFFC_FILE_SIZE,     SFFS_FIXED_MAPPING | SFFS_EXCLUSIVE,    ts_null},
    {FN_AISP_CFG,           0,      N6E_FN_AISP_FILE_SIZE,     SFFS_FIXED_MAPPING | SFFS_EXCLUSIVE,    ts_null},

    {FN_VOT_NUC_KB_CFG,     0,      N6E_FN_VOT_NUC_KB_FILE_SIZE,SFFS_FIXED_MAPPING | SFFS_EXCLUSIVE,   ts_null},


    /* 小尺寸，双备份，写入频繁的配置文件, 所有文件加起来最大为 120KB; 且内部不能使用 CRC 校验 */
    {FN_SUPER_TDK,         0,      512,                         SFFS_SUPER_SYNC | SFFS_IN_SUPER,        ts_null},
    {FN_SUPER_ISP,         0,      16 * 1024,                   SFFS_SUPER_SYNC | SFFS_IN_SUPER,        ts_null},
    {FN_SUPER_SNS,         0,      4 * 1024,                    SFFS_SUPER_SYNC | SFFS_IN_SUPER,        ts_null},
    {FN_SUPER_USR,         0,      8 * 1024,                    SFFS_SUPER_SYNC | SFFS_IN_SUPER,        ts_null},
    {FN_SUPER_CFG,         0,      9 * 1024,                    SFFS_SUPER_SYNC | SFFS_IN_SUPER,        ts_null},
    {FN_SUPER_LIFE,        0,      1 * 1024,                    SFFS_SUPER_SYNC | SFFS_IN_SUPER,        ts_null},
    {FN_SUPER_BP,          0,      12 * 1024,                   SFFS_SUPER_SYNC | SFFS_IN_SUPER,        ts_null},
    {FN_SUPER_EXT_BP,      0,      16 * 1024,                   SFFS_SUPER_SYNC | SFFS_IN_SUPER,        ts_null}, 

    {FN_APP_CFG,            0,      (1024 * 2 ),                 SFFS_IN_SUPER,       ts_null}, 
    {FN_APP_MENU2_CFG,      0,      (1024 * 2 ),                 SFFS_IN_SUPER,       ts_null},
    {FN_VOT_ISP_CFG,        0,      (1024 * 5 ),                 SFFS_IN_SUPER,       ts_null},
    {FN_MENU1_DEF_PARA_CFG, 0,      (1024 * 2 ),                 SFFS_IN_SUPER,       ts_null},
    {FN_MENU2_DEF_PARA_CFG, 0,      (1024 * 2 ),                 SFFS_IN_SUPER,       ts_null},
	{FN_MOTORSTORAGE_PARA_CFG,  0,  (1024 * 4 ),                 SFFS_IN_SUPER,       ts_null},
    {FN_VOT_TEMPPARA_CFG,   0,      (1024 * 2 ),                 SFFS_IN_SUPER,       ts_null},

    {0}
};


static sfc_fs_file_s_t sst_outer_snand_files[] = {
    /* 文件名(pc_fname)     保持 0  文件最大大小(u32_size)      标志(u32_flag)                          读回调(read_cb) */
    {0}
};


static sfc_fs_file_s_t sst_outer_snor_files[] = {
    /* 文件名(pc_fname)     保持 0  文件最大大小(u32_size)      标志(u32_flag)                          读回调(read_cb) */
    {0}
};

/* 用于保护 boot、sys 等镜像免于误擦除, 不能申请成 const , 需要可写权限开启和关闭使能 */
static sfc_protect_s_t sst_inner_snand_protect[] = {
    /* 是否有效配置     使能    (开始块地址  结束块地址):包括此块 */
    {1,                 1,      0,          3},     /* 保护所有 hook 分区, 0..0x7ffff */
    {1,                 1,      4,          11},    /* 保护主 img 0x80000..0x15ffff */
    {1,                 1,      12,         19},    /* 保护备份 img 0x180000..0x25ffff */
    {0}
};

static sfc_protect_s_t sst_inner_snor_protect[] = {
    /* 是否有效配置     使能    (开始块地址  结束块地址):包括此块 */
    {1,                 1,      0,          7},     /* 保护所有 hook 分区, 0..0x7ffff */
    {1,                 1,      8,          23},    /* 保护主 img 0x80000..0x15ffff */
    {1,                 1,      24,         39},    /* 保护备份 img 0x180000..0x25ffff */
    {0}
};


ts_void vision_sffs_init(ts_sffs_t hsffs)
{
    ts_u32 u32_inner_type, u32_inner_vol;
    ts_u32 u32_outer_type, u32_outer_vol;
    sffs_cfg_s_t st_inner, st_outer;
    sffs_cfg_s_t *pst_inner;
    sffs_cfg_s_t *pst_outer;

    TS_UNUSED(u32_inner_vol);
    TS_UNUSED(u32_outer_vol);

    u32_inner_type = TS_SFFS_GET_INNER_TYPE(hsffs);
    u32_inner_vol = TS_SFFS_GET_INNER_VOL(hsffs);
    u32_outer_type = TS_SFFS_GET_OUTER_TYPE(hsffs);
    u32_outer_vol = TS_SFFS_GET_OUTER_VOL(hsffs);

    /* 内部 Flash, 让驱动自动选择一个最大的频率 */
    st_inner.st_attr.u32_bus_clk = 0;
    st_inner.st_attr.u08_cs = 0;
    st_inner.st_attr.u08_rx_sample_dly = 0;
    st_inner.st_attr.u08_tmout_jfs = 200;
    if (u32_inner_type > 0) {
        if (SFFS_TYPE_SNAND == u32_inner_type) {
            printf("sffs inner SFFS_TYPE_SNAND \n");
            /* hook:6; cfg:6, app:12 = 24, 坏块率供应商反馈不超过 2%, 因此按照 5% 预留 */
            st_inner.u32_blk_cnt = 950;
            st_inner.u32_extra_blk_cnt = 50;
            st_inner.u32_offset_blk_addr = 24;
            st_inner.pv_fs = (ts_void *)&sst_inner_snand_files;
            st_inner.pv_protect = (ts_void *)&sst_inner_snand_protect;
    
            pst_inner = (sffs_cfg_s_t *)&st_inner;
        } else {
            printf("sffs inner SFFS_TYPE_SNOR \n");
            /* 按照 64KB 计算 */
            st_inner.u32_blk_cnt = 88;
            st_inner.u32_extra_blk_cnt = 0;

            /* 文件系统位于 3MB 开始的位置 */
            st_inner.u32_offset_blk_addr = 40;
            st_inner.pv_fs = (ts_void *)&sst_inner_snor_files;
            st_inner.pv_protect = (ts_void *)&sst_inner_snor_protect;
    
            pst_inner = (sffs_cfg_s_t *)&st_inner;
        }
    } else {
        printf("sffs inner is null\n");

        return;
    }

    if (u32_outer_type > 0) {
        /* 外部 Flash, 需配置管脚复用关系 */
        ts_lib_gpio_set_mux(TS_IO_SPI0_CLK, TS_IO_SPI0_CLK_FUN2);
        ts_lib_gpio_set_mux(TS_IO_SPI0_MOSI, TS_IO_SPI0_MOSI_FUN2);
        ts_lib_gpio_set_mux(TS_IO_SPI0_MISO, TS_IO_SPI0_MISO_FUN2);
        ts_lib_gpio_set_mux(TS_IO_SPI0_WP, TS_IO_SPI0_WP_FUN2);
        ts_lib_gpio_set_mux(TS_IO_SPI0_HOLD, TS_IO_SPI0_HOLD_FUN2);
        ts_lib_gpio_set_mux(TS_IO_SPI0_CSN0, TS_IO_SPI0_CSN0_FUN2);

        /* HOLD, WP, CSN 内部上拉 */
        ts_lib_gpio_set_pull(TS_IO_SPI0_WP, TS_ELEC_LVL_HI);
        ts_lib_gpio_set_pull(TS_IO_SPI0_HOLD, TS_ELEC_LVL_HI);
        ts_lib_gpio_set_pull(TS_IO_SPI0_MISO, TS_ELEC_LVL_HI);
        ts_lib_gpio_set_pull(TS_IO_SPI0_MOSI, TS_ELEC_LVL_HI);

        st_outer.st_attr.u32_bus_clk = 0;
        st_outer.st_attr.u08_cs = 0;
        st_outer.st_attr.u08_rx_sample_dly = 0;
        st_outer.st_attr.u08_tmout_jfs = 200;
        if (SFFS_TYPE_SNAND == u32_outer_type) {
            /* nand */
            st_outer.u32_blk_cnt = 950;
            st_outer.u32_extra_blk_cnt = 74;
            st_outer.u32_offset_blk_addr = 0;
            st_outer.pv_fs = (ts_void *)&sst_outer_snand_files;
        } else {
            /* nor */
            st_outer.u32_blk_cnt = 4096;
            st_outer.u32_extra_blk_cnt = 0;
            st_outer.u32_offset_blk_addr = 0;
            st_outer.pv_fs = (ts_void *)&sst_outer_snor_files;
        }

        st_outer.pv_protect = ts_null;

        pst_outer = (sffs_cfg_s_t *)&st_outer;
    } else {
        pst_outer = ts_null;
    }

    ts_lib_sfc_init(SFFS_MNT_ROOT, pst_inner, pst_outer);
}

static ts_void sffs_map_process(int argc, char **argv)
{
    ts_u32 u32_idx;

    if (argc > 0)
        u32_idx = ts_cpl_str2num((char *)argv[0]);
    else
        u32_idx = 0;

    if (u32_idx > 0)
        u32_idx = 1;

    ts_lib_sfc_info(u32_idx);
}


/* sffs 相关调试命令 */
TS_DECLARE_CMD(sffs)
{
    if (argc > 1) {
        if (0 == ts_cpl_strncmp("map", argv[1], 3)) {
            argc -= 2;
            argv += 2;

            sffs_map_process(argc, argv);
        } else if (0 == ts_cpl_strncmp("clr", argv[1], 3)) {
            ts_lib_sfc_clear();
        } else if (0 == ts_cpl_strncmp("eall", argv[1], 3)) {
            ts_lib_sfc_raw_erase(0x300000, 0x7d00000, 1);
        } else {

        }
    }

    return 0;
}
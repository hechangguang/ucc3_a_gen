/* 
 * vpo_v100.h -- vpo v100 驱动接口头文件
 */
#ifndef __API_VPO_V100_H__
#define __API_VPO_V100_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <base/ts_media.h>

/* 重定义 mdev 检测 */
#define vpo_chk_mdev(mdev)\
do{\
    if ((TS_MDEV_GET_MODID(mdev) != TS_IOC_MAGIC_VPO) || (TS_MDEV_GET_GRPID(mdev) >= TS_VPO_MAX_GRP_NUM) \
        || (TS_MDEV_GET_CHNID(mdev) >= TS_VPO_MAX_CHN_NUM)) {\
        ts_cpl_log(TS_LOG_ERR, gs32_tdk_log_lvl, __func__, __LINE__, "%s : %#x\n", TS_EINVALDEVID_STR, (ts_u32)mdev);\
        return -TS_EINVALDEVID;\
    }\
} while(0)


#define vpo_chk_grp(hgrp)\
do{\
    if (hgrp >= TS_VPO_MAX_GRP_NUM) {\
        ts_cpl_log(TS_LOG_ERR, gs32_tdk_log_lvl, __func__, __LINE__, "%s : %#x\n", TS_EINVALDEVID_STR, (ts_u32)hgrp);\
        return -TS_EINVALDEVID;\
    }\
} while(0)


/* 同步配置参数 */
typedef struct ts_comm_blanking_s {
    /* 场信号配置 */
    ts_u16 u16_vsync_bp;
    ts_u16 u16_vsync_per;
    ts_u16 u16_vsync_fp;

    /* 行信号配置 */
    ts_u16 u16_hsync_bp;
    ts_u16 u16_hsync_per;
    ts_u16 u16_hsync_fp;
} ts_comm_blanking_s_t;

/* ======== btenc 定义 ======== */
typedef enum ts_vpo_bte_bitmode_en {
    /* Y[15:8],UV[7:0] */
    TS_VPO_BTE_BITMODE_16B_MSB      = 0,

    /* UV[15:8],Y[7:0] */
    TS_VPO_BTE_BITMODE_16B_LSB      = 1,

    /* [7:0] Y->U->Y->V */
    TS_VPO_BTE_BITMODE_LO_8B_MSB    = 2,

    /* [7:0] U->Y->V->Y */
    TS_VPO_BTE_BITMODE_LO_8B_LSB    = 3,

    /* [15:8] Y->U->Y->V */
    TS_VPO_BTE_BITMODE_HI_8B_MSB    = 4,

    /* [15:8] U->Y->V->Y */
    TS_VPO_BTE_BITMODE_HI_8B_LSB    = 5,

    TS_VPO_BTE_BITMODE_BUTT
} ts_vpo_bte_bitmode_en_t;


typedef enum ts_vpo_bte_thr_en {
    /* [1:254] */
    TS_VPO_BTE_THR_WEAK = 0,
    /* [16:235] */
    TS_VPO_BTE_THR_STRONG = 1,
    /* [0:255] */
    TS_VPO_BTE_THR_NULL = 2,

    TS_VPO_BTE_THR_BUTT
} ts_vpo_bte_thr_en_t;

typedef enum ts_vpo_bte_io_remap_en {
    TS_VPO_BTE_IO_REMAP_DAT0 = 0,
    TS_VPO_BTE_IO_REMAP_DAT1 = 1,
    TS_VPO_BTE_IO_REMAP_DAT2 = 2,
    TS_VPO_BTE_IO_REMAP_DAT3 = 3,
    TS_VPO_BTE_IO_REMAP_DAT4 = 4,
    TS_VPO_BTE_IO_REMAP_DAT5 = 5,
    TS_VPO_BTE_IO_REMAP_DAT6 = 6,
    TS_VPO_BTE_IO_REMAP_DAT7 = 7,
    TS_VPO_BTE_IO_REMAP_DAT8 = 8,
    TS_VPO_BTE_IO_REMAP_DAT9 = 9,
    TS_VPO_BTE_IO_REMAP_DAT10 = 10,
    TS_VPO_BTE_IO_REMAP_DAT11 = 11,
    TS_VPO_BTE_IO_REMAP_DAT12 = 12,
    TS_VPO_BTE_IO_REMAP_DAT13 = 13,
    TS_VPO_BTE_IO_REMAP_DAT14 = 14,
    TS_VPO_BTE_IO_REMAP_DAT15 = 15,
    TS_VPO_BTE_IO_REMAP_VALID = 16,
    TS_VPO_BTE_IO_REMAP_HSYNC = 17,
    TS_VPO_BTE_IO_REMAP_VSYNC = 18
} ts_vpo_bte_io_remap_en_t;

typedef struct ts_vpo_bte_io_remap_attr_s {
    /* config number */
    ts_u32 u32_io_remap_num;

    /* detail config */
    ts_vpo_bte_io_remap_en_t *pst_cfg;
} ts_vpo_bte_io_remap_s_t;

/* 外同步配置参数 */
typedef struct ts_vpo_bte_exsync_s {
    /* 默认电平 */
    ts_elec_level_en_t en_def_lvl_v;
    ts_elec_level_en_t en_def_lvl_h;
    ts_elec_level_en_t en_def_lvl_de;
} ts_vpo_bte_exsync_s_t;

/* 内同步配置参数 */
typedef struct ts_vpo_bte_insync_s {
    ts_vpo_bte_thr_en_t en_thr;
    /* 0:0xff0000xy 1:0xffff00000000xyxy */
    ts_u32 u08_bit8_scode;
} ts_vpo_bte_insync_s_t;

typedef struct ts_vpo_sync_s {
    ts_vpo_bte_exsync_s_t st_ex_sync;
    ts_vpo_bte_insync_s_t st_in_sync;

    ts_u32 u08_redefine_timing_en;
    ts_u08 u08_padding_en;
    ts_u16 u16_output_width;
    ts_u16 u16_output_height;
    ts_u16 u16_rev0;
    ts_comm_blanking_s_t st_blanking_cfg;
} ts_vpo_sync_s_t;

/* 控制类属性定义 */
typedef struct ts_vpo_bte_attr_s {
    /* 使能测试数据输出 */
    ts_u08 u08_test_bar;

    /* rev0 */
    ts_u08 u08_rev0;

    /* 大小端设置, 配置参见 ts_vpo_bte_bitmode_en_t */
    ts_u08 u08_bit_mode;

    /* 内同步或者外同步 */
    ts_u08 u08_sync_mode;

    /* 同步配置 */
    ts_vpo_sync_s_t st_sync_cfg;
} ts_vpo_bte_attr_s_t;

/* ======== MIPI CSI2 定义 ======== */
typedef enum ts_vpo_mipi_csi2_datatype_en {
    TS_VPO_CSI2_DT_YUV422    = 0x1e,
    TS_VPO_CSI2_DT_RAW8      = 0x2a,
    TS_VPO_CSI2_DT_RAW16     = 0x2e,

    TS_VPO_CSI2_DT_BUTT
} ts_vpo_mipi_csi2_datatype_en_t;

typedef struct ts_vpo_mipi_csi2_attr_s {
    /* mipi data type , ref:ts_vpo_mipi_csi2_datatype_en_t */
    ts_u08 u08_data_type;

    /* using data lane number:1,2,4 */
    ts_u08 u08_dphy_lane_num;

    /* high speed transform data rate,unit:Mbps */
    ts_u16 u16_data_rate;

    /* dphy clock lane continue enable */
    ts_u08 u08_clock_continue;
    ts_u08 u08_dlane_o_strong;
    ts_u08 u08_clane_o_strong;

    /* 默认为 uyvy，符合 csi2 标准，用户可修改，兼容性无法保证 */
    ts_u08 u08_yuyv_en;
} ts_vpo_mipi_csi2_attr_s_t;

/* ======== MIPI DSI 定义 ======== */
typedef enum ts_vpo_mipi_dsi_datatype_en {
    TS_VPO_DSI_DT_24BIT = 0x3E,
    TS_VPO_DSI_DT_BUTT
} ts_vpo_mipi_dsi_datatype_en_t;


/* command mode type */
typedef enum ts_vpo_mipi_dsi_cmd_en {
    TS_VPO_DSI_DCS_WRITE,       // write cmd and parameter
    TS_VPO_DSI_CMD_DELAY,       // delay only
    TS_VPO_DSI_CMD_BUTT             
} ts_vpo_mipi_dsi_cmd_en_t;

typedef struct ts_vpo_mipi_dsi_cmd {
    ts_vpo_mipi_dsi_cmd_en_t ts_dsi_cmd_type;       // command type
    ts_u08 u08_cmd;                                 // dsi command
    ts_u08 u08_len;                                 // parameter length
    ts_u16 u16_rev0;                                // reserve
    ts_u08 *pu08_data;                        // parameter buffer
    ts_u32 u32_delay;                               // delay after current cmd,unit(ms)
} ts_vpo_mipi_dsi_cmd_t;

typedef struct ts_vpo_mipi_dsi_cmds {
    ts_vpo_mipi_dsi_cmd_t* ts_dsi_cmds;             // dcs command
    ts_u32 u32_cmds_len;                            // dcs command length
} ts_vpo_mipi_dsi_cmds_t;

typedef struct ts_vpo_mipi_dsi_attr_s {
    /* mipi data type , ref:ts_vpo_mipi_dsi_datatype_en_t */
    ts_u08 u08_data_type;

    /* using data lane number:1,2,4 */
    ts_u08 u08_dphy_lane_num;

    /* high speed transform data rate,unit:Mbps */
    ts_u16 u16_data_rate;

    /* dphy clock lane continue enable */
    ts_u08 u08_clock_continue;

    ts_u08 u08_dlane_o_strong;
    ts_u08 u08_clane_o_strong;

    /* dsi mode select: 0(video mode) 1(command mode)*/
    ts_u08 u08_dsi_mode;

    /* final output width and height */
    ts_u08 u08_padding_en;
    ts_u08 u08_mem_endian_inv;
    ts_u16 u16_output_width;
    ts_u16 u16_output_height;

    ts_u08 u08_padding_left;
    ts_u08 u08_padding_top;

    /* 同步配置 */
    ts_comm_blanking_s_t st_blanking_cfg;
} ts_vpo_mipi_dsi_attr_s_t;


/* info 配置 */
typedef struct vpo_info_attr_s {
    ts_void *pv_buf;
    ts_u32 u32_tm_ms_out;
} vpo_info_attr_s_t;

/* ======== LCD 定义 ======== */
typedef enum ts_vpo_lcd_datatype_en {
    TS_VPO_LCD_DT_RGB888 = 0,
    TS_VPO_LCD_DT_BUTT
} ts_vpo_lcd_datatype_en_t;

/* 同步配置参数 */
typedef struct ts_lcd_sync_s {
    /* 默认电平 */
    ts_elec_level_en_t vsync_en_def_lvl;
    ts_elec_level_en_t hsync_en_def_lvl;
    ts_elec_level_en_t valid_en_def_lvl;

    ts_comm_blanking_s_t st_blanking_cfg;
} ts_lcd_sync_s_t;

/* 控制类属性定义 */
typedef struct ts_vpo_lcd_attr_s {
    /* 使能测试数据输出 */
    ts_u08 u08_test_bar;

    /* 大小端设置, 配置参见 ts_vpo_lcd_bitmode_en_t */
    ts_u08 u08_data_type;

    /* valid flip enable */
    ts_u08 u08_valid_en;

    /* valid width and height */
    ts_u08 u08_padding_en;
    ts_u16 u16_output_width;
    ts_u16 u16_output_height;

    ts_u08 u08_padding_left;
    ts_u08 u08_padding_top;
    ts_u16 u16_rev;

    /* 同步配置 */
    ts_lcd_sync_s_t st_sync_cfg;
} ts_vpo_lcd_attr_s_t;

/* padding */
typedef struct ts_vpo_padding_attr_s {
    /* 只能由全局配置生效，此处只读 */
    ts_u32 u32_ro_en;

    /* 有效图像起始 left 位置 */
    ts_u32 u32_left;

    /* 有效图像起始 top 位置 */
    ts_u32 u32_top;

    /* 保留 */
    ts_u32 au32_rev[2];
} ts_vpo_padding_attr_s_t;

/* ======== cvbs 定义 ======== */
typedef struct ts_vpo_cvbs_attr_s {
    ts_u08 u08_test_bar;
    ts_u08 u08_bank_num;
    ts_u08 u08_short_delay_en;
    /* DAC drive strength,0~15 */
    ts_u08 u08_dac_drv_strength;
} ts_vpo_cvbs_attr_s_t;

/* ======== MEM 定义 ======== */
typedef struct ts_vpo_mem_attr_s {
    /* 配置范围 1..3 */
    ts_u08 u08_bank_num;

    /* 显示模式 */
    ts_u08 u08_mode;

    /* 是否启用填充功能， =0  取消， > 0 为填充的字节个数 */
    ts_u16 u16_fill_size;

    /* 启用填充功能后有效， 最终帧包结构为 = u16_fill_size + (u32_frm_size - u16_fill_size) */
    ts_u32 u32_frm_size;

    /* 0:bigEndian 1:littleEndian */
    ts_u08 u08_data_endian;
    ts_u08 u08_rev0;
    ts_u16 u16_rev0;
} ts_vpo_mem_attr_s_t;


typedef union chn_attr_un {
    ts_vpo_cvbs_attr_s_t st_cvbs_attr;
    ts_vpo_bte_attr_s_t st_bte_attr;
    ts_vpo_mem_attr_s_t st_mem_attr;
    ts_vpo_lcd_attr_s_t st_lcd_attr;
    ts_vpo_mipi_csi2_attr_s_t st_mipi_csi2_attr;
    ts_vpo_mipi_dsi_attr_s_t st_mipi_dsi_attr;
} chn_attr_un_t;


/* ======== 显示通道定义 ======== */
typedef struct ts_vpo_chn_attr_s {
    /* 显示宽高 */
    ts_u16 u16_disp_width;
    ts_u16 u16_disp_height;

    /* 显示帧率:
       when select cvbs channel,<=50.0 mean PAL otherwise is NTSC
    */
    ts_f32 f32_disp_fps;

    /* power on display mode enable*/
    ts_u08 u08_pod_en;
    ts_u08 u08_rev;
    ts_u16 u16_rev;

    /* 各个不同通道的属性配置 */
    chn_attr_un_t un_chn_attr;
} ts_vpo_chn_attr_s_t;


/* vpo 组属性结构体定义 */
typedef struct ts_vpo_grp_attr_s {
    /* 对源的剪切宽高， 必须是 2 的倍数, 0 : 不进行剪切 */
    ts_u16 u16_cut_width;
    ts_u16 u16_cut_height;

    /* 如果剪切宽高等于源宽高或者不进行剪切，则此配置不生效 */
    /* horizontal cut start point */
    ts_u16 u16_cut_start_h;
    /* vertical cut start point */
    ts_u16 u16_cut_start_v;

    /* yuv output channel 0/1 select 
     * channel 0 you can select:
     * TS_VPO_CVBS_MDEV
     * TS_VPO_NULL_MDEV
     * 
     * channel 1 you can select:
     * TS_VPO_LCD_MDEV
     * TS_VPO_BTENC_MDEV
     * TS_VPO_MIPI_CSI_MDEV
     * TS_VPO_MIPI_DSI_MDEV
     * TS_VPO_MEM_MDEV
     * TS_VPO_NULL_MDEV
     * 
     * when channel 1 select mipi dsi,channel 2 can select:
     * TS_VPO_BTENC_MDEV (8 bit mode)
     * else
     * TS_VPO_NULL_MDEV
     */
    ts_mdev_t u32_yuv_out_ch0_sel;
    ts_mdev_t u32_yuv_out_ch1_sel;
    ts_mdev_t u32_yuv_out_ch2_sel;

    /* raw output channel select 
     * TS_VPO_BTENC_MDEV
     * TS_VPO_MIPI_CSI_MDEV
     * TS_VPO_MEM_MDEV
     * TS_VPO_NULL_MDEV
     */
    ts_mdev_t u32_raw_out_ch_sel;

    /* vpo 内部内存缓存申请 bank 数, 最大 3 个, 如果输出和输入分辨率和帧率一致，
     * 且没有开启 cvbs 的情况下，不会使用此参数
     */
    ts_u08 u08_bank_num;

    /* 横向镜像使能 */
    ts_u08 u08_hmirr_en;

    /* ht info head enable */
    ts_u08 u08_head_en;
    ts_u08 u08_fire_en;
    ts_u16 u16_info_size;

    ts_u08 u08_rev0;

    /* 在 sr 和 翻转情况下，如果开启黑边，需要额外申请一部分内存 */
    ts_u08 u08_sr_pad_en;

    ts_u32 u32_info_addr;

    /* filter mode */
    /* include cvbs,0:design for integer scaling,1:suitable for all situations*/
    ts_u08 u08_ch0_filter_mode;

    /* include lcd,mipiDsi,0:deisgn for integer scaling,1:suitable for all situations*/
    ts_u08 u08_ch1_filter_mode;

    /* vertical mirror enable */
    ts_u08 u08_vmirr_en;

    /* include mipi csi2/bt channel,if en==1,only output y data */
    ts_u08 u08_digital_ch_yuv2y_en;

    /* let vpo prepare for super resolution */
    ts_u08 u08_super_resolution_en;
    
    /* step frame control , get 1 frame in (u08_step_frame_num+1) frames */
    ts_u08 u08_step_frame_num;

    ts_u08 u08_pod_en;

    /* force ts_npu_ops_en_t(aisp_v100.h) */
    ts_u08 u08_sr_type;

    /* 内存句柄，适用于 tmm , 如设置为 ts_null , 则从默认系统中申请 */
    ts_void *pv_tmm_zone;

    /* region zoom */
    ts_u08 u08_rz_en;
    /* 0:1X,1:2X,2:3X,3:4X */
    ts_u08 u08_rz_ratio;
    ts_u16 u16_rz_src_start_h;
    ts_u16 u16_rz_src_start_v;
    ts_u16 u16_rz_src_width;
    ts_u16 u16_rz_src_height;
    ts_u16 u16_rz_dst_start_h;
    ts_u16 u16_rz_dst_start_v;

    /* 初始 raw 输出位置，测温需要更改请使用 ts_tdk_vpo_set_templut_attr 接口 */
    ts_u08 u08_raw_pos;

    /* yuv and raw output sequence, 0: raw(left) and yuv(right), 1: yuv(left) and raw(right) */
    ts_u08 u08_digital_ch_yuv_raw_seq;

} ts_vpo_grp_attr_s_t;

/* 伪彩相关 */
typedef enum ts_vpo_pcolor_mode_en {
    /* 普通模式 */
    TS_VPO_PCOLOR_MODE_NORMAL = 0,

    /* 等温线模式 */
    TS_VPO_PCOLOR_MODE_ISO = 1,

    TS_VPO_PCOLOR_MODE_BUTT
} ts_vpo_pcolor_mode_en_t;

/* ROI 模式 */
typedef enum ts_vpo_pcolor_roi_en {
    /* 关闭 ROI */
    TS_VPO_PCOLOR_ROI_CLOSE = 0,

    /* 区域内为感兴趣区域 */
    TS_VPO_PCOLOR_ROI_INTERNAL = 1,

    /* 区域外为感兴趣区域 */
    TS_VPO_PCOLOR_ROI_EXTERNAL = 2,

    TS_VPO_PCOLOR_ROI_BUTT
} ts_vpo_pcolor_roi_en_t;

/* zoom  类型 */
typedef enum ts_vpo_zoom_en {
    /* 图像中心点 */
    TS_VPO_ZOOM_CENTER = 0,

    /* 图像左上角 */
    TS_VPO_ZOOM_LEFT_TOP = 1,

    /* 图像右上角 */
    TS_VPO_ZOOM_RIGHT_TOP = 2,

    /* 图像左下角 */
    TS_VPO_ZOOM_LEFT_BOTTOM = 3,

    /* 图像右下角 */
    TS_VPO_ZOOM_RIGHT_BOTTOM = 4,

    /* 感兴趣区域 */
    TS_VPO_ZOOM_ROI = 5,

    TS_VPO_ZOOM_BUTT
} ts_vpo_zoom_en_t;


typedef struct ts_vpo_pcolor_attr_s {
    /* 使能开关 */
    ts_u32 u32_enable;

    /* 模式, 仅支持标志位模式 */
    ts_vpo_pcolor_mode_en_t en_mode;

    /* 伪彩颜色配置，支持 256 个, alpha 会被驱动忽略 */
    ts_yuv_s_t *pst_pcolor;

    /* ROI */
    ts_vpo_pcolor_roi_en_t en_roi_mode;
    ts_u16 u16_roi_start_x;
    ts_u16 u16_roi_end_x;

    ts_u16 u16_roi_start_y;
    ts_u16 u16_roi_end_y;

    /* 保留位 */
    ts_u32 au32_rev[2];
} ts_vpo_pcolor_attr_s_t;

/* y16 obtain */
#define VPO_Y16_POS_NUM    (3)
typedef struct ts_vpo_y16_pos_s {
    ts_u16  u16_pos_x;
    ts_u16  u16_pos_y;
} ts_vpo_y16_pos_s_t;

typedef struct ts_vpo_y16_attr_s {
    ts_vpo_y16_pos_s_t          ast_pos[VPO_Y16_POS_NUM];       
    ts_u16                      u16_y16_val[VPO_Y16_POS_NUM];
    ts_u16                      u16_rev;
} ts_vpo_y16_attr_s_t;


/* 测温 raw 位置 */
typedef enum ts_lut_pos_en {
    TS_LUT_POS_BEFORE_AGC = 0,

    TS_LUT_POS_FINAL_RAW = 1,

    TS_LUT_POS_AFTER_BPC = 2,

    TS_LUT_POS_BEFORE_NUC = 3,

    TS_LUT_POS_BUTT
} ts_lut_pos_en_t;


/**
 * y = lut(x*mk+mb) + b1;
 * mk:U3.13,(8192==1.0)
 * mb:S16.0
 * b1:S15.0
 */
typedef struct ts_vpo_templut_attr_s {
    ts_u32                      u32_enable;
    ts_u16                      u16_mk;
    ts_u16                      u16_mb;
    ts_u16                      u16_b1;
    /**
     * 4096 depth and interpolate to 16384, lut can update when module is open.
     */
    ts_u08                      u08_lut_update_en;

    /* templut input raw select,0:linear 1:Nonlinear 2:after NUC and BPC 3:before NUC */
    ts_u08                      u08_source_sel;

    ts_u32                      u32_lut_base_addr;
} ts_vpo_templut_attr_s_t;


/* 放大属性 */
typedef struct ts_vpo_zoom_attr_s {
    /* 放大类型 */
    ts_vpo_zoom_en_t en_type;
    
    /* 感兴趣区域起始 x 坐标, 仅对 TS_VPO_ZOOM_ROI 生效 */
    ts_u16 u16_roi_x;

    /* 感兴趣的区域起始 y 坐标, 仅对 TS_VPO_ZOOM_ROI 生效 */
    ts_u16 u16_roi_y;

    /* 感兴趣的区域的宽,高； 仅对 TS_VPO_ZOOM_ROI 生效 */
    ts_u16 u16_roi_width;
    ts_u16 u16_roi_height;

    /* 放大或者缩小, 
     u16_zoom_out = 0: 放大;
     u16_zoom_out = 1: 缩小
     */
    ts_u16 u16_zoom_out;

    /* 1 代表 0.1， 10 代表原始尺寸 */
    ts_u16 u16_ratio;

    /* 是否进行水平镜像 */
    ts_u08 u08_hmirr_en;

    /* 是否进行垂直镜像 */
    ts_u08 u08_vmirr_en;

    /* 保留 */
    ts_u16 u16_rev0;
    ts_u32 u32_rev0;
} ts_vpo_zoom_attr_s_t;

/* --- function list --- */

/* 
 * 函数名 :  ts_tdk_vpo_set_grp_attr
 * 描  述 :  设置组属性
 * 输  入 :  参数               描述
 *          h_grp               vpo 组, 如 TS_VPO_GRP0, 具体定义参见 chip_def.h 中的 vpo 定义
 *          pst_attr            组属性结构体指针
 * 输  出 :  参数               描述
 *          无                  无
 * 返  回 :  返回值             描述
 *          TS_OK               设置组属性成功
 *          -TS_ENULLPTR        pst_attr 为空指针
 *          -TS_EINVALDEVID     h_grp 为非法组号
 *          -TS_ENODEV          vpo 设备未打开 
 *          TS_ERR              获取输入图像信息失败  
 */
ts_s32 ts_tdk_vpo_set_grp_attr(ts_grp_t h_grp, const ts_vpo_grp_attr_s_t *pst_attr);


/* 
 * 函数名 :  ts_tdk_vpo_get_grp_attr
 * 描  述 :  获取组属性
 * 输  入 :  参数               描述
 *          h_grp               vpo 组, 如 TS_VPO_GRP0, 具体定义参见 chip_def.h 中的 vpo 定义
 * 输  出 :  参数               描述
 *          pst_attr            组属性结构体指针
 * 返  回 :  返回值             描述
 *          TS_OK               获取组属性成功
 *          -TS_ENULLPTR        pst_attr 为空指针
 *          -TS_EINVALDEVID     h_grp 为非法组号
 *          -TS_ENODEV          vpo 设备未打开 
 */
ts_s32 ts_tdk_vpo_get_grp_attr(ts_grp_t h_grp, ts_vpo_grp_attr_s_t *pst_attr);


/* 
 * 函数名 :  ts_tdk_vpo_enable_grp
 * 描  述 :  设置组属性
 * 输  入 :  参数               描述
 *          h_grp               vpo 组, 如 TS_VPO_GRP0, 具体定义参见 chip_def.h 中的 vpo 定义
 * 输  出 :  参数               描述
 *          无                  无
 * 返  回 :  返回值             描述
 *          TS_OK               使能成功
 *          -TS_EINVALDEVID     h_grp 为非法组号
 *          -TS_ENODEV          vpo 设备未打开
 *          -TS_ENOTCFG         vpo 组未配置
 *          -TS_ENOMEM          申请内存失败
 */
ts_s32 ts_tdk_vpo_enable_grp(ts_grp_t h_grp);


/* 
 * 函数名 :  ts_tdk_vpo_disable_grp
 * 描  述 :  设置组属性
 * 输  入 :  参数               描述
 *          h_grp               vpo 组, 如 TS_VPO_GRP0, 具体定义参见 chip_def.h 中的 vpo 定义
 * 输  出 :  参数               描述
 *          无                  无
 * 返  回 :  返回值             描述
 *          -TS_ENOTSUP         不支持此项功能
 */
ts_s32 ts_tdk_vpo_disable_grp(ts_grp_t h_grp);


/* 
 * 函数名 :  ts_tdk_vpo_enable_chn
 * 描  述 :  使能指定 vpo 通道
 * 输  入 :  参数               描述
 *          h_vpo               vpo 通道, 如 TS_VPO_MDEV0, 具体定义参见 chip_def.h 中的 vpo 定义
 * 输  出 :  参数               描述
 *          无                  无
 * 返  回 :  返回值             描述
 *          TS_OK               使能成功
 *          TS_ERR              获取 vpo 组句柄失败
 *          -TS_EINVALDEVID     无效的通道号
 *          -TS_ENODEV          vpo 设备未打开
 *          -TS_ENOTCFG         vpo 组或者通道未配置
 *          -TS_ENOTSUP         不支持的通道号
 */
ts_s32 ts_tdk_vpo_enable_chn(ts_mdev_t h_vpo);


/* 
 * 函数名 :  ts_tdk_vpo_disable_chn
 * 描  述 :  禁止指定 vpo 通道
 * 输  入 :  参数               描述
 *          h_grp               vpo 组, 如 TS_VPO_GRP0, 具体定义参见 chip_def.h 中的 vpo 定义
 * 输  出 :  参数               描述
 *          无                  无
 * 返  回 :  返回值             描述
 *          TS_OK               禁止成功
 *          TS_ERR              获取 vpo 组句柄失败
 *          -TS_EINVALDEVID     无效的通道号
 *          -TS_ENODEV          vpo 设备未打开
 *          -TS_ENOTCFG         vpo 组未配置
 */
ts_s32 ts_tdk_vpo_disable_chn(ts_mdev_t h_vpo);


/* 
 * 函数名 :  ts_tdk_vpo_set_chn_attr
 * 描  述 :  设置指定 vpo 通道的属性配置
 * 输  入 :  参数               描述
 *          h_vpo               vpo 通道, 如 TS_VPO_MDEV0, 具体定义参见 chip_def.h 中的 vpo 定义
 *          pst_attr            属性配置指针
 * 输  出 :  参数               描述
 *          无                  无
 * 返  回 :  返回值             描述
 *          TS_OK               设置通道属性成功
 *          TS_ERR              获取 vpo 组句柄失败,或者获取输入图像信息失败
 *          -TS_ENULLPTR        pst_attr 为空指针
 *          -TS_EINVALDEVID     无效的通道号
 *          -TS_ENODEV          vpo 设备未打开
 *          -TS_ENOTCFG         vpo 组或者通道未配置
 *          -TS_ENOTSUP         不支持的通道号
 *          -TS_EINVAL          输入的属性参数非法
 */
ts_s32 ts_tdk_vpo_set_chn_attr(ts_mdev_t h_vpo, const ts_vpo_chn_attr_s_t *pst_attr);


/* 
 * 函数名 :  ts_tdk_vpo_get_chn_attr
 * 描  述 :  获取指定 vpo 通道的属性配置
 * 输  入 :  参数               描述
 *          h_vpo               vpo 通道, 如 TS_VPO_MDEV0, 具体定义参见 chip_def.h 中的 vpo 定义
 * 输  出 :  参数               描述
 *          pst_attr            属性配置指针,内存空间由用户申请
 * 返  回 :  返回值             描述
 *          TS_OK               获取通道属性成功
 *          TS_ERR              获取 vpo 组句柄失败或者通道配置错误
 *          -TS_ENULLPTR        pst_attr 为空指针
 *          -TS_EINVALDEVID     无效的通道号
 *          -TS_ENODEV          vpo 设备未打开
 */
ts_s32 ts_tdk_vpo_get_chn_attr(ts_mdev_t h_vpo, ts_vpo_chn_attr_s_t *pst_attr);


/* 
 * 函数名 :  ts_tdk_vpo_set_pcolor_attr
 * 描  述 :  设置指定 vpo 通道的伪彩属性
 * 输  入 :  参数               描述
 *          h_vpo               vpo 通道, 如 TS_VPO_MDEV0, 具体定义参见 chip_def.h 中的 vpo 定义
 *          pst_attr            伪彩属性指针
 * 输  出 :  参数               描述
 *          无                  无
 * 返  回 :  返回值             描述
 *          TS_OK               这是伪彩属性成功
 *          -TS_ENULLPTR        pst_attr 为空指针或者在 u32_enable 为 1 时 pst_pcolor 为空指针
 *          -TS_EINVALDEVID     无效的通道号
 *          -TS_ENODEV          vpo 设备未打开
 *          -TS_ENOINIT         vpo 组或者通道未初始化或者未配置
 */
ts_s32 ts_tdk_vpo_set_pcolor_attr(ts_mdev_t h_vpo, const ts_vpo_pcolor_attr_s_t *pst_attr);


/* 
 * 函数名 :  ts_tdk_vpo_get_pcolor_attr
 * 描  述 :  获取指定 vpo 通道的伪彩属性
 * 输  入 :  参数               描述
 *          h_vpo               vpo 通道, 如 TS_VPO_MDEV0, 具体定义参见 chip_def.h 中的 vpo 定义
 *          pst_attr            伪彩属性指针
 * 输  出 :  参数               描述
 *          无                  无
 * 返  回 :  返回值             描述
 *          TS_OK               获取伪彩属性成功
 *          -TS_ENULLPTR        pst_attr 为空指针
 *          -TS_EINVALDEVID     无效的通道号
 *          -TS_ENODEV          vpo 设备未打开
 *          -TS_ENOINIT         vpo 组或者通道未初始化或者未配置
 */
ts_s32 ts_tdk_vpo_get_pcolor_attr(ts_mdev_t h_vpo, ts_vpo_pcolor_attr_s_t *pst_attr);


/* 
 * 函数名 :  ts_tdk_vpo_get_pod_buf
 * 描  述 :  获取指定 vpo 通道的开机画面缓存
 * 输  入 :  参数               描述
 *          h_vpo               vpo 通道, 如 TS_VPO_MDEV0, 具体定义参见 chip_def.h 中的 vpo 定义
 * 输  出 :  参数               描述
 *          pst_buf              获取的开机画面缓存句柄
 * 返  回 :  返回值             描述
 *          TS_OK               获取开机画面缓存成功
 *          -TS_ENULLPTR        pst_buf 为空指针
 *          -TS_EINVALDEVID     无效的通道号
 *          -TS_ENODEV          vpo 设备未打开
 *          -TS_ENOTREADY       指定 vpo 通道未初始化
 *          TS_ERR              获取组句柄失败
 */
ts_s32 ts_tdk_vpo_get_pod_buf(ts_mdev_t h_vpo, ts_buffer_s_t *pst_buf);

/* 
 * 函数名 :  ts_tdk_vpo_get_grp_pod_buf
 * 描  述 :  获取指定 vpo 通道的开机画面缓存
 * 输  入 :  参数               描述
 *          h_grp              vpo 通道, 如 TS_VPO_MDEV0, 具体定义参见 chip_def.h 中的 vpo 定义
 * 输  出 :  参数               描述
 *          pst_buf            获取的开机画面缓存句柄
 * 返  回 :  返回值             描述
 *          TS_OK               获取开机画面缓存成功
 *          -TS_ENULLPTR        pst_buf 为空指针
 *          -TS_EINVALDEVID     无效的通道号
 *          -TS_ENODEV          vpo 设备未打开
 *          -TS_ENOTREADY       指定 vpo grp 未初始化
 *          TS_ERR              获取组句柄失败
 */
ts_s32 ts_tdk_vpo_get_grp_pod_buf(ts_grp_t h_grp, ts_buffer_s_t *pst_buf);


/* 
 * 函数名 :  ts_tdk_vpo_get_y16
 * 描  述 :  获取 y16 数据
 * 输  入 :  参数               描述
 *          无                  无
 * 输  出 :  参数               描述
 *          pst_buf              获取的 y16 数据
 * 返  回 :  返回值             描述
 *          TS_OK               获取 y16 成功
 *          -TS_ENULLPTR        pst_buf 为空指针
 *          -TS_ENODEV          vpo 设备未打开
 *          -TS_EINVAL          输入参数无效
 */
ts_s32 ts_tdk_vpo_get_y16(ts_vpo_y16_attr_s_t *pst_attr);


/* 
 * 函数名 :  ts_tdk_vpo_set_templut_attr
 * 描  述 :  设置温度矩阵属性
 * 输  入 :  参数               描述
 *          h_vpo               vpo 通道, 如 TS_VPO_MDEV0, 具体定义参见 chip_def.h 中的 vpo 定义
 *          pst_attr            测温矩阵属性
 * 输  出 :  参数               描述
 *          无                  无
 * 返  回 :  返回值             描述
 *          TS_OK               设置温度矩阵属性成功
 *          -TS_ENULLPTR        pst_attr 为空指针 u08_lut_update_en 为 1 时，u32_lut_base_addr 为空指针
 *          -TS_EINVALDEVID     无效的通道号
 *          -TS_ENODEV          vpo 设备未打开或者在 
 */
ts_s32 ts_tdk_vpo_set_templut_attr(ts_mdev_t h_vpo, const ts_vpo_templut_attr_s_t *pst_attr);


/* 
 * 函数名 :  ts_tdk_vpo_get_templut_attr
 * 描  述 :  获取温度矩阵属性
 * 输  入 :  参数               描述
 *          h_vpo               vpo 通道, 如 TS_VPO_MDEV0, 具体定义参见 chip_def.h 中的 vpo 定义
 * 输  出 :  参数               描述
 *          pst_attr            测温矩阵属性
 * 返  回 :  返回值             描述
 *          TS_OK               获取温度矩阵属性成功
 *          -TS_EINVALDEVID     无效的通道号
 *          -TS_ENODEV          vpo 设备未打开或者在 
 */
ts_s32 ts_tdk_vpo_get_templut_attr(ts_mdev_t h_vpo, ts_vpo_templut_attr_s_t *pst_attr);


/* 
 * 函数名 :  ts_tdk_vpo_set_info_buf
 * 描  述 :  设置附加信息缓冲，建议用户使用双缓冲进行操作，避免切换缓冲时出现错误数据
 * 输  入 :  参数               描述
 *          h_vpo               vpo 通道, 如 TS_VPO_MDEV0, 具体定义参见 chip_def.h 中的 vpo 定义
 *          pst_attr            info 参数结构体指针
 * 输  出 :  参数               描述
 *            无                无
 * 返  回 :  返回值             描述
 *          TS_OK               设置成功
 *          -TS_EINVALDEVID     无效的通道号
 *          -TS_ENODEV          vpo 设备未打开或者在 
 */
ts_s32 ts_tdk_vpo_set_info_buf(ts_mdev_t h_vpo, vpo_info_attr_s_t *pst_attr);


/* 
 * 函数名 :  ts_tdk_vpo_get_padding_attr
 * 描  述 :  获取 padding 的属性
 * 输  入 :  参数               描述
 *          h_vpo               vpo 通道, 如 TS_VPO_MDEV0, 具体定义参见 chip_def.h 中的 vpo 定义
 * 输  出 :  参数               描述
 *          pst_attr            padding 参数结构体指针
 * 返  回 :  返回值             描述
 *          TS_OK               获取成功
 *          -TS_EINVALDEVID     无效的通道号
 *          -TS_ENODEV          vpo 设备未打开或者在 
 */
ts_s32 ts_tdk_vpo_get_padding_attr(ts_mdev_t h_vpo, ts_vpo_padding_attr_s_t *pst_attr);


/* 
 * 函数名 :  ts_tdk_vpo_set_padding_attr
 * 描  述 :  设置 padding 的属性
 * 输  入 :  参数               描述
 *          h_vpo               vpo 通道, 如 TS_VPO_MDEV0, 具体定义参见 chip_def.h 中的 vpo 定义
 *          pst_attr            padding 参数结构体指针
 * 输  出 :  参数               描述
 *            无                无
 * 返  回 :  返回值             描述
 *          TS_OK               设置成功
 *          -TS_EINVALDEVID     无效的通道号
 *          -TS_ENODEV          vpo 设备未打开或者在 
 */
ts_s32 ts_tdk_vpo_set_padding_attr(ts_mdev_t h_vpo, ts_vpo_padding_attr_s_t *pst_attr);


/* 
 * 函数名 :  ts_tdk_vpo_cut_zoom_in
 * 描  述 :  通过 cut 功能实现电子放大,生效范围为整个 vpo 组
 * 输  入 :  参数               描述
 *          h_grp               vpo 组, 如 TS_VPO_GRP0, 具体定义参见 chip_def.h 中的 vpo 定义
 *          pst_attr            放大属性指针
 * 输  出 :  参数               描述
 *          无                  无
 * 返  回 :  返回值             描述
 *          TS_OK               设置剪切放大成功
 *          -TS_ENULLPTR        pst_buf 为空指针
 *          -TS_EINVALDEVID     无效组
 *          -TS_ENOTSUP         不支持的放大类型
 *          -TS_EINVAL          感兴趣区域输入参数错误、放大倍数超过范围
 *          -TS_ENODEV          vpo 设备未打开
 *          -TS_EINVAL          输入参数无效
 *          TS_ERR              获取输入图像信息失败  
 */
ts_s32 ts_tdk_vpo_cut_zoom_in(ts_grp_t h_grp, ts_vpo_zoom_attr_s_t *pst_attr);


/* 
 * 函数名 :  ts_tdk_vpo_freeze
 * 描  述 :  vpo 输出冻结
 * 输  入 :  参数               描述
 *          h_vpo               vpo 通道, 如 TS_VPO_MDEV0, 具体定义参见 chip_def.h 中的 vpo 定义
 * 输  出 :  参数               描述
 *          无                  无
 * 返  回 :  返回值             描述
 *          TS_OK               冻结成功
 *          -TS_EINVALDEVID     无效通道
 *          -TS_ENODEV          vpo 设备未打开
 */
ts_s32 ts_tdk_vpo_freeze(ts_mdev_t h_vpo);


/* 
 * 函数名 :  ts_tdk_vpo_unfreeze
 * 描  述 :  解除 vpo 输出冻结
 * 输  入 :  参数               描述
 *          h_vpo               vpo 通道, 如 TS_VPO_MDEV0, 具体定义参见 chip_def.h 中的 vpo 定义
 * 输  出 :  参数               描述
 *          无                  无
 * 返  回 :  返回值             描述
 *          TS_OK               冻结解除成功
 *          -TS_EINVALDEVID     无效通道
 *          -TS_ENODEV          vpo 设备未打开
 */
ts_s32 ts_tdk_vpo_unfreeze(ts_mdev_t h_vpo);

/* 
 * 函数名 :  ts_tdk_vpo_bte_cfg_io_remap
 * 描  述 :  remap bt encoder io
 * 输  入 :  参数               描述
 *          h_vpo               vpo 通道, 如 TS_VPO_MDEV0, 具体定义参见 chip_def.h 中的 vpo 定义
 *          pst_io_remap        io 位置配置
 * 输  出 :  参数               描述
 *          无                  无
 * 返  回 :  返回值             描述
 *          TS_OK               config ok
 */
ts_s32 ts_tdk_vpo_bte_cfg_io_remap(ts_mdev_t h_vpo, ts_vpo_bte_io_remap_s_t* pst_io_remap);


/* 
 * 函数名 :  ts_tdk_vpo_adj_cvbs_pos
 * 描  述 :  调整 cvbs 输出位置，确保中心点与数字中心点位置重叠
 * 输  入 :  参数               描述
 *          无                  无
 * 输  出 :  参数               描述
 *          pu32_line_cnt       行起始有效计数
 *          pu32_line_hb_cnt    最后半行消隐计数
 * 返  回 :  返回值             描述
 *          TS_OK               设置成功 ok
 */
ts_s32 ts_tdk_vpo_get_cvbs_pix_pos(ts_u32 *pu32_line_cnt, ts_u32 *pu32_line_hb_cnt);


/* 
 * 函数名 :  ts_tdk_vpo_adj_cvbs_pos
 * 描  述 :  调整 cvbs 输出位置，确保中心点与数字中心点位置重叠
 * 输  入 :  参数               描述
 *          u32_line_cnt       行起始有效计数， > 0 生效
 *          u32_line_hb_cnt    最后半行消隐计数， > 0 生效
 * 输  出 :  参数               描述
 *          无                  无
 * 返  回 :  返回值             描述
 *          TS_OK              设置成功
 */
ts_s32 ts_tdk_vpo_adj_cvbs_pix_pos(ts_u32 u32_line_cnt, ts_u32 u32_line_hb_cnt);


/* 
 * 函数名 :  ts_tdk_vpo_mipi_dcs_write
 * 描  述 :  write dsi command
 * 输  入 :  参数               描述
 *          h_vpo               vpo 通道, 如 TS_VPO_MDEV0, 具体定义参见 chip_def.h 中的 vpo 定义
 * 输  出 :  参数               描述
 *          无                  无
 * 返  回 :  返回值             描述
 *          TS_OK               config ok
 */
ts_s32 ts_tdk_vpo_mipi_dcs_write(ts_mdev_t h_vpo, const ts_vpo_mipi_dsi_cmd_t* pst_dsi_cmd);
ts_s32 ts_tdk_vpo_mipi_dcs_burst_write(ts_mdev_t h_vpo, const ts_vpo_mipi_dsi_cmds_t* pst_dsi_cmds);
ts_s32 ts_tdk_vpo_mipi_dsi_mode_sel(ts_mdev_t h_vpo, ts_u08 u08_mode);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __API_VPO_V100_H__ */


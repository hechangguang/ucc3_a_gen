/*
 * snr_comm.h -- vision 探测器公共头文件
 */
#ifndef __SNR_COMM_H__
#define __SNR_COMM_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <base/ts_type.h>
#include <base/ts_compiler.h>
#include <uapi/isp_v100.h>
#include <uapi/aisp_v100.h>
#include <uapi/viu_v100.h>
#include <uapi/snr_v100.h>
#include "sffs_files.h"

#define SNR_MAX_NAME        (32)

/* 计算 ISP 时钟, 对齐到 1M */
#define SNR_CALC_ISP_DIV(pll,w,h,f)     (pll / ((w + 80) * (h + 40) * f))
#define SNR_CALC_ISP_MDIV(pll,w,h,f)    ((pll / 1000000) / SNR_CALC_ISP_DIV(pll,w,h,f))

/* (w + 64) * (h + 32) * fps <= u32_isp_clk_hz */
#define SNR_CALC_ISP_CLK(pll,w,h,f)     (SNR_CALC_ISP_MDIV(pll,w,h,f) * 1000000)

/* (w + 64) * (h + 32) * fps <= u32_isp_clk_hz */
#define VPO_CALC_BT_CLK(pll,w,h,f)     (SNR_CALC_ISP_CLK(pll,w,h,f))

/* 探测器功能配置 */

#define SNR_CAPS_NULL               (0)

/* 探测器使能 CVBS 输出短延时，探测器输入需要 50 帧以上 */
#define SNR_CAPS_CVBS_SD_EN         (1 << 0)


/* 探测器控制相关配置 */
typedef struct tdk_sensor_cfg_s {
    /* 电源, -1 表示无需程序控制 */
    ts_s32 s32_pwr_gpio_num;

    /* 电源开关电平配置，初始化完成后，默认设置成 en_pwr_off_lvl 状态, 开状态为 en_pwr_off_lvl 的取反;
        TS_ELEC_LVL_LO or TS_ELEC_LVL_HI
     */
    ts_elec_level_en_t en_pwr_off_lvl;

    /* 复位管脚与复位时间配置，时间单位为 us; 管脚 -1 表示无需程序控制 */
    ts_s32 s32_rst_gpio_num;
    ts_u32 u32_rst_dly;

    /* 复位电平配置，初始化完成后，默认设置成 en_reset_lvl 状态, 释放复位为 en_reset_lvl 的取反
      TS_ELEC_LVL_LO or TS_ELEC_LVL_HI
     */
    ts_elec_level_en_t en_reset_lvl;

    /* shutter control mode 1, -1 表示无需程序控制, 默认输出为 0 */
    ts_s32 s32_sht_p_gpio_num;
    ts_s32 s32_sht_n_gpio_num;

    /* shutter control mode 2 , when (-1 == s32_sht_p_gpio_num) && (-1 == s32_sht_n_gpio_num) will use this mode */
    ts_s32 s32_sht_en_gpio_num;
    ts_s32 s32_sht_ctrl_gpio_num;
    ts_elec_level_en_t en_sht_open_lvl;

    /* 开门开关保持时间, 单位为 us */
    ts_u32 u32_sht_hold;

    /* 是否输出快门开关的提示 */
    ts_u32 u32_sht_dbg;
} tdk_sensor_cfg_s_t;


typedef struct sfc_cfg_sns_s 
{
    ts_u32 au32_smart_data[64];
    ts_u32 u32_adc_gain;
} sfc_cfg_sns_s_t;


/* snr 线序 */
typedef struct snr_remap_s {
    ts_u32 u32_i_order_hi;
    ts_u32 u32_i_order_lo;
    ts_u32 u32_o_order_hi;
    ts_u32 u32_o_order_lo;
} snr_remap_s_t;


/* CVBS 输出属性 */
typedef struct snr_cvbs_attr_s {
    /* 0: 关闭 CVBS 输出; > 0: 开启 CVBS 输出 */
    ts_u08 u08_en;

    /* 0: 关闭 CVBS 短延时; > 0: 开启 CVBS 短延时 */
    ts_u08 u08_sd_en;

    /* 0: 关闭 CVBS 原始尺寸输出; > 0: 开启 CVBS 原始尺寸输出 */
    ts_u08 u08_orgi_en;

    /* 0: NTSC 制式; > 0: PAL 制式 */
    ts_u08 u08_pal_en;
} snr_cvbs_attr_s_t;


/* BTE 输出属性, 固定逐行输出 */
typedef struct snr_bte_attr_s {
    /* 0: 关闭 BTE 输出; > 0: 开启 BTE 输出 */
    ts_u08 u08_en;

    /* 0: 输出 16 比特(LSB); > 0: 输出低 8 比特(LSB) */
    ts_u08 u08_8bit_en;

    /* 0: 内同步; > 0: 外同步 */
    ts_u08 u08_external_sync_en;

    ts_u08 u08_redefine_timing_en;
    ts_u16 u16_vsync_bp;
    ts_u16 u16_vsync_per;
    ts_u16 u16_vsync_fp;
    ts_u16 u16_hsync_bp;
    ts_u16 u16_hsync_per;
    ts_u16 u16_hsync_fp;
    ts_u16 u16_output_width;
    ts_u16 u16_output_height;
} snr_bte_attr_s_t;


/* mipi 输出属性 */
typedef struct snr_mipi_attr_s {
    /* 0: 关闭 MIPI 输出; > 0: 开启 MIPI 输出 */
    ts_u08 u08_en;

    /* 0: csi; > 0: dsi */
    ts_u08 u08_dsi_en;

    /* 参见 vpo_v100.h 中 ts_vpo_mipi_csi2_datatype_en_t 定义  */
    ts_u08 u08_data_type;

    /* using data lane number: 1,2,4 */
    ts_u08 u08_dphy_lane_num;

    /* dphy clock lane continue enable */
    ts_u08 u08_clock_continue;

    ts_u08 u08_mem_endian;

    /*high speed transform data rate,unit:Mbps  
    MIPI的传输速率根据传输的数据率和data lane的使用数据计算，
    以640x512 50fps单传yuyv使用2lane为例，datarate = 640*512*50*16/2/0.8 ≈ 164Mbps/lane，
    配置164即可。（0.8是保留的余量，如果帧消隐占比很大的话可以再调小点）。
    最小值不能小于 100MHZ
    */
    ts_u16 u16_data_rate;

    /* dsi timing config */
    ts_f32 f32_dsi_disp_fps;
    ts_u16 u16_dsi_disp_width;
    ts_u16 u16_dsi_disp_height;
    ts_u16 u16_dsi_vsync_bp;
    ts_u16 u16_dsi_vsync_per;
    ts_u16 u16_dsi_vsync_fp;
    ts_u16 u16_dsi_hsync_bp;
    ts_u16 u16_dsi_hsync_per;
    ts_u16 u16_dsi_hsync_fp;

    /* output width and height */
    ts_u16 u16_dsi_output_width;
    ts_u16 u16_dsi_output_height;

    /* data lane 输出强度 */
    ts_u08 u08_dlane_o_strong;

    /* clk lane 输出强度 */
    ts_u08 u08_clane_o_strong;

    /* 指定补黑边的起始位置 */
    ts_u08 u08_padding_left;
    ts_u08 u08_padding_top;
} snr_mipi_attr_s_t;


/* display 输出属性 */
typedef struct snr_lcd_attr_s {
    /* 0: 关闭 lcd 输出; > 0: 开启 lcd 输出 */
    ts_u08 u08_en;

    /* display timing config */
    ts_f32 f32_disp_fps;
    ts_u16 u16_disp_width;
    ts_u16 u16_disp_height;
    ts_u16 u16_vsync_bp;
    ts_u16 u16_vsync_per;
    ts_u16 u16_vsync_fp;
    ts_u16 u16_hsync_bp;
    ts_u16 u16_hsync_per;
    ts_u16 u16_hsync_fp;
} snr_lcd_attr_s_t;


/* NPU 属性 */
typedef struct snr_npu_attr_s {
    /* npu 应用 */
    ts_npu_ops_en_t en_ops;

    /* npu 时钟 */
    ts_u32 u32_clk;

    /* 如果是 90 度旋转，设置旋转方向, 0: 顺时针; >0: 逆时针 */
    ts_u08 u08_rotate_dir;

    /* 是否开启 ROI, 仅在超分模式下生效，ROI 的坐标必须 8 对齐，并且宽高限制到输入宽高的一半 */
    ts_u08 u08_roi_en;

    /* 必须 8 对齐 */
    ts_u16 u16_roi_left;
    ts_u16 u16_roi_top;

    /* trigger 模式使能 */
    ts_u08 u08_trig_en;

    /* 补黑边大小 */
    ts_u08 u08_pad_lines;
} snr_npu_attr_s_t;


/* 存放支持的探测器和分辨率 */
typedef struct snr_attr_s {
    /* 当前探测器名称,内部保存 */
    const ts_char *pc_name;
    
    /* 输入图像宽高、帧率 */
    ts_s32 s32_img_w;
    ts_s32 s32_img_h;
    ts_s32 s32_fps;

    /* 输出宽高 */
    ts_s32 s32_vo_img_w;
    ts_s32 s32_vo_img_h;

    /* 是否输出 raw, y */
    ts_u08 u08_raw_oe;
    ts_u08 u08_raw_pos;
    ts_u08 u08_y_oe;

    /* 0: raw(left) and yuv(right), 1: yuv(left) and raw(right) */
    ts_u08 u08_raw_seq;

    /* 是否水平镜像 */
    ts_u08 u08_hmirr_en;
    ts_u08 u08_vmirr_en;

    /* 是否开启 info 以及行数 */
    ts_u08 u08_info_en;
    ts_u08 u08_info_line_num;

    /* info mem */
    ts_void *pv_info_buf;

    /* 是否有开机画面, 0: 则关闭开机画面， > 0 则开启开机画面，并延时指定 ms 时间 */
    ts_u32 u32_pod_dly_ms;
    ts_u32 u32_pod_done;

    /* npu 配置 */
    snr_npu_attr_s_t st_npu;

    /* cvbs 通道配置 */
    snr_cvbs_attr_s_t st_cvbs;

    /* mem 通道配置 */
    ts_u32 u32_mem_en;

    /* bte 通道配置 */
    snr_bte_attr_s_t st_bte;

    /* mipi 通道配置 */
    snr_mipi_attr_s_t st_mipi;

    /* occ 复位默认值 */
    ts_u32 u32_ooc_rst_val;

    /* 获取 smart 配置大小 */
    ts_u32 (*get_snr_cfg_size)(ts_void);

    /* 启动入口以及启动入参 */
    ts_void (*start_snr)(ts_u32);
    ts_u32 u32_start_param;

    /* 等待探测器正常输出图像 */
    ts_s32 (*wait_snr)(ts_u32);

    /* 获取温度 */
    ts_s32 (*get_temperature)(ts_void);

    /* 挡片线程 */
    ts_cpl_kthread_t cut_process;
    ts_void *pv_cut_pdata;

    ts_snr_cut_cfg_s_t st_cut_ctx;
} snr_attr_s_t;


/* 探测器上下文 */
typedef struct snr_ctx_s {
    /* 探测器名称和宽高等配置 */
    snr_attr_s_t st_attr;

    /* 包括快门、复位、电源控制 */
    tdk_sensor_cfg_s_t st_io_cfg;

    /* 配置地址 */
    ts_u32 u32_isp_cfg_addr;

    /* occ calc enable when cut */
    ts_u32 u32_calc_occ_when_cut;

    /* 配置句柄 */
    sfc_cfg_file_s_t *pst_isp_cfg;

    /* tdk 文件 mmap 句柄 */
    tdk_cfg_s_t *pst_tdk_map;

    /* flash 类型 */
    ts_u08 u08_flash_type;

    ts_u08 u08_rev;
    ts_u16 u16_rev;

    /* 用于生产信息读取标志 */
    ts_u32 u32_tdk_pr_en;

    /* SNS 文件 mmap 句柄， 用探测器配置保存，各个探测器可能保存的数据完全不同 */
    sfc_cfg_file_s_t *pst_sns_map;

    /* 系统配置句柄，简单应用下，统一保存包括探测器、快门等相关配置 */
    ts_u32 u32_sync_scfg;
    ts_scfg_s_t *pst_scfg;

    /* 温补句柄 */
    ts_handle_t h_sl;
    ts_u32 u32_sl_en;

    /* 运行状态 */
    __IOM ts_s32 s32_run_isp;
    __IOM ts_s32 s32_run_auto_cut;
    __IOM ts_s32 s32_run_auto_b;
    __IOM ts_s32 s32_run_pause;
    __IOM ts_s32 s32_force_status;
    __IOM ts_s32 s32_force_cut;

    /* 锅盖算法: 0: 默认算法，为 TDK 最初的配置算法；  1: 新算法； */
    __IOM ts_s32 s32_sffc_alg;

    __IOM ts_s32 s32_fix_snc_trig;
    __IOM ts_s32 s32_fix_snc_done;
    __IOM ts_s32 s32_fix_snc_en;
} snr_ctx_s_t;

typedef struct ts_sffc_adj_s {
    ts_s32 s32_temp;
    ts_s32 s32_coef;
} ts_sffc_adj_s_t;

/* 注册与用户相关联探测器 */
typedef struct snr_usr_s {
    const ts_char *pc_name;
    ts_void (*start_snr)(ts_u32);
} snr_usr_s_t;

#define SNR_SECTION         __attribute__((section(".snr")))
#define snr_register(name, start_func)  \
        volatile const static SNR_SECTION snr_usr_s_t _snr_##name = {    \
            .pc_name = #name,           \
            .start_snr = start_func,    \
        };

/* 全局上下文 */
extern snr_ctx_s_t gst_snr_ctx;

/* 全局探测器 smart 配置 */
extern ts_viu_snr_cfg_s_t gast_viu_snr_cfg[];

/* 全局 viu 探测器属性配置 */
extern ts_viu_snr_attr_s_t gst_viu_snr_attr;

/* 用于管脚复用等 */
ts_void tdk_snrcfg_init(tdk_sensor_cfg_s_t *pst_cfg);
ts_s32 snr_gpio_cfg2out(ts_s32 s32_gpio_num, ts_elec_level_en_t en_o_def);
ts_void snr_gpio_set_out_lvl(ts_s32 s32_gpio_num, ts_elec_level_en_t en_o_lvl);

/* 对探测器进行电源配置，上电或者下电 */
ts_void tdk_snrcfg_pwr_on(ts_void);
ts_void tdk_snrcfg_pwr_off(ts_void);

/* 对探测器进行复位*/
ts_void tdk_snrcfg_reset(ts_void);

/* 开关快门 */
ts_void vot_templut_shutter_open(ts_void);
ts_void tdk_snrcfg_shutter_open(ts_void);
ts_void vot_templut_shutter_close(ts_void);
ts_void tdk_snrcfg_shutter_close(ts_void);

/* i2c 接口 */
ts_s32 snr_open_i2c_dev(ts_char *pc_devname, ts_u08 u08_dev_addr);
ts_s32 snr_i2c_getreg(ts_u08 u08_regaddr, ts_u08 *pu08_data);
ts_void snr_i2c_setreg(ts_u08 u08_regaddr, ts_u08 u08_data);
ts_s32 snr_i2c_get_r16d08(ts_u32 u32_reg_addr, ts_u08 *pu08_data);
ts_void snr_i2c_set_r16d08(ts_u32 u32_reg_addr, ts_u08 u08_data);
ts_s32 snr_i2c_get_r08d16(ts_u32 u32_reg_addr, ts_u16 *pu16_data);
ts_void snr_i2c_set_r08d16(ts_u32 u32_reg_addr, ts_u16 u16_data);
ts_s32 snr_i2c_get_r16d08_continue(ts_u32 u32_reg_addr, ts_u08 *pu08_data, ts_u32 u32_cnt);
ts_void snr_i2c_set_r16d08_continue(ts_u32 u32_reg_addr, ts_u08 *pu08_data, ts_u32 u32_cnt);

ts_s32 ts_tdk_snr_get_cut_auto(ts_void);
ts_s32 ts_tdk_snr_get_cut_temp_auto(ts_void);

/* 去横竖纹功能 */
ts_void snr_fix_snc_proc(ts_void);

/* 配置输出 */
ts_s32 snr_cfg_vpo(snr_attr_s_t *pst_snr);
ts_s32 vot_snr_cfg_vpo(snr_attr_s_t *pst_snr);

/* 通用函数接口 */
snr_attr_s_t * snr_get_attr(ts_void);
ts_void snr_insmod_all_tdk(ts_u32 u32_param);
ts_s32 snr_auto_cut_process(ts_void *pv_data);
ts_void snr_brd_init(ts_void);
ts_u32 snr_get_cfg_size(ts_u32 u32_def_size);
ts_void snr_init_smart(ts_void *pv_snr_smart, ts_u32 u32_size);
ts_void *snr_get_isp_cfg(ts_u32 u32_def_cfg_type);
ts_s32 snr_wait_ready(ts_u32 u32_tmout_ms);
ts_void snr_init_sffc(ts_isp_sffc_attr_s_t *pst_sffc_attr);
ts_void snr_init_ooc(ts_void);
ts_void snr_init_nuc(ts_isp_nuc_attr_s_t *pst_nuc_attr);


/* 矫正 */
ts_s32 sns_get_temperature(ts_void);
ts_void snr_isp_init(ts_s32 s32_img_w, ts_s32 s32_img_h, ts_s32 s32_fps, ts_void *pv_isp_cfg);
ts_s32 isp_get_raw_raw_sigle(ts_s32 s32_pos, ts_isp_image_s_t *pst_image, ts_u32 u32_timeout_ms);
// ts_void ts_isp_init_bp(ts_void);

ts_void snr_force_cut(ts_void);
ts_void snr_exec_ooc(ts_u32 u32_rst_en, ts_u32 u32_rst_val, ts_u32 u32_sht_en, ts_u32 u32_occ_en);

ts_void snr_init_ext_bp(ts_void);
ts_s32 save_ext_bp(ts_void);//坏斑保存

extern int cli_simple_run_command(const char *cmd);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __SNR_COMM_H__ */


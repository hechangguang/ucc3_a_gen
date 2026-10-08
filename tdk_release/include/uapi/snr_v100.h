/* 
 * snr_v100.h -- 探测器标定、优化接口头文件
 */
#ifndef __UAPI_SNR_V100_H__
#define __UAPI_SNR_V100_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <base/ts_type.h>
#include <base/ts_comm.h>
#include <uapi/viu_v100.h>
#include <uapi/isp_comm.h>
#include <uapi/alg_v100.h>

/* Sensor Config Select */
typedef enum ts_snr_coef_en {
    TS_SNR_COEF_INT_TIME = 0,
    TS_SNR_COEF_RST_TIME = 1,
    TS_SNR_COEF_INT_CAP = 2,
    TS_SNR_COEF_NTCC = 3,
    TS_SNR_COEF_ADC_GAIN = 4,
    TS_SNR_COEF_GOF = 5,
    TS_SNR_COEF_SFB_OOC = 6,

    TS_SNR_CFG_BUTT
} ts_snr_coef_en_t;

/* sensor config */
typedef struct ts_snr_coef_s {
    ts_snr_coef_en_t en_coef;
    ts_u32 u32_rwdata;
} ts_snr_coef_s_t;

/* sensor sd */
typedef struct ts_snr_sd_s {
    ts_u32 u32_msb;
    ts_u32 u32_lsb;
    ts_u32 u32_rwdata;
} ts_snr_sd_s_t;


#define SFC_FILE_VALID_CODE             (0X12345678)
#define SFC_FILE_MAX_ISP_CFG_NUM        (4)
#define SFC_FILE_MAX_SNS_CFG_NUM        (4)
#define SFC_FILE_MAX_SFFC_CFG_NUM       (4)

typedef struct sfc_cfg_file_s {
    ts_char ac_cfg_name[32];
    ts_u32  u32_version_code;
    ts_u32  u32_valid_code;
    ts_u32  u32_valid_flag;
    ts_u32  u32_cfg_num;
    ts_void *pv_usr_ptr;
    ts_u32  u32_usr_addr;
    ts_u08  au08_new_cfg_flag[4];
    ts_u32  u32_rev[9];
    ts_u64  au64_payload[0];
} sfc_cfg_file_s_t;


#define MAX_BP_POS_NUM              (2048)
#define BP_FILE_VALID_FLAG_0        (0XFEDCCA98)
#define BP_FILE_VALID_FLAG_1        (0X12345678)
typedef struct bp_pos_info_s{
    ts_void *pv_bp_buf_addr;        // 坏点缓冲区内存地址
    ts_u32  u32_bp_cnt;             // 实际收集到的坏点数量
    ts_u32  u32_bp_cnt_new_add;     // 实际收集到的坏点数量
    ts_u32  gu32_bp_col;
    ts_u32  gu32_bp_row;
    ts_void *pv_bp_save_addr;       // 坏点缓冲区内存地址
    ts_u32  u32_valid_code_0;
    ts_u32  u32_valid_code_1;
    ts_u32 reserved[5];
} bp_pos_info_s_t;

/* tdk 配置文件，大小 512 */
#define TS_TDK_FILE_MAGIC       (0x5555aaaa)
typedef struct tdk_cfg_s {
    /* 魔术字 */
    ts_u32 u32_magic;

    /* 应用调试等级配置 */
    ts_u32 u32_dbg_lvl;

    /* 其他调试等级,并做空间保留 */
    ts_u32 au32_dbg_lvl[31];

    /* 版本信息 */
    ts_char ac_version[16];

    /* 用户配置信息 */
    ts_char ac_usr_info[32];

    /* 用户配置时间 */
    ts_char ac_usr_date[32];

    /* 保留 */
    ts_u16 u16_rev0;

    /* usb 类型，仅支持: libusb(0) 和 uvc(1) 切换*/
    ts_u08 u08_usb_type;

    /* 开机执行脚本 */
    ts_u08 u08_sh_en;
    ts_char ac_sh[256];

    ts_u32 au32_rev[10];
} tdk_cfg_s_t;


typedef struct ts_extbpc_cfg_s {
    ts_u32 u32_en;

    /* =-1 ，自动匹配 x 和 y */
    ts_s32 s32_idx;

    /* 搜索窗配置 */
    ts_u32 u32_cor_win_sz;

    ts_u16 x;
    ts_u16 y;

    ts_u16 u16_x_m;
    ts_u16 u16_y_m;

    ts_u16 u16_size_x;
    ts_u16 u16_size_y;
} ts_extbpc_cfg_s_t;

/* cut_process */
typedef struct ts_snr_cut_cfg_s {
    /* 目标均值 */
    ts_u32 u32_target_mean;

    /* 是否开启温度变化打挡功能 */
    ts_u32 u32_temp_en;

    /* 在 u32_temp_en > 0 生效， 温度变化幅度 */
    ts_s32 s32_temp_delta;

    /* 温度修正,不需要的话置 0 */
    ts_s32 s32_temp_fix;

    /* 是否开启时间变化打挡功能 */
    ts_u32 u32_time_en;

    /* 在 u32_time_en > 0 生效， 温度变化幅度 */
    ts_u32 u32_time_delta;

    /* 是否开启环境判断 */
    ts_u32 u32_mean_en;

    /* u32_mean_en > 0 生效 */
    ts_u32 u32_mean_delta;

    /* 打挡倒计时使能 */
    ts_u32 u32_cd_en;

    /* 打挡倒计时设置时间, ms 为单位 */
    ts_u32 u32_cd_ms_set;

    /* 配置在开机不稳定状态下的快速挡片次数， > 0 生效 */
    ts_u32 u32_fast_cnt;

    /* 在 u32_fast_cnt > 0 时，为快速挡片的间隔时间 */
    ts_u32 u32_fast_time_delta;

    /* free shutter 使能 */
    ts_u08 u08_free_sht_en;

    /* AI free shutter 使能 */
    ts_u08 u08_ai_free_sht_en;

    ts_u16 u16_rev0;

    /* 温区使能 */
    ts_u32 u32_tempz_en;
    ts_u32 u32_temp_zone;

    /* 温度范围 */
    ts_s32 s32_tempz_lo;
    ts_s32 s32_tempz_hi;

    ts_u08 u08_gst_verion;

    /* 使用默认配置 */
    ts_u08 u08_def_en;

    ts_u16 u16_rev1;
	
	/* 温补句柄*/
    ts_handle_t h_free_sht;

    /* 用于粗调步进 */
    ts_u16 u16_fast_step;

    /* 用于细调延时， 内部为 (u16_trim_dly_ms * 2) + 20 */
    ts_u16 u16_trim_dly_ms;

    /* 保留 */
    ts_u32 au32_rev[1];

    /* 传入 smartdata 相关 */
    ts_void *pv_smart;

    /* 探测器 clc 初始化 */
    ts_s32 (*pfn_clc_init)(ts_void *);

    /* 关闭挡片 */
    ts_void (*pfn_sht_close)(ts_void);

    /* 打开挡片 */
    ts_void (*pfn_sht_open)(ts_void);

    /* 等待 pod 完成 */
    ts_void (*pfn_wait_pod)(ts_void);

    /* 获取 cut_force 状态 */
    ts_s32 (*pfn_get_force)(ts_void);

    /* 获取 cut 线程是否自动执行 */
    ts_s32 (*pfn_get_auto)(ts_void);

    /* 获取温补是否自动执行 */
    ts_s32 (*pfn_get_temp_auto)(ts_void);
    ts_s32 (*pfn_change_occ)(ts_u08);

    /* i2c ra08d08 */
    ts_void (*pfn_i2c_ra08d08_setreg)(ts_u08, ts_u08);
    ts_s32 (*pfn_i2c_ra08d08_getreg)(ts_u08, ts_u08 *);

    /* 获取温度 */
    ts_s32 (*pfn_get_temperature)(ts_void);

    /* 温补接口 */
    ts_s32 (*pfn_sl_proc)(ts_handle_t, ts_s32, ts_bool);
    ts_s32 (*pfn_sl_get_status)(ts_handle_t, ts_sl_status_s_t *);

    /* 私有变量 */
    ts_void *pv_priv;
} ts_snr_cut_cfg_s_t;


typedef struct ts_snr_calib_point_s {
    /* fpa 获取的 ad 数值 */
    ts_u32 u32_dv_temp;

    /* 此 ad 数值对应的真实温度 */
    ts_f32 f32_real_temp;
} ts_snr_calib_point_s_t;

#define SFFC_BAD_INFO_VALID_CODE  0x12345678

typedef struct ts_sffc_bp_info_s {
    ts_u32 u32_valid_code;
    ts_u32 u32_bad_num;
    ts_u32 u32_ret[62];          // 保留字段，保证头部大小不变
} ts_sffc_bp_info_s_t;



typedef enum burn_pt_state_e {
    SUN_PROTECT_STATE_SAFE = 0,          /* 安全状态：挡片打开 */
    SUN_PROTECT_STATE_PROTECT = 1,       /* 保护状态：挡片关闭 */
} burn_pt_state_e_t;

typedef ts_void (*burn_pt_set_auto_cut_t)(ts_u32 u32_enable);
typedef ts_void (*burn_pt_shutter_open_t)(ts_void);
typedef ts_void (*burn_pt_shutter_close_t)(ts_void);

typedef struct burn_pt_ext_func_s {
    burn_pt_set_auto_cut_t pfn_set_auto_cut;
    burn_pt_shutter_open_t pfn_shutter_open;
    burn_pt_shutter_close_t pfn_shutter_close;
} burn_pt_ext_func_s_t;

typedef struct burn_pt_attr_s {
    burn_pt_ext_func_s_t st_ext_func;       /* 外部函数注册接口 */
    ts_u32 u32_interval_ms;              /* 检测线程间隔 */
    ts_u32 u32_timeout_ms;               /* hot 统计超时时间 */
    ts_u32 u32_auto_open_ms;             /* 保护后自动打开挡片时间 */
    ts_u32 u32_edge_min;                 /* 单个 blob 边缘点数下限 */
    ts_u32 u32_sens;                     /* 灵敏度等级，1~5 */
} burn_pt_attr_s_t;



/* --- function list --- */

/* 挡片定义，需用户实现 */
ts_void ts_tdk_snr_sht_open(ts_void);
ts_void ts_tdk_snr_sht_close(ts_void);
ts_s32 ts_tdk_snr_get_cut_force(ts_void);
ts_s32 ts_tdk_snr_get_cut_auto(ts_void);
ts_void ts_tdk_snr_wait_pod_done(ts_void);
ts_s32 ts_tdk_snr_get_raw_sigle(ts_s32 s32_pos, ts_isp_image_s_t *pst_image, ts_u32 u32_timeout_ms);

/* 
 * 函数名 :  ts_tdk_snr_get_version
 * 描  述 :  获取版本号
 * 输  入 :  参数               描述
 *          无                  无
 * 输  出 :  参数               描述
 *          无                  无
 * 返  回 :  返回值             描述
 *          >= 100              版本号
 *          < 100               获取失败
 */
ts_s32 ts_tdk_cali_get_version(ts_void);


ts_void *cali_tempdif_init(ts_void *pv_tmm, ts_s32 s32_img_w, ts_s32 s32_img_h, ts_u32 *pu32_hsize);
ts_s32 cali_calc_tempdif(ts_void *pv_hdif, ts_u08 *pu08_nuc_buf, ts_s32 s32_temp_cur, ts_s32 s32_img_w, ts_s32 s32_img_h);
ts_s32 cali_calc_tempdif_coef(ts_s32 s32_temp_cur, ts_s32 s32_temp_last, ts_s32 s32_tmp_dif);

ts_s32 ts_tdk_snr_load_bcfg_hdr(ts_char *pc_fname);
ts_s32 ts_tdk_snr_write_bcfg_hdr(ts_char *pc_fname, ts_s32 s32_offset, ts_s32 s32_flag, ts_s32 s32_temp);
ts_s32 ts_tdk_snr_write_bbuf(ts_char *pc_fname, ts_s32 s32_offset, ts_s32 s32_temp_1st, \
                                ts_s32 s32_temp_2st, ts_u08 *pu08_bbuf, ts_u32 u32_size);

ts_void ts_tdk_snr_lib_init(ts_s32 s32_img_w, ts_s32 s32_img_h, ts_s32 s32_fps, bp_pos_info_s_t *pst_bp_info);
ts_s32 isp_set_bp_pos(ts_s32 s32_x, ts_s32 s32_y);
ts_s32 isp_delete_bp_pos(ts_s32 s32_x, ts_s32 s32_y);
ts_s32 isp_read_bp_pos(ts_s32 s32_x, ts_s32 s32_y, ts_s32 s32_x_len, ts_s32 s32_y_len);
ts_s32 isp_set_bp_block(ts_s32 s32_x, ts_s32 s32_y, ts_s32 s32_x_len, ts_s32 s32_y_len);
ts_s32 isp_delete_bp_block(ts_s32 s32_x, ts_s32 s32_y, ts_s32 s32_x_len, ts_s32 s32_y_len);
ts_s32 isp_set_bp_line(ts_s32 s32_line);
ts_s32 isp_delete_bp_line(ts_s32 s32_line);
ts_s32 isp_set_bp_col(ts_s32 s32_col);
ts_s32 isp_delete_bp_col(ts_s32 s32_col);
ts_s32 isp_set_sync_bp(ts_char *pc_fname);
ts_s32 isp_save_bp_flash(ts_char *pc_fname);
ts_s32 isp_clr_bp_flash(ts_char *pc_fname);
ts_void isp_clr_bp(ts_void);
ts_s32 isp_auto_bp_det(ts_u32 u32_bpc_en, ts_u32 u32_hist, ts_u32 u32_diff, ts_u32 u32_flt_size, ts_u32 u32_det_type, ts_u32 u32_pos);
ts_s32 ts_isp_set_bp_multi_data(ts_void);
ts_s32 isp_bp_flash_to_nuc_k(ts_char *pc_fname);
ts_void *ts_isp_get_bp_pos_addr(ts_u32 u32_u32_bp_cnt);
ts_void *ts_isp_get_bp_pos_buf(ts_u32 u32_type, ts_u32 *pu32_bp_cnt);
ts_s32 isp_sw_calc_k_nuc(ts_char *pc_fname, ts_u32 u32_file_size, ts_s32 s32_img_w, ts_s32 s32_img_h, ts_s32 s32_tmp, ts_s32 s32_step);
ts_void isp_sw_bp_cali(ts_s32 s32_img_w, ts_s32 s32_img_h);
ts_void isp_sw_calc_k_nuc_step_bp(ts_char *pc_fname, ts_u32 u32_file_size, ts_s32 s32_img_w, ts_s32 s32_img_h, ts_s32 s32_tmp, ts_s32 s32_step);
ts_s32 isp_sw_calc_k_nuc_step(ts_char *pc_fname, ts_u32 u32_file_size, ts_s32 s32_img_w, ts_s32 s32_img_h, ts_s32 s32_tmp, ts_s32 s32_step);
ts_void isp_sw_calc_k_lens(ts_char *pc_fname, ts_u32 u32_file_size, ts_s32 s32_img_w, ts_s32 s32_img_h, ts_s32 s32_tmp, ts_s32 s32_step);
ts_s32 isp_sw_calc_sffc(ts_char *pc_fname, ts_u32 u32_file_size, ts_s32 s32_img_w, ts_s32 s32_img_h, ts_s32 s32_index);
ts_s32 isp_sw_calc_sffc_refresh(ts_char *pc_fname, ts_u32 u32_file_size, ts_s32 s32_img_w, ts_s32 s32_img_h, ts_s32 s32_index);
ts_s32 isp_sw_calc_sffc_red(ts_char *pc_fname, ts_u32 u32_file_size, ts_s32 s32_img_w, ts_s32 s32_img_h);
ts_s32 isp_sw_calc_sffc_refresh_red(ts_char *pc_fname, ts_u32 u32_file_size, ts_s32 s32_img_w, ts_s32 s32_img_h);
//ts_s32 isp_sw_calc_sffc_min(ts_char *pc_fname, ts_u32 u32_file_size, ts_s32 s32_img_w, ts_s32 s32_img_h);
//ts_s32 isp_sw_calc_sffc_refresh_min(ts_char *pc_fname, ts_u32 u32_file_size, ts_s32 s32_img_w, ts_s32 s32_img_h);
ts_void sffc_cali_hw_flt(ts_s32 s32_img_w, ts_s32 s32_img_h);
ts_void isp_sw_calc_nuc_sffc(ts_s32 s32_img_w, ts_s32 s32_img_h, ts_s32 s32_index);
ts_s32  isp_sw_calc_k_sffc(ts_char *pc_fname_k, ts_char *pc_fname_sffc, ts_u32 u32_file_k_size, ts_u32 u32_file_sffc_size,ts_s32 s32_img_w, ts_s32 s32_img_h, ts_s32 s32_tmp, ts_s32 s32_step);
ts_s32 isp_sw_calc_sffc_nr(ts_char* pc_fname, ts_u32 u32_file_size,  ts_s32 s32_win_size, ts_s32 s32_img_w, ts_s32 s32_img_h, ts_s32 s32_index, ts_s32 s32_step);
ts_void ts_tdk_snr_set_debug(ts_u32 u32_lvl);
ts_void isp_cali_free_mem(ts_void);

ts_void *ts_tdk_extbpc_init(ts_void *pv_tmm, ts_u32 u32_dly_start_ms, ts_u32 u32_cor_max_num, ts_u32 u32_loop_cfg, ts_u32 u32_bypass_en);
ts_s32 ts_tdk_add_extbpc(ts_extbpc_cfg_s_t *pst_cfg);
ts_s32 ts_tdk_del_extbpc(ts_s32 s32_index);
ts_void ts_tdk_extbpc_status(ts_void);
ts_s32 ts_tdk_extbpc_sync(ts_void);
ts_void ts_tdk_extbpc_pause(ts_void);
ts_void ts_tdk_extbpc_run(ts_void);
ts_void ts_tdk_extbpc_set_bdg(ts_u32 u32_en, ts_u32 u32_val);


/* funtion related to sensor */
ts_s32 ts_tdk_snr_set_sdb(ts_snr_sd_s_t *pst_snr_sd);
ts_s32 ts_tdk_snr_get_sdb(ts_snr_sd_s_t *pst_snr_sd);
ts_s32 ts_tdk_snr_get_coef(ts_snr_coef_s_t *pst_snr_coef);
ts_s32 ts_tdk_snr_set_coef(ts_snr_coef_s_t *pst_snr_coef);
ts_s32 ts_tdk_snr_get_temp(ts_s32 *ps32_temp);
ts_s32 ts_tdk_snr_set_smd(ts_viu_snr_attr_s_t *pst_snr_attr);
ts_s32 ts_tdk_snr_set_mean(ts_u32 u32_target, ts_u32 u32_diff);
ts_s32 ts_tdk_snr_gain_corr(ts_void);
ts_s32 ts_tdk_snr_change_mean(ts_u32 u32_dir, ts_u32 u32_step);

/* cut 相关 */
ts_void ts_tdk_snr_ak612_set_mean(ts_u32 u32_target);

/* wy */
ts_s32 ts_tdk_snr_wy640_get_temperature(ts_void);
ts_s32 ts_tdk_snr_wy640_auto_cut_process(ts_void *pv_data);

/* gst */
ts_s32 ts_tdk_snr_gst_auto_cut_process(ts_void *pv_data);
ts_s32 ts_tdk_snr_gst612_auto_cut_process(ts_void *pv_data);

/* 标准接口 */
ts_s32 ts_tdk_snr_clc_v100_process(ts_void *pv_data);

/* 单独分开初始化，避免全部被链接 */
ts_s32 ts_tdk_snr_clc_rt_init(ts_void *pv_ctx);
ts_s32 ts_tdk_snr_clc_gst_init(ts_void *pv_ctx);
ts_s32 ts_tdk_snr_clc_ts_init(ts_void *pv_ctx);
ts_s32 ts_tdk_snr_clc_wy_init(ts_void *pv_ctx);
ts_s32 ts_tdk_snr_clc_cumec_init(ts_void *pv_ctx);
ts_s32 ts_tdk_snr_clc_zs_init(ts_void *pv_ctx);
ts_s32 ts_tdk_snr_clc_hv_init(ts_void *pv_ctx);

ts_s32 ts_tdk_snr_get_extended_temp(ts_u32 u32_dv_temp, ts_snr_calib_point_s_t *pst_cal_pnt, ts_u32 u32_max_num);

ts_void *ts_tdk_op_init(ts_void *pv_tmm, ts_u32 u32_dly_start_ms, ts_u32 u32_loop_cfg);
ts_s32 ts_op_s16_run_task(ts_op_task_s_t *pst_task, ts_u32 *pu32_time_cost);


ts_s32 ts_tdk_snr_burn_pt_set_attr(burn_pt_attr_s_t *pst_attr);
ts_s32 ts_tdk_snr_burn_pt_get_attr(burn_pt_attr_s_t *pst_attr);

ts_u32 ts_tdk_snr_burn_pt_get_task_status(ts_void);
ts_u32 ts_tdk_snr_burn_pt_get_state(ts_void);

ts_s32 ts_tdk_snr_burn_pt_run(burn_pt_attr_s_t *pst_attr);
ts_s32 ts_tdk_snr_burn_pt_stop(ts_void);


ts_s32 ts_tdk_snr_burn_pt_manual_open(ts_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __UAPI_SNR_V100_H__ */


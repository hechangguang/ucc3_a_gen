/*
 * @Author: git config user.name && git config user.email
 * @Date: 2025-11-26 11:39:21
 * @LastEditors: git config user.name && git config user.email
 * @LastEditTime: 2026-09-29 16:51:36
 * @FilePath: /ucc3_a_gen/vot_src/inc/snr_comm_vot.h
 * @Description: 
 * 
 * Copyright (c) 2025 by ${git_name_email}, All Rights Reserved. 
 */
#ifndef  __SNR_COMM_VOT_H__
#define  __SNR_COMM_VOT_H__

#include <base/ts_type.h>
#include <stdio.h>
#include <cpl/cpl_mutex.h>
#include <cpl/cpl_time.h>
#include <base/ts_comm.h>
#include <uapi/osd_v100.h>
#include <uapi/upl_v100.h>
#include <math.h>
#include <uapi/chip_def.h>
#include <uapi/isp_comm.h>


extern ts_void vot_uvc_usr_reset(ts_void);
extern ts_void vot_usb_uvc_init(ts_void *pv_h_tmm);

ts_s32 vot_uvc_stop_video(ts_void);
ts_s32 vot_uvc_start_video(ts_void);
ts_void vot_uvc_set_attr(ts_mdev_t h_dev, ts_u32 u32_sub, ts_u32 u32_mode);
// ts_u08 templut_Getcvbs_orgi_en(void);
ts_void cvbs_pod_set(ts_u32 u32_pod_dly_ms);
// ts_void cvbs_pod_set_v2(ts_u32 u32_pod_dly_ms);
ts_void cvbs_display_mode_set(ts_u08 cvbs_display_mode);
ts_s32 isp_dump_ad_info(ts_s32 s32_pos ,  ts_u16* ad_avg , ts_u16* ad_max, ts_u16* ad_min, void* frame_ptr);
ts_void vot_usb_device_init(void);
ts_void vot_isp_dump_bp_k(ts_void);
ts_void vot_snr_set_grp_pod(ts_grp_t h_grp);
ts_void vot_isp_set_sync_bp(ts_void);
ts_s32 vot_isp_bp_kb_table_init(ts_void);
ts_s32 vot_isp_bp_kb_table_write(ts_u16 * wr_kbuf);
ts_s32 vot_isp_bp_kb_table_merge_to_sdk_k_table(void);
ts_s32 vot_isp_bp_kb_table_read(ts_u16 * rd_kbuf);
ts_s32 vot_isp_sdk_k_table_read(ts_u16 * rd_kbuf);
ts_s32 vot_isp_sdk_k_table_write(ts_u16 * wr_kbuf);
ts_void vot_isp_clr_bp(ts_void);
ts_s32 vot_isp_bp_kb_table_reset(ts_void);

ts_void set_osd_position(ts_mdev_t h_osd_dev, ts_nodebuf_s_t *pst_hosd, ts_u16 u16_left, ts_u16 u16_top);
ts_void usb_uvc_reset(void);


ts_void bpc_mode_isp_set(ts_u08 ee_en, ts_u08 dde_en);
ts_void update_cvbs_vpo_attr(void);

ts_s32 vot_get_temperature_info(ts_u16* ad_value, ts_f32* temp_value);
ts_u16 vot_get_tes_info(ts_void);
ts_u08 get_sensor_status(ts_void);
ts_void wait_sensor_work(ts_void);

ts_s32 vot_vpo_unfreeze(ts_void);
ts_s32 vot_vpo_freeze(ts_void);

ts_u32 vot_get_sun_pt_targer_pixels(ts_u32 gray_level_value);
ts_void vot_sun_pt_shutter_close(ts_void);
ts_void vot_sun_pt_shutter_open(ts_void);

ts_void vot_shutter_force_close(ts_void);
ts_void vot_shutter_force_open(ts_void);

ts_u32 vot_whole_hist_target_pixels(ts_s32 level_value);
ts_void vot_whole_hist_test(ts_s32 level_value, ts_s32 pixel_num);
// ts_void vot_block_hist_test(ts_void);

ts_s32 planck384c_get_temperature_test(ts_u16* ad_value, ts_f32* temp_value ,  ts_u16 u16_coor_y);

ts_void vot_set_lut_info(ts_u32 u32_enable, ts_u08 u08_lut_update_en, ts_u08 src_sel, ts_u16 u16_mk, ts_u16 u16_mb, ts_u16 u16_b1);

ts_void vot_set_sun_pt_notice_info(int is_visible);
ts_void vot_set_sun_pt_osd_refresh(int is_visible );

ts_void ISP_set_det_switching_flag(ts_u08 det_switching_flag);
ts_u08 ISP_get_det_switching_flag(ts_void);
ts_void ISP_set_det_switch_force_flag(ts_u08 det_switch_force_flag);
ts_u08 ISP_get_det_switch_force_flag(ts_void);
ts_void ISP_set_bottom_statistic_force_flag(ts_u08 bottom_statistic_force_flag);
ts_u08 ISP_get_bottom_statistic_force_flag(ts_void);
ts_void ISP_set_update_fire_statis_force_flag(ts_u08 update_fire_statis_force_flag);
ts_u08 ISP_get_update_fire_statis_force_flag(ts_void);
ts_void ISP_set_feeding_flag(ts_u08 feeding_flag);
ts_u08 ISP_get_feeding_flag(ts_void);

ts_void vot_check_det_int_para(void);
ts_s32 vot_check_det_all_para(void);

ts_void ISP_set_det_ad_info(ts_f32 ad_f32, ts_u16 ad_u16);
ts_void ISP_get_det_ad_info(ts_f32* ad_f32_ptr, ts_u16* ad_u16_ptr);
ts_void vot_snr_update_det_ad(ts_void);

ts_void vot_snr_set_grp_pod_switch(ts_u08 is_visible);
ts_void vot_snr_set_cvbs_enlarge_switch(ts_u08 is_enlarge);
ts_void vot_snr_set_uvc_output_size(ts_u16 u16_disp_width, ts_u16 u16_disp_height);
ts_void vot_uaw_get_gos_from_det_temp(ts_void);
ts_void vot_snr_init_detector(ts_u08 freeze_flag, ts_u08 force_det_mode);

ts_s32 vot_snr_del_ext_bp(ts_s32 s32_index);
ts_s32 vot_snr_set_ext_bp1( ts_s32 s32_idx , ts_u32 u32_en,  ts_s32 s32_win_x_start, ts_s32 s32_win_y_start, ts_u16 u16_x_m,ts_u16 u16_y_m , ts_u16 u16_size_x,ts_u16 u16_size_y);
ts_s32 vot_snr_set_ext_bp2( ts_s32 s32_idx , ts_u32 u32_en,  ts_s32 s32_bad_x, ts_s32 s32_bad_y, ts_s32 s32_bad_w,ts_s32 s32_bad_h);
ts_s32 vot_snr_set_ext_bp3( ts_s32 s32_idx , ts_u32 u32_en, ts_s32 s32_bad_cx, ts_s32 s32_bad_cy, ts_s32 s32_diameter);
ts_void vot_check_bpc_ext_para(void);
ts_s32 vot_check_extbpc_info_vaild( ts_s32 s32_idx , ts_s32 s32_bad_x, ts_s32 s32_bad_y, ts_s32 s32_bad_w,ts_s32 s32_bad_h);

ts_u08  vot_snr_get_cvbs_en(void);
ts_u32  vot_snr_get_mem_en(void);
ts_u32  vot_snr_get_bte_en(void);
ts_u32  vot_snr_get_mipi_csi_en(void);
ts_u32  vot_snr_get_mipi_dsi_en(void);
// ts_u08  vot_snr_get_t8_mode_raw_en(void);
ts_u08  vot_check_lut_raw_pos(ts_u08 index);
ts_u08  vot_check_upl_raw_pos(ts_u08 index);


ts_void  vot_auto_calc_gos_for_det(ts_u08 DetMode);

ts_void  vot_add_user_metadata_info(ts_u08* user_metadata, ts_u32 metadata_size);

ts_void vot_isp_luma_set(ts_void);
ts_void vot_isp_coef_set(ts_void);
ts_void vot_pip_mode_set(ts_u08 pip_mode, ts_u08 rz_ratio, ts_u08 rz_en);

void vot_hst_mode_set(ts_u08 hst_mode);
void vot_set_hst_cursor_position(ts_nodebuf_s_t *cursor_node, ts_u16 pos_x, ts_u16 pos_y);
ts_void vot_get_fire_statis(ts_isp_fire_statis_s_t* out_fire_statis);
ts_void vot_hst_update_attr_safe(void);

ts_void vot_roi_zoom_set(ts_u16 zoom, ts_u16 u16_roi_x, ts_u16 u16_roi_y ,ts_u16 u16_roi_width , ts_u16 u16_roi_height);
ts_void vot_zoom_out_set(ts_u16 zoom, ts_u08 zoom_type);

ts_void vot_m_dev_env_mode_set(ts_u08 dev_env_mode);
ts_void vot_sun_pt_notify_peer(ts_u08 start_flag);

ts_u08 vot_get_force_debug_flag(ts_void);
ts_u08 vot_get_u08_step_frame_num(void);

ts_void vot_set_iso_parameters(ts_u08 switch_flag,ts_u08 iso_position_mode, ts_u08 iso_color_mode, ts_u16 u16_iso_h,ts_u16 u16_iso_l );
ts_void vot_set_ad_filter_mode(ts_u08 filter_ad_mode, ts_u16 manual_ad_high,ts_u16 manual_ad_low );

ts_s32 vot_isp_dump_raw_sigle(ts_s32 s32_pos, ts_void* pv_dst_raw, ts_u32 frm_size) ;
ts_void vot_refresh_osd_bind(ts_void);
ts_u08 vot_menu_osd_type(ts_void);
ts_void vot_set_temp_drift_calibration_mode(ts_u08 temp_drift_cali_mode, ts_u08 init_flag );

ts_void observation_auto_cut_process(ts_void);
ts_void measurement_auto_cut_process(ts_void);
ts_u32 vot_isp_get_upl_get_frm_strid(ts_s32 s32_pos);

void vot_occ_update_with_shutter(void);
void vot_occ_update_with_calibration(ts_u08 force_cut_flag);
void vot_occ_update_with_only_shutter(void);


ts_u08 vot_get_t8_mode_force_bpc(void);
void vot_set_t8_mode_force_bpc(ts_u08 t8_mode_force_bpc);
void vot_set_vot_pod_type(ts_u08 vot_pod_type);

ts_u08 vot_get_flash_type(void);
ts_u32 vot_get_fn_vot_nuc_kb_table_size(ts_s32 width, ts_s32 height);
ts_u32 vot_get_fn_vot_nuc_kb_file_size(void);
ts_u32 vot_get_fn_occ_file_size(void);
ts_u32 vot_get_fn_k_file_size(void);
ts_u32 vot_get_fn_b_file_size(void);
ts_u32 vot_get_fn_sffc_file_size(void);
ts_u32 vot_get_fn_aisp_file_size(void);

void vot_reset_sence_env(void);

void vot_easylog_init(void);

int vot_get_hot_statistics(void);

int vot_check_firmware_crc32_from_flash(ts_u16 u16_crc_src, ts_u32 u32_addr, ts_u32 u32_want_rd_size);

ts_u08 vot_get_temp_drift_aisp_enable(void);
ts_void vot_set_temp_drift_aisp_mode(ts_u08 u8_mode);

#endif
#ifndef ISP_FUNCTION_H
#define ISP_FUNCTION_H

#include <config.h>
#include <base/ts_type.h>
#include <cpl/ts_cpl.h>
#include <uapi/chip_def.h>
#include <uapi/isp_v100.h>
#include <uapi/isp_reg.h>
#include <uapi/sfc_v100.h>
#include <uapi/aisp_v100.h>
#include <uapi/viu_v100.h>
#include <uapi/vpo_v100.h>
#include <command.h>
#include <stdio.h>
#include <string.h>
#include <menu/votmain.h>
#include <menu/votmenu.h>
#include <menu/ispfunction.h>

ts_u08 ISP_dde_set_mode(ts_u08 u08_mode);
ts_void ISP_set_TDC_node(ts_s32 s32_oft);
ts_u08 ISP_luma_set(ts_u16 birt);
ts_u16 ISP_luma_get(void);
ts_void ISP_luma_and_ctr_set_manual(ts_u08 manual_flag, ts_u16 reset_luma_value, ts_u16 reset_ctr_value );
ts_void ISP_luma_and_ctr_by_drc( ts_s32 luma_value, ts_s32 ctr_value );
ts_u08 ISP_luma_set_by_percent(ts_u08 luma_percent_mode);
ts_u08 ISP_coef_set(ts_u16 ctr);
ts_u16 ISP_coef_get(void);
ts_u08 ISP_ctr_set_by_percent(ts_u08 ctr_percent_mode);
ts_u08 ISP_nr_set(ts_u16 nr);
ts_u08 ISP_ee_set(ts_u16 ee);
ts_u08 	ISP_continuous_zoom_set(ts_u16 zoom);
ts_void ISP_zoom_set(ts_u16 zoom);
ts_void ISP_vot_zoom_set(ts_u08 Enlarge);
ts_u08 ISP_mirr_set(ts_u16 mirr);
ts_u08 ISP_dde_level(ts_u08 level);
ts_u08 ISP_dde_set_en(ts_u08 u08_en);
ts_u08 ISP_snc_set_en(ts_u08 u08_en);
ts_u08 ISP_fpnc_set_en(ts_u08 u08_en);
ts_u08 ISP_enhance_set(ts_u16 enhance);
ts_u08 ISP_agc_luma_set(ts_u08 agc_luma);
ts_u08 ISP_shuttermode(ts_u08 shuttermode);
ts_u08 ISP_get_ShutterMode(void);
ts_s32 ISP_get_shutter_timer_gap(void);
ts_void ISP_fix_snc_switch_set(ts_u08 fix_snc_en);
ts_u16 ISP_get_det_ad_value(void);
ts_void ISP_auto_shutter_process(ts_void);
ts_void ISP_ee_enhance_mode_set(ts_u08 ee_enhance_mode, ts_u08 store_flag);
ts_u08 ISP_palette_set(ts_yuv_s_t *yuv_tab);
ts_void ISP_enable_drc( ts_u08 enable );
ts_void Isp_set_agc_loc_min_max(ts_u16 u16_ad_min, ts_u16 u16_ad_max);

ts_void ISP_force_cut(ts_s32 s32_force_cut);
ts_s32 ISP_get_auto_cut_flag(ts_void);
ts_void ISP_set_auto_cut_flag_auto(ts_void);
ts_void ISP_cut_v2(void);
ts_u08 ISP_cut_old(void);
ts_void ISP_cut_new(void);
ts_void ISP_cut(void);
ts_u08 ISP_pause(void);
ts_u08 ISP_auto_cut_off(void);
ts_u08 ISP_run(void);
ts_u08 ISP_auto_cut_on(void);
ts_void vot_isp_cut_with_flag(ts_u08 u08_need_unfreeze_flag, ts_u08 delay_flag, ts_u08 cut_type);
ts_void vot_isp_cut_with_pod_flag(ts_u08 u08_need_unfreeze_flag, ts_u08 pod_flag ,ts_u08 pod_type);

ts_void vot_isp_temp_cut_with_flag(ts_u08 u08_need_unfreeze_flag, ts_u08 delay_flag);
ts_u08 ISP_sffc_cali(ts_s32 s32_index);
ts_u08 ISP_sffc_init(void);
ts_u08 ISP_sffc_calc(void);
ts_void ISP_sffc_refresh(void);
ts_void ISP_sffc_nr_calc_and_save(void);
ts_void ISP_sffc_nr_save(void);
ts_void ISP_sffc_nr_calc(void);

ts_u08 ISP_occ_init(void);
ts_void ISP_occ_calc(void);
ts_void ISP_occ_calc_with_shutter(void);
ts_void ISP_update_b(void);
ts_u08 ISP_occ_save(void);

ts_u08 ISP_nuc_init(void);
ts_u08 ISP_nuc_calc_k_lowtemp(void);
ts_u08 ISP_nuc_calc_k_hightemp(void);
ts_u08 ISP_nuc_calc_k_calc(void);
ts_u08 ISP_nuc_calc_k_save(void);
ts_u08 ISP_nuc_vot_kb_save(void);
ts_void ISP_clr_nuc_k(ts_void);
ts_void ISP_reset_nuc_k(ts_void);

ts_u08 ISP_bpc_on(void);
ts_u08 ISP_bpc_off(void);
ts_void ISP_update_b_table(void);

ts_u08 ISP_read_sys_cfg(void);
ts_u08 ISP_write_sys_cfg(void);
ts_u08 ISP_read_sys_menu2_cfg( struct Menu2_Para *sys_menu2_cfg );
ts_u08 ISP_write_sys_menu2_cfg(struct Menu2_Para sys_menu2_cfg);
ts_u08 ISP_read_UserMenu_def_para_cfg(struct _Usermenu_Parameter *UserMenuDefPara);//从Flash读取用户菜单默认参数配置文件
ts_u08 ISP_write_UserMenu_def_para_cfg(struct _Usermenu_Parameter UserMenuDefPara);//FLash写入用户菜单默认参数配置文件
ts_u08 ISP_read_SysMenu_def_para_cfg(struct Menu2_Para *SysMenuDefPara);//从Flash读取系统菜单默认参数配置文件
ts_u08 ISP_write_SysMenu_def_para_cfg(struct Menu2_Para SysMenuDefPara);//FLash写入系统菜单默认参数配置文件
ts_u08 read_JsonUserMenuDefPara_valid_flag(struct _Usermenu_Parameter *UserParamValid);//从Flash读取用户菜单默认参数配置文件的有效性标志位
ts_u08 write_JsonUserMenuDefPara_valid_flag(struct _Usermenu_Parameter UserParamValid);//FLash写入JSON配置的用户菜单默认参数是否有效标志位
ts_u08 read_JsonSystemMenuDefPara_valid_flag(struct Menu2_Para *SysParamValid);//从Flash读取系统菜单默认参数配置文件的有效性标志位
ts_u08 write_JsonSystemMenuDefPara_valid_flag(struct Menu2_Para SysParamValid);//FLash写入JSON配置的系统菜单默认参数是否有效标志位

ts_u08 ISP_read_vot_misc_cfg( struct Vot_Misc_Para *misc_cfg );
ts_u08 ISP_write_vot_misc_cfg( struct Vot_Misc_Para misc_cfg);

ts_u08 ISP_read_vot_isp_cfg( ts_isp_cfg_reg_s_t *vot_isp_cfg );
ts_u08 ISP_write_vot_isp_cfg(ts_isp_cfg_reg_s_t vot_isp_cfg);

ts_u16 ISP_get_nuc_kvalue(ts_s32 s32_x, ts_s32 s32_y);

ts_void ISP_unset_bp_pos(ts_s32 s32_x, ts_s32 s32_y);
ts_void ISP_set_bp_pos(ts_s32 s32_x, ts_s32 s32_y);
ts_void ISP_init_user_cfg(ts_void);
ts_s32 ISP_mirr_get_mirr_enum(ts_u16 mirr);

ts_void ISP_set_usr_mode_enalbe(ts_u32 enable);
ts_void ISP_set_usr_cfg(ts_isp_cfg_reg_s_t *isp_cfg_input);
ts_void ISP_get_usr_cfg(ts_isp_cfg_reg_s_t *isp_cfg_output);
ts_isp_cfg_reg_s_t* ISP_get_usr_cfg_ptr(void);
ts_u08 ISP_get_output_fps(void);


ts_u08 ISP_get_det_calibrationing_flag(ts_void);
ts_void ISP_set_det_calibrationing_flag(ts_u08 det_calibrationing_flag);

ts_u08 ISP_get_sun_pt_force_flag(ts_void);
ts_void ISP_set_sun_pt_force_flag(ts_u08 sun_pt_force_flag);
ts_void ISP_set_sun_pt_val(ts_u16 target_pixels);
ts_void ISP_set_sun_pt_sw(ts_u16 pt_sw);
ts_void ISP_set_sun_pt_info_sw(ts_u08 pt_info_sw);
ts_void ISP_set_sun_pt_mode(ts_u08 sun_pt_mode);
ts_void ISP_set_sun_pt_gap(ts_u16 time_gap_sec);
ts_u16 ISP_get_sun_pt_gap(ts_void);

ts_void ISP_get_uvc_size_in_bpc_mode(ts_u16* u16_disp_width_ptr, ts_u16* u16_disp_height_ptr);

void  enter_exit_isp_bpc_adjust(ts_u08 calc_active);

void DeadPointCorrection(void);


ts_void ISP_color_dump_bp_k(ts_nodebuf_s_t * pst_lbl, ts_u08 u32_font_color ,  ts_u08 visible, ts_u08 is_clear);
void BPC_Maual_mode_set_or_unset_bpc_with_auto_whole_nuc(void);
ts_void vot_mirror_bp_position(ts_s32 in_s32_x, ts_s32 in_s32_y, ts_s32* out_s32_x, ts_s32* out_s32_y);

ts_void ISP_set_auto_cut_flag(ts_s32 cut_flag);
ts_void ISP_cut_new_exteral(void);
ts_u08 ISP_read_Motorparameter_cfg(void);
ts_u08 ISP_write_Motorparameter_cfg(void);



#endif

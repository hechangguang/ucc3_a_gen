#ifndef __ISP_REG_H__
#define __ISP_REG_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */
/* ========================================================================== */
/*                             头文件区                                        */
/* ========================================================================== */
#include <stdio.h>
#include <stdlib.h> 
#ifdef ALG_MOD_SIM
#include "ts_type.h"
#else
#include <base/ts_type.h>
#endif

/* ========================================================================== */
/*                            类型定义区                                       */
/* ========================================================================== */
#define ISP_CFG_REG_NUM   (760)

/************************模块参数定义****************************/
typedef struct ts_isp_statis_cfg_s         
{
	ts_u08  u08_mode                ;       /*分块大小， 0:4x4, 1: 8x8, 2:16x16, 3:32x32, 4:64x64*/       
	ts_u08  u08_rev[3]              ;
}ts_isp_statis_cfg_s_t; 

typedef struct ts_isp_agc_stat_cfg_s         
{
	ts_u16 u16_rect_lx;		/*统计左边界， U11.0*/
	ts_u16 u16_rect_uy;		/*统计上边界， U11.0*/
	ts_u16 u16_rect_rx;		/*统计右边界， U11.0*/
	ts_u16 u16_rect_dy;		/*统计下边界， U11.0*/
}ts_isp_agc_stat_cfg_s_t; 


typedef struct ts_isp_agc_v00_cfg_s         
{
	/* 有效范围搜索 */
	ts_u08 u08_mode                ;      /* 0:自动分析有效范围, 1：手动指定有效范围, U1.0 */
	ts_u08 u08_bound_ratio         ;      /* 边界融合比例, u8.0,范围[0:255] */
	ts_u08 u08_upd_speed           ;      /* 更新速度, u8.0,范围[0:255] */
	ts_u08 u08_blend_ratio         ;      /* 线性非线性混合比例, u8.0,范围[0:255] */

	ts_u16 u16_loc_min             ;      /* 手动指定的截止灰度最小值, s32_loc_mode 为1时生效,  U14.0 */
	ts_u16 u16_loc_max             ;      /* 手动指定的截止灰度最大值, s32_loc_mode 为1时生效,  U14.0 */

	ts_u32 u32_stat_start          ;      /* 有效范围搜寻左抛点, U22.0 */
	ts_u32 u32_stat_end            ;      /* 有效范围搜寻右抛点, U22.0 */

	/*映射范围*/
	ts_u16 u16_coef              	;      /* 拉伸系数      U10.4 */
	ts_u16 u16_mid_pos            	;      /* 中心点        U14.0 */
	ts_u16 u16_range_limit_min      ;      /* 范围限制      U14.0 */ 
	ts_u16 u16_range_limit_max      ;      /* 范围限制      U16.0 */ 
	ts_u16 u16_limit_min          	;      /* 限制值小端    U14.0 */
	ts_u16 u16_limit_max          	;      /* 限制值大端    U16.0 */

	ts_u16 u16_clip_ratio           ;      /* 限制值系数小端    U8.8 */
	ts_u16 u16_luma_adj             ;      /* 亮度调整    U14.0 */

	ts_u16 u16_max_clip_limit       ;       /* 限制值系数大端    U0.15 */
	ts_u16 u16_min_clip_limit       ;       /* 限制值系数小端    U0.15 */

    ts_u16 u16_clip_adj_coef        ;      /* 限制值调整系数 U5.10 */
    ts_u16 u16_target_per           ;      /* 目标判断比例 U0.12 */

	ts_u16 u16_clip_adj_thr_low     ;      /* 限制值系数大端  U8.7 */
	ts_u16 u16_clip_adj_thr_hig     ;      /* 限制值系数大端  U8.7 */
    ts_u16 u16_clip_min_adj_low     ;      /* 限制值调整系数  U5.10 */
    ts_u16 u16_clip_min_adj_hig     ;      /* 限制值调整系数  U5.10 */
    ts_u16 u16_clip_max_adj_low     ;      /* 限制值调整系数  U5.10 */
    ts_u16 u16_clip_max_adj_hig     ;      /* 限制值调整系数  U5.10 */

	ts_u16 u16_target_adj_thr_low     ;      /* 限制值系数大端  U14.0 */
	ts_u16 u16_target_adj_thr_hig     ;      /* 限制值系数大端  U14.0 */
    ts_u16 u16_target_adj_low_coef    ;      /* 限制值调整系数  U5.10 */
    ts_u16 u16_target_adj_hig_coef    ;      /* 限制值调整系数  U5.10 */

	ts_u16 u16_imp_l                ;      /* 重要区域低段    U14.0 */
	ts_u16 u16_imp_l_delta          ;      /* 重要区域低段过渡    U14.0*/
	ts_u16 u16_imp_h         		;      /* 重要区域高段    U14.0 */
	ts_u16 u16_imp_h_delta          ;      /* 重要区域高段过渡    U14.0*/
	ts_u16 u16_imp_adj_clip_l       ;      /* 重要区域调整最小值    U10.0*/
	ts_u16 u16_imp_adj_clip_h       ;      /* 重要区域调整最小值    U10.0*/

	ts_u16 u16_modify_low           ;      /* 低亮调节限制    U16.0*/
	ts_u16 u16_modify_hig           ;      /* 高亮调节限制    U16.0*/

	ts_u16 u16_valid_num_low        ;	   /* 重分配丢弃像素低阈值 U8.0 */
	ts_u16 u16_valid_num_hig        ;	   /* 重分配丢弃像素高阈值 U16.0 */

	ts_u16 u16_gamma                ;      /* gamma    U4.12 */
	ts_u16 u16_gamma_rev            ;      

}ts_isp_agc_v00_cfg_s_t; 


typedef struct ts_isp_agc_01_cfg_s         
{
	/* 有效范围搜索 */
	ts_u08 u08_mode                ;      /* 0:自动分析有效范围, 1：手动指定有效范围, U1.0 */
	ts_u08 u08_bound_ratio         ;      /* 边界融合比例, u8.0,范围[0:255] */
	ts_u08 u08_upd_speed           ;      /* 更新速度, u8.0,范围[0:255] */
	ts_u08 u08_blend_ratio         ;      /* 线性非线性混合比例, u8.0,范围[0:255] */

	ts_u16 u16_loc_min             ;      /* 手动指定的截止灰度最小值, s32_loc_mode 为1时生效,  U14.0 */
	ts_u16 u16_loc_max             ;      /* 手动指定的截止灰度最大值, s32_loc_mode 为1时生效,  U14.0 */

	ts_u32 u32_stat_start          ;      /* 有效范围搜寻左抛点, U22.0 */
	ts_u32 u32_stat_end            ;      /* 有效范围搜寻右抛点, U22.0 */

	/*映射范围*/
	ts_u16 u16_coef              	;      /* 拉伸系数      U10.4 */
	ts_u16 u16_mid_pos            	;      /* 中心点        U14.0 */
	ts_u16 u16_range_limit_min      ;      /* 范围限制      U14.0 */ 
	ts_u16 u16_range_limit_max      ;      /* 范围限制      U15.0 */ 
	ts_u16 u16_limit_min          	;      /* 限制值小端    U14.0 */
	ts_u16 u16_limit_max          	;      /* 限制值大端    U14.0 */

	ts_u16 u16_clip_ratio           ;      /* 限制值系数小端    U8.8 */
	ts_u16 u16_luma_adj             ;      /* 亮度调整    U4.12 */

	ts_u16 u16_coef_step_limit      ;      /* 限制值系数大端    U8.8 */
	ts_u16 u16_min_step_limit       ;      /* 限制值系数小端    U0.12 */

	ts_u16 u16_imp_l                ;      /* 重要区域低段    U14.0 */
	ts_u16 u16_imp_l_delta          ;      /* 重要区域低段过渡    U14*/
	ts_u16 u16_imp_h         		;      /* 重要区域高段    U14 */
	ts_u16 u16_imp_h_delta          ;      /* 重要区域高段过渡    U14*/
	ts_u16 u16_imp_adj_clip         ;      /* 重要区域调整最小值    U10*/

	ts_u16 u16_modify_low           ;
	ts_u16 u16_modify_hig           ;

	ts_u16 u16_valid_num_low        ;	   /* 重分配丢弃像素低阈值 U8.0 */
	ts_u16 u16_valid_num_hig        ;	   /* 重分配丢弃像素高阈值 U11.0 */

	ts_u16 u16_gamma                ;      /* gamma    U4.12 */
	ts_u16 u16_gamma_rev            ;      

}ts_isp_agc_v01_cfg_s_t; 


typedef struct ts_isp_agc_02_cfg_s         
{
	/* 有效范围搜索 */
	ts_u08 u08_mode                ;      /* 模式 选择U8.0  bit0: 0:自动分析有效范围, 1：手动指定有效范围, bit1: 0:分配平均分, 1：分配按照像素个数分, bit6：均匀面抑制使能*/
	ts_u08 u08_bound_ratio         ;      /* 边界融合比例, u8.0,范围[0:255] */
	ts_u08 u08_upd_speed           ;      /* 更新速度, u8.0,范围[0:255] */
	ts_u08 u08_blend_ratio         ;      /* 线性非线性混合比例, u8.0,范围[0:255] */

	ts_u16 u16_loc_min             ;      /* 手动指定的截止灰度最小值, s32_loc_mode 为1时生效,  U14.0 */
	ts_u16 u16_loc_max             ;      /* 手动指定的截止灰度最大值, s32_loc_mode 为1时生效,  U14.0 */

	ts_u32 u32_stat_start          ;      /* 有效范围搜寻左抛点, U22.0 */
	ts_u32 u32_stat_end            ;      /* 有效范围搜寻右抛点, U22.0 */

	/*映射范围*/
	ts_u16 u16_coef              	;      /* 拉伸系数      U10.4 */
	ts_u16 u16_mid_pos            	;      /* 中心点        U14.0 */
	ts_u16 u16_range_limit_min      ;      /* 范围限制      U14.0 */ 
	ts_u16 u16_range_limit_max      ;      /* 范围限制      U15.0 */ 
	ts_u16 u16_limit_min          	;      /* 限制值小端    U14.0 */
	ts_u16 u16_limit_max          	;      /* 限制值大端    U14.0 */

    ts_u16 u16_plt_thr_low          ;      /* 平台直方图低阈值  U0.16 */
    ts_u16 u16_plt_thr_hig          ;      /* 平台直方图高阈值  U0.16 */

    ts_u16 u16_max_step             ;      /* 平台直方图低阈值  U16.0 */
    ts_u16 u16_gamma                ;      /* gamma  u3.12*/

    ts_u16 u16_target               ;      /* 目标亮度  u14.0,范围[0:16383]*/
    ts_u16 u16_tgt_birt             ;      /* 亮处压制调整范围  u14.0,范围[0:16383]*/
    ts_u16 u16_tgt_dark             ;      /* 暗处提升调整范围  u14.0,范围[0:16383]*/
    ts_u16 u16_bg_adj               ;      /* 背景调整参数  u14.0,范围[0:16383]*/
    ts_u16 u16_ctr_str              ;      /* 对比度调整参数  u4.12,范围[0:65535] */
    ts_s16 s16_ctr_delta            ;      /* 对比度限制参数  s9.5,范围[-16383:16383] */

    ts_u16 u16_unif_thr_0           ;     /* 均匀面判断0  U10 */ 
    ts_u16 u16_unif_thr_1           ;  	  /* 均匀面判断1  U10 */ 
    ts_u16 u16_unif_coef_0          ;     /* 均匀面系数0  U3.10 */ 
    ts_u16 u16_unif_coef_1          ;  	  /* 均匀面系数0  U3.10 */ 	
	
    ts_u16 au16_rev[10]             ;    

}ts_isp_agc_v02_cfg_s_t; 


typedef struct ts_isp_sence_cfg_s         
{
    ts_u08 u08_enable;          /*模块使能，U8.0, 取值范围[0:255], bit0:均匀面,方差计算使能*/       
    ts_u08 u08_noise_mode;      /*噪声判断模式, U8.0, 取值范围[0:255], 0:计算模式1， 1:计算模式2， 2:计算模式3*/	
    ts_u08 u08_smooth;		
    ts_u08 au08_rev[1];
    ts_u16 u16_unisf_thr;       /* 均匀面判断阈值, U14.0, 取值范围[0:16383]，值越大，判断越严格 */			
    ts_u16 au16_rev[1];
}ts_isp_sence_cfg_s_t; 

typedef struct ts_isp_nr2d_raw_cfg_s         
{
	ts_u08 u08_enable				     ;  /* 模块使能, U8.0, 取值范围[0:288], bit0: C1: 0:关闭, 1:开启, bit1: C0: 1:关闭, 0:开启*/			
	ts_u08 u08_c0_mode				     ;  /* c0降噪模式, U2.0, 取值范围[0:3] */	
	ts_u08 au08_rev0[2]				     ;
	ts_u16 u16_c0_nr_thrl			     ;  /* c0小范围降噪阈值, U10.0, 取值范围[0:1023] */	
	ts_u16 u16_c0_nr_thrh			     ;  /* c0大范围降噪阈值, U10.0, 取值范围[0:1023] */	

	ts_u16 u16_c1_nr_thr  				 ;  /* c1降噪阈值, U10.0, 取值范围[0:1023] */	
	ts_u16 u16_c1_detail_thr  			 ;  /* c1细节判断阈值, U10.0, 取值范围[0:1023] */
	ts_u16 u16_c1_sm_str  			     ;  /* c1平滑强度, U10.0, 取值范围[0:1023] */
	ts_u16 u16_c1_sm_bld 			     ;  /* c1平滑强度混合, U1.0, 取值范围[0:1] */

 	ts_u16 u16_enh_low  				 ;  /* 增强低阈值, U8.0, 取值范围[0:255] */	
	ts_u16 u16_enh_hig  			     ;  /* 增强高阈值, U10.0, 取值范围[0:1023] */	
	ts_u16 u16_enh_str  			     ;  /* 增强系数, U2.8, 取值范围[0:1023] */	
	ts_u16 u16_enh_rev 			         ;  

    ts_u16 u16_c1_ns_thr1				 ;  /* 细节区判断低阈值， U10.0, 取值范围[0-1023]*/
    ts_u16 u16_c1_ns_delta1				 ;  /* 细节区判断过度， U4.0, 取值范围[0-8]*/
    ts_u16 u16_c1_ns_thr2				 ;  /* 细节区判断高阈值， U10.0, 取值范围[0-1023]*/
    ts_u16 u16_c1_ns_delta2				 ;  /* 细节区判断过度， U4.0, 取值范围[0-8]*/
    ts_u08 u08_c1_ns_coef0				 ;  /* 噪声区降噪强度调整系数， U3.5, 取值范围[0-255]*/
    ts_u08 u08_c1_ns_coef1				 ;  /* 过度区降噪强度调整系数， U3.5, 取值范围[0-255]*/
    ts_u08 u08_c1_ns_coef2				 ;  /* 细节区降噪强度调整系数， U3.5, 取值范围[0-255]*/
    ts_u08 u08_rev1						 ;

    ts_u16 u16_c1_adj_thr1				 ;   /* 细节区判断低阈值， U10.0, 取值范围[0-1023]*/
    ts_u16 u16_c1_adj_delta1			 ;   /* 细节区判断过度， U4.0, 取值范围[0-8]*/
    ts_u16 u16_c1_adj_thr2				 ;   /* 细节区判断高阈值， U10.0, 取值范围[0-1023]*/
    ts_u16 u16_c1_adj_delta2			 ;   /* 细节区判断过度， U4.0, 取值范围[0-8]*/
    ts_u08 u08_c1_ni_adj_coef0		     ;   /* 噪声区降噪强度调整系数， U3.5, 取值范围[0-255]*/
    ts_u08 u08_c1_ni_adj_coef1			 ;   /* 过度区降噪强度调整系数， U3.5, 取值范围[0-255]*/
    ts_u08 u08_c1_ni_adj_coef2           ;   /* 细节区降噪强度调整系数， U3.5, 取值范围[0-255]*/
	ts_u08 u08_rev2						 ;
    ts_u08 u08_c1_de_adj_coef0           ;   /* 噪声区降噪强度调整系数， U3.5, 取值范围[0-255]*/
    ts_u08 u08_c1_de_adj_coef1           ;   /* 过度区降噪强度调整系数， U3.5, 取值范围[0-255]*/
    ts_u08 u08_c1_de_adj_coef2           ;   /* 细节区降噪强度调整系数， U3.5, 取值范围[0-255]*/
	ts_u08 u08_rev3						 ;

    ts_u16 u16_noise_thr_low             ; /* 噪声判断低阈值 U10.0, 取值范围[0-1023] */
    ts_u16 u16_noise_thr_low_delta             ; /* 噪声判断低阈值 U10.0, 取值范围[0-1023] */
    ts_u16 u16_noise_thr_hig             ; /* 噪声判断低阈值 U10.0, 取值范围[0-1023] */
    ts_u16 u16_noise_thr_hig_delta             ; /* 噪声判断低阈值 U10.0, 取值范围[0-1023] */

    ts_u16 u16_noise_c00_r_low           ; /* 调整系数 U2.10, 取值范围[0-4095] */
    ts_u16 u16_noise_c00_r_mid           ; /* 调整系数 U2.10, 取值范围[0-4095] */
    ts_u16 u16_noise_c00_r_hig           ; /* 调整系数 U2.10, 取值范围[0-4095] */

    ts_u16 u16_noise_c01_r_low           ; /* 调整系数 U2.10, 取值范围[0-4095] */
    ts_u16 u16_noise_c01_r_mid           ; /* 调整系数 U2.10, 取值范围[0-4095] */
    ts_u16 u16_noise_c01_r_hig           ; /* 调整系数 U2.10, 取值范围[0-4095] */

    ts_u16 u16_noise_c1_r_low            ; /* 调整系数 U2.10, 取值范围[0-4095] */
    ts_u16 u16_noise_c1_r_mid            ; /* 调整系数 U2.10, 取值范围[0-4095] */
    ts_u16 u16_noise_c1_r_hig            ; /* 调整系数 U2.10, 取值范围[0-4095] */

    ts_u16 u16_noise_sm_r_low            ; /* 调整系数 U2.10, 取值范围[0-4095] */
    ts_u16 u16_noise_sm_r_mid            ; /* 调整系数 U2.10, 取值范围[0-4095] */
    ts_u16 u16_noise_sm_r_hig            ; /* 调整系数 U2.10, 取值范围[0-4095] */

    ts_u16 u16_noise_enh_r_low            ; /* 调整系数 U2.10, 取值范围[0-4095] */
    ts_u16 u16_noise_enh_r_mid            ; /* 调整系数 U2.10, 取值范围[0-4095] */
    ts_u16 u16_noise_enh_r_hig            ; /* 调整系数 U2.10, 取值范围[0-4095] */
    ts_u16 u16_noise_rev                  ; 

}ts_isp_nr2d_raw_cfg_s_t; 

typedef struct ts_isp_nr2d_yuv_cfg_s         
{
	ts_u08 u08_c0_en				     ;  /* 模块使能, U1.0, 取值范围[0:1], 0:关闭, 1:开启 */			
	ts_u08 u08_c1_en				     ;  /* 模块使能, U1.0, 取值范围[0:1], 0:关闭, 1:开启 */			
	ts_u08 u08_c0_mode				     ;  /* c0降噪模式, U2.0, 取值范围[0:3] */	
	ts_u08 u08_rev0				         ; 

	ts_u08 u08_c0_nr_thrl			     ;  /* c0小范围降噪阈值, U8.0, 取值范围[0:255] */	
	ts_u08 u08_c0_nr_thrh			     ;  /* c0大范围降噪阈值, U8.0, 取值范围[0:255] */	
	ts_u08 au08_rev1[2]				     ; 

    ts_u08 u08_c1_noise_mode			 ;  /* 噪声判断模式， U2.0, 取值范围[0-3] */
    ts_u08 u08_c1_detail_mode		     ;  /* 细节判断模式， U2.0, 取值范围[0-3] */
    ts_u16 u16_c1_de_thr			     ;  /* 整体降噪强度， U10.0, 取值范围[0-1023] */

    ts_u08 u08_c1_noise_adj_thr1		 ;  /* 噪声判断低阈值， U8.0, 取值范围[0-255]*/
    ts_u08 u08_c1_noise_adj_delta1       ;  /* 噪声判断过度，   U3.0, 取值范围[0-7] */
    ts_u08 u08_c1_noise_adj_thr2         ;  /* 噪声判断高阈值， U8.0, 取值范围[0-255] */
    ts_u08 u08_c1_noise_adj_delta2       ;  /* 噪声判断过度，   U3.0, 取值范围[0-7] */
    ts_u08 u08_c1_noise_adj_coef0        ;  /* 平坦区降噪强度调整系数，U3.5, 取值范围[0-255] */
    ts_u08 u08_c1_noise_adj_coef1        ;  /* 过度区降噪强度调整系数，U3.5, 取值范围[0-255] */
    ts_u08 u08_c1_noise_adj_coef2        ;  /* 细节区降噪强度调整系数，U3.5, 取值范围[0-255] */
    ts_u08 u08_rev2						 ;

    ts_u08 u08_c1_detail_adj_thr1        ;  /* 噪声判断低阈值， U8.0, 取值范围[0-255]  */
    ts_u08 u08_c1_detail_adj_delta1      ;  /* 噪声判断过度，   U3.0, 取值范围[0-7]   */
    ts_u08 u08_c1_detail_adj_thr2        ;  /* 噪声判断高阈值， U8.0, 取值范围[0-255]  */
    ts_u08 u08_c1_detail_adj_delta2      ;  /* 噪声判断过度，   U3.0, 取值范围[0-7]  */
    ts_u08 u08_c1_detail_adj_coef0       ;  /* 平坦区降噪强度调整系数，U3.5, 取值范围[0-255] */
    ts_u08 u08_c1_detail_adj_coef1       ;  /* 过度区降噪强度调整系数，U3.5, 取值范围[0-255] */
    ts_u08 u08_c1_detail_adj_coef2       ;  /* 细节区降噪强度调整系数，U3.5, 取值范围[0-255] */
    ts_u08 u08_rev3                      ;

    ts_u16 u16_noise_thr_low               ; /* 噪声判断低阈值 U12.0, 取值范围[0-4095] */
    ts_u16 u16_noise_thr_low_delta         ; /* 噪声判断低阈值 U12.0, 取值范围[0-4095] */
    ts_u16 u16_noise_thr_hig               ; /* 噪声判断高阈值 U12.0, 取值范围[0-4095] */
    ts_u16 u16_noise_thr_hig_delta         ; /* 噪声判断高阈值 U12.0, 取值范围[0-4095] */

    ts_u16 u16_noise_c0l_r_low             ; /* 第一级降噪调整系数 U2.10, 取值范围[0-4095] */
    ts_u16 u16_noise_c0l_r_mid             ; /* 第一级降噪调整系数 U2.10, 取值范围[0-4095] */
    ts_u16 u16_noise_c0l_r_hig             ; /* 第一级降噪调整系数 U2.10, 取值范围[0-4095] */

    ts_u16 u16_noise_c0h_r_low             ; /* 第二级降噪调整系数 U2.10, 取值范围[0-4095] */
    ts_u16 u16_noise_c0h_r_mid             ; /* 第二级降噪调整系数 U2.10, 取值范围[0-4095] */
    ts_u16 u16_noise_c0h_r_hig             ; /* 第二级降噪调整系数 U2.10, 取值范围[0-4095] */

    ts_u16 u16_noise_c1_r_low              ; /* 第三级调整系数 U2.10, 取值范围[0-4095] */
    ts_u16 u16_noise_c1_r_mid              ; /* 第三级调整系数 U2.10, 取值范围[0-4095] */
    ts_u16 u16_noise_c1_r_hig              ; /* 第三级调整系数 U2.10, 取值范围[0-4095] */
    ts_u16 u16_noise_rev                   ;
}ts_isp_nr2d_yuv_cfg_s_t; 

typedef struct ts_isp_nr2d_cfg_s   
{
	ts_isp_nr2d_raw_cfg_s_t     st_nr2d_raw;
	ts_isp_nr2d_yuv_cfg_s_t 	st_nr2d_yuv;	
}ts_isp_nr2d_cfg_s_t;

typedef struct ts_isp_nr3d_cfg_s         
{
    ts_u08 u08_en                    ;   /* 降噪使能，U1.0, 取值范围[0-1] */
    ts_u08 u08_gbl_en                ;   /* 降噪使能，U1.0, 取值范围[0-1] */
    ts_u08 u08_gbl_str               ;   /* 强度，U8.0, 取值范围[0 - 255] */
    ts_u08 au08_rev0[1]              ;

    ts_u08 u08_diff_flt_vsel         ;   /* diff滤波器选择，U1.0, 取值范围[0-1] */
    ts_u08 u08_diff_flt_hsel         ;   /* diff滤波器选择，U2.0, 取值范围[0-3] */
    ts_u08 u08_inter_flt_sel         ;   /* 背景滤波器选择，U2.0, 取值范围[0-2] */
    ts_u08 u08_inter_coef            ;   /* 运动判断滤波，U8.0, 取值范围[0-255] */

    ts_u16 u16_motion                ;   /* 运动判断，U12.0, 取值范围[0-4095] */
    ts_u08 au08_rev1[2]              ;

    ts_u08 u08_alpha_ratio           ;   /* 背景判断放大系数，U8.0, 取值范围[0 - 255] */
    ts_u08 au08_rev2[1]              ;
    ts_u16 u16_alpha_max             ;   /* 背景放大最大限制值, u12, 取值范围[0 - 4095] */

    ts_u08 u08_mv_flt_size           ;   /* 降噪窗口，U2.0, 取值范围[0-3], 值越大窗越大 */
    ts_u08 u08_mv_flt_str            ;   /* 运动降噪强度, 当u08_mv_flt_size < 2时生效， u8.0, 取值范围[0 - 255] */
    ts_u16 u16_mv_flt_str_l          ;   /* 运动降噪强度, 当u08_mv_flt_size >= 2时生效，u10.0, 取值范围[0 - 1023] */
    
    ts_u08 u08_mv_ratio              ;   /* 前景判断放大系数，U8.0, 取值范围[0 - 255] */
    ts_u08 au08_rev4[1]              ;
    ts_u16 u16_mv_max                ;   /* 前景放大最大限制值, u12.0, 取值范围[0 - 4095] */

    ts_u08 u08_diff_fus_clip_en      ; /* 降噪限制使能，U1.0, 取值范围[0-1] */
    ts_u08 au08_rev5[3]              ;

    ts_u16 u16_diff_fus_th           ; /* diff融合阈值, u10.0, 取值范围[0 - 1023]   */
    ts_u08 u08_diff_fus_delta        ; /* diff融合过度, u4.0, 取值范围[0 - 15]   */
    ts_u08 au08_rev6[1]              ;

    ts_u08 u08_diff_fus_coef1        ; /* diff融合系数, u8.0, 取值范围[0 - 255]   */
    ts_u08 u08_diff_fus_coef0        ; /* diff融合系数, u8.0, 取值范围[0 - 255]   */
    ts_u08 au08_rev7[2]              ;

    ts_u16 u16_tail_fus_th           ; /* 拖尾融合阈值, u10.0, 取值范围[0 - 1023]  */
    ts_u08 u08_tail_fus_delta        ; /* diff融合过度, u4.0, 取值范围[0 - 15]   */
    ts_u08 au08_rev8[1]              ;

    ts_u08 u08_tail_fus_coef1        ; /* 拖尾融合系数, u8.0, 取值范围[0 - 255]  */
    ts_u08 u08_tail_fus_coef0        ; /* 拖尾融合系数, u8.0, 取值范围[0 - 255]  */
    ts_u08 au08_rev9[2]              ;

    ts_u16 u16_noise_thr_low         ; /* 噪声判断低阈值 U12.0, 取值范围[0-4095] */
    ts_u16 u16_noise_thr_low_delta   ; /* 噪声判断低阈值 U12.0, 取值范围[0-4095] */
    ts_u16 u16_noise_thr_hig         ; /* 噪声判断高阈值 U12.0, 取值范围[0-4095] */
    ts_u16 u16_noise_thr_hig_delta   ; /* 噪声判断高阈值 U12.0, 取值范围[0-4095] */

    ts_u16 u16_noise_m_r_low         ; /* 动态降噪调整系数 U2.10, 取值范围[0-4095] */
    ts_u16 u16_noise_m_r_mid         ; /* 动态降噪调整系数 U2.10, 取值范围[0-4095] */
    ts_u16 u16_noise_m_r_hig         ; /* 动态降噪调整系数 U2.10, 取值范围[0-4095] */
    ts_u16 u16_noise_s_r_low         ; /* 静态降噪调整系数 U2.10, 取值范围[0-4095] */
    ts_u16 u16_noise_s_r_mid         ; /* 静态降噪调整系数 U2.10, 取值范围[0-4095] */
    ts_u16 u16_noise_s_r_hig         ; /* 静态降噪调整系数 U2.10, 取值范围[0-4095] */
}ts_isp_nr3d_cfg_s_t; 

typedef struct ts_isp_snc_cfg_s         
{
    ts_u08 u08_snc_en               ;  /* 去纹使能，U1.0, 取值范围[0-1] */
    ts_u08 u08_snc_fix_en           ;  /* 去纹使能，U1.0, 取值范围[0-1] */
    ts_u08 u08_snc_fix_clip_en      ;  /* 去纹限制使能，U1.0, 取值范围[0-1] */
    ts_u08 u08_snc_hist_cor_en      ; 

    ts_u08 u08_snc_sm_flt_size      ;  /* 去纹平滑滤波窗，U1.0, 取值范围[0-1] */
    ts_u08 u08_snc_sm_flt_str       ;  /* 去纹平滑滤强度，U6.0, 取值范围[0-63] */
    ts_u08 u08_snc_mode             ;  /* 模式选择，U1.0, 取值范围[0-1] */
    ts_u08 au08_rev1[1]             ; 

    ts_u16 u16_snc_juthr_v          ;  /* 判断阈值，U9.0, 取值范围[0-511] */
    ts_u16 u16_snc_juthr_n          ;  /* 判断阈值，U9.0, 取值范围[0-511] */   

    ts_u08 u08_snc_judelta_n        ;  /* 判断阈值过度，U3.0, 取值范围[0-7] */
    ts_u08 u08_snc_rm_flt_size      ;  /* 去纹窗口大小，U4.0, 取值范围[0-15] */
    ts_u16 u16_snc_noise_clip       ;  /* 噪声阈值，U10.0, 取值范围[0-1023] */

    ts_u08 u08_snc_noise_delta      ;   /* 噪声阈值过渡，U4.0, 取值范围[0-15] */
    ts_u08 u08_snc_fix_dif_max      ;  /* 去纹平滑滤强度，U6.0, 取值范围[0-63] */
    ts_u16 u16_snc_fix_noise_clip   ;  /* 噪声阈值，U10.0, 取值范围[0-1023] */

    ts_u08 au08_rev3[4]             ; 

}ts_isp_snc_cfg_s_t; 

typedef struct ts_isp_lce_cfg_s         
{
    ts_u08 u08_en                          ;  /* lce使能，U8.0, 取值范围[0-255] */
    ts_u08 au08_rev0[3]                    ; 
           
    ts_u08 u08_bgs_win_size                ;  /* 小背景滤波窗口，U2.0, 取值范围[0-3] */
    ts_u08 u08_bgl_blk_size                ;  /* 大背景分块大小，U2.0, 取值范围[0-3] */
    ts_u08 u08_bgl_win_size                ;  /* 大背景滤波窗口，U1.0, 取值范围[0-1] */
    ts_u08 au08_rev1[1]                    ; 
    
	ts_u16 u16_bgs_flt_thr00               ;  /* 小背景滤波阈值，U9.0, 取值范围[0-511] */      
    ts_u08 u08_bgs_flt_delta               ;  /* 小背景滤波过度，U4.0, 取值范围[0-8]   */
    ts_u08 u08_bgs_blend                   ;  /* 小背景混合系数，U5.0, 取值范围[0-31]  */

    ts_u08 u08_bgs_luma                    ;  /* 小背景亮度调整，U5.0, 取值范围[0-31]  */
    ts_u08 u08_bgs_ctr                     ;  /* 小背景对比度调整，U6.0, 取值范围[0-63] */        
    ts_u16 u16_bgl_enh                     ;  /* 大背景滤波强度，U15.0, 取值范围[0-32767] */
           
    ts_u16 u16_bgl_flt_str                 ;  /* 大背景滤波强度，U15.0, 取值范围[0-32767] */
    ts_u16 u16_bgl_ctr                     ;  /* 大背景对比度强度，U12.0, 取值范围[0-4095] */
    ts_u16 u16_bgl_clip                    ;  /* 大背景滤波限幅，小于该值则做抑制，U14.0, 取值范围[0-16383] */
    ts_s16 s16_bgl_delta                   ;  /* 大背景增强过渡，值越大对比度抑制越强，s12.0, 取值范围[-4095-4095] */

    ts_u16 u16_clip_l                      ; /* 小于u16_bgl_clip时的大范围细节的系数，U8.0, 取值范围[0-255]     */
    ts_u16 u16_clip_r                      ; /* 拉伸限制系数，U12.0, 取值范围[0-4095] */

    ts_u16 u16_noise_thr_low               ; /* 噪声判断低阈值 U10.0, 取值范围[0-1023] */
    ts_u16 u16_noise_thr_low_delta         ; /* 噪声判断低阈值 U10.0, 取值范围[0-1023] */
    ts_u16 u16_noise_thr_hig               ; /* 噪声判断低阈值 U10.0, 取值范围[0-1023] */
    ts_u16 u16_noise_thr_hig_delta         ; /* 噪声判断低阈值 U10.0, 取值范围[0-1023] */
   
    ts_u16 u16_noise_bgs_l_r_low            ; /* 小背景亮度调整系数 U2.10, 取值范围[0-4095] */
    ts_u16 u16_noise_bgs_l_r_mid            ; /* 小背景亮度调整系数 U2.10, 取值范围[0-4095] */
    ts_u16 u16_noise_bgs_l_r_hig            ; /* 小背景亮度调整系数 U2.10, 取值范围[0-4095] */

    ts_u16 u16_noise_bgs_c_r_low            ; /* 小背景对比度调整系数 U2.10, 取值范围[0-4095] */
    ts_u16 u16_noise_bgs_c_r_mid            ; /* 小背景对比度调整系数 U2.10, 取值范围[0-4095] */
    ts_u16 u16_noise_bgs_c_r_hig            ; /* 小背景对比度调整系数 U2.10, 取值范围[0-4095] */

    ts_u16 u16_noise_bgl_c_r_low             ; /* 大背景对比度调整系数 U2.10, 取值范围[0-4095] */
    ts_u16 u16_noise_bgl_c_r_mid             ; /* 大背景对比度调整系数 U2.10, 取值范围[0-4095] */
    ts_u16 u16_noise_bgl_c_r_hig             ; /* 大背景对比度调整系数 U2.10, 取值范围[0-4095] */

    ts_u16 u16_noise_str_r_low               ; /*  整体调整系数 U2.10, 取值范围[0-4095] */
    ts_u16 u16_noise_str_r_mid               ; /*  整体调整系数 U2.10, 取值范围[0-4095] */
    ts_u16 u16_noise_str_r_hig               ; /*  整体调整系数 U2.10, 取值范围[0-4095] */
    ts_u16 u16_noise_rev[2]                  ; 

    ts_u08 au08_adj_thr[4]                  ; /* lce 调整阈值， U0.8 */
    ts_u08 au08_adj_str[4]                  ; /* lce 调整强度， U0.8 */
   
   	ts_u64 au64_rev_p[3]                  ;     
}ts_isp_lce_cfg_s_t; 

typedef struct ts_isp_dde_cfg_s         
{
    ts_u08 u08_en                   ;  /* 使能，U8.0, 取值范围[0-255], bit0:模块使能， bit1：大背景调整使能 */
    ts_u08 au08_rev0[3]             ; 

    ts_u08 u08_det_judge            ; /*细节判断模式，U1.0, 取值范围[0-1] */
    ts_u08 u08_lc_acc               ; /*lc 精度，U3.0, 取值范围[1-6] */
    ts_u08 u08_lc_str               ; /*lc增强强度，U3.4, 取值范围[0-255] */
    ts_u08 u08_lc_clip              ; /*lc限制，U8.0, 取值范围[0-255] */

    ts_u08 u08_lc_coef_10           ; /*LC 滤波器系数, U0.8, 取值范围[0-255]*/
    ts_u08 u08_lc_coef_11           ; /*LC 滤波器系数, U0.8, 取值范围[0-255]*/
    ts_u08 u08_lc_coef_12           ; /*LC 滤波器系数, U0.8, 取值范围[0-255]*/
    ts_u08 au08_rev1[1]             ;

    ts_u08 u08_lc_coef_20           ; /*LC 滤波器系数, U0.8, 取值范围[0-255]*/
    ts_u08 u08_lc_coef_21           ; /*LC 滤波器系数, U0.8, 取值范围[0-255]*/
    ts_u08 u08_lc_coef_22           ; /*LC 滤波器系数, U0.8, 取值范围[0-255]*/
    ts_u08 au08_rev2[1]             ; 

    ts_u16 u16_delta_lc             ; /* lc限制强度,U16.0, 取值范围[0-65535] */ 
    ts_u16 au16_rev3[1]             ; 

    ts_u16 u16_det_flt_str          ; /* 细节滤波强度,U12.0, 取值范围[0-4095] */ 
    ts_u08 u08_det_clip             ; /* 细节clip操作,U8.0, 取值范围[0-255]  */ 
    ts_u08 u08_det_clip_delta       ; /* 细节clip操作过度,U4.0, 取值范围[0-7]  */ 

    ts_u08 u08_det_enh_str          ; /* 高频增强强度, U4.4， 值越大细节增强越强, 取值范围[0-255] */ 
    ts_u08 au08_rev4[3]             ; 

    ts_u16 u16_delta_det            ; /* det限制强度,U16.0, 取值范围[0-65535] */ 
    ts_u16 u16_lcl_nr               ; /* lcl 细节降噪,值越小，则降噪越强，U9.0, 取值范围[0-256] */ 

    ts_u16 u16_lcl_flt_thr           ; /* lcl滤波强度,U14.0, 取值范围[0-16383] */ 
    ts_u16 u16_lcl_flt_str           ; /* lcl增强强度,U7.5, 取值范围[0-4095] */ 
    ts_u16 u16_lcl_clip              ; /* lcl限制值,U10.0, 取值范围[0-1023]， 当大范围细节小于当前值的时，则进行抑制 */ 
    ts_u16 u16_delta_lcl             ; /* lcl限制强度,U10.0, 取值范围[0-1023] */ 

    ts_u16 u16_lum_adj_thr1         ;  /* 亮度判断低阈值， U10.0, [0-1023]*/
    ts_u16 u16_lum_adj_delta1       ;  /* 亮度判断过度，   U4.0, [0-8]*/
    ts_u16 u16_lum_adj_thr2         ;  /* 亮度判断高阈值， U10.0, [0-1023]*/
    ts_u16 u16_lum_adj_delta2       ;  /* 亮度判断过度，   U4.0, [0-8]*/
    ts_u08 u08_lum_lc_adj_coef0     ;  /* 根据亮度调整细节， U3.5, [0-255]*/
    ts_u08 u08_lum_lc_adj_coef1     ;  /* 根据亮度调整细节， U3.5, [0-255]*/
    ts_u08 u08_lum_lc_adj_coef2     ;  /* 根据亮度调整细节， U3.5, [0-255]*/
    ts_u08 u08_lum_det_adj_coef0    ;  /* 根据亮度调整细节， U3.5, [0-255]*/ 
    ts_u08 u08_lum_det_adj_coef1    ;  /* 根据亮度调整细节， U3.5, [0-255]*/
    ts_u08 u08_lum_det_adj_coef2    ;  /* 根据亮度调整细节， U3.5, [0-255]*/
    ts_u08 au08_rev6[2]; 

    ts_u16 u16_det_adj_thr1         ;  /* 细节判断低阈值， U10.0, [0-1023]*/
    ts_u16 u16_det_adj_delta1       ;  /* 细节判断过度，   U4.0, [0-8]*/
    ts_u16 u16_det_adj_thr2         ;  /* 细节判断高阈值， U10.0, [0-1023]*/
    ts_u16 u16_det_adj_delta2       ;  /* 细节判断过度，   U4.0, [0-8]*/
    ts_u08 u08_det_lc_adj_coef0     ;  /* 根据细节调整细节， U3.5, [0-255]*/
    ts_u08 u08_det_lc_adj_coef1     ;  /* 根据细节调整细节， U3.5, [0-255]*/
    ts_u08 u08_det_lc_adj_coef2     ;  /* 根据细节调整细节， U3.5, [0-255]*/
    ts_u08 u08_det_det_adj_coef0    ;  /* 根据细节调整细节， U3.5, [0-255]*/ 
    ts_u08 u08_det_det_adj_coef1    ;  /* 根据细节调整细节， U3.5, [0-255]*/
    ts_u08 u08_det_det_adj_coef2    ;  /* 根据细节调整细节， U3.5, [0-255]*/
    ts_u08 au08_rev7[2];  

    ts_u16 u16_noi_adj_thr1         ;  /* 噪声判断低阈值， U10.0, [0-1023]*/
    ts_u16 u16_noi_adj_delta1       ;  /* 噪声判断过度，   U4.0, [0-8]*/
    ts_u16 u16_noi_adj_thr2         ;  /* 噪声判断高阈值， U10.0, [0-1023]*/
    ts_u16 u16_noi_adj_delta2       ;  /* 噪声判断过度，   U4.0, [0-8]*/
    ts_u08 u08_noi_lc_adj_coef0     ;  /* 根据噪声调整细节， U3.5, [0-255]*/
    ts_u08 u08_noi_lc_adj_coef1     ;  /* 根据噪声调整细节， U3.5, [0-255]*/
    ts_u08 u08_noi_lc_adj_coef2     ;  /* 根据噪声调整细节， U3.5, [0-255]*/
    ts_u08 u08_noi_det_adj_coef0    ;  /* 根据噪声调整细节， U3.5, [0-255]*/ 
    ts_u08 u08_noi_det_adj_coef1    ;  /* 根据噪声调整细节， U3.5, [0-255]*/
    ts_u08 u08_noi_det_adj_coef2    ;  /* 根据噪声调整细节， U3.5, [0-255]*/
    ts_u08 au08_rev8[2];  

    ts_u16 u16_noise_thr_low        ; /* 噪声判断低阈值 U12.0, 取值范围[0-4095] */
    ts_u16 u16_noise_thr_low_delta  ; /* 噪声判断低阈值 U12.0, 取值范围[0-4095] */
    ts_u16 u16_noise_thr_hig        ; /* 噪声判断低阈值 U12.0, 取值范围[0-4095] */
    ts_u16 u16_noise_thr_hig_delta  ; /* 噪声判断低阈值 U12.0, 取值范围[0-4095] */
   
    ts_u16 u16_noise_lc_r_low       ; /* 调整系数 U0.10, 取值范围[0-2048] */
    ts_u16 u16_noise_lc_r_mid       ; /* 调整系数 U0.10, 取值范围[0-2048] */
    ts_u16 u16_noise_lc_r_hig       ; /* 调整系数 U0.10, 取值范围[0-2048] */

    ts_u16 u16_noise_det_r_low      ; /* 调整系数 U0.10, 取值范围[0-2048] */
    ts_u16 u16_noise_det_r_mid      ; /* 调整系数 U0.10, 取值范围[0-2048] */
    ts_u16 u16_noise_det_r_hig      ; /* 调整系数 U0.10, 取值范围[0-2048] */

    ts_u16 u16_noise_det_l_low      ; /* 调整系数 U0.10, 取值范围[0-2048] */
    ts_u16 u16_noise_det_l_mid      ; /* 调整系数 U0.10, 取值范围[0-2048] */
    ts_u16 u16_noise_det_l_hig      ; /* 调整系数 U0.10, 取值范围[0-2048] */

    ts_u16 u16_noise_lcl_r_low      ; /* 调整系数 U0.10, 取值范围[0-2048] */
    ts_u16 u16_noise_lcl_r_mid      ; /* 调整系数 U0.10, 取值范围[0-2048] */
    ts_u16 u16_noise_lcl_r_hig      ; /* 调整系数 U0.10, 取值范围[0-2048] */
	
    ts_u16 u16_noise_lcl_f_low      ; /* 调整系数 U0.10, 取值范围[0-2048] */
    ts_u16 u16_noise_lcl_f_mid      ; /* 调整系数 U0.10, 取值范围[0-2048] */
    ts_u16 u16_noise_lcl_f_hig      ; /* 调整系数 U0.10, 取值范围[0-2048] */

    ts_u16 au16_rev_noise[1]        ;

	ts_u64 au64_rev_p[2];
    
 } ts_isp_dde_cfg_s_t;


typedef struct ts_isp_ee_cfg_s
{
    ts_u08 u08_en                   ;  /* 使能，U1.0, 取值范围[0-1] */
    ts_u08 u08_str_pos              ;  /*白边强度， U3.5*/
    ts_u08 u08_str_neg              ;  /*黑边强度， U3.5*/
    ts_u08 u08_rev_1;

    ts_u08 u08_flt_pre               ; /* 前置滤波， U8.0, 取值范围[0-255]  */ 
    ts_u08 u08_bld_pre               ; /* 滤波混和模式，0更加平滑，1细节更好， U1.0, 取值范围[0-1]  */ 
    ts_u08 u08_edge_mode             ; /* 边缘模式，0小细节更好，1边缘更好，U1.0, 取值范围[0-1]  */ 
    ts_u08 u08_sm_str                ; /* 边缘平滑强度，越大边缘越平顺 U5.0, 取值范围[0-31]  */ 

    ts_u16 u16_det_flt_str           ; /* 细节滤波器强度， U16.0, 取值范围[0-65535]  */ 
    ts_u08 u08_det_flt_size          ; /* 细节滤波器选择， U1.0, 取值范围[0-1]  */ 
    ts_u08 u08_rev1                  ;

    ts_u08 u08_ee_dn_clip            ; /* 降噪限幅， U6.0, 取值范围[0-63] */ 
    ts_u08 u08_ee_dn_delta           ; /* 降噪过渡， U2.0, 取值范围[0-3] */ 
    ts_u16 u16_ee_res                ; /* 增强限幅，越大边缘抑制越强， U12.0, 取值范围[0-4095] */ 

    ts_u16 u16_base_thr1             ; /* 判断为平坦区域的阈值 U16.0, 取值范围[0-65535] */
    ts_u16 u16_base_thr2             ; /* 判断为细节区域的阈值 U16.0, 取值范围[0-65535] */
    ts_u08 u08_base_delta1           ; /* 过渡 U4.0, 取值范围[0-12] */
    ts_u08 u08_base_delta2           ; /* 过渡 U4.0, 取值范围[0-12] */
    ts_u08 u08_base_coef0            ; /* 根据细节情况调节背景噪声的系数， U3.5, 取值范围[0-255]*/
    ts_u08 u08_base_coef1            ; /* 根据细节情况调节背景噪声的系数， U3.5, 取值范围[0-255]*/
    ts_u08 u08_base_coef2            ; /* 根据细节情况调节背景噪声的系数， U3.5, 取值范围[0-255]*/
    ts_u08 u08_rev2[3]                  ;

    ts_u08 u08_edge_bld_thr1         ; /* 边缘融合阈值 U8.0, 取值范围[0-255] */
    ts_u08 u08_edge_bld_delta1       ; /* 过渡 U4.0, 取值范围[0-7] */
    ts_u08 u08_edge_bld_thr2         ; /* 边缘融合阈值 U8.0, 取值范围[0-255] */
    ts_u08 u08_edge_bld_delta2       ; /* 过渡 U4.0, 取值范围[0-7] */
    ts_u08 u08_edge_bld_coef0        ; /* 融合边缘系数， U0.5, 取值范围[0-31]*/
    ts_u08 u08_edge_bld_coef1        ; /* 融合边缘系数， U0.5, 取值范围[0-31]*/
    ts_u08 u08_edge_bld_coef2        ; /* 融合边缘系数， U0.5, 取值范围[0-31]*/
    ts_u08 u08_rev3                  ;

    ts_u08 u08_hf_bld_thr1           ; /* 细节融合阈值 U8.0, 取值范围[0-255] */
    ts_u08 u08_hf_bld_delta1         ; /* 过渡 U4.0, 取值范围[0-7] */
    ts_u08 u08_hf_bld_thr2           ; /* 细节融合阈值 U8.0, 取值范围[0-255] */
    ts_u08 u08_hf_bld_delta2         ; /* 过渡 U4.0, 取值范围[0-7] */
    ts_u08 u08_hf_bld_coef0          ; /* 融合细节系数， U0.5, 取值范围[0-31]*/
    ts_u08 u08_hf_bld_coef1          ; /* 融合细节系数， U0.5, 取值范围[0-31]*/
    ts_u08 u08_hf_bld_coef2          ; /* 融合细节系数， U0.5, 取值范围[0-31]*/
    ts_u08 u08_rev4                  ;

    ts_u08 u08_luma_adj_thr1        ; /* 亮度阈值 U8.0, 取值范围[0-255] */
    ts_u08 u08_luma_adj_delta1      ; /* 过渡 U4.0, 取值范围[0-7] */
    ts_u08 u08_luma_adj_thr2        ; /* 亮度阈值 U8.0, 取值范围[0-255] */
    ts_u08 u08_luma_adj_delta2      ; /* 过渡 U4.0, 取值范围[0-7] */
    ts_u08 u08_luma_adj_coef0       ; /* 根据亮度调整边缘系数， U3.5, 取值范围[0-255]*/
    ts_u08 u08_luma_adj_coef1       ; /* 根据亮度调整边缘系数， U3.5, 取值范围[0-255]*/
    ts_u08 u08_luma_adj_coef2       ; /* 根据亮度调整边缘系数， U3.5, 取值范围[0-255]*/
    ts_u08 u08_rev5                 ;

    ts_u16 u16_det_adj_thr1         ; /* 细节阈值 U16.0, 取值范围[0-65535] */
    ts_u16 u16_det_adj_thr2         ; /* 细节阈值 U16.0, 取值范围[0-65535] */
    ts_u08 u08_det_adj_delta1       ; /* 过渡 U4.0, 取值范围[0-12] */
    ts_u08 u08_det_adj_delta2       ; /* 过渡 U4.0, 取值范围[0-12] */
    ts_u08 u08_det_adj_coef0        ; /* 根据细节调整边缘系数， U3.5, 取值范围[0-255]*/
    ts_u08 u08_det_adj_coef1        ; /* 根据细节调整边缘系数， U3.5, 取值范围[0-255]*/
    ts_u08 u08_det_adj_coef2        ; /* 根据细节调整边缘系数， U3.5, 取值范围[0-255]*/
    ts_u08 u08_rev6[3]              ;

    ts_u08 u08_max_oft_pos          ; /* 最大偏移值 U8.0, 取值范围[0-255] */
    ts_u08 u08_max_oft_neg          ; /* 最大偏移值 U8.0, 取值范围[0-255] */
    ts_u08 u08_clip_ratio_pos       ; /* 最大偏增强系数 U8.0, 取值范围[0-255] */
    ts_u08 u08_clip_ratio_neg       ; /* 最大偏增强系数 U8.0, 取值范围[0-255] */

    ts_u16 u16_nr_thr1              ; /* 孤立点去除阈值1 U9.0, 取值范围[0-511] */
    ts_u16 u16_nr_thr2              ; /* 孤立点去除阈值2 U10.0, 取值范围[0-1023] */
    ts_u08 u08_nr_delta1            ; /* 过渡 U4.0, 取值范围[0-9] */
    ts_u08 u08_nr_delta2            ; /* 过渡 U4.0, 取值范围[0-9] */
    ts_u08 u08_rev7[2]               ; 

    ts_u16 u16_noise_thr_low        ; /* 噪声判断低阈值 U12.0, 取值范围[0-4095] */
    ts_u16 u16_noise_thr_low_delta  ; /* 噪声判断低阈值 U12.0, 取值范围[0-4095] */
    ts_u16 u16_noise_thr_hig        ; /* 噪声判断高阈值 U12.0, 取值范围[0-4095] */
    ts_u16 u16_noise_thr_hig_delta  ; /* 噪声判断高阈值 U12.0, 取值范围[0-4095] */

    ts_u08 u08_adj_flt_low          ; /* 平滑滤波调整系数 U1.7, 取值范围[0-255] */
    ts_u08 u08_adj_flt_mid          ; /* 平滑滤波调整系数 U1.7, 取值范围[0-255] */
    ts_u08 u08_adj_flt_hig          ; /* 平滑滤波调整系数 U1.7, 取值范围[0-255] */
    ts_u08 u08_adj_flt_rev          ;

    ts_u08 u08_adj_enh0_low         ; /* 小细节调整系数 U1.7, 取值范围[0-255] */
    ts_u08 u08_adj_enh0_mid         ; /* 小细节调整系数 U1.7, 取值范围[0-255] */
    ts_u08 u08_adj_enh0_hig         ; /* 小细节调整系数 U1.7, 取值范围[0-255] */
    ts_u08 u08_adj_enh0_rev         ; 

    ts_u08 u08_adj_enh1_low         ; /* 中大细节调整系数 U1.7, 取值范围[0-255] */
    ts_u08 u08_adj_enh1_mid         ; /* 中大细节调整系数 U1.7, 取值范围[0-255] */
    ts_u08 u08_adj_enh1_hig         ; /* 中大细节调整系数 U1.1.7取值范围[0-255] */
    ts_u08 u08_adj_enh1_rev         ; 

    ts_u08 u08_adj_base_low         ; /* 平坦区降噪调整系数 U1.7, 取值范围[0-255] */
    ts_u08 u08_adj_base_mid         ; /* 平坦区降噪调整系数 U1.7, 取值范围[0-255] */
    ts_u08 u08_adj_base_hig         ; /* 平坦区降噪调整系数 U1.7, 取值范围[0-255] */
    ts_u08 u08_adj_base_rev         ; 

    ts_u08 u08_adj_hf_low           ; /* 高频比例调整系数 U1.7, 取值范围[0-255] */
    ts_u08 u08_adj_hf_mid           ; /* 高频比例调整系数 U1.7, 取值范围[0-255] */
    ts_u08 u08_adj_hf_hig           ; /* 高频比例调整系数 U1.7, 取值范围[0-255] */
    ts_u08 u08_adj_hf_rev           ; 

}ts_isp_ee_cfg_s_t;


typedef struct ts_isp_fpnc_cfg_s
{
    ts_u08 u08_en                   ;  /* 使能，U1.0, 取值范围[0-1] */
    ts_u08 au08_rev0[3]             ; 

    ts_u16 u16_fpn_clip_thrl        ; /* 竖纹判断阈值， 越小越容易判断为竖纹， U12.0, 取值范围[0-4095]  */ 
    ts_u16 u16_fpn_clip_thrh        ; /* 竖纹判断阈值， 越小越容易判断为竖纹， U12.0, 取值范围[0-4095]  */ 

    ts_u16 u16_fpn_num_thrl         ; /* 竖纹判断阈值， 越大越容易判断为竖纹， U12.0, 取值范围[0-4095]  */ 
    ts_u16 u16_fpn_num_thrh         ; /* 竖纹判断阈值， 越大越容易判断为竖纹， U12.0, 取值范围[0-4095]  */

    ts_u16 u16_fpn_wt_thr           ; /* 竖纹判断阈值， 越大越容易判断为竖纹， U12.0, 取值范围[0-4095]  */ 
    ts_u16 u16_noise_clip           ; /* 噪声clip， U14.0, 取值范围[0-16383]  */ 

    ts_u16 u16_fpn_thr_modify       ; /* 阈值修正系数， U12.0, 取值范围[0-4095]  */ 
    ts_u16 u16_fpn_thr_limit        ; /* 阈值修正限制， U12.0, 取值范围[0-4095]  */ 
    ts_u16 u16_fpn_thr_weak         ; /* 弱纹理修正系数， U12.0, 取值范围[0-4095]  */ 

    ts_u08 u08_upd_speed            ;  /* 更新速度， U8.0, 取值范围[0-255]  */ 
    ts_u08 au08_rev1[1]             ; 

}ts_isp_fpnc_cfg_s_t;

typedef struct ts_isp_bpc_cfg_s
{
    ts_u08 u08_en                   ;  /* 使能，U1.0, 取值范围[0-1] */
    ts_u08 u08_rev0[3]              ;  /* 使能，U1.0, 取值范围[0-1] */

    ts_u16 u16_thr_k_min            ; /* k值判断阈值， U15.0, 取值范围[0-32767]  */ 
    ts_u16 u16_thr_k_max            ; /* k值判断阈值， U15.0, 取值范围[0-32767]  */ 

    ts_u16 au16_col_m[16]           ; /* 手动判断坏列， U11.0, 取值范围[0-2047]  */

    ts_u16 u16_thr_brit             ; /* 自动坏点阈值， U14.0, 取值范围[0-16383]  */
    ts_u16 u16_thr_dark             ; /* 自动坏点阈值， U14.0, 取值范围[0-16383]  */
    ts_u08 u08_thr_brit_delta       ; /* 自动坏点阈值过渡， U4.0, 取值范围[0-14]  */
    ts_u08 u08_thr_dark_delta       ; /* 自动坏点阈值过渡， U4.0, 取值范围[0-14]  */
    ts_u16 u16_thr_rev              ; 
}ts_isp_bpc_cfg_s_t;

typedef struct ts_isp_ltm_v00_cfg_s
{
    ts_u08 u08_en               ;  /* 使能，U8.0, 取值范围[0-255]，BIT0 模块使能， BIT1:模块加速使能，开启后CPU占用降低 */
    ts_u08 u08_smooth           ;  /* 平滑强度，U8.0, 取值范围[0-255] */
    ts_u08 u08_bld_ratio        ;  /* 融合系数，U2.6, 取值范围[0-255]， 值越大图像越自然，对比度受限 */
    ts_u08 u08_max_coef         ;  /* 最大放大倍数，U4.4, 取值范围[0-255]， 值越大图像对比度限制越宽 */             

    ts_u16 u16_clip_thr         ; /* 拉伸强度，U10.0, 取值范围[0-1023] */
    ts_u16 u16_ahl_str          ; /* 高亮抑制强度，U12.0, 取值范围[0-4095] */

    ts_u16 u16_dst_min          ;   /* 映射目标范围低阈值， U14.0 */
    ts_u16 u16_dst_max          ;   /* 映射目标范围高阈值， U14.0 */
  
    ts_u16 u16_luma_thr0        ; /* 亮度调整阈值， U14.0 */
    ts_u16 u16_luma_thr1        ; /* 亮度调整阈值， U14.0 */
    ts_u16 u16_luma_thr2        ; /* 亮度调整阈值， U14.0 */
    ts_u16 u16_luma_thr3        ; /* 亮度调整阈值， U14.0 */
    ts_u16 u16_luma_diff0       ; /* 亮度调整限幅， U14.0 */
    ts_u16 u16_luma_diff1       ; /* 亮度调整限幅， U14.0 */
    ts_u16 u16_luma_diff2       ; /* 亮度调整限幅， U14.0 */
    ts_u08 u08_min_luma_step    ; /* 最小亮度补偿， U5.0 */
    ts_u08 u08_rev_luma         ;

    ts_u08 au08_coef_zone[9]    ;  /* 区域强度调整系数 U1.7*/
    ts_u08 au08_rev_coef_zone[3];

    ts_u08 au08_ahl_zone[9]    ;  /* 区域高亮抑制调整系数 U1.7*/
    ts_u08 au08_rev_ahl_zone[3];

    ts_u16 u16_noise_mod_low         ; /* 噪声判断低阈值 U12.0, 取值范围[0-4095] */
    ts_u16 u16_noise_mod_low_delta   ; /* 噪声判断低阈值 U12.0, 取值范围[0-4095] */
    ts_u16 u16_noise_mod_hig         ; /* 噪声判断高阈值 U12.0, 取值范围[0-4095] */
    ts_u16 u16_noise_mod_hig_delta   ; /* 噪声判断高阈值 U12.0, 取值范围[0-4095] */   
    ts_u16 u16_noise_mod_low_coef    ; /* 调整系数 U2.10, 取值范围[0-4095] */
    ts_u16 u16_noise_mod_mid_coef    ; /* 调整系数 U2.10, 取值范围[0-4095] */
    ts_u16 u16_noise_mod_hig_coef    ; /* 调整系数 U2.10, 取值范围[0-4095] */

    ts_u16 u16_lum_mod_low           ; /* 亮度判断低阈值 U14.0, 取值范围[0-16383] */
    ts_u16 u16_lum_mod_low_delta     ; /* 亮度判断低阈值 U14.0, 取值范围[0-16383] */
    ts_u16 u16_lum_mod_hig           ; /* 亮度判断高阈值 U14.0, 取值范围[0-16383] */
    ts_u16 u16_lum_mod_hig_delta     ; /* 亮度判断高阈值 U14.0, 取值范围[0-16383] */
    ts_u16 u16_lum_mod_low_coef      ; /* 调整系数 U2.10, 取值范围[0-4095] */
    ts_u16 u16_lum_mod_mid_coef      ; /* 调整系数 U2.10, 取值范围[0-4095] */
    ts_u16 u16_lum_mod_hig_coef      ; /* 调整系数 U2.10, 取值范围[0-4095] */

    ts_u08 u08_noise_thr_low         ; /* 噪声判断低阈值 U8.0, 取值范围[0-255] */
    ts_u08 u08_noise_thr_low_delta   ; /* 噪声判断低阈值 U8.0, 取值范围[0-255] */
    ts_u08 u08_noise_thr_hig         ; /* 噪声判断高阈值 U8.0, 取值范围[0-255] */
    ts_u08 u08_noise_thr_hig_delta   ; /* 噪声判断高阈值 U8.0, 取值范围[0-255] */   
    ts_u08 u08_noise_str_low_coef    ; /* 调整系数 U2.6, 取值范围[0-255] */
    ts_u08 u08_noise_str_mid_coef    ; /* 调整系数 U2.6, 取值范围[0-255] */
    ts_u08 u08_noise_str_hig_coef    ; /* 调整系数 U2.6, 取值范围[0-255] */
    ts_u08 u08_noise_thr_low_coef    ; /* 调整系数 U2.6, 取值范围[0-255] */
    ts_u08 u08_noise_thr_mid_coef    ; /* 调整系数 U2.6, 取值范围[0-255] */
    ts_u08 u08_noise_thr_hig_coef    ; /* 调整系数 U2.6, 取值范围[0-255] */

    ts_u08 u08_lum_thr_low           ; /* 亮度判断低阈值 U8.0, 取值范围[0-255] */
    ts_u08 u08_lum_thr_low_delta     ; /* 亮度判断低阈值 U8.0, 取值范围[0-255] */
    ts_u08 u08_lum_thr_hig           ; /* 亮度判断高阈值 U8.0, 取值范围[0-255] */
    ts_u08 u08_lum_thr_hig_delta     ; /* 亮度判断高阈值 U8.0, 取值范围[0-255] */
    ts_u08 u08_lum_str_low_coef      ; /* 调整系数 U2.6, 取值范围[0-255] */
    ts_u08 u08_lum_str_mid_coef      ; /* 调整系数 U2.6, 取值范围[0-255] */
    ts_u08 u08_lum_str_hig_coef      ; /* 调整系数 U2.6, 取值范围[0-255] */
    ts_u08 u08_lum_thr_low_coef      ; /* 调整系数 U2.6, 取值范围[0-255] */
    ts_u08 u08_lum_thr_mid_coef      ; /* 调整系数 U2.6, 取值范围[0-255] */
    ts_u08 u08_lum_thr_hig_coef      ; /* 调整系数 U2.6, 取值范围[0-255] */

}ts_isp_ltm_v00_cfg_s_t;

typedef struct ts_isp_drc_cfg_s
{
    ts_u08 u08_en                   ;  /* 使能，U8.0, 取值范围[0-255]，BIT0:模块使能， BIT1:gamma使能 */
    ts_u08 u08_rev0[3]              ;  

    ts_u16  u16_detail_thr1         ; /* 细节判断低阈值 U8.0, 取值范围[0-255] */
    ts_u16  u16_detail_thr2         ; /* 细节判断高阈值 U8.0, 取值范围[0-255] */
    ts_u08  u08_detail_delta1       ; /* 细节判断过度 U3.0, 取值范围[0-7] */
    ts_u08  u08_detail_delta2       ; /* 细节判断过度 U3.0, 取值范围[0-7] */
    ts_u16  u16_rev_detail          ;

    ts_u08  u08_adj_coef0           ; /* 细节调整系数 U3.5, 取值范围[0-255] */
    ts_u08  u08_adj_coef1           ; /* 细节调整系数 U3.5, 取值范围[0-255] */
    ts_u08  u08_adj_coef2           ; /* 细节调整系数 U3.5, 取值范围[0-255] */
    ts_u08  u08_rev_adj             ; 

    ts_u16  u16_noise_thr_low        ; /* 噪声判断低阈值 U12.0, 取值范围[0-4095] */
    ts_u16  u16_noise_thr_low_delta  ; /* 噪声判断低阈值 U12.0, 取值范围[0-4095] */
    ts_u16  u16_noise_thr_hig        ; /* 噪声判断高阈值 U12.0, 取值范围[0-4095] */
    ts_u16  u16_noise_thr_hig_delta  ; /* 噪声判断高阈值 U12.0, 取值范围[0-4095] */

    ts_u08  u08_adj_coef0_low       ; /* 细节调整系数 U1.7, 取值范围[0-255] */
    ts_u08  u08_adj_coef0_mid       ; /* 细节调整系数 U1.7, 取值范围[0-255] */
    ts_u08  u08_adj_coef0_hig       ; /* 细节调整系数 U1.7, 取值范围[0-255] */ 
    ts_u08  u08_adj_coef1_low       ; /* 细节调整系数 U1.7, 取值范围[0-255] */
    ts_u08  u08_adj_coef1_mid       ; /* 细节调整系数 U1.7, 取值范围[0-255] */
    ts_u08  u08_adj_coef1_hig       ; /* 细节调整系数 U1.7, 取值范围[0-255] */ 
    ts_u08  u08_adj_coef2_low       ; /* 调整系数 U1.7, 取值范围[0-255] */
    ts_u08  u08_adj_coef2_mid       ; /* 调整系数 U1.7, 取值范围[0-255] */
    ts_u08  u08_adj_coef2_hig       ; /* 调整系数 U1.7, 取值范围[0-255] */
    ts_u08  u08_k_coef_low          ; /* 图像对比度调整系数 U1.7, 取值范围[0-255] */
    ts_u08  u08_k_coef_mid          ; /* 图像对比度调整系数 U1.7, 取值范围[0-255] */
    ts_u08  u08_k_coef_hig          ; /* 图像对比度调整系数 U1.7, 取值范围[0-255] */
 
    ts_u08 u08_work_mode            ;  /* 亮度工作模式，0：手动, U2.0, 取值范围[0-3] */
    ts_u08 u08_en_hig_only          ;  /* 高亮模式，取值范围[0-1] */
    ts_u08 u08_en_low_protect       ;  /* 低暗保护，取值范围[0-1] */
    ts_u08 u08_range_min_o          ;  /* 输出亮度范围 U8.0, 取值范围[0-255] */
    ts_u08 u08_range_max_o          ;  /* 输出亮度范围 U8.0, 取值范围[0-255] */
    ts_u08 u08_adj_speed            ;  /* 更新速度 U8.0, 取值范围[0-255] */
    ts_u08 u08_max_modify_1         ; /* 高亮阶段调整修正系数 U0.8, 取值范围[0-255] */
    ts_u08 u08_max_modify_2         ; /* 高亮阶段调整修正系数 U0.8, 取值范围[0-255] */
    
    ts_u16  u16_luma_adj            ; /* 亮度调整 u10.0, 取值范围[0-1023] */
    ts_u16  u16_coef_adj            ; /* 对比度调整 U4.10, 取值范围[0-16383] */
    ts_u16  u16_luma_target         ; /* 目标亮度调整 U0.10, 取值范围[0-1023]*/
    ts_u16  u16_ahl_delta           ; /* 过曝抑制强度 U16.0, 取值范围[0-65535]*/

    ts_u16  u16_luma_limit_l        ; /* 目标亮度调整限制 u.11, 取值范围[0-1024] */
    ts_u16  u16_luma_limit_h        ; /* 目标亮度调整限制 u.11, 取值范围[0-1024] */
    ts_u16  u16_ctr_modify_low      ; /* 自动对比度调节最小限制， U4.10*/
    ts_u16  u16_ctr_modify_hig      ; /* 自动对比度调节最大限制， U4.10*/

    ts_u08  u08_bld_thr_l            ; /* 亮度混合低阈值，取值范围[0-255] */
    ts_u08  u08_bld_thr_l_delta      ; /* 亮度混合过度，取值范围[0-255] */
    ts_u08  u08_bld_thr_h            ; /* 亮度混合高阈值，取值范围[0-255] */
    ts_u08  u08_bld_thr_h_delta      ; /* 亮度混合过度，取值范围[0-255] */
    ts_u08  u08_bld_coef_low         ; /* 亮度混合系数，U1.7, 取值范围[0-255] */
    ts_u08  u08_bld_coef_mid         ; /* 亮度混合系数，U1.7, 取值范围[0-255] */
    ts_u08  u08_bld_coef_hig         ; /* 亮度混合系数，U1.7, 取值范围[0-255] */
    ts_s08  s08_scale_coef           ; /* 缩放系数，s7.0, 取值范围[-127-127] */

    ts_u08  u08_fus_thr_l            ; /* 亮度混合低阈值，取值范围[0-255] */
    ts_u08  u08_fus_thr_l_delta      ; /* 亮度混合过度，取值范围[0-255] */
    ts_u08  u08_fus_thr_h            ; /* 亮度混合高阈值，取值范围[0-255] */
    ts_u08  u08_fus_thr_h_delta      ; /* 亮度混合过度，取值范围[0-255] */
    ts_u08  u08_fus_coef_low         ; /* 亮度混合系数，U1.7, 取值范围[0-255] */
    ts_u08  u08_fus_coef_mid         ; /* 亮度混合系数，U1.7, 取值范围[0-255] */
    ts_u08  u08_fus_coef_hig         ; /* 亮度混合系数，U1.7, 取值范围[0-255] */
    ts_u08  u08_fus_rev[1]           ; 

    ts_u08 au08_gamma[256]            ;  
}ts_isp_drc_cfg_s_t;

typedef struct ts_isp_pipe_cfg_s
{
    ts_u32 u32_mod_bypass;
    ts_u08 u08_ltm_pos;
    ts_u08 u08_fpnc_pos;
    ts_u08 u08_agc_sel;
    ts_u08 u08_ltm_sel;

    ts_u08 au08_rev[12];
}ts_isp_pipe_cfg_s_t;

#define ISP_USR_ADJ_EN (0x76543210)
typedef struct ts_isp_usr_cfg_s
{
    ts_u32 u32_usr_adj_en;      /*用户风格调节使能, 使能为ISP_USR_ADJ_EN时启用*/
    ts_u08 u08_luma_coef;       /*用户亮度调节系数， U2.6*/
    ts_u08 u08_ctr_coef;        /*用户对比度调节系数， U2.6*/
    ts_u08 u08_nr_coef;         /*用户降噪调节系数， U2.6*/
    ts_u08 u08_enh_coef;        /*用户增强调节系数， U2.6*/
    ts_u08 u08_agc_coef;        /*调光调节系数， U2.6*/

    ts_u08 au08_rev[11];
}ts_isp_usr_cfg_s_t;

typedef struct ts_isp_fix_snc_cfg_s
{
    ts_u08 u08_snc_h_en;       /*横纹去除使能，u8.0*/
    ts_u08 u08_snc_v_en;       /*竖纹纹去除使能，u8.0*/
    ts_u08 u08_sta_frm_num;    /*统计帧数，u8.0*/
    ts_u08 u08_smooth;         /*更新平滑，u8.0*/

    ts_u08 u08_mode;           /*模式，u8.0， bit0：0： 单次触发模式， 1：连续模式*/
    ts_u08 u08_upd_frm;        /*连续模式下每次更新帧数，u8.0*/
    ts_u08 au08_rev_0[2];     

    ts_u08 u08_slice_num_h;    /*横向分片数量，越大资源消耗越大，检测越精确，需要被图像宽整除，u7.0*/
    ts_u08 u08_slice_num_v;    /*纵向分片数量，越大资源消耗越大，检测越精确，需要被图像宽整除，u7.0*/
    ts_u08 u08_flt_h_size;     /*横向滤波窗大小，u8.0*/
    ts_u08 u08_flt_v_size;     /*纵向滤波窗大小，u8.0*/
    
    ts_u08 u08_flt_step_h;     /*横向滤波间隔，u8.0*/
    ts_u08 u08_flt_step_v;     /*纵向滤波间隔，u8.0*/
    ts_u08 u08_flt_clip_h;     /*滤波限幅，u7.0*/
    ts_u08 u08_flt_clip_v;     /*滤波限幅，u7.0*/ 

    ts_u16 u16_flt_h_thr;      /*横向滤波阈值，u16.0*/
    ts_u16 u16_flt_v_thr;      /*纵向滤波阈值，u16.0*/

    ts_s32 s32_rev;
}ts_isp_fix_snc_cfg_s_t;

typedef struct ts_isp_cfg_reg_s         
{
    /*pipe*/
	union
	{
		ts_isp_pipe_cfg_s_t         st_pipe;				
		ts_u32   				    au32_rev_pipe[8];	
	};

	/*statis*/
	union
	{
		ts_isp_statis_cfg_s_t       st_statis;				
		ts_u32   				    au32_rev_statis[8];		
	};	

	/*agc stat*/
	union
	{
		ts_isp_agc_stat_cfg_s_t     st_agc_stat;			
		ts_u32   				    au32_rev_agc_stat[8];	
	};

	/*sence*/
	union
	{
		ts_isp_sence_cfg_s_t        st_sence;				
		ts_u32   				    au32_rev_sence[16];		
	};	

	/*agc*/
	union
	{
		ts_isp_agc_v00_cfg_s_t      st_agc;				
		ts_isp_agc_v01_cfg_s_t      st_agc_v01;				
        ts_isp_agc_v02_cfg_s_t      st_agc_v02;				
		ts_u32   				    au32_rev_agc[64];		
	};	

	/*ltm*/
	union
	{
		ts_isp_ltm_v00_cfg_s_t       st_ltm; 			
		ts_u32 				         au32_rev_ltm[32];	
	};	

	/*nr2d_Raw*/
	union
	{
		ts_isp_nr2d_raw_cfg_s_t      st_nr2d_raw; 			
		ts_u32 				         au32_rev_nr2d_raw[32];	
	};	

	/*nr2d_y*/
	union
	{
		ts_isp_nr2d_yuv_cfg_s_t      st_nr2d_yuv; 			
		ts_u32 				         au32_rev_nr2d_yuv[16];	
	};	

	/*nr3d*/
	union
	{
		ts_isp_nr3d_cfg_s_t          st_nr3d; 				
		ts_u32 				         au32_rev_nr3d[32];		
	};	

	/*snc*/
	union
	{
		ts_isp_snc_cfg_s_t           st_snc; 			
		ts_u32 				         au32_rev_snc[8];	
	};	

	/*lce*/
	union
	{
		ts_isp_lce_cfg_s_t           st_lce; 			
		ts_u32 				         au32_rev_lce[32];	
	};	

	/*dde*/
	union
	{
		ts_isp_dde_cfg_s_t           st_dde; 			
		ts_u32 				         au32_rev_dde[48];	
	};	

	/*ee*/
	union
	{
		ts_isp_ee_cfg_s_t            st_ee; 			
		ts_u32 				         au32_rev_ee[32];	
	};	

	/*fpnc*/
	union
	{
		ts_isp_fpnc_cfg_s_t          st_fpnc; 			
		ts_u32 				         au32_rev_fpnc[16];	
	};	

	/*bpc*/
	union
	{
		ts_isp_bpc_cfg_s_t           st_bpc; 			
		ts_u32 				         au32_rev_bpc[16];	
	};	

	/*drc*/
	union
	{
		ts_isp_drc_cfg_s_t           st_drc; 			
		ts_u32 				         au32_rev_drc[128];	
	};	

	/*usr*/
	union
	{
		ts_isp_usr_cfg_s_t           st_usr; 			
		ts_u32 				         au32_rev_usr[8];	
	};	

	/*fix_snc*/
	union
	{
		ts_isp_fix_snc_cfg_s_t          st_fix_snc; 			
		ts_u32 				            au32_rev_fix_snc[16];	
	};	

}ts_isp_cfg_reg_s_t; 


typedef struct ts_isp_ctrl_reg_s         
{
	ts_u32                  u32_mod_sync;
	ts_u32                  u32_mod_bypass;
	ts_u32                  u32_mod_pause;
	ts_u32                  u32_mod_rst;
	ts_u32                  u32_mod_rst_val;
	ts_u32                  u32_mod_adj_val;
	ts_u32                  u32_mod_clr_val;
	ts_u32                  u32_rev[1];
}ts_isp_ctrl_reg_s_t; 


typedef struct ts_isp_status_reg_s         
{
	ts_u32 				 au32_rev[256];	
}ts_isp_status_reg_s_t; 

typedef struct ts_isp_reg_s         
{
	ts_isp_ctrl_reg_s_t     *pst_ctrl;
	ts_isp_cfg_reg_s_t	    *pst_cfg;
	ts_isp_status_reg_s_t   *pst_status;
}isp_reg_s_t; 


typedef struct ts_isp_reg_info_s         
{
    isp_reg_s_t         st_reg;
    ts_ulong            pa_reg;
    ts_void             *pv_reg;           
    ts_void             *pv_pri;           
}ts_isp_reg_info_s_t; 

typedef struct ts_isp_reg_list_s         
{
	ts_isp_ctrl_reg_s_t         st_ctrl;            //0x0000
	union                                           //0x0020    
    {
	    ts_isp_cfg_reg_s_t	    st_cfg;
        ts_u32                  au32_cfg_rev[ISP_CFG_REG_NUM];
    };

	ts_isp_status_reg_s_t       st_status;          //0x0C00  
}ts_isp_reg_list_s_t; 

/* ========================================================================== */
/*                            函数定义区                                       */
/* ========================================================================== */



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __FW_ISP_H__ */



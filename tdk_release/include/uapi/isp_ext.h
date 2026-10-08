/******************************************************************************
  Copyright (C), 2022-2023, TopSilicon CO.,LTD.
 ******************************************************************************
  File Name     : isp_ext.h
  Version       : 1020 for shihui
  Author        : 
  Created       : 2023.2.18
  Last Modified :
  Description   : The common data type defination
  Function List :
  DHstory       :
  1.Date        : 2023/2/18
    Modification: Create
******************************************************************************/
#ifndef __ISP_EXT_H__
#define __ISP_EXT_H__


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
#include "ts_comm.h"
#else
#include <base/ts_type.h>
#include <base/ts_comm.h>

#endif

/* ========================================================================== */
/*                            类型定义区                                       */
/* ========================================================================== */
typedef struct ts_isp_statis_s         
{
    ts_s32     s32_blk_num_w;       /*横向分块数目*/
    ts_s32     s32_blk_num_h;       /*纵向分块数目*/
    ts_s32     s32_blk_size_w;      /*横向分块大小*/
    ts_s32     s32_blk_size_h;      /*纵向分块大小*/
    ts_u16     *pu16_aver;
    ts_u16     *pu16_detail;   
    ts_u16     *pu16_lum_vari;
    ts_void    *pv_private;            
}ts_isp_statis_s_t;

typedef struct ts_isp_hist_s         
{
    ts_void    *pv_hist;               /*hist存放地址*/
    ts_s32     s32_hist_len;           /*hist存放长度*/
    ts_s32     s32_hist_valid_len;     /*hist有效长度*/
    ts_s32     s32_acc_bit;            /*统计精度*/
    ts_s32     s32_blk_w;              /*横向分块数量*/
    ts_s32     s32_blk_h;              /*纵向分块数量*/
    ts_void    *pv_private;             
}ts_isp_hist_s_t;

typedef struct ts_isp_analyse_data_s         
{
	ts_bool                 b_valid_agc;
	ts_bool                 b_valid_statis;	
	ts_bool                 b_valid_ltm;
	ts_bool                 b_valid_drc;

	ts_isp_hist_s_t         st_hist_agc;
	ts_isp_statis_s_t       st_statis;
	ts_isp_hist_s_t         st_ltm;
	ts_isp_hist_s_t         st_drc;

}ts_isp_analyse_data_s_t; 

#define FW_ISP_AGC_LUT_LEN (1025)
typedef struct ts_isp_agc_lut_cfg_s         
{
	ts_u16 au16_lut[FW_ISP_AGC_LUT_LEN];		/*AGC映射表*/
}ts_isp_agc_lut_cfg_s_t; 

#define FW_LTM_V100_BLK_NUM		       (3)
#define FW_LTM_V100_BLK_TOTAL          (FW_LTM_V100_BLK_NUM * FW_LTM_V100_BLK_NUM)
#define FW_LTM_V100_BIT_NUM            (8)
#define FW_LTM_V100_MAX_BIN            ((1 << FW_LTM_V100_BIT_NUM) + 1)
#define FW_LTM_V100_HIST_LEN           (((FW_LTM_V100_MAX_BIN + 1) / 2) * 2)
typedef struct ts_isp_ltm_lut_cfg_s         
{
	ts_u16 au16_lut[FW_LTM_V100_BLK_TOTAL][FW_LTM_V100_HIST_LEN];		/*LTM映射表*/
}ts_isp_ltm_lut_cfg_s_t; 

#define FW_ISP_DRC_LUT_LEN (1025)
typedef struct ts_isp_drc_lut_cfg_s         
{
	ts_u08 au08_lut[FW_ISP_DRC_LUT_LEN];		/*DRC映射表*/
    ts_u08 au08_rev[7];
}ts_isp_drc_lut_cfg_s_t; 

typedef struct ts_isp_usr_func_s         
{
    ts_void *pv_private;

    ts_bool b_func_statis_en;
    ts_bool b_func_agc_en;
    ts_bool b_func_ltm_en;
    ts_bool b_func_drc_en;

    ts_s32 (*pfn_fw_isp_usr_statis_handle)(ts_void *pv_private, ts_isp_statis_s_t *pst_statis);   //分块统计值， FULLMAKS 支持
    ts_s32 (*pfn_fw_isp_usr_agc_handle)(ts_void *pv_private, ts_isp_hist_s_t *pst_hist_agc, ts_isp_agc_lut_cfg_s_t *pst_agc_lut);//agc操作
    ts_s32 (*pfn_fw_isp_usr_ltm_handle)(ts_void *pv_private, ts_isp_hist_s_t *pst_hist_ltm, ts_isp_ltm_lut_cfg_s_t *pst_ltm_lut);//ltm操作
    ts_s32 (*pfn_fw_isp_usr_drc_handle)(ts_void *pv_private, ts_isp_hist_s_t *pst_hist_drc, ts_isp_drc_lut_cfg_s_t *pst_drc_lut);//RAW14->Y8 //FULLMASK支持

}ts_isp_usr_func_s_t;


/* 设备等状态定义 */
typedef enum ts_isp_fw_image_type_en {
    TS_FW_ISP_IMG_U16C1 = 0,
    TS_FW_ISP_IMG_U08C1 = 1,

    TS_FW_ISP_IMG_TYPE_BUTT
} ts_isp_fw_image_type_en_t;

typedef struct ts_isp_fw_image_s         
{
    ts_void    *pv_image_virt;
    ts_u16     u16_image_size_w;
    ts_u16     u16_image_size_h;
    ts_u16     u16_image_type;
    ts_u16     u16_frm_num;
    ts_void    *pv_private;             
}ts_isp_fw_image_s_t;

typedef struct ts_isp_fix_snc_lut_s         
{
    ts_bool     b_enable;
    ts_u16      u16_lut_len_h;
    ts_u16      u16_lut_len_v;
    ts_s08      as08_cor_h[1280];
    ts_s08      as08_cor_v[1024];
}ts_isp_fix_snc_lut_s_t; 

typedef struct fw_isp_ext_func_s         
{
    ts_s32 (*pfn_fw_isp_get_raw)(ts_s32 s32_pos, ts_isp_fw_image_s_t *pst_fw_image, ts_u32 u32_timeout_ms);
    ts_void *apv_rev[7];
}fw_isp_ext_func_s_t;

/* ========================================================================== */
/*                            函数定义区                                       */
/* ========================================================================== */
ts_s32 ts_tdk_isp_fw_set_usr_func(ts_handle_t ph_handle, ts_isp_usr_func_s_t *pst_usr_func);
ts_s32 ts_tdk_isp_fw_get_usr_func(ts_handle_t ph_handle, ts_isp_usr_func_s_t *pst_usr_func);
ts_s32 ts_tdk_isp_fw_set_ext_func(ts_handle_t ph_handle, fw_isp_ext_func_s_t *pst_ext_func);
ts_s32 ts_tdk_isp_fw_get_ext_func(ts_handle_t ph_handle, fw_isp_ext_func_s_t *pst_ext_func);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __ISP_EXT_H__ */



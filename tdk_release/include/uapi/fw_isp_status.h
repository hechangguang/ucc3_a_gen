#ifndef __FW_ISP_STATUS_H__
#define __FW_ISP_STATUS_H__


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
typedef struct fw_isp_agc_status_s         
{
 	ts_s32 s32_noise;                   /*噪声判断值*/
 	ts_s32 s32_mean;                    /*均值*/
 	ts_s32 s32_valid_num;               /*有效灰度数量*/
 	ts_s32 s32_clip_base;               /*限制值*/
    ts_u16 u16_src_l;                   /*原始有效灰度起始*/
    ts_u16 u16_src_r;                   /*原始有效灰度结束*/
    ts_u16 u16_dst_l;                   /*目标有效灰度起始*/
    ts_u16 u16_dst_r;                   /*目标有效灰度结束*/
    ts_u16 u16_diff_range;              /*原始数据范围*/
    ts_u16 u16_src_mean;                /*原始数据均值*/
    ts_u16 u16_src_mid;                 /*原始数据中值*/
    ts_u16 u16_rev[5]; 
}fw_isp_agc_status_s_t; 

typedef struct fw_isp_sence_blk_s         
{
    ts_s32 s32_min;
    ts_s32 s32_max;
    ts_s32 s32_aver;
    ts_s32 s32_var_max;
    ts_s32 s32_var_min;
    ts_s32 s32_ctr_max;
    ts_s32 s32_ctr_min;
    ts_s32 s32_aver_ctr;
    ts_s32 s32_aver_var;
}fw_isp_sence_blk_s_t; 

typedef struct fw_isp_sence_gbl_s         
{
    fw_isp_sence_blk_s_t ast_blk_info[3][3];
    ts_s32               s32_min_val;
    ts_s32               s32_max_val;
    ts_s32               s32_mean_val; 
    ts_s32               s32_mean_noise; 
    ts_s32               s32_unisf; //均匀面
    ts_s32               s32_mean_var; 
}fw_isp_sence_gbl_s_t; 

typedef struct fw_isp_sence_info_s         
{
    fw_isp_sence_gbl_s_t  st_sen_gbl;
}fw_isp_sence_status_s_t;


typedef struct tag_fw_isp_ltm_status_s         
{
 	ts_s32 s32_noise;
}fw_isp_ltm_status_s_t; 

typedef struct fw_isp_nr2d_raw_status_s         
{
	ts_u32 au32_rev[4];
}fw_isp_nr2d_raw_status_s_t; 

typedef struct fw_isp_nr2d_yuv_status_s         
{
	ts_u32 au32_rev[6];
}fw_isp_nr2d_yuv_status_s_t; 

typedef struct fw_isp_nr3d_status_s         
{
	ts_u32 au32_rev[4];
}fw_isp_nr3d_status_s_t; 

typedef struct fw_isp_snc_status_s         
{
	ts_u32 au32_rev[4];
}fw_isp_snc_status_s_t; 

typedef struct fw_isp_lce_status_s         
{
	ts_u32 au32_rev[4];
}fw_isp_lce_status_s_t; 

typedef struct fw_isp_dde_status_s         
{
	ts_u32 au32_rev[4];
}fw_isp_dde_status_s_t; 

typedef struct fw_isp_ee_status_s         
{
	ts_u32 au32_rev[4];
}fw_isp_ee_status_s_t; 

typedef struct fw_isp_fpnc_status_s         
{
	ts_u32 au32_rev[4];
}fw_isp_fpnc_status_s_t; 

typedef struct fw_isp_bpc_status_s         
{
	ts_u32 au32_rev[4];
}fw_isp_bpc_status_s_t; 

typedef struct fw_isp_drc_status_s         
{
	ts_u32 au32_rev[4];
}fw_isp_drc_status_s_t; 

typedef struct fw_isp_status_s         
{
    union 
    {
        fw_isp_agc_status_s_t         st_agc_status;
        ts_u32                        au32_rev_agc[32];
    };

    union 
    {
        fw_isp_sence_status_s_t       st_sence_status;
        ts_u32                        au32_rev_sence[128];
    };

    union 
    {
        fw_isp_ltm_status_s_t         st_ltm_status;
        ts_u32                        au32_rev_ltm[16];
    };

    union 
    {
        fw_isp_nr2d_raw_status_s_t    st_nr2d_raw_status;
        ts_u32                        au32_rev_nr2d_raw[4];
    };

    union 
    {
        fw_isp_nr2d_yuv_status_s_t    st_nr2d_yuv_status;
        ts_u32                        au32_rev_nr2d_yuv[4];
    };

    union 
    {
        fw_isp_nr3d_status_s_t      st_nr3d_status;
        ts_u32                      au32_rev_nr3d[4];
    };

    union 
    {
        fw_isp_snc_status_s_t       st_snc_status;
        ts_u32                      au32_rev_snc[4];
    };

    union 
    {
        fw_isp_lce_status_s_t       st_lce_status;
        ts_u32                      au32_rev_lce[4];
    };

    union 
    {
        fw_isp_dde_status_s_t       st_dde_status;
        ts_u32                      au32_rev_dde[4]; 
    };

    union 
    {
        fw_isp_ee_status_s_t        st_ee_status;
        ts_u32                      au32_rev_ee[4];  
    };

    union 
    {
        fw_isp_fpnc_status_s_t       st_fpnc_status;
        ts_ulong                     aul_rev_fpnc[4];  
    };

    union 
    {
        fw_isp_bpc_status_s_t       st_bpc_status;
        ts_u32                      au32_rev_bpc[4];  
    };

    union 
    {
        fw_isp_drc_status_s_t       st_drc_status;
        ts_u32                      au32_rev_drc[4];  
    };

}fw_isp_status_s_t; 



/* ========================================================================== */
/*                            函数定义区                                       */
/* ========================================================================== */



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __FW_ISP_H__ */



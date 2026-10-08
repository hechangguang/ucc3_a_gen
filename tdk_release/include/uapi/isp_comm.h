/******************************************************************************
  Copyright (C), 2022-2023, TopSilicon CO.,LTD.
 ******************************************************************************
  File Name     : isp_common.h
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
#ifndef __ISP_COMM_H__
#define __ISP_COMM_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

/* ========================================================================== */
/*                             头文件区                                        */
/* ========================================================================== */
#include <base/ts_type.h>
#include <stdio.h>
#include <stdlib.h> 

/* ========================================================================== */
/*                            类型定义区                                       */
/* ========================================================================== */
typedef enum ts_isp_pipe_pos_en {
    /* LTM 位置， 支持 0, 1, 2*/
    TS_ISP_PIPE_LTM_SEL = 0,

    /* FPNC 位置， 支持 0, 1*/
    TS_ISP_PIPE_FPNC_SEL = 1,

    /* AFS 位置， 支持 0, 1*/
    TS_ISP_PIPE_AFS_SEL = 2,

    /* FIRE 位置， 支持 0, 1*/
    TS_ISP_PIPE_FIRE_SEL = 3,

    /* GDC 位置， 支持 0, 1*/
    TS_ISP_PIPE_GDC_SEL = 4,

    TS_ISP_PIPE_SEL_BUTT
} ts_isp_pipe_pos_en_t;

typedef struct ts_isp_pipe_attr_s {
    ts_isp_pipe_pos_en_t    en_pipe_module_sel;
    ts_u08                  u08_mod_pos;
} ts_isp_pipe_attr_s_t;

/* 基础参数 */
typedef struct ts_isp_comm_attr_s {
    ts_bool                 b_module_en;
    ts_u32                  u32_start_offset;
} ts_isp_comm_attr_s_t;


/* nuc 模块属性参数 */

/* 工作模式 */
typedef enum ts_isp_nuc_mode_en {
    /* 透传，o = i + FRM OFT, FRM OFT: 数据格式 S16*/
    TS_ISP_NUC_MODE_PASSTHROUGH = 0,

    /* o = i + offset + FRM OFT，offset 数据格式S15.0， FRM OFT: 数据格式 S15.0*/
    TS_ISP_NUC_MODE_OFFSET = 1,

    /* o = i * g  + FRM OFT， g: 数据格式 U1.14， FRM OFT: 数据格式 S15.0*/
    TS_ISP_NUC_MODE_GAIN = 2,

    /* o = i * g + offset + FRM OFT， g: 数据格式 U1.14， FRM OFT: 数据格式 S15.0，offset 数据格式S15.0*/
    TS_ISP_NUC_MODE_GAIN_OFFSET = 3,

    /* o = i * g + offset - g * coef * deltab + FRM OFT， g: 数据格式 U1.14， FRM OFT: 数据格式 S15.0，offset 数据格式S15.0， coef 数据格式S2.13， deltab为S15.0 */
    TS_ISP_NUC_MODE_DELTAB = 4,

    TS_ISP_NUC_MODE_BUTT
} ts_isp_nuc_mode_en_t;

/* statis frame numer */
typedef enum ts_isp_nuc_stat_num_en {
    TS_ISP_NUC_STAT_NUM_4 = 4,
    TS_ISP_NUC_STAT_NUM_8 = 8,
    TS_ISP_NUC_STAT_NUM_16 = 16,

    TS_ISP_NUC_STAT_NUM_BUTT
} ts_isp_nuc_stat_num_en_t;


/* statis mode */
typedef enum ts_isp_nuc_stat_mode_en {
    /* mean only */
    TS_ISP_NUC_STAT_MEAN = 0,

    /* mean + offset */
    TS_ISP_NUC_STAT_MEAN_OFFET = 1,

    /* mean + offset + deltab */
    TS_ISP_NUC_STAT_MEAN_OFFET_DELTA = 2,

    /* mean + offset + deltab + upd */
    TS_ISP_NUC_STAT_ALL = 3,

    TS_ISP_NUC_STAT_BUTT
} ts_isp_nuc_stat_mode_en_t;


typedef struct ts_isp_nuc_attr_s {
    /* TS_DISABLE : bypass;  TS_ENABLE : work */
    ts_s32 s32_en;

    /* TS_DISABLE : nuc 执行过程中，会后传数据； TS_ENABLE : 不后传数据 */
    ts_s32 s32_cutoff;

    /* 工作模式 */
    ts_isp_nuc_mode_en_t en_mode;

    /* 统计模式 */
    ts_isp_nuc_stat_mode_en_t en_stat_mode;

    /* 每次工作帧率, 取值范围 [1:255]*/
    ts_u08 u08_stat_frm_num;
    ts_u08 u08_stat_fack_frm2_en;

    /* 0 : auto calc; > 0 : use this config, 取值范围[1..0x3ffe] */
    ts_u16 u16_mean_val;

    /* 如果 en_mode = TS_ISP_NUC_MODE_DELTAB, u16_coef 需要设置，s2.13 取值范围 0..0xFFFF */
    ts_u16 u16_coef;
    ts_u16 u16_frm_oft;

    /* bbuf 下是否需要双缓存;仅在第一次配置时生效 */
    ts_u08 u08_bbuf_dbl;

    /* TS_ISP_NUC_STAT_ALL == en_stat_mode || TS_ISP_NUC_MODE_DELTAB == en_mode 下是否需要双缓存；仅在第一次配置时生效 */
    ts_u08 u08_dbuf_dbl;

    ts_u16 u16_rev;
} ts_isp_nuc_attr_s_t;

typedef struct ts_isp_offline_comm_s {
    ts_u16   u16_buf_num;       /* 模块数量*/
    ts_u16   u16_rev;

    ts_u32   au32_rev[8];

} ts_isp_offline_comm_s_t;

typedef struct ts_isp_afs_luma_adj_s {
    ts_u16 u16_lum_thr1;        /*亮度阈值， U14.0*/
    ts_u16 u16_lum_thr2;        /*亮度阈值， U14.0*/
    ts_u08 u08_lum_delta1;      /*亮度阈值过渡， U11.0*/
    ts_u08 u08_lum_delta2;      /*亮度阈值过渡， U11.0*/
    ts_u08 u08_lum_coef1;       /*根据亮度权重修正系数，U1.7*/
    ts_u08 u08_lum_coef2;       /*根据亮度权重修正系数，U1.7*/
    ts_u08 u08_lum_coef3;       /*根据亮度权重修正系数，U1.7*/
    ts_u08 u08_lum_rev;     
}ts_isp_afs_luma_adj_s_t;

typedef struct ts_isp_afs_hf_adj_s {
    ts_u32 u32_hf_thr1;        /*高频阈值U20.0*/
    ts_u32 u32_hf_thr2;        /*高频阈值U20.0*/
    ts_u08 u08_hf_delta1;      /*高频阈值过渡U11.0*/
    ts_u08 u08_hf_delta2;      /*高频阈值过渡U11.0*/
    ts_u08 u08_hf_coef1;       /*根据高频权重修正系数U2.6*/
    ts_u08 u08_hf_coef2;       /*根据高频权重修正系数U2.6*/
    ts_u08 u08_hf_coef3;       /*根据高频权重修正系数U2.6*/
    ts_u08 u08_hf_rev;     
    ts_u32 u32_hf_clip;       /*高频限幅U20.0*/
}ts_isp_afs_hf_adj_s_t;

typedef struct ts_isp_afs_attr_s {
    ts_u08 u08_af_en;                           /*统计使能，0 关闭， 1，开启*/
    ts_u08 u08_statis_mode;                     /*统计模式，默认0， u2.0*/
    ts_u08 u08_hf_shift_h;                      /*归一化bit，u5.0*/
    ts_u08 u08_hf_shift_v;                      /*归一化bit，u5.0*/
    ts_u08 au08_rev_0[4];    
    
    ts_u16 u16_pre_flt_str;                     /*前置滤波强度[0:65535]， 值越大滤波越强*/
    ts_u16 au16_luma_hist_thr[3];               /*直方图统计阈值*/
    
    ts_isp_afs_luma_adj_s_t st_luma_adj_h;      /*横向加权系数根据亮度进行调节*/
    ts_isp_afs_luma_adj_s_t st_luma_adj_v;      /*纵向加权系数根据亮度进行调节*/
    ts_isp_afs_hf_adj_s_t   st_hf_adj_h;        /*横向加权系数根据细节进行调节*/
    ts_isp_afs_hf_adj_s_t   st_hf_adj_v;        /*纵向加权系数根据细节进行调节*/

} ts_isp_afs_attr_s_t;

#define AFS_BLK_NUM_W (32)
#define AFS_BLK_NUM_H (32)
typedef struct ts_isp_afs_blk_info_s {
    ts_s32 s32_h1;                      /*横向滤波器1结果*/
    ts_s32 s32_h2;                      /*横向滤波器2结果*/
    ts_s32 s32_v1;                      /*纵向滤波器1结果*/
    ts_s32 s32_v2;                      /*纵向滤波器2结果*/
    ts_u16 u16_aver;                    /*块内均值*/
    ts_u16 au16_hist[3];                /*块内直方图*/
} ts_isp_afs_blk_info_s_t;


typedef struct ts_isp_af_statis_s {
    ts_isp_afs_blk_info_s_t *pst_af_statis_info;    /*分块统计信息*/
    ts_u16 u16_blk_num_w;                           /*横向分块数量*/
    ts_u16 u16_blk_num_h;                           /*纵向分块数量*/
    ts_void *pv_private;
} ts_isp_af_statis_s_t;


typedef struct ts_isp_fire_rect_s {
    ts_bool  b_enable;                       /*使能，0 关闭， 1，开启*/       
    ts_u16  u16_start_x;                     /*横向起始位置*/
    ts_u16  u16_start_y;                     /*纵向起始位置*/
    ts_u16  u16_end_x;                       /*横向结束位置*/
    ts_u16  u16_end_y;                       /*纵向结束位置*/
} ts_isp_fire_rect_s_t;

typedef struct ts_isp_fire_pos_s {
    ts_u16  u16_pos_x;                  /*横向坐标*/
    ts_u16  u16_pos_y;                  /*纵向坐标*/
} ts_isp_fire_pos_s_t;

#define ISP_FIRE_RECT_NUM   (3)
#define ISP_FIRE_POS_NUM    (3)
typedef struct ts_isp_fire_attr_s {
    ts_u08                      u08_enable;                         /*使能，0 关闭， 1，开启*/                      
    ts_u08                      u08_blk_size_w;                     /*横向块大小*/    
    ts_u08                      u08_blk_size_h;                     /*纵向块大小*/   
    ts_u08                      u08_rev;
    ts_isp_fire_rect_s_t        ast_rect[ISP_FIRE_RECT_NUM];        /*手动区域*/   
    ts_isp_fire_pos_s_t         ast_pos[ISP_FIRE_POS_NUM];          /*手动位置*/   
} ts_isp_fire_attr_s_t;

typedef struct ts_isp_fire_blk_info_s {
    ts_u16 u16_min_pos_y;           /*最小值位置y坐标*/
    ts_u16 u16_min_pos_x;           /*最小值位置x坐标*/
    ts_u16 u16_min_val;             /*最小值*/
    ts_u16 u16_max_pos_y;           /*最大值位置y坐标*/
    ts_u16 u16_max_pos_x;           /*最大值位置x坐标*/
    ts_u16 u16_max_val;             /*最大值*/ 
} ts_isp_fire_blk_info_s_t;

typedef struct ts_isp_fire_auto_info_s {
    ts_u16                   u16_blk_num_x;             /*横向分块数量*/
    ts_u16                   u16_blk_num_y;             /*纵向分块数量*/
    ts_u16                   u16_blk_size_x;            /*横向块大小*/
    ts_u16                   u16_blk_size_y;            /*纵块大小*/
    ts_isp_fire_blk_info_s_t *pst_blk_info;             /*统计值*/
} ts_isp_fire_auto_info_s_t;

typedef struct ts_isp_fire_frm_info_s {
    ts_u16 u16_min_pos_y;                     /*最小点纵向坐标*/
    ts_u16 u16_min_pos_x;                     /*最小点横向坐标*/
    ts_u16 u16_min_val;                       /*最小值*/
    ts_u16 u16_max_pos_y;                     /*最大点纵向坐标*/
    ts_u16 u16_max_pos_x;                     /*最大点横向坐标*/
    ts_u16 u16_max_val;                       /*最大值*/  
    ts_u16 u16_mean;                          /*均值*/  
    ts_u16 au16_rev[1];
} ts_isp_fire_frm_info_s_t;

typedef struct ts_isp_fire_man_info_s {
    ts_u16                   u16_mean;              /*均值*/
    ts_u16                   u16_rev;
    ts_isp_fire_blk_info_s_t st_blk_info;           /*块内信息*/
} ts_isp_fire_man_info_s_t;

typedef struct ts_isp_fire_statis_s {
    ts_isp_fire_auto_info_s_t  st_auto;                             /*自动分块统计值*/
    ts_isp_fire_frm_info_s_t   st_frm_info;                         /*整帧信息*/
    ts_isp_fire_man_info_s_t   ast_man_info[ISP_FIRE_RECT_NUM];     /*手动指定区域内信息*/
    ts_u16                     au16_pos_value[ISP_FIRE_POS_NUM];    /*手动指定点信息*/
    ts_u16                     u16_rev;
    ts_void                    *pv_private;
} ts_isp_fire_statis_s_t;

typedef struct ts_isp_hot_attr_s {
    ts_u08                      u08_enable;           /*使能，0 关闭， 1，开启*/       
    ts_u08                      u08_samp_step;        /*采样间隔*/        
    ts_u16                      u16_hot_thr;          /*高热点阈值*/ 
    ts_u16                      u16_edge_thr;         /*边缘阈值*/ 
    ts_u16                      u16_rev;
} ts_isp_hot_attr_s_t;

typedef struct ts_isp_hot_pos_s {
    ts_u16  u16_pos_x;                              /*边缘X坐标*/ 
    ts_u16  u16_pos_y;                              /*边缘Y坐标*/ 
} ts_isp_hot_pos_s_t;

typedef struct ts_isp_hot_statis_s {
    ts_u32                      u32_hot_cnt;
    ts_u32                      u32_edge_sum_w;
    ts_u32                      u32_edge_sum_h;
    ts_u32                      u32_edge_cnt;
    ts_isp_hot_pos_s_t          *pst_edge_pos;     /*边缘点坐标*/ 
    ts_void                     *pv_private;
} ts_isp_hot_statis_s_t;

typedef struct ts_isp_sffc_attr_s {
    ts_u08      u08_enable;             /* 模块使能, u1.0, 范围[0:1] */
    ts_u08      u08_en_man;             /* 手动强度设置使能, u1.0,范围[0:1] */
    ts_u08      u08_upd_speed;          /*更新速度, u8.0, 范围[0:1] */
    ts_u08      u08_rev;
    ts_u16      u16_flt_thr;            /*自适应强度判断阈值， U16.0 ,范围[0:65535]*/ 
    ts_u16      u16_man_cor_v;          /*手动矫正强度, U3.13, 范围[0:65535]*/ 

    ts_u32      au32_rev[8];
} ts_isp_sffc_attr_s_t;

typedef struct ts_isp_sffc_blk_info_s {
    ts_u08      u08_blk_size;           /*块大小，0:63*/ 
    ts_u08      u08_blk_rect_mode;      /*0xA5 开启XY分别设置模式*/
    ts_u08      u08_blk_size_x;         /*块大小，0:63*/ 
    ts_u08      u08_blk_size_y;         /*块大小，0:63*/ 

    ts_u08      u08_rev[4];
} ts_isp_sffc_blk_info_s_t;

typedef struct ts_isp_sffc_statis_s {
    ts_s16      s16_ratio;           
    ts_u16      u08_rev[3];
} ts_isp_sffc_statis_s_t;

typedef struct ts_isp_sffc_cali_s {
    ts_u08                      u08_sample_cnt;     /*采样帧数， [4:31]*/
    ts_u08                      au08_rev[3];
} ts_isp_sffc_cali_s_t;

typedef struct ts_isp_sffc_cali_res_s {
    ts_void     *pv_virt_addr;
    ts_u32      u32_phy_addr;
    ts_u32      u32_size;
    ts_u08      u08_blk_size_h;
    ts_u08      u08_blk_size_v;
    ts_u08      au08_rev[2];
} ts_isp_sffc_cali_res_s_t;


typedef struct ts_isp_sffc_buf_s {
    ts_bool       b_working;
    ts_buffer_s_t st_buf;
} ts_isp_sffc_buf_s_t;


typedef struct ts_isp_snc_statis_s {
    ts_u16                      u16_len_h;
    ts_u16                      u16_len_v;  
    ts_u16                      *pu16_statis_h;
    ts_u16                      *pu16_statis_v;  
    ts_void                     *pv_private;
} ts_isp_snc_statis_s_t;

typedef struct ts_isp_snc_fix_cor_s {
    ts_u08 u08_enable;
    ts_u08 au08_rev[3];
    ts_s08 *ps08_cor_v;
    ts_s08 *ps08_cor_h;
    ts_u16 u16_cor_len_v;
    ts_u16 u16_cor_len_h;   
} ts_isp_snc_fix_cor_s_t;

typedef struct ts_isp_gdc_pos_s {
    ts_u16 u16_pos_y; /*u12.4*/
    ts_u16 u16_pos_x; /*u12.4*/
} ts_isp_gdc_pos_s_t;

typedef struct ts_isp_gdc_buf_s {
    ts_bool       b_working;
    ts_buffer_s_t st_buf;
} ts_isp_gdc_buf_s_t;

typedef struct ts_isp_gdc_attr_s {
    ts_bool             b_enable;
    ts_u32              au32_rev[5];
} ts_isp_gdc_attr_s_t;

typedef struct ts_isp_isotherm_attr_s {
    ts_u08      u08_enable;
    ts_u08      u08_mode;               /* 等温线模式。0：区间内，1：区间外*/
    ts_u08      u08_y8_high;            /* 映射后Y8数据最大值 */
    ts_u08      u08_y8_low;             /* 映射后Y8数据最小值，要求y8_high>=y8_low */
    ts_u16      u16_isotherm_high;      /* 上等温线，通过用户指定温度转化得到的y16值，U16.0 */
    ts_u16      u16_isotherm_low;       /* 下等温线，通过用户指定温度转化得到的y16值，U16.0，要求isotherm_high>=isotherm_low */
    ts_u16      u16_max_y16;     
    ts_u16      u16_min_y16;      
    ts_u32      au32_rev[1];
} ts_isp_isotherm_attr_s_t;

typedef struct ts_isp_offline_frm_s {
    ts_buffer_s_t  st_buf;
    ts_void        *pv_private;
} ts_isp_offline_frm_s_t;


typedef struct ts_isp_agc_usr_hist_s         
{
    ts_void    *pv_hist;               /*hist存放地址*/
    ts_s32     s32_hist_len;           /*hist存放长度*/
    ts_s32     s32_hist_valid_len;     /*hist有效长度*/
    ts_s32     s32_acc_bit;            /*统计精度*/
    ts_s32     s32_blk_w;              /*横向分块数量*/
    ts_s32     s32_blk_h;              /*纵向分块数量*/
    ts_void    *pv_private;             
}ts_isp_agc_usr_hist_s_t;

typedef struct ts_isp_sta_usr_s         
{
    ts_s32     s32_blk_num_w;       /*横向分块数目*/
    ts_s32     s32_blk_num_h;       /*纵向分块数目*/
    ts_s32     s32_blk_size_w;      /*横向分块大小*/
    ts_s32     s32_blk_size_h;      /*纵向分块大小*/
    ts_u16     *pu16_aver;          /*均值存放地址，s32_blk_num_w * s32_blk_num_h 块连续存放*/
    ts_u16     *pu16_detail;        /*细节存放地址，s32_blk_num_w * s32_blk_num_h 块连续存放*/
    ts_u16     *pu16_lum_vari;      /*方差存放地址，s32_blk_num_w * s32_blk_num_h 块连续存放*/
    ts_void    *pv_private;            
}ts_isp_statis_usr_s_t;


/* 设备等状态定义 */
typedef enum ts_isp_image_type_en {
    TS_ISP_IMG_U16C1 = 0,
    TS_ISP_IMG_U08C1 = 1,

    TS_ISP_IMG_TYPE_BUTT
} ts_isp_image_type_en_t;

typedef struct ts_isp_image_s         
{
    ts_void    *pv_image_virt;
    ts_u16     u16_image_size_w;
    ts_u16     u16_image_size_h;
    ts_u16     u16_image_type;
    ts_u16     u16_frm_num;
    ts_void    *pv_private;             
}ts_isp_image_s_t;

typedef struct ts_isp_ext_func_s         
{
    ts_s32 (*pfn_fw_isp_get_raw)(ts_s32 s32_pos, ts_isp_image_s_t *pst_image, ts_u32 u32_timeout_ms);
    ts_void *apv_rev[7];
}ts_isp_ext_func_s_t;



typedef struct ts_op_img_s16_s         
{
    ts_s16 *ps16_addr;
    ts_s32 s32_vld_w;
    ts_s32 s32_vld_h;
    ts_s32 s32_tride;
}ts_ts_op_img_s16_s_t; 

typedef struct ts_op_flt_5x5_s         
{
    ts_s16 as16_flt_wt[5][5];
}ts_ts_op_flt_5x5_s_t; 

typedef struct ts_op_flt_3x3_s         
{
    ts_s16 as16_flt_wt[3][3];
}ts_ts_op_flt_3x3_s_t; 

typedef struct ts_op_flt_s         
{
    ts_u08 u08_flt_size;    /*滤波器选择， 3：3X3，  其余5X5*/
    ts_u08 u08_nor_bit;     /*归一化bit数*/
    union 
    {
        ts_ts_op_flt_5x5_s_t st_wt_5x5;
        ts_ts_op_flt_3x3_s_t st_wt_3x3;
    };

}ts_op_flt_s_t; 

typedef struct ts_op_meds_s         
{
    ts_u08 u08_flt_size;    /*滤波器选择， 3：3X3，  其余5X5*/
}ts_ts_op_meds_s_t; 


typedef struct ts_op_dila_s         
{
    ts_u08 u08_flt_size;    /*滤波器选择， 3：3X3，  其余5X5*/
}ts_ts_op_dila_s_t; 

typedef struct ts_op_eros_s         
{
    ts_u08 u08_flt_size;    /*滤波器选择， 3：3X3，  其余5X5*/
}ts_ts_op_eros_s_t; 

typedef struct ts_op_para_s         
{
    union 
    {
        ts_op_flt_s_t       st_flt;
        ts_ts_op_meds_s_t   st_meds;
        ts_ts_op_dila_s_t   st_dila;
        ts_ts_op_eros_s_t   st_eros;
    };

}ts_op_para_s_t; 


typedef enum ts_op_type_en {
    TS_OP_TYPE_FLT     = 0,     /*权重滤波*/
    TS_OP_TYPE_MEDS    = 1,     /*类中值滤波*/
    TS_OP_TYPE_DILA    = 2,     /*膨胀*/
    TS_OP_TYPE_EROS    = 3,     /*腐蚀*/

    TS_OP_TYPE_BUTT
} ts_op_type_en_t;

typedef struct ts_op_task_s {
    ts_op_type_en_t             en_op_type;         /*算子类型*/
    ts_ts_op_img_s16_s_t        st_img_in;          /*输入图像， 内存需要用户分配*/
    ts_ts_op_img_s16_s_t        st_img_out;         /*输出图像， 内存需要用户分配*/
    ts_op_para_s_t              st_para;            /*算子参数*/
} ts_op_task_s_t;

/* ========================================================================== */
/*                            函数定义区                                       */
/* ========================================================================== */



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __ISP_COMM_H__ */



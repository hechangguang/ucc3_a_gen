/******************************************************************************
  Copyright (C), 2022-2023, TopSilicon CO.,LTD.
 ******************************************************************************
  File Name     : aisp_common.h
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
#ifndef __AISP_COMM_H__
#define __AISP_COMM_H__


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
#include <base/ts_type.h>
/* ========================================================================== */
/*                            类型定义区                                       */
/* ========================================================================== */
typedef struct ts_aisp_comm_attr_s 
{
    ts_s32 s32_task_num;
} ts_aisp_comm_attr_s_t;

typedef struct ts_aisp_model_flie_s 
{
    ts_void *pv_model_virt_addr;      /*模型文件虚拟地址*/
    ts_u32  u32_model_phy_addr;       /*模型文件物理拟地址*/  
    ts_u32  u32_model_len;                /*模型文件长度*/
} ts_aisp_model_flie_s_t;

typedef struct ts_aisp_task_attr_s 
{
    ts_aisp_model_flie_s_t st_model_file;
    ts_u32  u32_task_node_num;  /*任务节点数量*/
} ts_aisp_task_attr_s_t;

typedef struct ts_aisp_img_info_s 
{
    ts_u32 u32_img_phy_addr;
    ts_u32 u32_img_virt_addr;
    ts_u16 u16_img_width;
    ts_u16 u16_img_height;
    ts_u16 u16_img_stride;
} ts_aisp_img_info_s_t;

#define TS_AISP_MAX_SRC_NUM (4)
#define TS_AISP_MAX_DST_NUM (8)
typedef struct ts_aisp_task_proc_s 
{
    ts_handle_t                 h_task;
    ts_u16                      u16_input_num;
    ts_u16                      u16_output_num;
    ts_aisp_img_info_s_t        ast_src_img[TS_AISP_MAX_SRC_NUM];
    ts_aisp_img_info_s_t        ast_dst_img[TS_AISP_MAX_DST_NUM];
    ts_bool                     b_flush_input;
    ts_bool                     b_flush_output;
    ts_void                     *pv_private;
} ts_aisp_task_proc_s_t;

/* ========================================================================== */
/*                            函数定义区                                       */
/* ========================================================================== */



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __AISP_COMM_H__ */



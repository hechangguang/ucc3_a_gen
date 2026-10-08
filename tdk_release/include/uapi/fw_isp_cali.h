#ifndef __FW_ISP_CALI_H__
#define __FW_ISP_CALI_H__


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
#include "isp_reg.h"
#include  "isp_ext.h"
#else
#include <base/ts_type.h>
#include <base/ts_comm.h>
#include <uapi/isp_reg.h>
#include <uapi/isp_ext.h>
#endif


/* ========================================================================== */
/*                            类型定义区                                       */
/* ========================================================================== */
typedef struct fw_isp_cali_bp_proc_s         
{
    ts_u16      u16_flt_size;       /*滤波窗大小， U4.0*/
    ts_u16      u16_diff_thr;       /*差异阈值，   U14.0*/
    ts_u16      u16_hist_thr;       /*统计值阈值， U14.0*/
    ts_u16      au16_rev[5];        
    ts_void     *pv_calc_buf;       /*用户指定计算内存， 内存大小计算为: img_w * img_h * 4 + 2048*/
}fw_isp_cali_bp_proc_s_t;

/* ========================================================================== */
/*                            函数定义区                                       */
/* ========================================================================== */
ts_s32 isp_fw_cali_det_bp_u08(fw_isp_cali_bp_proc_s_t *pst_proc, ts_isp_fw_image_s_t *pst_img_in, ts_u16 *pu16_flag);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __FW_ISP_CALI_H__ */



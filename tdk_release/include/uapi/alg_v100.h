/* 
 * sl_v100.h -- 探测器标定、优化接口头文件
 */
#ifndef __UAPI_SL_V100_H__
#define __UAPI_SL_V100_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <base/ts_type.h>
#include <base/ts_comm.h>
#include <uapi/viu_v100.h>
#include <uapi/isp_comm.h>

#define TS_SL_MAX_TPL_NUM           (32)            /*模板最大数量*/


typedef struct ts_sl_ext_func_s         
{
    ts_s32 (*pfn_sl_get_temp_cur)(ts_s32 *ps32_temp);                                       /*获取当前温度, temp * 1000*/
    ts_s32 (*pfn_sl_set_coef)(ts_u32 u32_coef);                                             /*设置coef*/
    ts_s32 (*pfn_sl_get_cetpl)(ts_void **ppv_tpl);                                            /*获取补偿模板地址*/
    ts_s32 (*pfn_sl_get_kbuf)(ts_void **ppv_tpl);                                             /*设置补偿模板地址*/
    ts_s32 (*pfn_sl_set_cetpl)(ts_void *pv_tpl);                                            /*设置补偿模板地址*/
    ts_s32 (*pfn_sl_get_cttpl)(ts_u08 u08_frm_num, ts_void *pv_tpl);                        /*根据帧数获取标定模板*/

    ts_s32 (*pfn_sl_read_tpl)(ts_u32 u32_oft_addr, ts_u32 u32_len, ts_void *pv_tpl);        /*从存储内获取对应的模板，index小于0则全部获取*/
    ts_s32 (*pfn_sl_write_tpl)(ts_u32 u32_oft_addr, ts_u32 u32_len, ts_void *pv_tpl);       /*将模板写入存储空间，index小于0则全部写*/

    ts_s32 (*pfn_sl_read_tpl_all)(ts_u32 u32_num, ts_u32 u32_tpl_sz, ts_void *pv_tpl);        /*从存储内获取对应的模板，index小于0则全部获取*/
    ts_s32 (*pfn_sl_write_tpl_all)(ts_u32 u32_num, ts_u32 u32_tpl_sz, ts_void *pv_tpl);       /*将模板写入存储空间，index小于0则全部写*/

    ts_void *apv_rev[5];
}ts_sl_ext_func_s_t;

typedef struct ts_sl_para_s {
    ts_u32 u32_tpl_min_temp_dif;               /*模板温度最小差值，范围[0-100000], 建议500*/
    ts_u32 u32_tpl_max_temp_dif;               /*标定模板时两次温度最大差值，范围[0-100000], 建议1000*/
}ts_sl_para_s_t;

typedef struct ts_sl_lock_func_s         
{
    ts_void *pv_lock;
    ts_s32 (*pfn_sl_lock)(ts_void *pv_lock);
    ts_s32 (*pfn_sl_unlock)(ts_void *pv_lock);
}ts_sl_lock_func_s_t;

typedef struct ts_sl_create_s         
{
    ts_u16              u16_img_w;    
    ts_u16              u16_img_h;   
    ts_u08              u08_tpl_num;     /*标定模板数量*/
    ts_u08              u08_rev[3];

    ts_void *           *pv_tmm_zone;    /*指定内存申请*/
    ts_sl_ext_func_s_t  st_ext_func;
    ts_sl_lock_func_s_t st_loc_func;
    ts_u32              u32_rev[10];
}ts_sl_create_s_t; 

typedef struct ts_sl_tpl_info_s         
{
    ts_u08 a08_usr_data[8];  /*用户自定义数据*/
    ts_u08 u08_tpl_grp_id;	 /*模板所属的组号*/	
    ts_u08 au08_rev[7];	 	 /*预留*/	
}ts_sl_tpl_info_s_t; 

typedef struct ts_sl_status_s         
{
    ts_s16 s16_tpl_grp_id;	 /*模板所属的组号*/	
    ts_u08 au08_rev[6];	 	 /*预留*/	
}ts_sl_status_s_t; 

ts_s32 ts_tdk_sl_create(ts_sl_create_s_t *pst_create, ts_handle_t *ph_handle);
ts_s32 ts_tdk_sl_destroy(ts_handle_t h_handle);
ts_s32 ts_tdk_sl_process(ts_handle_t h_handle, ts_s32 s32_temp, ts_bool b_switch);
ts_s32 ts_tdk_sl_get_para(ts_handle_t h_handle, ts_sl_para_s_t *pst_sl_para);
ts_s32 ts_tdk_sl_set_para(ts_handle_t h_handle, ts_sl_para_s_t *pst_sl_para);
ts_s32 ts_tdk_sl_cali_tpl(ts_handle_t h_handle, ts_u08 u08_idx, ts_sl_tpl_info_s_t *pst_tpl_info, ts_u08 u08_frm);
ts_s32 ts_tdk_sl_read_tpl(ts_handle_t h_handle, ts_u08 u08_idx);
ts_s32 ts_tdk_sl_clr_tpl(ts_handle_t h_handle, ts_u08 u08_idx);
ts_s32 ts_tdk_sl_save_tpl(ts_handle_t h_handle, ts_u08 u08_idx);

ts_s32 ts_tdk_set_tpl_info(ts_handle_t h_handle, ts_u08 u08_idx, ts_sl_tpl_info_s_t *pst_tpl_info);
ts_s32 ts_tdk_get_tpl_info(ts_handle_t h_handle, ts_u08 u08_idx, ts_sl_tpl_info_s_t *pst_tpl_info);
ts_void ts_tdk_sl_show_tpl(ts_handle_t h_handle);
ts_s32 ts_tdk_sl_force_temp_range(ts_handle_t h_handle, ts_u08 u08_idx_first, ts_u08 u08_idx_second);
ts_s32 ts_tdk_sl_get_status(ts_handle_t h_handle, ts_sl_status_s_t *pst_status);
ts_void ts_tdk_sl_set_dbg_lvl(ts_u32 u32_lvl);

ts_s32 ts_tdk_sl_oft_process(ts_handle_t h_handle, ts_s32 s32_temp, ts_bool b_switch);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __UAPI_SNR_V100_H__ */


/* 
 * isp_v100.h -- isp v100 驱动接口头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2024 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __API_ISP_V100_H__
#define __API_ISP_V100_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <base/ts_type.h>
#include <base/ts_comm.h>
#include <uapi/isp_comm.h>
#include <uapi/aisp_v100.h>

#define TDK_ISP_DEFCFG_128P     (0)
#define TDK_ISP_DEFCFG_256P     (1)
#define TDK_ISP_DEFCFG_512P     (2)
#define TDK_ISP_DEFCFG_1024P    (3)

typedef struct ts_isp_create_s {
    ts_size_s_t         st_img_size;
    ts_u32              u32_fps;       /* 帧数 * 100 */
    ts_void             *pv_cfg_addr;
} ts_isp_create_s_t;


ts_s32 ts_tdk_isp_set_comm_attr(ts_mdev_t h_isp_dev, const ts_isp_comm_attr_s_t *pst_comm);
ts_s32 ts_tdk_isp_init(ts_mdev_t h_isp_dev, ts_isp_create_s_t *pst_create);
ts_s32 ts_tdk_isp_destory(ts_mdev_t h_isp_dev);
ts_s32 ts_tdk_isp_process(ts_mdev_t h_isp_dev);
ts_s32 ts_tdk_isp_set_pause(ts_mdev_t h_isp_dev, ts_u32 u32_pause);
ts_s32 ts_tdk_isp_set_rst_val(ts_mdev_t h_isp_dev, ts_u32 u32_rs_valt);
ts_s32 ts_tdk_isp_set_adj_val(ts_mdev_t h_isp_dev, ts_u32 u32_adj_val);

ts_s32 ts_tdk_isp_write_reg(ts_mdev_t h_isp_dev, ts_u32 u32_addr, ts_u32 u32_val);
ts_s32 ts_tdk_isp_read_reg(ts_mdev_t h_isp_dev, ts_u32 u32_addr, ts_u32 *pu32_val);
ts_s32 ts_tdk_isp_get_reg_addr(ts_mdev_t h_isp_dev, ts_u32 **pu32_addr, ts_u32 *pu32_reg_len);
ts_s32 ts_tdk_isp_get_status_addr(ts_mdev_t h_isp_dev, ts_u32 **pu32_addr, ts_u32 *pu32_reg_len);
ts_s32 ts_tdk_isp_update_reg(ts_mdev_t h_isp_dev);

ts_s32 ts_tdk_isp_set_nuc_attr(ts_mdev_t h_isp_dev, const ts_isp_nuc_attr_s_t *pst_attr);
ts_s32 ts_tdk_isp_get_nuc_attr(ts_mdev_t h_isp_dev, ts_isp_nuc_attr_s_t *pst_attr);

ts_s32 ts_tdk_isp_get_nuc_kbuf(ts_mdev_t h_isp_dev, ts_buffer_s_t *pst_kbuf);
ts_s32 ts_tdk_isp_flush_nuc_kbuf(ts_mdev_t h_isp_dev, ts_buffer_s_t *pst_kbuf);
ts_s32 ts_tdk_isp_get_nuc_bbuf(ts_mdev_t h_isp_dev, ts_buffer_s_t *pst_bbuf);
ts_s32 ts_tdk_isp_get_nuc_free_bbuf(ts_mdev_t h_isp_dev, ts_buffer_s_t *pst_bbuf);
ts_s32 ts_tdk_isp_flush_nuc_bbuf(ts_mdev_t h_isp_dev, ts_buffer_s_t *pst_bbuf);
ts_s32 ts_tdk_isp_get_nuc_mbuf(ts_mdev_t h_isp_dev, ts_buffer_s_t *pst_mbuf);   
ts_s32 ts_tdk_isp_get_nuc_dbuf(ts_mdev_t h_isp_dev, ts_buffer_s_t *pst_mbuf);
ts_s32 ts_tdk_isp_get_nuc_free_dbuf(ts_mdev_t h_isp_dev, ts_buffer_s_t *pst_mbuf);
ts_s32 ts_tdk_isp_flush_nuc_dbuf(ts_mdev_t h_isp_dev, ts_buffer_s_t *pst_dbuf);
ts_s32 ts_tdk_isp_flush_nuc_all_buf(ts_mdev_t h_isp_dev, ts_buffer_s_t *pst_bbuf, ts_buffer_s_t *pst_dbuf, ts_u32 u32_coef);
ts_s32 ts_tdk_isp_calc_nuc(ts_mdev_t h_isp_dev);


/* afs */
ts_s32 ts_tdk_isp_set_afs_attr(ts_mdev_t h_isp_dev, const ts_isp_afs_attr_s_t *pst_attr);
ts_s32 ts_tdk_isp_get_afs_attr(ts_mdev_t h_isp_dev, ts_isp_afs_attr_s_t *pst_attr);
ts_s32 ts_tdk_isp_get_af_statis(ts_mdev_t h_isp_dev, ts_isp_af_statis_s_t *pst_af_statis, ts_u32 u32_millisec);
ts_s32 ts_tdk_isp_free_af_statis(ts_mdev_t h_isp_dev, ts_isp_af_statis_s_t *pst_af_statis);


/*fire*/
ts_s32 ts_tdk_isp_set_fire_attr(ts_mdev_t h_isp_dev, const ts_isp_fire_attr_s_t *pst_attr);
ts_s32 ts_tdk_isp_get_fire_attr(ts_mdev_t h_isp_dev, ts_isp_fire_attr_s_t *pst_attr);
ts_s32 ts_tdk_isp_get_fire_statis(ts_mdev_t h_isp_dev, ts_isp_fire_statis_s_t *pst_fire_statis, ts_u32 u32_millisec);
ts_s32 ts_tdk_isp_free_fire_statis(ts_mdev_t h_isp_dev, ts_isp_fire_statis_s_t *pst_fire_statis);

/*hot*/
ts_s32 ts_tdk_isp_set_hot_attr(ts_mdev_t h_isp_dev, const ts_isp_hot_attr_s_t *pst_attr);
ts_s32 ts_tdk_isp_get_hot_attr(ts_mdev_t h_isp_dev, ts_isp_hot_attr_s_t *pst_attr);
ts_s32 ts_tdk_isp_get_hot_statis(ts_mdev_t h_isp_dev, ts_isp_hot_statis_s_t *pst_hot_statis, ts_u32 u32_millisec);
ts_s32 ts_tdk_isp_free_hot_statis(ts_mdev_t h_isp_dev, ts_isp_hot_statis_s_t *pst_hot_statis);


/*sffc*/
ts_s32 ts_tdk_isp_set_sffc_attr(ts_mdev_t h_isp_dev, const ts_isp_sffc_attr_s_t *pst_attr);
ts_s32 ts_tdk_isp_get_sffc_attr(ts_mdev_t h_isp_dev, ts_isp_sffc_attr_s_t *pst_attr);
ts_s32 ts_tdk_isp_run_sffc_cali(ts_mdev_t h_isp_dev, ts_isp_sffc_cali_s_t *pst_cali, ts_isp_sffc_cali_res_s_t *pst_cali_res);
ts_s32 ts_tdk_isp_get_sffc_buf(ts_mdev_t h_isp_dev, ts_isp_sffc_buf_s_t *pst_sffc_buf);
ts_s32 ts_tdk_isp_flush_sffc_buf(ts_mdev_t h_isp_dev);
ts_s32 ts_tdk_isp_set_sffc_blk(ts_mdev_t h_isp_dev, const ts_isp_sffc_blk_info_s_t *pst_attr);
ts_s32 ts_tdk_isp_get_sffc_statis(ts_mdev_t h_isp_dev, ts_isp_sffc_statis_s_t *pst_statis);
ts_s32 ts_isp_get_sffc_usr_statis(ts_mdev_t h_isp_dev, ts_isp_sffc_statis_s_t *pst_sffc_statis);
ts_s32 ts_isp_set_sffc_usr_blk_info(ts_mdev_t h_isp_dev, const ts_isp_sffc_blk_info_s_t *pst_blk_info);

/*fix snc*/
ts_s32 ts_tdk_isp_get_snc_statis(ts_mdev_t h_isp_dev, ts_isp_snc_statis_s_t *pst_snc_statis, ts_u32 u32_millisec);
ts_s32 ts_tdk_isp_free_snc_statis(ts_mdev_t h_isp_dev, ts_isp_snc_statis_s_t *pst_snc_statis);
ts_s32 ts_tdk_isp_set_snc_fix_cor(ts_mdev_t h_isp_dev, ts_isp_snc_fix_cor_s_t *pst_snc_fix_cor);

/*gdc*/
ts_s32 ts_tdk_isp_set_gdc_enable(ts_mdev_t h_isp_dev, ts_u32 u32_en);
ts_s32 ts_tdk_isp_get_gdc_buf(ts_mdev_t h_isp_dev, ts_isp_gdc_buf_s_t *pst_gdc_buf);
ts_s32 ts_tdk_isp_flush_gdc_buf(ts_mdev_t h_isp_dev);
ts_s32 ts_tdk_isp_set_gdc_attr(ts_mdev_t h_isp_dev, const ts_isp_gdc_attr_s_t *pst_gdc_attr);
ts_s32 ts_tdk_isp_get_gdc_attr(ts_mdev_t h_isp_dev, ts_isp_gdc_attr_s_t *pst_gdc_attr);

/*freeze*/
ts_s32 ts_tdk_isp_set_frame_freeze(ts_mdev_t h_isp_dev, ts_bool b_frezze_en);

/*isotherm*/
ts_s32 ts_tdk_isp_set_isotherm_attr(ts_mdev_t h_isp_dev, const ts_isp_isotherm_attr_s_t *pst_attr);
ts_s32 ts_tdk_isp_get_isotherm_attr(ts_mdev_t h_isp_dev, ts_isp_isotherm_attr_s_t *pst_attr);

/*offline*/
ts_s32 ts_tdk_isp_set_offline_enable(ts_mdev_t h_isp_dev, ts_bool b_enable);
ts_s32 ts_tdk_isp_get_offline_frm(ts_mdev_t h_isp_dev, ts_isp_offline_frm_s_t *pst_offline_frm, ts_u32 u32_millisec);
ts_s32 ts_tdk_isp_send_offline_frm(ts_mdev_t h_isp_dev, ts_isp_offline_frm_s_t *pst_offline_frm);
ts_s32 ts_tdk_isp_free_offline_frm(ts_mdev_t h_isp_dev, ts_isp_offline_frm_s_t *pst_offline_frm);
ts_s32 ts_tdk_isp_set_offline_comm(ts_mdev_t h_isp_dev, ts_isp_offline_comm_s_t *pst_attr);

ts_s32 ts_tdk_isp_get_handle(ts_mdev_t h_isp_dev, ts_handle_t *ph_isp);

/* upload 相关 */
ts_void *tdl_isp_get_dev(ts_mdev_t h_dev);
ts_void tdl_isp_get_frm_info(ts_mdev_t h_isp, ts_u32 *pu32_width, ts_u32 *pu32_height);

/* config */
ts_void *ts_tdk_isp_get_def_cfg(ts_u32 u32_type);
ts_u32 ts_tdk_isp_get_cfg_size(ts_void);

ts_s32 ts_tdk_isp_get_pipe_pos(ts_mdev_t h_isp_dev, ts_isp_pipe_attr_s_t *pst_pipe_attr);
ts_s32 ts_tdk_isp_set_pipe_pos(ts_mdev_t h_isp_dev, const ts_isp_pipe_attr_s_t *pst_pipe_attr);

ts_s32 ts_tdk_isp_get_agc_usr_hist(ts_mdev_t h_isp_dev, ts_isp_agc_usr_hist_s_t *pst_hist, ts_u32 u32_millisec);
ts_s32 ts_tdk_isp_release_agc_usr_hist(ts_mdev_t h_isp_dev, ts_isp_agc_usr_hist_s_t *pst_hist);
ts_s32 ts_tdk_isp_get_usr_statis(ts_mdev_t h_isp_dev, ts_isp_statis_usr_s_t *pst_statis, ts_u32 u32_millisec);
ts_s32 ts_tdk_isp_release_usr_statis(ts_mdev_t h_isp_dev, ts_isp_statis_usr_s_t *pst_statis);

ts_s32 ts_tdk_isp_fix_snc_proc(ts_mdev_t h_isp_dev);
ts_s32 ts_tdk_isp_set_ext_func(ts_mdev_t h_isp_dev, ts_isp_ext_func_s_t *pst_ext_func);
ts_s32 ts_tdk_isp_get_ext_func(ts_mdev_t h_isp_dev, ts_isp_ext_func_s_t *pst_ext_func);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __API_ISP_V100_H__ */


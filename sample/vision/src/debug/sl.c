/*
 * sl.c -- 温补相关实现
 */
#include <base/ts_type.h>
#include <base/ts_comm.h>
#include <base/ts_errno.h>
#include <base/ts_media.h>
#include <cpl/ts_cpl.h>
#include <uapi/base_v100.h>
#include <uapi/chip_def.h>
#include <uapi/osd_v100.h>
#include <uapi/vpo_v100.h>
#include <uapi/alg_v100.h>
#include <command.h>
#include <stdio.h>
#include <uapi/sfc_v100.h>
#include "snr_comm.h"
#include "api.h"
#include "sl.h"


/* hbp协议获取 */
ts_void ts_tcorr_get_stat(ts_u32 *pu32_cnt, ts_u08 *pu08_buf)
{
    ts_u32 i;
    ts_u08 au08_buf[TCORR_MAX_TPL_NUM];
    ts_u08 *pu08_ptr;
    ts_u32 u32_cnt;
    ts_sl_tpl_info_s_t st_tpl_info;

    pu08_ptr = pu08_buf;
    u32_cnt = 0;

    for (i = 0; i < TCORR_MAX_TPL_NUM; ++i) {
        ts_cpl_memset(au08_buf, 0, sizeof(au08_buf));
        ts_tdk_get_tpl_info(gst_snr_ctx.h_sl, i, (ts_sl_tpl_info_s_t *)&st_tpl_info);
        if (0xa5 == st_tpl_info.a08_usr_data[1]) {
            pu08_ptr[0 + u32_cnt * 3] = i;
            pu08_ptr[1 + u32_cnt * 3] = st_tpl_info.a08_usr_data[0];
            pu08_ptr[2 + u32_cnt * 3] = st_tpl_info.u08_tpl_grp_id;
            u32_cnt++;
        }
    }

    *pu32_cnt = u32_cnt * 3;
}


static ts_s32 ts_tcorr_read_tpl_all(ts_u32 u32_num, ts_u32 u32_tpl_sz, ts_void *pv_tpl)
{
    ts_s32 s32_hfile = ts_lib_sfc_open(FN_B_CFG);
    if (s32_hfile <= 0) {
        printf("open file fail\n");

        return TS_ERR;
    }

    ts_s32 s32_ret = ts_lib_sfc_read(s32_hfile, 0, (ts_u08 *)pv_tpl, u32_num * u32_tpl_sz);
    if ((u32_num * u32_tpl_sz) != s32_ret) {
        printf("ts_lib_sfc_read err ret = %d\n", s32_ret);
        ts_lib_sfc_close(s32_hfile);

        return TS_ERR;
    }

    ts_lib_sfc_close(s32_hfile);

    return TS_OK;
}

static ts_s32 ts_tcorr_write_tpl_all(ts_u32 u32_num, ts_u32 u32_tpl_sz, ts_void *pv_tpl)
{
    ts_s32 s32_hfile = ts_lib_sfc_open(FN_B_CFG);
    if (s32_hfile <= 0) {
        printf("open flash cfg fail\n");
        return TS_ERR;
    }

    ts_s32 s32_ret = ts_lib_sfc_write(s32_hfile, 0, (ts_u08 *)pv_tpl, u32_num * u32_tpl_sz);
    if((u32_num * u32_tpl_sz) != s32_ret) {
        printf("ts_lib_sfc_write err ret = %d\n", s32_ret);
        ts_lib_sfc_close(s32_hfile);

        return TS_ERR;
    }

    ts_lib_sfc_close(s32_hfile);
    return TS_OK;
}


static ts_s32 ts_tcorr_get_temp_cur(ts_s32 *ps32_temp)
{
    *ps32_temp = sns_get_temperature();

    return TS_OK;
}

static ts_s32 ts_tcorr_set_coef(ts_u32 u32_coef)
{
    ts_isp_nuc_attr_s_t st_nuc_attr;

    ts_tdk_isp_get_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr);
    st_nuc_attr.u16_coef = u32_coef;
    ts_tdk_isp_set_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr);

    return TS_OK;
}

static ts_s32 ts_tcorr_get_cetpl(ts_void **ppv_tpl)
{
    ts_buffer_s_t st_dbuf;
    ts_s32 s32_ret;

    s32_ret = ts_tdk_isp_get_nuc_dbuf(TS_ISP_MDEV0, &st_dbuf);
    if (s32_ret != TS_OK) {
        printf("ts_tdk_isp_get_nuc_dbuf err !\n");

        return TS_ERR;
    }

    *ppv_tpl = st_dbuf.pu08_ptr;

    return TS_OK;
}

static ts_s32 ts_tcorr_set_cetpl(ts_void *pv_tpl)
{
    ts_buffer_s_t st_dbuf;
    ts_s32 s32_ret;

    if (ts_null == pv_tpl) {
        printf("ts_tcorr_set_cetpl pv_tpl NULL!\n");

        return TS_ERR;
    }

    s32_ret = ts_tdk_isp_get_nuc_dbuf(TS_ISP_MDEV0, &st_dbuf);
    if (s32_ret != TS_OK) {
        printf("ts_tdk_isp_get_nuc_dbuf err !\n");

        return TS_ERR;
    }

    s32_ret = ts_tdk_isp_flush_nuc_dbuf(TS_ISP_MDEV0, &st_dbuf);
    if (s32_ret != TS_OK) {
        printf("flush nuc dbuf err\n");

        return TS_ERR;
    }

    return TS_OK;
}

static ts_s32 ts_tcorr_get_cttpl(ts_u08 u08_frm_num, ts_void *pv_tpl)
{ 
    /* 按帧读标定模板 */
    ts_isp_nuc_attr_s_t st_nuc_attr;
    ts_buffer_s_t st_mbuf;
    ts_s32 s32_frm;

    ts_tdk_isp_get_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr); 
    s32_frm = st_nuc_attr.u08_stat_frm_num;
    st_nuc_attr.u08_stat_frm_num = u08_frm_num;
    ts_tdk_isp_set_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr); 

    tdk_snrcfg_shutter_close();
    ts_tdk_isp_calc_nuc(TS_ISP_MDEV0);  
    tdk_snrcfg_shutter_open();

    st_nuc_attr.u08_stat_frm_num = s32_frm;
    ts_tdk_isp_set_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr); 
    ts_tdk_isp_get_nuc_mbuf(TS_ISP_MDEV0, &st_mbuf);

    ts_cpl_memcpy(pv_tpl, st_mbuf.pu08_ptr, st_mbuf.sz_size);

    return TS_OK; 
}

ts_s32 ts_tcorr_read_tpl(ts_u32 u32_oft_addr, ts_u32 u32_len, ts_void *pv_tpl)
{
    ts_s32 s32_hfile;
    ts_s32 s32_ret;
    
    s32_hfile = ts_lib_sfc_open(FN_B_CFG);
    if (s32_hfile <= 0) {
        printf("open file fail\n");

        return TS_ERR;
    }

    s32_ret = ts_lib_sfc_read(s32_hfile, u32_oft_addr, (ts_u08 *)pv_tpl, u32_len);
    if(u32_len != s32_ret) {
        printf("ts_lib_sfc_read err ret = %d\n", s32_ret);
        ts_lib_sfc_close(s32_hfile);

        return TS_ERR;
    }

    ts_lib_sfc_close(s32_hfile);

    return TS_OK;
}


ts_s32 ts_tcorr_write_tpl(ts_u32 u32_oft_addr, ts_u32 u32_len, ts_void *pv_tpl)
{
    ts_s32 s32_hfile;
    ts_s32 s32_ret;
    
    s32_hfile = ts_lib_sfc_open(FN_B_CFG);
    if (s32_hfile <= 0) {
        printf("open flash cfg fail\n");

        return TS_ERR;
    }

    s32_ret = ts_lib_sfc_write(s32_hfile, u32_oft_addr, (ts_u08 *)pv_tpl, u32_len);
    if(u32_len != s32_ret) {
        printf("ts_lib_sfc_write err ret = %d\n", s32_ret);
        ts_lib_sfc_close(s32_hfile);

        return TS_ERR;
    }

    ts_lib_sfc_close(s32_hfile);

    return TS_OK;
}

extern ts_void iray6122_find_sfb(ts_u32 u32_ave);

ts_s32 ts_sl_init(ts_void)
{
    snr_attr_s_t *pst_attr;
    ts_sl_create_s_t st_create_cfg;
    ts_sl_ext_func_s_t st_sl_ext_func;
    ts_s32 s32_ret;

    pst_attr = (snr_attr_s_t *)&gst_snr_ctx.st_attr;
    vision_log_notice("Temperature Compensation Usage Plan %d\n", 1);
    ts_cpl_memset((ts_void *)&st_sl_ext_func, 0, sizeof(ts_sl_ext_func_s_t));
    st_sl_ext_func.pfn_sl_get_temp_cur = ts_tcorr_get_temp_cur;
    st_sl_ext_func.pfn_sl_set_coef = ts_tcorr_set_coef;
    st_sl_ext_func.pfn_sl_get_cetpl = ts_tcorr_get_cetpl;
    st_sl_ext_func.pfn_sl_set_cetpl = ts_tcorr_set_cetpl;
    st_sl_ext_func.pfn_sl_get_cttpl = ts_tcorr_get_cttpl;
    st_sl_ext_func.pfn_sl_read_tpl = ts_tcorr_read_tpl;
    st_sl_ext_func.pfn_sl_write_tpl = ts_tcorr_write_tpl;
    st_sl_ext_func.pfn_sl_read_tpl_all = ts_tcorr_read_tpl_all;
    st_sl_ext_func.pfn_sl_write_tpl_all = ts_tcorr_write_tpl_all;

    ts_cpl_memset(&st_create_cfg, 0, sizeof(st_create_cfg));
    st_create_cfg.u16_img_w = pst_attr->s32_img_w;
    st_create_cfg.u16_img_h = pst_attr->s32_img_h;
    st_create_cfg.u08_tpl_num = TCORR_MAX_TPL_NUM;
    st_create_cfg.pv_tmm_zone = gpv_tmm_isp0;
    st_create_cfg.st_ext_func = st_sl_ext_func;

    ts_tdk_sl_set_dbg_lvl(TS_LOG_DEBUG);

    s32_ret = ts_tdk_sl_create(&st_create_cfg, &gst_snr_ctx.h_sl);
    if (TS_OK != s32_ret) {
        vision_log_err("ts_tdk_sl_create fail\n");
    
        return TS_ERR;
    }

    return TS_OK;
}



extern ts_void snr_exec_ooc(ts_u32 u32_rst_en, ts_u32 u32_rst_val, ts_u32 u32_sht_en, ts_u32 u32_occ_en);


/* 工具端同步使用 */ 
TS_DECLARE_CMD(sl)
{
    if (!ts_cpl_strcmp(argv[1], "set_dbuf")) {
        ts_u08 u08_idx_first;
        ts_u08 u08_idx_second;

        u08_idx_first = ts_cpl_str2num(argv[2]);
        u08_idx_second = ts_cpl_str2num(argv[3]);
        ts_tdk_sl_force_temp_range(gst_snr_ctx.h_sl, u08_idx_first, u08_idx_second);

        return 0;
    }

    if (!ts_cpl_strcmp(argv[1], "show_tpl")) {
        ts_tdk_sl_show_tpl(gst_snr_ctx.h_sl);

        return 0;
    }


    // if(!ts_cpl_strcmp(argv[1], "tpl_test")) {
    //     ts_s08 s08_tmp[8] = {0};
    //     ts_s32 s32_idx = ts_cpl_str2num(argv[2]);

    //     ts_tdk_get_usr_data(gst_snr_ctx.h_sl, s32_idx, (ts_sl_usr_data_s_t *)s08_tmp);
    //     printf("tpl idx = %d, data = %d, %d, %d, %d, %d, %d, %d, %d\n", s32_idx,
    //             s08_tmp[0], s08_tmp[1], s08_tmp[2], s08_tmp[3],
    //             s08_tmp[4], s08_tmp[5], s08_tmp[6], s08_tmp[7]);

    //     return 0;
    // }

    if (!ts_cpl_strcmp(argv[1], "coef")) {
        ts_isp_nuc_attr_s_t st_nuc_attr;
        ts_tdk_isp_get_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr); 
        printf("s32_coef = %d\n", (ts_s16)st_nuc_attr.u16_coef );

        // ts_tdk_isp_set_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr);

        return 0;
    }


    if (!ts_cpl_strcmp(argv[1], "cali_tpl")) {
        ts_sl_tpl_info_s_t st_tpl_info;
        ts_s32 s32_idx;

        s32_idx = ts_cpl_str2num(argv[2]);
        /* 0byte 温度 */
        /* 1byte 标定标志 0xa5已进行标定 */
        st_tpl_info.a08_usr_data[0] = ts_cpl_str2num(argv[3]);
        st_tpl_info.a08_usr_data[1] = 0xa5;
        st_tpl_info.u08_tpl_grp_id = ts_cpl_str2num(argv[4]);

        ts_tdk_sl_cali_tpl(gst_snr_ctx.h_sl, s32_idx, &st_tpl_info, 128);
        // ts_tdk_set_tpl_info(gst_snr_ctx.h_sl, s32_idx, (ts_sl_usr_data_s_t *)s08_tmp);

        return 0;
    }

    if (!ts_cpl_strcmp(argv[1], "save_tpl")){
        ts_s32 s32_idx = ts_cpl_str2num(argv[2]);
        ts_tdk_sl_save_tpl(gst_snr_ctx.h_sl, s32_idx);

        return 0;
    }

    if (!ts_cpl_strcmp(argv[1], "read_tpl")){
        ts_s32 s32_idx = ts_cpl_str2num(argv[2]);
        ts_tdk_sl_read_tpl(gst_snr_ctx.h_sl, s32_idx);

        return 0;
    }

    if (!ts_cpl_strcmp(argv[1], "clr_tpl")){
        ts_s32 s32_idx = ts_cpl_str2num(argv[2]);
        ts_tdk_sl_clr_tpl(gst_snr_ctx.h_sl, s32_idx);

        return 0;
    }

    return 0;
}
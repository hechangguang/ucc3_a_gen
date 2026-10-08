/*
 * rtd123c.c -- vision for rtd123c
 */
#include <base/ts_type.h>
#include <base/ts_compiler.h>
#include <cpl/ts_cpl.h>
#include <uapi/chip_def.h>
#include <uapi/isp_v100.h>
#include <uapi/aisp_v100.h>
#include <uapi/viu_v100.h>
#include <uapi/clkm_v100.h>
#include <uapi/vpo_v100.h>
#include <uapi/wdt_v100.h>
#include <uapi/base_v100.h>
#include <uapi/gpio_v100.h>
#include <uapi/isp_comm.h>
#include <uapi/sfc_v100.h>
#include <uapi/i2c_v100.h>
#include <uapi/snr_v100.h>
#include "api.h"
#include "snr_comm.h"
#include "menu/votmenu.h"
#include <planck640c_vot.h>
#include <planck1280c_vot.h>
#include <menu/ispfunction.h>

extern SysCtrlInfo_s_t global_sys_ctrl_info;
// ts_s32 rtd123c_get_temperature(ts_void)
// {
//     ts_s32 s32_ret ,s32_temp;
//     s32_ret = ts_tdk_snr_get_temp(&s32_temp);
//     if (TS_OK == s32_ret) {
//         return s32_temp;
//     } else {
//         return 0;
//     }
// }

ts_s32 rtd123c_get_temperature(ts_void)
{
    ts_u32 u32_fv;
    ts_s32 s32_ret, s32_temp;

    ts_cpl_mutex_lock(&global_sys_ctrl_info.m_viu_stats_mutex);

    s32_ret = ts_tdk_viu_get_stats(TS_VIU_MDEV0, TS_VIU_STATS_COL, 1294, 0, (ts_u32 *)&u32_fv, ts_null);
    if (TS_OK == s32_ret) {
        ts_s32 s32_fv = u32_fv;
        ts_f32 f32_fv = s32_fv;
        f32_fv = 20.0 + (f32_fv / 522.0 - 6867.0) / (0 - 33.8);
        s32_temp = 1000.0 * f32_fv;
    } else {
        s32_temp = 0;
    }

    ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_viu_stats_mutex);

    return s32_temp;
}

#define CORR_SNR_MEAN            (0)
#define CORR_SNR_MEAN_THR        (1600)
#define CORR_SNR_MEAN_GOAL       (8000)
#define CORR_SNR_MEAN_GOAL_THR   (800)

ts_u32 rtds123_snr_corr_mean(ts_void) 
{
    ts_viu_array_info_s_t st_viu_array_info;
    // ts_u32 u32_mean_diff = 0;
    // ts_u08 u08_mean_diff_pos = 1;
    // ts_u08 u08_mean_diff_pos_new = 1;
    // ts_u32 s32_ntcc = 0;
    // ts_snr_coef_s_t st_snr_coef;
    ts_u32 u32_mean;
    // st_snr_coef.en_coef = TS_SNR_COEF_NTCC;

    u32_mean = 0;
    ts_tdk_viu_get_mean(TS_VIU_MDEV0, &st_viu_array_info);
    u32_mean = st_viu_array_info.u32_mean;

    // if(st_viu_array_info.u32_mean > CORR_SNR_MEAN_GOAL) {
    //     u32_mean_diff = st_viu_array_info.u32_mean - CORR_SNR_MEAN_GOAL;
    // } else {
    //     u32_mean_diff = CORR_SNR_MEAN_GOAL - st_viu_array_info.u32_mean;
    // }

    // vision_log_debug("current mean diff is %d\n",u32_mean_diff);

    // if(u32_mean_diff > CORR_SNR_MEAN_THR) {
    //     vision_log_debug("start change work point...\n");

    //     ts_tdk_snr_get_coef(&st_snr_coef);
    //     vision_log_debug("initial nttc is %d\n",st_snr_coef.u32_rwdata);
    //     for(ts_s32 i=0; i<32; i++){
    //         vision_log_debug("current mean is %d\n",st_viu_array_info.u32_mean);
    //         u08_mean_diff_pos = (st_viu_array_info.u32_mean > CORR_SNR_MEAN_GOAL) ? 1 : 0;
    //         ts_tdk_snr_get_coef(&st_snr_coef);
    //         s32_ntcc = st_snr_coef.u32_rwdata;
    //         if(!u08_mean_diff_pos) {
    //             s32_ntcc -= 1;
    //         } else {
    //             s32_ntcc += 1;
    //         }

    //         if(s32_ntcc > 8191)
    //             s32_ntcc = 8191;

    //         if(s32_ntcc < 0)
    //             s32_ntcc = 0;

    //         st_snr_coef.u32_rwdata = s32_ntcc;
    //         ts_tdk_snr_set_coef(&st_snr_coef);

    //         ts_cpl_msleep(1000/50*2);
    //         ts_tdk_viu_get_mean(TS_VIU_MDEV0, &st_viu_array_info);

    //         if(st_viu_array_info.u32_mean > CORR_SNR_MEAN_GOAL) {
    //             u08_mean_diff_pos_new = 1;
    //             u32_mean_diff = st_viu_array_info.u32_mean - CORR_SNR_MEAN_GOAL;
    //         } else {
    //             u08_mean_diff_pos_new = 0;
    //             u32_mean_diff = CORR_SNR_MEAN_GOAL - st_viu_array_info.u32_mean;
    //         }

    //         if(u32_mean_diff < CORR_SNR_MEAN_GOAL_THR && (u08_mean_diff_pos != u08_mean_diff_pos_new)){
    //             break;
    //         }
    //     }
    //     u32_mean = st_viu_array_info.u32_mean;
    //     vision_log_debug("final ntcc is %d\n",s32_ntcc);
    //     vision_log_debug("final mean is %d\n",st_viu_array_info.u32_mean);

    //     ts_cpl_msleep(1000/50);
    // }

    return u32_mean;
}

static ts_void snr_cut(ts_u32 u32_cut_status,ts_s32 *ps32_temp,ts_u32 *pu32_mean)
{
    ts_cpl_timeval_s_t st_time0, st_time1, st_time2, st_time3, st_time4, st_time5, st_time6, st_time7, st_time8, st_time9;

    ts_cpl_gettimeofday(&st_time0);
    ts_tdk_vpo_freeze(TS_VPO_MDEV0);
    ts_cpl_gettimeofday(&st_time1);

    ts_tdk_isp_set_pause(TS_ISP_MDEV0, 0xfffffffc);////暂停计算
    ts_tdk_isp_set_rst_val(TS_ISP_MDEV0, 0x7);////复位fw，清除内部迭代信息
    ts_cpl_gettimeofday(&st_time2);

    //数据流已经关闭，进行相应操作
    tdk_snrcfg_shutter_close();
    ts_cpl_gettimeofday(&st_time3);

    ts_s32 s32_temp_1st = sns_get_temperature();
    ts_cpl_gettimeofday(&st_time4);

    rtds123_snr_corr_mean();

    ts_tdk_isp_calc_nuc(TS_ISP_MDEV0);  
    ts_cpl_gettimeofday(&st_time5);

    ts_s32 s32_temp_2st = sns_get_temperature();
    ts_cpl_gettimeofday(&st_time6);

    tdk_snrcfg_shutter_open();
    ts_cpl_gettimeofday(&st_time7);

    ts_tdk_isp_set_pause(TS_ISP_MDEV0, 0);////复位fw，清除内部迭代信息
    ts_cpl_msleep(1010 * 3 / gst_snr_ctx.st_attr.s32_fps); //此处为系统数据流刷新时间， 建议为3-4， 如果打挡片前后图像差异不大，可以适当减少。
    ts_tdk_isp_set_rst_val(TS_ISP_MDEV0, 0);////pause
    ts_cpl_gettimeofday(&st_time8);

    ts_tdk_vpo_unfreeze(TS_VPO_MDEV0);
    ts_cpl_gettimeofday(&st_time9);

    *ps32_temp = (s32_temp_1st + s32_temp_2st) / 2;

#if 0
    vision_log_debug("cut time cost list : \n");
    vision_log_debug("total = %d ms : \n",  ((st_time9.tv_sec - st_time0.tv_sec) * 1000000 + st_time9.tv_usec - st_time0.tv_usec) / 1000);
    vision_log_debug("vpo frz on  %d ms : \n",  ((st_time1.tv_sec - st_time0.tv_sec) * 1000000 + st_time1.tv_usec - st_time0.tv_usec) / 1000);
    vision_log_debug("isp frz   %d ms : \n",  ((st_time2.tv_sec - st_time1.tv_sec) * 1000000 + st_time2.tv_usec - st_time1.tv_usec) / 1000);
    vision_log_debug("sht close   %d ms : \n",  ((st_time3.tv_sec - st_time2.tv_sec) * 1000000 + st_time3.tv_usec - st_time2.tv_usec) / 1000);
    vision_log_debug("temp 1   %d ms : \n",  ((st_time4.tv_sec - st_time3.tv_sec) * 1000000 + st_time4.tv_usec - st_time3.tv_usec) / 1000);
    vision_log_debug("nuc    %d ms : \n",   ((st_time5.tv_sec - st_time4.tv_sec) * 1000000 + st_time5.tv_usec - st_time4.tv_usec) / 1000);
    vision_log_debug("temp 2   %d ms : \n",  ((st_time6.tv_sec - st_time5.tv_sec) * 1000000 + st_time6.tv_usec - st_time5.tv_usec) / 1000);
    vision_log_debug("sht on   %d ms : \n",  ((st_time7.tv_sec - st_time6.tv_sec) * 1000000 + st_time7.tv_usec - st_time6.tv_usec) / 1000);
    vision_log_debug("wait frz  %d ms : \n",  ((st_time8.tv_sec - st_time7.tv_sec) * 1000000 + st_time8.tv_usec - st_time7.tv_usec) / 1000);
    vision_log_debug("vpo frz off  %d ms : \n",  ((st_time9.tv_sec - st_time8.tv_sec) * 1000000 + st_time9.tv_usec - st_time8.tv_usec) / 1000);
#endif
}

ts_void rtds123_snr_auto_cut_process(ts_void)
{
    ts_cpl_timeval_s_t st_time_start, st_time_cur, st_time_last_cut;
    ts_u32 u32_time_diff, u32_reg_clr;
    ts_u32 u32_cut_status = 0;
    ts_bool b_non_static = 1;
    ts_s32 s32_cut_time_gap_non_static = 30 * 1000;
    ts_s32 s32_cut_time_gap_static = 60 * 1000;
    ts_s32 s32_static_time = 3 * 60 * 1000;
    ts_s32 s32_temp_diff_thr = 2000;
    ts_u32 u32_shutter_mean_last = 0xffffffff;
    ts_s32 s32_temp_last;

    gst_snr_ctx.s32_run_auto_cut = 1;

    ts_cpl_gettimeofday(&st_time_start);
    snr_cut(u32_cut_status,&s32_temp_last,&u32_shutter_mean_last);
    ts_cpl_gettimeofday(&st_time_last_cut);
    while (1) {
        u32_cut_status = 0;
        if (b_non_static) {
            ts_cpl_gettimeofday(&st_time_cur);
            u32_time_diff = ts_cpl_get_timedif(&st_time_last_cut, &st_time_cur);
            u32_time_diff = u32_time_diff / 1000;
            if (u32_time_diff >= s32_cut_time_gap_non_static)
               u32_cut_status |= 0x1; 

            u32_time_diff = ts_cpl_get_timedif(&st_time_start, &st_time_cur);
            u32_time_diff = u32_time_diff / 1000;
            if (u32_time_diff >= s32_static_time)
                b_non_static = 0;
        } else {
            ts_cpl_gettimeofday(&st_time_cur);
            u32_time_diff = ts_cpl_get_timedif(&st_time_last_cut, &st_time_cur);
            u32_time_diff = u32_time_diff / 1000;
            if (u32_time_diff >= s32_cut_time_gap_static)
               u32_cut_status |= 0x2;
        }

        ts_s32 s32_temp = sns_get_temperature();
        ts_s32 s32_diff_temp = s32_temp - s32_temp_last;
        if (s32_diff_temp < 0)
            s32_diff_temp = 0 - s32_diff_temp;

        if (s32_diff_temp >= s32_temp_diff_thr)
            u32_cut_status |= 0x4; 

        if (gst_snr_ctx.s32_force_cut) {
            u32_cut_status |= 0x8;
            gst_snr_ctx.s32_force_cut = 0;
        }

        if ((u32_cut_status != 0) && gst_snr_ctx.s32_run_auto_cut) {
            ts_tdk_isp_read_reg(TS_ISP_MDEV0, 0x18, &u32_reg_clr);
            u32_reg_clr |= (1 << 15);
            ts_tdk_isp_write_reg(TS_ISP_MDEV0, 0x18, u32_reg_clr);

            snr_cut(u32_cut_status,&s32_temp_last,&u32_shutter_mean_last);
            ts_cpl_gettimeofday(&st_time_last_cut);     
            vision_log_debug("to cut, status = 0x%x \n", u32_cut_status);       

            ts_tdk_isp_read_reg(TS_ISP_MDEV0, 0x18, &u32_reg_clr);
            u32_reg_clr &= ~(1 << 15);
            ts_tdk_isp_write_reg(TS_ISP_MDEV0, 0x18, u32_reg_clr);
        }

        ts_cpl_msleep(100);  
    }
}



ts_void rtd123c_clkm_init(snr_attr_s_t *pst_attr)
{
    ts_clkm_cfg_s_t st_clk_cfg;
    ts_clkm_viu_cfg_s_t *pst_viu_clk;
    ts_clkm_vpo_cfg_s_t *pst_vpo_clk;

    pst_viu_clk = (ts_clkm_viu_cfg_s_t *)&st_clk_cfg.st_viu_clk;
    pst_vpo_clk = (ts_clkm_vpo_cfg_s_t *)&st_clk_cfg.st_vpo_clk;

    /* viu 时钟 */
    pst_viu_clk->u32_clk_hz = 90000000;
    pst_viu_clk->en_clk_src = TS_CLKM_SRC_ISP_CLK;

    pst_viu_clk->u32_snr_clk = 85000000;
    pst_viu_clk->u32_snr_internal_clk_en = 1;
    pst_viu_clk->u32_snr_o_clk_phase = 0;
    pst_viu_clk->u32_snr_o_clk_inv = 0;

    pst_viu_clk->u32_sample_clk_en = 1;
    pst_viu_clk->en_sample_clk_src = TS_CLKM_SRC_VIU_SNR_CLK;
    pst_viu_clk->u32_sample_clk_phase = 0;
    pst_viu_clk->u32_sample_clk_inv = 0;

    /* isp clock */
    if (30 == pst_attr->s32_fps) {
        st_clk_cfg.u32_isp_clk_hz = 45000000;

        /* vpo module */
        pst_vpo_clk->en_vpo_clk_src = TS_CLKM_SRC_VPO_PLL;
        pst_vpo_clk->u32_vpo_clk_hz = 90000000;

        /* vpo btenv */
        pst_vpo_clk->u32_btenc_pix_clk_hz = 45000000;
        pst_vpo_clk->u32_btenc_mod_clk_hz = 45000000;
        pst_vpo_clk->u32_btenc_o_clk_inv = 0;

        /* vpo cvbs */
        pst_vpo_clk->u32_cvbs_clk_en = 1;
        pst_vpo_clk->u32_dac_clk_en = 1;
        pst_vpo_clk->en_dac_clk_src = TS_CLKM_SRC_CVBS_PLL;
        pst_vpo_clk->u32_dac_clk_inv = 1;

        /* vpo lcd */
        pst_vpo_clk->u32_lcd_pix_clk_hz = 45000000;
        pst_vpo_clk->u32_lcd_mod_clk_hz = 45000000;
        pst_vpo_clk->u32_lcd_o_clk_en = 0;
    } 
    else if (50 == pst_attr->s32_fps) 
    {
        st_clk_cfg.u32_isp_clk_hz = 90000000;

        /* vpo module */
        pst_vpo_clk->en_vpo_clk_src = TS_CLKM_SRC_VPO_PLL;
        pst_vpo_clk->u32_vpo_clk_hz = 90000000;

        /* vpo btenv */
        pst_vpo_clk->u32_btenc_pix_clk_hz = 90000000;
        pst_vpo_clk->u32_btenc_mod_clk_hz = 90000000;
        pst_vpo_clk->u32_btenc_o_clk_inv = 1;

        /* vpo cvbs */
        pst_vpo_clk->u32_cvbs_clk_en = 1;
        pst_vpo_clk->u32_dac_clk_en = 1;
        pst_vpo_clk->en_dac_clk_src = TS_CLKM_SRC_CVBS_PLL;
        pst_vpo_clk->u32_dac_clk_inv = 1;

        /* vpo lcd */
        pst_vpo_clk->u32_lcd_pix_clk_hz = 90000000;
        pst_vpo_clk->u32_lcd_mod_clk_hz = 90000000;
        pst_vpo_clk->u32_lcd_o_clk_en = 0;
    }

    /* vpo mipi */
    pst_vpo_clk->u32_mipi_dhpy_ref_clk_en = 0;

    ts_tdk_clkm_set((const ts_clkm_cfg_s_t *)&st_clk_cfg);
}


ts_void rtd123c_viu_init(snr_attr_s_t *pst_attr, ts_viu_snr_attr_s_t *pst_snr_attr)
{
    ts_viu_comm_attr_s_t st_comm_attr;
    ts_viu_corr_attr_s_t *pst_corr;
    ts_viu_snr_timing_attr_s_t *pst_timing_attr;

    ts_cpl_memset32((ts_u32 *)&st_comm_attr, 0, (sizeof(ts_viu_comm_attr_s_t) >> 2));

    st_comm_attr.en_snr = TS_VIU_SNRT_RTD123C;
    pst_timing_attr = &st_comm_attr.st_timing_attr;

    if (30 == pst_attr->s32_fps) {
#if 0
        pst_timing_attr->u08_redefine_timing_en = 0;
#else
        pst_timing_attr->u08_redefine_timing_en = 1;

        pst_timing_attr->u08_img_inv_en = 0;
        pst_timing_attr->u16_raw_h = 1055;
        pst_timing_attr->u16_raw_w = 2683;
        pst_timing_attr->u16_extra_timing = 2683;
        pst_timing_attr->u16_vsync_start = 2;
        pst_timing_attr->u16_hsync_start = 0;
        pst_timing_attr->u16_vld_h = 1031;
        pst_timing_attr->u16_vld_w = 1296;
        pst_timing_attr->u16_cut_start_v = 5;
        pst_timing_attr->u16_cut_start_h = 10;
        pst_timing_attr->u32_data_dly = 14272;
#endif
    } else if (50 == pst_attr->s32_fps) {
        pst_timing_attr->u08_redefine_timing_en = 1;

        pst_timing_attr->u08_img_inv_en = 0;
        pst_timing_attr->u16_raw_h = 1055;
        pst_timing_attr->u16_raw_w = 1610;
        pst_timing_attr->u16_extra_timing = 1450;
        pst_timing_attr->u16_vsync_start = 2;
        pst_timing_attr->u16_hsync_start = 0;
        pst_timing_attr->u16_vld_h = 1031;
        pst_timing_attr->u16_vld_w = 1296;
        pst_timing_attr->u16_cut_start_v = 5;
        pst_timing_attr->u16_cut_start_h = 10;
        pst_timing_attr->u32_data_dly = 8591;
    }

    st_comm_attr.u16_height = 1024;
    st_comm_attr.u16_width = 1280;
    st_comm_attr.u08_pixel_ratio = 1;
    st_comm_attr.f32_fps = (ts_f32)pst_attr->s32_fps;
    st_comm_attr.u16_rst_dly_us = 10000;

    if ((pst_attr->st_cvbs.u08_en > 0) && (pst_attr->st_cvbs.u08_sd_en > 0)) {
        st_comm_attr.u08_ex_vsync_en = 1;
        st_comm_attr.u08_ex_vsync_mode = TS_VIU_EX_VSYNC_CVBS;
    } else {
        st_comm_attr.u08_ex_vsync_en = 0;
    }

    /* 线序 */
    st_comm_attr.u32_i_order_hi = 0xfedcba98;
    st_comm_attr.u32_i_order_lo = 0x76543210;
    st_comm_attr.u32_o_order_hi = 0x00000098;
    st_comm_attr.u32_o_order_lo = 0x76543210;

    pst_corr = (ts_viu_corr_attr_s_t *)&st_comm_attr.st_corr_attr;
    pst_corr->u08_alg = TS_VIU_CORR_ALG_SA;
    pst_corr->u08_reset_val = 0x80;
    pst_corr->u08_max = 0xf8;
    pst_corr->u08_min = 0;

    /* 单次触发 */
    pst_corr->u08_freq = 1;

    /* 快调 */
    pst_corr->st_fast.u16_thr_hi = 0x2400;
    pst_corr->st_fast.u16_thr_lo = 0x1c00;
    pst_corr->st_fast.u08_mode = TS_VIU_ADJ_MODE_BACKWARD;
    pst_corr->st_fast.u08_step = 4;
    pst_corr->st_fast.u08_mask = 0x00;
    pst_corr->st_fast.u08_frame_cnt = 0;

    /* 细调 */
    pst_corr->st_trim.u16_thr_hi = 0x2100;
    pst_corr->st_trim.u16_thr_lo = 0x1f00;
    pst_corr->st_trim.u08_mode = TS_VIU_ADJ_MODE_BACKWARD;
    pst_corr->st_trim.u08_step = 4;
    pst_corr->st_trim.u08_mask = 0xfc;
    pst_corr->st_trim.u08_frame_cnt = 0x20;

    ts_tdk_viu_set_comm_attr(TS_VIU_MDEV0, (const ts_viu_comm_attr_s_t *)&st_comm_attr);
    ts_tdk_viu_set_snr_attr(TS_VIU_MDEV0, (const ts_viu_snr_attr_s_t *)pst_snr_attr);
}


/*
 * rtd_6122.c -- vision iray 6122 实现，for 6122
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
#include <stdio.h>
#include "api.h"
#include "snr_comm.h"
#include "menu/votmenu.h"
#include <planck640c_vot.h>
#include <planck1280c_vot.h>

#include <menu/ispfunction.h>

// extern ts_u08 preStartForceShiftFlag;

#define RTD6122_WIDTH           (640)
#define RTD6122_HEIGHT          (512)

extern SysCtrlInfo_s_t global_sys_ctrl_info;

ts_viu_snr_cfg_s_t sast_rtd6122c_evb_cfg[] = {
    {0x0000001f, 0x0001390A},
    {0x0000001e, 0x373453BA},
    {0x0000001d, 0xF2733B57},
    {0x0000001c, 0xFFFFA50C},
    {0x0000001b, 0x0C035D82},
    {0x0000001a, 0xA1701FF0},
    {0x00000019, 0x325CB84C},
    {0x00000018, 0x878C16C5},
    {0x00000017, 0xB36DB61B},
    {0x00000016, 0x6DC10080},
    {0x00000015, 0x08045133},
    {0x00000014, 0x3E83A6BD},
    {0x00000013, 0x1C400000},
};

static ts_viu_snr_attr_s_t sst_rtd6122c_attr = {
    .u16_uint_len = 0x197,
    .u16_cfg_num = sizeof(sast_rtd6122c_evb_cfg) / sizeof(ts_viu_snr_cfg_s_t),
    .pst_cfg = (ts_viu_snr_cfg_s_t *)&sast_rtd6122c_evb_cfg,
};


ts_s32 rtd6122_get_temperature(ts_void)
{
    ts_u32 u32_fv;
    ts_s32 s32_ret, s32_temp;
    ts_cpl_mutex_lock(&global_sys_ctrl_info.m_viu_stats_mutex);
    s32_ret = ts_tdk_viu_get_stats(TS_VIU_MDEV0, TS_VIU_STATS_COL, 654, 0, (ts_u32 *)&u32_fv, ts_null);
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


ts_void rtd6122_clkm_init(snr_attr_s_t *pst_attr, ts_u32 u32_snr_clk, ts_u32 u32_btenc_pix_clk_hz)
{
    ts_clkm_cfg_s_t st_clk_cfg;
    ts_clkm_viu_cfg_s_t *pst_viu_clk;
    ts_clkm_vpo_cfg_s_t *pst_vpo_clk;

    pst_viu_clk = (ts_clkm_viu_cfg_s_t *)&st_clk_cfg.st_viu_clk;
    pst_vpo_clk = (ts_clkm_vpo_cfg_s_t *)&st_clk_cfg.st_vpo_clk;

    /* viu 时钟 */
    pst_viu_clk->u32_snr_clk = 76500000;
    pst_viu_clk->u32_clk_hz = 22500000;
    pst_viu_clk->en_clk_src = TS_CLKM_SRC_ISP_CLK;

    pst_viu_clk->u32_snr_internal_clk_en = 1;
    pst_viu_clk->u32_snr_o_clk_phase = 0;
    pst_viu_clk->u32_snr_o_clk_inv = 0;

    pst_viu_clk->u32_sample_clk_en = 1;

    pst_viu_clk->en_sample_clk_src = TS_CLKM_SRC_VIU_SNR_CLK;

    pst_viu_clk->u32_sample_clk_phase = 0;
    pst_viu_clk->u32_sample_clk_inv = 0;

    st_clk_cfg.u32_isp_clk_hz = 20000000;

    /* vpo module */
    pst_vpo_clk->en_vpo_clk_src = TS_CLKM_SRC_VPO_PLL;
    pst_vpo_clk->u32_vpo_clk_hz = 54000000; //需要把vpo的频率提上来，否则，可能cvbs输出出现异常

    /* vpo btenv */
    pst_vpo_clk->u32_btenc_pix_clk_hz = 27000000;
    pst_vpo_clk->u32_btenc_mod_clk_hz = 54000000;
    pst_vpo_clk->u32_btenc_o_clk_inv = 1;

    /* vpo lcd */
    pst_vpo_clk->u32_lcd_pix_clk_hz = 27000000;
    pst_vpo_clk->u32_lcd_mod_clk_hz = 27000000;
    pst_vpo_clk->u32_lcd_o_clk_en = 0;
    pst_vpo_clk->u32_lcd_o_clk_inv = 0;

    if(SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_BT656 || SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_BT601 ){

        vision_log_debug("VIDEO_OUTPUT_MODE_ENUM_BT656 \n");

        if(SystemParaMenu2.m_bt_time_sequece_mode==BT_TIMING_SEQ_MODE_NONE){
            if(SystemParaMenu2.m_raw_yuv_mode == RAW_YUV_MODE_RAW_YUV || SystemParaMenu2.m_raw_yuv_mode == RAW_YUV_MODE_YUV_RAW){
                pst_vpo_clk->u32_vpo_clk_hz = 108000000; 
                pst_vpo_clk->u32_btenc_pix_clk_hz = 54000000; 
                pst_vpo_clk->u32_btenc_mod_clk_hz = 108000000;
            }
            else{
                pst_vpo_clk->u32_btenc_pix_clk_hz = 27000000; 
                pst_vpo_clk->u32_btenc_mod_clk_hz = 27000000;
            }
        }
        else{ 
             if(SystemParaMenu2.m_bt_time_sequece_mode==BT_TIMING_SEQ_MODE_WIN2_50HZ){
                pst_vpo_clk->u32_btenc_pix_clk_hz = 27000000; 
                pst_vpo_clk->u32_btenc_mod_clk_hz = 27000000;
                pst_vpo_clk->u32_lcd_pix_clk_hz = 27000000;
                pst_vpo_clk->u32_lcd_mod_clk_hz = 27000000;
                pst_vpo_clk->u32_vpo_clk_hz = 54000000; 
                vision_log_warn("BT_TIMING_SEQ_MODE_WIN2_50HZ \n");

             }
             else if(SystemParaMenu2.m_bt_time_sequece_mode==BT_TIMING_SEQ_MODE_WIN2_25HZ){
                pst_vpo_clk->u32_vpo_clk_hz = 54000000; 
                pst_vpo_clk->u32_btenc_pix_clk_hz = 13500000; 
                pst_vpo_clk->u32_btenc_mod_clk_hz = 13500000;
                pst_vpo_clk->u32_lcd_pix_clk_hz = 13500000;
                pst_vpo_clk->u32_lcd_mod_clk_hz = 13500000;
                vision_log_warn("BT_TIMING_SEQ_MODE_WIN2_25HZ \n");

             } 
             
        }
    
        pst_vpo_clk->u32_btenc_o_clk_inv = 1;

    }
    
    if(SystemParaMenu2.m_ts_npu_ops_sr_en==NPU_SR_640X512_960X768)//超分使能
    {
        /* isp clock */
        //st_clk_cfg.u32_isp_clk_hz = SNR_CALC_ISP_CLK(180000000, TEST_BAR_WIDTH, TEST_BAR_HEIGHT, TEST_BAR_FPS);
        st_clk_cfg.u32_isp_clk_hz = SNR_CALC_ISP_CLK(180000000, 640, 512, 50);
        st_clk_cfg.u32_npu_clk_hz = 64000000;
         /* vpo module */
        pst_vpo_clk->en_vpo_clk_src = TS_CLKM_SRC_VPO_PLL;
        pst_vpo_clk->u32_vpo_clk_hz = 108000000; //需要把vpo的频率提上来，否则，可能cvbs输出出现异常
        /* vpo btenv clock*/
        //pst_vpo_clk->u32_btenc_pix_clk_hz = VPO_CALC_BT_CLK(180000000, SR_WIDTH, SR_HEIGHT, 50);
        pst_vpo_clk->u32_btenc_pix_clk_hz = VPO_CALC_BT_CLK(180000000, 960, 768, 50);//45000000
        pst_vpo_clk->u32_btenc_mod_clk_hz = pst_vpo_clk->u32_btenc_pix_clk_hz;//45000000
        pst_vpo_clk->u32_btenc_o_clk_inv = 0;
        vision_log_warn("pst_vpo_clk->u32_btenc_pix_clk_hz:%d \n",pst_vpo_clk->u32_btenc_pix_clk_hz);
        /* vpo lcd clock*/
        pst_vpo_clk->u32_lcd_pix_clk_hz = 45000000;//VPO_CALC_BT_CLK(180000000, 960, 768, 50);
        pst_vpo_clk->u32_lcd_mod_clk_hz = 45000000;//pst_vpo_clk->u32_lcd_pix_clk_hz;
        pst_vpo_clk->u32_lcd_o_clk_en = 0;
        pst_vpo_clk->u32_lcd_o_clk_inv = 0;
    }

    /* vpo cvbs */
    pst_vpo_clk->u32_cvbs_clk_en = 1;
    pst_vpo_clk->u32_dac_clk_en = 1;
    pst_vpo_clk->en_dac_clk_src = TS_CLKM_SRC_CVBS_PLL;
    pst_vpo_clk->u32_dac_clk_inv = 1;


    /* vpo mipi */
    pst_vpo_clk->u32_mipi_dhpy_ref_clk_en = 0;

    ts_tdk_clkm_set((const ts_clkm_cfg_s_t *)&st_clk_cfg);
}

// ts_void rtd6122_clkm_init(snr_attr_s_t *pst_attr, ts_u32 u32_snr_clk, ts_u32 u32_btenc_pix_clk_hz)
// {
//     ts_clkm_cfg_s_t st_clk_cfg;
//     ts_clkm_viu_cfg_s_t *pst_viu_clk;
//     ts_clkm_vpo_cfg_s_t *pst_vpo_clk;
//     //ts_u32 u32_pll_isp;

//     pst_viu_clk = (ts_clkm_viu_cfg_s_t *)&st_clk_cfg.st_viu_clk;
//     pst_vpo_clk = (ts_clkm_vpo_cfg_s_t *)&st_clk_cfg.st_vpo_clk;

//     pst_viu_clk->en_clk_src = TS_CLKM_SRC_ISP_CLK;

//     /* viu 时钟 */
//     if (50 == pst_attr->s32_fps) {
//         if (u32_snr_clk > 0)
//             pst_viu_clk->u32_snr_clk = u32_snr_clk;
//         else
//             pst_viu_clk->u32_snr_clk = 76000000;

//         pst_viu_clk->u32_clk_hz = 22500000;
//     } else {
//         /* for 25hz */
//         pst_viu_clk->u32_snr_clk = 50000000;
//         pst_viu_clk->u32_clk_hz = 25000000;
//     }

//     pst_viu_clk->u32_snr_internal_clk_en = 1;
//     pst_viu_clk->u32_snr_o_clk_phase = 0;
//     pst_viu_clk->u32_snr_o_clk_inv = 0;

//     pst_viu_clk->u32_sample_clk_en = 1;
//     pst_viu_clk->en_sample_clk_src = TS_CLKM_SRC_VIU_SNR_CLK;
//     pst_viu_clk->u32_sample_clk_phase = 0;
//     pst_viu_clk->u32_sample_clk_inv = 0;

//     if (50 == pst_attr->s32_fps) {
//         st_clk_cfg.u32_isp_clk_hz = 20000000;
//     } else {
//         st_clk_cfg.u32_isp_clk_hz = 12500000;
//     }

//     #if 0
//     ts_tdk_clkm_get_pll((ts_u32 *)&u32_pll_isp, ts_null, ts_null);
//     st_clk_cfg.u32_isp_clk_hz = SNR_CALC_ISP_CLK(u32_pll_isp, RTD6122_WIDTH, RTD6122_HEIGHT, u32_fps);
//     if (st_clk_cfg.u32_isp_clk_hz > pst_viu_clk->u32_clk_hz)
//         st_clk_cfg.u32_isp_clk_hz = pst_viu_clk->u32_clk_hz;
//     #endif

//     /* vpo module */
//     pst_vpo_clk->en_vpo_clk_src = TS_CLKM_SRC_VPO_PLL;
//     pst_vpo_clk->u32_vpo_clk_hz = 45000000;

//     /* vpo btenv */
//     if (u32_btenc_pix_clk_hz > 0)
//         pst_vpo_clk->u32_btenc_pix_clk_hz = u32_btenc_pix_clk_hz;
//     else
//         pst_vpo_clk->u32_btenc_pix_clk_hz = 27000000;

//     pst_vpo_clk->u32_btenc_mod_clk_hz = 54000000;
//     pst_vpo_clk->u32_btenc_o_clk_inv = 1;

//     /* vpo cvbs */
//     pst_vpo_clk->u32_cvbs_clk_en = 1;
//     pst_vpo_clk->u32_dac_clk_en = 1;
//     pst_vpo_clk->en_dac_clk_src = TS_CLKM_SRC_CVBS_PLL;
//     pst_vpo_clk->u32_dac_clk_inv = 1;

//     /* vpo lcd */
//     pst_vpo_clk->u32_lcd_pix_clk_hz = 45000000;
//     pst_vpo_clk->u32_lcd_mod_clk_hz = 45000000;
//     pst_vpo_clk->u32_lcd_o_clk_en = 0;

//     /* vpo mipi */
//     pst_vpo_clk->u32_mipi_dhpy_ref_clk_en = 0;

//     ts_tdk_clkm_set((const ts_clkm_cfg_s_t *)&st_clk_cfg);

// }

ts_void rtd6122_viu_init(snr_attr_s_t *pst_attr, ts_viu_snr_attr_s_t *pst_snr_attr, \
                            ts_viu_snr_timing_attr_s_t *pst_tim_attr, snr_remap_s_t *pst_snr_remap)
{
    ts_viu_comm_attr_s_t st_comm_attr;
    ts_viu_corr_attr_s_t *pst_corr;

    ts_cpl_memset32((ts_u32 *)&st_comm_attr, 0, (sizeof(ts_viu_comm_attr_s_t) >> 2));

    st_comm_attr.en_snr = TS_VIU_SNRT_RTD6122;

    if (ts_null == pst_tim_attr)
        st_comm_attr.st_timing_attr.u08_redefine_timing_en = 0;
    else
        ts_cpl_memcpy(&st_comm_attr.st_timing_attr, pst_tim_attr, sizeof(ts_viu_snr_timing_attr_s_t));

    st_comm_attr.u16_height = RTD6122_HEIGHT;
    st_comm_attr.u16_width = RTD6122_WIDTH;
    st_comm_attr.u08_pixel_ratio = 2;
    st_comm_attr.f32_fps = 50; //(ts_f32)pst_attr->s32_fps;
    st_comm_attr.u16_rst_dly_us = 10000;

    if ((pst_attr->st_cvbs.u08_en > 0) && (pst_attr->st_cvbs.u08_sd_en > 0)) {
        st_comm_attr.u08_ex_vsync_en = 1;
        st_comm_attr.u08_ex_vsync_mode = TS_VIU_EX_VSYNC_CVBS;
    } else {
        st_comm_attr.u08_ex_vsync_en = 0;
    }

    /* 线序 */
    if (ts_null == pst_snr_remap) {
        st_comm_attr.u32_i_order_lo = 0x76543210;
        st_comm_attr.u32_i_order_hi = 0xfedcba98;
    
        st_comm_attr.u32_o_order_lo = 0x76543210;
        st_comm_attr.u32_o_order_hi = 0xfedcba98;
    } else {
        st_comm_attr.u32_i_order_lo = pst_snr_remap->u32_i_order_lo;
        st_comm_attr.u32_i_order_hi = pst_snr_remap->u32_i_order_hi;
        st_comm_attr.u32_o_order_lo = pst_snr_remap->u32_o_order_lo;
        st_comm_attr.u32_o_order_hi = pst_snr_remap->u32_o_order_hi;
    }


    pst_corr = (ts_viu_corr_attr_s_t *)&st_comm_attr.st_corr_attr;
    pst_corr->u08_alg = TS_VIU_CORR_ALG_SA;
    pst_corr->u08_reset_val = 0x20  << 2;
    pst_corr->u08_max = 0xfc;
    pst_corr->u08_min = 0;

    /* 单次触发 */
    pst_corr->u08_freq = 0;

    /* 快调 */
    pst_corr->st_fast.u16_thr_hi = 0x2400;
    pst_corr->st_fast.u16_thr_lo = 0x1c00;
    pst_corr->st_fast.u08_mode = TS_VIU_ADJ_MODE_BACKWARD;
    pst_corr->st_fast.u08_step = 16;
    pst_corr->st_fast.u08_mask = 0xff;
    pst_corr->st_fast.u08_frame_cnt = 0;

    /* 细调 */
    pst_corr->st_trim.u16_thr_hi = 0x2180;
    pst_corr->st_trim.u16_thr_lo = 0x1e00;
    pst_corr->st_trim.u08_mode = TS_VIU_ADJ_MODE_BACKWARD;
    pst_corr->st_trim.u08_step = 4;
    pst_corr->st_trim.u08_mask = 0xfc;
    pst_corr->st_trim.u08_frame_cnt = 0x20;

    ts_tdk_viu_set_comm_attr(TS_VIU_MDEV0, (const ts_viu_comm_attr_s_t *)&st_comm_attr);
    ts_tdk_viu_set_snr_attr(TS_VIU_MDEV0, (const ts_viu_snr_attr_s_t *)pst_snr_attr);
}


//static 
ts_void rtd6122_cut(ts_s32 *ps32_temp)
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


// ts_void rtd6122_auto_cut_process(ts_void)
// {
//     // ts_s32  ret = TS_OK;
//     ts_cpl_timeval_s_t st_time_start, st_time_cur, st_time_last_cut;   
//     ts_cpl_gettimeofday(&st_time_start);

//     ts_u32 u32_cut_status = 0;
//     // ts_s32 s32_temp_last;
//     ts_u32 u32_shutter_mean_last = 0xffffffff;
//     preStartForceShiftFlag = 1;
//     ts_s32 s32_cut_time_gap_non_static = SystemParaMenu2.m_st1_s * 1000; //15秒
//     ts_s32 s32_cut_time_gap_static = 60 * 1000; //60秒
//     ts_s32 s32_static_time = SystemParaMenu2.m_st1_m * 60 * 1000;  //开机的第一分钟，按照s32_cut_time_gap_non_static间隔打档
//     ts_viu_array_info_s_t st_viu_array_info;
//     if(SystemParaMenu2.m_st1_m == 0) {
//         preStartForceShiftFlag = 0;
//     }

//     gst_snr_ctx.s32_run_auto_cut = 0;
//     vision_log_debug("gst_snr_ctx.s32_run_auto_cut %d\n",gst_snr_ctx.s32_run_auto_cut);
//     ts_s32 s32_temp_inital = sns_get_temperature();
//     ts_u32 u32_wait_sensor_valid_cnt = 0;
//     while (1) {
//         ts_s32 s32_temp_cur = sns_get_temperature();
//         if (s32_temp_cur != s32_temp_inital) {
//             u32_wait_sensor_valid_cnt ++;
//             s32_temp_inital = s32_temp_cur;
//         }

//         if (u32_wait_sensor_valid_cnt > 10) {
//             ts_tdk_viu_get_mean(TS_VIU_MDEV0, &st_viu_array_info);
//             u32_shutter_mean_last = st_viu_array_info.u32_mean;
//             vision_log_debug("Power on mean is %d",u32_shutter_mean_last);
//             break;
//         }

//         ts_cpl_msleep(100);
//     }
    
//     ts_cpl_gettimeofday(&st_time_last_cut);
//     ts_u32 temp_cnt=0;
//     // ts_u16 last_temp_ad_value=0;
//     ts_f32 last_det_temp_value = 0.0;
//     // ts_u16 diff_ad_value;
//     ts_u16 diff_temp_value=0;
//     vision_log_debug("gst_snr_ctx.s32_run_auto_cut %d \n",gst_snr_ctx.s32_run_auto_cut);

//     while(1) {

//         //开启阳光保护中，强制打档无效
//         // if(ISP_get_sun_pt_force_flag()>0){
//         //     ts_cpl_fix_msleep(1000);
//         //     // vision_log_debug("ISP_get_sun_pt_force_flag %d \n",ISP_get_sun_pt_force_flag());
//         //     continue;
//         // }

//         if (preStartForceShiftFlag) {
//             ts_cpl_gettimeofday(&st_time_cur);
//             ts_s32 s32_time_diff;
//             s32_time_diff = ((st_time_cur.tv_sec - st_time_start.tv_sec) * 1000000 + st_time_cur.tv_usec - st_time_start.tv_usec) / 1000;
//             if (s32_time_diff >= s32_static_time) 
//             {
//                 //打开自动打档全局开关
//                 //gst_snr_ctx.s32_run_auto_cut = 1;
//                 preStartForceShiftFlag = 0;
//                 continue;
//             }

//             s32_time_diff = ((st_time_cur.tv_sec - st_time_last_cut.tv_sec) * 1000000 + st_time_cur.tv_usec - st_time_last_cut.tv_usec) / 1000;
//             if (s32_time_diff >= s32_cut_time_gap_non_static) 
//             {
//                 ISP_cut_new();
//                 ts_cpl_gettimeofday(&st_time_last_cut); // 获取最新时间
//             }
            
//             ts_cpl_msleep(100);
//             continue;
//         }

//         if (0 == gst_snr_ctx.s32_run_auto_cut) {
//             ts_cpl_msleep(100);
//             ts_cpl_gettimeofday(&st_time_last_cut);  
//             continue;
//         }
        
//         //矫正的时候不进行自动打档
//         if(ISP_get_det_calibrationing_flag()>0){
//             // vision_log_debug(" det calibrationing ! stop update statics \n");
//             ts_cpl_fix_msleep(500);
//             // ts_cpl_gettimeofday(&st_time_last_cut);  
//             continue;
//         }

//         //det参数切换的时候不进行打档
//         // if(ISP_get_det_switch_force_flag()>0){
//         //     vision_log_debug(" ISP_get_det_switch_force_flag  ! stop update statics \n");
//         //     ts_cpl_fix_msleep(200);
//         //     // ts_cpl_gettimeofday(&st_time_last_cut);  
//         //     continue;
//         // }
        

//         //更新统计信息
//         {
//             global_sys_ctrl_info.isp_auto_cut_run_cnt_u64++;
//             ts_cpl_gettimeofday(&st_time_cur);
//             ts_s32 s32_time_diff = ((st_time_cur.tv_sec - st_time_start.tv_sec) * 1000000 + st_time_cur.tv_usec - st_time_start.tv_usec) / 1000000;
//             global_sys_ctrl_info.isp_cut_time_sec_u32 = s32_time_diff;
//             global_sys_ctrl_info.isp_temp_ad_value_u16 = ISP_get_det_ad_value();
//         }
        

//         if(ISP_get_ShutterMode() != SHUTTER_MODE_OFF){
//             //获取实际的打档间隔
//             s32_cut_time_gap_static  = ISP_get_shutter_timer_gap();
// //            if (preStartForceShiftFlag) {                
// //                ts_cpl_gettimeofday(&st_time_cur);
// //                ts_s32 s32_time_diff = ((st_time_cur.tv_sec - st_time_last_cut.tv_sec) * 1000000 + st_time_cur.tv_usec - st_time_last_cut.tv_usec) / 1000;
// //                //vision_log_debug("power st s32_time_diff = %d\n", s32_time_diff);
// //                if (s32_time_diff >= s32_cut_time_gap_non_static) u32_cut_status |= 0x1;
// //                s32_time_diff = ((st_time_cur.tv_sec - st_time_start.tv_sec) * 1000000 + st_time_cur.tv_usec - st_time_start.tv_usec) / 1000;
// //                if (s32_time_diff >= s32_static_time) preStartForceShiftFlag = 0;
// //            }else 
// 	    {
//                 ts_cpl_gettimeofday(&st_time_cur);
//                 ts_s32 s32_time_diff = ((st_time_cur.tv_sec - st_time_last_cut.tv_sec) * 1000000 + st_time_cur.tv_usec - st_time_last_cut.tv_usec) / 1000;
//                 if (s32_time_diff >= s32_cut_time_gap_static)
//                 u32_cut_status |= 0x2;
//             }

//             temp_cnt++;
//             if(temp_cnt>10)
//             {
//                 temp_cnt = 0;
//                 if(SystemParaMenu2.m_det_st != 0) {
//                     vot_read_det_ad();
//                     // ts_u16 ad_value = ISP_get_det_ad_value();
//                     // diff_ad_value  = abs( ad_value - last_temp_ad_value);
//                     diff_temp_value = (global_sys_ctrl_info.m_det_temp_value - last_det_temp_value) * 10;
                    
//                     if(abs(diff_temp_value) >= SystemParaMenu2.m_det_st) {
//                         //vision_log_debug("diff_temp_value = %d\n", diff_temp_value);
//                         //vision_log_debug("SystemParaMenu2.m_det_st = %d\n", SystemParaMenu2.m_det_st);
//                         u32_cut_status |= 0x4; 
//                         last_det_temp_value = global_sys_ctrl_info.m_det_temp_value;
//                     }                

//                     // if(diff_ad_value>16){
//                     //     u32_cut_status |= 0x4; 
//                     //     last_temp_ad_value = ad_value;
//                     // }    
//                 }            
//             }
 
//             if (gst_snr_ctx.s32_force_cut) {
//                 u32_cut_status |= 0x8;
//                 gst_snr_ctx.s32_force_cut = 0;
//             }

//             if ((u32_cut_status != 0) && gst_snr_ctx.s32_run_auto_cut) 
//             {
                  
//                 if(ISP_get_ShutterMode() != SHUTTER_MODE_OFF){
//                     vision_log_debug("to cut, status = 0x%x, diff_temp_value = %d \n", u32_cut_status, diff_temp_value);
//                     ISP_cut_new();
                    
//                     // ts_s32 s32_temp;
//                     // ts_u32 u32_mean;
//                     // snr_planck640_cut(u32_cut_status, &s32_temp, &u32_mean);
//                     //更新det 温度值，避免短间隔打档
//                     // last_temp_ad_value = vot_read_det_ad();
//                     last_det_temp_value = global_sys_ctrl_info.m_det_temp_value;
//                     temp_cnt = 0;
//                     u32_cut_status = 0;
//                 }

//                 ts_cpl_gettimeofday(&st_time_last_cut);     
//             }
//         }

//         ts_cpl_fix_msleep(100);  
//     }
// }


/* 启动例子 */
static ts_void start_rtd6122(ts_u32 u32_param)
{
    snr_attr_s_t *pst_attr;
    snr_ctx_s_t *pst_ctx;
    ts_u32 u32_npu_en=0;

    pst_ctx = (snr_ctx_s_t *)u32_param;
    pst_attr = (snr_attr_s_t *)&pst_ctx->st_attr;
    pst_attr->get_temperature = rtd6122_get_temperature;
    pst_attr->s32_img_w = 640;
    pst_attr->s32_img_h = 512;
    pst_attr->s32_fps = ISP_get_output_fps();
    pst_attr->s32_vo_img_w = (ts_s32)640;
    pst_attr->s32_vo_img_h = (ts_s32)512;

    /* 是否开启开机画面 */
    pst_attr->u32_pod_dly_ms = 1000;

    /* npu 设置 */
    pst_attr->st_npu.en_ops = TS_NPU_OPS_DISABLE; //TS_NPU_OPS_MEM;
    pst_attr->st_npu.u32_clk = 0;
    u32_npu_en = 0;
    /* cvbs 通道配置 */
    pst_attr->st_cvbs.u08_en = 1;
    pst_attr->st_cvbs.u08_sd_en = 1;
    pst_attr->st_cvbs.u08_orgi_en = 0;
    pst_attr->st_cvbs.u08_pal_en = 1;

    if(SystemParaMenu2.m_cvbs_display_mode==DEVICE_CVBS_DISPLAY_MODE_ENLARGE){
        pst_attr->st_cvbs.u08_orgi_en = 0;
    }
    else{
        pst_attr->st_cvbs.u08_orgi_en = 1;
    }
    /* mem 通道配置 */
    pst_attr->u32_mem_en = 1;

    /* 是否开启 raw 同传 */
    pst_attr->u08_raw_oe = 0;
    pst_attr->u08_raw_pos = 0;

    pst_attr->u08_y_oe = 1;



    pst_attr->st_bte.u08_en = 0;
    pst_attr->st_mipi.u08_en = 0;
    pst_attr->st_mipi.u08_dsi_en = 0;
    pst_attr->st_bte.u08_redefine_timing_en = 0;
    /* 是否开启 raw 同传 raw+yuv */
    if(SystemParaMenu2.m_raw_yuv_mode == RAW_YUV_MODE_RAW_YUV || SystemParaMenu2.m_raw_yuv_mode == RAW_YUV_MODE_YUV_RAW){
        pst_attr->u08_raw_oe = 1;
        pst_attr->u08_raw_pos = SystemParaMenu2.m_raw_node_pos; 
        pst_attr->u08_y_oe = 1;

        if(SystemParaMenu2.m_raw_yuv_mode == RAW_YUV_MODE_YUV_RAW){
            pst_attr->u08_raw_seq = 1;
        }
        else{
            pst_attr->u08_raw_seq = 0;
        }
        vision_log_debug("m_raw_node_pos %d  \n",SystemParaMenu2.m_raw_node_pos);
        vision_log_debug(" SystemParaMenu2.m_raw_yuv_mode  %d  \n",SystemParaMenu2.m_raw_yuv_mode);
        //关闭坏斑功能，此功能和raw+yuv冲突
        pst_attr->st_npu.en_ops = TS_NPU_OPS_DISABLE;
    }


 
    if(SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_BT656 || SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_BT601 ){

        // gpio cfg for 

        bt656_bt1120_pin_init();

        //配置bt656//bt1120
        pst_attr->st_bte.u08_en = 1;
        pst_attr->st_bte.u08_8bit_en = 1;
        if(SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_BT601 ){
            pst_attr->st_bte.u08_external_sync_en = 1;
            vision_log_debug("VIDEO_OUTPUT_MODE_ENUM_BT601  \n");
        }
        else{
            pst_attr->st_bte.u08_external_sync_en = 0;
            vision_log_debug("VIDEO_OUTPUT_MODE_ENUM_BT656  \n");
        }
        pst_attr->u32_mem_en = 0;


        if(SystemParaMenu2.m_bt_time_sequece_mode==BT_TIMING_SEQ_MODE_WIN2_50HZ){
            pst_attr->st_bte.u08_redefine_timing_en = 1; 
            pst_attr->st_bte.u16_vsync_bp = 20;
            pst_attr->st_bte.u16_vsync_per = 4;
            pst_attr->st_bte.u16_vsync_fp = 16;

            pst_attr->st_bte.u16_hsync_bp = 111;
            pst_attr->st_bte.u16_hsync_per = 32;
            pst_attr->st_bte.u16_hsync_fp = 32;

            pst_attr->st_bte.u16_output_width = 640;
            pst_attr->st_bte.u16_output_height = 512;
        }
        else  if(SystemParaMenu2.m_bt_time_sequece_mode==BT_TIMING_SEQ_MODE_WIN2_25HZ){
            pst_attr->st_bte.u08_redefine_timing_en = 1; 
            pst_attr->st_bte.u16_vsync_bp = 20;
            pst_attr->st_bte.u16_vsync_per = 4;
            pst_attr->st_bte.u16_vsync_fp = 16;

            pst_attr->st_bte.u16_hsync_bp = 101;
            pst_attr->st_bte.u16_hsync_per = 32;
            pst_attr->st_bte.u16_hsync_fp = 32;

            pst_attr->st_bte.u16_output_width = 640;
            pst_attr->st_bte.u16_output_height = 512;
        }


    }
    else if(SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_MIPI){
    // #ifdef VOT_MIPI_EN
            // pst_attr->st_cvbs.u08_en = 1;
            // pst_attr->st_cvbs.u08_sd_en = 1;
            // pst_attr->st_cvbs.u08_orgi_en = 0;
            // pst_attr->st_cvbs.u08_pal_en = 1;

            pst_attr->st_mipi.u08_en = 1;
            pst_attr->st_mipi.u08_dsi_en = 0;
            pst_attr->st_mipi.u08_data_type = TS_VPO_CSI2_DT_YUV422;

 
            if(SystemParaMenu2.m_mipi_lane_mode  == MIPI_LANE_MODE_4_LANES){
                pst_attr->st_mipi.u08_dphy_lane_num = 4;
                pst_attr->st_mipi.u16_data_rate = 200;
            }
            else{
                pst_attr->st_mipi.u08_dphy_lane_num = 2;
                pst_attr->st_mipi.u16_data_rate = 400;
            }
            vision_log_warn("SystemParaMenu2.m_mipi_lane_mode  %d  \n",SystemParaMenu2.m_mipi_lane_mode );
            
    // #if defined(VOT_MIPI_EN) 
            // pst_attr->st_mipi.u16_data_rate = 100;
    // #endif
            pst_attr->st_mipi.u08_clock_continue = 0;

            pst_attr->st_bte.u08_en = 0;
            pst_attr->u32_mem_en = 0;
    // #endif

    }

    // ts_cpl_insmod(clkm, "pllsys=100000000 pllddr=250000000 pllviu=153000000 pllisp=180000000 pllvpo=216000000 pllsafe=0");
    // ts_cpl_insmod(clkm, "pllsys=100000000 pllddr=250000000 pllviu=153000000 pllisp=180000000 pllvpo=216000000 ");

    ts_cpl_insmod(clkm, "pllsys=100000000 pllddr=250000000 pllviu=153000000 pllisp=180000000 pllvpo=216000000");

    rtd6122_clkm_init(pst_attr, 0, 0);

    snr_insmod_all_tdk(u32_npu_en);

    // brd_init(TS_VIU_SNRT_RTD6122);

    snr_init_smart(sast_rtd6122c_evb_cfg, sizeof(sast_rtd6122c_evb_cfg));
    rtd6122_viu_init(pst_attr, (ts_viu_snr_attr_s_t *)&sst_rtd6122c_attr, ts_null, ts_null);

    snr_cfg_vpo(pst_attr);

    /*配置ISP参数*/
    ts_void *pv_isp_cfg_to = snr_get_isp_cfg(TDK_ISP_DEFCFG_512P);
    snr_isp_init(pst_attr->s32_img_w, pst_attr->s32_img_h, pst_attr->s32_fps, pv_isp_cfg_to);

    /*使能输入*/
    ts_tdk_viu_enable(TS_VIU_MDEV0);
    vot_snr_init_detector(0,DET_TEMP_MODE_NORMAL);
    ts_cpl_fix_msleep(1000);  

    tdk_snrcfg_shutter_close();
	
	snr_wait_ready(6000);

    ts_viu_corr_rst_s_t st_rst_attr;
    st_rst_attr.u08_reset_only = 0;
    st_rst_attr.u08_reset_val = 0x80;
    ts_tdk_viu_exec_correct(TS_VIU_MDEV0, &st_rst_attr);

    tdk_snrcfg_shutter_open();

    /*OCC 初始化*/
    snr_init_ooc();

    /*K 初始化*/
    ts_isp_nuc_attr_s_t st_nuc_attr;
    st_nuc_attr.s32_en = TS_ENABLE;
    st_nuc_attr.s32_cutoff = TS_DISABLE;
    st_nuc_attr.en_mode = TS_ISP_NUC_MODE_DELTAB; 
    st_nuc_attr.en_stat_mode = 2;  
    st_nuc_attr.u08_stat_frm_num = 16;//12;//48; //////////
    st_nuc_attr.u16_mean_val = 0;
    st_nuc_attr.u16_coef = 0;
    st_nuc_attr.u08_stat_fack_frm2_en = 0;
    st_nuc_attr.u16_frm_oft = 0;
    st_nuc_attr.u08_bbuf_dbl = 0;
    st_nuc_attr.u08_dbuf_dbl = 0;
    snr_init_nuc((ts_isp_nuc_attr_s_t *)&st_nuc_attr);

    ts_isp_sffc_attr_s_t sffc_attr;
    sffc_attr.u08_enable = 0;
    sffc_attr.u08_en_man = 0;
    sffc_attr.u08_upd_speed = 32;
    sffc_attr.u16_flt_thr = 2560;
    sffc_attr.u16_man_cor_v = 3000;
    snr_init_sffc((ts_isp_sffc_attr_s_t *)&sffc_attr);

    // if(SystemParaMenu2.Bpc_ext_SW>0){
    //     //ts_tdk_extbpc_init(pst_attr->s32_img_w, pst_attr->s32_img_h, 0x00160234, 3, 1);
    //     //ts_tdk_extbpc_init(pst_attr->s32_img_w, pst_attr->s32_img_h, 0x00160234, 0, 1);
    //     ts_tdk_extbpc_init(gpv_tmm_sys, 1000, 16, 1150);
    // }


    // ts_cpl_kthread_create(ts_null, (ts_cpl_kthread_t)snr_fix_snc_proc, ts_null, "fix_snc", 27, 4096);
}

snr_register(rtd6122c, start_rtd6122);

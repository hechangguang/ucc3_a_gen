/*
 * planck640c.c -- vision planck640c
 */
#include <base/ts_type.h>
#include <base/ts_compiler.h>
#include <cpl/ts_cpl.h>
#include <uapi/chip_def.h>
#include <uapi/isp_v100.h>
#include <uapi/viu_v100.h>
#include <uapi/clkm_v100.h>
#include <uapi/vpo_v100.h>
#include <uapi/gpio_v100.h>
#include <uapi/sfc_v100.h>
#include <stdio.h>
#include "api.h"
#include "snr_comm.h"
#include "menu/votmenu.h"
#include "menu/ispfunction.h"
#include <planck640c_vot.h>

extern SysCtrlInfo_s_t global_sys_ctrl_info;

// extern ts_u08 preStartForceShiftFlag;

#define PLANCK640_TEST_COL_GOAL (16383 - 256)
#define SDL_PLANCK640 408
#define SDL_PLANCK640_M1 (SDL_PLANCK640 - 1)

extern ts_u32 det_gos_switch_force_last;
//#define SUPER_RESOLUTION_EN         (1)//SystemParaMenu2.m_ts_npu_ops_sr_en
#define SR_WIDTH        (960)
#define SR_HEIGHT       (768)
#define SR_FPS          (50)

ts_s32 planck640c_get_temperature(ts_void)
{
    ts_u32 u32_fv,u32_mean;
    ts_s32 s32_ret ,s32_temp;

    ts_cpl_mutex_lock(&global_sys_ctrl_info.m_viu_stats_mutex);

    s32_ret = ts_tdk_viu_get_stats(TS_VIU_MDEV0, TS_VIU_STATS_COL, 0x28e, 200, (ts_u32 *)&u32_fv, &u32_mean);
    u32_mean = u32_mean & 0x3fff;
    if (TS_OK == s32_ret) {
        ts_s32 s32_fv = u32_fv;
        ts_f32 f32_fv = s32_fv;
        f32_fv = (7416.0f-u32_mean)/ (33.0f);
        s32_temp = 1000.0 * f32_fv;
      
    } else {
        s32_temp = 0;
    }

    ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_viu_stats_mutex);
    return s32_temp;
}


ts_void planck640c_clkm_init(ts_void)
{
    ts_clkm_cfg_s_t st_clk_cfg;
    ts_clkm_viu_cfg_s_t *pst_viu_clk;
    ts_clkm_vpo_cfg_s_t *pst_vpo_clk;
    ts_u08 m_device_fps = 50;

    pst_viu_clk = (ts_clkm_viu_cfg_s_t *)&st_clk_cfg.st_viu_clk;
    pst_vpo_clk = (ts_clkm_vpo_cfg_s_t *)&st_clk_cfg.st_vpo_clk;

    /* viu 时钟 */
    pst_viu_clk->u32_snr_clk = 76500000;// pllviu/2=153000000/2
    pst_viu_clk->u32_clk_hz = 22500000;
    pst_viu_clk->en_clk_src = TS_CLKM_SRC_ISP_CLK;

    if(SystemParaMenu2.m_raw_yuv_mode != RAW_YUV_MODE_NONE && (SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_MIPI || SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_BT601 || SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_BT656)){
        pst_viu_clk->u32_snr_clk = 78000000;// pllviu/2=156000000/2
    }

    pst_viu_clk->u32_snr_internal_clk_en = 1;
    pst_viu_clk->u32_snr_o_clk_phase = 0;
    pst_viu_clk->u32_snr_o_clk_inv = 0;
    pst_viu_clk->u32_sample_clk_en = 1;
    pst_viu_clk->en_sample_clk_src = TS_CLKM_SRC_VIU_SNR_CLK;
    pst_viu_clk->u32_sample_clk_phase = 0;
    pst_viu_clk->u32_sample_clk_inv = 0;

    st_clk_cfg.u32_isp_clk_hz = 20000000;//SNR_CALC_ISP_CLK(180000000, TEST_BAR_WIDTH, TEST_BAR_HEIGHT, TEST_BAR_FPS);
    /* vpo module */
    pst_vpo_clk->en_vpo_clk_src = TS_CLKM_SRC_VPO_PLL;
    pst_vpo_clk->u32_vpo_clk_hz = 108000000; //216000000/2 需要把vpo的频率提上来，否则，可能cvbs输出出现异常

    /* vpo btenv */
    pst_vpo_clk->u32_btenc_pix_clk_hz = 54000000;
    pst_vpo_clk->u32_btenc_mod_clk_hz = 54000000;
    pst_vpo_clk->u32_btenc_o_clk_inv = 1;
    /* vpo lcd */
    pst_vpo_clk->u32_lcd_pix_clk_hz = 54000000;
    pst_vpo_clk->u32_lcd_mod_clk_hz = 54000000;
    pst_vpo_clk->u32_lcd_o_clk_en = 0;
    pst_vpo_clk->u32_lcd_o_clk_inv = 0;

 
    if(SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_BT656 || SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_BT601 ){

        vision_log_debug("VIDEO_OUTPUT_MODE_ENUM_BT656 \n");

        if(SystemParaMenu2.m_bt_time_sequece_mode==BT_TIMING_SEQ_MODE_NONE){
            if(SystemParaMenu2.m_raw_yuv_mode == RAW_YUV_MODE_RAW_YUV || SystemParaMenu2.m_raw_yuv_mode == RAW_YUV_MODE_YUV_RAW){
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
             else if(SystemParaMenu2.m_bt_time_sequece_mode==BT_TIMING_SEQ_MODE_WIN2_25HZ || SystemParaMenu2.m_bt_time_sequece_mode==BT_TIMING_SEQ_MODE_WIN2_30HZ){
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
    
    if(SystemParaMenu2.m_ts_npu_ops_sr_en==NPU_SR_640X512_960X768)//超分使能 640x512@50-->960x768@50超分
    {
       
         if(SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_MIPI || SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_BT601 || SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_BT656)
        {
            /* 加载时钟模块 */
            pst_viu_clk->u32_snr_clk = 78000000;  // pllviu/2
            pst_viu_clk->u32_clk_hz = 22500000;
        }
        else{
            pst_viu_clk->u32_snr_clk = 76500000;  // pllviu/2
            pst_viu_clk->u32_clk_hz = 22500000;
        }
        pst_viu_clk->en_clk_src = TS_CLKM_SRC_ISP_CLK;
        // #define SR_WIDTH        (960)
        // #define SR_HEIGHT       (768)
        // #define SR_FPS          (50)
        /* isp clock */
        //st_clk_cfg.u32_isp_clk_hz = SNR_CALC_ISP_CLK(180000000, TEST_BAR_WIDTH, TEST_BAR_HEIGHT, TEST_BAR_FPS);
        st_clk_cfg.u32_isp_clk_hz = SNR_CALC_ISP_CLK(180000000, 640, 512, m_device_fps);//pllisp=180000000
        st_clk_cfg.u32_npu_clk_hz =180000000;
         /* vpo module */
        pst_vpo_clk->en_vpo_clk_src = TS_CLKM_SRC_VPO_PLL;
        pst_vpo_clk->u32_vpo_clk_hz =90000000;//VPO_CALC_BT_CLK(360000000, SR_WIDTH, SR_HEIGHT, m_device_fps);//需要把vpo的频率提上来，否则，可能cvbs输出出现异常
        /* vpo btenv clock*/
        pst_vpo_clk->u32_btenc_pix_clk_hz = VPO_CALC_BT_CLK(180000000, SR_WIDTH, SR_HEIGHT, m_device_fps);//pllvpo=180000000
        pst_vpo_clk->u32_btenc_mod_clk_hz = pst_vpo_clk->u32_btenc_pix_clk_hz;//45000000
        pst_vpo_clk->u32_btenc_o_clk_inv = 1;
        vision_log_warn("pst_vpo_clk->u32_btenc_pix_clk_hz:%d \n",pst_vpo_clk->u32_btenc_pix_clk_hz);
        /* vpo lcd clock*/
        pst_vpo_clk->u32_lcd_pix_clk_hz = VPO_CALC_BT_CLK(180000000,SR_WIDTH, SR_HEIGHT, m_device_fps);//45000000;
        pst_vpo_clk->u32_lcd_mod_clk_hz = pst_vpo_clk->u32_lcd_pix_clk_hz;
        pst_vpo_clk->u32_lcd_o_clk_en = 0;
        pst_vpo_clk->u32_lcd_o_clk_inv = 0;
    }
    else  if(SystemParaMenu2.m_ts_npu_ops_sr_en==NPU_SR_640X512_1280X1024)//超分使能 640x512@50-->1280x1024@50超分
    {
        if(SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_MIPI || SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_BT601 || SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_BT656)
        {
            /* 加载时钟模块 */
            pst_viu_clk->u32_snr_clk = 78000000;  // pllviu/2
            pst_viu_clk->u32_clk_hz = 22500000;
        }
        else{
            pst_viu_clk->u32_snr_clk = 76500000;  // pllviu/2
            pst_viu_clk->u32_clk_hz = 22500000;
        }
        pst_viu_clk->en_clk_src = TS_CLKM_SRC_ISP_CLK;
        /* isp clock */
        //st_clk_cfg.u32_isp_clk_hz = SNR_CALC_ISP_CLK(180000000, TEST_BAR_WIDTH, TEST_BAR_HEIGHT, TEST_BAR_FPS);//pllisp=180000000
        st_clk_cfg.u32_isp_clk_hz = SNR_CALC_ISP_CLK(360000000, 640, 512, m_device_fps);//pllisp=360000000
        st_clk_cfg.u32_npu_clk_hz =180000000;
         /* vpo module */
        pst_vpo_clk->en_vpo_clk_src = TS_CLKM_SRC_VPO_PLL;
        pst_vpo_clk->u32_vpo_clk_hz =90000000;//VPO_CALC_BT_CLK(180000000, 1280, 1024, m_device_fps);//需要把vpo的频率提上来，否则，可能cvbs输出出现异常
        /* vpo btenv clock*/
        pst_vpo_clk->u32_btenc_pix_clk_hz = VPO_CALC_BT_CLK(360000000, 1280, 1024, m_device_fps);//pllvpo=360000000
        pst_vpo_clk->u32_btenc_mod_clk_hz = pst_vpo_clk->u32_btenc_pix_clk_hz;//90000000
        pst_vpo_clk->u32_btenc_o_clk_inv = 1;
        vision_log_warn("pst_vpo_clk->u32_btenc_pix_clk_hz:%d \n",pst_vpo_clk->u32_btenc_pix_clk_hz);
        /* vpo lcd clock*/
        pst_vpo_clk->u32_lcd_pix_clk_hz = VPO_CALC_BT_CLK(360000000,1280, 1024, m_device_fps);//90000000;
        pst_vpo_clk->u32_lcd_mod_clk_hz = pst_vpo_clk->u32_lcd_pix_clk_hz;
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


ts_void planck640c_viu_init(snr_attr_s_t *pst_attr, ts_viu_snr_attr_s_t *pst_snr_attr)
{
    ts_viu_comm_attr_s_t st_comm_attr;
    ts_viu_corr_attr_s_t *pst_corr;
    ts_viu_snr_timing_attr_s_t *pst_timing_attr;

    pst_timing_attr = (ts_viu_snr_timing_attr_s_t *)&st_comm_attr.st_timing_attr;

    pst_timing_attr->u08_redefine_timing_en = 0;
    pst_timing_attr->u08_img_inv_en = 0;

    pst_timing_attr->u16_raw_h =  546;
    pst_timing_attr->u16_raw_w = 800;
    pst_timing_attr->u16_extra_timing = 1200;

    if(SystemParaMenu2.m_raw_yuv_mode != RAW_YUV_MODE_NONE && (SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_MIPI || SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_BT601 || SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_BT656))
    {
        pst_timing_attr->u16_raw_h = 562;
        pst_timing_attr->u08_redefine_timing_en = 1;
    }

    pst_timing_attr->u16_vsync_start = 2;
    pst_timing_attr->u16_hsync_start = 0;
    pst_timing_attr->u16_vld_h = 522;
    pst_timing_attr->u16_vld_w = 656;
    pst_timing_attr->u16_cut_start_v = 5;
    pst_timing_attr->u16_cut_start_h = 10;
    pst_timing_attr->u32_data_dly = 7426;

    st_comm_attr.en_snr = TS_VIU_SNRT_PLANCK640C;
    st_comm_attr.u16_height = 512;
    st_comm_attr.u16_width = 640;
    st_comm_attr.u08_pixel_ratio = 2;
    st_comm_attr.f32_fps = 50;
    st_comm_attr.u16_rst_dly_us = 10000;
  
    if ((pst_attr->st_cvbs.u08_en > 0) && (pst_attr->st_cvbs.u08_sd_en > 0)) {
        st_comm_attr.u08_ex_vsync_en = 1;
        st_comm_attr.u08_ex_vsync_mode = TS_VIU_EX_VSYNC_CVBS;
        pst_timing_attr->u16_extra_timing = 1;
    } else {
        st_comm_attr.u08_ex_vsync_en = 0;
    }
    if (get_customer_type() == CUSTOMER_TYPE_ZM && SystemParaMenu2.m_video_out_mode  == VIDEO_OUTPUT_MODE_ENUM_MIPI)//卓目客户定制新硬件ZM-MIPI板采用外部信号触发采集数据，外部一个电平信号，触发传输一帧MIPI数据
    {
        st_comm_attr.u08_ex_vsync_en = 1;
        st_comm_attr.u08_ex_vsync_mode = TS_VIU_EX_VSYNC_FROM_IO;
        pst_timing_attr->u16_extra_timing = 1;
    }

    /* 线序 */
    st_comm_attr.u32_i_order_lo = 0x76543210;
    st_comm_attr.u32_i_order_hi = 0xfedcba98;

    st_comm_attr.u32_o_order_lo = 0x76543210;
    st_comm_attr.u32_o_order_hi = 0xfedcba98;

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
    pst_corr->st_fast.u08_mask = 0xfc;
    pst_corr->st_fast.u08_frame_cnt = 0;

    /* 细调 */
    pst_corr->st_trim.u16_thr_hi = 0x2280;
    pst_corr->st_trim.u16_thr_lo = 0x1e00;
    pst_corr->st_trim.u08_mode = TS_VIU_ADJ_MODE_BACKWARD;
    pst_corr->st_trim.u08_step = 4;
    pst_corr->st_trim.u08_mask = 0xfc;
    pst_corr->st_trim.u08_frame_cnt = 0x20;

    ts_tdk_viu_set_comm_attr(TS_VIU_MDEV0, (const ts_viu_comm_attr_s_t *)&st_comm_attr);
    ts_tdk_viu_set_snr_attr(TS_VIU_MDEV0, (const ts_viu_snr_attr_s_t *)pst_snr_attr);
}


#define CORR_SFB_OOC (0)
#define CORR_SFB_OOC_THR (3000)
#define CORR_SFB_OOC_GOAL (8192)
#define CORR_SFB_OOC_GOAL_THR (1500)
#define SFFC_ADJ_INDEX_NUM (16)
static ts_sffc_adj_s_t ast_sffc_adj[SFFC_ADJ_INDEX_NUM] = 
{
    {-50000, 8000},
    {-40000, 8000},
    {-30000, 8000},
    {-20000, 8000},
    {-10000, 8000},
    {     0, 8000},
    { 10000, 8000},
    { 20000, 8000},
    { 30000, 8000},
    { 40000, 8000},
    { 50000, 8000},
    { 60000, 8000},
    { 70000, 8000},
    { 80000, 8000},
    { 90000, 8000},
    {100000, 8000},
};

//static 
ts_void snr_planck640_cut(ts_u32 u32_cut_status, ts_s32 *ps32_temp, ts_u32 *pu32_mean)
{
    ts_cpl_timeval_s_t st_time0, st_time1, st_time2, st_time3, st_time4, st_time5, st_time6, st_time7, st_time8, st_time9, st_time10;

	if(ISP_get_sun_pt_force_flag()>0 ||  global_sys_ctrl_info.m_shutter_force_close_flag==1){
		return;
	}

	ts_cpl_mutex_lock(&global_sys_ctrl_info.m_shutter_fd_mutex);
    ts_cpl_gettimeofday(&st_time0);
    ts_tdk_vpo_freeze(TS_VPO_MDEV0);
    ts_cpl_gettimeofday(&st_time1);

    if (0 == gst_snr_ctx.s32_sffc_alg) {
        ts_s32 s32_temp_sffc = sns_get_temperature();
        ts_s32 s32_sffc_index = 0;
        for (ts_s32 i = 0; i < SFFC_ADJ_INDEX_NUM; i++) {
            if (s32_temp_sffc < ast_sffc_adj[i].s32_temp) {
                s32_sffc_index = i;

                break;
            }
        }

        ts_s32 s32_sffc_coef = 0;
        if (0 == s32_sffc_index) {
            s32_sffc_coef = ast_sffc_adj[s32_sffc_index].s32_coef;
        } else if (s32_sffc_index < SFFC_ADJ_INDEX_NUM - 1) {
            s32_sffc_coef = ast_sffc_adj[s32_sffc_index - 1].s32_coef \
                            + (ast_sffc_adj[s32_sffc_index].s32_coef - ast_sffc_adj[s32_sffc_index - 1].s32_coef) / 10000;
        } else {
            s32_sffc_coef = ast_sffc_adj[SFFC_ADJ_INDEX_NUM - 1].s32_coef;
        }

        ts_isp_sffc_attr_s_t st_sffc_attr;
        ts_tdk_isp_get_sffc_attr(TS_ISP_MDEV0, &st_sffc_attr); 
        st_sffc_attr.u08_enable = 1;
        st_sffc_attr.u08_en_man = 1;   
        st_sffc_attr.u16_man_cor_v = s32_sffc_coef;
        ts_tdk_isp_set_sffc_attr(TS_ISP_MDEV0, &st_sffc_attr);
    }

    ts_tdk_isp_set_pause(TS_ISP_MDEV0, 0xfffffffc);////暂停计算
    ts_tdk_isp_set_rst_val(TS_ISP_MDEV0, 0x7);////复位fw，清除内部迭代信息
    ts_cpl_gettimeofday(&st_time2);

    /* 暂停坏斑，将数据缓存恢复 */
    //ts_tdk_extbpc_pause();
    //ts_tdk_extbpc_sync2dst();

    //数据流已经关闭，进行相应操作
    tdk_snrcfg_shutter_close();
    ts_cpl_msleep(1000/50*2);
    ts_cpl_gettimeofday(&st_time3);

#if CORR_SFB_OOC
    ts_u32 u32_mean_diff = 0;
    ts_u08 u08_mean_diff_pos = 1;
    ts_u08 u08_mean_diff_pos_new = 1;
    ts_u32 s32_sfb = 0;
    ts_tdk_viu_get_mean(TS_VIU_MDEV0, &st_viu_array_info);

    if(st_viu_array_info.u32_mean > CORR_SFB_OOC_GOAL) {
        u32_mean_diff = st_viu_array_info.u32_mean - CORR_SFB_OOC_GOAL;
    } else {
        u32_mean_diff = CORR_SFB_OOC_GOAL - st_viu_array_info.u32_mean;
    }
    vision_log_debug("current mean diff is %d\n",u32_mean_diff);

    if(u32_mean_diff > CORR_SFB_OOC_THR) {
        vision_log_debug("start change work point...\n");
        vision_log_debug("initial sfb is %d\n",vot_detetor_read_sd(195,190));
        for(ts_s32 i=0; i<64; i++){
            vision_log_debug("current mean is %d\n",st_viu_array_info.u32_mean);
            u08_mean_diff_pos = (st_viu_array_info.u32_mean > CORR_SFB_OOC_GOAL) ? 1 : 0;
            s32_sfb = vot_detetor_read_sd(195,190);
            if(!u08_mean_diff_pos) {
                s32_sfb += 1;
            } else {
                s32_sfb -= 1;
            }
            if(s32_sfb > 63)
                s32_sfb = 63;
            if(s32_sfb < 0)
                s32_sfb = 0;
            vot_detetor_write_sd(195,190,s32_sfb);

            ts_cpl_msleep(1000/50*2);
            ts_tdk_viu_get_mean(TS_VIU_MDEV0, &st_viu_array_info);

            if(st_viu_array_info.u32_mean > CORR_SFB_OOC_GOAL) {
                u08_mean_diff_pos_new = 1;
                u32_mean_diff = st_viu_array_info.u32_mean - CORR_SFB_OOC_GOAL;
            } else {
                u08_mean_diff_pos_new = 0;
                u32_mean_diff = CORR_SFB_OOC_GOAL - st_viu_array_info.u32_mean;
            }
            if(u32_mean_diff < CORR_SFB_OOC_GOAL_THR && (u08_mean_diff_pos != u08_mean_diff_pos_new)){
                break;
            }
        }
        *pu32_mean = st_viu_array_info.u32_mean;
        vision_log_debug("final sfb is %d\n",s32_sfb);
        vision_log_debug("final mean is %d\n",st_viu_array_info.u32_mean);

        ts_cpl_msleep(1000/50);
    }

#endif
    ts_cpl_gettimeofday(&st_time4);

    ts_s32 s32_temp_1st = sns_get_temperature();
    ts_cpl_gettimeofday(&st_time5);

    ts_tdk_isp_calc_nuc(TS_ISP_MDEV0);
    if (gst_snr_ctx.s32_sffc_alg > 0) {
        ts_isp_sffc_attr_s_t st_sffc_attr;
        ts_tdk_isp_get_sffc_attr(TS_ISP_MDEV0, &st_sffc_attr); 
        st_sffc_attr.u08_enable = 0;
        ts_tdk_isp_set_sffc_attr(TS_ISP_MDEV0, &st_sffc_attr);

        ts_s32 s32_img_w, s32_img_h;
        snr_attr_s_t *pst_attr;
        pst_attr = (snr_attr_s_t *)&gst_snr_ctx.st_attr;
        s32_img_w = pst_attr->s32_img_w;
        s32_img_h = pst_attr->s32_img_h;
        isp_sw_calc_nuc_sffc(s32_img_w, s32_img_h, 0); 
    }  

    ts_cpl_gettimeofday(&st_time6);

    ts_s32 s32_temp_2st = sns_get_temperature();
    ts_cpl_gettimeofday(&st_time7);

    tdk_snrcfg_shutter_open();
    ts_cpl_gettimeofday(&st_time8);

    /* 同步坏斑，并启动 */
    //ts_tdk_extbpc_sync(1, 1, 1, 0);
    //ts_tdk_extbpc_run();

    ts_tdk_isp_set_pause(TS_ISP_MDEV0, 0);////复位fw，清除内部迭代信息
    ts_cpl_msleep(1010 * 3 / gst_snr_ctx.st_attr.s32_fps); //此处为系统数据流刷新时间， 建议为3-4， 如果打挡片前后图像差异不大，可以适当减少。
    ts_tdk_isp_set_rst_val(TS_ISP_MDEV0, 0);////pause
    ts_cpl_gettimeofday(&st_time9);

    ts_tdk_vpo_unfreeze(TS_VPO_MDEV0);
    ts_cpl_gettimeofday(&st_time10);

    *ps32_temp = (s32_temp_1st + s32_temp_2st) / 2;

#if 0
    vision_log_debug("cut, sffc = %d,  time cost list : \n", s32_sffc_coef);
    vision_log_debug("total = %d ms : \n",  ((st_time10.tv_sec - st_time0.tv_sec) * 1000000 + st_time10.tv_usec - st_time0.tv_usec) / 1000);
    vision_log_debug("vpo frz on  %d ms : \n",  ((st_time1.tv_sec - st_time0.tv_sec) * 1000000 + st_time1.tv_usec - st_time0.tv_usec) / 1000);
    vision_log_debug("isp frz   %d ms : \n",  ((st_time2.tv_sec - st_time1.tv_sec) * 1000000 + st_time2.tv_usec - st_time1.tv_usec) / 1000);
    vision_log_debug("sht close   %d ms : \n",  ((st_time3.tv_sec - st_time2.tv_sec) * 1000000 + st_time3.tv_usec - st_time2.tv_usec) / 1000);
    vision_log_debug("change work point %d ms : \n",  ((st_time4.tv_sec - st_time3.tv_sec) * 1000000 + st_time4.tv_usec - st_time3.tv_usec) / 1000);
    vision_log_debug("temp 1   %d ms : \n",  ((st_time5.tv_sec - st_time4.tv_sec) * 1000000 + st_time5.tv_usec - st_time4.tv_usec) / 1000);
    vision_log_debug("nuc    %d ms : \n",   ((st_time6.tv_sec - st_time5.tv_sec) * 1000000 + st_time6.tv_usec - st_time5.tv_usec) / 1000);
    vision_log_debug("temp 2   %d ms : \n",  ((st_time7.tv_sec - st_time6.tv_sec) * 1000000 + st_time7.tv_usec - st_time6.tv_usec) / 1000);
    vision_log_debug("sht on   %d ms : \n",  ((st_time8.tv_sec - st_time7.tv_sec) * 1000000 + st_time8.tv_usec - st_time7.tv_usec) / 1000);
    vision_log_debug("wait frz  %d ms : \n",  ((st_time9.tv_sec - st_time8.tv_sec) * 1000000 + st_time9.tv_usec - st_time8.tv_usec) / 1000);
    vision_log_debug("vpo frz off  %d ms : \n",  ((st_time10.tv_sec - st_time9.tv_sec) * 1000000 + st_time10.tv_usec - st_time9.tv_usec) / 1000);
#endif

	ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_shutter_fd_mutex);
}


// ts_void planck640c_auto_cut_process(ts_void)
// {
//     ts_s32 s32_temp_dif = 0;
//     ts_s32 s32_temp_cur=0;
//     ts_s32 s32_cut_cnt=0;
//     ts_cpl_timeval_s_t st_time_start, st_time_cur, st_time_last_cut;   
//     ts_cpl_gettimeofday(&st_time_start);

//     ts_u32 u32_cut_status = 0;
//     preStartForceShiftFlag = 1;
//     ts_s32 s32_cut_time_gap_non_static = SystemParaMenu2.m_st1_s * 1000; //15秒
//     ts_s32 s32_cut_time_gap_static = 60 * 1000; //60秒
//     ts_s32 s32_static_time = SystemParaMenu2.m_st1_m * 60 * 1000;  //开机的第一分钟，按照s32_cut_time_gap_non_static间隔打档
//     if(SystemParaMenu2.m_st1_m == 0) {
//         preStartForceShiftFlag = 0;
//     }

//     ts_u32 u32_hsize;

//     ts_s32 s32_temp_last = 0;
//     ts_void *pv_htempdif=ts_null;
//     // ts_s32 s32_force_inter = 2;
//     ts_buffer_s_t st_dbuf;
//     snr_attr_s_t pst_attr;
//     ts_buffer_s_t st_occ_buf;
//     ts_s32 s32_ret=TS_OK;
//     ts_s32 s32_frm=16;
//     ts_isp_nuc_attr_s_t st_nuc_attr;
//     pst_attr = gst_snr_ctx.st_attr;
    
//     ts_s32 s32_img_w = pst_attr.s32_img_w;
//     ts_s32 s32_img_h = pst_attr.s32_img_h;

//     if(get_powerup_temp_drift_comp_mode()==TEMP_DRIFT_COMPENSATION_MODE_ON){
   
//         ts_tdk_viu_get_correct_buf(TS_VIU_MDEV0, &st_occ_buf);

//         pv_htempdif = cali_tempdif_init(ts_null, s32_img_w, s32_img_h, (ts_u32 *)&u32_hsize);
//         if (ts_null == pv_htempdif) {
//             vision_log_warn("cali_tempdif_init err!\n");

//             return ;     
//         }

//         // 第一次打挡片
//         ISP_cut_new();
//         vision_log_debug("1st proc cut !\n");

//         ts_s32 s32_hfile_r = ts_lib_sfc_open(FN_B_CFG);
//         if (s32_hfile_r > 0){
//             ts_lib_sfc_read(s32_hfile_r, 0, (ts_u08*)pv_htempdif, u32_hsize);
//         }

//         ///////////////////////////////////////////////////////
//         vision_log_debug("2st proc cut !\n");

//     }

//     s32_ret = ts_tdk_isp_get_nuc_dbuf(TS_ISP_MDEV0, &st_dbuf);
//     if (s32_ret != TS_OK)
//     {
//         vision_log_warn("ts_tdk_isp_get_nuc_dbuf err !\n");
//         return ;       
//     }

//     ///////////////////////////////////////////////////////
//     //刚启动，帧数设置为32帧
//     ts_tdk_isp_get_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr); 
//     s32_frm = st_nuc_attr.u08_stat_frm_num;
//     st_nuc_attr.u08_stat_frm_num = 32;
//     ts_tdk_isp_set_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr); 

//     // 根据温度判断是否需要重新做occ，第二次打挡片
//     ISP_cut_new();

//     gst_snr_ctx.s32_run_auto_cut = 1;

//     ts_u32 temp_cnt=0;
//     ts_f32 last_det_temp_value = 0.0;
//     ts_u16 diff_temp_value=0;

//     vision_log_debug("gst_snr_ctx.s32_run_auto_cut %d \n",gst_snr_ctx.s32_run_auto_cut);

//     static int test_cnt = 0;
//     while(1) {

//         u32_cut_status = 0;

//         if (SystemParaMenu2.Usermenu_SW) {
//             if (global_sys_ctrl_info.Shifting_Gears_Flag || SystemParaMenu2.m_temperCalibra_Flag || SystemParaMenu2.m_user_tempFrame_Mode == TEMP_CALIBRATION_DISP_AD) {
//                 /* TODO: 测温进入标定状态 */
//                 ts_cpl_msleep(300);
//                 continue;
//             }
//         }

//         if (preStartForceShiftFlag) {
//             ts_cpl_gettimeofday(&st_time_cur);
//             ts_s32 s32_time_diff;
//             s32_time_diff = ((st_time_cur.tv_sec - st_time_start.tv_sec) * 1000000 + st_time_cur.tv_usec - st_time_start.tv_usec) / 1000;
//             if (s32_time_diff >= s32_static_time) 
//             {
//                 vision_log_warn(" preStartForceShiftFlag %d !\n",preStartForceShiftFlag);
//                 preStartForceShiftFlag = 0;
//                 continue;
//             }

//             s32_time_diff = ((st_time_cur.tv_sec - st_time_last_cut.tv_sec) * 1000000 + st_time_cur.tv_usec - st_time_last_cut.tv_usec) / 1000;
//             if (s32_time_diff >= s32_cut_time_gap_non_static) 
//             {

//                 u32_cut_status |= 0x1;
//                 ts_cpl_gettimeofday(&st_time_last_cut); // 获取最新时间
//             }
            
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
//             continue;
//         }


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
// 	        {
//                 ts_cpl_gettimeofday(&st_time_cur);
//                 ts_s32 s32_time_diff = ((st_time_cur.tv_sec - st_time_last_cut.tv_sec) * 1000000 + st_time_cur.tv_usec - st_time_last_cut.tv_usec) / 1000;
//                 if (s32_time_diff >= s32_cut_time_gap_static){
//                     u32_cut_status |= 0x2;
//                 }
                
//             }

//             temp_cnt++;
//             if(temp_cnt>10)
//             {
//                 temp_cnt = 0;
//                 if(SystemParaMenu2.m_det_st != 0) {
//                     vot_read_det_ad();

//                     diff_temp_value = (global_sys_ctrl_info.m_det_temp_value - last_det_temp_value) * 10;
                    
//                     if(abs(diff_temp_value) >= SystemParaMenu2.m_det_st) {
//                         u32_cut_status |= 0x4; 
//                         last_det_temp_value = global_sys_ctrl_info.m_det_temp_value;
//                     }                   
//                 }            
//             }
//         }
 
        
//         if (gst_snr_ctx.s32_force_cut) {
//             u32_cut_status |= 0x8;
//             gst_snr_ctx.s32_force_cut = 0;
//         }


//         if (6 == s32_cut_cnt)
//         { 
//             // vision_log_warn("s32_force_inter = %d s32_frm %d \n", s32_force_inter,s32_frm);
//             st_nuc_attr.u08_stat_frm_num = s32_frm;
//             ts_tdk_isp_set_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr);
//             // s32_force_inter=0;
//         }

//         if ((u32_cut_status != 0) && gst_snr_ctx.s32_run_auto_cut) 
//         {
            
//             //兼容手动打档优先
//             if( (u32_cut_status & 0x8) || (u32_cut_status & 0x1)|| (ISP_get_ShutterMode() != SHUTTER_MODE_OFF)){
    
//                 vision_log_debug("to cut, status = 0x%x, diff_temp_value = %d \n", u32_cut_status, diff_temp_value);
//                 ISP_cut_new();
//                 s32_temp_cur = global_sys_ctrl_info.m_cali_temp_value;
//                 last_det_temp_value = global_sys_ctrl_info.m_det_temp_value;
//                 temp_cnt = 0;
//                 s32_cut_cnt++;
                
//             }

//             ts_cpl_gettimeofday(&st_time_last_cut);    
            
//             if(get_powerup_temp_drift_comp_mode()==TEMP_DRIFT_COMPENSATION_MODE_ON){
//                 vision_log_debug("s32_temp_cur = %d\n", s32_temp_cur);

//                 s32_temp_dif = cali_calc_tempdif(pv_htempdif, st_dbuf.pu08_ptr, s32_temp_cur, s32_img_w, s32_img_h);

//                 ts_tdk_isp_flush_nuc_dbuf(TS_ISP_MDEV0, &st_dbuf);

//                 s32_temp_last = s32_temp_cur;
//                 vision_log_debug("s32_temp_dif = %d\n", s32_temp_dif);
//                 vision_log_debug("gst_snr_ctx.s32_run_auto_b = %d\n", gst_snr_ctx.s32_run_auto_b);
//                 vision_log_debug("gst_snr_ctx.s32_run_auto_cut = %d\n", gst_snr_ctx.s32_run_auto_cut);
//             }
//             u32_cut_status = 0;

//         }
//         else //没有开机/强制/定时打档，则进行判断是否需要温漂打档
//         {
//             if(get_powerup_temp_drift_comp_mode()==TEMP_DRIFT_COMPENSATION_MODE_ON){

//                 if (gst_snr_ctx.s32_run_auto_b && gst_snr_ctx.s32_run_auto_cut)
//                 {
                    
//                     ts_s32 s32_coef = 0;
//                     // printf("s32_temp_dif = %d\n", s32_temp_dif);
//                     if (s32_cut_cnt>0 && s32_temp_dif > 300)
//                     {
//                         s32_temp_cur = sns_get_temperature();
                        
//                         // vision_log_debug("s32_temp_cur = %d\n", s32_temp_cur);

//                         s32_coef = cali_calc_tempdif_coef(s32_temp_cur, s32_temp_last, s32_temp_dif);
                        
//                         // vision_log_debug("s32_coef = %d\n", s32_coef);
//                         ts_isp_nuc_attr_s_t st_nuc_attr;
//                         ts_tdk_isp_get_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr); 
//                         st_nuc_attr.u16_coef = s32_coef;
//                         ts_tdk_isp_set_nuc_attr(TS_ISP_MDEV0, &st_nuc_attr); 
//                     }  
//                     if (0 == test_cnt % 50)
//                     {
//                         vision_log_debug("s32_temp_dif = %d, temp = [%d,%d] coef = %d \n", s32_temp_dif, s32_temp_cur, s32_temp_last, s32_coef);
//                     }
//                 }
//             }
            
//         }
        
//         test_cnt++;
//         ts_cpl_fix_msleep(100);  
//     }
// }


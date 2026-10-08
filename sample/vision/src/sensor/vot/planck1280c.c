/*
 * planck1280c.c -- vision planck1280c
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
#include "vot_comm.h"

extern SysCtrlInfo_s_t global_sys_ctrl_info;

#define PLANCK1280_TEST_COL_GOAL (16383 - 256)
#define SDL_PLANCK1280 256
#define SDL_PLANCK1280_M1 (SDL_PLANCK1280 - 1)

ts_s32 planck1280c_get_temperature(ts_void)
{
    ts_u32 u32_fv;
    ts_s32 s32_ret ,s32_temp;

    ts_cpl_mutex_lock(&global_sys_ctrl_info.m_viu_stats_mutex);

    s32_ret = ts_tdk_viu_get_stats(TS_VIU_MDEV0, TS_VIU_STATS_COL, 1294, 0, (ts_u32 *)&u32_fv, ts_null); 
    if (TS_OK == s32_ret) {
        ts_s32 s32_fv = u32_fv;
        ts_f32 f32_fv = s32_fv;
        f32_fv = 20.0 + (f32_fv / 1034.0 - 6867.0) / (0 - 33.8);
        s32_temp = 1000.0 * f32_fv;
    } else {
        s32_temp = 0;
    }

    ts_cpl_mutex_unlock(&global_sys_ctrl_info.m_viu_stats_mutex);
    return s32_temp;
}

#if 1
////设置viu、isp时钟50MHz  
ts_void planck1280c_clkm_init(ts_void)
{
    ts_clkm_cfg_s_t st_clk_cfg;
    ts_clkm_viu_cfg_s_t *pst_viu_clk;
    ts_clkm_vpo_cfg_s_t *pst_vpo_clk;

    pst_viu_clk = (ts_clkm_viu_cfg_s_t *)&st_clk_cfg.st_viu_clk;
    pst_vpo_clk = (ts_clkm_vpo_cfg_s_t *)&st_clk_cfg.st_vpo_clk;

    /* viu 时钟 */
    pst_viu_clk->u32_snr_clk = 50000000;
    pst_viu_clk->u32_clk_hz = 50000000; 
    pst_viu_clk->en_clk_src = TS_CLKM_SRC_VIU_SNR_CLK;//TS_CLKM_SRC_ISP_CLK;

    pst_viu_clk->u32_snr_internal_clk_en = 1;
    pst_viu_clk->u32_snr_o_clk_phase = 0;
    pst_viu_clk->u32_snr_o_clk_inv = 1;//0;
    pst_viu_clk->u32_sample_clk_en = 1;
    pst_viu_clk->en_sample_clk_src = TS_CLKM_SRC_VIU_SNR_CLK;
    pst_viu_clk->u32_sample_clk_phase = 0;
    pst_viu_clk->u32_sample_clk_inv = 1;//0;

    /* isp clock */
    st_clk_cfg.u32_isp_clk_hz = 50000000;
    /* vpo module */
    pst_vpo_clk->en_vpo_clk_src = TS_CLKM_SRC_VPO_PLL;
    pst_vpo_clk->u32_vpo_clk_hz = 50000000;
    /* vpo btenv */
    pst_vpo_clk->u32_btenc_pix_clk_hz = 50000000;
    pst_vpo_clk->u32_btenc_mod_clk_hz = 50000000;
    pst_vpo_clk->u32_btenc_o_clk_inv = 1;

    /* vpo lcd */
    pst_vpo_clk->u32_lcd_pix_clk_hz = 50000000;
    pst_vpo_clk->u32_lcd_mod_clk_hz =50000000;
    pst_vpo_clk->u32_lcd_o_clk_en = 0;
    pst_vpo_clk->u32_lcd_o_clk_inv = 0;
    //vision_log_warn("pst_vpo_clk->u32_lcd_pix_clk_hz:%d \n",pst_vpo_clk->u32_lcd_pix_clk_hz);
    /* vpo cvbs */
    pst_vpo_clk->u32_cvbs_clk_en = 1;
    pst_vpo_clk->u32_dac_clk_en = 1;
    pst_vpo_clk->en_dac_clk_src = TS_CLKM_SRC_CVBS_PLL;
    pst_vpo_clk->u32_dac_clk_inv = 1;

    /* vpo mipi */
    pst_vpo_clk->u32_mipi_dhpy_ref_clk_en = 0;

    ts_tdk_clkm_set((const ts_clkm_cfg_s_t *)&st_clk_cfg);

}
#else
//设置viu、isp时钟42MHz 
ts_void planck1280c_clkm_init(ts_void)
{
    ts_clkm_cfg_s_t st_clk_cfg;
    ts_clkm_viu_cfg_s_t *pst_viu_clk;
    ts_clkm_vpo_cfg_s_t *pst_vpo_clk;

    pst_viu_clk = (ts_clkm_viu_cfg_s_t *)&st_clk_cfg.st_viu_clk;
    pst_vpo_clk = (ts_clkm_vpo_cfg_s_t *)&st_clk_cfg.st_vpo_clk;

    /* viu 时钟 */
    pst_viu_clk->u32_snr_clk = 42000000;// pllviu/4=168000000/4
    pst_viu_clk->u32_clk_hz = 42000000; // pllviu/4=168000000/4
    pst_viu_clk->en_clk_src = TS_CLKM_SRC_VIU_SNR_CLK;//TS_CLKM_SRC_ISP_CLK;

    pst_viu_clk->u32_snr_internal_clk_en = 1;
    pst_viu_clk->u32_snr_o_clk_phase = 0;
    pst_viu_clk->u32_snr_o_clk_inv = 0;
    pst_viu_clk->u32_sample_clk_en = 1;
    pst_viu_clk->en_sample_clk_src = TS_CLKM_SRC_VIU_SNR_CLK;
    pst_viu_clk->u32_sample_clk_phase = 0;
    pst_viu_clk->u32_sample_clk_inv = 0;

    /* isp clock */
    st_clk_cfg.u32_isp_clk_hz = 42000000;//SNR_CALC_ISP_CLK(168000000, 1280, 1024, 25);//pllisp=168000000
   // vision_log_warn("st_clk_cfg.u32_isp_clk_hz:%d \n",st_clk_cfg.u32_isp_clk_hz);
    /* vpo module */
    pst_vpo_clk->en_vpo_clk_src = TS_CLKM_SRC_VPO_PLL;
    pst_vpo_clk->u32_vpo_clk_hz = 50000000;// VPO_CALC_BT_CLK(300000000, 1280, 1024, 25);//pllvpo=300000000;//需要把vpo的频率提上来，否则，可能cvbs输出出现异常
   // vision_log_warn("pst_vpo_clk->u32_vpo_clk_hz:%d \n",pst_vpo_clk->u32_vpo_clk_hz);
    /* vpo btenv */
    pst_vpo_clk->u32_btenc_pix_clk_hz = 50000000;// VPO_CALC_BT_CLK(300000000, 1280, 1024, 25);//pllvpo=300000000
    pst_vpo_clk->u32_btenc_mod_clk_hz = pst_vpo_clk->u32_btenc_pix_clk_hz;//50000000;
    pst_vpo_clk->u32_btenc_o_clk_inv = 1;
   // vision_log_warn("pst_vpo_clk->u32_btenc_pix_clk_hz:%d \n",pst_vpo_clk->u32_btenc_pix_clk_hz);
    /* vpo lcd */
    pst_vpo_clk->u32_lcd_pix_clk_hz = 50000000;// VPO_CALC_BT_CLK(300000000, 1280, 1024, 25);//pllvpo=300000000
    pst_vpo_clk->u32_lcd_mod_clk_hz =pst_vpo_clk->u32_lcd_pix_clk_hz;// 50000000;
    pst_vpo_clk->u32_lcd_o_clk_en = 0;
    pst_vpo_clk->u32_lcd_o_clk_inv = 0;
    //vision_log_warn("pst_vpo_clk->u32_lcd_pix_clk_hz:%d \n",pst_vpo_clk->u32_lcd_pix_clk_hz);
    /* vpo cvbs */
    pst_vpo_clk->u32_cvbs_clk_en = 1;
    pst_vpo_clk->u32_dac_clk_en = 1;
    pst_vpo_clk->en_dac_clk_src = TS_CLKM_SRC_CVBS_PLL;
    pst_vpo_clk->u32_dac_clk_inv = 1;
    /* vpo mipi */
    pst_vpo_clk->u32_mipi_dhpy_ref_clk_en = 0;
    ts_tdk_clkm_set((const ts_clkm_cfg_s_t *)&st_clk_cfg);
}
#endif

ts_void planck1280c_viu_init(snr_attr_s_t *pst_attr, ts_viu_snr_attr_s_t *pst_snr_attr)
{
    ts_viu_comm_attr_s_t st_comm_attr;
    ts_viu_corr_attr_s_t *pst_corr;
    ts_viu_snr_timing_attr_s_t *pst_timing_attr;

    ts_cpl_memset32((ts_u32 *)&st_comm_attr, 0, (sizeof(ts_viu_comm_attr_s_t) >> 2));

    st_comm_attr.en_snr = TS_VIU_SNRT_RV1280;
    pst_timing_attr = &st_comm_attr.st_timing_attr;

    pst_timing_attr->u08_redefine_timing_en = 1;
    pst_timing_attr->u08_img_inv_en = 0;
    #if 1
    //设置viu、isp时钟50MHz 
    pst_timing_attr->u16_raw_h = 1249;
    pst_timing_attr->u16_raw_w = 1600;
    pst_timing_attr->u16_extra_timing = 1600;
    pst_timing_attr->u32_data_dly = 8128;
    #else
    //设置viu、isp时钟42MHz 
    pst_timing_attr->u16_raw_h = 1080;//40ms/37us=1081
    pst_timing_attr->u16_raw_w = 1554;//42MHz*37ms
    pst_timing_attr->u16_extra_timing = 1554;
    pst_timing_attr->u32_data_dly = 7898;
    #endif
    
    pst_timing_attr->u16_vsync_start = 2;
    pst_timing_attr->u16_hsync_start = 0;
    pst_timing_attr->u16_vld_h = 1034;
    pst_timing_attr->u16_vld_w = 1312;
    pst_timing_attr->u16_cut_start_v = 2;
    pst_timing_attr->u16_cut_start_h = 12;
   // pst_timing_attr->u32_data_dly = 7898;//8128;

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
    pst_corr->u08_freq = 2;

    /* 快调 */
    pst_corr->st_fast.u16_thr_hi = 0x2400;
    pst_corr->st_fast.u16_thr_lo = 0x1c00;
    pst_corr->st_fast.u08_mode = TS_VIU_ADJ_MODE_BACKWARD;
    pst_corr->st_fast.u08_step = 4;
    pst_corr->st_fast.u08_mask = 0x00;
    pst_corr->st_fast.u08_frame_cnt = 0;

    /* 细调 */
    pst_corr->st_trim.u16_thr_hi = 0x2280;
    pst_corr->st_trim.u16_thr_lo = 0x1d80;
    pst_corr->st_trim.u08_mode = TS_VIU_ADJ_MODE_BACKWARD;
    pst_corr->st_trim.u08_step = 4;
    pst_corr->st_trim.u08_mask = 0xfc;
    pst_corr->st_trim.u08_frame_cnt = 0x20;

    ts_tdk_viu_set_comm_attr(TS_VIU_MDEV0, (const ts_viu_comm_attr_s_t *)&st_comm_attr);
    ts_tdk_viu_set_snr_attr(TS_VIU_MDEV0, (const ts_viu_snr_attr_s_t *)pst_snr_attr);
}

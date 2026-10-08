/*
 * rv1280.c -- vision planck1280 虚拟转接版本
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

/* 25HZ */
static ts_viu_snr_cfg_s_t sast_rv1280_cfg[] = {
    {0x0000001f, 0x00000003},//[255:224]
    {0x0000001e, 0x1B80ECC6},//[223:192]
    {0x0000001d, 0x48780653},//[191:160]
    {0x0000001c, 0xB1653AF4},//[159:128]
    {0x0000001b, 0x5C031F88},//[127:96]
    {0x0000001a, 0xFF025CA4},//[95:64]
    {0x00000019, 0xE4E4375C},//[63:32]
    {0x00000018, 0x0BC68AF0},//[31:0]
};

static ts_viu_snr_attr_s_t sst_rv1280_attr = {
    .u16_uint_len = 256 - 1,
    .u16_cfg_num = sizeof(sast_rv1280_cfg) / sizeof(ts_viu_snr_cfg_s_t),
    .pst_cfg = (ts_viu_snr_cfg_s_t *)&sast_rv1280_cfg,
};

static ts_void rv1280_clkm_init(snr_attr_s_t *pst_attr)
{
    ts_clkm_cfg_s_t st_clk_cfg;
    ts_clkm_viu_cfg_s_t *pst_viu_clk;
    ts_clkm_vpo_cfg_s_t *pst_vpo_clk;

    pst_viu_clk = (ts_clkm_viu_cfg_s_t *)&st_clk_cfg.st_viu_clk;
    pst_vpo_clk = (ts_clkm_vpo_cfg_s_t *)&st_clk_cfg.st_vpo_clk;

    /* viu 时钟 */
    pst_viu_clk->u32_clk_hz = 50000000;
    pst_viu_clk->en_clk_src = TS_CLKM_SRC_ISP_CLK;

    pst_viu_clk->u32_snr_clk = 50000000;
    pst_viu_clk->u32_snr_internal_clk_en = 1;
    pst_viu_clk->u32_snr_o_clk_phase = 0;
    pst_viu_clk->u32_snr_o_clk_inv = 0;

    pst_viu_clk->u32_sample_clk_en = 1;
    pst_viu_clk->en_sample_clk_src = TS_CLKM_SRC_VIU_SNR_CLK;
    pst_viu_clk->u32_sample_clk_phase = 0;
    pst_viu_clk->u32_sample_clk_inv = 0;

    /* isp clock */
    st_clk_cfg.u32_isp_clk_hz = 50000000;
    st_clk_cfg.u32_npu_clk_hz = 0;
    pst_attr->st_npu.u32_clk = st_clk_cfg.u32_npu_clk_hz;

    /* vpo module */
    pst_vpo_clk->en_vpo_clk_src = TS_CLKM_SRC_VPO_PLL;
    pst_vpo_clk->u32_vpo_clk_hz = 50000000;

    /* vpo btenv */
    pst_vpo_clk->u32_btenc_pix_clk_hz = 50000000;
    pst_vpo_clk->u32_btenc_mod_clk_hz = 50000000;
    pst_vpo_clk->u32_btenc_o_clk_inv = 0;

    /* vpo cvbs */
    pst_vpo_clk->u32_cvbs_clk_en = 1;
    pst_vpo_clk->u32_dac_clk_en = 1;
    pst_vpo_clk->en_dac_clk_src = TS_CLKM_SRC_CVBS_PLL;
    pst_vpo_clk->u32_dac_clk_inv = 1;

    /* vpo lcd */
    pst_vpo_clk->u32_lcd_pix_clk_hz = 50000000;
    pst_vpo_clk->u32_lcd_mod_clk_hz = 50000000;
    pst_vpo_clk->u32_lcd_o_clk_en = 0;

    /* vpo mipi */
    pst_vpo_clk->u32_mipi_dhpy_ref_clk_en = 0;

    ts_tdk_clkm_set((const ts_clkm_cfg_s_t *)&st_clk_cfg);
}

static ts_void rv1280_viu_init(snr_attr_s_t *pst_attr, ts_viu_snr_attr_s_t *pst_snr_attr)
{
    ts_viu_comm_attr_s_t st_comm_attr;
    ts_viu_corr_attr_s_t *pst_corr;
    ts_viu_snr_timing_attr_s_t *pst_timing_attr;

    ts_cpl_memset32((ts_u32 *)&st_comm_attr, 0, (sizeof(ts_viu_comm_attr_s_t) >> 2));

    st_comm_attr.en_snr = TS_VIU_SNRT_RV1280;
    pst_timing_attr = &st_comm_attr.st_timing_attr;

    pst_timing_attr->u08_redefine_timing_en = 1;
    pst_timing_attr->u08_img_inv_en = 0;
    pst_timing_attr->u16_raw_h = 1249;
    pst_timing_attr->u16_raw_w = 1600;
    pst_timing_attr->u16_extra_timing = 1600;
    pst_timing_attr->u16_vsync_start = 2;
    pst_timing_attr->u16_hsync_start = 0;
    pst_timing_attr->u16_vld_h = 1034;
    pst_timing_attr->u16_vld_w = 1312;
    pst_timing_attr->u16_cut_start_v = 2;
    pst_timing_attr->u16_cut_start_h = 12;
    pst_timing_attr->u32_data_dly = 8128;

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

/* 用户板级初始化实现 */
TS_WEAK ts_void rv1280_brd_init(snr_attr_s_t *pst_attr)
{

}


/* 用户板级修正实现，主要用于探测器线序调整等配置 */
TS_WEAK ts_void rv1280_brd_fix(snr_attr_s_t *pst_attr)
{

}

static ts_void start_rv1280(ts_u32 u32_param)
{
    snr_attr_s_t *pst_attr;
    snr_ctx_s_t *pst_ctx;
    ts_void *pv_isp_cfg_to;
    ts_isp_nuc_attr_s_t st_nuc_attr;
    ts_isp_sffc_attr_s_t sffc_attr;
    ts_viu_corr_rst_s_t st_rst_attr;

    pst_ctx = (snr_ctx_s_t *)u32_param;
    pst_attr = (snr_attr_s_t *)&pst_ctx->st_attr;
    pst_attr->get_temperature = ts_null;
    pst_attr->s32_img_w = (ts_s32)1280;
    pst_attr->s32_img_h = (ts_s32)1024;
    pst_attr->s32_fps = (ts_f32)25;
    pst_attr->s32_vo_img_w = (ts_s32)1280;
    pst_attr->s32_vo_img_h = (ts_s32)1024;

    /* 是否开启开机画面 */
    pst_attr->u32_pod_dly_ms = 1000;

    /* 是否开启 raw 同传 */
    pst_attr->u08_y_oe = 1;
    pst_attr->u08_raw_oe = 0;
    pst_attr->u08_raw_pos = TS_LUT_POS_AFTER_BPC;

    /* npu 设置 */
    pst_attr->st_npu.en_ops = TS_NPU_OPS_DISABLE;
    pst_attr->st_npu.u32_clk = 0;

    /* cvbs 通道配置 */
    pst_attr->st_cvbs.u08_en = 0;
    pst_attr->st_cvbs.u08_sd_en = 0;
    pst_attr->st_cvbs.u08_orgi_en = 0;
    pst_attr->st_cvbs.u08_pal_en = 1;

    /* mem 通道配置 */
    pst_attr->u32_mem_en = 1;

    /* bte 通道配置: bt601: 8bit,sync 都设置成1；*/
    pst_attr->st_bte.u08_en = 0;
    pst_attr->st_bte.u08_8bit_en = 0;
    pst_attr->st_bte.u08_external_sync_en = 1;

    /* mipi 通道配置 */
    pst_attr->st_mipi.u08_en = 0;
    pst_attr->st_mipi.u08_dsi_en = 0;
    pst_attr->st_mipi.u08_data_type = TS_VPO_CSI2_DT_YUV422;
    pst_attr->st_mipi.u08_dphy_lane_num = 2;
    pst_attr->st_mipi.u16_data_rate = 120;
    pst_attr->st_mipi.u08_clock_continue = 1;

    rv1280_brd_init(pst_attr);

    /* 加载时钟模块 */
    ts_cpl_insmod(clkm, "pllsys=100000000 pllddr=250000000 pllviu=300000000 pllisp=300000000 pllvpo=300000000");

    rv1280_clkm_init(pst_attr);

    snr_insmod_all_tdk(0);

    tdk_snrcfg_reset();

    snr_init_smart(sast_rv1280_cfg, sizeof(sast_rv1280_cfg));
    rv1280_viu_init(pst_attr, (ts_viu_snr_attr_s_t *)&sst_rv1280_attr);

    rv1280_brd_fix(pst_attr);

    snr_cfg_vpo(pst_attr);

    /* 配置ISP参数 */
    pv_isp_cfg_to = snr_get_isp_cfg(TDK_ISP_DEFCFG_128P);
    snr_isp_init(pst_attr->s32_img_w, pst_attr->s32_img_h, pst_attr->s32_fps, pv_isp_cfg_to);

    /* 使能输入 */
    ts_tdk_viu_enable(TS_VIU_MDEV0);

    tdk_snrcfg_shutter_close();

    st_rst_attr.u08_reset_only = 0;
    st_rst_attr.u08_reset_val = 0x80;
    ts_tdk_viu_exec_correct(TS_VIU_MDEV0, &st_rst_attr);

    tdk_snrcfg_shutter_open();

    /* OCC 初始化 */
    //snr_init_ooc();

    st_nuc_attr.s32_en = TS_ENABLE;
    st_nuc_attr.s32_cutoff = TS_DISABLE;
    #if 0
    st_nuc_attr.en_mode = TS_ISP_NUC_MODE_GAIN_OFFSET; 
    st_nuc_attr.en_stat_mode = 1;  
    #else
    st_nuc_attr.en_mode = TS_ISP_NUC_MODE_DELTAB; 
    st_nuc_attr.en_stat_mode = 2;  
    #endif
    st_nuc_attr.u08_stat_frm_num = 31;
    st_nuc_attr.u16_mean_val = 0;
    st_nuc_attr.u16_coef = 0;
    st_nuc_attr.u08_stat_fack_frm2_en = 0;
    st_nuc_attr.u16_frm_oft = 0;
    st_nuc_attr.u08_bbuf_dbl = 0;
    st_nuc_attr.u08_dbuf_dbl = 0;

    snr_init_nuc((ts_isp_nuc_attr_s_t *)&st_nuc_attr);

    sffc_attr.u08_enable = 1;
    sffc_attr.u08_en_man = 1;
    sffc_attr.u08_upd_speed = 32;
    sffc_attr.u16_flt_thr = 2560;
    sffc_attr.u16_man_cor_v = 8500;
    snr_init_sffc((ts_isp_sffc_attr_s_t *)&sffc_attr);
}

snr_register(rv1280, start_rv1280);


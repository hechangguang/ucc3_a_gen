/*
 * testbar.c -- tdk 测试模式使用例子
 */
#include <base/ts_type.h>
#include <base/ts_bitop.h>
#include <cpl/ts_cpl.h>
#include <uapi/chip_def.h>
#include <uapi/isp_v100.h>
#include <uapi/viu_v100.h>
#include <uapi/clkm_v100.h>
#include <uapi/vpo_v100.h>
#include <command.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "api.h"
#include "sffs_files.h"
#include "snr_comm.h"

/* 256,384,640 */
#define TEST_BAR_PIX        (640)
#define TEST_BAR_FPS        (50)

#if (256 == TEST_BAR_PIX)
#define TEST_BAR_WIDTH      (256)
#define TEST_BAR_HEIGHT     (192)
#elif (384 == TEST_BAR_PIX)
#define TEST_BAR_WIDTH      (384)
#define TEST_BAR_HEIGHT     (288)
#elif (400 == TEST_BAR_PIX)
#define TEST_BAR_WIDTH      (400)
#define TEST_BAR_HEIGHT     (300)
#elif (640 == TEST_BAR_PIX)
#define TEST_BAR_WIDTH      (640)
#define TEST_BAR_HEIGHT     (512)
#else
#define TEST_BAR_WIDTH      (1280)
#define TEST_BAR_HEIGHT     (1024)
#endif

#define SUPER_RESOLUTION_EN         (0)

#define SR_WIDTH        (960)
#define SR_HEIGHT       (768)

//#define TEST_BAR_RUN_ISP


static ts_void viu_init_testbar(ts_u16 u16_imgW,ts_u16 u16_imgH,ts_u16 u16_fps)
{
    ts_viu_comm_attr_s_t st_comm_attr;

    st_comm_attr.en_snr = TS_VIU_SNRT_TESTBAR;
    st_comm_attr.u16_height = u16_imgH;
    st_comm_attr.u16_width = u16_imgW;
    st_comm_attr.u08_pixel_ratio = 1;
    st_comm_attr.f32_fps = u16_fps;
    st_comm_attr.u16_rst_dly_us = 0;

    ts_tdk_viu_set_comm_attr(TS_VIU_MDEV0, (const ts_viu_comm_attr_s_t *)&st_comm_attr);
}


static ts_void clkm_init_testbar(ts_void)
{
    ts_clkm_cfg_s_t st_clk_cfg;
    ts_clkm_viu_cfg_s_t *pst_viu_clk;
    ts_clkm_vpo_cfg_s_t *pst_vpo_clk;

    pst_viu_clk = (ts_clkm_viu_cfg_s_t *)&st_clk_cfg.st_viu_clk;
    pst_vpo_clk = (ts_clkm_vpo_cfg_s_t *)&st_clk_cfg.st_vpo_clk;

    /* 彩条无需配置 viu 时钟 */
    pst_viu_clk->u32_clk_hz = 0;
    pst_viu_clk->u32_snr_clk = 0;
    pst_viu_clk->en_clk_src = TS_CLKM_SRC_ISP_CLK;

    /* isp clock */
    st_clk_cfg.u32_isp_clk_hz = SNR_CALC_ISP_CLK(180000000, TEST_BAR_WIDTH, TEST_BAR_HEIGHT, TEST_BAR_FPS);
    //st_clk_cfg.u32_isp_clk_hz = 16000000;

#if SUPER_RESOLUTION_EN
    //st_clk_cfg.u32_npu_clk_hz = 64000000;
#else
    //st_clk_cfg.u32_npu_clk_hz = 0;
#endif

    /* vpo module */
    pst_vpo_clk->en_vpo_clk_src = TS_CLKM_SRC_VPO_PLL;
    pst_vpo_clk->u32_vpo_clk_hz = 45000000;

    /* vpo btenv */
#if SUPER_RESOLUTION_EN
    pst_vpo_clk->u32_btenc_pix_clk_hz = VPO_CALC_BT_CLK(180000000, SR_WIDTH, SR_HEIGHT, TEST_BAR_FPS);
#else
    pst_vpo_clk->u32_btenc_pix_clk_hz = VPO_CALC_BT_CLK(180000000, TEST_BAR_WIDTH, TEST_BAR_HEIGHT, TEST_BAR_FPS);
#endif

    pst_vpo_clk->u32_btenc_pix_clk_hz = 45000000;
    pst_vpo_clk->u32_btenc_mod_clk_hz = pst_vpo_clk->u32_btenc_pix_clk_hz;
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

    /* vpo mipi */
    pst_vpo_clk->u32_mipi_dhpy_ref_clk_en = 0;

    ts_tdk_clkm_set((const ts_clkm_cfg_s_t *)&st_clk_cfg);
}


ts_void start_testbar(ts_u32 u32_param)
{
    snr_attr_s_t *pst_attr;
    snr_ctx_s_t *pst_ctx;

    pst_ctx = (snr_ctx_s_t *)u32_param;
    pst_attr = (snr_attr_s_t *)&pst_ctx->st_attr;

    pst_attr->s32_img_w = (ts_s32)TEST_BAR_WIDTH;
    pst_attr->s32_img_h = (ts_s32)TEST_BAR_HEIGHT;
    pst_attr->s32_fps = (ts_f32)TEST_BAR_FPS;

    /* npu 功能 */
    pst_attr->st_npu.en_ops = TS_NPU_OPS_DISABLE;
    pst_attr->st_npu.u32_clk = 0;

    /* 是否开启开机画面 */
    pst_attr->u32_pod_dly_ms = 0;

    pst_attr->u08_y_oe = 1;
    pst_attr->u08_raw_oe = 0;
    pst_attr->u08_raw_pos = TS_LUT_POS_FINAL_RAW;
    pst_attr->u08_info_en = 1;
    pst_attr->u08_info_line_num = 4;

    /* cvbs 通道配置 */
    pst_attr->st_cvbs.u08_en = 1;
    pst_attr->st_cvbs.u08_sd_en = 0;
    pst_attr->st_cvbs.u08_orgi_en = 0;
    pst_attr->st_cvbs.u08_pal_en = 1;

    /* mem 通道配置 */
    pst_attr->u32_mem_en = 1;

    if (TS_NPU_OPS_SR == pst_attr->st_npu.en_ops) {
        pst_attr->s32_vo_img_w = (ts_s32)(TEST_BAR_WIDTH * 2);
        pst_attr->s32_vo_img_h = (ts_s32)(TEST_BAR_HEIGHT * 2);
    } else {
        pst_attr->s32_vo_img_w = (ts_s32)400;
        pst_attr->s32_vo_img_h = (ts_s32)300;
    }

    /* bte 通道配置: bt601: 8bit,sync 都设置成1；*/
    pst_attr->st_bte.u08_en = 0;
    pst_attr->st_bte.u08_8bit_en = 1;
    pst_attr->st_bte.u08_external_sync_en = 0;

    /* 关闭 pll 安全模式， vco 计算将不会按照 IP 要求进行，后果不保证 */
    ts_cpl_insmod(clkm, "pllsys=100000000 pllddr=250000000 pllviu=162000000 pllisp=180000000 pllvpo=180000000 pllsafe=1");

    clkm_init_testbar();
    snr_insmod_all_tdk(0);

    viu_init_testbar(TEST_BAR_WIDTH, TEST_BAR_HEIGHT, TEST_BAR_FPS);

    snr_cfg_vpo(pst_attr);

#ifdef TEST_BAR_RUN_ISP
    ts_void *pv_isp_cfg_to = snr_get_isp_cfg(TDK_ISP_DEFCFG_512P);
    snr_isp_init(pst_attr->s32_img_w, pst_attr->s32_img_h, pst_attr->s32_fps, pv_isp_cfg_to);
#else
    isp_demo();
#endif

    ts_tdk_viu_enable(TS_VIU_MDEV0);

#ifdef TEST_BAR_RUN_ISP
    ts_cpl_kthread_create(ts_null, (ts_cpl_kthread_t)snr_auto_cut_process, ts_null, "auto_cut", 5, 4096);
#endif
}


TS_DECLARE_CMD(cvbs)
{
    ts_u32 u32_cfg;
    snr_attr_s_t *pst_attr;

    u32_cfg = ts_cpl_str2num(argv[1]);
    pst_attr = &gst_snr_ctx.st_attr;

    ts_tdk_vpo_disable_chn(TS_VPO_CVBS_MDEV);

    if (u32_cfg > 0)
        pst_attr->st_cvbs.u08_orgi_en = 1;
    else
        pst_attr->st_cvbs.u08_orgi_en = 0;


    snr_cfg_vpo(pst_attr);

    return 0;
}

snr_register(testbar, start_testbar);


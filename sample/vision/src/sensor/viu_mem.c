/*
 * mem.c -- tdk 测试模式使用例子
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
#include "snr_comm.h"


typedef struct sim_ctx_s {
    ts_u32 u32_width;
    ts_u32 u32_height;
    ts_u32 u32_fps;
    ts_u32 u32_isp_clk;
} sim_ctx_s_t;

#ifdef CONFIG_SNR_PLANCK1280C
static sim_ctx_s_t sst_ctx = {
    .u32_width = 1280,
    .u32_height = 1024,
    .u32_fps = 25,
    .u32_isp_clk = 0
};
#else
static sim_ctx_s_t sst_ctx = {
    .u32_width = 640,
    .u32_height = 512,
    .u32_fps = 50,
    .u32_isp_clk = 0
};
#endif
ts_void sim_set_cfg(ts_u32 u32_width, ts_u32 u32_height, ts_u32 u32_fps, ts_u32 u32_clk)
{
    sst_ctx.u32_width = u32_width;
    sst_ctx.u32_height = u32_height;
    sst_ctx.u32_fps = u32_fps;

    /* 计算 isp 时钟 */
    if (0 == u32_clk)
        sst_ctx.u32_isp_clk = SNR_CALC_ISP_CLK(180000000, u32_width, u32_height, u32_fps);
    else
        sst_ctx.u32_isp_clk = u32_clk;
}

static ts_void viu_init_mem(ts_u16 u16_imgW, ts_u16 u16_imgH, ts_u16 u16_fps)
{
    ts_viu_comm_attr_s_t st_comm_attr;

    st_comm_attr.en_snr = TS_VIU_SNRT_MEM;
    st_comm_attr.u16_height = u16_imgH;
    st_comm_attr.u16_width = u16_imgW;
    st_comm_attr.u08_pixel_ratio = 1;
    st_comm_attr.f32_fps = u16_fps;
    st_comm_attr.u16_rst_dly_us = 0;

    ts_tdk_viu_set_comm_attr(TS_VIU_MDEV0, (const ts_viu_comm_attr_s_t *)&st_comm_attr);
}


static ts_void clkm_init_mem(ts_void)
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
    //st_clk_cfg.u32_isp_clk_hz = 20000000;
    //st_clk_cfg.u32_isp_clk_hz = 45000000;
    st_clk_cfg.u32_isp_clk_hz = sst_ctx.u32_isp_clk;

    /* vpo module */
    pst_vpo_clk->en_vpo_clk_src = TS_CLKM_SRC_VPO_PLL;
    pst_vpo_clk->u32_vpo_clk_hz = 45000000;

    /* vpo btenv */
    pst_vpo_clk->u32_btenc_pix_clk_hz = 45000000;
    pst_vpo_clk->u32_btenc_mod_clk_hz = 45000000;
    pst_vpo_clk->u32_btenc_o_clk_inv = 1;

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


ts_void start_viu_mem(ts_u32 u32_param)
{
    snr_attr_s_t *pst_attr;
    snr_ctx_s_t *pst_ctx;

    pst_ctx = (snr_ctx_s_t *)u32_param;
    pst_attr = (snr_attr_s_t *)&pst_ctx->st_attr;

    ts_cpl_insmod(clkm, "pllsys=100000000 pllddr=250000000 pllviu=162000000 pllisp=180000000 pllvpo=180000000");

    pst_attr->s32_img_w = (ts_u32)sst_ctx.u32_width;
    pst_attr->s32_img_h = (ts_u32)sst_ctx.u32_height;
    pst_attr->s32_fps = (ts_u32)sst_ctx.u32_fps;
    pst_attr->s32_vo_img_w = pst_attr->s32_img_w;
    pst_attr->s32_vo_img_h = pst_attr->s32_img_h;

    /* npu 功能 */
    pst_attr->st_npu.en_ops = TS_NPU_OPS_DISABLE;
    pst_attr->st_npu.u32_clk = 0;

    /* 是否开启开机画面 */
    pst_attr->u32_pod_dly_ms = 0;

    /* cvbs 通道配置 */
    pst_attr->st_cvbs.u08_en = 1;
    pst_attr->st_cvbs.u08_sd_en = 0;
    pst_attr->st_cvbs.u08_orgi_en = 0;
    pst_attr->st_cvbs.u08_pal_en = 1;

    /* mem 通道配置 */
    pst_attr->u32_mem_en = 1;

    pst_attr->u08_raw_oe = 0;
    pst_attr->u08_raw_pos = TS_LUT_POS_FINAL_RAW;
    pst_attr->u08_y_oe = 1;

    clkm_init_mem();
    snr_insmod_all_tdk(0);

    viu_init_mem(sst_ctx.u32_width, sst_ctx.u32_height, (ts_u16)sst_ctx.u32_fps);

    snr_cfg_vpo(pst_attr);

    /* 开启 isp */
#if 1
    snr_isp_init(pst_attr->s32_img_w, pst_attr->s32_img_h, pst_attr->s32_fps, ts_null);
#else
    isp_demo();
#endif

    ts_tdk_viu_enable(TS_VIU_MDEV0);
}

TS_DECLARE_CMD(sim)
{
    ts_u32 u32_width, u32_height, u32_fps, u32_clk;

    if (argc >= 4) {
        /* 仿真不做参数检测，由用户保证 */
        u32_width = atoi(argv[1]);
        u32_height = atoi(argv[2]);
        u32_fps = atoi(argv[3]);

        if (4 == argc) {
            u32_clk = 0;
        } else {
            u32_clk = atoi(argv[4]);
        }
    } 
#ifdef CONFIG_SNR_PLANCK1280C
 else  // if (get_dector_resolution_type()==DECTOR_RESOLUTION_1280X1024) 
    {
        /* 使用默认配置 1280*1024@25 */
        u32_width = 1280;
        u32_height = 1024;
        u32_fps = 25;
        u32_clk = 0;
    }
#else
    else {
        /* 使用默认配置 640*512@50 */
        u32_width = 640;
        u32_height = 512;
        u32_fps = 50;
        u32_clk = 0;
    }
#endif
    sim_set_cfg(u32_width, u32_height, u32_fps, u32_clk);

    /* 软仿 */
    start_isp("viu_mem", 1, 0, 0);

    return 0;
}

snr_register(viu_mem, start_viu_mem);


/*
 * iary_comm.c -- vision iray 公共代码实现
 */
#include <base/ts_type.h>
#include <base/ts_compiler.h>
#include <cpl/ts_cpl.h>
#include <uapi/chip_def.h>
#include "api.h"
#include "snr_comm.h"

#include <base/ts_type.h>
#include <base/ts_compiler.h>
#include <cpl/ts_cpl.h>
#include <uapi/chip_def.h>
#include <uapi/gpio_v100.h>
#include <uapi/aisp_v100.h>
#include <stdio.h>
#include "api.h"
#include "snr_comm.h"
#include "./rtd/iray_comm.h"
#include "menu/votmenu.h"
#include <menu/ispfunction.h>

//smartdata： 4049C000 9FFFFFE8 B688CDB5 6F1DD08C 9A92C93B C44F0470 417E5C18 C93C
/* 30HZ */
// static ts_viu_snr_cfg_s_t sast_rtd123c_cfg[] = {
//     {0x0000001f, 0x4049C000},
//     {0x0000001e, 0x9FFF27E8},
//     {0x0000001d, 0xB688CD85},
//     {0x0000001c, 0x6F1DD08C},
//     {0x0000001b, 0x9A92C937},
//     {0x0000001a, 0xCC2F02F0},
//     {0x00000019, 0x417E5C18},
//     {0x00000018, 0xC93C0000},
// };

static ts_viu_snr_cfg_s_t sast_rtd123c_cfg[] = {
    {0x0000001f, 0x4049C000},
    {0x0000001e, 0x9FFFFFE8},
    {0x0000001d, 0xB688CDB5},
    {0x0000001c, 0x6F1DD08C},
    {0x0000001b, 0x9A92C93B},
    {0x0000001a, 0xC44F0470},
    {0x00000019, 0x417E5C18},
    {0x00000018, 0xC93C0000},
};

static ts_viu_snr_attr_s_t sst_rtd123c_attr = {
    .u16_uint_len = 240 - 1,
    .u16_cfg_num = sizeof(sast_rtd123c_cfg) / sizeof(ts_viu_snr_cfg_s_t),
    .pst_cfg = (ts_viu_snr_cfg_s_t *)&sast_rtd123c_cfg,
};


//static 
ts_void brd_rtd123c_init(ts_void)
{
    tdk_sensor_cfg_s_t st_snr_io_cfg;

    ts_lib_gpio_set_mux(TS_IO_SR_OUT_DAT6, TS_IO_SR_OUT_DAT6_FUN3_GPIO45);//SHTCTL0
    ts_lib_gpio_set_mux(TS_IO_SPI1_CSN0, TS_IO_SPI1_CSN0_FUN3_GPIO24);//SHTCTL1
    ts_lib_gpio_set_mux(TS_IO_SPI1_CLK, TS_IO_SPI1_CLK_FUN3_GPIO21);//AVDD_EN/VDET_EN1
    ts_lib_gpio_set_mux(TS_IO_SPI1_MISO, TS_IO_SPI1_MISO_FUN3_GPIO23);//VDET_EN2
    ts_lib_gpio_set_mux(TS_IO_SR_SEQ_SYNC, TS_IO_SR_SEQ_SYNC_FUN3_GPIO46);//POWER_ON

    //VDET_CTRL set to high
    snr_gpio_cfg2out(23, TS_ELEC_LVL_HI);

    //AVDD&VDET EN
    snr_gpio_cfg2out(21, TS_ELEC_LVL_HI);

    /*******************
    ** IO DRIVER LEVEL
    *******************/
    ts_lib_gpio_set_o_strong(TS_IO_SR_CLK_OUT, 3);
    ts_lib_gpio_set_o_strong(TS_IO_SR_OUT_DAT0, 3);
    ts_lib_gpio_set_o_strong(TS_IO_SR_OUT_DAT1, 3);
    ts_lib_gpio_set_o_strong(TS_IO_SR_SEQ_SYNC, 3);

    /* 配置探测器复位、电源、开门的 IO 控制信息 */
    st_snr_io_cfg.s32_pwr_gpio_num = -1;
    st_snr_io_cfg.s32_rst_gpio_num = 46;
    st_snr_io_cfg.en_reset_lvl = TS_ELEC_LVL_LO;
    st_snr_io_cfg.u32_rst_dly = 100000;

    st_snr_io_cfg.s32_sht_p_gpio_num = 45;
    st_snr_io_cfg.s32_sht_n_gpio_num = 24;
    st_snr_io_cfg.u32_sht_dbg = 1;
    st_snr_io_cfg.u32_sht_hold = 60000;

    tdk_snrcfg_init((tdk_sensor_cfg_s_t *)&st_snr_io_cfg);
    tdk_snrcfg_pwr_on();
    tdk_snrcfg_reset();
}

static ts_void start_rtd123c_30hz(ts_u32 u32_param)
{
    snr_attr_s_t *pst_attr;
    snr_ctx_s_t *pst_ctx;
    ts_void *pv_isp_cfg_to;
    ts_isp_nuc_attr_s_t st_nuc_attr;
    ts_isp_sffc_attr_s_t sffc_attr;
    ts_viu_corr_rst_s_t st_rst_attr;
    ts_viu_comm_attr_s_t st_comm_attr;

    pst_ctx = (snr_ctx_s_t *)u32_param;
    pst_attr = (snr_attr_s_t *)&pst_ctx->st_attr;
    pst_attr->get_temperature = rtd123c_get_temperature;
    pst_attr->s32_img_w = (ts_s32)1280;
    pst_attr->s32_img_h = (ts_s32)1024;
    pst_attr->s32_fps = (ts_f32)30;
    pst_attr->s32_vo_img_w = (ts_s32)1280;
    pst_attr->s32_vo_img_h = (ts_s32)1024;

    /* 是否开启开机画面 */
    pst_attr->u32_pod_dly_ms = 1000;

    /* 是否开启 raw 同传 */
    pst_attr->u08_y_oe = 1;
    pst_attr->u08_raw_oe = 0;
    pst_attr->u08_raw_pos = 0;

    /* npu 设置 */
    pst_attr->st_npu.en_ops = TS_NPU_OPS_DISABLE;
    pst_attr->st_npu.u32_clk = 0;

    /* cvbs 通道配置 */
    pst_attr->st_cvbs.u08_en = 1;
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

    /* 加载时钟模块 */
    ts_cpl_insmod(clkm, "pllsys=100000000 pllddr=250000000 pllviu=170000000 pllisp=180000000 pllvpo=180000000");

    rtd123c_clkm_init(pst_attr);

    snr_insmod_all_tdk(0);

    // brd_rtd123c_init();

    snr_init_smart(sast_rtd123c_cfg, sizeof(sast_rtd123c_cfg));
    rtd123c_viu_init(pst_attr, (ts_viu_snr_attr_s_t *)&sst_rtd123c_attr);

    ts_tdk_viu_get_comm_attr(TS_VIU_MDEV0,&st_comm_attr);
    st_comm_attr.u32_o_order_lo = 0x76543201;
    ts_tdk_viu_set_comm_attr(TS_VIU_MDEV0,&st_comm_attr);

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
    snr_init_ooc();

    st_nuc_attr.s32_en = TS_ENABLE;
    st_nuc_attr.s32_cutoff = TS_DISABLE;
    st_nuc_attr.en_mode = TS_ISP_NUC_MODE_GAIN_OFFSET; 
    st_nuc_attr.en_stat_mode = 1;  
    st_nuc_attr.u08_stat_frm_num = 15;
    st_nuc_attr.u16_mean_val = 0;
    st_nuc_attr.u16_coef = 0;
    st_nuc_attr.u08_stat_fack_frm2_en = 0;
    st_nuc_attr.u16_frm_oft = 0;
    st_nuc_attr.u08_bbuf_dbl = 0;
    st_nuc_attr.u08_dbuf_dbl = 0;

    snr_init_nuc((ts_isp_nuc_attr_s_t *)&st_nuc_attr);


    sffc_attr.u08_enable = 0;
    sffc_attr.u08_en_man = 0;
    sffc_attr.u08_upd_speed = 32;
    sffc_attr.u16_flt_thr = 2560;
    sffc_attr.u16_man_cor_v = 3000;
    snr_init_sffc((ts_isp_sffc_attr_s_t *)&sffc_attr);

    // ts_cpl_kthread_create(ts_null, (ts_cpl_kthread_t)snr_auto_cut_process, ts_null, "auto_cut", 5, 4096);

}

snr_register(rtds123c, start_rtd123c_30hz);
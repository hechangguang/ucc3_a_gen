/* 
 * api_clkm.h -- clkm 用户接口头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2024 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __API_CLKM_H__
#define __API_CLKM_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <config.h>
#include <base/ts_type.h>

typedef enum ts_clkm_src_en {
    TS_CLKM_SRC_ISP_CLK = 0,
    TS_CLKM_SRC_VIU_PLL = 1,
    TS_CLKM_SRC_VPO_PLL = 2,
    TS_CLKM_SRC_CVBS_PLL = 3,
    TS_CLKM_SRC_EXT_CLK = 4,
    TS_CLKM_SRC_LCD_PIX_CLK = 5,
    TS_CLKM_SRC_LCD_MOD_CLK = 6,
    TS_CLKM_SRC_VIU_SNR_CLK = 7,

    TS_CLKM_SRC_BUTT
} ts_clkm_src_en_t;



/* viu 时钟配置 */
typedef struct ts_clkm_viu_cfg_s {
    /* viu 模块时钟 */
    ts_u32 u32_clk_hz;

    /* 仅限于 TS_CLKM_SRC_ISP_CLK 和 TS_CLKM_SRC_VIU_PLL */
    ts_clkm_src_en_t en_clk_src;

    /* viu 对接探测器时钟模块 */
    ts_u32 u32_snr_clk;
    ts_u32 u32_snr_internal_clk_en;

    ts_u32 u32_snr_o_clk_phase;
    ts_u32 u32_snr_o_clk_inv;

    ts_u32 u32_sample_clk_en;

    /* 仅限于 TS_CLKM_SRC_VIU_SNR_CLK 和 TS_CLKM_SRC_EXT_CLK */
    ts_clkm_src_en_t en_sample_clk_src;
    ts_u32 u32_sample_clk_phase;
    ts_u32 u32_sample_clk_inv;
} ts_clkm_viu_cfg_s_t;

/* vpo 时钟配置 */
typedef struct ts_clkm_vpo_cfg_s {
    /* 仅限于 TS_CLKM_SRC_ISP_CLK 和 TS_CLKM_SRC_LCD_PLL */
    ts_clkm_src_en_t en_vpo_clk_src;

    /* vpo 模块时钟, 如果为 0 , 则关闭时钟  */
    ts_u32 u32_vpo_clk_hz;

    /* --- btenc 通道 ---- */

    /* btenc 像素时钟, 如果为 0 , 则关闭时钟 */
    ts_u32 u32_btenc_pix_clk_hz;

    /* btenc 模块时钟, 如果为 0 , 则关闭时钟 */
    ts_u32 u32_btenc_mod_clk_hz;

    /* btenc 输出像素时钟到 IO，及是否取反, 0: 不取反; 1: 取反 */
    ts_u32 u32_btenc_o_clk_inv;

    /* --- cvbs 通道 --- */

    /* 是否使能 cvbs 时钟 */
    ts_u32 u32_cvbs_clk_en;
    ts_u32 u32_dac_clk_en;

    /* 仅限于 TS_CLKM_SRC_CVBS_PLL 和 TS_CLKM_SRC_EXT_CLK */
    ts_clkm_src_en_t en_dac_clk_src;

    /* dac 时钟是否取反, 0: 不取反; 1: 取反  */
    ts_u32 u32_dac_clk_inv;

    /* --- lcd 通道 --- */

    /* lcd 像素时钟 , 如果为 0 , 则关闭时钟 */
    ts_u32 u32_lcd_pix_clk_hz;

    /* lcd 模块时钟 , 如果为 0 , 则关闭时钟 */
    ts_u32 u32_lcd_mod_clk_hz;

    /* lcd 输出时钟源选择, 限于 TS_CLKM_SRC_LCD_BT_CLK 和 TS_CLKM_SRC_LCD_CLK */
    ts_clkm_src_en_t en_lcd_o_clk_src;

    /* lcd 时钟输出到 IO 上使能和取反开关 */

    ts_u32 u32_lcd_o_clk_en;
    ts_u32 u32_lcd_o_clk_inv;

    /* --- mipi 通道 ---*/

    /* 是否输出 MIPI DPHY 参考时钟 */
    ts_u32 u32_mipi_dhpy_ref_clk_en;
} ts_clkm_vpo_cfg_s_t;


/* 各个模块时钟配置 */
typedef struct ts_clkm_cfg_s {
    /* viu 相关时钟配置 */
    ts_clkm_viu_cfg_s_t st_viu_clk;

    /* isp 相关时钟配置 */
    ts_u32 u32_isp_clk_hz;

    /* npu 相关时钟配置 */
    ts_u32 u32_npu_clk_hz;

    /* vpo 相关时钟配置 */
    ts_clkm_vpo_cfg_s_t st_vpo_clk;
} ts_clkm_cfg_s_t;


ts_s32 ts_tdk_clkm_set_cpu(ts_u32 u32_clk_hz, ts_u32 u32_cpu_index);
ts_s32 ts_tdk_clkm_set(const ts_clkm_cfg_s_t *pst_clk_cfg);
ts_s32 ts_tdk_clkm_get(ts_clkm_cfg_s_t *pst_clk_cfg);

ts_s32 ts_tdk_clkm_set_isp(ts_clkm_cfg_s_t *pst_cfg);
ts_s32 ts_tdk_clkm_get_isp(ts_u32 *pu32_clk_hz);

ts_s32 ts_tdk_clkm_set_viu(const ts_clkm_viu_cfg_s_t *pst_clk_cfg);
ts_s32 ts_tdk_clkm_get_viu(ts_clkm_viu_cfg_s_t *pst_clk_cfg);

ts_s32 ts_tdk_clkm_set_vpo(const ts_clkm_vpo_cfg_s_t *pst_clk_cfg);
ts_s32 ts_tdk_clkm_get_vpo(ts_clkm_vpo_cfg_s_t *pst_clk_cfg);

ts_s32 ts_tdk_clkm_get_pll(ts_u32 *pu32_isp_pll, ts_u32 *pu32_viu_pll, ts_u32 *pu32_vpo_pll);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __API_CLKM_H__ */

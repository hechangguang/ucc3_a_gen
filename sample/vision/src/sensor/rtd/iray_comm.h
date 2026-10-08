/*
 * iray_comm.h -- vision iray 公共头文件
 */
#ifndef __IRAY_COMM_H__
#define __IRAY_COMM_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <base/ts_type.h>
#include <uapi/chip_def.h>
#include <uapi/viu_v100.h>

/* comm */

/* rtc3123 */
ts_s32 rtc3123_get_temperature(ts_void);
ts_void rtc3123_clkm_init(snr_attr_s_t *pst_attr, ts_u32 u32_mode_sel);
ts_void rtc3123_viu_init(snr_attr_s_t *pst_attr, ts_viu_snr_attr_s_t *pst_snr_attr);
ts_void rtc3123_mode0_viu_init(snr_attr_s_t *pst_attr, ts_viu_snr_attr_s_t *pst_snr_attr);
ts_void rtc3123_mode0_sensor_init(ts_f32 f32_fps);
ts_void rtc3123_auto_cut_process(ts_void);

/* rtd6122c */
ts_s32 rtd6122_get_temperature(ts_void);
ts_void rtd6122_clkm_init(snr_attr_s_t *pst_attr, ts_u32 u32_snr_clk, ts_u32 u32_btenc_pix_clk_hz);
ts_void rtd6122_viu_init(snr_attr_s_t *pst_attr, ts_viu_snr_attr_s_t *pst_snr_attr, \
                            ts_viu_snr_timing_attr_s_t *pst_tim_attr, snr_remap_s_t *pst_snr_remap);
ts_void rtd6122_auto_cut_process(ts_void);

/* rtd3122 */
ts_s32 rtd3122_get_temperature(ts_void);
ts_void rtd3122_clkm_init(snr_attr_s_t *pst_attr, ts_u32 u32_snr_clk, ts_u32 u32_btenc_pix_clk_hz);
ts_void rtd3122_viu_init(snr_attr_s_t *pst_attr, ts_viu_snr_attr_s_t *pst_snr_attr, \
                            ts_viu_snr_timing_attr_s_t *pst_tim_attr, snr_remap_s_t *pst_snr_remap);
ts_void rtd3122_snr_auto_cut_process(ts_void);

/* rtc6081 */
ts_s32 rtc6081_get_temperature(ts_void);
ts_s32 rtc6081_smartdata_corr(ts_viu_snr_attr_s_t *pst_snr_attr);
ts_void rtc6081_clkm_init(snr_attr_s_t *pst_attr);
ts_void rtc6081_viu_init(snr_attr_s_t *pst_attr, ts_viu_snr_attr_s_t *pst_snr_attr);
ts_void rtc6081_auto_cut_process(ts_void);

/* rtd123 */
ts_void rtd123c_clkm_init(snr_attr_s_t *pst_attr);
ts_void rtd123c_viu_init(snr_attr_s_t *pst_attr, ts_viu_snr_attr_s_t *pst_snr_attr);
ts_s32 rtd123c_get_temperature(ts_void);
ts_void rtds123_snr_auto_cut_process(ts_void);
ts_u32 rtds123_snr_corr_mean(ts_void);

/* elf256 */
ts_void elf256_clkm_init(snr_attr_s_t *pst_attr);
ts_void elf256_viu_init(snr_attr_s_t *pst_attr, ts_viu_snr_attr_s_t *pst_snr_attr);
ts_s32 elf256_get_temperature(ts_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __IRAY_COMM_H__ */


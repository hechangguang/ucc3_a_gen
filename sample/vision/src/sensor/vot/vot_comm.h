/*
 * vot_comm.h -- vision vot 公共头文件
 */
#ifndef __VOT_COMM_H__
#define __VOT_COMM_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <base/ts_type.h>
#include <uapi/chip_def.h>
#include <uapi/viu_v100.h>
#include <planck640c_vot.h>
#include <planck1280c_vot.h>
/* for planck640 */
ts_s32 planck640c_get_temperature(ts_void);
ts_u32 planck640c_smartdata_corr(ts_void);
ts_void planck640c_clkm_init(ts_void);
ts_void planck640c_viu_init(snr_attr_s_t *pst_attr, ts_viu_snr_attr_s_t *pst_snr_attr);
ts_void planck640c_auto_cut_process(ts_void);

/* for planck384 */
ts_s32 planck384c_get_temperature(ts_void);
ts_u32 planck384c_smartdata_corr(ts_void);
ts_void planck384c_clkm_init(ts_void);
ts_void planck384c_viu_init(snr_attr_s_t *pst_attr, ts_viu_snr_attr_s_t *pst_snr_attr);
ts_void planck384c_auto_cut_process(ts_void);

/* for planck1280 */
ts_s32 planck1280c_get_temperature(ts_void);
ts_u32 planck1280c_smartdata_corr(ts_void);
ts_void planck1280c_clkm_init(ts_void);
ts_void planck1280c_viu_init(snr_attr_s_t *pst_attr, ts_viu_snr_attr_s_t *pst_snr_attr);
//ts_void planck1280c_auto_cut_process(ts_void);


/* for rv1280 */
// ts_void rv1280_brd_init(ts_void);
// ts_void rv1280_brd_fix(ts_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __VOT_COMM_H__ */


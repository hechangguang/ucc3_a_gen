/*
 * sl.h -- 温补 接口等声明头文件
 */
#ifndef __SL_H__
#define __SL_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif

#endif /* __cplusplus */
#include <base/ts_type.h>
#include <base/ts_comm.h>
#include <base/ts_bitop.h>
#include <cpl/ts_cpl.h>


#define TCORR_MAX_TPL_NUM         (12)

ts_s32 ts_sl_init(ts_void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __SL_H__ */


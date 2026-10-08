/* 
 * cpl_fops.h -- 内核态文件的操作实现 cpl 头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __CPL_FOPS_H__
#define __CPL_FOPS_H__

#define TS_CPL_RO           (0)
#define TS_CPL_WO           (1)
#define TS_CPL_RDWR         (2)
#define TS_CPL_CREAT        (100)

extern ts_void *ts_cpl_klib_fopen(const ts_char *, ts_s32, ts_s32);
extern ts_s32 ts_cpl_klib_fread(ts_char *, ts_s32, ts_void *);
extern ts_s32 ts_cpl_klib_fwrite(ts_char *, ts_s32, ts_void *);
extern ts_void ts_cpl_klib_set_fpos(ts_void *, ts_loff_t);
extern ts_loff_t ts_cpl_klib_get_fpos(ts_void *);
extern ts_void ts_cpl_klib_fclose(ts_void *);

#endif /* __CPL_FOPS_H__ */

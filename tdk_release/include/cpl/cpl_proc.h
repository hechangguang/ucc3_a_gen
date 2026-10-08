/* 
 * cpl_proc.h -- cpl proc 实现头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2022-2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __CPL_PROC_H__
#define __CPL_PROC_H__

#include <base/ts_type.h>

typedef struct ts_cpl_proc_s {
    const ts_char *pc_name;
    ts_cpl_fileops_s_t *pst_fops;
	ts_void *pv_entry;
} ts_cpl_proc_s_t;

typedef struct ts_cpl_seq_ops_s {
    ts_void * (*start) (ts_void *m, ts_loff_t *pos);
    ts_void (*stop) (ts_void *m, ts_void *v);
    ts_void * (*next) (ts_void *m, ts_void *v, ts_off_t *pos);
    ts_s32 (*show) (ts_void *m, ts_void *v);
} ts_cpl_seq_ops_s_t;

extern ts_s32 ts_cpl_seq_open(ts_void *pv_file, ts_cpl_seq_ops_s_t *pst_seq_ops);
extern ts_s32 ts_cpl_seq_read(ts_void *pv_file, char *buf, ts_size_t size, ts_off_t *ppos);
extern ts_loff_t ts_cpl_seq_lseek(ts_void *pv_file, ts_off_t offset, ts_s32 whence);
extern ts_s32 ts_cpl_seq_release(ts_void *inode, ts_void *pv_file);
extern ts_void ts_cpl_seq_printf(ts_void *m, const ts_char *f, ...);
extern ts_s32 ts_cpl_single_release(ts_void *inode, ts_void *pv_file);


/* proc */
extern ts_s32 ts_cpl_create_proc(ts_cpl_proc_s_t *pst_proc, const ts_char *pc_proc_name);
extern ts_void ts_cpl_remove_proc(ts_cpl_proc_s_t *pst_proc);

#endif /* __CPL_PROC_H__ */

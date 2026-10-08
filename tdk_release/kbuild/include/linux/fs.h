/*
 * fs.h -- tsp 文件系统头文件,使用为 linux 版本的简化代码, 并参考 nuttx 结构
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2024 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */

#ifndef __FS_H__
#define __FS_H__

#include <config.h>
#include <base/ts_type.h>
#include <cpl/cpl_list.h>
#include <linux/module.h>
#include <linux/poll.h>

struct file_operations;
struct file;
struct inode;


/* 简化文件系统仅采用单 inode 方式 */
struct inode {
    ts_cpl_list_head_s_t st_list;

    const ts_char *i_name;

    /* 文件名大小 */
    ts_u08 i_name_len;

    /* References to inode */
    ts_u08 i_refs;

    /* devfs, procfs 等文件系统识别标志 */
    ts_u16 i_flags;

    /* operations for inode */
    ts_void *i_ops;

    /* Per inode driver private data */
    ts_void *i_priv;
};


struct file {
    ts_cpl_list_head_s_t st_list;
    int f_num;

    /* open 模式 */
    int f_oflgas;

    /* for llseek */
    ts_loff_t f_pos;

    /* inode, 每个设备或者 proc 占用一个节点 */
    struct inode *f_inode;

    /* Per file driver private data */
    ts_void *f_priv;
};

struct file_operations {
    ts_loff_t (*llseek) (struct file *, ts_loff_t, int);
    ts_ssize_t (*read) (struct file *, char *, ts_size_t, ts_loff_t *);
    ts_ssize_t (*write) (struct file *, const char *, ts_size_t, ts_loff_t *);
    long (*unlocked_ioctl) (struct file *, unsigned int, unsigned long);
    int (*open) (struct inode *, struct file *);
    int (*release) (struct inode *, struct file *);
    int (*poll)(struct file *, struct pollfd *fds, ts_bool setup);
};

ts_s32 tsp_fs_fncmp(const char *cs, const char *ct, ts_size_t count);
ts_char *tsp_fs_trim_space(const ts_char *pc_path, ts_u32 *pu32_max_len);

ts_s32 tsp_fs_create_inode(const ts_char *pc_path, const struct file_operations *pst_fops, ts_void *pv_priv);
ts_s32 tsp_fs_destory_inode(const ts_char *pc_path);

struct file *tsp_fs_get_file(ts_s32 s32_fd);

_ts_init_ ts_s32 tsp_fs_init(ts_u32 u32_max_files, ts_u32 u32_max_inodes);




#endif /* __FS_H__ */


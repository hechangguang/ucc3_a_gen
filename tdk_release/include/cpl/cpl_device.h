/* 
 * cpl_device.h -- cpl 设备实现头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2022-2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __CPL_DEVICE_H__
#define __CPL_DEVICE_H__

#include <base/ts_type.h>
#include <base/ts_errno.h>

#include <linux/module.h>

#define TS_CPL_DYNAMIC_MAJOR        (0)
#define TS_CPL_DYNAMIC_MINOR        (0)


#define ts_cpl_module_driver(ts_cpl_driver, ts_cpl_register, ts_cpl_unregister)   \
    static ts_s32 __init ts_cpl_driver##_init(ts_void)                             \
    {                                                                      \
        return ts_cpl_register(&(ts_cpl_driver));                              \
    }                                                                      \
    module_init(ts_cpl_driver##_init);                                       \
    static ts_void __exit ts_cpl_driver##_exit(ts_void)                            \
    {                                                                      \
        ts_cpl_unregister(&(ts_cpl_driver));                                  \
    }                                                                      \
    module_exit(ts_cpl_driver##_exit);

#define ts_cpl_module_init(x)       module_init(x)
#define ts_cpl_module_exit(x)       module_exit(x)

#define ts_cpl_module_param(name, type, perm)      module_param(name, type, perm)

#define ts_cpl_module_platform_driver(platform_driver)                           \
                    ts_cpl_module_driver(platform_driver, ts_cpl_platform_driver_register,     \
                                                    ts_cpl_platform_driver_unregister)

/* TODO implement the parameter setting and acquisition of poll table */
typedef struct ts_cpl_poll_table_s {
    ts_void *pv_poll_table;
    ts_void *pv_data;
} ts_cpl_poll_table_s_t;

/* TODO implement the parameter setting and acquisition of vm area */
typedef struct ts_cpl_vm_area_s {
    ts_void *pv_vmarea;
} ts_cpl_vm_area_s_t;

typedef struct ts_cpl_fileops_s {
    ts_s32 (*open)(ts_void *inode, ts_void *file);
    ts_ssize_t (*read)(ts_void *file, char *, ts_size_t, ts_loff_t *);
    ts_ssize_t (*write)(ts_void *file, const ts_char *, ts_size_t, ts_loff_t *);
    ts_u32 (*poll)(ts_void *file, ts_cpl_poll_table_s_t *);
    ts_loff_t (*llseek)(ts_void *file, ts_loff_t, ts_s32);
    ts_s32 (*release)(ts_void *inode, ts_void *file);
    ts_long (*unlocked_ioctl)(ts_void *file, ts_s32, ts_ulong);
    ts_s32 (*mmap)(ts_void *file, ts_cpl_vm_area_s_t *);
    ts_s32 (*fasync)(ts_s32, ts_void *file, ts_s32);
} ts_cpl_fileops_s_t;

typedef struct ts_cpl_dev_s {
    const ts_char *pc_name;
    ts_cpl_fileops_s_t *pst_cpl_fops;
    ts_void *pv_fops;
    ts_void *pv_private;
} ts_cpl_dev_s_t;


extern ts_cpl_dev_s_t *ts_cpl_dev_register(ts_char *pc_dev_name, \
                            ts_cpl_fileops_s_t *pst_cpl_fops, ts_u32 u32_priv_size);
extern ts_s32 ts_cpl_dev_unregister(ts_cpl_dev_s_t *pv_dev);

extern ts_s32 ts_cpl_set_file_private(ts_void *pv_file, ts_void *pv_priv);
extern ts_s32 ts_cpl_get_file_private(ts_void *pv_file, ts_void **ppv_priv);
extern ts_cpl_dev_s_t *ts_cpl_get_dev(ts_void *pv_file);
extern ts_void *ts_cpl_get_dev_priv(ts_void *pv_file);

//extern ts_void ts_cpl_poll_wait(ts_void *filep, ts_cpl_poll_t *table, ts_cpl_wait_t *ts_cpl_wait);
//extern ts_void ts_cpl_poll_wait(ts_void *filep, struct ts_cpl_poll_table *table, ts_cpl_wait_t *ts_cpl_wait);

extern ts_void insmod_parse_param(ts_char *pc_mod_name, ts_char *pc_mod_args);

#define ts_cpl_insmod(name, param) 	\
        extern ts_s32 name##_init(ts_void); \
        insmod_parse_param(#name, param);    \
        name##_init()

#define insmod          ts_cpl_insmod

#ifdef CONFIG_TDK_INSMOD
#define ts_cpl_ins_module_init(mod_name, init_call)                 ins_module_init(mod_name, init_call)
#define ts_cpl_ins_module_param(mod_name, param_name, type, perm)   ins_module_param(mod_name, param_name, type, perm)
#else
#define ts_cpl_ins_module_init(mod_name, init_call)                 module_init(init_call)
#define ts_cpl_ins_module_param(mod_name, param_name, type, perm)   module_param(param_name, type, perm)
#endif

#ifndef ioctl
extern int ioctl(int fd, int req, ...);
#endif

#endif /* __CPL_DEVICE_H__ */

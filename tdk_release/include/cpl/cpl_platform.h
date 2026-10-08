/* 
 * cpl_platform.h -- cpl 层平台设备头文件,为兼容 linux ，内部变量保持一致
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __CPL_PLATFORM_H__
#define __CPL_PLATFORM_H__

#include <base/ts_type.h>
#include <linux/module.h>

#define TS_CPL_IORESOURCE_MEM       (0x00000200)
#define TS_CPL_IORESOURCE_REG       (0x00000300)
#define TS_CPL_IORESOURCE_IRQ       (0x00000400)
#define TS_CPL_IORESOURCE_DMA       (0x00000800)
#define TS_CPL_IORESOURCE_BUS       (0x00001000)

#ifndef resource_size_t
typedef ts_u32 resource_size_t;
#endif

/* 避免申请内存和做设备管理，固定最多设备数量 */
#define CPL_MAX_DEVS                (4)

typedef struct ts_cpl_res_s {
    resource_size_t start;
    resource_size_t end;
    resource_size_t size;
} ts_cpl_res_s_t;

/* 简化设备 ID  */
typedef struct ts_cpl_of_devid_s {
    char compatible[32];
} ts_cpl_of_devid_s_t;


/* RTOS 内部使用 */
typedef struct ts_cpl_platdev_s {
    ts_s32 s32_id;
    ts_cpl_res_s_t *pst_res;
} ts_cpl_platdev_s_t;

typedef struct ts_cpl_dev_drv_s {
    const char *name;
    const ts_cpl_of_devid_s_t *of_match_table;

    /* rtos 内部使用 */
    ts_cpl_platdev_s_t st_plat_dev;
} ts_cpl_dev_drv_s_t;


typedef struct ts_cpl_pm_msg_s {
    int event;
} ts_cpl_pm_msg_s_t;

typedef struct ts_cpl_plat_drv_s {
    int (*probe)(ts_cpl_platdev_s_t *);
    int (*remove)(ts_cpl_platdev_s_t *);
    void (*shutdown)(ts_cpl_platdev_s_t *);
    int (*suspend)(ts_cpl_platdev_s_t *, ts_cpl_pm_msg_s_t state);
    int (*resume)(ts_cpl_platdev_s_t *);
    ts_cpl_dev_drv_s_t driver;
} ts_cpl_plat_drv_s_t;

extern ts_s32 ts_cpl_plat_driver_register(ts_cpl_plat_drv_s_t *);
extern ts_void ts_cpl_plat_driver_unregister(ts_cpl_plat_drv_s_t *);

extern ts_s32 ts_cpl_plat_get_reg(ts_cpl_platdev_s_t *, ts_cpl_res_s_t *, ts_u32);
extern ts_s32 ts_cpl_plat_get_irq(ts_cpl_platdev_s_t *, ts_u32);
extern ts_s32 ts_cpl_plat_get_irq_count(ts_cpl_platdev_s_t *);
extern ts_s32 ts_cpl_plat_get_clocks(ts_cpl_platdev_s_t *, ts_u32);
extern ts_s32 ts_cpl_plat_get_value(ts_cpl_platdev_s_t *, ts_char *, ts_u32, ts_u32);
extern ts_bool ts_cpl_plat_get_bool(ts_cpl_platdev_s_t *, ts_char *);

#endif /* __CPL_PLATFORM_H__ */

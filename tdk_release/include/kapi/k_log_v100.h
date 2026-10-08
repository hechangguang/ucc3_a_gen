/*
 * log_v100.h -- tdk log 驱动头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2024 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __K_LOG_V100_H__
#define __K_LOG_V100_H__

#include <config.h>
#include <base/ts_type.h>
#include <cpl/ts_cpl.h>


typedef struct log_attr_s log_attr_s_t;

/* ioctl */
typedef enum ts_log_ioctl_en {
    IOC_NR_LOG_INIT = 0,

    IOC_NR_LOG_FLUSH = 1,

    IOC_NR_LOG_CLEAR = 2,

    IOC_NR_LOG_READ = 3,

    IOC_NR_GPIO_BUTT,
} ts_log_ioctl_en_t;

#define TS_IOCTRL_IOC_NR_LOG_INIT                   TS_CPL_IOW(TS_IOC_MAGIC_LOG, IOC_NR_LOG_INIT, log_attr_s_t)
#define TS_IOCTRL_IOC_NR_LOG_FLUSH                  TS_CPL_IOW(TS_IOC_MAGIC_LOG, IOC_NR_LOG_FLUSH, ts_u32)
#define TS_IOCTRL_IOC_NR_LOG_CLEAR                  TS_CPL_IOW(TS_IOC_MAGIC_LOG, IOC_NR_LOG_CLEAR, ts_u32)
#define TS_IOCTRL_IOC_NR_LOG_READ                   TS_CPL_IOW(TS_IOC_MAGIC_LOG, IOC_NR_LOG_READ, ts_u32)


#define FLASH_PAGE_MAX_SIZE                         (2048)
#define LOG_SLOT_MAX_SIZE                           (512)
#define LOG_PAYLOAD_MAX                             (LOG_SLOT_MAX_SIZE - 4)

typedef struct log_iter {
    ts_u32 u32_units_total;
    ts_u32 au32_unit_order[8];
    ts_u32 u32_unit_pos;
    ts_u32 u32_next_slot;
    ts_u08 *pu08_page_buf;
    ts_bool b_page_loaded;
    ts_u32 u32_iter_phys_unit;
    ts_u32 u32_iter_page_in_unit;
    ts_u32 u32_unit_data_end;
} log_iter;


typedef struct log_attr_s {
    /* log区域总大小 */
    ts_u32 u32_region_size;

    /* flash页大小 */
    ts_u32 u32_page_size;

    /* flash 擦除单元大小 */
    ts_u32 u32_erase_unit_size;

    /* 槽大小 */
    ts_u32 u32_slot_size;

    /* flash读写文件句柄 */
    ts_s32 s32_hdl;
} log_dev_s_t;


typedef struct log_unit_hdr_s {
    ts_u32 u32_magic;
    ts_u16 u16_version;
    ts_u16 u16_header_size;
    ts_u32 u32_unit_seq;
    ts_u32 u32_reserved;
    ts_u08 au08_pad[496];
} log_unit_hdr_s_t;



ts_s32 tdl_log_init(log_dev_s_t *pst_log_dev);
ts_s32 ts_drv_log_init(ts_void);
ts_s32 log_clear(ts_void);
ts_s32 flush_cur_log(ts_void);
ts_s32 log_putc(ts_u08 u08_ch);
ts_s32 log_all_print(ts_void);

#endif  //__K_LOG_V100_H__



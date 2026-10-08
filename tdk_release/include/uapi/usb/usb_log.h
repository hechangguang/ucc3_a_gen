/*
 * Copyright (c) 2022, sakumisu
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef USB_LOG_H
#define USB_LOG_H

#include <tdl/usb_v100.h>
#include <cpl/ts_cpl.h>

#define USB_LOG_DBG         usb_log_debug
#define USB_LOG_INFO        usb_log_info
#define USB_LOG_WRN         usb_log_warn
#define USB_LOG_ERR         usb_log_err
#define USB_LOG_RAW         ts_cpl_printk

#define USB_ASSERT(f)       TS_CPL_ASSERT(f)

#define ___is_print(ch)     ((unsigned int)((ch) - ' ') < 127u - ' ')

static inline void usb_hexdump(const ts_void *ptr, ts_u32 buflen)
{
    ts_u08 *buf = (ts_u08 *)ptr;
    ts_u32 i, j;

    (ts_void)buf;

    for (i = 0; i < buflen; i += 16) {
        ts_cpl_printk("%08x:", i);

        for (j = 0; j < 16; j++) {
            if (i + j < buflen) {
                if ((j % 8) == 0) {
                    ts_cpl_printk("  ");
                }

                ts_cpl_printk("%02X ", buf[i + j]);
            } else {
                ts_cpl_printk("   ");
            }
        }
        ts_cpl_printk(" ");

        for (j = 0; j < 16; j++) {
            if (i + j < buflen)
                ts_cpl_printk("%c", ___is_print(buf[i + j]) ? buf[i + j] : '.');
        }

        ts_cpl_printk("\n");
    }
}

#endif /* USB_LOG_H */

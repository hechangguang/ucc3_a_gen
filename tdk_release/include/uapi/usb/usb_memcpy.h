/*
 * Copyright (c) 2024, sakumisu
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef USB_MEMCPY_H
#define USB_MEMCPY_H

#include <base/ts_type.h>
#include <cpl/ts_cpl.h>

#define ALIGN_UP_DWORD(x) ((ts_u32)(uintptr_t)(x) & (sizeof(ts_u32) - 1))

static inline ts_void dword2array(ts_char *addr, ts_u32 w)
{
    addr[0] = w;
    addr[1] = w >> 8;
    addr[2] = w >> 16;
    addr[3] = w >> 24;
}

static inline ts_void *usb_memcpy(ts_void *s1, const ts_void *s2, size_t n)
{
    ts_char *b1 = (ts_char *)s1;
    const ts_char *b2 = (const ts_char *)s2;
    ts_u32 *w1;
    const ts_u32 *w2;

    if (ALIGN_UP_DWORD(b1) == ALIGN_UP_DWORD(b2)) {
        while (ALIGN_UP_DWORD(b1) != 0 && n > 0) {
            *b1++ = *b2++;
            --n;
        }

        w1 = (ts_u32 *)b1;
        w2 = (const ts_u32 *)b2;

        while (n >= 4 * sizeof(ts_u32)) {
            *w1++ = *w2++;
            *w1++ = *w2++;
            *w1++ = *w2++;
            *w1++ = *w2++;
            n -= 4 * sizeof(ts_u32);
        }

        while (n >= sizeof(ts_u32)) {
            *w1++ = *w2++;
            n -= sizeof(ts_u32);
        }

        b1 = (ts_char *)w1;
        b2 = (const ts_char *)w2;

        while (n--) {
            *b1++ = *b2++;
        }
    } else {
        while (n > 0 && ALIGN_UP_DWORD(b2) != 0) {
            *b1++ = *b2++;
            --n;
        }

        w2 = (const ts_u32 *)b2;

        while (n >= 4 * sizeof(ts_u32)) {
         dword2array(b1, *w2++);
            b1 += sizeof(ts_u32);
         dword2array(b1, *w2++);
            b1 += sizeof(ts_u32);
         dword2array(b1, *w2++);
            b1 += sizeof(ts_u32);
         dword2array(b1, *w2++);
            b1 += sizeof(ts_u32);
            n -= 4 * sizeof(ts_u32);
        }

        while (n >= sizeof(ts_u32)) {
         dword2array(b1, *w2++);
            b1 += sizeof(ts_u32);
            n -= sizeof(ts_u32);
        }

        b2 = (const ts_char *)w2;

        while (n--) {
            *b1++ = *b2++;
        }
    }
    return s1;
}
#endif

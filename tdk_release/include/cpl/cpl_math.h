/* 
 * cpl_math.h -- cpl 数学库头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __CPL_MATH_H__
#define __CPL_MATH_H__

#include <base/ts_type.h>

#define ts_cpl_min(x, y) ({                \
    __typeof__(x) _min1 = (x);          \
     __typeof__(y) _min2 = (y);          \
     (void) (&_min1 == &_min2);      \
     _min1 < _min2 ? _min1 : _min2; })

#define ts_cpl_max(x, y) ({                            \
        __typeof__(x) _max1 = (x);                  \
        __typeof__(y) _max2 = (y);                  \
        (void) (&_max1 == &_max2);              \
        _max1 > _max2 ? _max1 : _max2; })

#define ts_cpl_abs(x) ({                       \
     long ret;                   \
     if (sizeof(x) == sizeof(long)) {        \
         long __x = (x);             \
         ret = (__x < 0) ? -__x : __x;       \
     } else {                    \
         int __x = (x);              \
         ret = (__x < 0) ? -__x : __x;       \
     }                       \
     ret;                        \
})


#define ts_cpl_min_t(type, x, y) ({            \
    type __min1 = (x);            \
    type __min2 = (y);            \
    __min1 < __min2 ? __min1: __min2; })

#define ts_cpl_max_t(type, x, y) ({            \
    type __max1 = (x);            \
    type __max2 = (y);            \
    __max1 > __max2 ? __max1: __max2; })

extern ts_u64 ts_cpl_div_u64(ts_u64 dividend, ts_u32 divisor);
extern ts_s64 ts_cpl_div_s64(ts_s64 dividend, ts_s32 divisor);
extern ts_u64 ts_cpl_div64_u64(ts_u64  dividend, ts_u64  divisor);
extern ts_s64 ts_cpl_div64_s64(ts_s64 dividend, ts_s64 divisor);

extern ts_u64 ts_cpl_div_u64_rem(ts_u64  dividend, ts_u32 divisor, ts_u32 *remainder);
extern ts_s64 ts_cpl_div_s64_rem(ts_s64 dividend, ts_s32 divisor, ts_s32 *remainder);
extern ts_u64 ts_cpl_div64_u64_rem(ts_u64 dividend, ts_u64 divisor, ts_u64 *remainder);

extern ts_void ts_cpl_srand(ts_u32 seed);
extern ts_u32 ts_cpl_random(ts_void);

#endif /* __CPL_MATH_H__ */

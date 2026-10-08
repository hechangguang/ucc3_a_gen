/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_MATH64_H
#define _LINUX_MATH64_H

#include <base/ts_type.h>
#include <linux/div64.h>


#define div64_long(x, y) div_s64((x), (y))
#define div64_ul(x, y)   div_u64((x), (y))

static inline ts_u64 div_u64_rem(ts_u64 dividend, ts_u32 divisor, ts_u32 *remainder)
{
    *remainder = do_div(dividend, divisor);
    return dividend;
}

extern ts_s64 div_s64_rem(ts_s64 dividend, ts_s32 divisor, ts_s32 *remainder);
extern ts_u64 div64_u64_rem(ts_u64 dividend, ts_u64 divisor, ts_u64 *remainder);
extern ts_u64 div64_u64(ts_u64 dividend, ts_u64 divisor);
extern ts_s64 div64_s64(ts_s64 dividend, ts_s64 divisor);


/**
 * div_u64 - unsigned 64bit divide with 32bit divisor
 * @dividend: unsigned 64bit dividend
 * @divisor: unsigned 32bit divisor
 *
 * This is the most common 64bit divide and should be used if possible,
 * as many 32bit archs can optimize this variant better than a full 64bit
 * divide.
 */
static inline ts_u64 div_u64(ts_u64 dividend, ts_u32 divisor)
{
    ts_u32 remainder;
    return div_u64_rem(dividend, divisor, &remainder);
}

/**
 * div_s64 - signed 64bit divide with 32bit divisor
 * @dividend: signed 64bit dividend
 * @divisor: signed 32bit divisor
 */
static inline ts_s64 div_s64(ts_s64 dividend, ts_s32 divisor)
{
    ts_s32 remainder;
    return div_s64_rem(dividend, divisor, &remainder);
}

ts_u32 iter_div_u64_rem(ts_u64 dividend, ts_u32 divisor, ts_u64 *remainder);

static inline ts_u32 __iter_div_u64_rem(ts_u64 dividend, ts_u32 divisor, ts_u64 *remainder)
{
    ts_u32 ret = 0;

    while (dividend >= divisor) {
        /* The following asm() prevents the compiler from
           optimising this loop into a modulo operation.  */
        asm("" : "+rm"(dividend));

        dividend -= divisor;
        ret++;
    }

    *remainder = dividend;

    return ret;
}


/*
 * Many a GCC version messes this up and generates a 64x64 mult :-(
 */
static inline ts_u64 mul_u32_u32(ts_u32 a, ts_u32 b)
{
    return (ts_u64)a * b;
}


static inline ts_u64 mul_u64_u32_shr(ts_u64 a, ts_u32 mul, unsigned int shift)
{
    ts_u32 ah, al;
    ts_u64 ret;

    al = a;
    ah = a >> 32;

    ret = mul_u32_u32(al, mul) >> shift;
    if (ah)
        ret += mul_u32_u32(ah, mul) << (32 - shift);

    return ret;
}

static inline ts_u64 mul_u64_u64_shr(ts_u64 a, ts_u64 b, unsigned int shift)
{
    union {
        ts_u64 ll;
        struct {
            ts_u32 low, high;
        } l;
    } rl, rm, rn, rh, a0, b0;
    ts_u64 c;

    a0.ll = a;
    b0.ll = b;

    rl.ll = mul_u32_u32(a0.l.low, b0.l.low);
    rm.ll = mul_u32_u32(a0.l.low, b0.l.high);
    rn.ll = mul_u32_u32(a0.l.high, b0.l.low);
    rh.ll = mul_u32_u32(a0.l.high, b0.l.high);

    /*
     * Each of these lines computes a 64-bit intermediate result into "c",
     * starting at bits 32-95.  The low 32-bits go into the result of the
     * multiplication, the high 32-bits are carried into the next step.
     */
    rl.l.high = c = (ts_u64)rl.l.high + rm.l.low + rn.l.low;
    rh.l.low = c = (c >> 32) + rm.l.high + rn.l.high + rh.l.low;
    rh.l.high = (c >> 32) + rh.l.high;

    /*
     * The 128-bit result of the multiplication is in rl.ll and rh.ll,
     * shift it right and throw away the high part of the result.
     */
    if (shift == 0)
        return rl.ll;
    if (shift < 64)
        return (rl.ll >> shift) | (rh.ll << (64 - shift));
    return rh.ll >> (shift & 63);
}


static inline ts_u64 mul_u64_u32_div(ts_u64 a, ts_u32 mul, ts_u32 divisor)
{
    union {
        ts_u64 ll;
        struct {
            ts_u32 low, high;
        } l;
    } u, rl, rh;

    u.ll = a;
    rl.ll = mul_u32_u32(u.l.low, mul);
    rh.ll = mul_u32_u32(u.l.high, mul) + rl.l.high;

    /* Bits 32-63 of the result will be in rh.l.low. */
    rl.l.high = do_div(rh.ll, divisor);

    /* Bits 0-31 of the result will be in rl.l.low.    */
    do_div(rl.ll, divisor);

    rl.l.high = rh.l.low;
    return rl.ll;
}

#define DIV64_U64_ROUND_UP(ll, d)    ({ u64 _tmp = (d); div64_u64((ll) + _tmp - 1, _tmp); })

#endif /* _LINUX_MATH64_H */

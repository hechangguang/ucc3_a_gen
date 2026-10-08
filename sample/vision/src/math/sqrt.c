/*
 * sqrt.c -- tdk sqrt 实现 copy from nuttx
 */
#include <base/ts_type.h>
#include "tdk_math.h"

static float lib_sqrtapprox(float x)
{
    union {
        int i;
        float x;
    } u;

    /* Floats + bit manipulation = +inf fun! */
    u.x = x;
    u.i = 0x1fc00000 + (u.i >> 1);

    return u.x;
}

double tdk_sqrt(double x)
{
    long double y;
    long double y1;

    if (x < 0.0)
        return NAN;

    if (isnan(x))
      return NAN;

    if (isinf(x))
        return INFINITY;

    if (x == 0.0)
        return 0.0;

    /* Guess square root (using bit manipulation) */
    y = lib_sqrtapprox(x);

    /* Perform four iterations of approximation.  This number (4) is
    * definitely optimal
    */
    y = 0.5 * (y + x / y);
    y = 0.5 * (y + x / y);
    y = 0.5 * (y + x / y);
    y = 0.5 * (y + x / y);

    /* If guess was terribe (out of range of float).  Repeat approximation
    * until convergence.
    */
    if (y * y < x - 1.0 || y * y > x + 1.0) {
        y1 = -1.0;
        while (y != y1) {
            y1 = y;
            y = 0.5 * (y + x / y);
        }
    }

    return y;
}


float tdk_sqrtf(float x)
{
    float y;

    /* Filter out invalid/trivial inputs */
    if (x < 0.0F)
        return NAN_F;

    if (isnan(x))
        return NAN_F;

    if (isinf_f(x))
        return INFINITY_F;

    if (x == 0.0F)
        return 0.0F;

    /* Guess square root (using bit manipulation) */
    y = lib_sqrtapprox(x);

    /* Perform three iterations of approximation. This number (3) is
    * definitely optimal
    */
    y = 0.5F * (y + x / y);
    y = 0.5F * (y + x / y);
    y = 0.5F * (y + x / y);

    return y;
}


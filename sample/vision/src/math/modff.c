/*
 * modff.c -- tdk modff 实现, copy from nuttx
 */
#include <base/ts_type.h>
#include "tdk_math.h"

float tdk_modff(float x, float *iptr)
{
    if (tdk_fabsf(x) >= 8388608.0F) {
        *iptr = x;

        return 0.0F;
    } else if (tdk_fabsf(x) < 1.0F) {
        *iptr = 0.0F;

        return x;
    } else {
        *iptr = (float)(int)x;

        return (x - *iptr);
    }
}


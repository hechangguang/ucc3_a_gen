/*
 * floorf.c -- tdk floorf 实现, copy from nuttx
 */
#include <base/ts_type.h>
#include "inc/tdk_math.h"


float tdk_floorf(float x)
{
    float modx;

    /* modf() will return the integer part of X.  The return value of floor
    * differs for non-integer, negative values.
    *
    *  x   modff floor
    * ---- ----- -----
    *  2.0  2.0   2.0
    *  2.4  2.0   2.0
    *  2.9  2.0   2.0
    * -2.7 -2.0  -3.0
    * -2.0 -2.0  -2.0
    */

    tdk_modff(x, &modx);
    if (x < 0.0F && x < modx)
      modx -= 1.0F;

    return modx;
}
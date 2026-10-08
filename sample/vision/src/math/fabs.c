/*
 * fabs.c -- tdk fabs 实现, copy from nuttx
 */
#include <base/ts_type.h>
#include "tdk_math.h"

double tdk_fabs(double x)
{
    return ((x < 0) ? -x : x);
}

float tdk_fabsf(float x)
{
    return ((x < 0) ? -x : x);
}


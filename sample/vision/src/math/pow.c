/*
 * pow.c -- tdk pow 实现 copy from nuttx
 */
#include <base/ts_type.h>
#include "tdk_math.h"


double tdk_pow(double b, double e)
{
    return tdk_exp(e * tdk_log(b));
}

float tdk_powf(float b, float e)
{
    return tdk_expf(e * tdk_logf(b));
}


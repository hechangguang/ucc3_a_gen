/*
 * log.c -- tdk log 实现, copy from nuttx
 */
#include <base/ts_type.h>
#include "tdk_math.h"

#define FLT_MAX_EXP_X           88.0F

#define LOGF_MAX_ITER           10
#define LOGF_RELAX_MULTIPLIER   2

double tdk_log(double x)
{
    double y, y_old, ney, epsilon;
    int relax_factor, iter;

    y = 0.0;
    y_old = 1.0;
    epsilon = DBL_EPSILON;

    iter = 0;
    relax_factor = 1;

    while (y > y_old + epsilon || y < y_old - epsilon) {
        y_old = y;
        ney = tdk_exp(-y);
        y -= 1.0 - x * ney;

        if (y > 700.0)
            y = 700.0;

        if (y < -700.0)
            y = -700.0;

        epsilon = (tdk_fabs(y) > 1.0) ? tdk_fabs(y) * DBL_EPSILON : DBL_EPSILON;

        if (++iter >= LOGF_MAX_ITER) {
            relax_factor *= LOGF_RELAX_MULTIPLIER;
            iter = 0;
        }

        if (relax_factor > 1)
          epsilon *= relax_factor;
    }

    if (y == 700.0)
        return INFINITY;

    if (y == -700.0)
      return INFINITY;

    return y;
}

double tdk_log2(double x)
{
    return (tdk_log(x) / M_LN2);
}


double tdk_log10(double x)
{
    return (tdk_log(x) / M_LN10);
}

/* float */
float tdk_logf(float x)
{
    float y, y_old, ney, epsilon;
    int relax_factor, iter;

    y = 0.0F;
    y_old = 1.0F;
    epsilon = FLT_EPSILON;

    iter = 0;
    relax_factor = 1;
    while (y > y_old + epsilon || y < y_old - epsilon) {
        y_old = y;
        ney = tdk_expf(-y);
        y -= 1.0F - x * ney;

        if (y > FLT_MAX_EXP_X)
            y = FLT_MAX_EXP_X;

        if (y < -FLT_MAX_EXP_X)
            y = -FLT_MAX_EXP_X;

        epsilon = (tdk_fabsf(y) > 1.0F) ? tdk_fabsf(y) * FLT_EPSILON : FLT_EPSILON;
        if (++iter >= LOGF_MAX_ITER) {
            relax_factor *= LOGF_RELAX_MULTIPLIER;
            iter = 0;
        }

        if (relax_factor > 1)
          epsilon *= relax_factor;
    }

    if (y == FLT_MAX_EXP_X)
        return INFINITY;

    if (y == -FLT_MAX_EXP_X)
        return INFINITY;

    return y;
}

float tdk_log2f(float x)
{
    return (tdk_logf(x) / (float)M_LN2);
}


float tdk_log10f(float x)
{
    return (tdk_logf(x) / (float)M_LN10);
}


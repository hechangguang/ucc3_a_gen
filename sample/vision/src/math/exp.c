/*
 * exp.c -- tdk exp 实现 copy from nuttx
 */
#include <base/ts_type.h>
#include "tdk_math.h"

static float _flt_inv_fact[] =
{
    1.0 / 1.0,                    /* 1/0! */
    1.0 / 1.0,                    /* 1/1! */
    1.0 / 2.0,                    /* 1/2! */
    1.0 / 6.0,                    /* 1/3! */
    1.0 / 24.0,                   /* 1/4! */
    1.0 / 120.0,                  /* 1/5! */
    1.0 / 720.0,                  /* 1/6! */
    1.0 / 5040.0,                 /* 1/7! */
    1.0 / 40320.0,                /* 1/8! */
    1.0 / 362880.0,               /* 1/9! */
    1.0 / 3628800.0,              /* 1/10! */
};

static double _dbl_inv_fact[] =
{
    1.0 / 1.0,                      /* 1 / 0! */
    1.0 / 1.0,                      /* 1 / 1! */
    1.0 / 2.0,                      /* 1 / 2! */
    1.0 / 6.0,                      /* 1 / 3! */
    1.0 / 24.0,                     /* 1 / 4! */
    1.0 / 120.0,                    /* 1 / 5! */
    1.0 / 720.0,                    /* 1 / 6! */
    1.0 / 5040.0,                   /* 1 / 7! */
    1.0 / 40320.0,                  /* 1 / 8! */
    1.0 / 362880.0,                 /* 1 / 9! */
    1.0 / 3628800.0,                /* 1 / 10! */
    1.0 / 39916800.0,               /* 1 / 11! */
    1.0 / 479001600.0,              /* 1 / 12! */
    1.0 / 6227020800.0,             /* 1 / 13! */
    1.0 / 87178291200.0,            /* 1 / 14! */
    1.0 / 1307674368000.0,          /* 1 / 15! */
    1.0 / 20922789888000.0,         /* 1 / 16! */
    1.0 / 355687428096000.0,        /* 1 / 17! */
    1.0 / 6402373705728000.0,       /* 1 / 18! */
};

#define M_E2    (M_E    * M_E)
#define M_E4    (M_E2   * M_E2)
#define M_E8    (M_E4   * M_E4)
#define M_E16   (M_E8   * M_E8)
#define M_E32   (M_E16  * M_E16)
#define M_E64   (M_E32  * M_E32)
#define M_E128  (M_E64  * M_E64)
#define M_E256  (M_E128 * M_E128)
#define M_E512  (M_E256 * M_E256)

static const float g_expif_square_tbl[] =
{
    (float)M_E,    /* e^1 */
    (float)M_E2,   /* e^2 */
    (float)M_E4,   /* e^4 */
    (float)M_E8,   /* e^8 */
    (float)M_E16,  /* e^16 */
    (float)M_E32,  /* e^32 */
    (float)M_E64   /* e^64 */
};

static const double g_expi_square_tbl[] =
{
    M_E,          /* e^1 */
    M_E2,         /* e^2 */
    M_E4,         /* e^4 */
    M_E8,         /* e^8 */
    M_E16,        /* e^16 */
    M_E32,        /* e^32 */
    M_E64,        /* e^64 */
    M_E128,       /* e^128 */
    M_E256,       /* e^256 */
    M_E512        /* e^512 */
};

static double lib_expi(ts_size_t n)
{
    ts_size_t i;
    double val;

    /* The largest calculable value for n is floor(ln(DBL_MAX)) */
    if (n > 709)
        return INFINITY;

    val = 1.0;
    for (i = 0; n != 0; i++) {
        if ((n & (1 << i)) != 0) {
            n &= ~(1 << i);
            val *= g_expi_square_tbl[i];
        }
    }

    return val;
}

static float lib_expif(ts_size_t n)
{
    ts_size_t i;
    float val;

    /* The largest calculable value for n is floor(ln(FLT_MAX)) */
    if (n > 88)
        return INFINITY_F;

    val = 1.0F;
    for (i = 0; n != 0; i++) {
        if ((n & (1 << i)) != 0) {
            n &= ~(1 << i);
            val *= g_expif_square_tbl[i];
        }
    }

    return val;
}



double tdk_exp(double x)
{
    ts_size_t i, int_part;
    ts_bool invert;
    double value;
    double x0;

    if (x == 0) {
        return 1;
    } else if (x < 0) {
        invert = ts_true;
        x = -x;
    } else {
        invert = ts_false;
    }

    /* Extract integer component */
    int_part = (ts_size_t)x;

    /* Set x to fractional component */
    x -= (double)int_part;

    /* Perform Taylor series approximation with nineteen terms */
    value = 0.0;
    x0 = 1.0;
    for (i = 0; i < 19; i++) {
        value += x0 * _dbl_inv_fact[i];
        x0 *= x;
    }

    /* Multiply by exp of the integer component */
    value *= lib_expi(int_part);

    if (ts_true == invert)
        return (1.0 / value);
    else
        return value;
}

/* float */
float tdk_expf(float x)
{
    ts_size_t i, int_part;
    ts_bool invert;
    float value, x0;

    if (x == 0) {
        return 1;
    } else if (x < 0) {
        invert = ts_true;
        x = -x;
    } else {
        invert = ts_false;
    }

    /* Extract integer component */
    int_part = (ts_size_t)x;

    /* set x to fractional component */
    x -= (float)int_part;

    /* Perform Taylor series approximation with eleven terms */
    value = 0.0F;
    x0 = 1.0F;
    for (i = 0; i < 10; i++) {
        value += x0 * _flt_inv_fact[i];
        x0 *= x;
    }

    /* Multiply by exp of the integer component */
    value *= lib_expif(int_part);

    if (invert)
        return (1.0F / value);
    else
        return value;
}


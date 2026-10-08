/*
 * tdk_math.h -- tdk math 接口等声明头文件
 */
#ifndef __TDK_MATH_H__
#define __TDK_MATH_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif

#endif /* __cplusplus */
#include <base/ts_type.h>
#include <base/ts_bitop.h>

#define CONFIG_HAVE_DOUBLE

#define INFINITY        (1.0/0.0)
#define NAN             (0.0/0.0)
#define HUGE_VAL        INFINITY

#define INFINITY_F      (1.0F/0.0F)
#define NAN_F           (0.0F/0.0F)

#define isnan(x)        ((x) != (x))
#define isinf(x)        (((x) == INFINITY) || ((x) == -INFINITY))
#define isfinite(x)     (!(isinf(x) || isnan(x)))

#define isinf_f(x)      (((x) == INFINITY_F) || ((x) == -INFINITY_F))

/* May be defined in toolchain header */
#ifndef FLT_EPSILON
#  define FLT_EPSILON 1.1920929e-07F /* 1E-5 */
#endif

 /* May be defined in toolchain header */
#ifndef DBL_EPSILON
#  ifdef CONFIG_HAVE_DOUBLE
#    define DBL_EPSILON 2.2204460492503131e-16  /* 1E-9 */
#  else
#    define DBL_EPSILON FLT_EPSILON
#  endif
#endif

/* Exponential and Logarithmic constants ************************************/
#define M_E        2.7182818284590452353602874713526625
#define M_SQRT2    1.4142135623730950488016887242096981
#define M_SQRT1_2  0.7071067811865475244008443621048490
#define M_LOG2E    1.4426950408889634073599246810018921
#define M_LOG10E   0.4342944819032518276511289189166051
#define M_LN2      0.6931471805599453094172321214581765
#define M_LN10     2.3025850929940456840179914546843642

/* fabs */
double tdk_fabs(double x);
float tdk_fabsf(float x);

/* pow */
float tdk_powf(float b, float e);
double tdk_pow(double b, double e);

/* exp */
float tdk_expf(float x);
double tdk_exp(double x);

/* log */
float tdk_logf(float x);
float tdk_log10f(float x);
float tdk_log2f(float x);

double tdk_log(double x);
double tdk_log10(double x);
double tdk_log2(double x);

/* sqrt */
float tdk_sqrtf(float x);
double tdk_sqrt(double x);

/* floorf */
float tdk_floorf(float x);

/* modff */
float tdk_modff(float x, float *iptr);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __TDK_MATH_H__ */


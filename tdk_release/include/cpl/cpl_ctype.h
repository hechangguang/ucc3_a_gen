/* 
 * cpl_ctype.h -- cpl ctype 实现头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2024 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __CPL_CTYPE_H__
#define __CPL_CTYPE_H__

/*
 * NOTE! This ctype does not handle EOF like the standard C
 * library is required to.
 */

#define _CPL_U      0x01    /* upper */
#define _CPL_L      0x02    /* lower */
#define _CPL_D      0x04    /* digit */
#define _CPL_C      0x08    /* cntrl */
#define _CPL_P      0x10    /* punct */
#define _CPL_S      0x20    /* white space (space/lf/tab) */
#define _CPL_X      0x40    /* hex digit */
#define _CPL_SP     0x80    /* hard space (0x20) */

extern const unsigned char _cpl_ctype[];

#define __cpl_ismask(x)         (_cpl_ctype[(int)(unsigned char)(x)])

#define ts_cpl_isalnum(c)       ((__cpl_ismask(c) & (_CPL_U | _CPL_L | _CPL_D)) != 0)
#define ts_cpl_isalpha(c)       ((__cpl_ismask(c) & (_CPL_U | _CPL_L)) != 0)
#define ts_cpl_iscntrl(c)       ((__cpl_ismask(c) & (_CPL_C)) != 0)

static inline int ts_cpl_isdigit(int c)
{
    return '0' <= c && c <= '9';
}

#define ts_cpl_isgraph(c)       ((__cpl_ismask(c)&(_CPL_P | _CPL_U |_CPL_L | _CPL_D)) != 0)
#define ts_cpl_islower(c)       ((__cpl_ismask(c)&(_CPL_L)) != 0)
#define ts_cpl_isprint(c)       ((__cpl_ismask(c)&(_CPL_P | _CPL_U | _CPL_L | _CPL_D | _CPL_SP)) != 0)
#define ts_cpl_ispunct(c)       ((__cpl_ismask(c)&(_CPL_P)) != 0)

/* Note: isspace() must return false for %NUL-terminator */
#define ts_cpl_isspace(c)       ((__cpl_ismask(c)&(_CPL_S)) != 0)
#define ts_cpl_isupper(c)       ((__cpl_ismask(c)&(_CPL_U)) != 0)
#define ts_cpl_isxdigit(c)      ((__cpl_ismask(c)&(_CPL_D | _CPL_X)) != 0)

#define ts_cpl_isascii(c)       (((unsigned char)(c)) <= 0x7f)
#define ts_cpl_toascii(c)       (((unsigned char)(c)) & 0x7f)

static inline unsigned char __cpl_tolower(unsigned char c)
{
    if (ts_cpl_isupper(c))
        c -= 'A'-'a';
    return c;
}

static inline unsigned char __cpl_toupper(unsigned char c)
{
    if (ts_cpl_islower(c))
        c -= 'a'-'A';
    return c;
}

#define ts_cpl_tolower(c)       __cpl_tolower(c)
#define ts_cpl_toupper(c)       __cpl_toupper(c)

/*
 * Fast implementation of tolower() for internal usage. Do not use in your
 * code.
 */
static inline char _cpl_tolower(const char c)
{
    return c | 0x20;
}

/* Fast check for octal digit */
static inline int ts_cpl_isodigit(const char c)
{
    return c >= '0' && c <= '7';
}

#endif /* __CPL_CTYPE_H__ */

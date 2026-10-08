#ifndef _LINUX_TYPES_H
#define _LINUX_TYPES_H

//#include <linux/posix_types.h>
//#include <asm/types.h>
//#include <stdbool.h>

/* bsd */
typedef unsigned char		u_char;
typedef unsigned short		u_short;
typedef unsigned int		u_int;
typedef unsigned long		u_long;

/* sysv */
typedef unsigned char		uchar;
typedef unsigned short		ushort;
typedef unsigned int		uint;
typedef unsigned long		ulong;

/* u-boot and linux */
#ifndef uint32_t
//typedef unsigned int		uint32_t;
#endif

/* int-ll64.h */
#ifndef __s8
typedef signed char __s8;
typedef __s8  s8;
#endif

#ifndef __u8
typedef unsigned char __u8;
typedef __u8  u8;
#endif

#ifndef __s16
typedef signed short __s16;
typedef __s16 s16;
#endif

#ifndef __u16
typedef unsigned short __u16;
typedef __u16 u16;
#endif

#ifndef __s32
typedef signed int __s32;
typedef __s32 s32;
#endif

#ifndef __u32
typedef unsigned int __u32;
typedef __u32 u32;
#endif

#ifndef __u64
typedef unsigned long long __u64;
typedef __u64 u64;
#endif


/*
 * Below are truly Linux-specific types that should never collide with
 * any application/library that wants linux/types.h.
 */
#ifdef __CHECKER__
#define __bitwise__ __attribute__((bitwise))
#else
#define __bitwise__
#endif

#ifdef __CHECK_ENDIAN__
#define __bitwise __bitwise__
#else
#define __bitwise
#endif

typedef __u16 __bitwise __le16;
typedef __u16 __bitwise __be16;
typedef __u32 __bitwise __le32;
typedef __u32 __bitwise __be32;

#if defined(__GNUC__)
typedef __u64 __bitwise __le64;
typedef __u64 __bitwise __be64;
#endif

typedef __u16 __bitwise __sum16;
typedef __u32 __bitwise __wsum;


#endif /* _LINUX_TYPES_H */

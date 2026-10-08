/* 
 * cpl_fcntl.h -- fasync cpl 层实现头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2022-2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __CPL_FCNTL_H__
#define __CPL_FCNTL_H__

#include <base/ts_type.h>

#define TS_CPL_SIGIO        (29)
#define TS_CPL_SIGINT       (2)
#define TS_CPL_POLLIN       (0x0001)

/* +1 == FREAD */
#define	TS_CPL_O_RDONLY	    (0x0000)

/* +1 == FWRITE */
#define	TS_CPL_O_WRONLY     (0x0001)

/* +1 == FREAD|FWRITE */
#define	TS_CPL_O_RDWR       (0x0002)

/* append (writes guaranteed at the end) */
#define	TS_CPL_O_APPEND	    (0x0008)

/* open with file create */
#define	TS_CPL_O_CREAT		(0x0200)

/* open with truncation */
#define	TS_CPL_O_TRUNC		(0x0400)

/* error on open if file exists */
#define	TS_CPL_O_EXCL		(0x0800)

/* do all writes synchronously */
#define TS_CPL_O_SYNC		(0x2000)

/* non blocking I/O (POSIX style) */
#define	TS_CPL_O_NONBLOCK	(0x4000)

/* don't assign a ctty on this open */
#define	TS_CPL_O_NOCTTY     (0x8000)

extern ts_s32 ts_cpl_fasync_helper(ts_s32, ts_void *, ts_s32, ts_void **);
extern ts_s32 ts_cpl_kill_fasync(ts_void *, ts_s32, ts_s32);

#endif /* __CPL_FCNTL_H__ */

/*
 * ts_cpl.h -- tdk 跨平台头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __TS_CPL_H__
#define __TS_CPL_H__


#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <base/ts_type.h>
#include <base/ts_errno.h>

#define TS_CPL_VERSION      (0x56000100)

#define TS_CPL_UNUSED(x)    (ts_void)(x)


#define TS_CPL_MAX_PATH_LEN     (47)

#include <cpl/cpl_addr.h>
#include <cpl/cpl_atomic.h>
#include <cpl/cpl_barrier.h>
#include <cpl/cpl_base.h>
#include <cpl/cpl_cache.h>
#include <cpl/cpl_crc.h>
#include <cpl/cpl_ctype.h>
#include <cpl/cpl_debug.h>
#include <cpl/cpl_device.h>
#include <cpl/cpl_dma.h>
#include <cpl/cpl_fctl.h>
#include <cpl/cpl_fops.h>
#include <cpl/cpl_interrupt.h>
#include <cpl/cpl_ioctl.h>
#include <cpl/cpl_list.h>
#include <cpl/cpl_malloc.h>
#include <cpl/cpl_math.h>
#include <cpl/cpl_mutex.h>
#include <cpl/cpl_platform.h>
#include <cpl/cpl_proc.h>
#include <cpl/cpl_schedule.h>
#include <cpl/cpl_semaphore.h>
//#include <cpl/cpl_seq.h>
#include <cpl/cpl_spinlock.h>
#include <cpl/cpl_string.h>
#include <cpl/cpl_task.h>
#include <cpl/cpl_time.h>
#include <cpl/cpl_timer.h>
#include <cpl/cpl_tmm.h>
#include <cpl/cpl_tty.h>
#include <cpl/cpl_wait.h>
#include <cpl/cpl_workqueue.h>
#include <cpl/cpl_kv.h>
#include <cpl/cpl_zip.h>


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __TS_CPL_H__ */

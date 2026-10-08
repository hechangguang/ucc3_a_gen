/*
 * poll.h -- tsp 文件系统 poll 头文件,使用为 linux 版本的简化代码, 并参考 nuttx 结构
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2024 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */

#ifndef __POLL__
#define __POLL__
#include <config.h>
#include <base/ts_compiler.h>
#include <linux/semaphore.h>


/* Poll event definitions:
 *
 *   POLLIN
 *     Data other than high-priority data may be read without blocking.
 *   POLLRDNORM
 *     Normal data may be read without blocking.
 *   POLLRDBAND
 *     Priority data may be read without blocking.
 *   POLLPRI
 *     High priority data may be read without blocking.
 *
 *   POLLOUT
 *     Normal data may be written without blocking.
 *   POLLWRNORM
 *     Equivalent to POLLOUT.
 *   POLLWRBAND
 *     Priority data may be written.
 *
 *   POLLERR
 *     An error has occurred (revents only).
 *   POLLHUP
 *     Device has been disconnected (revents only).
 *   POLLNVAL
 *     Invalid fd member (revents only).
 */

#define POLLIN       (0x01)  /* NuttX does not make priority distinctions */
#define POLLRDNORM   (0x01)
#define POLLRDBAND   (0x01)

#define POLLPRI      (0x02)

#define POLLOUT      (0x04)  /* NuttX does not make priority distinctions */
#define POLLWRNORM   (0x04)
#define POLLWRBAND   (0x04)

#define POLLERR      (0x08)
#define POLLHUP      (0x10)
#define POLLRDHUP    (0x10)  /* NuttX does not support shutdown(fd, SHUT_RD) */
#define POLLNVAL     (0x20)


/* The number of poll descriptors (required by poll() specification */
typedef unsigned int nfds_t;

/* In the standard poll() definition, the size of the event set is 'short'.
 * Here we pick the uint32_t type to be consistent with the linux to support
 * EPOLLRDHUP, EPOLLWAKEUP, EPOLLONESHOT...
 */
typedef unsigned int pollevent_t;

/* The poll callback type */
struct pollfd;
typedef void (*pollcb_t)(struct pollfd *fds);

/* This is the NuttX variant of the standard pollfd structure.  The poll()
 * interfaces receive a variable length array of such structures.
 *
 * REVISIT: In a multi-threaded environment, one use case might be to share
 * a single, array of struct pollfd in poll calls on different threads.
 * That use case is not supportable with this variant due way in which the
 * non-standard internal fields are used in the implementation of poll().
 */
struct pollfd
{
  /* Standard fields */

  int          fd;      /* The descriptor being polled */
  pollevent_t  events;  /* The input event flags */
  pollevent_t  revents; /* The output event flags */

  /* Non-standard fields used internally by NuttX. */

  void    *arg;     /* The poll callback function argument */
  pollcb_t     cb;      /* The poll callback function */
  void    *priv;    /* For use by drivers */
};

/****************************************************************************
 * Public Data
 ****************************************************************************/

#undef EXTERN
#if defined(__cplusplus)
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

int poll(struct pollfd *fds, nfds_t nfds, int timeout);
int poll_fdsetup(int fd, struct pollfd *fds, ts_bool setup);
void poll_default_cb(struct pollfd *fds);
void poll_notify(struct pollfd **afds, int nfds, pollevent_t eventset);

#undef EXTERN
#if defined(__cplusplus)
}
#endif

#endif /* __POLL__ */

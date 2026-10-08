/*
 * ts_errno.h -- tdk 错误码统一声明
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __TS_ERRNO_H__
#define __TS_ERRNO_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

extern int s32_errno;

#define ts_set_errno(e) \
  do \
    { \
      s32_errno = (int)(e); \
    } \
  while (0)
#define ts_get_errno() s32_errno


#define TS_OK               (0)
#define TS_ERR              (-1)


/* 公共错误码范围 */
#define TS_EPERM            (1)
#define TS_EPERM_STR        "Operation not permitted"

#define TS_ENOENT           (2)
#define TS_ENOENT_STR       "No such file or directory"

#define TS_EINTR            (4)
#define TS_EINTR_STR        "Interrupted system call"

#define TS_EIO              (5)
#define TS_EIO_STR          "I/O error"

#define TS_ENXIO            (6)
#define TS_ENXIO_STR        "No such device or address"

#define TS_EBADF            (9)
#define TS_EBADF_STR        "Bad file number"

#define TS_EAGAIN           (11)
#define TS_EAGAIN_STR       "Try again"

#define TS_ENOMEM           (12)
#define TS_ENOMEM_STR       "Out of memory"

#define TS_EACCES           (13)
#define TS_EACCES_STR       "Permission denied"

#define TS_EFAULT           (14)                         /* Linux errno extension */
#define TS_EFAULT_STR       "Bad address"

#define TS_EBUSY            (16)
#define TS_EBUSY_STR        "Device or resource busy"

#define TS_EEXIST           (17)
#define TS_EEXIST_STR       "File or device exists"

#define TS_ENODEV           (19)
#define TS_ENODEV_STR       "No such device"

#define TS_ENOTDIR          (20)
#define TS_ENOTDIR_STR      "Not a directory"

#define TS_EISDIR           (21)
#define TS_EISDIR_STR       "Is a directory"

#define TS_EINVAL           (22)
#define TS_EINVAL_STR       "Invalid argument"

#define TS_ENFILE           (23)
#define TS_ENFILE_STR       "File table overflow"

#define TS_EMFILE           (24)
#define TS_EMFILE_STR       "Too many open files"

#define TS_ENOTTY           (25)
#define TS_ENOTTY_STR       "Not a typewriter"

#define TS_ENOSPC           (28)
#define TS_ENOSPC_STR       "No space left on device"


#define	TS_ENAMETOOLONG     (36)
#define	TS_ENAMETOOLONG_STR "File name too long"

#define TS_ENOSYS           (38)
#define TS_ENOSYS_STR       "Invalid system call number"

#define TS_ELOOP            (40)
#define TS_ELOOP_STR        "Too many symbolic links encountered"

#define TS_EBADFD           (77)                         /* Linux errno extension */
#define TS_EBADFD_STR       "File descriptor in bad state"

#define	TS_EOPNOTSUPP	      (95)
#define TS_EOPNOTSUPP_STR   "Operation not supported on transport endpoint"

#define	TS_ECONNRESET       (104)
#define TS_ECONNRESET_STR   "Connection reset by peer"

#define	TS_ESHUTDOWN	      (108)
#define TS_ESHUTDOWN_STR    "Cannot send after transport endpoint shutdown"

#define	TS_EINPROGRESS      (115)
#define TS_EINPROGRESS_STR  "Operation now in progress"

#define TS_ETIMEDOUT        (116)
#define TS_ETIMEDOUT_STR    "Timed out"

#define TS_ECANCELED        (125)
#define TS_ECANCELED_STR    "Operation cancelled"

#define TS_ENOTSUP          (134)
#define TS_ENOTSUP_STR      "Not supported"

/* tdk 特定公共错误码 */
#define TS_ENONEXIST        (150)
#define TS_ENONEXIST_STR    "File or device non-exists"

#define TS_ENULLPTR         (151)
#define TS_ENULLPTR_STR     "Null pointer"

#define TS_EDISABLE         (152)
#define TS_EDISABLE_STR     "Device or resource is disable"

#define TS_ENOTREADY        (153)
#define TS_ENOTREADY_STR    "Device or resource not ready"

#define TS_ENOTCFG          (154)
#define TS_ENOTCFG_STR      "Resource not configured"

#define TS_EEMPTY           (155)
#define TS_EEMPTY_STR       "Buffer or FIFO empty"

#define TS_EFULL            (156)
#define TS_EFULL_STR        "Buffer or FIFO full"

#define TS_EVERIFY          (157)
#define TS_EVERIFY_STR      "Verify fail"

#define TS_ETSBUF           (158)
#define TS_ETSBUF_STR       "Ts_buffer invalid"

#define TS_EINVALDEVID      (160)
#define TS_EINVALDEVID_STR  "Invalid device ID"

#define TS_EINVALGRPID      (161)
#define TS_EINVALGRPID_STR  "Invalid group ID"

#define TS_EINVALCHNID      (162)
#define TS_EINVALCHNID_STR  "Invalid channel ID"

#define TS_EINVALSIZE       (163)
#define TS_EINVALSIZE_STR   "Invalid size"

#define TS_EINVALIOCTL      (164)
#define TS_EINVALIOCTL_STR  "Invalid ioctl command"

#define TS_ENOINIT          (165)
#define TS_ENOINIT_STR      "Uninitialized"


/* 模块错误码 
 * 32Bit : 31-24Bit     23-16Bit    15-8Bit     7-0Bit
 *          魔术字      日志等级    模块编码      错误码     
 */
#define TS_ERRNO_MAGIC      (0x5A)
#define TS_DEFINE_ERRNO(log_level, module_id, errno)    \
                        ((TS_ERRNO_MAGIC << 24) | (log_level << 16) | (module_id << 8) | (errno))


/* 此处统一对模块编码 */
#define TS_MODULE_SYS       (0x00)
#define TS_MODULE_BASE      (0x01)
#define TS_MODULE_CPL       (0x02)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __TS_ERRNO_H__ */



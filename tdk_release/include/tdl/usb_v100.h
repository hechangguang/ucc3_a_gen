/* 
 * tdl_usb_v00.h -- usb  驱动接口头文件

 */
#ifndef __TDL_USB_V00_H__
#define __TDL_USB_V00_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <config.h>
#include <base/ts_type.h>
#include <base/ts_bitop.h>
#include <base/ts_log.h>
#include <cpl/ts_cpl.h>

/* 当前模块日志等级 */
extern ts_s32 gs32_usb_log_lvl;


/* 对日志输出接口重定义，便于使用 */
#define usb_log_err(fmt, args ...) \
do { \
    ts_cpl_log(TS_LOG_ERR, gs32_usb_log_lvl, __func__, __LINE__, fmt, ## args); \
} \
while(0)

#define usb_log_warn(fmt, args ...) \
do { \
    ts_cpl_log(TS_LOG_WARNING, gs32_usb_log_lvl, __func__, __LINE__, fmt, ## args); \
} \
while(0)

#define usb_log_notice(fmt, args ...) \
do { \
    ts_cpl_log(TS_LOG_NOTICE, gs32_usb_log_lvl, __func__, __LINE__, fmt, ## args); \
} \
while(0)

#define usb_log_debug(fmt, args ...) \
do { \
    ts_cpl_log(TS_LOG_DEBUG, gs32_usb_log_lvl, __func__, __LINE__, fmt, ## args); \
} \
while(0)

#define usb_log_info(fmt, args ...) \
do { \
    ts_cpl_log(TS_LOG_INFO, gs32_usb_log_lvl, __func__, __LINE__, fmt, ## args); \
} \
while(0)


/* VID 定义 */
#define TS_TDK_USB_VID          (0x5474)

/* PID 定义, 共 VIDEO | AUDIO | MIDI | HID | MSC | CDC 这些类型 */
#define TS_TDK_USB_TUD_CDC          (0)
#define TS_TDK_USB_TUD_MSC          (1)
#define TS_TDK_USB_TUD_HID          (2)
#define TS_TDK_USB_TUD_MIDI         (3)
#define TS_TDK_USB_TUD_AUDIO        (4)
#define TS_TDK_USB_TUD_VIDEO        (5)
#define TS_TDK_USB_TUD_VENDOR       (6)

#define TS_TDK_USB_PID(ift)         ((0x5400 | TS_BIT(ift)) & 0xFFFF)

/* for bcdDevice */
#define TS_TDK_USB_BCD              (0x0100)        /* V100 */

/* for STRING_MANUFACTURER */
#define TS_TDK_USB_MANUFACTURER     "top-silicon"

/* serial number */
#define TS_TDK_USBUUID_LEN          (32)


#define TS_TDK_EP0_SIZE             (64)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __TDL_USB_V00_H__ */

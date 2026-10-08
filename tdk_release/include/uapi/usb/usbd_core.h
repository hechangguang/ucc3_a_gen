/*
 * Copyright (c) 2022, sakumisu
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef USBD_CORE_H
#define USBD_CORE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <base/ts_type.h>
#include <tdl/usb_v100.h>
#include <cpl/ts_cpl.h>

typedef struct usbd_endpoint usbd_edpt_s_t;
typedef struct usb_req_s usb_req_s_t;
typedef ts_void (*usbd_req_complete)(usbd_edpt_s_t *edpt, usb_req_s_t *req);

#include "usb_config.h"
#include "usb_util.h"
#include "usb_errno.h"
#include "usb_def.h"
#include "usb_list.h"
#include "usb_log.h"
#include "usb_dc.h"
#include "usb_memcpy.h"
#include "usb_version.h"

enum usbd_event_type {
    /* USB DCD IRQ */
    USBD_EVENT_ERROR,        /** USB error reported by the controller */
    USBD_EVENT_RESET,        /** USB reset */
    USBD_EVENT_SOF,          /** Start of Frame received */
    USBD_EVENT_CONNECTED,    /** USB connected*/
    USBD_EVENT_DISCONNECTED, /** USB disconnected */
    USBD_EVENT_SUSPEND,      /** USB connection suspended by the HOST */
    USBD_EVENT_RESUME,       /** USB connection resumed by the HOST */

    /* USB DEVICE STATUS */
    USBD_EVENT_CONFIGURED,        /** USB configuration done */
    USBD_EVENT_SET_INTERFACE,     /** USB interface selected */
    USBD_EVENT_SET_REMOTE_WAKEUP, /** USB set remote wakeup */
    USBD_EVENT_CLR_REMOTE_WAKEUP, /** USB clear remote wakeup */
    USBD_EVENT_INIT,              /** USB init done when call usbd_initialize */
    USBD_EVENT_DEINIT,            /** USB deinit done when call usbd_deinitialize */
    USBD_EVENT_UNKNOWN
};

typedef int (*usbd_request_handler)(struct usb_setup_packet *pst_setup, ts_u08 **ppu08_data, ts_u32 *pu32_len);
typedef ts_void (*usbd_endpoint_callback)(ts_u08 u08_ep_addr, ts_u32 u32_nbytes);
typedef ts_void (*usbd_notify_handler)(ts_u08 u08_event, ts_void *pv_arg);


#define USBD_VLEN_EN        (0x8000)
typedef struct usb_req_s {
    /* 队列控制 */
    ts_cpl_list_head_s_t list;

    /* dam 地址 */
    ts_dma_addr_t dma_addr;

    /* 可 cache 地址 */
    ts_void *buf;

    /* 包总大小 */
    ts_u32 length;

    /* 实际传输大小 */
    ts_u32 actual;

    /* 单次传输大小 */
    ts_u32 pre_cnt;

    /* 序列号 */
    ts_u16 stream_id;

    /* 是否需要 dma map */
    ts_u16 dma_sync;

    /* 状态 */
    int status;

    /* 此节点是否属于驱动内部 */
    ts_u32 req_owner;

    /* 节点完成回调 */
    usbd_req_complete complete;
} usb_req_s_t;


typedef struct usbd_endpoint {
    /* ep0 etc.. */
    const ts_char *name;

    ts_u08 claimed;
    ts_u08 enabled;
    ts_u08 ep_addr;
    ts_u08 rev;

    ts_u16 maxpacket;
    ts_u16 maxpacket_limit;
    ts_u16 max_streams;
    ts_u16 mult;

    /* 对应描述符 */
    const struct usb_endpoint_descriptor *desc;

    /* 节点管理 */
    ts_cpl_list_head_s_t queue;

    /* 私有变量 */
    ts_void *priv;
} usbd_edpt_s_t;

struct usbd_interface {
    usbd_request_handler class_interface_handler;
    usbd_request_handler class_endpoint_handler;
    usbd_request_handler vendor_handler;
    usbd_notify_handler notify_handler;
    const ts_u08 *hid_report_descriptor;
    ts_u32 hid_report_descriptor_len;
    ts_u08 intf_num;
};

struct usb_descriptor {
    const ts_u08 *(*device_descriptor_callback)(ts_u08 speed);
    const ts_u08 *(*config_descriptor_callback)(ts_u08 speed);
    const ts_u08 *(*device_quality_descriptor_callback)(ts_u08 speed);
    const ts_u08 *(*other_speed_descriptor_callback)(ts_u08 speed);
    const char *(*string_descriptor_callback)(ts_u08 speed, ts_u08 index);
    const struct usb_msosv1_descriptor *msosv1_descriptor;
    const struct usb_msosv2_descriptor *msosv2_descriptor;
    const struct usb_webusb_descriptor *webusb_url_descriptor;
    const struct usb_bos_descriptor *bos_descriptor;
};


#ifdef USBD_IRQHandler
#error USBD_IRQHandler is obsolete, please call USBD_IRQHandler(xxx) in your irq
#endif

ts_void usbd_desc_register(const struct usb_descriptor *desc);

ts_void usbd_add_interface(struct usbd_interface *intf);
ts_void usbd_add_endpoint(struct usbd_endpoint *ep);

ts_u32 usbd_get_speed(ts_void);
ts_u16 usbd_get_ep_mps(ts_u08 ep);
ts_u08 usbd_get_ep_mult(ts_u08 ep);
ts_bool usb_device_is_configured(ts_void);
ts_u08 *usbd_get_setup_buf(ts_void);
ts_bool usb_device_is_suspend(ts_void);
ts_void usbd_after_configuration(ts_u32 u32_setted);
int usbd_send_remote_wakeup(ts_void);

int usbd_initialize(ts_u32 u32_node_cnt, ts_void (*event_handler)(ts_u08 event));
int usbd_deinitialize(ts_void);
ts_s32 usbd_dequeue(ts_u08 u08_ep_addr, usb_req_s_t *pst_req);

#ifdef __cplusplus
}
#endif

#endif /* USBD_CORE_H */

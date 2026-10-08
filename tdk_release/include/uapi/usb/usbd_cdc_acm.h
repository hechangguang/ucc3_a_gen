/*
 * Copyright (c) 2022, sakumisu
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef USBD_CDC_ACM_H
#define USBD_CDC_ACM_H

#include <base/ts_type.h>
#include "usb_cdc.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Init cdc acm interface driver */
struct usbd_interface *usbd_cdc_acm_init_intf(struct usbd_interface *intf);

/* Setup request command callback api */
ts_void usbd_cdc_acm_set_line_coding(ts_u08 intf, struct cdc_line_coding *line_coding);
ts_void usbd_cdc_acm_get_line_coding(ts_u08 intf, struct cdc_line_coding *line_coding);
ts_void usbd_cdc_acm_set_dtr(ts_u08 intf, ts_bool dtr);
ts_void usbd_cdc_acm_set_rts(ts_u08 intf, ts_bool rts);
ts_void usbd_cdc_acm_send_break(ts_u08 intf);

#ifdef __cplusplus
}
#endif

#endif /* USBD_CDC_ACM_H */

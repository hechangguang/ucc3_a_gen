/*
 * Copyright (c) 2022, sakumisu
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <base/ts_type.h>
#include <cpl/ts_cpl.h>
#include <uapi/usb/usbd_core.h>
#include <uapi/usb/usb_cdc.h>
#include <uapi/usb/usbd_cdc_acm.h>

const char *stop_name[] = { "1", "1.5", "2" };
const char *parity_name[] = { "N", "O", "E", "M", "S" };

#ifndef LO_BYTE
#define LO_BYTE(x) ((ts_u08)(x & 0x00FF))
#endif

#ifndef HI_BYTE
#define HI_BYTE(x) ((ts_u08)((x & 0xFF00) >> 8))
#endif

static int cdc_acm_class_interface_request_handler(struct usb_setup_packet *setup, ts_u08 **data, ts_u32 *len)
{
    USB_LOG_DBG("CDC Class request: bRequest 0x%02x\r\n",setup->bRequest);

    struct cdc_line_coding line_coding;
    ts_bool dtr, rts;
    ts_u08 intf_num = LO_BYTE(setup->wIndex);

    switch (setup->bRequest) {
        case CDC_REQUEST_SET_LINE_CODING:

            /*******************************************************************************/
            /* Line Coding Structure                                                       */
            /*-----------------------------------------------------------------------------*/
            /* Offset | Field       | Size | Value  | Description                          */
            /* 0      | dwDTERate   |   4  | Number |Data terminal rate, in bits per second*/
            /* 4      | bCharFormat |   1  | Number | Stop bits                            */
            /*                                        0 - 1 Stop bit                       */
            /*                                        1 - 1.5 Stop bits                    */
            /*                                        2 - 2 Stop bits                      */
            /* 5      | bParityType |  1   | Number | Parity                               */
            /*                                        0 - None                             */
            /*                                        1 - Odd                              */
            /*                                        2 - Even                             */
            /*                                        3 - Mark                             */
            /*                                        4 - Space                            */
            /* 6      | bDataBits  |   1   | Number Data bits (5, 6, 7, 8 or 16).          */
            /*******************************************************************************/
            ts_cpl_memcpy(&line_coding, *data, setup->wLength);
            USB_LOG_DBG("Set intf:%d linecoding <%d %d %s %s>\r\n",
                        intf_num,
                        line_coding.dwDTERate,
                        line_coding.bDataBits,
                        parity_name[line_coding.bParityType],
                        stop_name[line_coding.bCharFormat]);

            usbd_cdc_acm_set_line_coding(intf_num, &line_coding);
            break;

        case CDC_REQUEST_SET_CONTROL_LINE_STATE:
            dtr = (setup->wValue & 0x0001);
            rts = (setup->wValue & 0x0002);
            USB_LOG_DBG("Set intf:%d DTR 0x%x,RTS 0x%x\r\n",
                        intf_num,
                        dtr,
                        rts);
            usbd_cdc_acm_set_dtr(intf_num, dtr);
            usbd_cdc_acm_set_rts(intf_num, rts);
            break;

        case CDC_REQUEST_GET_LINE_CODING:
            usbd_cdc_acm_get_line_coding(intf_num, &line_coding);
            ts_cpl_memcpy(*data, &line_coding, 7);
            *len = 7;
            USB_LOG_DBG("Get intf:%d linecoding %d %d %d %d\r\n",
                        intf_num,
                        line_coding.dwDTERate,
                        line_coding.bCharFormat,
                        line_coding.bParityType,
                        line_coding.bDataBits);
            break;
        case CDC_REQUEST_SEND_BREAK:
            usbd_cdc_acm_send_break(intf_num);
            break;
        default:
            USB_LOG_WRN("Unhandled CDC Class bRequest 0x%02x\r\n", setup->bRequest);
            return -1;
    }

    return 0;
}

struct usbd_interface *usbd_cdc_acm_init_intf(struct usbd_interface *intf)
{
    intf->class_interface_handler = cdc_acm_class_interface_request_handler;
    intf->class_endpoint_handler = ts_null;
    intf->vendor_handler = ts_null;
    intf->notify_handler = ts_null;

    return intf;
}

TS_WEAK ts_void usbd_cdc_acm_set_line_coding(ts_u08 intf, struct cdc_line_coding *line_coding)
{
    (void)intf;
    (void)line_coding;
}

TS_WEAK ts_void usbd_cdc_acm_get_line_coding(ts_u08 intf, struct cdc_line_coding *line_coding)
{
    (void)intf;

    line_coding->dwDTERate = 2000000;
    line_coding->bDataBits = 8;
    line_coding->bParityType = 0;
    line_coding->bCharFormat = 0;
}

TS_WEAK ts_void usbd_cdc_acm_set_dtr(ts_u08 intf, ts_bool dtr)
{
    (void)intf;
    (void)dtr;
}

TS_WEAK ts_void usbd_cdc_acm_set_rts(ts_u08 intf, ts_bool rts)
{
    (void)intf;
    (void)rts;
}

TS_WEAK ts_void usbd_cdc_acm_send_break(ts_u08 intf)
{
    (void)intf;
}

/*
 * Copyright (c) 2022, sakumisu
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef USB_DC_H
#define USB_DC_H

//#include <stdint.h>
#include <base/ts_type.h>
#include "usbd_core.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief init device controller registers.
 * @return On success will return 0, and others indicate fail.
 */
ts_s32 usb_dc_init(ts_u32 u32_node_cnt);

/**
 * @brief deinit device controller registers.
 * @return On success will return 0, and others indicate fail.
 */
ts_s32 usb_dc_deinit(ts_void);

/**
 * @brief Set USB device address
 *
 * @param[in] addr Device address
 *
 * @return On success will return 0, and others indicate fail.
 */
ts_s32 usbd_set_address(const ts_u08 u08_dev_addr);

/**
 * @brief Set remote wakeup feature
 *
 * @return On success will return 0, and others indicate fail.
 */
ts_s32 usbd_set_remote_wakeup(ts_void);

/**
 * @brief Get USB device speed
 *
 * @param[in] busid bus index
 *
 * @return port speed, USB_SPEED_LOW or USB_SPEED_FULL or USB_SPEED_HIGH
 */
ts_u08 usbd_get_port_speed(ts_void);

/**
 * @brief configure and enable endpoint.
 *
 * @param [in]  ep_cfg Endpoint config.
 *
 * @return On success will return 0, and others indicate fail.
 */
ts_s32 usbd_ep_open(const struct usb_endpoint_descriptor *pst_ep_desc);

/**
 * @brief Disable the selected endpoint
 *
 * @param[in] ep Endpoint address
 *
 * @return On success will return 0, and others indicate fail.
 */
ts_s32 usbd_ep_close(const ts_u08 u08_ep_addr);

/**
 * @brief Set stall condition for the selected endpoint
 *
 * @param[in] ep Endpoint address
 *
 *
 * @return On success will return 0, and others indicate fail.
 */
ts_s32 usbd_ep_set_stall(const ts_u08 u08_ep_addr);

/**
 * @brief Clear stall condition for the selected endpoint
 *
 * @param[in] ep Endpoint address corresponding to the one
 *               listed in the device configuration table
 *
 * @return On success will return 0, and others indicate fail.
 */
ts_s32 usbd_ep_clear_stall(const ts_u08 u08_ep_addr);

/**
 * @brief Check if the selected endpoint is stalled
 *
 * @param[in]  ep       Endpoint address
 *
 * @param[out] stalled  Endpoint stall status
 *
 * @return On success will return 0, and others indicate fail.
 */
ts_s32 usbd_ep_is_stalled(const ts_u08 u08_ep_addr, ts_u08 *pu08_stalled);


#define USBD_PERF_MODE      (0)
#define USBD_NORMAL_MODE    (1)
#define USBD_EXCHG_MODE     (2)

ts_void usbd_ep_set_mode(ts_u32 u32_bw_mode);

ts_s32 usbd_ep_get_status(const ts_u08 u08_ep_addr);
ts_void usbd_set_plug(ts_u32 u32_unplug);

ts_void usbd_set_uvc_hook(ts_void (*uvc_open)(ts_u08), ts_void (*uvc_close)(ts_u08), ts_u32 u32_hdr_size, ts_u32 u32_payload_size);
ts_void usbd_start_uvc(ts_void);
ts_void usbd_stop_uvc(ts_void);

ts_s32 usbd_ep_queue(const ts_u08 u08_ep_addr, usb_req_s_t *pst_req);
ts_s32 usbd_ep_dequeue(const ts_u08 u08_ep_addr, usb_req_s_t *pst_req);

ts_u32 usb_ep_chk_dma_ready(ts_u08 u08_ep_addr);

/* u32_str:0..7; def:3 */
ts_void usbd_set_o_strong(ts_u32 u32_str);


/**
 * @brief Setup in ep transfer setting and start transfer.
 *
 * This function is asynchronous.
 * This function is similar to uart with tx dma.
 *
 * This function is called to write data to the specified endpoint. The
 * supplied usbd_endpoint_callback function will be called when data is transmitted
 * out.
 *
 * @param[in]  ep        Endpoint address corresponding to the one
 *                       listed in the device configuration table
 * @param[in]  req       Pointer to request to write
 * @return 0 on success, negative errno code on fail.
 */
ts_s32 usbd_ep_start_write(const ts_u08 u08_ep_addr, const ts_u08 *pu08_data, ts_u32 u32_data_len, usbd_req_complete pfn_cb);


/**
 * @brief Setup out ep transfer setting and start transfer.
 *
 * This function is asynchronous.
 * This function is similar to uart with rx dma.
 *
 * This function is called to read data to the specified endpoint. The
 * supplied usbd_endpoint_callback function will be called when data is received
 * in.
 *
 * @param[in]  ep        Endpoint address corresponding to the one
 *                       listed in the device configuration table
 * @param[in]  data      Pointer to data to read
 * @param[in]  data_len  Max length of the data requested to read.
 *
 * @return 0 on success, negative errno code on fail.
 */
ts_s32 usbd_ep_start_read(const ts_u08 u08_epaddr, ts_u08 *pu08_data, ts_u32 u32_data_len, usbd_req_complete pfn_cb);

/* usb dcd irq callback, called by user */

/**
 * @brief Usb connect irq callback.
 */
ts_void usbd_event_connect_handler(ts_void);

/**
 * @brief Usb disconnect irq callback.
 */
ts_void usbd_event_disconnect_handler(ts_void);

/**
 * @brief Usb resume irq callback.
 */
ts_void usbd_event_resume_handler(ts_void);

/**
 * @brief Usb suspend irq callback.
 */
ts_void usbd_event_suspend_handler(ts_void);

/**
 * @brief Usb reset irq callback.
 */
ts_void usbd_event_reset_handler(ts_void);

/**
 * @brief Usb setup packet recv irq callback.
 * @param[in]  psetup  setup packet.
 */
ts_void usbd_event_ep0_setup_complete_handler(ts_u08 *pu08_setup);

/**
 * @brief In ep transfer complete irq callback.
 * @param[in]  ep        Endpoint address corresponding to the one
 *                       listed in the device configuration table
 * @param[in]  nbytes    How many nbytes have transferred.
 */
ts_void usbd_event_ep_in_complete_handler(ts_u08 u08_ep_addr, ts_u32 u32_nbytes);

/**
 * @brief Out ep transfer complete irq callback.
 * @param[in]  ep        Endpoint address corresponding to the one
 *                       listed in the device configuration table
 * @param[in]  nbytes    How many nbytes have transferred.
 */
ts_void usbd_event_ep_out_complete_handler(ts_u08 u08_ep_addr, ts_u32 u32_nbytes);

#ifdef CONFIG_USBDEV_TEST_MODE
/**
 * @brief Usb execute test mode
 * @param[in]  busid     device busid
 * @param[in]  test_mode usb test mode
 */
ts_void usbd_execute_test_mode(, ts_u08 test_mode);
#endif

/* called by user */
ts_void USBD_IRQHandler(ts_void);

#ifdef __cplusplus
}
#endif

#endif /* USB_DC_H */

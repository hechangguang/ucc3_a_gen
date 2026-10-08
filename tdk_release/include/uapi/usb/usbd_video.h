/*
 * Copyright (c) 2022, sakumisu
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef USBD_VIDEO_H
#define USBD_VIDEO_H

#include "usb_video.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Init video interface driver */
struct usbd_interface *usbd_video_init_intf(struct usbd_interface *intf,
                                            ts_u32 dwFrameInterval,
                                            ts_u32 dwMaxVideoFrameSize,
                                            ts_u32 dwMaxPayloadTransferSize);

ts_void usbd_video_set_format(ts_u08 u08_format_idx, ts_u08 u08_frame_idx);

ts_void usbd_video_open(ts_u08 intf);
ts_void usbd_video_close(ts_u08 intf);

ts_bool usbd_video_stream_split_transfer(ts_u08 ep);
ts_s32 usbd_video_stream_start_write(ts_u08 ep, ts_u08 *ep_buf0, ts_u08 *ep_buf1, ts_u32 ep_bufsize, ts_u08 *stream_buf, ts_u32 stream_len);

#ifdef __cplusplus
}
#endif

#endif /* USBD_VIDEO_H */

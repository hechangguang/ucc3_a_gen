/*
 * Copyright (c) 2022, sakumisu
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <uapi/usb/usbd_core.h>
#include <uapi/usb/usbd_video.h>

struct video_entity_info {
    ts_u08 bDescriptorSubtype;
    ts_u08 bEntityId;
    ts_u16 wTerminalType;
};

struct usbd_video_priv {
    struct video_probe_and_commit_controls probe;
    struct video_probe_and_commit_controls commit;
    ts_u08 power_mode;
    ts_u08 error_code;
    struct video_entity_info info[3];
    ts_u08 *ep_buf0;
    ts_u08 *ep_buf1;
    ts_bool ep_buf0_ready;
    ts_bool ep_buf1_ready;
    ts_u32 ep_buf0_len;
    ts_u32 ep_buf1_len;
    ts_u08 ep_buf_idx;

    struct usb_req_s req_buf0;
    struct usb_req_s req_buf1;

    ts_bool stream_finish;
    ts_u32 max_packets;
    ts_u08 *stream_buf;
    ts_u32 stream_len;
    ts_u32 stream_offset;
    ts_u08 stream_frameid;
    ts_u32 stream_headerlen;
} g_usbd_video;

static int usbd_video_control_request_handler(struct usb_setup_packet *setup, ts_u08 **data, ts_u32 *len)
{
    ts_u08 control_selector = (ts_u08)(setup->wValue >> 8);

    switch (control_selector) {
        case VIDEO_VC_VIDEO_POWER_MODE_CONTROL:
            switch (setup->bRequest) {
                case VIDEO_REQUEST_SET_CUR:
                    break;
                case VIDEO_REQUEST_GET_CUR:
                    break;
                case VIDEO_REQUEST_GET_INFO:
                    break;
                default:
                    USB_LOG_WRN("Unhandled Video Class bRequest 0x%02x\r\n", setup->bRequest);
                    return -1;
            }

            break;
        case VIDEO_VC_REQUEST_ERROR_CODE_CONTROL:
            switch (setup->bRequest) {
                case VIDEO_REQUEST_GET_CUR:
                    (*data)[0] = 0x06;
                    *len = 1;
                    break;
                case VIDEO_REQUEST_GET_INFO:
                    break;
                default:
                    USB_LOG_WRN("Unhandled Video Class bRequest 0x%02x\r\n", setup->bRequest);
                    return -1;
            }

            break;
        default:
            break;
    }

    return 0;
}

static int usbd_video_control_unit_terminal_request_handler(struct usb_setup_packet *setup, ts_u08 **data, ts_u32 *len)
{
    ts_u08 entity_id = (ts_u08)(setup->wIndex >> 8);
    ts_u08 control_selector = (ts_u08)(setup->wValue >> 8);

    for (ts_u08 i = 0; i < 3; i++) {
        struct video_entity_info *entity_info = &g_usbd_video.info[i];
        if (entity_info->bEntityId == entity_id) {
            switch (entity_info->bDescriptorSubtype) {
                case VIDEO_VC_HEADER_DESCRIPTOR_SUBTYPE:
                    break;
                case VIDEO_VC_INPUT_TERMINAL_DESCRIPTOR_SUBTYPE:
                    if (entity_info->wTerminalType == VIDEO_ITT_CAMERA) {
                        switch (control_selector) {
                            case VIDEO_CT_AE_MODE_CONTROL:
                                switch (setup->bRequest) {
                                    case VIDEO_REQUEST_GET_CUR:
                                        (*data)[0] = 0x08;
                                        *len = 1;
                                        break;
                                    default:
                                        USB_LOG_WRN("Unhandled Video Class bRequest 0x%02x\r\n", setup->bRequest);
                                        return -1;
                                }
                                break;
                            case VIDEO_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL:
                                switch (setup->bRequest) {
                                    case VIDEO_REQUEST_GET_CUR: {
                                        ts_u32 dwExposureTimeAbsolute = 2500;
                                        ts_cpl_memcpy(*data, (ts_u08 *)&dwExposureTimeAbsolute, 4);
                                        *len = 4;
                                    } break;
                                    case VIDEO_REQUEST_GET_MIN: {
                                        ts_u32 dwExposureTimeAbsolute = 5; //0.0005sec
                                        ts_cpl_memcpy(*data, (ts_u08 *)&dwExposureTimeAbsolute, 4);
                                        *len = 4;
                                    } break;
                                    case VIDEO_REQUEST_GET_MAX: {
                                        ts_u32 dwExposureTimeAbsolute = 2500; //0.2500sec
                                        ts_cpl_memcpy(*data, (ts_u08 *)&dwExposureTimeAbsolute, 4);
                                        *len = 4;
                                    } break;
                                    case VIDEO_REQUEST_GET_RES: {
                                        ts_u32 dwExposureTimeAbsolute = 5; //0.0005sec
                                        ts_cpl_memcpy(*data, (ts_u08 *)&dwExposureTimeAbsolute, 4);
                                        *len = 4;
                                    } break;
                                    case VIDEO_REQUEST_GET_INFO:
                                        (*data)[0] = 0x03; //struct video_camera_capabilities
                                        *len = 1;
                                        break;
                                    case VIDEO_REQUEST_GET_DEF: {
                                        ts_u32 dwExposureTimeAbsolute = 2500; //0.2500sec
                                        ts_cpl_memcpy(*data, (ts_u08 *)&dwExposureTimeAbsolute, 4);
                                        *len = 4;
                                    } break;
                                    default:
                                        USB_LOG_WRN("Unhandled Video Class bRequest 0x%02x\r\n", setup->bRequest);
                                        return -1;
                                }
                                break;
                            case VIDEO_CT_FOCUS_ABSOLUTE_CONTROL:
                                switch (setup->bRequest) {
                                    case VIDEO_REQUEST_GET_CUR: {
                                        ts_u16 wFocusAbsolute = 0x0080;
                                        ts_cpl_memcpy(*data, (ts_u08 *)&wFocusAbsolute, 2);
                                        *len = 2;
                                    } break;
                                    case VIDEO_REQUEST_GET_MIN: {
                                        ts_u16 wFocusAbsolute = 0;
                                        ts_cpl_memcpy(*data, (ts_u08 *)&wFocusAbsolute, 2);
                                        *len = 2;
                                    } break;
                                    case VIDEO_REQUEST_GET_MAX: {
                                        ts_u16 wFocusAbsolute = 0x00ff;
                                        ts_cpl_memcpy(*data, (ts_u08 *)&wFocusAbsolute, 2);
                                        *len = 2;
                                    } break;
                                    case VIDEO_REQUEST_GET_RES: {
                                        ts_u16 wFocusAbsolute = 0x0001;
                                        ts_cpl_memcpy(*data, (ts_u08 *)&wFocusAbsolute, 2);
                                        *len = 2;
                                    } break;
                                    case VIDEO_REQUEST_GET_INFO:
                                        (*data)[0] = 0x03; //struct video_camera_capabilities
                                        *len = 1;
                                        break;
                                    case VIDEO_REQUEST_GET_DEF: {
                                        ts_u16 wFocusAbsolute = 0x0080;
                                        ts_cpl_memcpy(*data, (ts_u08 *)&wFocusAbsolute, 2);
                                        *len = 2;
                                    } break;
                                    default:
                                        USB_LOG_WRN("Unhandled Video Class bRequest 0x%02x\r\n", setup->bRequest);
                                        return -1;
                                }
                                break;
                            case VIDEO_CT_ZOOM_ABSOLUTE_CONTROL:
                                switch (setup->bRequest) {
                                    case VIDEO_REQUEST_GET_CUR: {
                                        ts_u16 wObjectiveFocalLength = 0x0064;
                                        ts_cpl_memcpy(*data, (ts_u08 *)&wObjectiveFocalLength, 2);
                                        *len = 2;
                                    } break;
                                    case VIDEO_REQUEST_GET_MIN: {
                                        ts_u16 wObjectiveFocalLength = 0x0064;
                                        ts_cpl_memcpy(*data, (ts_u08 *)&wObjectiveFocalLength, 2);
                                        *len = 2;
                                    } break;
                                    case VIDEO_REQUEST_GET_MAX: {
                                        ts_u16 wObjectiveFocalLength = 0x00c8;
                                        ts_cpl_memcpy(*data, (ts_u08 *)&wObjectiveFocalLength, 2);
                                        *len = 2;
                                    } break;
                                    case VIDEO_REQUEST_GET_RES: {
                                        ts_u16 wObjectiveFocalLength = 0x0001;
                                        ts_cpl_memcpy(*data, (ts_u08 *)&wObjectiveFocalLength, 2);
                                        *len = 2;
                                    } break;
                                    case VIDEO_REQUEST_GET_INFO:
                                        (*data)[0] = 0x03; //struct video_camera_capabilities
                                        *len = 1;
                                        break;
                                    case VIDEO_REQUEST_GET_DEF: {
                                        ts_u16 wObjectiveFocalLength = 0x0064;
                                        ts_cpl_memcpy(*data, (ts_u08 *)&wObjectiveFocalLength, 2);
                                        *len = 2;
                                    } break;
                                    default:
                                        USB_LOG_WRN("Unhandled Video Class bRequest 0x%02x\r\n", setup->bRequest);
                                        return -1;
                                }
                                break;
                            case VIDEO_CT_ROLL_ABSOLUTE_CONTROL:
                                switch (setup->bRequest) {
                                    case VIDEO_REQUEST_GET_CUR: {
                                        ts_u16 wRollAbsolute = 0x0000;
                                        ts_cpl_memcpy(*data, (ts_u08 *)&wRollAbsolute, 2);
                                        *len = 2;
                                    } break;
                                    case VIDEO_REQUEST_GET_MIN: {
                                        ts_u16 wRollAbsolute = 0x0000;
                                        ts_cpl_memcpy(*data, (ts_u08 *)&wRollAbsolute, 2);
                                        *len = 2;
                                    } break;
                                    case VIDEO_REQUEST_GET_MAX: {
                                        ts_u16 wRollAbsolute = 0x00ff;
                                        ts_cpl_memcpy(*data, (ts_u08 *)&wRollAbsolute, 2);
                                        *len = 2;
                                    } break;
                                    case VIDEO_REQUEST_GET_RES: {
                                        ts_u16 wRollAbsolute = 0x0001;
                                        ts_cpl_memcpy(*data, (ts_u08 *)&wRollAbsolute, 2);
                                        *len = 2;
                                    } break;
                                    case VIDEO_REQUEST_GET_INFO:
                                        (*data)[0] = 0x03; //struct video_camera_capabilities
                                        *len = 1;
                                        break;
                                    case VIDEO_REQUEST_GET_DEF: {
                                        ts_u16 wRollAbsolute = 0x0000;
                                        ts_cpl_memcpy(*data, (ts_u08 *)&wRollAbsolute, 2);
                                        *len = 2;
                                    } break;
                                    default:
                                        USB_LOG_WRN("Unhandled Video Class bRequest 0x%02x\r\n", setup->bRequest);
                                        return -1;
                                }
                                break;
                            case VIDEO_CT_FOCUS_AUTO_CONTROL:
                                switch (setup->bRequest) {
                                    case VIDEO_REQUEST_GET_CUR: {
                                        ts_u16 wFocusAuto = 0x0000;
                                        ts_cpl_memcpy(*data, (ts_u08 *)&wFocusAuto, 2);
                                        *len = 2;
                                    } break;
                                    default:
                                        USB_LOG_WRN("Unhandled Video Class bRequest 0x%02x\r\n", setup->bRequest);
                                        return -1;
                                }
                                break;
                            default:
                                USB_LOG_WRN("Unhandled Video Class control selector 0x%02x\r\n", control_selector);
                                return -1;
                        }
                    } else {
                        USB_LOG_WRN("Unhandled Video Class wTerminalType 0x%02x\r\n", entity_info->wTerminalType);
                        return -2;
                    }
                    break;
                case VIDEO_VC_OUTPUT_TERMINAL_DESCRIPTOR_SUBTYPE:
                    break;
                case VIDEO_VC_SELECTOR_UNIT_DESCRIPTOR_SUBTYPE:
                    break;
                case VIDEO_VC_PROCESSING_UNIT_DESCRIPTOR_SUBTYPE:
                    switch (control_selector) {
                        case VIDEO_PU_BACKLIGHT_COMPENSATION_CONTROL:
                            switch (setup->bRequest) {
                                case VIDEO_REQUEST_GET_CUR: {
                                    ts_u16 wBacklightCompensation = 0x0004;
                                    ts_cpl_memcpy(*data, (ts_u08 *)&wBacklightCompensation, 2);
                                    *len = 2;
                                } break;
                                case VIDEO_REQUEST_GET_MIN: {
                                    ts_u16 wBacklightCompensation = 0;
                                    ts_cpl_memcpy(*data, (ts_u08 *)&wBacklightCompensation, 2);
                                    *len = 2;
                                } break;
                                case VIDEO_REQUEST_GET_MAX: {
                                    ts_u16 wBacklightCompensation = 8;
                                    ts_cpl_memcpy(*data, (ts_u08 *)&wBacklightCompensation, 2);
                                    *len = 2;
                                } break;
                                case VIDEO_REQUEST_GET_RES: {
                                    ts_u16 wBacklightCompensation = 1;
                                    ts_cpl_memcpy(*data, (ts_u08 *)&wBacklightCompensation, 2);
                                    *len = 2;
                                } break;
                                case VIDEO_REQUEST_GET_INFO:
                                    (*data)[0] = 0x03; //struct video_camera_capabilities
                                    *len = 1;
                                    break;
                                case VIDEO_REQUEST_GET_DEF: {
                                    ts_u16 wBacklightCompensation = 4;
                                    ts_cpl_memcpy(*data, (ts_u08 *)&wBacklightCompensation, 2);
                                    *len = 2;
                                } break;
                                default:
                                    USB_LOG_WRN("Unhandled Video Class bRequest 0x%02x\r\n", setup->bRequest);
                                    return -1;
                            }
                            break;
                        case VIDEO_PU_BRIGHTNESS_CONTROL:
                            switch (setup->bRequest) {
                                case VIDEO_REQUEST_SET_CUR: {
                                    //ts_u16 wBrightness = (ts_u16)(*data)[1] << 8 | (ts_u16)(*data)[0];
                                } break;
                                case VIDEO_REQUEST_GET_CUR: {
                                    ts_u16 wBrightness = 0x0080;
                                    ts_cpl_memcpy(*data, (ts_u08 *)&wBrightness, 2);
                                    *len = 2;
                                } break;
                                case VIDEO_REQUEST_GET_MIN: {
                                    ts_u16 wBrightness = 0x0001;
                                    ts_cpl_memcpy(*data, (ts_u08 *)&wBrightness, 2);
                                    *len = 2;
                                } break;
                                case VIDEO_REQUEST_GET_MAX: {
                                    ts_u16 wBrightness = 0x00ff;
                                    ts_cpl_memcpy(*data, (ts_u08 *)&wBrightness, 2);
                                    *len = 2;
                                } break;
                                case VIDEO_REQUEST_GET_RES: {
                                    ts_u16 wBrightness = 0x0001;
                                    ts_cpl_memcpy(*data, (ts_u08 *)&wBrightness, 2);
                                    *len = 2;
                                } break;
                                case VIDEO_REQUEST_GET_INFO:
                                    (*data)[0] = 0x03; //struct video_camera_capabilities
                                    *len = 1;
                                    break;
                                case VIDEO_REQUEST_GET_DEF: {
                                    ts_u16 wBrightness = 0x0080;
                                    ts_cpl_memcpy(*data, (ts_u08 *)&wBrightness, 2);
                                    *len = 2;
                                } break;
                                default:
                                    USB_LOG_WRN("Unhandled Video Class bRequest 0x%02x\r\n", setup->bRequest);
                                    return -1;
                            }
                            break;
                        case VIDEO_PU_CONTRAST_CONTROL:
                            switch (setup->bRequest) {
                                case VIDEO_REQUEST_GET_CUR: {
                                    ts_u16 wContrast = 0x0080;
                                    ts_cpl_memcpy(*data, (ts_u08 *)&wContrast, 2);
                                    *len = 2;
                                } break;
                                case VIDEO_REQUEST_GET_MIN: {
                                    ts_u16 wContrast = 0x0001;
                                    ts_cpl_memcpy(*data, (ts_u08 *)&wContrast, 2);
                                    *len = 2;
                                } break;
                                case VIDEO_REQUEST_GET_MAX: {
                                    ts_u16 wContrast = 0x00ff;
                                    ts_cpl_memcpy(*data, (ts_u08 *)&wContrast, 2);
                                    *len = 2;
                                } break;
                                case VIDEO_REQUEST_GET_RES: {
                                    ts_u16 wContrast = 0x0001;
                                    ts_cpl_memcpy(*data, (ts_u08 *)&wContrast, 2);
                                    *len = 2;
                                } break;
                                case VIDEO_REQUEST_GET_INFO:
                                    (*data)[0] = 0x03; //struct video_camera_capabilities
                                    *len = 1;
                                    break;
                                case VIDEO_REQUEST_GET_DEF: {
                                    ts_u16 wContrast = 0x0080;
                                    ts_cpl_memcpy(*data, (ts_u08 *)&wContrast, 2);
                                    *len = 2;
                                } break;
                                default:
                                    USB_LOG_WRN("Unhandled Video Class bRequest 0x%02x\r\n", setup->bRequest);
                                    return -1;
                            }
                            break;
                        case VIDEO_PU_HUE_CONTROL:
                            switch (setup->bRequest) {
                                case VIDEO_REQUEST_GET_CUR: {
                                    ts_u16 wHue = 0x0080;
                                    ts_cpl_memcpy(*data, (ts_u08 *)&wHue, 2);
                                    *len = 2;
                                } break;
                                case VIDEO_REQUEST_GET_MIN: {
                                    ts_u16 wHue = 0x0001;
                                    ts_cpl_memcpy(*data, (ts_u08 *)&wHue, 2);
                                    *len = 2;
                                } break;
                                case VIDEO_REQUEST_GET_MAX: {
                                    ts_u16 wHue = 0x00ff;
                                    ts_cpl_memcpy(*data, (ts_u08 *)&wHue, 2);
                                    *len = 2;
                                } break;
                                case VIDEO_REQUEST_GET_RES: {
                                    ts_u16 wHue = 0x0001;
                                    ts_cpl_memcpy(*data, (ts_u08 *)&wHue, 2);
                                    *len = 2;
                                } break;
                                case VIDEO_REQUEST_GET_INFO:
                                    (*data)[0] = 0x03; //struct video_camera_capabilities
                                    *len = 1;
                                    break;
                                case VIDEO_REQUEST_GET_DEF: {
                                    ts_u16 wHue = 0x0080;
                                    ts_cpl_memcpy(*data, (ts_u08 *)&wHue, 2);
                                    *len = 2;
                                } break;
                                default:
                                    USB_LOG_WRN("Unhandled Video Class bRequest 0x%02x\r\n", setup->bRequest);
                                    return -1;
                            }
                            break;
                        case VIDEO_PU_SATURATION_CONTROL:
                            switch (setup->bRequest) {
                                case VIDEO_REQUEST_GET_MIN: {
                                    ts_u16 wSaturation = 0x0001;
                                    ts_cpl_memcpy(*data, (ts_u08 *)&wSaturation, 2);
                                    *len = 2;
                                } break;
                                case VIDEO_REQUEST_GET_MAX: {
                                    ts_u16 wSaturation = 0x00ff;
                                    ts_cpl_memcpy(*data, (ts_u08 *)&wSaturation, 2);
                                    *len = 2;
                                } break;
                                case VIDEO_REQUEST_GET_RES: {
                                    ts_u16 wSaturation = 0x0001;
                                    ts_cpl_memcpy(*data, (ts_u08 *)&wSaturation, 2);
                                    *len = 2;
                                } break;
                                case VIDEO_REQUEST_GET_INFO:
                                    (*data)[0] = 0x03; //struct video_camera_capabilities
                                    *len = 1;
                                    break;
                                case VIDEO_REQUEST_GET_DEF: {
                                    ts_u16 wSaturation = 0x0080;
                                    ts_cpl_memcpy(*data, (ts_u08 *)&wSaturation, 2);
                                    *len = 2;
                                } break;
                                default:
                                    USB_LOG_WRN("Unhandled Video Class bRequest 0x%02x\r\n", setup->bRequest);
                                    return -1;
                            }
                            break;
                        case VIDEO_PU_SHARPNESS_CONTROL:
                            switch (setup->bRequest) {
                                case VIDEO_REQUEST_GET_MIN: {
                                    ts_u16 wSharpness = 0x0001;
                                    ts_cpl_memcpy(*data, (ts_u08 *)&wSharpness, 2);
                                    *len = 2;
                                } break;
                                case VIDEO_REQUEST_GET_MAX: {
                                    ts_u16 wSharpness = 0x00ff;
                                    ts_cpl_memcpy(*data, (ts_u08 *)&wSharpness, 2);
                                    *len = 2;
                                } break;
                                case VIDEO_REQUEST_GET_RES: {
                                    ts_u16 wSharpness = 0x0001;
                                    ts_cpl_memcpy(*data, (ts_u08 *)&wSharpness, 2);
                                    *len = 2;
                                } break;
                                case VIDEO_REQUEST_GET_INFO:
                                    (*data)[0] = 0x03; //struct video_camera_capabilities
                                    *len = 1;
                                    break;
                                case VIDEO_REQUEST_GET_DEF: {
                                    ts_u16 wSharpness = 0x0080;
                                    ts_cpl_memcpy(*data, (ts_u08 *)&wSharpness, 2);
                                    *len = 2;
                                } break;
                                default:
                                    USB_LOG_WRN("Unhandled Video Class bRequest 0x%02x\r\n", setup->bRequest);
                                    return -1;
                            }
                            break;
                        case VIDEO_PU_GAIN_CONTROL:
                            switch (setup->bRequest) {
                                case VIDEO_REQUEST_GET_MIN: {
                                    ts_u16 wGain = 0;
                                    ts_cpl_memcpy(*data, (ts_u08 *)&wGain, 2);
                                    *len = 2;
                                } break;
                                case VIDEO_REQUEST_GET_MAX: {
                                    ts_u16 wGain = 255;
                                    ts_cpl_memcpy(*data, (ts_u08 *)&wGain, 2);
                                    *len = 2;
                                } break;
                                case VIDEO_REQUEST_GET_RES: {
                                    ts_u16 wGain = 1;
                                    ts_cpl_memcpy(*data, (ts_u08 *)&wGain, 2);
                                    *len = 2;
                                } break;
                                case VIDEO_REQUEST_GET_INFO:
                                    (*data)[0] = 0x03; //struct video_camera_capabilities
                                    *len = 1;
                                    break;
                                case VIDEO_REQUEST_GET_DEF: {
                                    ts_u16 wGain = 255;
                                    ts_cpl_memcpy(*data, (ts_u08 *)&wGain, 2);
                                    *len = 2;
                                } break;
                                default:
                                    USB_LOG_WRN("Unhandled Video Class bRequest 0x%02x\r\n", setup->bRequest);
                                    return -1;
                            }
                            break;
                        case VIDEO_PU_WHITE_BALANCE_TEMPERATURE_CONTROL:
                            switch (setup->bRequest) {
                                case VIDEO_REQUEST_GET_CUR: {
                                    ts_u16 wWhiteBalance_Temprature = 417;
                                    ts_cpl_memcpy(*data, (ts_u08 *)&wWhiteBalance_Temprature, 2);
                                    *len = 2;
                                } break;
                                case VIDEO_REQUEST_GET_MIN: {
                                    ts_u16 wWhiteBalance_Temprature = 300;
                                    ts_cpl_memcpy(*data, (ts_u08 *)&wWhiteBalance_Temprature, 2);
                                    *len = 2;
                                } break;
                                case VIDEO_REQUEST_GET_MAX: {
                                    ts_u16 wWhiteBalance_Temprature = 600;
                                    ts_cpl_memcpy(*data, (ts_u08 *)&wWhiteBalance_Temprature, 2);
                                    *len = 2;
                                } break;
                                case VIDEO_REQUEST_GET_RES: {
                                    ts_u16 wWhiteBalance_Temprature = 1;
                                    ts_cpl_memcpy(*data, (ts_u08 *)&wWhiteBalance_Temprature, 2);
                                    *len = 2;
                                } break;
                                case VIDEO_REQUEST_GET_INFO:
                                    (*data)[0] = 0x03; //struct video_camera_capabilities
                                    *len = 1;
                                    break;
                                case VIDEO_REQUEST_GET_DEF: {
                                    ts_u16 wWhiteBalance_Temprature = 417;
                                    ts_cpl_memcpy(*data, (ts_u08 *)&wWhiteBalance_Temprature, 2);
                                    *len = 2;
                                } break;
                                default:
                                    USB_LOG_WRN("Unhandled Video Class bRequest 0x%02x\r\n", setup->bRequest);
                                    return -1;
                            }
                            break;
                        case VIDEO_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL:
                            switch (setup->bRequest) {
                                case VIDEO_REQUEST_GET_CUR: {
                                    ts_u16 wWhiteBalance_Temprature_Auto = 1;
                                    ts_cpl_memcpy(*data, (ts_u08 *)&wWhiteBalance_Temprature_Auto, 1);
                                    *len = 1;
                                } break;
                                default:
                                    USB_LOG_WRN("Unhandled Video Class bRequest 0x%02x\r\n", setup->bRequest);
                                    return -1;
                            }
                            break;
                        default:
                            g_usbd_video.error_code = 0x06;
                            USB_LOG_WRN("Unhandled Video Class control selector 0x%02x\r\n", control_selector);
                            return -1;
                    }
                    break;
                case VIDEO_VC_EXTENSION_UNIT_DESCRIPTOR_SUBTYPE:
                    break;
                case VIDEO_VC_ENCODING_UNIT_DESCRIPTOR_SUBTYPE:
                    break;

                default:
                    break;
            }
        }
    }
    return 0;
}

static int usbd_video_stream_request_handler(struct usb_setup_packet *setup, ts_u08 **data, ts_u32 *len)
{
    ts_u08 control_selector = (ts_u08)(setup->wValue >> 8);

    switch (control_selector) {
        case VIDEO_VS_PROBE_CONTROL:
            switch (setup->bRequest) {
                case VIDEO_REQUEST_SET_CUR:
                    //ts_cpl_memcpy((ts_u08 *)&g_usbd_video.probe, *data, setup->wLength);
                    break;
                case VIDEO_REQUEST_GET_CUR:
                    ts_cpl_memcpy(*data, (ts_u08 *)&g_usbd_video.probe, setup->wLength);
                    *len = sizeof(struct video_probe_and_commit_controls);
                    break;

                case VIDEO_REQUEST_GET_MIN:
                case VIDEO_REQUEST_GET_MAX:
                case VIDEO_REQUEST_GET_RES:
                case VIDEO_REQUEST_GET_DEF:
                    ts_cpl_memcpy(*data, (ts_u08 *)&g_usbd_video.probe, setup->wLength);
                    *len = sizeof(struct video_probe_and_commit_controls);
                    break;
                case VIDEO_REQUEST_GET_LEN:
                    (*data)[0] = sizeof(struct video_probe_and_commit_controls);
                    *len = 1;
                    break;

                case VIDEO_REQUEST_GET_INFO:
                    (*data)[0] = 0x03;
                    *len = 1;
                    break;

                default:
                    USB_LOG_WRN("Unhandled Video Class bRequest 0x%02x\r\n", setup->bRequest);
                    return -1;
            }
            break;
        case VIDEO_VS_COMMIT_CONTROL:
            switch (setup->bRequest) {
                case VIDEO_REQUEST_SET_CUR:
                    //ts_cpl_memcpy((ts_u08 *)&g_usbd_video.commit, *data, setup->wLength);
                    break;
                case VIDEO_REQUEST_GET_CUR:
                    ts_cpl_memcpy(*data, (ts_u08 *)&g_usbd_video.commit, setup->wLength);
                    *len = sizeof(struct video_probe_and_commit_controls);
                    break;
                case VIDEO_REQUEST_GET_MIN:
                case VIDEO_REQUEST_GET_MAX:
                case VIDEO_REQUEST_GET_RES:
                case VIDEO_REQUEST_GET_DEF:
                    ts_cpl_memcpy(*data, (ts_u08 *)&g_usbd_video.commit, setup->wLength);
                    *len = sizeof(struct video_probe_and_commit_controls);
                    break;

                case VIDEO_REQUEST_GET_LEN:
                    (*data)[0] = sizeof(struct video_probe_and_commit_controls);
                    *len = 1;
                    break;

                case VIDEO_REQUEST_GET_INFO:
                    (*data)[0] = 0x03;
                    *len = 1;
                    break;

                default:
                    USB_LOG_WRN("Unhandled Video Class bRequest 0x%02x\r\n", setup->bRequest);
                    return -1;
            }
            break;
        case VIDEO_VS_STREAM_ERROR_CODE_CONTROL:
            switch (setup->bRequest) {
                case VIDEO_REQUEST_GET_CUR:
                    (*data)[0] = g_usbd_video.error_code;
                    *len = 1;
                    break;
                case VIDEO_REQUEST_GET_INFO:
                    (*data)[0] = 0x01;
                    *len = 1;
                    break;
                default:
                    USB_LOG_WRN("Unhandled Video Class bRequest 0x%02x\r\n", setup->bRequest);
                    return -1;
            }
            break;
        default:
            break;
    }

    return 0;
}

static int video_class_interface_request_handler(struct usb_setup_packet *setup, ts_u08 **data, ts_u32 *len)
{
    USB_LOG_DBG("Video Class request: ""bRequest 0x%02x\r\n",setup->bRequest);

    ts_u08 intf_num = (ts_u08)setup->wIndex;
    ts_u08 entity_id = (ts_u08)(setup->wIndex >> 8);

    if (intf_num == 0) { /* Video Control Interface */
        if (entity_id == 0) {
            return usbd_video_control_request_handler(setup, data, len); /* Interface Control Requests */
        } else {
            return usbd_video_control_unit_terminal_request_handler(setup, data, len); /* Unit and Terminal Requests */
        }
    } else if (intf_num == 1) {                                            /* Video Stream Inteface */
        return usbd_video_stream_request_handler(setup, data, len); /* Interface Stream Requests */
    }
    return -1;
}

static void video_notify_handler(ts_u08 event, void *arg)
{
    switch (event) {
        case USBD_EVENT_RESET:
            g_usbd_video.error_code = 0;
            g_usbd_video.power_mode = 0;
            break;

        case USBD_EVENT_SET_INTERFACE: {
            struct usb_interface_descriptor *intf = (struct usb_interface_descriptor *)arg;
            if (intf->bAlternateSetting == 1) {
                usbd_video_open(intf->bInterfaceNumber);
            } else {
                usbd_video_close(intf->bInterfaceNumber);
            }
        }

        break;
        default:
            break;
    }
}

static void usbd_video_probe_and_commit_controls_init(ts_u32 dwFrameInterval, ts_u32 dwMaxVideoFrameSize, ts_u32 dwMaxPayloadTransferSize)
{
    g_usbd_video.probe.hintUnion.bmHint = 0x01;
    g_usbd_video.probe.hintUnion1.bmHint = 0;
    g_usbd_video.probe.bFormatIndex = 1;
    g_usbd_video.probe.bFrameIndex = 1;
    g_usbd_video.probe.dwFrameInterval = dwFrameInterval;
    g_usbd_video.probe.wKeyFrameRate = 0;
    g_usbd_video.probe.wPFrameRate = 0;
    g_usbd_video.probe.wCompQuality = 0;
    g_usbd_video.probe.wCompWindowSize = 0;
    g_usbd_video.probe.wDelay = 0;
    g_usbd_video.probe.dwMaxVideoFrameSize = dwMaxVideoFrameSize;
    g_usbd_video.probe.dwMaxPayloadTransferSize = dwMaxPayloadTransferSize;
    g_usbd_video.probe.dwClockFrequency = 0;
    g_usbd_video.probe.bmFramingInfo = 0;
    g_usbd_video.probe.bPreferedVersion = 0;
    g_usbd_video.probe.bMinVersion = 0;
    g_usbd_video.probe.bMaxVersion = 0;

    g_usbd_video.commit.hintUnion.bmHint = 0x01;
    g_usbd_video.commit.hintUnion1.bmHint = 0;
    g_usbd_video.commit.bFormatIndex = 1;
    g_usbd_video.commit.bFrameIndex = 1;
    g_usbd_video.commit.dwFrameInterval = dwFrameInterval;
    g_usbd_video.commit.wKeyFrameRate = 0;
    g_usbd_video.commit.wPFrameRate = 0;
    g_usbd_video.commit.wCompQuality = 0;
    g_usbd_video.commit.wCompWindowSize = 0;
    g_usbd_video.commit.wDelay = 0;
    g_usbd_video.commit.dwMaxVideoFrameSize = dwMaxVideoFrameSize;
    g_usbd_video.commit.dwMaxPayloadTransferSize = dwMaxPayloadTransferSize;
    g_usbd_video.commit.dwClockFrequency = 0;
    g_usbd_video.commit.bmFramingInfo = 0;
    g_usbd_video.commit.bPreferedVersion = 0;
    g_usbd_video.commit.bMinVersion = 0;
    g_usbd_video.commit.bMaxVersion = 0;

    g_usbd_video.stream_frameid = 0;
    g_usbd_video.stream_headerlen = 2;
}

static int copy_en = 0;

static ts_u32 usbd_video_prepare_ep_buf_data(ts_u32 remain, ts_u08 *ep_buf)
{
    struct video_payload_header *header;
    ts_u32 len;
    ts_u32 offset;

    len = MIN(remain, (g_usbd_video.probe.dwMaxPayloadTransferSize - g_usbd_video.stream_headerlen) * g_usbd_video.max_packets);
    offset = 0;
    while (len > 0) {
        header = (struct video_payload_header *)&ep_buf[offset];
        header->bHeaderLength = g_usbd_video.stream_headerlen;
        header->headerInfoUnion.bmheaderInfo = 0;
        header->headerInfoUnion.headerInfoBits.endOfHeader = 1;
        header->headerInfoUnion.headerInfoBits.endOfFrame = 0;
        header->headerInfoUnion.headerInfoBits.frameIdentifier = g_usbd_video.stream_frameid;

        ts_u32 len2 = MIN(len, g_usbd_video.probe.dwMaxPayloadTransferSize - g_usbd_video.stream_headerlen);

        if (0 == copy_en) {
            usb_memcpy(&ep_buf[offset + g_usbd_video.stream_headerlen],
                &g_usbd_video.stream_buf[g_usbd_video.stream_offset],
                len2);
        }

        g_usbd_video.stream_offset += len2;
        len -= len2;
        offset += (len2 + g_usbd_video.stream_headerlen);

        if (g_usbd_video.stream_offset == g_usbd_video.stream_len) {
            header->headerInfoUnion.headerInfoBits.endOfFrame = 1;
        }
    }

    return offset;
}

struct usbd_interface *usbd_video_init_intf(struct usbd_interface *intf,
                                            ts_u32 dwFrameInterval,
                                            ts_u32 dwMaxVideoFrameSize,
                                            ts_u32 dwMaxPayloadTransferSize)
{
    intf->class_interface_handler = video_class_interface_request_handler;
    intf->class_endpoint_handler = NULL;
    intf->vendor_handler = NULL;
    intf->notify_handler = video_notify_handler;

    g_usbd_video.info[0].bDescriptorSubtype = VIDEO_VC_INPUT_TERMINAL_DESCRIPTOR_SUBTYPE;
    g_usbd_video.info[0].bEntityId = 0x01;
    g_usbd_video.info[0].wTerminalType = VIDEO_ITT_CAMERA;
    g_usbd_video.info[1].bDescriptorSubtype = VIDEO_VC_OUTPUT_TERMINAL_DESCRIPTOR_SUBTYPE;
    g_usbd_video.info[1].bEntityId = 0x03;
    g_usbd_video.info[1].wTerminalType = 0x00;
    g_usbd_video.info[2].bDescriptorSubtype = VIDEO_VC_PROCESSING_UNIT_DESCRIPTOR_SUBTYPE;
    g_usbd_video.info[2].bEntityId = 0x02;
    g_usbd_video.info[2].wTerminalType = 0x00;

    usbd_video_probe_and_commit_controls_init(dwFrameInterval, dwMaxVideoFrameSize, dwMaxPayloadTransferSize);
    return intf;
}

extern ts_bool iso_tx_busy;
static ts_cpl_semphr_s_t stream_sema;
extern ts_void *gpv_tmm_usr;
static int s32_inited = 0;


ts_void usbd_video_iso_callback(usbd_edpt_s_t *edpt, usb_req_s_t *req)
{
    ts_cpl_up((ts_cpl_semphr_s_t *)&stream_sema);
}

int usbd_video_stream_start_write(ts_u08 ep, ts_u08 *ep_buf0, ts_u08 *ep_buf1, ts_u32 ep_bufsize, ts_u08 *stream_buf, ts_u32 stream_len)
{
    ts_u32 remain, precnt, wrcnt;
    ts_u08 *wr_buf;

    if (0 == s32_inited) {
        ts_cpl_semphr_init((ts_cpl_semphr_s_t *)&stream_sema, 0);
        s32_inited = 1;
    }

    g_usbd_video.max_packets = ep_bufsize / g_usbd_video.probe.dwMaxPayloadTransferSize;
    g_usbd_video.stream_buf = stream_buf;
    g_usbd_video.stream_len = stream_len;
    g_usbd_video.stream_offset = 0;
    wr_buf = ep_buf0;
    wrcnt = 0;
    while (1) {
        remain = g_usbd_video.stream_len - g_usbd_video.stream_offset;
        if (0 == remain) {
            g_usbd_video.stream_frameid ^= 1;
            g_usbd_video.stream_finish = ts_true;

            break;
        }

        precnt = usbd_video_prepare_ep_buf_data(remain, wr_buf);

        wr_buf += precnt;
        wrcnt += precnt;

        //vision_log_debug("wr_buf=%p, %d\n", wr_buf, wrcnt);
    }

    usbd_ep_start_write(ep, ep_buf0, wrcnt, ts_null);
   // ts_cpl_down((ts_cpl_semphr_s_t *)&stream_sema);

    if (0 == copy_en) {
        vision_log_debug("wrcnt = %d\n", wrcnt);
        copy_en = 1;
    }


    return wrcnt;
}

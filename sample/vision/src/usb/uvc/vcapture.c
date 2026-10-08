/*
 * Copyright (c) 2024, sakumisu
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <base/ts_type.h>
#include <base/ts_compiler.h>
#include <cpl/ts_cpl.h>
#include <uapi/usb/usbd_core.h>
#include <uapi/usb/usbd_video.h>

#ifdef CONFIG_UVC_WITH_ACM
#include <uapi/sfc_v100.h>
#include <uapi/usb/usbd_cdc_acm.h>
#endif

#include <uapi/upl_v100.h>
#include <command.h>
#include <stdio.h>
#include "api.h"
#include "f_tdk.h"
#include <menu/votmain.h>
#include <snr_comm_vot.h>
#include <fontlib_vot.h>
#include <planck640c_vot.h>
#include <planck1280c_vot.h>

#include <menu/ispfunction.h>
#include <menu/ispfunction.h>
extern ts_void *gpv_tmm_usr;

typedef struct vot_uvc_upload_attr_s
{
    /* raw 从指定位置模块导出 */
    ts_u32 u32_module;

    /* 指定通道, 可配置成 TS_ISP_UPL_PRIMARY_CH 和 TS_ISP_UPL_SECONDARY_CH */
    ts_u32 u32_chn;

    /* 导出帧数, 内存分配时 w * h * (2 or 1) * (s32_frame_num + 1) */
    ts_s32 s32_frame_num;

    /* 超时时间 */
    ts_u32 u32_tmout_ms;

    ts_u08 u08_reset_flag;
    ts_mdev_t h_dev;

} vot_uvc_upload_attr_s_t;

extern struct Menu2_Para SystemParaMenu2;

vot_uvc_upload_attr_s_t vot_uvc_st_attr;

// #define VIDEO_ISO_EN

/* UVC EP */
#define VIDEO_IN_EP 0x81

/* 实际不存在 */
#define VIDEO_INT_EP 0x85

#ifdef CONFIG_UVC_WITH_ACM
/* CDC-ACM EP */
#define CDC_IN_EP 0x83
#define CDC_OUT_EP 0x02
#define CDC_INT_EP 0x87

#define CDC_MAX_MPS 512
#define CDC_MAX_BUF CDC_MAX_MPS

#define CDC_MAX_RBUF_SIZE (4096)
#define CDC_MAX_USR_SIZE (CDC_MAX_RBUF_SIZE - CDC_MAX_BUF)

#define VC_ITT_NUM 4

#else
#define VC_ITT_NUM 2
#endif

#ifdef VIDEO_ISO_EN
#define VIDEO_TRAN_NUM (2)
#if (2 == VIDEO_TRAN_NUM)
#define MAX_PAYLOAD_SIZE (2048) // for high speed with two transcations every one micro frame
#define VIDEO_PACKET_SIZE (ts_u32)(((MAX_PAYLOAD_SIZE / 2)) | (0x01 << 11))
#elif (3 == VIDEO_TRAN_NUM)
#define MAX_PAYLOAD_SIZE (3072) // for high speed with three transcations every one micro frame
#define VIDEO_PACKET_SIZE (ts_u32)(((MAX_PAYLOAD_SIZE / 3)) | (0x02 << 11))
#else
#define MAX_PAYLOAD_SIZE (1024) // for high speed with one transcations every one micro frame
#define VIDEO_PACKET_SIZE (ts_u32)(((MAX_PAYLOAD_SIZE / 1)) | (0x00 << 11))
#endif
#else
#define MAX_PAYLOAD_SIZE 64 * 1024
#define VIDEO_PACKET_SIZE 512
#endif

#define VS_HEADER_SIZ (ts_u32)(VIDEO_SIZEOF_VS_INPUT_HEADER_DESC(1, 1) + VIDEO_SIZEOF_VS_FORMAT_UNCOMPRESSED_DESC + VIDEO_SIZEOF_VS_FRAME_UNCOMPRESSED_DESC(1))

#define VIDEO_VC_NOEP

#ifdef VIDEO_VC_NOEP
#define VIDEO_VC_DES_LEN VIDEO_VC_NOEP_DESCRIPTOR_LEN
#else
#define VIDEO_VC_DES_LEN VIDEO_VC_DESCRIPTOR_LEN
#endif

#ifdef VIDEO_ISO_EN
#define VIDEO_EP_DESC_LEN (9 + 7)
#else
#define VIDEO_EP_DESC_LEN (7)
#endif

#ifndef CONFIG_UVC_WITH_ACM
#define USB_VIDEO_DESC_SIZ (ts_ulong)(9 + VIDEO_VC_DES_LEN + 9 + VS_HEADER_SIZ + 6 + VIDEO_EP_DESC_LEN)
#else
#define USB_VIDEO_DESC_SIZ (ts_ulong)(9 + VIDEO_VC_DES_LEN + 9 + VS_HEADER_SIZ + 6 + VIDEO_EP_DESC_LEN + CDC_ACM_DESCRIPTOR_LEN)
#endif

#define USBD_VID 0x5474
#define USBD_PID 0xffff
#define USBD_MAX_POWER 100
#define USBD_LANGID_STRING 1033

// #define UVC_Y8_ENABLE

static const ts_u08 device_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xef, 0x02, 0x01, USBD_VID, USBD_PID, 0x0001, 0x01)};

static const ts_u08 config_descriptor[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_VIDEO_DESC_SIZ, VC_ITT_NUM, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),

#ifdef VIDEO_VC_NOEP
    VIDEO_VC_NOEP_DESCRIPTOR_INIT(0x00, VIDEO_INT_EP, 0x0150, VIDEO_VC_TERMINAL_LEN, 48000000, 0x02),
#else
    VIDEO_VC_DESCRIPTOR_INIT(0x00, VIDEO_INT_EP, 0x0100, VIDEO_VC_TERMINAL_LEN, 48000000, 0x02),
#endif

#ifdef VIDEO_ISO_EN
    VIDEO_VS_DESCRIPTOR_INIT(0x01, 0x00, 0x00),
#else
    VIDEO_VS_DESCRIPTOR_INIT(0x01, 0x00, 0x01),
#endif

    VIDEO_VS_INPUT_HEADER_DESCRIPTOR_INIT(0x01, VS_HEADER_SIZ, VIDEO_IN_EP, 0x00),

#ifdef UVC_Y8_ENABLE
    VIDEO_VS_FORMAT_UNCOMPRESSED_DESCRIPTOR_INIT(0x01, 0x01, VIDEO_GUID_Y800, 0x08),
#else
    VIDEO_VS_FORMAT_UNCOMPRESSED_DESCRIPTOR_INIT(0x01, 0x01, VIDEO_GUID_YUY2, 0x10),
#endif

    VIDEO_VS_FRAME_UNCOMPRESSED_DESCRIPTOR_INIT(0x01, 0, 0, 0, 0, 0, DBVAL(0), 1, DBVAL(0)),
    VIDEO_VS_COLOR_MATCHING_DESCRIPTOR_INIT(),

#ifdef VIDEO_ISO_EN
    VIDEO_VS_DESCRIPTOR_INIT(0x01, 0x01, 0x01),
    /* 1.2.2.2 Standard VideoStream Isochronous Video Data Endpoint Descriptor */
    USB_ENDPOINT_DESCRIPTOR_INIT(VIDEO_IN_EP, 5, VIDEO_PACKET_SIZE, 0x01),
#else
    USB_ENDPOINT_DESCRIPTOR_INIT(VIDEO_IN_EP, USB_ENDPOINT_TYPE_BULK, 512, 0x01),
#endif

/* CDC-ACM */
#ifdef CONFIG_UVC_WITH_ACM
    CDC_ACM_DESCRIPTOR_INIT(0x02, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, CDC_MAX_MPS, 0x02)
#endif
};

static const ts_u08 config_descriptor_uyvy[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_VIDEO_DESC_SIZ, VC_ITT_NUM, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),

#ifdef VIDEO_VC_NOEP
    VIDEO_VC_NOEP_DESCRIPTOR_INIT(0x00, VIDEO_INT_EP, 0x0150, VIDEO_VC_TERMINAL_LEN, 48000000, 0x02),
#else
    VIDEO_VC_DESCRIPTOR_INIT(0x00, VIDEO_INT_EP, 0x0100, VIDEO_VC_TERMINAL_LEN, 48000000, 0x02),
#endif

#ifdef VIDEO_ISO_EN
    VIDEO_VS_DESCRIPTOR_INIT(0x01, 0x00, 0x00),
#else
    VIDEO_VS_DESCRIPTOR_INIT(0x01, 0x00, 0x01),
#endif

    VIDEO_VS_INPUT_HEADER_DESCRIPTOR_INIT(0x01, VS_HEADER_SIZ, VIDEO_IN_EP, 0x00),

#ifdef UVC_Y8_ENABLE
    VIDEO_VS_FORMAT_UNCOMPRESSED_DESCRIPTOR_INIT(0x01, 0x01, VIDEO_GUID_Y800, 0x08),
#else
    VIDEO_VS_FORMAT_UNCOMPRESSED_DESCRIPTOR_INIT(0x01, 0x01, VIDEO_GUID_UYVY, 0x10),
#endif

    VIDEO_VS_FRAME_UNCOMPRESSED_DESCRIPTOR_INIT(0x01, 0, 0, 0, 0, 0, DBVAL(0), 1, DBVAL(0)),
    VIDEO_VS_COLOR_MATCHING_DESCRIPTOR_INIT(),

#ifdef VIDEO_ISO_EN
    VIDEO_VS_DESCRIPTOR_INIT(0x01, 0x01, 0x01),
    /* 1.2.2.2 Standard VideoStream Isochronous Video Data Endpoint Descriptor */
    USB_ENDPOINT_DESCRIPTOR_INIT(VIDEO_IN_EP, 5, VIDEO_PACKET_SIZE, 0x01),
#else
    USB_ENDPOINT_DESCRIPTOR_INIT(VIDEO_IN_EP, USB_ENDPOINT_TYPE_BULK, 512, 0x01),
#endif

/* CDC-ACM */
#ifdef CONFIG_UVC_WITH_ACM
    CDC_ACM_DESCRIPTOR_INIT(0x02, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, CDC_MAX_MPS, 0x02)
#endif
};

static const ts_u08 device_quality_descriptor[] = {
    ///////////////////////////////////////
    /// device qualifier descriptor
    ///////////////////////////////////////
    0x0a,
    USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
    0x00,
    0x02,
    0x00,
    0x00,
    0x00,
    0x40,
    0x00,
    0x00,
};

#define UVC_HD_SIZE (2)
#define UVC_PAYLOAD_SIZE (MAX_PAYLOAD_SIZE - UVC_HD_SIZE)
#ifdef CONFIG_SNR_PLANCK1280C
#define UVC_REQ_MAX_NUM (3)
#else
#define UVC_REQ_MAX_NUM (4)
#endif
typedef struct uvc_ctx_s
{
    /* 记录初始化状态 */
    ts_u32 u32_inited;

    /* usb 已经被配置，可以使用 */
    ts_u32 u32_cfged;

    /* uvc 被 open */
    ts_u32 u32_open;

    /* uvc 准备发送信号量 */
    ts_cpl_semphr_s_t st_ready_sema;

    ts_u32 u32_mem_addr;

    tdk_tuv_status_en_t en_status;
    usb_req_s_t ast_video_req[UVC_REQ_MAX_NUM];

    /* 视频完成队列 */
    ts_cpl_list_head_s_t st_rdy_head;

    /* 同步信号以及结构体保护锁 */
    ts_cpl_semphr_s_t st_uvc_sema;
    ts_cpl_spinlock_s_t st_lock;

    /* 帧标志 */
    ts_u32 u32_frm_id;

    /* 保存当前的模块 */
    ts_u32 u32_module;

    ts_u32 u32_interval;
    ts_u32 u32_max_frm_size;

#ifdef CONFIG_UVC_WITH_ACM
    /* 接收信号量 */
    ts_cpl_semphr_s_t st_acm_rd_sema;

    /* 接收 request */
    usb_req_s_t st_acm_rd_req;

    /* 发送信号量，使用信号量会阻塞程序，如果不想阻塞，可以不使用信号量和回调，但是需要注意缓存不能被覆盖 */
    ts_cpl_semphr_s_t st_acm_wr_sema;

    /* 单帧最大 CDC_MAX_MPS 接收缓存 */
    ts_u08 *pu08_pkg_buf;

    /* 对接用户缓存和读写索引 */
    ts_cpl_semphr_s_t st_acm_usr_sema;
    ts_u08 *pu08_usr_buf;
    ts_u32 u32_pkg_w_idx;
    ts_u32 u32_usr_r_idx;
    ts_u32 u32_usr_size;
#endif
    ts_u08 m_reset_uvc_flag;           // 是否重置这个uvc链路
    ts_u08 m_ringbuf_process_flag;     // ringbuf消息处理线程状态
    ts_u08 m_usb_descriptor_init_flag; // usb_descriptor 初始化标志

    ts_u08 m_routine_flag;          // 运行标志位
} uvc_ctx_s_t;

static uvc_ctx_s_t sst_ctx = {0};

static ts_void upl_req_ready_cb(ts_void *pv_node);

static const char *string_descriptors[] = {
    (const char[]){0x09, 0x04}, /* Langid */
    "VOT",                      /* Manufacturer */
    "VOT UVC",                  /* Product */
    "202505161451",             /* Serial Number */
    "UVC Control",              // 4: UVC Interface
    "UVC Streaming",            // 5: UVC Interface
    "none",                     // 6: UVC Interface
};

extern void ring_buffer_init(ring_buffer_t *rb, ts_char *buf_name);
static ring_buffer_t m_acm_ring_buf_instance;

static const ts_u08 *device_descriptor_callback(ts_u08 speed)
{
    return device_descriptor;
}

static const ts_u08 *config_descriptor_callback(ts_u08 speed)
{
    if (SystemParaMenu2.m_uvc_output_format == UVC_YUV_FORMAT_UYVY) {
        return config_descriptor_uyvy;
    }
    return config_descriptor;
}

static const ts_u08 *device_quality_descriptor_callback(ts_u08 speed)
{
    return device_quality_descriptor;
}

static const ts_char *string_descriptor_callback(ts_u08 speed, ts_u08 index)
{
    if (index >= 6)
    {
        return "none";
    }

    if (get_customer_type() == CUSTOMER_TYPE_GENERAL){

        if (index==1){
            return "Infrared";
        }
        else if (index==2){
            return "Infrared Camera";
        }
    }
 

    return string_descriptors[index];
}

const struct usb_descriptor video_descriptor = {
    .device_descriptor_callback = device_descriptor_callback,
    .config_descriptor_callback = config_descriptor_callback,
    .device_quality_descriptor_callback = device_quality_descriptor_callback,
    .string_descriptor_callback = string_descriptor_callback};

static ts_void usbd_event_handler(ts_u08 event)
{
    vision_log_debug("ent=%d\n", event);

    switch (event)
    {
    case USBD_EVENT_RESET:
        sst_ctx.u32_cfged = 0;
        sst_ctx.u32_open = 0;
        break;
    case USBD_EVENT_CONNECTED:
        break;
    case USBD_EVENT_DISCONNECTED:
        break;
    case USBD_EVENT_RESUME:
        sst_ctx.u32_cfged = 1;
        break;
    case USBD_EVENT_SUSPEND:
        sst_ctx.u32_cfged = 0;
        usbd_video_close(0);
        break;
    case USBD_EVENT_CONFIGURED:
#ifdef VIDEO_ISO_EN
        sst_ctx.u32_open = 0;
#endif
        sst_ctx.u32_cfged = 1;
        break;
    case USBD_EVENT_SET_REMOTE_WAKEUP:
        break;
    case USBD_EVENT_CLR_REMOTE_WAKEUP:
        break;

    default:
        break;
    }
}

static ts_u32 uvc_get_guid(ts_u08 *pu08_desc)
{
    ts_u08 au08_guid[4];
    ts_u32 *pu32_ptr;
    ts_u08 *pu08_ptr;
    ts_u32 i;

    pu08_ptr = pu08_desc;
    pu08_ptr += 5;
    for (i = 0; i < 4; i++)
        au08_guid[i] = pu08_ptr[i];

    pu32_ptr = (ts_u32 *)&au08_guid;

    return *pu32_ptr;
}

/* 从 vpo 获取设备信息，动态修改 uvc 描述符 */
static ts_void uvc_update_desc(ts_u08 *pu08_cfg_desc, ts_u32 u32_cfg_size)
{
    ts_u32 u32_width, u32_height, u32_fps, u32_bitw;
    ts_u32 u32_interval, u32_bit_rate, u32_max_frm_size, u32_guid;
    ts_u32 i, u32_sz, u32_len;
    ts_u08 *pu08_cfg;
    ts_u08 *pu08_ptr;

    sst_ctx.u32_max_frm_size = 0;
    tdl_mupl_get_frm_info((ts_u32 *)&u32_width, (ts_u32 *)&u32_height, (ts_u32 *)&u32_fps);

    u32_interval = 10000000 / ISP_get_output_fps();
    pu08_cfg = pu08_cfg_desc;
    u32_sz = u32_cfg_size;
    u32_bitw = 1;
    i = 0;
    while (1)
    {
        if (i >= u32_sz)
            break;

        pu08_ptr = pu08_cfg;

        /* 匹配目标， 长度 >= 0x1a, 描述符类型 0x24, 子类型 0x04 or 0x05 */
        u32_len = pu08_ptr[0];
        if ((u32_len >= 0x1a) && (0x24 == pu08_ptr[1]))
        {
            if (0x04 == pu08_ptr[2])
            {
                u32_guid = uvc_get_guid(pu08_ptr);
                if (0x30303859 == u32_guid)
                    sst_ctx.u32_module = TS_UPL_POS_VPO_Y8;
                else
                    sst_ctx.u32_module = TS_UPL_POS_VPO_YUV;

                /* 偏移 21 位是位宽 */
                if (0x10 == pu08_ptr[21])
                    u32_bitw = 2;
                else
                    u32_bitw = 1;
            }
            else if (0x05 == pu08_ptr[2])
            {
                u32_max_frm_size = u32_width * u32_height * u32_bitw;
                u32_bit_rate = u32_max_frm_size * u32_fps * 8;

                if (u32_max_frm_size > sst_ctx.u32_max_frm_size)
                    sst_ctx.u32_max_frm_size = u32_max_frm_size;

                /* 5,6 位 width, 低位在前 */
                pu08_ptr[5] = (ts_u08)(u32_width >> 0);
                pu08_ptr[6] = (ts_u08)(u32_width >> 8);

                /* 7,8 位 height, 低位在前 */
                pu08_ptr[7] = (ts_u08)(u32_height >> 0);
                pu08_ptr[8] = (ts_u08)(u32_height >> 8);

                /* 9..12  dwMinBitRate, 低位在前 */
                pu08_ptr[9] = (ts_u08)(u32_bit_rate >> 0);
                pu08_ptr[10] = (ts_u08)(u32_bit_rate >> 8);
                pu08_ptr[11] = (ts_u08)(u32_bit_rate >> 16);
                pu08_ptr[12] = (ts_u08)(u32_bit_rate >> 24);

                /* 13..16  dwMaxBitRate, 低位在前 */
                pu08_ptr[13] = (ts_u08)(u32_bit_rate >> 0);
                pu08_ptr[14] = (ts_u08)(u32_bit_rate >> 8);
                pu08_ptr[15] = (ts_u08)(u32_bit_rate >> 16);
                pu08_ptr[16] = (ts_u08)(u32_bit_rate >> 24);

                /* 17..20 dwMaxVideoFrameBufferSize, 低位在前 */
                pu08_ptr[17] = (ts_u08)(u32_max_frm_size >> 0);
                pu08_ptr[18] = (ts_u08)(u32_max_frm_size >> 8);
                pu08_ptr[19] = (ts_u08)(u32_max_frm_size >> 16);
                pu08_ptr[20] = (ts_u08)(u32_max_frm_size >> 24);

                /* 21..24 dwDefaultFrameInterval, 低位在前 */
                pu08_ptr[21] = (ts_u08)(u32_interval >> 0);
                pu08_ptr[22] = (ts_u08)(u32_interval >> 8);
                pu08_ptr[23] = (ts_u08)(u32_interval >> 16);
                pu08_ptr[24] = (ts_u08)(u32_interval >> 24);

                /* 26..29 dwDefaultFrameInterval, 低位在前 */
                pu08_ptr[26] = (ts_u08)(u32_interval >> 0);
                pu08_ptr[27] = (ts_u08)(u32_interval >> 8);
                pu08_ptr[28] = (ts_u08)(u32_interval >> 16);
                pu08_ptr[29] = (ts_u08)(u32_interval >> 24);
            }
        }

#if 0
        for (j = 0; j < u32_len; j++)
            vision_log_debug("%02x ", pu08_ptr[j]);
    
        vision_log_debug("\n");
#endif

        i += u32_len;
        pu08_cfg += u32_len;
    }

    sst_ctx.u32_interval = u32_interval;
}

#if 0
static ts_void uvc_show_desc(ts_void)
{
    ts_u32 i, j, u32_sz, u32_len;
    ts_u08 *pu08_cfg;
    ts_u08 *pu08_ptr;

    pu08_cfg = (ts_u08 *)&config_descriptor;
    u32_sz = sizeof(config_descriptor);

    i = 0;
    while (1) {
        if (i >= u32_sz)
            break;
        
        pu08_ptr = pu08_cfg;

        /* 第一个获取长度 */
        u32_len = pu08_ptr[0];

        for (j = 0; j < u32_len; j++)
            vision_log_debug("%02x ", pu08_ptr[j]);
    
        vision_log_debug("\n");
        i += u32_len;
        pu08_cfg += u32_len;
    }
}
#endif

static ts_void uvc_prepare_vbuf(usb_req_s_t *req)
{
    ts_u08 *pu08_ptr;
    struct video_payload_header *header;
    ts_u32 u32_frm_size, u32_fix_size;
    ts_u08 u08_frm_id;

    pu08_ptr = (ts_u08 *)req->dma_addr;
    if (0 == sst_ctx.u32_frm_id)
        u08_frm_id = 0;
    else
        u08_frm_id = 1;

    u32_frm_size = sst_ctx.u32_max_frm_size;
    u32_fix_size = u32_frm_size;
    while (1)
    {
        header = (struct video_payload_header *)pu08_ptr;

        header->bHeaderLength = UVC_HD_SIZE;
        header->headerInfoUnion.bmheaderInfo = 0;
        header->headerInfoUnion.headerInfoBits.endOfHeader = 1;
        header->headerInfoUnion.headerInfoBits.endOfFrame = 0;
        header->headerInfoUnion.headerInfoBits.frameIdentifier = u08_frm_id;

        pu08_ptr += MAX_PAYLOAD_SIZE;
        if (u32_frm_size > UVC_PAYLOAD_SIZE)
            u32_frm_size -= UVC_PAYLOAD_SIZE;
        else
            u32_frm_size = 0;

        u32_fix_size += UVC_HD_SIZE;
        if (0 == u32_frm_size)
        {
            header->headerInfoUnion.headerInfoBits.endOfFrame = 1;

            break;
        }
    }

    req->length = u32_fix_size;

    if (0 == sst_ctx.u32_frm_id)
        sst_ctx.u32_frm_id = 1;
    else
        sst_ctx.u32_frm_id = 0;
}

static ts_void upl_req_unbind(ts_void)
{
    ts_u32 i;
    ts_cpl_list_head_s_t *pst_list;

    /* 全部从挂载点上清除 */
    for (i = 0; i < UVC_REQ_MAX_NUM; i++)
    {
        pst_list = (ts_cpl_list_head_s_t *)&sst_ctx.ast_video_req[i].list;
        if (0 == ts_cpl_list_empty(pst_list))
            ts_cpl_list_del_init(pst_list);
    }
}

static ts_void upl_req_bind(ts_void)
{
    ts_u32 i;
    ts_s32 s32_ret;
    ts_cpl_list_head_s_t *pst_list;

    /* 全部从挂载点上清除 */
    for (i = 0; i < UVC_REQ_MAX_NUM; i++)
    {
        pst_list = (ts_cpl_list_head_s_t *)&sst_ctx.ast_video_req[i].list;
        if (0 == ts_cpl_list_empty(pst_list))
            ts_cpl_list_del_init(pst_list);

        tdl_isp_upload_put_free_node(0, (ts_void *)pst_list, TS_ISP_UPL_PRIMARY_CH);
    }

    ts_cpl_init_list_head((ts_cpl_list_head_s_t *)&sst_ctx.st_rdy_head);

    if (sst_ctx.st_uvc_sema.pv_semphr != ts_null)
    {
        /* 确保所有 up 完成 down */
        for (i = 0; i < UVC_REQ_MAX_NUM; i++)
        {
            s32_ret = ts_cpl_down_timeout((ts_cpl_semphr_s_t *)&sst_ctx.st_uvc_sema, 100);
            if (s32_ret != 0)
                break;
        }
    }

    tdl_isp_upload_set_cb(0, TS_ISP_UPL_PRIMARY_CH, upl_req_ready_cb);
}

//static 
ts_s32 uvc_stop_video(ts_void)
{
    if (TDK_TUV_STATUS_RUN == sst_ctx.en_status)
    {
        tdl_isp_stop_upload(0, TS_ISP_UPL_PRIMARY_CH);

        /* reclainm all req */
        upl_req_unbind();

        sst_ctx.en_status = TDK_TUV_STATUS_IDLE;

        return TS_OK;
    }
    else
    {
        return TS_ERR;
    }
}

static ts_s32 uvc_start_video(ts_void)
{
    ts_isp_upload_attr_s_t st_attr;

    if (sst_ctx.en_status != TDK_TUV_STATUS_IDLE)
        return -1;
    else
        sst_ctx.en_status = TDK_TUV_STATUS_RUN;

    upl_req_bind();

    st_attr.u32_module = sst_ctx.u32_module;
    st_attr.u32_chn = TS_ISP_UPL_PRIMARY_CH;
    st_attr.s32_frame_num = 0;
    st_attr.u32_tmout_ms = 500;

    tdl_isp_run_upload(0, (ts_isp_upload_attr_s_t *)&st_attr);

    return 0;
}

ts_void usbd_video_open(ts_u08 intf)
{
    if (0 == sst_ctx.u32_open)
    {
        sst_ctx.u32_open = 1;
        USB_LOG_RAW("OPEN\r\n");
  
        vot_uvc_set_attr(0, sst_ctx.u32_module, 0);
        usb_uvc_reset();
        ts_cpl_up((ts_cpl_semphr_s_t *)&sst_ctx.st_ready_sema);
    }
}

ts_void usbd_video_close(ts_u08 intf)
{
    if (sst_ctx.u32_open > 0)
    {
        USB_LOG_RAW("CLOSE\r\n");
        sst_ctx.u32_open = 0;

        ts_cpl_up((ts_cpl_semphr_s_t *)&sst_ctx.st_uvc_sema);
    }
}

static struct usbd_interface intf0;
static struct usbd_interface intf1;

#ifdef CONFIG_UVC_WITH_ACM
static struct usbd_interface acm_intf0;
static struct usbd_interface acm_intf1;
#endif

static ts_void uvc_req_cb(usbd_edpt_s_t *edpt, usb_req_s_t *req)
{
    tdl_isp_upload_put_free_node(0, (ts_void *)req, TS_ISP_UPL_PRIMARY_CH);
    // req->buf = (ts_u08 *)req->dma_addr;
}

static ts_void upl_req_ready_cb(ts_void *pv_node)
{
    if (pv_node != ts_null)
    {
        if ((0 == sst_ctx.u32_cfged) || (0 == sst_ctx.u32_open))
        {
            tdl_isp_upload_put_free_node(0, pv_node, TS_ISP_UPL_PRIMARY_CH);
        }
        else
        {
#if 0
                uvc_prepare_vbuf((usb_req_s_t *)pv_node);
                usbd_ep_queue(VIDEO_IN_EP, (usb_req_s_t *)pv_node);
                //vision_log_debug("p:%p\n", pv_node);
#else
            ts_cpl_list_add_tail((ts_cpl_list_head_s_t *)pv_node, (ts_cpl_list_head_s_t *)&sst_ctx.st_rdy_head);
            ts_cpl_up((ts_cpl_semphr_s_t *)&sst_ctx.st_uvc_sema);

            // vision_log_debug("p:%p\n", pv_node);
#endif
        }
    }
}

#ifdef CONFIG_UVC_WITH_ACM
static ts_void acm_read_cb(usbd_edpt_s_t *edpt, usb_req_s_t *req)
{
    ts_cpl_up((ts_cpl_semphr_s_t *)&sst_ctx.st_acm_rd_sema);
}

static ts_u32 acm_read_pkg(ts_u08 *pu08_rbuf, ts_u32 u32_max_size)
{
    usb_req_s_t *pst_req;

    /* 硬件限制，单次最大 maxpacket */
    if (u32_max_size > CDC_MAX_MPS)
    {
        vision_log_debug("CDC-ACM max size(%d) must < %d\n", u32_max_size, CDC_MAX_MPS);

        return 0;
    }

    pst_req = (usb_req_s_t *)&sst_ctx.st_acm_rd_req;
    pst_req->buf = (ts_void *)pu08_rbuf;
    pst_req->dma_addr = (ts_dma_addr_t)pu08_rbuf;
    pst_req->actual = 0;
    pst_req->dma_sync = 1;

    pst_req->complete = acm_read_cb;
    pst_req->length = u32_max_size;
    ts_cpl_list_del_init((ts_cpl_list_head_s_t *)&pst_req->list);

    usbd_ep_queue(CDC_OUT_EP, pst_req);
    ts_cpl_down((ts_cpl_semphr_s_t *)&sst_ctx.st_acm_rd_sema);

    return pst_req->actual;
}

/* 例子为单线程读，如多线程，需加入互斥锁 */
ts_u32 acm_read(ts_u08 *pu08_rbuf, ts_u32 u32_max_size, ts_u32 u32_tmout)
{
    ts_u32 i, j, u32_cnt, u32_time_diff, u32_rd_cnt;
    ts_cpl_timeval_s_t st_time0, st_time1;
    ts_s32 s32_ret, s32_ms;

    u32_time_diff = 0;
    ts_cpl_gettimeofday(&st_time0);
    while (1)
    {
        if (sst_ctx.u32_pkg_w_idx >= sst_ctx.u32_usr_r_idx)
            u32_cnt = sst_ctx.u32_pkg_w_idx - sst_ctx.u32_usr_r_idx;
        else
            u32_cnt = (CDC_MAX_USR_SIZE - sst_ctx.u32_usr_r_idx) + sst_ctx.u32_pkg_w_idx;

        if (u32_cnt >= u32_max_size)
            break;

        s32_ms = (ts_s32)(u32_tmout - u32_time_diff);
        s32_ret = ts_cpl_down_timeout((ts_cpl_semphr_s_t *)&sst_ctx.st_acm_usr_sema, s32_ms);
        if (TS_ERR == s32_ret)
            break;

        ts_cpl_gettimeofday(&st_time1);
        u32_time_diff = ts_cpl_get_timedif(&st_time0, &st_time1);
        u32_time_diff = u32_time_diff / 1000;
        if (u32_time_diff >= u32_tmout)
            break;
    }

    if (u32_cnt > u32_max_size)
        u32_rd_cnt = u32_max_size;
    else
        u32_rd_cnt = u32_cnt;

    if (u32_rd_cnt > 0)
    {
        j = sst_ctx.u32_usr_r_idx;
        for (i = 0; i < u32_rd_cnt; i++)
        {
            pu08_rbuf[i] = sst_ctx.pu08_usr_buf[j];
            j++;
            if (j >= CDC_MAX_USR_SIZE)
                j = 0;
        }

        sst_ctx.u32_usr_r_idx = j;
    }

    return u32_rd_cnt;
}

static ts_void acm_write_cb(usbd_edpt_s_t *edpt, usb_req_s_t *req)
{
    ts_cpl_up((ts_cpl_semphr_s_t *)&sst_ctx.st_acm_wr_sema);
}

ts_u32 acm_write(ts_u08 *pu08_rbuf, ts_u32 u32_size)
{
    usbd_ep_start_write(CDC_IN_EP, pu08_rbuf, u32_size, acm_write_cb);
    // ts_cpl_down((ts_cpl_semphr_s_t *)&sst_ctx.st_acm_wr_sema);
    // if (ts_cpl_down_timeout((ts_cpl_semphr_s_t *)&sst_ctx.st_acm_wr_sema, 1000) != 0) {
    //     vision_log_warn("ACM TX timeout\n");
    //     // return 0;
    // }

    if (0 == sst_ctx.u32_cfged)
        return 0;
    else
        return u32_size;
}

ts_u32 vot_acm_write(ts_u08 *pu08_rbuf, ts_u32 u32_size)
{
    // if (sst_ctx.u32_cfged)
    {

        usbd_ep_start_write(CDC_IN_EP, pu08_rbuf, u32_size, acm_write_cb);
        // ts_cpl_down((ts_cpl_semphr_s_t *)&sst_ctx.st_acm_wr_sema);
        if (ts_cpl_down_timeout((ts_cpl_semphr_s_t *)&sst_ctx.st_acm_wr_sema, 100) != 0) {
            vision_log_warn("ACM TX timeout\n");
            // return 0;
        }

        if (0 == sst_ctx.u32_cfged)
            return 0;
        else
            return u32_size;
    }

    return 0;
}

extern SysCtrlInfo_s_t global_sys_ctrl_info;

static ts_void cdc_acm_msg_process(ts_void *pv_param)
{
    TS_UNUSED(pv_param);

    // ts_s32 s32_ret;
    ring_buffer_t uart_rb;
    ts_u08 *rd_buf;
    ts_u32 rnd_cnt;
    ts_u32 bytes_index;
    ts_u32 left_bytes;
    // ts_u32  m_left_bytes;

    state_machine_t m_state_machine;
    m_state_machine.m_state = STATE_WAIT_HEADER;
    m_state_machine.m_frame_type = 0;
    m_state_machine.m_complete_frame_len = 0;
    m_state_machine.m_frame_bytes_index = 0;
#ifdef Receive_JSON_File_EN
    ts_u08 *rd_json_buf = (ts_u08 *)global_sys_ctrl_info.m_json_buff; // add by changguanghe 2026-03-18, 避免每次解析都在栈上申请大内存，改为全局内存
    ts_u08 *json_str = (ts_u08 *)global_sys_ctrl_info.m_json_buff;
    static ts_u16 rx_json_buf_count = 0;
    memset(rd_json_buf, 0, 1024 * 4);
#endif
    while (sst_ctx.m_ringbuf_process_flag == 0)
    {

        get_ring_buffer_info(&uart_rb, &m_acm_ring_buf_instance);
        if (uart_rb.write_index != uart_rb.read_index)
        {

            get_ring_buffer_info(&uart_rb, &m_acm_ring_buf_instance);
            if (uart_rb.left_bytes == 0)
            {
                ts_cpl_msleep(20);
                continue;
            }

            bytes_index = uart_rb.left_bytes + uart_rb.read_index;

            // vision_log_debug("uart_rb.read_index=%u  uart_rb.left_bytes=%u uart_rb.write_index %u \n", uart_rb.read_index ,uart_rb.left_bytes ,uart_rb.write_index);
            if (bytes_index > uart_rb.uart_buff_len)
            {

                rd_buf = uart_rb.uart_buff;
                rd_buf += uart_rb.read_index;
                rnd_cnt = uart_rb.uart_buff_len - uart_rb.read_index;
                vot_hbp_parse_cb_v2(&m_state_machine, rd_buf, rnd_cnt, &vot_acm_write);
#ifdef Receive_JSON_File_EN
                memcpy(rd_json_buf, rd_buf, rnd_cnt); // add by changguanghe 2026-03-18
                rd_json_buf += rnd_cnt;
#endif
                rd_buf = uart_rb.uart_buff;
                left_bytes = uart_rb.left_bytes - rnd_cnt;
                vot_hbp_parse_cb_v2(&m_state_machine, rd_buf, left_bytes, &vot_acm_write);
                uart_rb.read_index = (bytes_index % uart_rb.uart_buff_len);
#ifdef Receive_JSON_File_EN
                memcpy(rd_json_buf, rd_buf, left_bytes); // add by changguanghe 2026-03-18
                rd_json_buf += left_bytes;
#endif
            }
            else
            {

                rd_buf = uart_rb.uart_buff;
                rd_buf += uart_rb.read_index;
                rnd_cnt = uart_rb.left_bytes;

                //  vot_uart_write(rd_buf,rnd_cnt);

                vot_hbp_parse_cb_v2(&m_state_machine, rd_buf, rnd_cnt, &vot_acm_write);
                uart_rb.read_index = (bytes_index % uart_rb.uart_buff_len);
#ifdef Receive_JSON_File_EN
                memcpy(rd_json_buf, rd_buf, rnd_cnt); // add by changguanghe 2026-03-18
                rd_json_buf += rnd_cnt;
#endif
            }
#ifdef Receive_JSON_File_EN
            rx_json_buf_count++;
#endif
            ring_buffer_update_read(&uart_rb, &m_acm_ring_buf_instance);
            // vision_log_debug("uart_rb.read_index=%u  uart_rb.left_bytes=%u uart_rb.write_index %u \n", uart_rb.read_index ,uart_rb.left_bytes ,uart_rb.write_index);
        }
        else
        {
            ts_cpl_msleep(50);
#ifdef Receive_JSON_File_EN
            if (rx_json_buf_count > 0)
            {
                rx_json_buf_count++;
                if ((rx_json_buf_count > 30)) // add by changguanghe 2026-03-18, 避免长时间未解析导致的json数据过大占满内存，增加解析次数限制，超过限制则丢弃数据并重置计数器
                {
                    // vision_log_debug("json_str len: %d \n", strlen(json_str));
                    if (strlen((char *)json_str) > 250) // add by changguanghe 2026-03-18, 避免解析异常数据导致的系统问题，增加json长度限制，超过限制则丢弃数据并重置计数器
                    {
                        ParseJsonParameters((char *)json_str);
                    }
                    rd_json_buf = (ts_u08 *)global_sys_ctrl_info.m_json_buff;
                    memset(json_str, 0, 1024 * 4); // add by changguanghe 2026-03-18, 解析完成后重置json缓冲区和计数器
                    rx_json_buf_count = 0;
                }
            }
#endif
        }
    }
    vision_log_warn("leave: \n");
}

/* 模拟串口终端，将 cdc 读取的数据依次写入用户待取缓存 */
// static ts_void uvc_cdc_acm_process(ts_void *pv_param)
// {
//     ts_u32 i, j, k, u32_mbase, u32_rcnt;
//     ts_u08 *pu08_rbuf;

//     u32_mbase = (ts_u32)ts_cpl_tmm_malloc(pv_param, CDC_MAX_BUF, 32, "acm_rbuf");
//     k = 0;
//     while (1) {
//         /* 复用 uvc 状态 */
//         if (0 == sst_ctx.u32_cfged) {
//             ts_cpl_msleep(10);

//             continue;
//         }
//         u32_rcnt = 0;
//         pu08_rbuf = (ts_u08 *)u32_mbase;
//         u32_rcnt = acm_read_pkg(pu08_rbuf, CDC_MAX_BUF);
//         if (u32_rcnt > 0) {

//             ring_buffer_update_write( &m_acm_ring_buf_instance, pu08_rbuf, u32_rcnt);

// 			j = 0;
//             for (i = 0; i < u32_rcnt; i++) {
//                 vision_log_debug("0x%02x ", pu08_rbuf[i]);

//                 j++;
//                 if (j >= 16) {
//                     j = 0;
//                     vision_log_debug("\n");
//                 }
//             }

//             // if (j > 0)
//             //     vision_log_debug("\n");

//             // pu08_rbuf[0] = 0x55;
//             // pu08_rbuf[1] = (ts_u08)k;
//             // pu08_rbuf[2] = 0x32;
//             // pu08_rbuf[3] = 0x33;
//             // pu08_rbuf[4] = 0x34;
//             // pu08_rbuf[5] = 0x35;
//             // pu08_rbuf[6] = 0x36;
//             // pu08_rbuf[7] = 0x37;

//             // k++;
//             // acm_write(pu08_rbuf, 8);

//         }
//     }
// }

/* 模拟串口终端，将 cdc 读取的数据依次写入用户待取缓存 */
static ts_void uvc_cdc_acm_process(ts_void *pv_param)
{
    ts_u32 u32_rcnt;
    // ts_u32 u32_new_w_idx , u32_remain;
    ts_u08 *pu08_rbuf;

    ts_u32 u32_mbase;

    u32_mbase = (ts_u32)ts_cpl_tmm_malloc(pv_param, CDC_MAX_BUF, 32, "vot_acm_rbuf");


    while(sst_ctx.m_routine_flag == 1){
        if(sst_ctx.m_reset_uvc_flag == 1){
            ts_cpl_msleep(200);
            continue;
        }

        while (sst_ctx.m_reset_uvc_flag == 0)
        {
            /* 复用 uvc 状态 */
            // if (0 == sst_ctx.u32_cfged || sst_ctx.u32_open == 0)
            if (0 == sst_ctx.u32_cfged )
            {
                ts_cpl_msleep(10);

                /* 清零复位 */
                sst_ctx.u32_pkg_w_idx = 0;
                sst_ctx.u32_usr_r_idx = 0;

                continue;
            }

    #if 0
            u32_rcnt = acm_read_pkg(sst_ctx.pu08_pkg_buf, CDC_MAX_BUF);
            if (u32_rcnt > 0) {
                u32_remain = CDC_MAX_USR_SIZE - sst_ctx.u32_pkg_w_idx;
                pu08_rbuf = sst_ctx.pu08_usr_buf;
                pu08_rbuf += sst_ctx.u32_pkg_w_idx;
                u32_new_w_idx = sst_ctx.u32_pkg_w_idx;
                if (u32_remain >= u32_rcnt) {
                    ts_cpl_memcpy(pu08_rbuf, sst_ctx.pu08_pkg_buf, u32_rcnt);
                    u32_new_w_idx += u32_rcnt;
                    if (u32_new_w_idx >= CDC_MAX_USR_SIZE)
                        u32_new_w_idx = 0;
                } else {
                    /* 分两次写入 */
                    if (u32_remain > 0)
                        ts_cpl_memcpy(pu08_rbuf, sst_ctx.pu08_pkg_buf, u32_remain);

                    ts_cpl_memcpy(sst_ctx.pu08_usr_buf, (sst_ctx.pu08_pkg_buf + u32_remain), (u32_rcnt - u32_remain));
                    u32_new_w_idx += u32_rcnt;
                    u32_new_w_idx = u32_new_w_idx - CDC_MAX_USR_SIZE;
                }
                sst_ctx.u32_pkg_w_idx = u32_new_w_idx;
                ts_cpl_up((ts_cpl_semphr_s_t *)&sst_ctx.st_acm_usr_sema);

            

                u32_rcnt = 0;
                pu08_rbuf = (ts_u08 *)u32_mbase;
                u32_rcnt = acm_read(pu08_rbuf, CDC_MAX_BUF, 100);
                if (u32_rcnt > 0) {
                    ring_buffer_update_write( &m_acm_ring_buf_instance, pu08_rbuf, u32_rcnt);
                }

            }

    #else

            u32_rcnt = 0;
            pu08_rbuf = (ts_u08 *)u32_mbase;
            u32_rcnt = acm_read_pkg(pu08_rbuf, CDC_MAX_BUF);
            if (u32_rcnt > 0)
            {

                ring_buffer_update_write(&m_acm_ring_buf_instance, pu08_rbuf, u32_rcnt);
                vision_log_debug("cdc recv bytes: %d\n", u32_rcnt);
                // j = 0;
                // for (i = 0; i < u32_rcnt; i++) {
                //     vision_log_debug("0x%02x ", pu08_rbuf[i]);

                //     j++;
                //     if (j >= 16) {
                //         j = 0;
                //         vision_log_debug("\n");
                //     }
                // }
            }
    #endif
        }
        vision_log_warn("continue: \n");    
    }

    if (u32_mbase)
    {
        ts_cpl_tmm_free((ts_void *)u32_mbase);
    }
    vision_log_warn("leave: \n");
}

#endif

static ts_void uvc_process(ts_void *pv_param)
{
    usb_req_s_t *req;
    ts_ulong ul_flags;
    ts_u08 first_flag = 1;

    ts_mdev_t h_dev = 0;
    ts_u32 u32_sub = sst_ctx.u32_module;
    ts_u32 u32_mode = 0;

    while(sst_ctx.m_routine_flag == 1){
        if(sst_ctx.m_reset_uvc_flag == 1){
            ts_cpl_msleep(200);
            continue;
        }
        vot_uvc_set_attr(h_dev, u32_sub, u32_mode);

        while (sst_ctx.m_reset_uvc_flag == 0)
        {
            if ((0 == sst_ctx.u32_open) || (0 == sst_ctx.u32_cfged))
            {
                /* 等待唤醒 */
                vision_log_debug("sst_ctx.u32_open %d sst_ctx.u32_cfged %d \n", sst_ctx.u32_open, sst_ctx.u32_cfged);
                ts_cpl_down((ts_cpl_semphr_s_t *)&sst_ctx.st_ready_sema);
                ts_cpl_msleep(200);
                continue;
            }

            /* 启动发送 */
            if (first_flag == 1)
            {
                if ((1 == sst_ctx.u32_open) && (1 == sst_ctx.u32_cfged)){
                    uvc_start_video();
                    vision_log_debug("after uvc_start_video \n");
                }
                
                first_flag = 0;
            }
            else
            {
                if ((1 == sst_ctx.u32_open) && (1 == sst_ctx.u32_cfged)){
                    vot_uvc_start_video();
                    vision_log_debug("after vot_uvc_start_video \n");
                }
                
                vot_uvc_st_attr.u08_reset_flag = 0;
            }

            vision_log_debug("sst_ctx.u32_cfged %d  sst_ctx.u32_open %d\n", sst_ctx.u32_cfged, sst_ctx.u32_open);
            /* 循环发送 */
            while (sst_ctx.m_reset_uvc_flag == 0)
            {
                if ((0 == sst_ctx.u32_cfged) || (0 == sst_ctx.u32_open) || (vot_uvc_st_attr.u08_reset_flag == 1))
                    break;

                /* 等待图像数据 */
                if (ts_cpl_list_empty((ts_cpl_list_head_s_t *)&sst_ctx.st_rdy_head) > 0)
                    ts_cpl_down((ts_cpl_semphr_s_t *)&sst_ctx.st_uvc_sema);

                /* 可能是 close 处唤醒 */
                if ((0 == sst_ctx.u32_cfged) || (0 == sst_ctx.u32_open))
                {
                    vision_log_debug("uvc close\n");
                    vision_log_debug("sst_ctx.u32_cfged %d  sst_ctx.u32_open %d\n", sst_ctx.u32_cfged, sst_ctx.u32_open);
                    break;
                }

                /* 取出当前节点 */
                ts_cpl_spin_lock_irqsave((ts_cpl_spinlock_s_t *)&sst_ctx.st_lock, &ul_flags);

                if (0 == ts_cpl_list_empty((ts_cpl_list_head_s_t *)&sst_ctx.st_rdy_head))
                {
                    req = (usb_req_s_t *)sst_ctx.st_rdy_head.next;
                    // vision_log_debug("g:%p\n", req);
                    ts_cpl_list_del_init((ts_cpl_list_head_s_t *)req);
                }
                else
                {
                    req = ts_null;
                }

                ts_cpl_spin_unlock_irqrestore((ts_cpl_spinlock_s_t *)&sst_ctx.st_lock, &ul_flags);

                if (req != ts_null)
                {
                    uvc_prepare_vbuf(req);
                    usbd_ep_queue(VIDEO_IN_EP, req);
                }
                else
                {
                    vision_log_debug("Invalid wake-up\n");
                }
            }

            // vision_log_debug("vot_uvc_st_attr.u08_reset_flag %d : \n",vot_uvc_st_attr.u08_reset_flag);
            /* UVC 关闭，停止采集 */
            // uvc_stop_video();
            vot_uvc_stop_video();

            /* 全部取消已经挂载的队列 */
            usbd_dequeue(VIDEO_IN_EP, ts_null);
            vision_log_debug("leave: vot_uvc_st_attr.u08_reset_flag %d : \n", vot_uvc_st_attr.u08_reset_flag);
        }

        tdl_isp_stop_upload(0, TS_ISP_UPL_PRIMARY_CH);

        /* release all req */
        usbd_dequeue(VIDEO_IN_EP, ts_null);

        /* reclainm all req */
        upl_req_unbind();

        /* 全部取消已经挂载的队列 */
        usbd_dequeue(VIDEO_IN_EP, ts_null);

        vision_log_warn("continue: \n");    
    }
    vision_log_warn("leave uvc process: \n");
}

ts_void usb_uvc_reset(void)
{

    if (!(SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_BT656 || SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_BT601))
    {

        // #if !defined(VOT_MIPI_EN)
        if (!(SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_MIPI))
        {

            if (SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_UVC || SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_CVBS)
            {
                // dv210sdk，修复uvc画面错位问题

#ifdef VIDEO_ISO_EN
                usbd_set_uvc_hook(ts_null, ts_null, 2, MAX_PAYLOAD_SIZE);
#else
                usbd_set_uvc_hook(usbd_video_open, usbd_video_close, UVC_HD_SIZE, MAX_PAYLOAD_SIZE);
#endif
            }
            // #endif
        }
    }
}

ts_void usb_capture_init(ts_void)
{
#ifdef VIDEO_ISO_EN
    usbd_set_uvc_hook(ts_null, ts_null, 2, MAX_PAYLOAD_SIZE);
#else
    usbd_set_uvc_hook(usbd_video_open, usbd_video_close, UVC_HD_SIZE, MAX_PAYLOAD_SIZE);
#endif

    usbd_initialize(8, usbd_event_handler);
}

ts_void vot_uvc_ring_init(ts_void *pv_h_tmm)
{
    ts_cpl_task_s_t st_msg_task;
    sst_ctx.m_ringbuf_process_flag = 0;
#ifdef CONFIG_UVC_WITH_ACM
    ring_buffer_init(&(m_acm_ring_buf_instance), "vot_acm_ring_buf");
    /* cdc-acm 处理线程 */
    ts_cpl_kthread_tmm_create(&st_msg_task, (ts_cpl_kthread_t)cdc_acm_msg_process, pv_h_tmm, "acm_msg", 18, 10 * 2048, ts_null);

#endif
}

ts_void vot_uvc_ring_deinit(ts_void)
{
#ifdef CONFIG_UVC_WITH_ACM
    sst_ctx.m_ringbuf_process_flag = 1;
    ts_cpl_msleep(1000);
    ring_buffer_deinit(&(m_acm_ring_buf_instance));
#endif
}

ts_void vot_usb_uvc_deinit(void)
{
    vision_log_warn("enter: vot_usb_uvc_deinit \n");
    sst_ctx.m_reset_uvc_flag = 1;
    sst_ctx.u32_cfged = 0;
    sst_ctx.u32_open = 0;

// #ifdef CONFIG_UVC_WITH_ACM
//     usbd_remove_interface(&acm_intf0);    // 移除接口
//     usbd_remove_interface(&acm_intf1);    // 移除接口
// #endif
//     usbd_remove_interface(&intf1);    // 移除接口1 
//     usbd_remove_interface(&intf0);    // 移除接口0

    usbd_deinitialize();              // 释放USB设备核心资源
    ts_cpl_fix_msleep(1000);
    if (sst_ctx.u32_mem_addr)
    {
        ts_cpl_tmm_free((ts_void *)sst_ctx.u32_mem_addr);
        sst_ctx.u32_mem_addr = 0;
    }

    ts_cpl_semphr_destory((ts_cpl_semphr_s_t *)&sst_ctx.st_uvc_sema);
    ts_cpl_semphr_destory((ts_cpl_semphr_s_t *)&sst_ctx.st_ready_sema);
    ts_cpl_spinlock_destroy((ts_cpl_spinlock_s_t *)&sst_ctx.st_lock);

#ifdef CONFIG_UVC_WITH_ACM
    ts_cpl_semphr_destory((ts_cpl_semphr_s_t *)&sst_ctx.st_acm_rd_sema);
    ts_cpl_semphr_destory((ts_cpl_semphr_s_t *)&sst_ctx.st_acm_wr_sema);
    ts_cpl_semphr_destory((ts_cpl_semphr_s_t *)&sst_ctx.st_acm_usr_sema);

    if (sst_ctx.pu08_pkg_buf)
    {
        ts_cpl_tmm_free(sst_ctx.pu08_pkg_buf);
        sst_ctx.pu08_pkg_buf = 0;
    }
#endif

    vision_log_warn("leave: \n");
}

ts_void vot_usb_uvc_init(ts_void *pv_h_tmm)
{
    sst_ctx.m_usb_descriptor_init_flag = 0;
    sst_ctx.m_routine_flag = 1;
    usb_usr_init(pv_h_tmm);
    vot_uvc_ring_init(pv_h_tmm);

}

ts_void vot_uvc_usr_reset(ts_void)
{

    if (SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_UVC || SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_CVBS)
    {

        vot_usb_uvc_deinit();
        ts_cpl_msleep(1000);
        cvbs_pod_set(0);
        ts_cpl_msleep(1000);
        usb_usr_init(gpv_tmm_usr);

    }
}

ts_void usb_usr_init(ts_void *pv_h_tmm)
{
    ts_u32 i, u32_frm_size, u32_mem_size, u32_addr;
    usb_req_s_t *pst_req;
    ts_cpl_task_s_t st_task;
    static ts_u08 first_init_flag = 1;


    sst_ctx.m_ringbuf_process_flag = 0;
    // vision_log_debug("DBVAL(INTERVAL) = %d,%d,%d,%d\n", MAX_FRAME_SIZE, INTERVAL, (ts_ulong)(10000000 / CAM_FPS), MIN_BIT_RATE);

    /* 从 vpo 获取帧大小，此处需保证 vpo 帧大小 >= isp 帧大小 */
    tdl_mupl_get_frm_size((ts_u32 *)&u32_frm_size);

    /* 对获取的帧大小进行合法性检测，可根据实际情况修改, 此处不能小于 256x128x2 且不能大于 1280*1024*2, 修改成 640x(512+4)x2 */
    if ((u32_frm_size < 0x10000) || (u32_frm_size > 0x290000))
    {
        vision_log_debug("%s: frm size=%d\n", TS_EINVAL_STR, u32_frm_size);

        u32_frm_size = 640 * (512 + 8) * 2;
         //如果是vot_planck1280c
        // if(gu08_vot_planck1280c_en==1)
        #ifdef CONFIG_SNR_PLANCK1280C
            u32_frm_size = 1280 * 1024 * 2;
        #endif
    }
    else
    {
        /* 加上 hd_len */
        u32_frm_size += 4 * 1024;
    }

    vision_log_debug("fsz=%d\n", u32_frm_size);

    u32_mem_size = u32_frm_size * UVC_REQ_MAX_NUM;
    
    /* 申请 4 个视频缓冲 */
    tdl_mupl_get_frm_size((ts_u32 *)&u32_frm_size);

#if 1
    sst_ctx.u32_mem_addr = (ts_u32)ts_cpl_tmm_malloc(pv_h_tmm, u32_mem_size, 32, "uvc-mem");
#else
    sst_ctx.u32_mem_addr = 0x22000000;
#endif

    if (0 == sst_ctx.u32_mem_addr)
    {
        vision_log_debug("%s: memaddr=%x, task down\n", TS_ENOMEM_STR, sst_ctx.u32_mem_addr);

        return;
    }

    /* 将申请到的缓冲挂载到 req 上 */
    u32_addr = sst_ctx.u32_mem_addr;
    for (i = 0; i < UVC_REQ_MAX_NUM; i++)
    {
        pst_req = &sst_ctx.ast_video_req[i];

        pst_req->buf = (ts_void *)u32_addr;
        pst_req->dma_addr = u32_addr | 0x10000000;
        pst_req->dma_sync = 0;
        pst_req->complete = uvc_req_cb;

        u32_addr += u32_frm_size;
    }

    vision_log_debug("pkgsz=%d,paysz=%d\n", VIDEO_PACKET_SIZE, MAX_PAYLOAD_SIZE);
    // uvc_show_desc();
    if (SystemParaMenu2.m_uvc_output_format == UVC_YUV_FORMAT_UYVY) {
        uvc_update_desc((ts_u08 *)&config_descriptor_uyvy, sizeof(config_descriptor_uyvy));
    }
    else {
        uvc_update_desc((ts_u08 *)&config_descriptor, sizeof(config_descriptor));
    }

    sst_ctx.en_status = TDK_TUV_STATUS_IDLE;
    sst_ctx.u32_frm_id = 0;
    ts_cpl_semphr_init((ts_cpl_semphr_s_t *)&sst_ctx.st_uvc_sema, 0);
    ts_cpl_semphr_init((ts_cpl_semphr_s_t *)&sst_ctx.st_ready_sema, 0);
    ts_cpl_spinlock_init((ts_cpl_spinlock_s_t *)&sst_ctx.st_lock);

    sst_ctx.u32_cfged = 0;
    sst_ctx.u32_open = 0;

#ifdef CONFIG_UVC_WITH_ACM
    ts_cpl_semphr_init((ts_cpl_semphr_s_t *)&sst_ctx.st_acm_rd_sema, 0);
    ts_cpl_semphr_init((ts_cpl_semphr_s_t *)&sst_ctx.st_acm_wr_sema, 0);
    ts_cpl_semphr_init((ts_cpl_semphr_s_t *)&sst_ctx.st_acm_usr_sema, 0);

    /* 申请两片缓冲，一片用于单次传输 512 大小，一片模拟串口内部大缓存，用户从此缓存读取数据, 可以根据实际情况调整 */
    u32_addr = (ts_u32)ts_cpl_tmm_malloc(pv_h_tmm, CDC_MAX_RBUF_SIZE, 32, "acm-rbuf");
    if (0 == u32_addr)
    {
        vision_log_debug("%s: for acm\n", TS_ENOMEM_STR);

        ts_cpl_tmm_free((ts_void *)sst_ctx.u32_mem_addr);

        return;
    }

    sst_ctx.pu08_pkg_buf = (ts_u08 *)(u32_addr);
    sst_ctx.pu08_usr_buf = (ts_u08 *)(u32_addr + CDC_MAX_BUF);
    sst_ctx.u32_pkg_w_idx = 0;
    sst_ctx.u32_usr_r_idx = 0;
    sst_ctx.u32_usr_size = CDC_MAX_USR_SIZE;
#endif

    sst_ctx.u32_open = 0;
    if (sst_ctx.m_usb_descriptor_init_flag == 0)
    {
        usbd_desc_register(&video_descriptor);
        sst_ctx.m_usb_descriptor_init_flag = 1;
    }
    usbd_add_interface(usbd_video_init_intf(&intf0, sst_ctx.u32_interval, sst_ctx.u32_max_frm_size, MAX_PAYLOAD_SIZE));
    usbd_add_interface(usbd_video_init_intf(&intf1, sst_ctx.u32_interval, sst_ctx.u32_max_frm_size, MAX_PAYLOAD_SIZE));

#ifdef CONFIG_UVC_WITH_ACM
    usbd_add_interface(usbd_cdc_acm_init_intf(&acm_intf0));
    usbd_add_interface(usbd_cdc_acm_init_intf(&acm_intf1));
#endif

    usb_capture_init();

    if(first_init_flag == 1){
        /* uvc 包处理线程 */
        ts_cpl_kthread_tmm_create(&st_task, (ts_cpl_kthread_t)uvc_process, ts_null, "uvc_pthr", 4, 2*2048, ts_null);

    #ifdef CONFIG_UVC_WITH_ACM
        /* cdc-acm 接收处理线程 */
        ts_cpl_kthread_tmm_create(&st_task, (ts_cpl_kthread_t)uvc_cdc_acm_process, pv_h_tmm, "acm_pthr", 12, 2*2048, ts_null);

    #endif
        first_init_flag = 0;
    }
    sst_ctx.m_reset_uvc_flag = 0;
}

#ifdef CONFIG_UVC_WITH_ACM
extern ts_s32 ymodem_receive(ts_u08 *pu08_buf, ts_u32 *pu32_act_len);
static ts_u08 *spu08_ybuf = ts_null;
TS_DECLARE_CMD(cdc)
{
#if 1
    ts_u32 u32_size;

    if (ts_null == spu08_ybuf)
    {
        spu08_ybuf = (ts_u08 *)ts_cpl_tmm_malloc(ts_null, 1 * 1024 * 1024, 32, "ybuf");
        if (ts_null == spu08_ybuf)
            return 0;
    }

    ymodem_receive(spu08_ybuf, (ts_u32 *)&u32_size);
    if (u32_size > 0)
    {
        ts_lib_sfc_set_protect(0, 4, 0);

        ts_lib_sfc_raw_write(0x80000, spu08_ybuf, u32_size);

        ts_lib_sfc_set_protect(0, 4, 1);
    }
#else
    ts_u08 au08_buf[64];
    ts_u32 i, j, u32_rcnt;

    u32_rcnt = acm_read(&au08_buf, 60, 100);
    if (u32_rcnt > 0)
    {
        j = 0;
        for (i = 0; i < u32_rcnt; i++)
        {
            vision_log_debug("%02x ", au08_buf[i]);

            j++;
            if (j >= 16)
            {
                j = 0;
                vision_log_debug("\n");
            }
        }

        if (j > 0)
            vision_log_debug("\n");
    }

#endif

    return 0;
}
#endif

ts_s32 vot_uvc_stop_video(ts_void)
{

    if (!(SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_CVBS || SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_UVC))
    {
        return TS_ERR;
    }

    if (TDK_TUV_STATUS_RUN == sst_ctx.en_status)
    {
        tdl_isp_stop_upload(0, TS_ISP_UPL_PRIMARY_CH);

        /* release all req */
        usbd_dequeue(VIDEO_IN_EP, ts_null);

        /* reclainm all req */
        upl_req_unbind();

        sst_ctx.en_status = TDK_TUV_STATUS_IDLE;
        vision_log_debug("sst_ctx.u32_open %d sst_ctx.u32_cfged %d en_status %d \n", sst_ctx.u32_open, sst_ctx.u32_cfged, sst_ctx.en_status);
        return TS_OK;
    }
    else
    {
        return TS_ERR;
    }
    vision_log_debug("leave: \n");
}

ts_void vot_uvc_set_attr(ts_mdev_t h_dev, ts_u32 u32_sub, ts_u32 u32_mode)
{
    ts_u32 index = u32_sub;
    if (TS_UPL_POS_VPO_Y8 == index || index == TS_UPL_POS_VPO_YUV || (index >= TS_UPL_POS_RAW && index <= TS_UPL_POS_ISP))
    {
        u32_sub = index;
    }
    else
    {
        u32_sub = TS_UPL_POS_VPO_YUV;
        vision_log_debug("error : index %d \n", index);
    }

    vision_log_debug("u32_sub :  %d \n", u32_sub);
    vot_uvc_st_attr.u32_module = u32_sub;
    vot_uvc_st_attr.u32_chn = TS_ISP_UPL_PRIMARY_CH;
    vot_uvc_st_attr.s32_frame_num = 0;
    vot_uvc_st_attr.u32_tmout_ms = 500;
    vot_uvc_st_attr.h_dev = h_dev;
    vot_uvc_st_attr.u08_reset_flag = 1;
}

ts_s32 vot_uvc_start_video(void)
{
    ts_isp_upload_attr_s_t st_attr;


    if (!(SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_CVBS || SystemParaMenu2.m_video_out_mode == VIDEO_OUTPUT_MODE_ENUM_UVC))
    {
        return -1;
    }

    if (sst_ctx.en_status != TDK_TUV_STATUS_IDLE)
        return -1;
    else
        sst_ctx.en_status = TDK_TUV_STATUS_RUN;

    upl_req_bind();

    st_attr.u32_module = vot_uvc_st_attr.u32_module;
    st_attr.u32_chn = vot_uvc_st_attr.u32_chn; // TS_ISP_UPL_PRIMARY_CH;
    st_attr.s32_frame_num = vot_uvc_st_attr.s32_frame_num;
    st_attr.u32_tmout_ms = vot_uvc_st_attr.u32_tmout_ms;

    tdl_isp_run_upload(0, (ts_isp_upload_attr_s_t *)&st_attr);

    vision_log_debug("enter:sst_ctx.en_status %d \n" ,sst_ctx.en_status );

    // vision_log_debug("leave: vot_uvc_st_attr.u32_module %d \n" ,vot_uvc_st_attr.u32_module);
    return 0;
}

/*
 * Copyright (c) 2024, sakumisu
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <base/ts_type.h>
#include <cpl/ts_cpl.h>
#include <uapi/usb/usbd_core.h>
#include <uapi/usb/usb_cdc.h>
#include <uapi/usb/usbd_cdc_acm.h>

/*!< endpoint address */
#define CDC_IN_EP  0x83
#define CDC_OUT_EP 0x02

/* 实际不存在 */
#define CDC_INT_EP 0x87

#define USBD_VID           0x5474
#define USBD_PID           0xFFFF
#define USBD_MAX_POWER     100
#define USBD_LANGID_STRING 1033

/*!< config descriptor size */
#define USB_CONFIG_SIZE     (9 + CDC_ACM_DESCRIPTOR_LEN)
#define CDC_MAX_MPS         512
#define CDC_MAX_BUF         CDC_MAX_MPS

static const ts_u08 device_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xEF, 0x02, 0x01, USBD_VID, USBD_PID, 0x0100, 0x01)
};

static const ts_u08 config_descriptor[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, 0x02, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    CDC_ACM_DESCRIPTOR_INIT(0x00, CDC_INT_EP, CDC_OUT_EP, CDC_IN_EP, CDC_MAX_MPS, 0x02)
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

static const char *string_descriptors[] = {
    (const char[]){ 0x09, 0x04 }, /* Langid */
    "tdk",                  /* Manufacturer */
    "tdk cdc demo",         /* Product */
    "202506241005",         /* Serial Number */
};

typedef struct acm_ctx_s {
    /* usb 可用信号量 */
    ts_cpl_semphr_s_t st_ready_sema;

    /* acm 接收完成信号量和互斥量 */
    ts_cpl_semphr_s_t st_read_sema;
    ts_cpl_mutex_s_t st_read_mutex;

    ts_u32 u32_rd_cnt;

    /* usb 发完信号量 */
    ts_cpl_semphr_s_t st_done_sema;

    ts_u32 u32_ready;
    ts_u32 u32_rdy_up;
} acm_ctx_s_t;

static acm_ctx_s_t sst_acm_ctx = {0};

static const ts_u08 *device_descriptor_callback(ts_u08 speed)
{
    return device_descriptor;
}

static const ts_u08 *config_descriptor_callback(ts_u08 speed)
{
    return config_descriptor;
}

static const ts_u08 *device_quality_descriptor_callback(ts_u08 speed)
{
    return device_quality_descriptor;
}

static const char *string_descriptor_callback(ts_u08 speed, ts_u08 index)
{
    if (index > 3) {
        return ts_null;
    }
    return string_descriptors[index];
}

const struct usb_descriptor cdc_descriptor = {
    .device_descriptor_callback = device_descriptor_callback,
    .config_descriptor_callback = config_descriptor_callback,
    .device_quality_descriptor_callback = device_quality_descriptor_callback,
    .string_descriptor_callback = string_descriptor_callback
};

static ts_void usbd_event_handler(ts_u08 event)
{
    switch (event) {
        case USBD_EVENT_RESET:
            sst_acm_ctx.u32_ready = 0;

            break;
        case USBD_EVENT_CONNECTED:
            break;
        case USBD_EVENT_DISCONNECTED:
            break;
        case USBD_EVENT_RESUME:
            break;
        case USBD_EVENT_SUSPEND:
            sst_acm_ctx.u32_ready = 0;

            break;
        case USBD_EVENT_CONFIGURED:
            sst_acm_ctx.u32_ready = 1;
            if (sst_acm_ctx.u32_rdy_up > 0)
                ts_cpl_up((ts_cpl_semphr_s_t *)&sst_acm_ctx.st_ready_sema);

            break;
        case USBD_EVENT_SET_REMOTE_WAKEUP:
            break;
        case USBD_EVENT_CLR_REMOTE_WAKEUP:
            break;

        default:
            break;
    }
}

static struct usbd_interface intf0;
static struct usbd_interface intf1;


static ts_void acm_send_pkg_cb(usbd_edpt_s_t *edpt, usb_req_s_t *req)
{
    ts_cpl_up((ts_cpl_semphr_s_t *)&sst_acm_ctx.st_done_sema);
}

static ts_void acm_read_cb(usbd_edpt_s_t *edpt, usb_req_s_t *req)
{
    sst_acm_ctx.u32_rd_cnt = req->actual;
    ts_cpl_up((ts_cpl_semphr_s_t *)&sst_acm_ctx.st_read_sema);
}


/* 单线程读，无需互斥 */
static ts_u32 acm_read(ts_u08 *pu08_rbuf, ts_u32 u32_max_size)
{
    usbd_ep_start_read(CDC_OUT_EP, pu08_rbuf, u32_max_size, acm_read_cb);
    ts_cpl_down((ts_cpl_semphr_s_t *)&sst_acm_ctx.st_read_sema);

    return sst_acm_ctx.u32_rd_cnt;
}


static ts_void cdc_acm_process(ts_void *pv_param)
{
    ts_u32 i, j, k, u32_mbase, u32_rcnt;
    ts_u08 *pu08_rbuf;

    u32_mbase = (ts_u32)ts_cpl_tmm_malloc(pv_param, CDC_MAX_BUF, 32, "acm_rbuf");
    k = 0;
    while (1) {
        if (0 == sst_acm_ctx.u32_ready) {
            sst_acm_ctx.u32_rdy_up = 1;
            ts_cpl_down((ts_cpl_semphr_s_t *)&sst_acm_ctx.st_ready_sema);
            sst_acm_ctx.u32_rdy_up = 0;
            k = 0;
        }

        pu08_rbuf = (ts_u08 *)u32_mbase;
        u32_rcnt = acm_read(pu08_rbuf, CDC_MAX_BUF);
        if (u32_rcnt > 0) {
            j = 0;
            for (i = 0; i < u32_rcnt; i++) {
                ts_cpl_printk("%02x ", pu08_rbuf[i]);

                j++;
                if (j >= 16) {
                    j = 0;
                    ts_cpl_printk("\n");
                }
            }

            if (j > 0)
                ts_cpl_printk("\n");

            pu08_rbuf[0] = 0x55;
            pu08_rbuf[1] = (ts_u08)k;
            pu08_rbuf[2] = 0x32;
            pu08_rbuf[3] = 0x33;
            pu08_rbuf[4] = 0x34;
            pu08_rbuf[5] = 0x35;
            pu08_rbuf[6] = 0x36;
            pu08_rbuf[7] = 0x37;

            k++;
    
            usbd_ep_start_write(CDC_IN_EP, pu08_rbuf, 8, acm_send_pkg_cb);
            ts_cpl_down((ts_cpl_semphr_s_t *)&sst_acm_ctx.st_done_sema);
        }
    }
}

ts_void usb_usr_init(ts_void *pv_tmm)
{
    ts_cpl_task_s_t st_task;

    ts_cpl_semphr_init((ts_cpl_semphr_s_t *)&sst_acm_ctx.st_ready_sema, 0);
    ts_cpl_semphr_init((ts_cpl_semphr_s_t *)&sst_acm_ctx.st_done_sema, 0);
    ts_cpl_semphr_init((ts_cpl_semphr_s_t *)&sst_acm_ctx.st_read_sema, 0);

    ts_cpl_mutex_init((ts_cpl_mutex_s_t *)&sst_acm_ctx.st_read_mutex);

    usbd_desc_register(&cdc_descriptor);

    usbd_add_interface(usbd_cdc_acm_init_intf(&intf0));
    usbd_add_interface(usbd_cdc_acm_init_intf(&intf1));

    usbd_initialize(8, usbd_event_handler);

    ts_cpl_kthread_create(&st_task, (ts_cpl_kthread_t)cdc_acm_process, pv_tmm, "acm", 12, 2048);
}

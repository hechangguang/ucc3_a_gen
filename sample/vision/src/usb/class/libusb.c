/*
 * libusb.c -- tdk libusb 使用例子
 */
#include <base/ts_compiler.h>
#include <uapi/usb/usbd_core.h>
#include <uapi/upl_v100.h>
#include <uapi/viu_v100.h>
#include <stdio.h>
#include <stdlib.h>
#include "api.h"
#include "f_tdk.h"

/* T10x 视频输出端口 */
#define LIBUSB_EP_STREAM        0x81

/* 板端命令接收端口 */
#define LIBUSB_EP_CMD           0x02

/* 板端命令回应端口 */
#define LIBUSB_EP_RESPONSE      0x83

/* 作为仿真源输入 */
#define LIBUSB_EP_VIU           0x06

#define LIBUSB_EP_NUM           4

#define USBD_VID                0x5474
#define USBD_PID                0x5410
#define USBD_MAX_POWER          100
#define USBD_LANGID_STRING      1033

#define USB_CONFIG_SIZE         (9 + 9 + 7 + 7 + 7 + 7)

#define INTF_NUM                1

/* USB_ENDPOINT_TYPE_BULK 类型部分主机不支持 1024 */
#define LIBUSB_EP_MPS           512


static const ts_u08 device_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, USBD_VID, USBD_PID, 0x0200, 0x01)
};

static const ts_u08 config_descriptor[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, INTF_NUM, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
    USB_INTERFACE_DESCRIPTOR_INIT(0x00, 0x00, LIBUSB_EP_NUM, 0xff, 0x00, 0x00, 0x04),

    USB_ENDPOINT_DESCRIPTOR_INIT(LIBUSB_EP_STREAM, USB_ENDPOINT_TYPE_BULK, LIBUSB_EP_MPS, 0x00),

#if 0
    USB_ENDPOINT_DESCRIPTOR_INIT(LIBUSB_EP_CMD, USB_ENDPOINT_TYPE_INTERRUPT, LIBUSB_EP_MPS, 0x00),
    USB_ENDPOINT_DESCRIPTOR_INIT(LIBUSB_EP_RESPONSE, USB_ENDPOINT_TYPE_INTERRUPT, LIBUSB_EP_MPS, 0x00),
    USB_ENDPOINT_DESCRIPTOR_INIT(LIBUSB_EP_VIU, USB_ENDPOINT_TYPE_BULK, LIBUSB_EP_MPS, 0x00),
#else
    USB_ENDPOINT_DESCRIPTOR_INIT(LIBUSB_EP_VIU, USB_ENDPOINT_TYPE_BULK, LIBUSB_EP_MPS, 0x00),
    USB_ENDPOINT_DESCRIPTOR_INIT(LIBUSB_EP_RESPONSE, USB_ENDPOINT_TYPE_BULK, LIBUSB_EP_MPS, 0x00),
    USB_ENDPOINT_DESCRIPTOR_INIT(LIBUSB_EP_CMD, USB_ENDPOINT_TYPE_BULK, LIBUSB_EP_MPS, 0x00),
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

#define LIBUSB_MAX_STRING_INDEX     (5)
static const char *string_descriptors[] = {
    (const char[]){ 0x09, 0x04 }, /* Langid */
    "top-silicon",                  /* Manufacturer */
    "host client interface",      /* Product */
    "2022123456",                 /* Serial Number */
    "libusb DEMO 1",            /* STRING4 */
    "libusb DEMO 2",            /* STRING5 */
};


#define LIBUSB_CMD_BUF_SIZE         (8 * 1024 + 512)
#define HCI_CFG_WIDTH               (1280)
#define HCI_CFG_HEIGHT              (1024)

#ifdef CONFIG_SNR_PLANCK1280C
//释放1帧1280x1024x2的空间
#define HCI_CFG_INREQ_NUM           (3)
#else
#define HCI_CFG_INREQ_NUM           (4)
#endif
#define HCI_CFG_CMDBUF_NUM          (2)
#define HCI_CFG_VLEN_NUM            (2)

/* 支持 yuv 或者 r16 格式，所有最大需要乘以 2 */
#define HCI_CFG_BUF_SIZE            ((HCI_CFG_WIDTH * (HCI_CFG_HEIGHT + 4)) * 2)
#define HCI_CFG_TATOL_MEM_SIZE      (HCI_CFG_BUF_SIZE * HCI_CFG_INREQ_NUM)

typedef struct libusb_ctx_s {
    /* 记录初始化状态 */
    ts_u32 u32_inited;

    ts_void *pv_tmm;

    /* usb 是否已配置可以使用 */
    ts_u32 u32_ready;

    /* 保存线程句柄 */
    ts_cpl_task_s_t st_task;

    /* 命令接收和回应内存 */
    ts_void *pv_cmd_tmm;
    ts_u08 *pu08_cmd_buf;
    ts_u08 *pu08_resp_buf;

    /* 命令接收 req */
    usb_req_s_t ast_cmd_req[HCI_CFG_CMDBUF_NUM];
    ts_u32 u32_max_size;
    ts_u32 u32_buf_id;
    ts_cpl_mutex_s_t st_cmd_mutex;
    ts_cpl_semphr_s_t st_cmd_sema;
    ts_u32 u32_ack_index;
    ts_u32 u32_up_en;

    /* 演示不定长读取，调图工具不需要 */
    usb_req_s_t st_vlen_req;
    ts_cpl_semphr_s_t st_vlen_sema;

    /* viu 接收 req 和相应的信号量,不进行互斥，靠用户保证 */
    usb_req_s_t st_viu_req;
    ts_cpl_semphr_s_t st_viu_sema;

    /* 视频流控制 */
    ts_u32 u32_start;

    /* 0 : 普通模式， > 0: 防破帧模式 */
    ts_u32 u32_mode;

    /* 帧计数 */
    ts_u32 u32_frm_cnt;

    ts_u32 u32_mem_addr;
    usb_req_s_t ast_video_req[HCI_CFG_INREQ_NUM];
    tdk_tuv_status_en_t en_status;

    /* 单次触发模式下 req 队列 */
    ts_cpl_list_head_s_t st_rdy_head;

    /* 单次触发下同步信号以及自旋锁 */
    ts_cpl_semphr_s_t st_mode_sema;
    ts_cpl_spinlock_s_t st_lock;

    /* 配置完成信号量 */
    ts_cpl_semphr_s_t *pst_ready_sema;

    /* 用于接收命令的同步信息量 */
    ts_cpl_semphr_s_t st_resp_sema;
} libusb_ctx_s_t;

static struct usbd_interface intf0;
static libusb_ctx_s_t sst_ctx = {0};

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
    if (index >= LIBUSB_MAX_STRING_INDEX)
        return "none";

    return string_descriptors[index];
}

const struct usb_descriptor winusb_descriptor = {
    .device_descriptor_callback = device_descriptor_callback,
    .config_descriptor_callback = config_descriptor_callback,
    .device_quality_descriptor_callback = device_quality_descriptor_callback,
    .string_descriptor_callback = string_descriptor_callback,
};

static ts_void usbd_event_handler(ts_u08 event)
{
    //ts_cpl_printk("ev=%d\n", event);

    switch (event) {
        case USBD_EVENT_RESET:
            sst_ctx.u32_ready = 0;

            /* 如果有命令等待，唤醒 */
            if (sst_ctx.u32_up_en > 0)
                ts_cpl_up((ts_cpl_semphr_s_t *)&sst_ctx.st_cmd_sema);

            break;
        case USBD_EVENT_CONNECTED:
            break;
        case USBD_EVENT_DISCONNECTED:
            break;
        case USBD_EVENT_RESUME:
            sst_ctx.u32_ready = 1;
            ts_cpl_up((ts_cpl_semphr_s_t *)sst_ctx.pst_ready_sema);
            break;
        case USBD_EVENT_SUSPEND:
            sst_ctx.u32_ready = 0;

            /* 如果有命令等待，唤醒 */
            if (sst_ctx.u32_up_en > 0)
                ts_cpl_up((ts_cpl_semphr_s_t *)&sst_ctx.st_cmd_sema);

            break;
        case USBD_EVENT_CONFIGURED:
            sst_ctx.u32_ready = 1;
            ts_cpl_up((ts_cpl_semphr_s_t *)sst_ctx.pst_ready_sema);
            break;
        case USBD_EVENT_SET_REMOTE_WAKEUP:
            break;
        case USBD_EVENT_CLR_REMOTE_WAKEUP:
            break;

        default:
            break;
    }
}


ts_u32 libusb_chk_alive(ts_void)
{
    return sst_ctx.u32_ready;
}

ts_void libusb_close_all(ts_void)
{
    usbd_ep_close(LIBUSB_EP_STREAM);
    usbd_ep_close(LIBUSB_EP_CMD);
    usbd_ep_close(LIBUSB_EP_RESPONSE);
    usbd_ep_close(LIBUSB_EP_VIU);
}


/* 不定长读取示例 */
static ts_void libusb_vlen_cb(usbd_edpt_s_t *edpt, usb_req_s_t *req)
{
    ts_cpl_up((ts_cpl_semphr_s_t *)&sst_ctx.st_vlen_sema);
}

/* 多线程需要锁，此应用为单线程，不进行互斥操作 */
ts_u32 libusb_vlen_read(ts_u08 *pu08_rbuf, ts_u32 u32_max_size)
{
    usb_req_s_t *pst_req;

    pst_req = (usb_req_s_t *)&sst_ctx.st_vlen_req;
    pst_req->buf = (ts_void *)pu08_rbuf;
    pst_req->dma_addr = (ts_dma_addr_t)pu08_rbuf;
    pst_req->actual = 0;

    /* CPU 与 DMA 需要同步,并使能不定长传输 */
    pst_req->dma_sync = 1 | 0x8000;

    pst_req->complete = libusb_vlen_cb;
    pst_req->length = u32_max_size;
    ts_cpl_list_del_init((ts_cpl_list_head_s_t *)&pst_req->list);

    usbd_ep_queue(LIBUSB_EP_CMD, pst_req);
    ts_cpl_down((ts_cpl_semphr_s_t *)&sst_ctx.st_vlen_sema);

    return pst_req->actual;
}


static ts_void libusb_read_viu_cb(usbd_edpt_s_t *edpt, usb_req_s_t *req)
{
    ts_cpl_up((ts_cpl_semphr_s_t *)&sst_ctx.st_viu_sema);
}


ts_s32 f_hci_read_viu(ts_u08 *pu08_buf, ts_u32 u32_size, ts_u32 u32_tm_out)
{
    usb_req_s_t *req;
    ts_s32 s32_ret;

    req = &sst_ctx.st_viu_req;

    req->buf = (ts_void *)pu08_buf;
    req->dma_addr = (ts_u32)pu08_buf;
    req->dma_sync = 1;
    req->length = u32_size;
    req->actual = 0;
    req->complete = libusb_read_viu_cb;
    usbd_ep_queue(LIBUSB_EP_VIU, req);
    s32_ret = ts_cpl_down_timeout((ts_cpl_semphr_s_t *)&sst_ctx.st_viu_sema, u32_tm_out);
    if (0 == s32_ret) {
        return (ts_s32)req->actual;
    } else {
        return -1;
    }
}

#if (0 == HCI_CMD_QUERY)
static ts_void libusb_read_pkg_cb(usbd_edpt_s_t *edpt, usb_req_s_t *req)
{
    if (sst_ctx.u32_up_en > 0)
        ts_cpl_up((ts_cpl_semphr_s_t *)&sst_ctx.st_cmd_sema);
}
#endif

ts_void f_hci_read_init(ts_u08 *pu08_buf, ts_u32 u32_max_size)
{
    ts_u32 i;
    usb_req_s_t *req;
    ts_u08 *pu08_ptr;

    /* 可重入初始化，用于多次枚举 */
    if (u32_max_size >= HCI_CMD_LEN) {
        pu08_ptr = pu08_buf;
        for (i = 0; i < HCI_CFG_CMDBUF_NUM; i++) {
            req = &sst_ctx.ast_cmd_req[i];

            req->buf = pu08_ptr;
            req->dma_addr = (ts_dma_addr_t)pu08_ptr;
            req->length = u32_max_size;
            req->actual = 0;
            req->dma_sync = 1;

            #if (1 == HCI_CMD_QUERY)
                req->complete = ts_null;
            #else
                req->complete = libusb_read_pkg_cb;
            #endif

            pu08_ptr += u32_max_size;
            
        }

        sst_ctx.u32_max_size = u32_max_size;
        sst_ctx.u32_buf_id = 0;

        #if (1 == HCI_CMD_QUERY)
            /* 挂上 req 等待接收 */
            req = &sst_ctx.ast_cmd_req[0];
            ts_cpl_memset32((ts_void *)req->buf, 0, 8);
            usbd_ep_queue(LIBUSB_EP_CMD, req);
        #endif
    }
}



ts_s32 vot_f_hci_read_pkg(ts_u08 **ppu08_buf, ts_u32 u32_size, ts_u32 u32_tm_out)
{
    usb_req_s_t *req;
    // ts_u32 *pu32_cmd;
    ts_u32 u32_rcnt, u32_pre_cnt, u32_pre_tmout;
    // ts_u32 u32_cmd_done;

    req = &sst_ctx.ast_cmd_req[0];
    req->buf = (ts_void *)req->dma_addr;

    /* 第一次接收 HCI_CMD_LEN 个长度命令 */
    u32_rcnt = 0;
    // u32_cmd_done = 0;
    u32_pre_cnt = 512;
    u32_pre_tmout = 0xffffffff;
    while (1) {
        ts_cpl_memset32((ts_void *)req->buf, 0, 8);
        req->length = u32_pre_cnt;
        req->actual = 0;
        sst_ctx.u32_up_en = 1;
        usbd_ep_queue(LIBUSB_EP_CMD, req);

        /* 第一个命令一直等待 */
        ts_cpl_down_timeout((ts_cpl_semphr_s_t *)&sst_ctx.st_cmd_sema, (ts_long)u32_pre_tmout);
        sst_ctx.u32_up_en = 0;

        /* 是否断线 */
        if (0 == sst_ctx.u32_ready)
            return -1;

        if (req->actual > 0) {
            u32_rcnt = req->actual;
            *ppu08_buf = (ts_u08 *)req->dma_addr;
            vision_log_debug("u32_rcnt %d \n", u32_rcnt);
            return u32_rcnt;
        }
    }
}


ts_s32 f_hci_read_pkg(ts_u08 **ppu08_buf, ts_u32 u32_size, ts_u32 u32_tm_out)
{
    usb_req_s_t *req;
    ts_u32 *pu32_cmd;
    ts_u32 u32_done, u32_rcnt, u32_pre_cnt, u32_pre_tmout;
    ts_u32 u32_cmd_done;

    req = &sst_ctx.ast_cmd_req[0];
    req->buf = (ts_void *)req->dma_addr;

    /* 第一次接收 HCI_CMD_LEN 个长度命令 */
    u32_rcnt = 0;
    u32_cmd_done = 0;
    u32_pre_cnt = HCI_CMD_LEN;
    u32_pre_tmout = 0xffffffff;
    while (1) {
        ts_cpl_memset32((ts_void *)req->buf, 0, 8);
        req->length = u32_pre_cnt;
        req->actual = 0;
        sst_ctx.u32_up_en = 1;
        usbd_ep_queue(LIBUSB_EP_CMD, req);

        /* 第一个命令一直等待 */
        ts_cpl_down_timeout((ts_cpl_semphr_s_t *)&sst_ctx.st_cmd_sema, (ts_long)u32_pre_tmout);
        sst_ctx.u32_up_en = 0;

        /* 是否断线 */
        if (0 == sst_ctx.u32_ready)
            return -1;

        u32_done = 0;
        if (0 == u32_cmd_done) {
            if (HCI_CMD_LEN == req->actual) {
                pu32_cmd = (ts_u32 *)req->buf;
                if (HCI_CMD_MAGIC == pu32_cmd[0]) {
                    if (pu32_cmd[1] != HCI_CMD_USBW) {
                        u32_done = 1;
                    } else {
                        if (HCI_USBW_SINGLE == pu32_cmd[2])
                            u32_done = 1;
                        else {
                            req->buf = (ts_void *)((ts_u32)req->dma_addr + HCI_CMD_LEN);
                            u32_pre_tmout = u32_tm_out;
                            if (HCI_USBW_CONTINUE == pu32_cmd[2])
                                u32_pre_cnt = pu32_cmd[3] * 4;
                            else
                                u32_pre_cnt = pu32_cmd[3] * 8;
                        }
                    }

                    u32_cmd_done = 1;
                    u32_rcnt = HCI_CMD_LEN;
                }
            }
        } else {
            u32_done = 1;
        }

        if (u32_done > 0) {
            u32_rcnt += req->actual;
            *ppu08_buf = (ts_u08 *)req->dma_addr;

            return u32_rcnt;
        }
    }
}


#define HCI_SUPPORT_CMD_CB
static ts_void libusb_send_pkg_cb(usbd_edpt_s_t *edpt, usb_req_s_t *req)
{
#ifndef HCI_SUPPORT_CMD_CB
    free(req->buf);
#else
    ts_cpl_up((ts_cpl_semphr_s_t *)&sst_ctx.st_resp_sema);
#endif
}


/* 用户传入可能是局部内存，需拷贝一次,不建议使用 */
int f_hci_send_resp_pkg(ts_u08 *pu08_buf, ts_u32 u32_size, ts_u32 u32_weak_mem)
{
#ifndef HCI_SUPPORT_CMD_CB
    ts_u08 *pu08_mbuf;

    if (u32_weak_mem > 0) {
        /* 此处为 SRAM 申请，不能申请过大，如果大内存申请，请外部自行使用 tmm 申请后再使用本函数 */
        pu08_mbuf = (ts_u08 *)malloc(u32_size);
        if (ts_null == pu08_mbuf) {
            printf("%s\n", TS_ENOMEM_STR);

            return  -TS_ENOMEM;
        }

        ts_cpl_memcpy((ts_void *)pu08_mbuf, (ts_void *)pu08_buf, u32_size);
        usbd_ep_start_write(LIBUSB_EP_RESPONSE, pu08_mbuf, u32_size, libusb_send_pkg_cb);
    } else {
        usbd_ep_start_write(LIBUSB_EP_RESPONSE, pu08_buf, u32_size, ts_null);
    }
#else
    usbd_ep_start_write(LIBUSB_EP_RESPONSE, pu08_buf, u32_size, libusb_send_pkg_cb);
    ts_cpl_down((ts_cpl_semphr_s_t *)&sst_ctx.st_resp_sema);
#endif

    return u32_size;
}


static ts_void libusb_video_cb(usbd_edpt_s_t *edpt, usb_req_s_t *req)
{
#if 1
    tdl_isp_upload_put_free_node(TS_ISP_MDEV0, (ts_void *)req, TS_ISP_UPL_PRIMARY_CH);

    /* 如果有帧头传输时，需要将地址和大小修正回去 */
    if (sst_ctx.u32_mode > 0) {
        req->length -= (sizeof(hci_frame_head_s_t) << 1);
        req->buf = (ts_void *)req->dma_addr;
    }
#else
    ts_cpl_printk("treq done\n");
#endif
}


void f_hci_test_req(ts_u08 *pu08_buf, ts_u32 u32_size)
{
    usb_req_s_t *req;

    req = &sst_ctx.ast_video_req[0];

    req->buf = (ts_void *)pu08_buf;
    req->dma_addr = (ts_u32)pu08_buf;
    req->dma_sync = 1;
    req->length = u32_size;
    req->actual = 0;
    req->complete = libusb_video_cb;

    usbd_ep_queue(LIBUSB_EP_STREAM, req);
    //ts_cpl_down((ts_cpl_semphr_s_t *)&sst_ctx.st_mode_sema);
}


static void hci_req_ready_cb(ts_void *pv_node)
{
    usb_req_s_t *req;
    hci_frame_head_s_t *pst_info;
    ts_u08 *pu08_ptr;

    if (0 == sst_ctx.u32_ready) {
        /* FIXME: 是否需要关闭 upl??? */
        tdl_isp_upload_put_free_node(TS_ISP_MDEV0, (ts_void *)pv_node, TS_ISP_UPL_PRIMARY_CH);
    } else {
        req = (usb_req_s_t *)pv_node;
        if (req != ts_null) {
            if (sst_ctx.u32_mode > 0) {
                /* 防破帧模式，在数据流前后个增加 16 个字节的标志,  dma_addr 未做 dma 映射，此处简单根据体系结构处理 */
                pu08_ptr = (ts_u08 *)(req->dma_addr | 0x10000000);

                //ts_cpl_printk("g0:%p,s:%d\n", req->buf, req->length);

                /* 帧头 */
                pst_info = (hci_frame_head_s_t *)(pu08_ptr - sizeof(hci_frame_head_s_t));
                pst_info->u32_flag = HCI_FRAME_START;
                pst_info->u32_flag_n = ~HCI_FRAME_START;
                pst_info->u32_fno = sst_ctx.u32_frm_cnt;
                pst_info->u32_rev = 0;

                /* 帧尾, length 必须保证 4 字节对齐 */
                pst_info = (hci_frame_head_s_t *)(pu08_ptr + req->length);
                pst_info->u32_flag = HCI_FRAME_END;
                pst_info->u32_flag_n = ~HCI_FRAME_END;
                pst_info->u32_fno = sst_ctx.u32_frm_cnt;
                pst_info->u32_rev = 0;

                sst_ctx.u32_frm_cnt++;

                /* 修正传输地址和传输大小 */
                req->length += (sizeof(hci_frame_head_s_t) << 1);
                req->buf = (ts_void *)(req->dma_addr - sizeof(hci_frame_head_s_t));

                //ts_cpl_printk("g1:%p,s:%d\n", req->buf, req->length);
            }

            usbd_ep_queue(LIBUSB_EP_STREAM, req);
        }
    }
}



static void f_hci_req_unbind(void)
{
    int i;
    libusb_ctx_s_t *hci = (libusb_ctx_s_t *)&sst_ctx;
    struct ts_cpl_list_head_s *list;

    /* 全部从挂载点上清除 */
    for (i = 0; i < HCI_CFG_INREQ_NUM; i++) {
        list = (struct ts_cpl_list_head_s *)&hci->ast_video_req[i].list;
        if (0 == ts_cpl_list_empty(list))
            ts_cpl_list_del_init(list);
    }
}


/* 将所有内置 req 挂载到指定设备，设备支持 VIU(nuc),ISP(upload),VPO(bus) */
static void f_hci_req_bind(ts_mdev_t h_dev)
{
    int i, ret;
    libusb_ctx_s_t *hci = (libusb_ctx_s_t *)&sst_ctx;
    struct ts_cpl_list_head_s *list;
    usb_req_s_t *req;

    /* 全部从挂载点上清除 */
    for (i = 0; i < HCI_CFG_INREQ_NUM; i++) {
        req = &hci->ast_video_req[i];
        list = (struct ts_cpl_list_head_s *)&req->list;
        if (0 == ts_cpl_list_empty(list))
            ts_cpl_list_del_init(list);

        /* 重置地址，dma_addr 保留了原始地址，避免带帧头模式下， buf 地址被修改 */
        req->buf = (ts_void *)req->dma_addr;
        tdl_isp_upload_put_free_node(TS_ISP_MDEV0, (ts_void *)req, TS_ISP_UPL_PRIMARY_CH);
    }

    ts_cpl_init_list_head((ts_cpl_list_head_s_t *)&hci->st_rdy_head);

    if (hci->st_mode_sema.pv_semphr != ts_null) {
        /* 确保所有 up 完成 down */
        for (i = 0; i < HCI_CFG_INREQ_NUM; i++) {
            ret = ts_cpl_down_timeout((ts_cpl_semphr_s_t *)&hci->st_mode_sema, 100);
            //ts_cpl_printk("r:%d\n", ret);
            if (ret != 0)
                break;
        }
    }

    tdl_isp_upload_set_cb(TS_ISP_MDEV0, TS_ISP_UPL_PRIMARY_CH, hci_req_ready_cb);
}

void f_hci_set_status(tdk_tuv_status_en_t en_status)
{
    if (TDK_TUV_STATUS_FREEZE == en_status)
        tdl_isp_upload_set_p_freeze(TS_ISP_MDEV0, 1);
    else
        tdl_isp_upload_set_p_resume(TS_ISP_MDEV0);
}

int f_hci_stop_video(ts_mdev_t h_dev, ts_u32 u32_sync_en)
{
    libusb_ctx_s_t *hci = (libusb_ctx_s_t *)&sst_ctx;
    ts_u32 i;

    if (TDK_TUV_STATUS_RUN == hci->en_status) {
        tdl_isp_stop_upload(TS_ISP_MDEV0, TS_ISP_UPL_PRIMARY_CH);

        /* 仿真模式下，多发几次 exsync, 使得后级可以有机会完成停止 */
        if (u32_sync_en > 0) {
            for (i = 0; i < 4; i++) {
                ts_tdk_viu_exvsync(TS_ISP_MDEV0, 1);
                ts_cpl_msleep(100);
            }
        }

        /* release all req */
        usbd_dequeue(LIBUSB_EP_STREAM, ts_null);

        /* reclainm all req */
        f_hci_req_unbind();

        hci->en_status = TDK_TUV_STATUS_IDLE;

        return TS_OK;
    } else {
        return TS_ERR;
    }
}

int f_hci_start_video(ts_mdev_t h_dev, ts_u32 u32_sub, ts_u32 u32_mode)
{
    libusb_ctx_s_t *hci = (libusb_ctx_s_t *)&sst_ctx;
    ts_isp_upload_attr_s_t st_attr;
    int ret;

    if (hci->en_status != TDK_TUV_STATUS_IDLE)
        return -1;

    hci->u32_mode = u32_mode;
    f_hci_req_bind(h_dev);

    st_attr.u32_module = u32_sub;
    st_attr.u32_chn = TS_ISP_UPL_PRIMARY_CH;
    st_attr.s32_frame_num = 0;
    st_attr.u32_tmout_ms = 500;

    ret = tdl_isp_run_upload(TS_ISP_MDEV0, (ts_isp_upload_attr_s_t *)&st_attr);
    if (0 == ret) {
        hci->en_status = TDK_TUV_STATUS_RUN;
        hci->u32_frm_cnt = 0;
    }

    return ret;
}

ts_void usb_libusb_init(ts_void)
{
    usbd_initialize(8, usbd_event_handler);
}


#if 0
ts_void usb_usr_init(ts_void *pv_h_tmm)
#else
int f_hci_init(const char *name, ts_cpl_semphr_s_t *init_done_sem, ts_void *pv_tmm)
#endif
{
    ts_u32 i, u32_frm_size, u32_mem_size, u32_addr, u32_fix_addr, u32_viu_size;
    usb_req_s_t *pst_req;
    ts_viu_comm_attr_s_t st_viu_attr;

    if (0 == sst_ctx.u32_inited) {
        sst_ctx.u32_inited = 1;
        sst_ctx.pv_tmm = pv_tmm;
        sst_ctx.u32_ready = 0;
        sst_ctx.u32_ack_index = 0;
        sst_ctx.u32_start = 0;

        sst_ctx.pst_ready_sema = init_done_sem;
        ts_cpl_semphr_init((ts_cpl_semphr_s_t *)&sst_ctx.st_resp_sema, 0);
        ts_cpl_semphr_init((ts_cpl_semphr_s_t *)&sst_ctx.st_cmd_sema, 0);
        ts_cpl_semphr_init((ts_cpl_semphr_s_t *)&sst_ctx.st_vlen_sema, 0);
        ts_cpl_semphr_init((ts_cpl_semphr_s_t *)&sst_ctx.st_mode_sema, 0);
        ts_cpl_semphr_init((ts_cpl_semphr_s_t *)&sst_ctx.st_viu_sema, 0);

        ts_cpl_spinlock_init((ts_cpl_spinlock_s_t *)&sst_ctx.st_lock);
        ts_cpl_mutex_init((ts_cpl_mutex_s_t *)&sst_ctx.st_cmd_mutex);

        usbd_desc_register(&winusb_descriptor);
        usbd_add_interface(&intf0);

        usb_libusb_init();

        /* 从 vpo 获取帧大小，此处需保证 vpo 帧大小 >= isp 帧大小 */
        tdl_mupl_get_frm_size((ts_u32 *)&u32_frm_size);
        ts_tdk_viu_get_comm_attr(TS_VIU_MDEV0, (ts_viu_comm_attr_s_t *)&st_viu_attr);

        /* 按照 y16 设置 */
        u32_viu_size = st_viu_attr.u16_height * st_viu_attr.u16_width * 2;

        /* vpo 和 isp 取最大配置 */
        if (u32_viu_size > u32_frm_size)
            u32_frm_size = u32_viu_size;

        /* 对获取的帧大小进行合法性检测，可根据实际情况修改, 此处不能小于 256x128x2 且不能大于 1280 * 2 *1024*2, 修改成 640x(512+4)x2 */
        //if ((u32_frm_size < 0x2000) || (u32_frm_size > 0x510000)) {
        if ((u32_frm_size < 0x10000) || (u32_frm_size > 0x290000)) {
            vision_log_err("%s: frm size=%d\n", TS_EINVAL_STR, u32_frm_size);

            u32_frm_size = HCI_CFG_BUF_SIZE;
        }

        /* 每帧需头尾需加入额外信息 */
        u32_frm_size += 128;
        u32_mem_size = u32_frm_size * HCI_CFG_INREQ_NUM;

        vision_log_debug("frm_size=%d\n", u32_frm_size);

        /* 申请 4 个视频缓冲 */
    #if 1
        sst_ctx.u32_mem_addr = (ts_u32)ts_cpl_tmm_malloc(pv_tmm, u32_mem_size, 32, "hci-mem");
        // if(0 == sst_ctx.u32_mem_addr) {
        //     vision_log_warn("try all zone\n");
        //     sst_ctx.u32_mem_addr = (ts_u32)ts_cpl_tmm_malloc(ts_null, u32_mem_size, 32, "hci-mem");
        // }

    #else
        sst_ctx.u32_mem_addr = 0x22000000;
    #endif

        if (0 == sst_ctx.u32_mem_addr) {
            vision_log_err("%s: memaddr=%x, task down\n", TS_ENOMEM_STR, sst_ctx.u32_mem_addr);

            while (1) {
                ts_cpl_msleep(1000);
            }
        }

        u32_addr = sst_ctx.u32_mem_addr;
        for (i = 0; i < HCI_CFG_INREQ_NUM; i++) {
            pst_req = &sst_ctx.ast_video_req[i];

            /* 空出标志空间 */
            u32_fix_addr = u32_addr + 0x40;
            pst_req->buf = (ts_void *)u32_fix_addr;
            pst_req->dma_addr = u32_fix_addr;
            pst_req->dma_sync = 0;
            pst_req->complete = libusb_video_cb;

            u32_addr += u32_frm_size;
        }
    }

    return 0;
}


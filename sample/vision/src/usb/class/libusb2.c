/*
 * libusb.c -- tdk libusb 两个 interface 使用例子
 */
#include <base/ts_compiler.h>
#include <uapi/usb/usbd_core.h>
#include <uapi/upl_v100.h>
#include <uapi/viu_v100.h>
#include <stdio.h>
#include <stdlib.h>
#include "api.h"
#include "f_tdk.h"


/* 图像业务接口 IR */

/* 用于图像传输 */
#define IR_IEP_STREAM           (0x81)

/* 命令接收 */
#define IR_OEP_CMD              (0x02)

/* 命令回应 */
#define IR_IEP_ACK              (0x83)


/* 苹果认证 MFI 或者安卓下传图 */
#define MFI_IEP_STREAM          (0x85)

/* 命令接收 */
#define MFI_OEP_CMD             (0x04)

/* 命令回应 */
#define MFI_IEP_ACK             (0x87)

/* 每个接口 3 个 EP */
#define LIBUSB_EP_NUM           (3)

#define USBD_VID                (0x5474)
#define USBD_PID                (0x5430)
#define USBD_MAX_POWER          (100)
#define USBD_LANGID_STRING      (1033)

#define USB_CONFIG_SIZE         (9 + 9 + 7 + 7 + 7 + 9 + 7 + 7 + 7 + 9)

#define INTF_NUM                (3)

/* USB_ENDPOINT_TYPE_BULK 类型部分主机不支持 1024 */
#define LIBUSB_EP_MPS           512


static const ts_u08 device_descriptor[] = {
    USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0x00, 0x00, 0x00, USBD_VID, USBD_PID, 0x0200, 0x01)
};

static const ts_u08 config_descriptor[] = {
    USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, INTF_NUM, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),

    /* 第一个接口 for IR */
    USB_INTERFACE_DESCRIPTOR_INIT(0x00, 0x00, LIBUSB_EP_NUM, 0xff, 0x00, 0x00, 0x04),
    USB_ENDPOINT_DESCRIPTOR_INIT(IR_IEP_STREAM, USB_ENDPOINT_TYPE_BULK, LIBUSB_EP_MPS, 0x00),
    USB_ENDPOINT_DESCRIPTOR_INIT(IR_OEP_CMD, USB_ENDPOINT_TYPE_BULK, LIBUSB_EP_MPS, 0x00),
    USB_ENDPOINT_DESCRIPTOR_INIT(IR_IEP_ACK, USB_ENDPOINT_TYPE_BULK, LIBUSB_EP_MPS, 0x00),

    /* 第二接口 for MFI or android */
    USB_INTERFACE_DESCRIPTOR_INIT(0x01, 0x00, LIBUSB_EP_NUM, 0xff, 0x00, 0x00, 0x05),
    USB_ENDPOINT_DESCRIPTOR_INIT(MFI_IEP_STREAM, USB_ENDPOINT_TYPE_BULK, LIBUSB_EP_MPS, 0x00),
    USB_ENDPOINT_DESCRIPTOR_INIT(MFI_OEP_CMD, USB_ENDPOINT_TYPE_BULK, LIBUSB_EP_MPS, 0x00),
    USB_ENDPOINT_DESCRIPTOR_INIT(MFI_IEP_ACK, USB_ENDPOINT_TYPE_BULK, LIBUSB_EP_MPS, 0x00),

    /* 空接口 */
    USB_INTERFACE_DESCRIPTOR_INIT(0x02, 0x00, 0, 0xff, 0x00, 0x00, 0x06),
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

#define LIBUSB_MAX_STRING_INDEX     (7)
static const char *string_descriptors[] = {
    (const char[]){ 0x09, 0x04 }, /* Langid */
    "top-silicon",              /* Manufacturer */
    "ir_mfi_demo",              /* Product */
    "2022123456",               /* Serial Number */
    "libusb ir",                /* STRING4 */
    "libusb mfi",               /* STRING5 */
    "libusb null",              /* STRING6 */
};


#define IR_MAX_REQ_NUM              (4)
#define IR_EXTRA_INFO_SIZE          (128)

#define IR_CMC_SIZE                 (1024)

typedef struct ir_ctx_s {
    /* 记录初始化状态 */
    ts_u32 u32_inited;

    /* usb 是否已配置可以使用,以及相应信号量 */
    ts_u32 u32_usb_rdy;
    ts_cpl_semphr_s_t st_rdy_sema;

    /* 保护上下文自旋锁 */
    ts_cpl_spinlock_s_t st_lock;

    /* -- 以下传图上下文，可以分配给 IR 或者 MFI_ANDROID 传图端点 -- */

    /* req 数组，用于流传输 */
    usb_req_s_t ast_video_req[IR_MAX_REQ_NUM];

    /* 码流模式 */
    ts_u32 u32_mode;

    /* 指定出流端点 */
    ts_u08 u08_ep_stream;
    ts_u08 u08_rev;

    ts_u16 u16_extra_size;

    /* 保存帧存首地址 */
    ts_void *pv_fb_addr;

    /* 保存当前传输状态 */
    tdk_tuv_status_en_t en_video_status;

    /* 命令和接收 req */
    usb_req_s_t st_ir_cmd_req;
    usb_req_s_t st_ir_ack_req;
    usb_req_s_t st_mfi_cmd_req;
    usb_req_s_t st_mfi_ack_req;

    /* 对应命令接收完成信号量，如果无需同步，可以不使用此信号的 down 和 up 功能 */
    ts_cpl_semphr_s_t st_ir_cmd_sema;
    ts_cpl_semphr_s_t st_mfi_cmd_sema;

    /* 对应回应发送完成信号量，可不使用 */
    ts_cpl_semphr_s_t st_ir_ack_sema;
    ts_cpl_semphr_s_t st_mfi_ack_sema;

    /* 命令与回应互斥量，如果允许多次发送，可以不使用互斥量，需要注意 */
    ts_cpl_mutex_s_t st_ir_cmd_mutex;
    ts_cpl_mutex_s_t st_ir_ack_mutex;
    ts_cpl_mutex_s_t st_mfi_cmd_mutex;
    ts_cpl_mutex_s_t st_mfi_ack_mutex;
} ir_ctx_s_t;

static struct usbd_interface intf0;
static struct usbd_interface intf1;
static ir_ctx_s_t sst_ir_ctx = {0};

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
        return "null";

    return string_descriptors[index];
}

const struct usb_descriptor libusb_ir_mfi_descriptor = {
    .device_descriptor_callback = device_descriptor_callback,
    .config_descriptor_callback = config_descriptor_callback,
    .device_quality_descriptor_callback = device_quality_descriptor_callback,
    .string_descriptor_callback = string_descriptor_callback,
};

ts_void libusb_close_ir_all(ts_void)
{
    usbd_ep_close(IR_IEP_STREAM);
    usbd_ep_close(IR_OEP_CMD);
    usbd_ep_close(IR_IEP_ACK);
}

ts_void libusb_close_mfi_all(ts_void)
{
    usbd_ep_close(MFI_IEP_STREAM);
    usbd_ep_close(MFI_OEP_CMD);
    usbd_ep_close(MFI_IEP_ACK);
}


static ts_void usbd_event_handler(ts_u08 event)
{
    switch (event) {
        case USBD_EVENT_RESET:
            sst_ir_ctx.u32_usb_rdy = 0;

            break;
        case USBD_EVENT_CONNECTED:
            break;

        case USBD_EVENT_DISCONNECTED:
            break;

        case USBD_EVENT_RESUME:
            sst_ir_ctx.u32_usb_rdy = 1;
            //ts_cpl_up((ts_cpl_semphr_s_t *)&sst_ir_ctx.st_rdy_sema);

            break;

        case USBD_EVENT_SUSPEND:
            sst_ir_ctx.u32_usb_rdy = 0;

            libusb_close_ir_all();
            libusb_close_mfi_all();

            break;

        case USBD_EVENT_CONFIGURED:
            sst_ir_ctx.u32_usb_rdy = 1;
            //ts_cpl_up((ts_cpl_semphr_s_t *)&sst_ir_ctx.st_rdy_sema);

            break;

        case USBD_EVENT_SET_REMOTE_WAKEUP:
            break;

        case USBD_EVENT_CLR_REMOTE_WAKEUP:
            break;

        default:
            break;
    }
}


ts_void lusb_ir_mfi_wait_usb_rdy(ts_void)
{
    while (1) {
        if (sst_ir_ctx.u32_usb_rdy > 0)
            break;

        ts_cpl_msleep(100);

        if (sst_ir_ctx.u32_usb_rdy > 0) {
            vision_log_debug("usb rdy!\n");

            break;
        }

        //ts_cpl_down((ts_cpl_semphr_s_t *)&sst_ir_ctx.st_rdy_sema);
        //vision_log_debug("usb rdy!\n");
    }
}


ts_u32 lusb_ir_mfi_chk_usb_rdy(ts_void)
{
    return sst_ir_ctx.u32_usb_rdy;
}


static ts_void ir_read_cb(usbd_edpt_s_t *edpt, usb_req_s_t *req)
{
    ts_cpl_up((ts_cpl_semphr_s_t *)&sst_ir_ctx.st_ir_cmd_sema);
}

static ts_void mfi_read_cb(usbd_edpt_s_t *edpt, usb_req_s_t *req)
{
    ts_cpl_up((ts_cpl_semphr_s_t *)&sst_ir_ctx.st_mfi_cmd_sema);
}


static ts_void ir_ack_cb(usbd_edpt_s_t *edpt, usb_req_s_t *req)
{
    ts_cpl_up((ts_cpl_semphr_s_t *)&sst_ir_ctx.st_ir_ack_sema);
}


ts_s32 lusb_ir_read(ts_u08 *pu08_buf, ts_u32 u32_read_size)
{
    usb_req_s_t *pst_req;

    /* 单线程可以不用锁，多线程操作需要打开 */
    //ts_cpl_mutex_lock((ts_cpl_mutex_s_t *)&sst_ir_ctx.st_ir_cmd_mutex);

    pst_req = (usb_req_s_t *)&sst_ir_ctx.st_ir_cmd_req;
    pst_req->buf = (ts_void *)pu08_buf;
    pst_req->dma_addr = (ts_dma_addr_t)pu08_buf;
    pst_req->dma_sync = 1;
    pst_req->complete = ir_read_cb;
    pst_req->length = u32_read_size;

    usbd_ep_queue(IR_OEP_CMD, pst_req);

    /* 等待完成 */
    ts_cpl_down((ts_cpl_semphr_s_t *)&sst_ir_ctx.st_ir_cmd_sema);

    //ts_cpl_mutex_unlock((ts_cpl_mutex_s_t *)&sst_ir_ctx.st_ir_cmd_mutex);

    return pst_req->actual;
}

ts_void lusb_ir_write(ts_u08 *pu08_buf, ts_u32 u32_write_size)
{
    /* 不等待完成 */
    usbd_ep_start_write(IR_IEP_ACK, (const ts_u08 *)pu08_buf, u32_write_size, ir_ack_cb);

    /* 等待完成 */
    ts_cpl_down((ts_cpl_semphr_s_t *)&sst_ir_ctx.st_ir_ack_sema);
}

ts_s32 lusb_mfi_read(ts_u08 *pu08_buf, ts_u32 u32_read_size)
{
    usb_req_s_t *pst_req;

    /* 单线程可以不用锁，多线程操作需要打开 */
    //ts_cpl_mutex_lock((ts_cpl_mutex_s_t *)&sst_ir_ctx.st_mfi_cmd_mutex);

    pst_req = (usb_req_s_t *)&sst_ir_ctx.st_mfi_cmd_req;
    pst_req->buf = (ts_void *)pu08_buf;
    pst_req->dma_addr = (ts_dma_addr_t)pu08_buf;
    pst_req->dma_sync = 1;
    pst_req->complete = mfi_read_cb;
    pst_req->length = u32_read_size;

    usbd_ep_queue(MFI_OEP_CMD, pst_req);

    /* 等待完成 */
    ts_cpl_down((ts_cpl_semphr_s_t *)&sst_ir_ctx.st_mfi_cmd_sema);

    //ts_cpl_mutex_unlock((ts_cpl_mutex_s_t *)&sst_ir_ctx.st_mfi_cmd_mutex);

    return pst_req->actual;
}


ts_void lusb_mfi_write(ts_u08 *pu08_buf, ts_u32 u32_write_size)
{
    /* 不等待完成 */
    usbd_ep_start_write(MFI_IEP_ACK, (const ts_u08 *)pu08_buf, u32_write_size, ts_null);
}

static ts_void ir_video_cb(usbd_edpt_s_t *edpt, usb_req_s_t *req)
{
    /* 还会节点 */
    tdl_isp_upload_put_free_node(TS_ISP_MDEV0, (ts_void *)req, TS_ISP_UPL_PRIMARY_CH);

    /* 如果有帧头传输时，需要将地址和大小修正回去 */
    if (sst_ir_ctx.u16_extra_size > 0) {
        req->length -= (sst_ir_ctx.u16_extra_size >> 1);
        req->buf = (ts_void *)req->dma_addr;
    }
}


static ts_void ir_req_ready_cb(ts_void *pv_node)
{
    usb_req_s_t *pst_req;

    if (0 == sst_ir_ctx.u32_usb_rdy) {
        /* FIXME: 是否需要关闭 upl??? */
        tdl_isp_upload_put_free_node(TS_ISP_MDEV0, (ts_void *)pv_node, TS_ISP_UPL_PRIMARY_CH);
    } else {
        pst_req = (usb_req_s_t *)pv_node;

        /* 如需监测破帧，请参考 libusb.c 中的做法 */
        if (pst_req != ts_null)
            usbd_ep_queue(sst_ir_ctx.u08_ep_stream, pst_req);
    }
}


static ts_void ir_req_unbind(ts_void)
{
    ts_u32 i;
    ts_cpl_list_head_s_t *pst_list;

    /* 全部从挂载点上清除 */
    for (i = 0; i < IR_MAX_REQ_NUM; i++) {
        pst_list = (ts_cpl_list_head_s_t *)&sst_ir_ctx.ast_video_req[i].list;
        if (0 == ts_cpl_list_empty(pst_list))
            ts_cpl_list_del_init(pst_list);
    }
}

static ts_void ir_req_bind(ts_void)
{
    ts_u32 i;
    ts_cpl_list_head_s_t *pst_list;
    usb_req_s_t *pst_req;

    /* 全部从挂载点上清除 */
    for (i = 0; i < IR_MAX_REQ_NUM; i++) {
        pst_req = (usb_req_s_t *)&sst_ir_ctx.ast_video_req[i];
        pst_list = (ts_cpl_list_head_s_t *)&pst_req->list;
        if (0 == ts_cpl_list_empty(pst_list))
            ts_cpl_list_del_init(pst_list);

        /* 重置地址，dma_addr 保留了原始地址，避免带帧头模式下， buf 地址被修改 */
        pst_req->buf = (ts_void *)pst_req->dma_addr;
        tdl_isp_upload_put_free_node(TS_ISP_MDEV0, (ts_void *)pst_req, TS_ISP_UPL_PRIMARY_CH);
    }

    tdl_isp_upload_set_cb(TS_ISP_MDEV0, TS_ISP_UPL_PRIMARY_CH, ir_req_ready_cb);
}



ts_s32 lusb_stop_video(ts_void)
{
    if (TDK_TUV_STATUS_RUN == sst_ir_ctx.en_video_status) {
        tdl_isp_stop_upload(TS_ISP_MDEV0, TS_ISP_UPL_PRIMARY_CH);

        /* release all req */
        usbd_dequeue(sst_ir_ctx.u08_ep_stream, ts_null);

        /* reclainm all req */
        ir_req_unbind();

        sst_ir_ctx.en_video_status = TDK_TUV_STATUS_IDLE;

        return TS_OK;
    } else {
        return TS_ERR;
    }
}


/*
u32_mode : 普通模式
u32_mode : 前后加额外数据模式
*/
ts_s32 lusb_start_video(ts_u32 u32_mode)
{
    ts_isp_upload_attr_s_t st_attr;
    ts_s32 s32_ret;

    if (sst_ir_ctx.en_video_status != TDK_TUV_STATUS_IDLE)
        return TS_ERR;

    sst_ir_ctx.u32_mode = u32_mode;

    /* 挂载队列 */
    ir_req_bind();

    st_attr.u32_module = TS_UPL_POS_VPO_YUV;
    st_attr.u32_chn = TS_ISP_UPL_PRIMARY_CH;
    st_attr.s32_frame_num = 0;
    st_attr.u32_tmout_ms = 500;

    s32_ret = tdl_isp_run_upload(TS_ISP_MDEV0, (ts_isp_upload_attr_s_t *)&st_attr);
    if (TS_OK == s32_ret)
        sst_ir_ctx.en_video_status = TDK_TUV_STATUS_RUN;

    return s32_ret;
}


ts_void lusb_init(ts_void)
{
    usbd_ep_set_mode(USBD_NORMAL_MODE);
    usbd_initialize(16, usbd_event_handler);
}


ts_s32 lusb_ir_mfi_init(ts_void *pv_htmm, ts_u32 u32_extar_size)
{
    ts_u32 i, u32_frm_size, u32_mem_size;
    ts_u32 u32_addr, u32_fix_addr;
    usb_req_s_t *pst_req;

    if (0 == sst_ir_ctx.u32_inited) {
        sst_ir_ctx.u32_usb_rdy = 0;

        if (u32_extar_size > (16 * 1024)) {
            vision_log_err("extra size is to large\n");

            return -TS_EINVAL;
        }

        /* 获取 vpo_mem 的尺寸 */
        tdl_mupl_get_frm_size((ts_u32 *)&u32_frm_size);

        /* 对获取的帧大小进行合法性检测，可根据实际情况修改, 此处不能小于 256x128x2 且不能大于 1280*1024*2, 修改成 640x(512+4)x2 */
        if ((u32_frm_size < 0x10000) || (u32_frm_size > 0x290000)) {
            vision_log_err("%s: frm size=%d\n", TS_EINVAL_STR, u32_frm_size);

            return TS_ERR;
        } else {
            vision_log_debug("ir:frm_size=%d\n", u32_frm_size);
        }

        /* 每帧需头尾需加入额外信息 */
        u32_frm_size += u32_extar_size;
        u32_mem_size = u32_frm_size * IR_MAX_REQ_NUM;

        /* 申请内存 */
        sst_ir_ctx.pv_fb_addr = ts_cpl_tmm_malloc(pv_htmm, u32_mem_size, 32, "irfb");
        if (ts_null == sst_ir_ctx.pv_fb_addr) {
            vision_log_err("%s\n", TS_ENOMEM_STR);

            return -TS_ENOMEM;
        }

        sst_ir_ctx.u16_extra_size = (ts_u16)u32_extar_size;

        /* 分配码流空间 */
        sst_ir_ctx.u08_ep_stream = IR_IEP_STREAM;
        u32_addr = (ts_u32)sst_ir_ctx.pv_fb_addr;
        for (i = 0; i < IR_MAX_REQ_NUM; i++) {
            pst_req = (usb_req_s_t *)&sst_ir_ctx.ast_video_req[i];

            /* 例子采用码流两端加头,可以按需修改 */
            u32_fix_addr = u32_addr;
            if (u32_extar_size > 0)
                u32_fix_addr += u32_extar_size >> 1;

            pst_req->buf = (ts_void *)u32_fix_addr;
            pst_req->dma_addr = u32_fix_addr;
            pst_req->dma_sync = 0;
            pst_req->complete = ir_video_cb;

            u32_addr += u32_frm_size;
        }

        /* 自旋锁初始化 */
        ts_cpl_spinlock_init((ts_cpl_spinlock_s_t *)&sst_ir_ctx.st_lock);

        /* 信号量初始化 */
        ts_cpl_semphr_init((ts_cpl_semphr_s_t *)&sst_ir_ctx.st_rdy_sema, 0);
        ts_cpl_semphr_init((ts_cpl_semphr_s_t *)&sst_ir_ctx.st_ir_cmd_sema, 0);
        ts_cpl_semphr_init((ts_cpl_semphr_s_t *)&sst_ir_ctx.st_mfi_cmd_sema, 0);
        ts_cpl_semphr_init((ts_cpl_semphr_s_t *)&sst_ir_ctx.st_ir_ack_sema, 0);
        ts_cpl_semphr_init((ts_cpl_semphr_s_t *)&sst_ir_ctx.st_mfi_ack_sema, 0);

        /* 互斥初始化 */
        ts_cpl_mutex_init((ts_cpl_mutex_s_t *)&sst_ir_ctx.st_ir_cmd_mutex);
        ts_cpl_mutex_init((ts_cpl_mutex_s_t *)&sst_ir_ctx.st_ir_ack_mutex);
        ts_cpl_mutex_init((ts_cpl_mutex_s_t *)&sst_ir_ctx.st_mfi_cmd_mutex);
        ts_cpl_mutex_init((ts_cpl_mutex_s_t *)&sst_ir_ctx.st_mfi_ack_mutex);

        /* 启动 usb 初始化 */
        usbd_desc_register(&libusb_ir_mfi_descriptor);
        usbd_add_interface(&intf0);
        usbd_add_interface(&intf1);

        //usbd_ep_set_mode(USBD_NORMAL_MODE);
        //usbd_initialize(16, usbd_event_handler);
        lusb_init();

        sst_ir_ctx.u32_inited = 1;
    }

    return TS_OK;
}


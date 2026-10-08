// /*
//  * libusb.c -- tdk libusb 使用例子
//  */
// #include <base/ts_compiler.h>
// #include <uapi/usb/usbd_core.h>
// #include <uapi/upl_v100.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include "f_tdk.h"

// #define LIBUSB_EP_STREAM        0x81
// #define LIBUSB_EP_CMD           0x02
// #define LIBUSB_EP_RESPONSE      0x83
// #define LIBUSB_EP_NUM           3

// // #define USBD_VID                0x04b4 //0x5474 
// // #define USBD_PID                0x00f0 //0x5410 
// #define USBD_MAX_POWER          100
// #define USBD_LANGID_STRING      1033

// #if (LIBUSB_EP_NUM == 3)
// #define USB_CONFIG_SIZE         (9 + 9 + 7 + 7 + 7)
// #else
// #define USB_CONFIG_SIZE         (9 + 9 + 7 + 7)
// #endif

// #define INTF_NUM                1
// #define LIBUSB_EP_MPS           512


// static const ts_u08 device_descriptor[] = {
//     USB_DEVICE_DESCRIPTOR_INIT(USB_2_0, 0xff, 0x01, 0x00, USBD_VID, USBD_PID, 0x0200, 0x01)
// };

// static const ts_u08 config_descriptor[] = {
//     USB_CONFIG_DESCRIPTOR_INIT(USB_CONFIG_SIZE, INTF_NUM, 0x01, USB_CONFIG_BUS_POWERED, USBD_MAX_POWER),
//     USB_INTERFACE_DESCRIPTOR_INIT(0x00, 0x00, LIBUSB_EP_NUM, 0xff, 0xff, 0x00, 0x04),

// #if 1
//     USB_ENDPOINT_DESCRIPTOR_INIT(LIBUSB_EP_STREAM, USB_ENDPOINT_TYPE_BULK, LIBUSB_EP_MPS, 0x00),
// #else
//     USB_ENDPOINT_DESCRIPTOR_INIT(LIBUSB_EP_STREAM, USB_ENDPOINT_TYPE_ISOCHRONOUS, 1024, 0x00),
// #endif
//     USB_ENDPOINT_DESCRIPTOR_INIT(LIBUSB_EP_CMD, USB_ENDPOINT_TYPE_INTERRUPT, LIBUSB_EP_MPS, 0x00),

// #if (LIBUSB_EP_NUM == 3)
//     USB_ENDPOINT_DESCRIPTOR_INIT(LIBUSB_EP_RESPONSE, USB_ENDPOINT_TYPE_INTERRUPT, LIBUSB_EP_MPS, 0x00),
// #endif
// };

// static const ts_u08 device_quality_descriptor[] = {
//     ///////////////////////////////////////
//     /// device qualifier descriptor
//     ///////////////////////////////////////
//     0x0a,
//     USB_DESCRIPTOR_TYPE_DEVICE_QUALIFIER,
//     0x00,
//     0x02,
//     0x00,
//     0x00,
//     0x00,
//     0x40,
//     0x00,
//     0x00,
// };

// #define LIBUSB_MAX_STRING_INDEX     (5)
// static const char *string_descriptors[] = {
//     (const char[]){ 0x09, 0x04 }, /* Langid */
//     "top-silicon",                  /* Manufacturer */
//     "host client interface",      /* Product */
//     "2022123456",                 /* Serial Number */
//     "libusb DEMO 1",            /* STRING4 */
//     "libusb DEMO 2",            /* STRING5 */
// };


// #define LIBUSB_CMD_BUF_SIZE         (8 * 1024 + 512)
// #define HCI_CFG_WIDTH               (640)
// #define HCI_CFG_HEIGHT              (512)
// #define HCI_CFG_INREQ_NUM           (4)

// /* 支持 yuv 或者 r16 格式，所有最大需要乘以 2 */
// #define HCI_CFG_BUF_SIZE            ((HCI_CFG_WIDTH * (HCI_CFG_HEIGHT + 4)) * 2)
// #define HCI_CFG_TATOL_MEM_SIZE      (HCI_CFG_BUF_SIZE * HCI_CFG_INREQ_NUM)

// typedef struct libusb_ctx_s {
//     /* 记录初始化状态 */
//     ts_u32 u32_inited;

//     /* usb 是否已配置可以使用 */
//     ts_u32 u32_ready;

//     /* 保存线程句柄 */
//     ts_cpl_task_s_t st_task;

//     /* 命令接收和回应内存 */
//     ts_void *pv_cmd_tmm;
//     ts_u08 *pu08_cmd_buf;
//     ts_u08 *pu08_resp_buf;

//     /* 命令接收 req */
//     usb_req_s_t st_cmd_req;
//     ts_cpl_mutex_s_t st_cmd_mutex;
//     ts_u32 u32_ack_index;

//     /* 视频流控制 */
//     ts_u32 u32_start;
//     ts_u32 u32_v_width;
//     ts_u32 u32_v_height;
//     ts_u32 u32_v_size;
//     ts_u32 u32_mode;
//     ts_u32 u32_mem_addr;
//     usb_req_s_t ast_video_req[HCI_CFG_INREQ_NUM];
//     tdk_tuv_status_en_t en_status;

//     /* 单次触发模式下 req 队列 */
//     ts_cpl_list_head_s_t st_rdy_head;

//     /* 单次触发下同步信号以及自旋锁 */
//     ts_cpl_semphr_s_t st_mode_sema;
//     ts_cpl_spinlock_s_t st_lock;

//     /* 配置完成信号量 */
//     ts_cpl_semphr_s_t *pst_ready_sema;

//     /* 用于接收命令的同步信息量 */
//     ts_cpl_semphr_s_t st_resp_sema;
// } libusb_ctx_s_t;

// struct usbd_interface intf0;
// static libusb_ctx_s_t sst_ctx = {0};

// /* 控制日志等级 */
// ts_s32 ss32_f_tdk_lvl;


// static const ts_u08 *device_descriptor_callback(ts_u08 speed)
// {
//     return device_descriptor;
// }

// static const ts_u08 *config_descriptor_callback(ts_u08 speed)
// {
//     return config_descriptor;
// }

// static const ts_u08 *device_quality_descriptor_callback(ts_u08 speed)
// {
//     return device_quality_descriptor;
// }

// static const char *string_descriptor_callback(ts_u08 speed, ts_u08 index)
// {
//     if (index > LIBUSB_MAX_STRING_INDEX)
//         return ts_null;

//     return string_descriptors[index];
// }

// const struct usb_descriptor winusb_descriptor = {
//     .device_descriptor_callback = device_descriptor_callback,
//     .config_descriptor_callback = config_descriptor_callback,
//     .device_quality_descriptor_callback = device_quality_descriptor_callback,
//     .string_descriptor_callback = string_descriptor_callback,
// };

// static ts_void usbd_event_handler(ts_u08 event)
// {
//     //vision_log_debug("ev=%d\n", event);

//     switch (event) {
//         case USBD_EVENT_RESET:
//             sst_ctx.u32_ready = 0;
//             break;
//         case USBD_EVENT_CONNECTED:
//             break;
//         case USBD_EVENT_DISCONNECTED:
//             break;
//         case USBD_EVENT_RESUME:
//             sst_ctx.u32_ready = 1;
//             ts_cpl_up((ts_cpl_semphr_s_t *)sst_ctx.pst_ready_sema);
//             break;
//         case USBD_EVENT_SUSPEND:
//             sst_ctx.u32_ready = 0;
//             break;
//         case USBD_EVENT_CONFIGURED:
//             sst_ctx.u32_ready = 1;
//             ts_cpl_up((ts_cpl_semphr_s_t *)sst_ctx.pst_ready_sema);
//             break;
//         case USBD_EVENT_SET_REMOTE_WAKEUP:
//             break;
//         case USBD_EVENT_CLR_REMOTE_WAKEUP:
//             break;

//         default:
//             break;
//     }
// }


// ts_u32 libusb_chk_alive(ts_void)
// {
//     return sst_ctx.u32_ready;
// }

// ts_void libusb_close_all(ts_void)
// {
//     usbd_ep_close(LIBUSB_EP_STREAM);
//     usbd_ep_close(LIBUSB_EP_CMD);
//     usbd_ep_close(LIBUSB_EP_RESPONSE);
// }


// #if 0
// static ts_void libusb_send_pkg_cb(usbd_edpt_s_t *edpt, usb_req_s_t *req)
// {
//     ts_cpl_up((ts_cpl_semphr_s_t *)&sst_ctx.st_resp_sema);
// }


// static ts_s32 libusb_send_resp_pkg(ts_u08 *pu08_buf, ts_u32 u32_size, ts_u32 u32_tm_out)
// {
//     if (u32_tm_out > 0) {
//         usbd_ep_start_write(LIBUSB_EP_RESPONSE, pu08_buf, u32_size, libusb_send_pkg_cb);
//         ts_cpl_down_timeout((ts_cpl_semphr_s_t *)&sst_ctx.st_resp_sema, u32_tm_out);
//     } else {
//         usbd_ep_start_write(LIBUSB_EP_RESPONSE, pu08_buf, u32_size, ts_null);
//     }

//     return TS_OK;
// }
// #endif

// ts_s32 f_hci_read_pkg(ts_u08 *pu08_buf, ts_u32 u32_size, ts_u32 u32_tm_out)
// {
//     ts_u32 pre_cnt, rd_size, u32_chk_size;
//     usb_req_s_t *req;
//     ts_u32 u32_dly, u32_done;
//     ts_u32 *pu32_cmd;

//     //vision_log_debug("read in..\n");

//     /* 互斥，同时进允许一个读等待 */
//     ts_cpl_mutex_lock((ts_cpl_mutex_s_t *)&sst_ctx.st_cmd_mutex);

//     req = &sst_ctx.st_cmd_req;
//     pre_cnt = 0;
//     rd_size = 0;
//     u32_dly = 0;
//     u32_chk_size = 0;

//     req->length = u32_size;
//     req->actual = 0;
//     req->buf = (ts_void *)pu08_buf;
//     req->dma_addr = (ts_dma_addr_t)pu08_buf;
//     req->dma_sync = 1;
//     req->complete = ts_null;
//     usbd_ep_queue(LIBUSB_EP_CMD, req);
//     while (1) {
//         /* 失去连接 */
//         if (0 == sst_ctx.u32_ready)
//             return -1;

//         ts_cpl_msleep(10);
//         u32_dly += 10;

//         if (req->actual >= u32_size) {
//             rd_size = req->actual;
//             break;
//         }

//         pre_cnt = usb_ep_chk(LIBUSB_EP_CMD, req);
//         if (rd_size > 0) {
//             pu32_cmd = (ts_u32 *)pu08_buf;
//             u32_done = 0;
//             if ((rd_size >= HCI_CMD_LEN) && (HCI_CMD_MAGIC == pu32_cmd[0])) {
//                 if (pu32_cmd[1] != HCI_CMD_USBW) {
//                     u32_done = 1;
//                 } else {
//                     if (HCI_USBW_SINGLE == pu32_cmd[2])
//                         u32_done = 1;
//                     else {
//                         if (0 == u32_chk_size) {
//                             u32_chk_size = HCI_CMD_LEN;
//                             if (HCI_USBW_CONTINUE == pu32_cmd[2])
//                                 u32_chk_size += pu32_cmd[3] * 4;
//                             else
//                                 u32_chk_size += pu32_cmd[3] * 8;
//                         }
                        
//                     }
//                 }
//             }

//             if ((u32_chk_size > 0) && (rd_size >= u32_chk_size)) {
//                 u32_done = 1;
//             } else {
//                 if (rd_size >= pre_cnt) {
//                     if (u32_dly >= u32_tm_out) {
//                         u32_done = 1;
//                     }
//                 } else {
//                     rd_size = pre_cnt;
//                     u32_dly = 0;
//                 }
//             }

//             if (u32_done > 0) {
//                 /* 当前接收到了数据，超时退出，结束当前 req */
//                 usb_ep_done(LIBUSB_EP_CMD, req);
//                 break;
//             }
//         } else {
//             /* 没任何数据收到，就一直等 */
//             rd_size = pre_cnt;
//             u32_dly = 0;
//         }

//     }

//     ts_cpl_mutex_unlock((ts_cpl_mutex_s_t *)&sst_ctx.st_cmd_mutex);
//     return (ts_s32)req->actual;
// }

// static ts_void libusb_send_pkg_cb(usbd_edpt_s_t *edpt, usb_req_s_t *req)
// {
//     //ts_cpl_up((ts_cpl_semphr_s_t *)&sst_ctx.st_resp_sema);
//     free(req->buf);
// }


// /* 用户传入可能是局部内存，需拷贝一次,不建议使用 */
// int f_hci_send_resp_pkg(ts_u08 *pu08_buf, ts_u32 u32_size, ts_u32 u32_weak_mem)
// {
//     ts_u08 *pu08_mbuf;

//     if (u32_weak_mem > 0) {
//         /* 此处为 SRAM 申请，不能申请过大，如果大内存申请，请外部自行使用 tmm 申请后再使用本函数 */
//         pu08_mbuf = (ts_u08 *)malloc(u32_size);
//         if (ts_null == pu08_mbuf) {
//             vision_log_debug("%s\n", TS_ENOMEM_STR);

//             return  -TS_ENOMEM;
//         }

//         ts_cpl_memcpy((ts_void *)pu08_mbuf, (ts_void *)pu08_buf, u32_size);
//         usbd_ep_start_write(LIBUSB_EP_RESPONSE, pu08_mbuf, u32_size, libusb_send_pkg_cb);
//     } else {
//         usbd_ep_start_write(LIBUSB_EP_RESPONSE, pu08_buf, u32_size, ts_null);
//     }

//     return u32_size;
// }


// static ts_void libusb_video_cb(usbd_edpt_s_t *edpt, usb_req_s_t *req)
// {
//     tdl_isp_upload_put_free_node(TS_ISP_MDEV0, (ts_void *)req, TS_ISP_UPL_PRIMARY_CH);
//     //vision_log_debug("up\n");
//     //ts_cpl_up((ts_cpl_semphr_s_t *)&sst_ctx.st_mode_sema);
// }



// void f_hci_test_req(ts_u08 *pu08_buf, ts_u32 u32_size)
// {
//     usb_req_s_t *req;

//     req = &sst_ctx.ast_video_req[0];

//     req->buf = (ts_void *)pu08_buf;
//     req->dma_addr = (ts_u32)pu08_buf;
//     req->dma_sync = 1;
//     req->length = u32_size;
//     req->actual = 0;
//     req->complete = libusb_video_cb;

//     usbd_ep_queue(LIBUSB_EP_STREAM, req);
//     ts_cpl_down((ts_cpl_semphr_s_t *)&sst_ctx.st_mode_sema);
// }


// static void hci_req_ready_cb(ts_void *pv_node)
// {
//     libusb_ctx_s_t *hci = (libusb_ctx_s_t *)&sst_ctx;
//     usb_req_s_t *req;

//     if (0 == sst_ctx.u32_ready) {
//         /* FIXME: 是否需要关闭 upl??? */
//         tdl_isp_upload_put_free_node(TS_ISP_MDEV0, (ts_void *)pv_node, TS_ISP_UPL_PRIMARY_CH);
//     } else {
//         req = (usb_req_s_t *)pv_node;
//         if (req != ts_null) {
//             if (HCI_MODE_CONTINUE == hci->u32_mode) {
//                 usbd_ep_queue(LIBUSB_EP_STREAM, req);
//             } else {
//                 ts_cpl_list_add_tail((ts_cpl_list_head_s_t *)pv_node, (ts_cpl_list_head_s_t *)&hci->st_rdy_head);
//                 ts_cpl_up((ts_cpl_semphr_s_t *)&hci->st_mode_sema);
//             }
//         }
//     }
// }

// ts_void f_hci_put_one_fb(ts_void)
// {
//     libusb_ctx_s_t *hci = (libusb_ctx_s_t *)&sst_ctx;
//     ts_ulong ul_flags;
//     usb_req_s_t *req;

//     if (hci->u32_mode != HCI_MODE_TRIGGER)
//         return;

//     ts_cpl_down_timeout((ts_cpl_semphr_s_t *)&hci->st_mode_sema, 200);

//     ts_cpl_spin_lock_irqsave((ts_cpl_spinlock_s_t *)&hci->st_lock, &ul_flags);

//     if (0 == ts_cpl_list_empty((ts_cpl_list_head_s_t *)&hci->st_rdy_head)) {
//         req = (usb_req_s_t *)hci->st_rdy_head.next;
//         ts_cpl_list_del_init((ts_cpl_list_head_s_t *)req);
//     } else {
//         req = ts_null;
//     }

//     ts_cpl_spin_unlock_irqrestore((ts_cpl_spinlock_s_t *)&hci->st_lock, &ul_flags);

//     if (req != ts_null)
//         usbd_ep_queue(LIBUSB_EP_STREAM, req);
// }

// static void f_hci_req_unbind(void)
// {
//     int i;
//     libusb_ctx_s_t *hci = (libusb_ctx_s_t *)&sst_ctx;
//     struct ts_cpl_list_head_s *list;

//     /* 全部从挂载点上清除 */
//     for (i = 0; i < HCI_CFG_INREQ_NUM; i++) {
//         list = (struct ts_cpl_list_head_s *)&hci->ast_video_req[i].list;
//         if (0 == ts_cpl_list_empty(list))
//             ts_cpl_list_del_init(list);
//     }
// }


// /* 将所有内置 req 挂载到指定设备，设备支持 VIU(nuc),ISP(upload),VPO(bus) */
// static void f_hci_req_bind(ts_mdev_t h_dev)
// {
//     int i, ret;
//     libusb_ctx_s_t *hci = (libusb_ctx_s_t *)&sst_ctx;
//     struct ts_cpl_list_head_s *list;

//     /* 全部从挂载点上清除 */
//     for (i = 0; i < HCI_CFG_INREQ_NUM; i++) {
//         list = (struct ts_cpl_list_head_s *)&hci->ast_video_req[i].list;
//         if (0 == ts_cpl_list_empty(list))
//             ts_cpl_list_del_init(list);

//         tdl_isp_upload_put_free_node(TS_ISP_MDEV0, (ts_void *)list, TS_ISP_UPL_PRIMARY_CH);
//     }

//     ts_cpl_init_list_head((ts_cpl_list_head_s_t *)&hci->st_rdy_head);

//     if (hci->st_mode_sema.pv_semphr != ts_null) {
//         /* 确保所有 up 完成 down */
//         for (i = 0; i < HCI_CFG_INREQ_NUM; i++) {
//             ret = ts_cpl_down_timeout((ts_cpl_semphr_s_t *)&hci->st_mode_sema, 100);
//             //vision_log_debug("r:%d\n", ret);
//             if (ret != 0)
//                 break;
//         }
//     }

//     tdl_isp_upload_set_cb(TS_ISP_MDEV0, TS_ISP_UPL_PRIMARY_CH, hci_req_ready_cb);
// }

// void f_hci_set_status(tdk_tuv_status_en_t en_status)
// {
//     if (TDK_TUV_STATUS_FREEZE == en_status)
//         tdl_isp_upload_set_p_freeze(TS_ISP_MDEV0, 1);
//     else
//         tdl_isp_upload_set_p_resume(TS_ISP_MDEV0);
// }

// int f_hci_stop_video(ts_mdev_t h_dev)
// {
//     libusb_ctx_s_t *hci = (libusb_ctx_s_t *)&sst_ctx;

//     if (TDK_TUV_STATUS_RUN == hci->en_status) {
//         tdl_isp_stop_upload(TS_ISP_MDEV0, TS_ISP_UPL_PRIMARY_CH);

//         /* release all req */
//         usbd_dequeue(LIBUSB_EP_STREAM, ts_null);

//         /* reclainm all req */
//         f_hci_req_unbind();

//         hci->en_status = TDK_TUV_STATUS_IDLE;

//         return TS_OK;
//     } else {
//         return TS_ERR;
//     }
// }

// int f_hci_start_video(ts_mdev_t h_dev, ts_u32 u32_sub, ts_u32 u32_mode)
// {
//     libusb_ctx_s_t *hci = (libusb_ctx_s_t *)&sst_ctx;
//     ts_isp_upload_attr_s_t st_attr;

//     if (hci->en_status != TDK_TUV_STATUS_IDLE)
//         return -1;
//     else
//         hci->en_status = TDK_TUV_STATUS_RUN;

//     if (HCI_MODE_TRIGGER == u32_mode)
//         hci->u32_mode = HCI_MODE_TRIGGER;
//     else
//         hci->u32_mode = HCI_MODE_CONTINUE;

//     f_hci_req_bind(h_dev);

//     st_attr.u32_module = u32_sub;
//     st_attr.u32_chn = TS_ISP_UPL_PRIMARY_CH;
//     st_attr.s32_frame_num = 0;
//     st_attr.u32_tmout_ms = 500;

//     tdl_isp_run_upload(TS_ISP_MDEV0, (ts_isp_upload_attr_s_t *)&st_attr);

//     return 0;
// }


// #if 0
// ts_void usb_usr_init(ts_void *pv_h_tmm)
// #else
// int f_hci_init(const char *name, ts_cpl_semphr_s_t *init_done_sem, ts_void *pv_tmm)
// #endif
// {
//     ts_u32 i, u32_frm_size, u32_mem_size, u32_addr;
//     usb_req_s_t *pst_req;

//     if (0 == sst_ctx.u32_inited) {
//         sst_ctx.u32_inited = 1;
//         sst_ctx.u32_ready = 0;
//         sst_ctx.u32_ack_index = 0;
//         sst_ctx.u32_start = 0;

//         sst_ctx.pst_ready_sema = init_done_sem;
//         ss32_f_tdk_lvl = TS_LOG_DEBUG;

//         ts_cpl_semphr_init((ts_cpl_semphr_s_t *)&sst_ctx.st_resp_sema, 0);
//         ts_cpl_semphr_init((ts_cpl_semphr_s_t *)&sst_ctx.st_mode_sema, 0);

//         ts_cpl_spinlock_init((ts_cpl_spinlock_s_t *)&sst_ctx.st_lock);
//         ts_cpl_mutex_init((ts_cpl_mutex_s_t *)&sst_ctx.st_cmd_mutex);


//         usbd_desc_register(&winusb_descriptor);
//         usbd_add_interface(&intf0);

//         usbd_initialize(8, usbd_event_handler);

//         /* 从 vpo 获取帧大小，此处需保证 vpo 帧大小 >= isp 帧大小 */
//         tdl_mupl_get_frm_size((ts_u32 *)&u32_frm_size);

//         /* 对获取的帧大小进行合法性检测，可根据实际情况修改, 此处不能小于 256x128x2 且不能大于 1280*1024*2, 修改成 640x(512+4)x2 */
//         if ((u32_frm_size < 0x10000) && (u32_frm_size > 0x280000)) {
//             ftdk_log_err("%s: frm size=%d\n", TS_EINVAL_STR, u32_frm_size);

//             u32_frm_size = HCI_CFG_BUF_SIZE;
//         }

//         u32_mem_size = u32_frm_size * HCI_CFG_INREQ_NUM;

//         /* 申请 4 个视频缓冲 */
//         sst_ctx.u32_mem_addr = (ts_u32)ts_cpl_tmm_malloc(pv_tmm, u32_mem_size, 32, "hci-mem");
//         if (0 == sst_ctx.u32_mem_addr) {
//             ftdk_log_err("%s: memaddr=%x, task down\n", TS_ENOMEM_STR, sst_ctx.u32_mem_addr);

//             while (1) {
//                 ts_cpl_msleep(1000);
//             }
//         }

//         u32_addr = sst_ctx.u32_mem_addr;
//         for (i = 0; i < HCI_CFG_INREQ_NUM; i++) {
//             pst_req = &sst_ctx.ast_video_req[i];

//             pst_req->buf = (ts_void *)u32_addr;
//             pst_req->dma_addr = u32_addr;
//             pst_req->dma_sync = 0;
//             pst_req->complete = libusb_video_cb;

//             u32_addr += u32_frm_size;
//         }
//     }

//     return 0;
// }

/*
 * f_tdk.h -- tdk 对接 usb 通讯头文件
 */
#ifndef __F_TDK_H__
#define __F_TDK_H__

#include <base/ts_type.h>
#include <cpl/ts_cpl.h>
#include <uapi/chip_def.h>


/* 命令长度 */
#define HCI_CMD_LEN             (32)

/* 命令协议魔术字 */
#define HCI_CMD_MAGIC           (0x5af00fa5)

/* 获取视频 */
#define HCI_CMD_USBV            (0x75736276)
#define HCI_CMD_USBF            (0x75736266)
#define HCI_CMD_USBI            (0x75736249)

/* 暂停视频 */
#define HCI_CMD_USBP            (0x75736270)

/* 读内存 */
#define HCI_CMD_USBR            (0x75736272)

/* 写内存 */
#define HCI_CMD_USBW            (0x75736277)

/* ACK */
#define HCI_CMD_USBA            (0x75736261)

/* 执行命令 */
#define HCI_CMD_USBC            (0x75736243)

/*获取坏点坐标存放地址和大小 */
#define HCI_CMD_USBGBP          (0x75736279)

/*获取isp存放工具端坏点数据的内存地址，以及接收工具端下发坏点个数 */
#define HCI_CMD_USBGADDR        (0x7573627A)

/*工具通知板端坏点数据写ram完成 */
#define HCI_CMD_USBNC           (0x7573627B)

/* for write */
#define HCI_USBW_SINGLE         (0)
#define HCI_USBW_CONTINUE       (1)
#define HCI_USBW_NON_CONTINUE   (2)


/* 视频帧标志 */
#define HCI_FRAME_START         (0x5AFAF5A5)
#define HCI_FRAME_END           (0xA5E5EA5A)

typedef struct hci_frame_head_s {
    /* 头或者尾标志 */
    ts_u32 u32_flag;

    /* u32_flag 取反 */
    ts_u32 u32_flag_n;

    /* 帧计数 */
    ts_u32 u32_fno;

    /* 保留 */
    ts_u32 u32_rev;
} hci_frame_head_s_t;


/* read struct */
typedef struct tdk_tuv_read_s {
    ts_u32 u32_magic;
    ts_u32 u32_cmd;
    ts_u32 u32_addr;
    ts_u32 u32_cnt;
} tdk_tuv_read_s_t;


/* 取流状态 */
typedef enum tdk_tuv_status_en {
    /* 停止状态 */
    TDK_TUV_STATUS_IDLE = 0,

    /* 运行状态 */
    TDK_TUV_STATUS_RUN = 1,

    /* 暂停状态， 前端数据正常获取， USB 不发送流 */
    TDK_TUV_STATUS_PAUSE = 2,

    /* 冻结状态, 前端数据正常获取， USB 重复发送冻结前的最后一帧数据 */
    TDK_TUV_STATUS_FREEZE = 3,
    
    TDK_TUV_STATUS_BUTT
} tdk_tuv_status_en_t;

#define HCI_CMD_QUERY       0


int f_hci_init(const char *name, ts_cpl_semphr_s_t *init_done_sem, ts_void *pv_tmm);
int f_hci_send_resp_pkg(ts_u08 *pu08_buf, ts_u32 u32_size, ts_u32 u32_weak_mem);
ts_void f_hci_read_init(ts_u08 *pu08_buf, ts_u32 u32_max_size);
ts_s32 f_hci_read_pkg(ts_u08 **ppu08_buf, ts_u32 u32_size, ts_u32 u32_tm_out);
ts_u32 libusb_chk_alive(ts_void);
ts_void libusb_close_all(ts_void);
int f_hci_start_video(ts_mdev_t h_dev, ts_u32 u32_sub, ts_u32 u32_mode);
int f_hci_stop_video(ts_mdev_t h_dev, ts_u32 u32_sync_en);


#ifdef CONFIG_AUTO_USB
ts_void libusb_usr_init(ts_void *pv_tmm);
ts_void uvc_usr_init(ts_void *pv_h_tmm);
#else
ts_void usb_usr_init(ts_void *pv_tmm);
#endif

ts_void isp_set_run(ts_u32 u32_run_en);

#endif /* __F_TDK_H__ */


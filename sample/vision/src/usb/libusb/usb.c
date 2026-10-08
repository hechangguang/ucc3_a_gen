/*
 * usb.c -- tdk isp usb 使用例子
 */
#include <base/ts_type.h>
#include <base/ts_bitop.h>
#include <cpl/ts_cpl.h>
#include <uapi/base_v100.h>
#include <uapi/upl_v100.h>
#include <uapi/viu_v100.h>
#include <uapi/sfc_v100.h>
#include <command.h>
#include <stdio.h>
#include "api.h"
#include "f_tdk.h"
#include <api.h>
#include "snr_comm.h"
#include "sffs_files.h"
#include <menu/votmain.h>
#include <menu/votmenu.h>

extern vot_hbp_ctx_s_t vot_uart0_ctx;
extern SysCtrlInfo_s_t global_sys_ctrl_info;
extern struct Menu2_Para SystemParaMenu2;

extern void ring_buffer_init(ring_buffer_t *rb, ts_char* buf_name) ;
static ring_buffer_t m_libusb_ring_buf_instance;

ts_s32 vot_f_hci_read_pkg(ts_u08 **ppu08_buf, ts_u32 u32_size, ts_u32 u32_tm_out);

int f_hci_init_send_pkg(ts_u32 *pu32_buf, ts_u32 req_idx);
int f_hci_send_pkg(ts_u08 *pu08_buf, ts_u32 u32_size, ts_u32 req_idx);
int f_hci_send_req(ts_u32 req_idx, ts_u32 u32_size);
int f_hci_req_bind(ts_mdev_t h_dev);
ts_void f_hci_put_one_fb(ts_void);
ts_s32 f_hci_read_viu(ts_u08 *pu08_buf, ts_u32 u32_size, ts_u32 u32_tm_out);

void f_hci_set_status(tdk_tuv_status_en_t en_status);

#define USB_CMD_BUF_SIZE    (17 * 1024)
#define USB_ACK_BUF_SIZE    (5 * 1024)
#define USB_RD_BUF_SIZE     (17 * 1024)

#define USB_RW_MAX_LEN      (USB_CMD_BUF_SIZE * 2 + USB_ACK_BUF_SIZE + USB_RD_BUF_SIZE)

typedef struct hci_ctx_s {
    /* 初始化状态 */
    ts_u32 u32_inited;

    /* 任务句柄 */
    ts_cpl_task_s_t st_task;

    /* 总内存句柄 */
    ts_u32 u32_mem_addr;

    /* 命令接收缓冲, 双缓冲 */
    ts_u08 *pu08_cmd_buf;

    /* 命令 ack 回应缓冲，内部做简单 ringbuf */
    ts_u08 *pu08_ack_buf;
    ts_u32 u32_ack_inc;

    /* 读缓冲 */
    ts_u08 *pu08_rd_buf;

    /* 调试开关 */
    ts_u32 u32_dbg_on;
} hci_ctx_s_t;

static hci_ctx_s_t sst_ctx = {0};

static ts_void hci_do_read(ts_u32 u32_addr, ts_u32 u32_cnt)
{
    ts_u32 i, u32_pkg_size;
    ts_u32 *pu32_buf;
    ts_u32 *pu32_ptr;

    u32_pkg_size = u32_cnt << 2;
    pu32_buf = (ts_u32 *)sst_ctx.pu08_rd_buf;
    pu32_ptr = (ts_u32 *)u32_addr;
    for (i = 0; i < u32_cnt; i++)
        pu32_buf[i] = pu32_ptr[i];

    f_hci_send_resp_pkg((ts_u08 *)pu32_buf, u32_pkg_size, 0);
}

static ts_void hci_do_ack(ts_u32 u32_offset)
{
    ts_u32 *pu32_buf;

    pu32_buf = (ts_u32 *)sst_ctx.pu08_ack_buf;

    /* ack 都是固定长度，如果不固定长度，简单处理可以使用最大的缓冲长度做偏移 */
    pu32_buf += u32_offset * (HCI_CMD_LEN >> 2);

    ts_cpl_memset32((ts_void *)pu32_buf, 0, (HCI_CMD_LEN >> 2));

    pu32_buf[0] = HCI_CMD_MAGIC;
    pu32_buf[1] = HCI_CMD_USBA;
    pu32_buf[2] = sst_ctx.u32_ack_inc;
    

    f_hci_send_resp_pkg((ts_u08 *)pu32_buf, HCI_CMD_LEN, 0);

    //ts_cpl_printk("i=%d\n", sst_ctx.u32_ack_inc);

    sst_ctx.u32_ack_inc++;
}

static ts_u32 hci_mem_init(ts_void *pv_param)
{
    /* 内存大小：
        pu08_cmd_buf : 17KB + 17KB = 34KB
        pu08_ack_buf : 5KB
        pu08_rd_buf  ：17KB
        total = 34KB + 5KB + 17KB = 56KB
    */
   sst_ctx.u32_mem_addr = (ts_u32)ts_cpl_tmm_malloc(pv_param, USB_RW_MAX_LEN, 32, "rpkg");
   if (0 == sst_ctx.u32_mem_addr)
       return 0;

   sst_ctx.u32_inited = 1;

   sst_ctx.pu08_cmd_buf = (ts_u08 *)(sst_ctx.u32_mem_addr);
   sst_ctx.pu08_ack_buf = sst_ctx.pu08_cmd_buf + (USB_CMD_BUF_SIZE * 2);
   sst_ctx.pu08_rd_buf = sst_ctx.pu08_ack_buf + USB_ACK_BUF_SIZE;

   printf("cmd_buf,ack_buf,rd_bud:%p,%p,%p\n", sst_ctx.pu08_cmd_buf, sst_ctx.pu08_ack_buf, sst_ctx.pu08_rd_buf);

   return sst_ctx.u32_mem_addr;
}



TS_WEAK ts_void *ts_isp_get_bp_pos_buf(ts_u32 u32_type, ts_u32 *pu32_bp_cnt)
{
    *pu32_bp_cnt = 0;

    return ts_null;
}


//static 
ts_void get_bp_pos(ts_u32 u32_type)
{
    ts_u32 *pu32_buf;
    ts_u08 *pu08_ptr;
    ts_u32 u32_addr;
    ts_u32 u32_bp_cnt;

    pu08_ptr = ts_isp_get_bp_pos_buf(u32_type, &u32_bp_cnt);
    pu32_buf = (ts_u32 *)sst_ctx.pu08_rd_buf;
    u32_addr = (ts_u32)pu08_ptr;

    pu32_buf[0] = u32_addr;
    pu32_buf[1] = u32_bp_cnt;

    f_hci_send_resp_pkg((ts_u08 *)pu32_buf, 12, 0);
}

TS_WEAK ts_void *ts_isp_get_bp_pos_addr(ts_u32 u32_bp_cnt)
{
    return ts_null;
}

//static 
ts_void get_bp_addr(ts_u32 u32_bp_cnt)
{
    ts_u32 u32_addr;
    ts_u32 *pu32_buf;

    printf("u32_bp_cnt %d\n", u32_bp_cnt);
    pu32_buf = (ts_u32 *)sst_ctx.pu08_rd_buf;
    u32_addr = (ts_u32)ts_isp_get_bp_pos_addr(u32_bp_cnt);

    pu32_buf[0] = u32_addr;

    f_hci_send_resp_pkg((ts_u08 *)pu32_buf, 8, 0);
}


//static 
ts_void notic_isp_complete(ts_void)
{
    ts_u32 *pu32_buf;

    pu32_buf = (ts_u32 *)sst_ctx.pu08_rd_buf;

    ts_isp_set_bp_multi_data();

    pu32_buf[0] = HCI_CMD_MAGIC;
    pu32_buf[1] = HCI_CMD_USBA;
    pu32_buf[2] = 0;
    pu32_buf[3] = 0;

    f_hci_send_resp_pkg((ts_u08 *)pu32_buf, 32, 0);
}

ts_u32 libusb_write_msg(ts_u08 *pu08_rbuf, ts_u32 u32_size)
{
    f_hci_send_resp_pkg(pu08_rbuf,  u32_size, 0);
    return 0;
}


//static 
ts_void libusb_msg_process(ts_void *pv_param)
{
    TS_UNUSED(pv_param);

    // ts_s32 s32_ret;
    ring_buffer_t uart_rb;
    ts_u08* rd_buf;
    ts_u32  rnd_cnt;
    ts_u32  bytes_index;
    ts_u32  left_bytes;
    // ts_u32  m_left_bytes;

    state_machine_t m_state_machine;
    m_state_machine.m_state = STATE_WAIT_HEADER;
    m_state_machine.m_frame_type = 0;
    m_state_machine.m_complete_frame_len = 0;
    m_state_machine.m_frame_bytes_index = 0;

    while(1){

        get_ring_buffer_info(&uart_rb,&m_libusb_ring_buf_instance );
        if(uart_rb.write_index != uart_rb.read_index){

            get_ring_buffer_info(&uart_rb ,&m_libusb_ring_buf_instance );
            if(uart_rb.left_bytes==0){
                ts_cpl_msleep(20);
                continue;
            }

            bytes_index = uart_rb.left_bytes + uart_rb.read_index;

            vision_log_debug("uart_rb.read_index=%u  uart_rb.left_bytes=%u uart_rb.write_index %u \n", uart_rb.read_index ,uart_rb.left_bytes ,uart_rb.write_index);
            if(bytes_index > uart_rb.uart_buff_len){

                rd_buf = uart_rb.uart_buff;
                rd_buf += uart_rb.read_index;
                rnd_cnt = uart_rb.uart_buff_len - uart_rb.read_index;
                vot_hbp_parse_cb_v2(&m_state_machine, rd_buf ,  rnd_cnt, &libusb_write_msg);

                rd_buf = uart_rb.uart_buff ;
                left_bytes = uart_rb.left_bytes - rnd_cnt;
                vot_hbp_parse_cb_v2(&m_state_machine, rd_buf ,  left_bytes, &libusb_write_msg);
                uart_rb.read_index = (bytes_index%uart_rb.uart_buff_len);
              
            }
            else{

                 rd_buf = uart_rb.uart_buff;
                 rd_buf += uart_rb.read_index;
                 rnd_cnt = uart_rb.left_bytes;

                //  vot_uart_write(rd_buf,rnd_cnt);

                 vot_hbp_parse_cb_v2(&m_state_machine, rd_buf ,  rnd_cnt, &libusb_write_msg);
                 uart_rb.read_index = (bytes_index %  uart_rb.uart_buff_len); 
                
            }
        
            ring_buffer_update_read(&uart_rb,&m_libusb_ring_buf_instance );
            vision_log_debug("uart_rb.read_index=%u  uart_rb.left_bytes=%u uart_rb.write_index %u \n", uart_rb.read_index ,uart_rb.left_bytes ,uart_rb.write_index);
        
        }
        else{
            ts_cpl_msleep(50);
        }
	}
			
}

static ts_void hci_vidoe_process(ts_void *pv_param)
{
    ts_u32 *pu32_cmd;
    ts_u32 u32_cmd, u32_mode, u32_first_run, u32_frm_mode;
    ts_s32 s32_start, s32_ret;
    static ts_cpl_semphr_s_t st_sema;
    ts_mdev_t h_dev;
    ts_u32 u32_sub_dev;
    ts_u32 i, u32_w_type, u32_w_cnt, u32_w_addr, u32_w_val, u32_w_cur_addr, u32_ack_idx, u32_alive;
    ts_u32 *pu32_ptr;
    ts_u08 *pu08_cmd;
    ts_char *pc_cmd_str;
    ts_buffer_s_t st_viu_buf;

#ifdef USB_PREF_EN
    ts_cpl_timeval_s_t st_start, st_end;
    ts_u32 u32_frm_cnt;
#endif

    ts_cpl_semphr_init((ts_cpl_semphr_s_t *)&st_sema, 0);

    f_hci_init("ts_hci", (ts_cpl_semphr_s_t *)&st_sema, pv_param);

    ts_cpl_down((ts_cpl_semphr_s_t *)&st_sema);

    //ts_cpl_semphr_destory((ts_cpl_semphr_s_t *)&st_sema);

    hci_mem_init(pv_param);
    if (0 == sst_ctx.u32_mem_addr)
       return;

    f_hci_read_init(sst_ctx.pu08_cmd_buf, USB_CMD_BUF_SIZE);

    s32_start = 0;
    u32_ack_idx = 0;
    u32_mode = 0;
    u32_frm_mode = 0;
    u32_first_run = 1;
    u32_sub_dev = 100;
    while (1) {
        /* 判断是否有连接 */
        u32_alive = libusb_chk_alive();
        if (0 == u32_alive) {
            /* 目前 suspend 只会在拔出线缆才会发生 */
            libusb_close_all();

            s32_start = 0;

            f_hci_stop_video(TS_ISP_MDEV0, u32_mode);

            printf("down for usb\n");
            ts_cpl_down((ts_cpl_semphr_s_t *)&st_sema);
            f_hci_read_init(sst_ctx.pu08_cmd_buf, USB_CMD_BUF_SIZE);
            printf("usb ready\n");
        }

        /* FIXME : 此处多次 queue ， 超时未做 dequeue */
        //ts_cpl_memset32((ts_void *)sst_ctx.pu08_cmd_buf, 0, 8);
        s32_ret = f_hci_read_pkg(&pu08_cmd, USB_CMD_BUF_SIZE, 500);
        if (s32_ret > 0) {
            pu32_cmd = (ts_u32 *)pu08_cmd;
            //printf("pcmd = %p, %d\n", pu32_cmd, s32_ret);
            if (sst_ctx.u32_dbg_on > 0)
                printf("cbuf:%x,%x,%x,%x\n", pu32_cmd[0], pu32_cmd[1], pu32_cmd[2], pu32_cmd[3]);

            if (HCI_CMD_MAGIC == pu32_cmd[0]) {
                u32_cmd = pu32_cmd[1];
                switch (u32_cmd) {
                    case HCI_CMD_USBV:
                        if (0 == s32_start) {
                            h_dev = TS_VPO_MEM_MDEV;
                            u32_sub_dev = pu32_cmd[3];
                            u32_frm_mode = pu32_cmd[5];
                            s32_ret = f_hci_start_video(h_dev, u32_sub_dev, u32_frm_mode);
                            if (TS_OK == s32_ret) {
                                s32_start = 1;
                                if (pu32_cmd[4] > 0)
                                    u32_mode = 1;   /* 仿真模式 */
                                else
                                    u32_mode = 0;

                                printf("dev=%x, sundev=%d, fmode=%d, sim_mode=%d\n", h_dev, u32_sub_dev, u32_frm_mode, u32_mode);
                            } else {
                                printf("start video fail.\n");
                            }
                        }

                        break;

                    case HCI_CMD_USBF:
                        if ((s32_start > 0) && (u32_mode > 0)) {
                            /* 获取空闲帧存 */
                            ts_tdk_viu_get_fb(TS_VIU_MDEV0, (ts_buffer_s_t *)&st_viu_buf);

                            pu32_ptr = (ts_u32 *)st_viu_buf.pu08_ptr;
                            f_hci_read_viu((ts_u08 *)pu32_ptr, (ts_u32)st_viu_buf.sz_size, 0xffffffff);
                            ts_tdk_viu_set_fb(TS_VIU_MDEV0, (ts_buffer_s_t *)&st_viu_buf);
                            ts_tdk_viu_exvsync(TS_VIU_MDEV0, 0);

                            if (1 == u32_first_run) {
                                if (u32_sub_dev >= TS_UPL_POS_VPO_YUV) {
                                    ts_cpl_msleep(100);

                                    /* vpo 取流需要两帧 */
                                    ts_tdk_viu_set_fb(TS_VIU_MDEV0, (ts_buffer_s_t *)&st_viu_buf);
                                    ts_tdk_viu_exvsync(TS_VIU_MDEV0, 0);
                                }

                                u32_first_run = 0;
                            } else {
                                isp_set_run(1);
                            }
                        } else {
                            printf("invalid mode.\n");
                        }

                        break;

                    case HCI_CMD_USBP:
                        if (s32_start > 0) {
                            s32_start = 0;
                            f_hci_stop_video(TS_ISP_MDEV0, u32_mode);

                            if (u32_mode > 0)
                                isp_set_run(0);

                            u32_mode = 0;
                            u32_frm_mode = 0;
                        }

                        break;

                    case HCI_CMD_USBR:
                        hci_do_read(pu32_cmd[2], pu32_cmd[3]);

                        break;

                    case HCI_CMD_USBW:
                        u32_w_type = pu32_cmd[2];
                        u32_w_addr = 0;
                        if (HCI_USBW_SINGLE == u32_w_type) {
                            u32_w_addr = pu32_cmd[3];
                            u32_w_val = pu32_cmd[4];
                            ts_cpl_writel(u32_w_val, u32_w_addr);
                        } else {
                            u32_w_cnt = pu32_cmd[3];
                            pu32_ptr = &pu32_cmd[8];
                            if (HCI_USBW_CONTINUE == u32_w_type) {
                                u32_w_addr = pu32_cmd[4];
                            } else {
                                u32_w_addr = 0;
                            }

                            //ts_cpl_printk("rcnt=%d\n", u32_w_cnt);

                            for (i = 0; i < u32_w_cnt; i++) {
                                if (HCI_USBW_NON_CONTINUE == u32_w_type) {
                                    u32_w_cur_addr = *pu32_ptr++;
                                } else {
                                    u32_w_cur_addr = u32_w_addr + (i * 4);
                                }

                                u32_w_val = *pu32_ptr++;

                                if (sst_ctx.u32_dbg_on > 0)
                                    ts_cpl_printk("wr: %x->%x\n", u32_w_cur_addr, u32_w_val);

                                ts_cpl_writel(u32_w_val, u32_w_cur_addr);
                            }
                        }

                        hci_do_ack(u32_ack_idx);
                        u32_ack_idx++;
                        if (u32_ack_idx >= 152)
                            u32_ack_idx = 0;

                        break;

                    case HCI_CMD_USBC:
                        pc_cmd_str = (ts_char *)&pu32_cmd[2];

                        cli_simple_run_command(pc_cmd_str);

                        break;

                    default:
                        break;
                }
            }
        }
    }  
}


static ts_void *spv_hci_tmm;

ts_void usb_usr_init_libusb(ts_void *pv_tmm)
{
  
    if (0 == sst_ctx.u32_inited) {
        spv_hci_tmm = pv_tmm;

        // if(SystemParaMenu2.m_video_out_mode==VIDEO_OUTPUT_MODE_ENUM_LIBUSB){
        //     ts_cpl_task_s_t st_libusb_msg_task;
        //     ring_buffer_init(&(m_libusb_ring_buf_instance), "vot_libusb_ring_buf");

        //     ts_cpl_kthread_create(&st_libusb_msg_task, (ts_cpl_kthread_t)libusb_msg_process, pv_tmm, "hci_libusb_msg", 4, 4*2048);
        // }
        ts_cpl_kthread_create(&sst_ctx.st_task, (ts_cpl_kthread_t)hci_vidoe_process, pv_tmm, "hci", 4, 2048);
    }
}


/*
 * usb.c -- tdk isp usb 使用例子
 */
#include <base/ts_type.h>
#include <base/ts_bitop.h>
#include <cpl/ts_cpl.h>
#include <uapi/base_v100.h>
#include <uapi/upl_v100.h>
#include <uapi/viu_v100.h>
#include <command.h>
#include <stdio.h>
#include "f_tdk.h"
#include <api.h>

/* from libusb2 */
extern ts_s32 lusb_ir_mfi_init(ts_void *pv_htmm, ts_u32 u32_extar_size);
extern ts_void lusb_ir_mfi_wait_usb_rdy(ts_void);
extern ts_u32 lusb_ir_mfi_chk_usb_rdy(ts_void);

extern ts_s32 lusb_ir_read(ts_u08 *pu08_buf, ts_u32 u32_read_size);
extern ts_s32 lusb_mfi_read(ts_u08 *pu08_buf, ts_u32 u32_read_size);

extern ts_void lusb_ir_write(ts_u08 *pu08_buf, ts_u32 u32_write_size);
extern ts_void lusb_mfi_write(ts_u08 *pu08_buf, ts_u32 u32_write_size);

extern ts_s32 lusb_start_video(ts_u32 u32_mode);
extern ts_s32 lusb_stop_video(ts_void);


#define IR_MAX_CMC_SIZE     (1024)

typedef struct ir_ctx_s {
    /* 保存命令接收和回应的总缓存地址 */
    ts_void *pv_cmc_buf;

    /* IR 命令和回应 */
    ts_u08 *pu08_ir_cmd_buf;
    ts_u08 *pu08_ir_ack_buf;

    /* MFI 命令和回应 */
    ts_u08 *pu08_mfi_cmd_buf;
    ts_u08 *pu08_mfi_ack_buf;
} ir_ctx_s_t;

static ir_ctx_s_t sst_ctx = {0};

static ts_void ir_process(ts_void *pv_param)
{
    ts_s32 s32_ret;
    ts_u32 i, u32_rd_cnt, u32_rdy, u32_start;
    
    for (i = 0; i < 64; i++)
        sst_ctx.pu08_ir_ack_buf[i] = i;

    u32_start = 0;
    while (1) {
        /* 等待 usb 设备正常连接，如果处于断线则休眠等待 */
        u32_rdy = lusb_ir_mfi_chk_usb_rdy();
        if (0 == u32_rdy) {
            if (u32_start > 0) {
                u32_start = 0;
                lusb_stop_video();
            }

            vision_log_debug("ir down\n");
            while (1) {
                u32_rdy = lusb_ir_mfi_chk_usb_rdy();
                if (u32_rdy > 0)
                    break;
                else
                    ts_cpl_msleep(100);
            }

            vision_log_debug("ir ready\n");
        }

        /* 等待命令接收, 不定长接收最大可以接收 512, 指定大小接收可以最大到缓存大小 */
        s32_ret = lusb_ir_read(sst_ctx.pu08_ir_cmd_buf, 512);
        if (s32_ret > 0) {
            u32_rd_cnt = (ts_u32)s32_ret;
            vision_log_debug("ir_rd=%d\n", u32_rd_cnt);

            lusb_ir_write(sst_ctx.pu08_ir_ack_buf, 64);
            sst_ctx.pu08_ir_ack_buf[0] = sst_ctx.pu08_ir_ack_buf[0] + 1;

            if (0 == u32_start) {
                s32_ret = lusb_start_video(0);
                if (TS_OK == s32_ret)
                    u32_start = 1;
            }
        }
    }
}


static ts_void mfi_process(ts_void *pv_param)
{
    ts_s32 s32_ret;
    ts_u32 i, u32_rd_cnt, u32_rdy;

    for (i = 0; i < 1024; i++)
        sst_ctx.pu08_mfi_ack_buf[i] = i;

    while (1) {
        /* 等待 usb 设备正常连接，如果处于断线则休眠等待 */
        u32_rdy = lusb_ir_mfi_chk_usb_rdy();
        if (0 == u32_rdy) {
            vision_log_debug("mfi down\n");
            while (1) {
                u32_rdy = lusb_ir_mfi_chk_usb_rdy();
                if (u32_rdy > 0)
                    break;
                else
                    ts_cpl_msleep(100);
            }

            vision_log_debug("mfi ready\n");
        }

        /* 等待命令接收, 不定长接收最大可以接收 512, 指定大小接收可以最大到缓存大小 */
        s32_ret = lusb_mfi_read(sst_ctx.pu08_mfi_cmd_buf, 512);
        if (s32_ret > 0) {
            u32_rd_cnt = (ts_u32)s32_ret;
            vision_log_debug("mfi_rd=%d\n", u32_rd_cnt);

            lusb_mfi_write(sst_ctx.pu08_mfi_ack_buf, 64);
            sst_ctx.pu08_mfi_ack_buf[0] = sst_ctx.pu08_mfi_ack_buf[0] + 1;
        }
    }
}


ts_void usb_usr_init(ts_void *pv_tmm)
{
    ts_cpl_task_s_t st_task;
    ts_u32 u32_size;
    ts_s32 s32_ret;
    
    if (ts_null == sst_ctx.pv_cmc_buf) {
        u32_size = IR_MAX_CMC_SIZE * 4;
        sst_ctx.pv_cmc_buf = ts_cpl_tmm_malloc(pv_tmm, u32_size, 32, "ir-cmc");
        if (ts_null == sst_ctx.pv_cmc_buf) {
            vision_log_debug("%s\n", TS_ENOMEM_STR);

            return;
        }

        sst_ctx.pu08_ir_cmd_buf = (ts_u08 *)sst_ctx.pv_cmc_buf;
        sst_ctx.pu08_ir_ack_buf = sst_ctx.pu08_ir_cmd_buf + IR_MAX_CMC_SIZE;
        sst_ctx.pu08_mfi_cmd_buf = sst_ctx.pu08_ir_ack_buf + IR_MAX_CMC_SIZE;
        sst_ctx.pu08_mfi_ack_buf = sst_ctx.pu08_mfi_cmd_buf + IR_MAX_CMC_SIZE;
    }

    s32_ret = lusb_ir_mfi_init(pv_tmm, 0);
    if (TS_OK == s32_ret) {
        /* 等待 usb 设备正常连接，如果处于断线则休眠等待 */
        lusb_ir_mfi_wait_usb_rdy();

        ts_cpl_kthread_create(&st_task, (ts_cpl_kthread_t)ir_process, ts_null, "ir_task", 12, 4096);
        ts_cpl_kthread_create(&st_task, (ts_cpl_kthread_t)mfi_process, ts_null, "mfi_task", 12, 4096);
    } else {
        if (sst_ctx.pv_cmc_buf != ts_null) {
            ts_cpl_tmm_free(sst_ctx.pv_cmc_buf);

            sst_ctx.pv_cmc_buf = ts_null;
        }
    }
}

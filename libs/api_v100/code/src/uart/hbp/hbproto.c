/*
 * hbproto.c -- host bootup protocol 从机实现
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2022-2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */

#include <config.h>
#include <base/ts_type.h>
#include <base/ts_errno.h>
#include <base/ts_bitop.h>
#include <uapi/base_v100.h>
#include <uapi/sfc_v100.h>
#include <cpl/ts_cpl.h>
#include <kapi/k_uart_v100.h>
#include <unistd.h>
#include "hbproto.h"
#include <command.h>


#define HBP_TRY_COUNT       (100)
#define MAX_ARG_SIZE        (32)
#define CONFIG_BUFFER_SIZE_2K   1



/* 100ms 超时 */
#define HBP_FLAG_TIMEOUT    (5000)
#define HBP_TRY_TIMEOUT     (200000)

#define HBP_SFFS_MAX_FN     (63)


/* 如果使用 sffs 写，避免 2K 分散写入，频繁对 flash 擦除，降低寿命, 申请大缓存 */
#define HBP_SFFS_MAX_SIZE   (128 * 1024)


extern ts_u16 vot_uart_protocol(ts_u08 *pu08_ptr);
static ts_u32 sau32_packet_buf[TS_HBP_BUF_MAX_SIZE >> 2];


/* 保存当前的 sffs 文件名 */
static ts_u08 sau08_sffs_name[HBP_SFFS_MAX_FN + 1];
static ts_s32 ss32_sffs_hdl = -1;

/* 内部上下文 */
typedef struct hbp_ctx_s {
    /* 对接设备 */
    ts_s32 s32_hdev;

    /* 当前解析的总大小 */
    ts_u32 u32_total_len;

    /* 当前写指针 */
    ts_u32 u32_wr_idx;

    /* 保存的当前校验和 */
    ts_u08 u08_chk_sum;

    ts_u08 au08_rev[3];

    ts_cpl_semphr_s_t st_sem;
    ts_cpl_spinlock_s_t st_spinlock;

    /* 统计信息 */
    ts_u32 u32_err_cnt;
    ts_u32 u32_nrml_cnt;
    ts_u32 u32_tot_frm;

    /* 保存当前 sffs 使用的缓冲，只在有写操作的时候才申请 */
    ts_void *pv_sffs_buf;

    /* 当前保存在缓冲的数据量 */
    ts_u32 u32_sffs_size;

    /* 保存当前的起始偏移地址 */
    ts_u32 u32_sffs_offset;

    /* hbp接口与终端接口相同标志 */
    ts_u32 u32_flag;
} hbp_ctx_s_t;


static hbp_ctx_s_t sst_ctx = {0};

/*
 * 函数名 :  get_ptr_by_offset
 * 描  述 :  使用 sau32_packet_buf 作为协议缓存, 不同命令字，需要获取对应的帧头、属性、帧尾等指针
 * 输  入 :  参数           描述
 *           u32_cmd        命令字
 *           u32_type       类型(帧头、属性包，数据包、帧尾等)
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           无             无
 */
static ts_void *get_ptr_by_offset(ts_u32 u32_type, ts_u32 u32_ignore_attr)
{
    ts_u08 *pu08_ptr;

    pu08_ptr = (ts_u08 *)&sau32_packet_buf;
    if (TS_HBP_FT_ATTR == u32_type) {
        pu08_ptr += sizeof(hbp_head_s_t);
    } else if (TS_HBP_FT_DATA == u32_type) {
        pu08_ptr += sizeof(hbp_head_s_t);
        if (u32_ignore_attr != TS_HBP_IGNORE_ATTR)
            pu08_ptr += sizeof(hbp_attr_s_t);
    }

    return (ts_void *)pu08_ptr;
}


/*
 * 函数名 :  send_packet
 * 描  述 :  整理协议包，必然使用 sau32_packet_buf 作为协议缓存,仅用于支持调试，暂不关注效率;
 * 输  入 :  参数           描述
 *           u32_cmd        命令字
 *           u32_size       扩展包大小，不做大小判断
 *           pst_tail       帧尾， 如果为 ts_null ， 则发送默认数据
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           无             无
 */
static ts_void send_packet(ts_u32 u32_cmd, ts_u32 u32_size, hbp_tail_s_t *pst_tail)
{
    ts_u32 i, u32_cnt;
    hbp_head_s_t *pst_head;
    hbp_tail_s_t *pst_buf_tail;
    ts_u08 *pu08_ptr;
    ts_u08 u08_sum;
    ts_ulong ulg_flag;

    pst_head = (hbp_head_s_t *)get_ptr_by_offset(TS_HBP_FT_HEAD, TS_HBP_IGNORE_ATTR);
    pst_head->u08_flag = TS_HBP_START;
    pst_head->u08_cmd = (ts_u08)u32_cmd;
    pst_head->u16_size = (ts_u16)u32_size;

    u32_cnt = u32_size + sizeof(hbp_head_s_t);
    pu08_ptr = (ts_u08 *)pst_head;

    /* 定位到尾部信息 */
    pu08_ptr += u32_cnt;
    pst_buf_tail = (hbp_tail_s_t *)pu08_ptr;

    /* 准备帧尾 */
    if (ts_null == pst_tail) {
        /* 无输入则使用 0 填充 */
        pst_buf_tail->au08_rev[0] = 0;
        pst_buf_tail->au08_rev[1] = 0;
        pst_buf_tail->au08_rev[2] = 0;
    } else {
        pst_buf_tail->au08_rev[0] = pst_tail->au08_rev[0];
        pst_buf_tail->au08_rev[1] = pst_tail->au08_rev[1];
        pst_buf_tail->au08_rev[2] = pst_tail->au08_rev[2];
    }

    u32_cnt += sizeof(hbp_tail_s_t);

    /* 计算校验和，避免计算最后一个 */
    u08_sum = 0;
    pu08_ptr = (ts_u08 *)pst_head;
    for (i = 0; i < (u32_cnt - 1); i++)
        u08_sum += pu08_ptr[i];

    /* 校验和取反 */
    pst_buf_tail->u08_sum_n = ~u08_sum;

    if (u32_size > 16)
        ts_cpl_set_log_en(0);
    else
        ts_cpl_spin_lock_irqsave(&sst_ctx.st_spinlock, &ulg_flag);

    write(sst_ctx.s32_hdev, pu08_ptr, u32_cnt);

    if (u32_size > 16)
        ts_cpl_set_log_en(1);
    else
        ts_cpl_spin_unlock_irqrestore(&sst_ctx.st_spinlock, &ulg_flag);
}


/*
 * 函数名 :  clean_parse_info
 * 描  述 :  清除协议解析时需要用到的全局变量
 * 输  入 :  参数        描述
 *           无         无
 * 输  出 :  参数        描述
 *           无         无
 * 返  回 :  返回值      描述
 *           无         无
 */
static ts_void clean_parse_info(ts_void)
{
    sst_ctx.u08_chk_sum = 0;
    sst_ctx.u32_wr_idx = 0;

    /* 最小接收个数为一个帧头 */
    sst_ctx.u32_total_len = sizeof(hbp_head_s_t);
}


/*
 * 函数名 :  hbp_parse_cb
 * 描  述 :  hbp串口驱动的回调函数
 * 输  入 :  参数           描述
 *           u08_val           串口此次读取到的数据
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          数据包接收完成
 *           TS_ERR         数据包接收未完成
 */
static ts_bool hbp_parse_cb(ts_u08 u08_val)
{
    ts_u08 *pu08_ptr;
    hbp_head_s_t *pst_head;
    ts_u08 u08_sum_tmp;

    if (sst_ctx.u32_wr_idx >= TS_HBP_BUF_MAX_SIZE) {
        sst_ctx.u32_err_cnt++;
        clean_parse_info();

        return ts_true;
    }

    pu08_ptr = (ts_u08 *)&sau32_packet_buf;
    pu08_ptr += sst_ctx.u32_wr_idx;
    *pu08_ptr = u08_val;
    sst_ctx.u32_wr_idx++;
    if (sizeof(hbp_head_s_t) == sst_ctx.u32_wr_idx) {
        pst_head = (hbp_head_s_t *)get_ptr_by_offset(TS_HBP_FT_HEAD, TS_HBP_IGNORE_ATTR);
        sst_ctx.u32_total_len = pst_head->u16_size + sizeof(hbp_head_s_t) + sizeof(hbp_tail_s_t);
        sst_ctx.u32_tot_frm++;
    } else if (sst_ctx.u32_wr_idx == sst_ctx.u32_total_len) {
        u08_sum_tmp = ~(sst_ctx.u08_chk_sum);
        /* clean */
        clean_parse_info();

        if (u08_val == u08_sum_tmp) {
            sst_ctx.u32_nrml_cnt++;
            ts_cpl_up(&sst_ctx.st_sem);
        } else {
            sst_ctx.u32_err_cnt++;
        }

        return ts_true;
    }

    sst_ctx.u08_chk_sum += u08_val;

    return ts_false;
}


/*
 * 函数名 :  do_read_ram
 * 描  述 :  按照指定配置读数据到 RAM
 * 输  入 :  参数           描述
 *           pst_attr       读写配置
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          读取正常
 *           TS_EINVALSIZE  无效大小
 */
static ts_void do_read_ram(ts_void *pv_rd_addr, ts_void *pv_wr_addr, ts_u32 u32_cnt, ts_u32 u32_bw)
{
    ts_u32 *pu32_wr_ptr, *pu32_rd_ptr, u32_tmp;
    ts_u16 *pu16_wr_ptr, *pu16_rd_ptr, u16_tmp;
    ts_u08 *pu08_wr_ptr, *pu08_rd_ptr, u08_tmp;
    ts_u32 i;

    pu08_wr_ptr = (ts_u08 *)pv_wr_addr;
    pu08_rd_ptr = (ts_u08 *)pv_rd_addr;
    if (TS_BW16 == u32_bw) {
        pu16_rd_ptr = (ts_u16 *)pu08_rd_ptr;
        pu16_wr_ptr = (ts_u16 *)pu08_wr_ptr;
        for (i = 0; i < (u32_cnt >> 1); i++) {
            u16_tmp = *pu16_rd_ptr++;
            *pu16_wr_ptr++ = u16_tmp;
        }
    } else if (TS_BW32 == u32_bw) {
        pu32_rd_ptr = (ts_u32 *)pu08_rd_ptr;
        pu32_wr_ptr = (ts_u32 *)pu08_wr_ptr;
        for (i = 0; i < (u32_cnt >> 2); i++) {
            u32_tmp = *pu32_rd_ptr++;
            *pu32_wr_ptr++ = u32_tmp;
        }
    } else {
        for (i = 0; i < u32_cnt; i++) {
            u08_tmp = *pu08_rd_ptr++;
            *pu08_wr_ptr++ = u08_tmp;
        }
    }

    /* 使用标准 tail */
    send_packet(TS_HBP_CMD_ACK_DONE, u32_cnt, ts_null);
}


/*
 * 函数名 :  do_write_ram
 * 描  述 :  按照指定配置写数据到 RAM
 * 输  入 :  参数           描述
 *           pst_attr       读写配置
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          读取正常
 *           TS_EINVALSIZE  无效大小
 */
static ts_void do_write_ram(ts_void *pv_rd_addr, ts_void *pv_wr_addr, ts_u32 u32_cnt, ts_u32 u32_bw)
{
    ts_u32 *pu32_wr_ptr, *pu32_rd_ptr;
    ts_u16 *pu16_wr_ptr, *pu16_rd_ptr;
    ts_u08 *pu08_wr_ptr, *pu08_rd_ptr;
    ts_u32 i;

    pu08_rd_ptr = (ts_u08 *)pv_rd_addr;
    pu08_wr_ptr = (ts_u08 *)pv_wr_addr;
    if (TS_BW16 == u32_bw) {
        pu16_wr_ptr = (ts_u16 *)pu08_wr_ptr;
        pu16_rd_ptr = (ts_u16 *)pu08_rd_ptr;
        for (i = 0; i < (u32_cnt >> 1); i++)
            *pu16_wr_ptr++ = *pu16_rd_ptr++;
    } else if (TS_BW32 == u32_bw) {
        pu32_wr_ptr = (ts_u32 *)pu08_wr_ptr;
        pu32_rd_ptr = (ts_u32 *)pu08_rd_ptr;
        for (i = 0; i < (u32_cnt >> 2); i++)
            *pu32_wr_ptr++ = *pu32_rd_ptr++;
    } else {
        for (i = 0; i < u32_cnt; i++)
            *pu08_wr_ptr++ = *pu08_rd_ptr++;
    }

    send_packet(TS_HBP_CMD_ACK_DONE, 0, ts_null);
}


static ts_s32 do_rw_ram(hbp_rw_attr_s_t *pst_attr, ts_u32 u32_cmd)
{
    ts_u32 i, u32_cnt, u32_bw, u32_rw_cnt;
    ts_void *pv_rd_addr;
    ts_void *pv_wr_addr;
    ts_u08 *pu08_rd_buf;
    ts_u08 *pu08_sffs;

    u32_cnt = pst_attr->u32_size;
    u32_bw = pst_attr->u32_bw;
    if (u32_cnt > TS_HBP_BUF_MAX_SIZE)
        return TS_ERR;

    if (TS_HBP_CMD_READ == u32_cmd) {
        pv_rd_addr = (ts_void *)pst_attr->u32_addr;
        pv_wr_addr = (ts_void *)get_ptr_by_offset(TS_HBP_FT_DATA, TS_HBP_IGNORE_ATTR);

        if (pst_attr->u32_type != BOOTUP_DEVTYPE_SFFS) {
            do_read_ram(pv_rd_addr, pv_wr_addr, u32_cnt, u32_bw);
        } else {
            if (ss32_sffs_hdl > 0) {
                u32_rw_cnt = ts_lib_sfc_read(ss32_sffs_hdl, pst_attr->u32_addr, (ts_u08 *)pv_wr_addr, u32_cnt);
                if (0 == u32_rw_cnt)
                    return TS_ERR;
                else
                    send_packet(TS_HBP_CMD_ACK_DONE, u32_cnt, ts_null);
            } else {
                return TS_ERR;
            }
        }
    } else {
        pv_wr_addr = (ts_void *)pst_attr->u32_addr;
        pv_rd_addr = (ts_void *)get_ptr_by_offset(TS_HBP_FT_DATA, TS_HBP_NEED_ATTR);

        if (pst_attr->u32_type != BOOTUP_DEVTYPE_SFFS) {
            do_write_ram(pv_rd_addr, pv_wr_addr, u32_cnt, u32_bw);
        } else {
            if (ts_null == sst_ctx.pv_sffs_buf) {
                sst_ctx.pv_sffs_buf = ts_cpl_tmm_malloc(ts_null, HBP_SFFS_MAX_SIZE, 32, "hbp_ssfs");
                if (ts_null == sst_ctx.pv_sffs_buf)
                    return TS_ERR;
            }

            if ((ss32_sffs_hdl <= 0) || ((sst_ctx.u32_sffs_size + u32_cnt) > HBP_SFFS_MAX_SIZE))
                return TS_ERR;

            if (0 == sst_ctx.u32_sffs_size)
                sst_ctx.u32_sffs_offset = pst_attr->u32_addr;

            /* 数据凑满一整块才正式写入文件 */
            pu08_rd_buf = (ts_u08 *)pv_rd_addr;
            pu08_sffs = (ts_u08 *)sst_ctx.pv_sffs_buf;
            pu08_sffs += sst_ctx.u32_sffs_size;
            for (i = 0; i < u32_cnt; i++)
                pu08_sffs[i] = pu08_rd_buf[i];

            sst_ctx.u32_sffs_size += u32_cnt;
            if ((sst_ctx.u32_sffs_size >= HBP_SFFS_MAX_SIZE) || (pst_attr->u32_index > 0)) {
                u32_rw_cnt = ts_lib_sfc_write(ss32_sffs_hdl, sst_ctx.u32_sffs_offset, sst_ctx.pv_sffs_buf, sst_ctx.u32_sffs_size);
                if (0 == u32_rw_cnt)
                    return TS_ERR;

                /* clear */
                sst_ctx.u32_sffs_offset = 0;
                sst_ctx.u32_sffs_size = 0;
            }

            send_packet(TS_HBP_CMD_ACK_DONE, 0, ts_null);
        }
    }

    return TS_OK;
}


/*
 * 函数名 :  clone_attr
 * 描  述 :  避免后续操作将 buf 中的 attr 改写，这里简单 copy 出来单独使用
 * 输  入 :  参数        描述
 *           无         无
 * 输  出 :  参数        描述
 *           无         无
 * 返  回 :  返回值      描述
 *           无         无
 */
static ts_void clone_attr(hbp_attr_s_t *pst_attr)
{
    hbp_attr_s_t *pst_old_attr;

    pst_old_attr = (hbp_attr_s_t *)get_ptr_by_offset(TS_HBP_FT_ATTR, TS_HBP_IGNORE_ATTR);
    ts_cpl_memcpy((ts_void *)pst_attr, (ts_void *)pst_old_attr, sizeof(hbp_attr_s_t));
}


static ts_s32 exec_read_write(ts_u32 u32_cmd, hbp_rw_attr_s_t *pst_rw_attr)
{
    /* 操作属性内部数据按照 8bit 单位计量 */
    if (pst_rw_attr->u32_size > TS_HBP_DATA_MAX_SIZE)
        return TS_EINVALSIZE;

    return do_rw_ram(pst_rw_attr, u32_cmd);
}

TS_WEAK ts_void *ts_isp_get_bp_pos_buf(ts_u32 u32_type, ts_u32 *pu32_bp_cnt)
{
    *pu32_bp_cnt = 0;

    /* u32_type: 0 板端标记 1 显示所有坏点 */

    return ts_null;
}


/* 获取isp坏点数据内存地址及坏点个数 */
static ts_s32 exec_get_bp_pos(ts_void)
{
    ts_u32 *pu32_buf;
    ts_u08 *pu08_ptr;
    ts_u32 u32_addr;
    ts_u32 u32_bp_cnt;

    pu32_buf = (ts_u32 *)get_ptr_by_offset(TS_HBP_FT_DATA, TS_HBP_IGNORE_ATTR);
    pu08_ptr = ts_isp_get_bp_pos_buf(pu32_buf[0], &u32_bp_cnt);

    if (ts_null == pu08_ptr || 0 == u32_bp_cnt) {
        *pu32_buf++ = 0;
        *pu32_buf = 0;
    } else {
        u32_addr = (ts_u32)pu08_ptr;
        *pu32_buf++ = u32_addr;
        *pu32_buf = u32_bp_cnt;
    }

    send_packet(TS_HBP_CMD_ACK_DONE, 8, ts_null);

    return TS_OK;
}


TS_WEAK ts_void *ts_isp_get_bp_pos_addr(ts_u32 u32_bp_cnt)
{
    ts_cpl_log_warn("do nothing\n");

    return ts_null;
}


/* 获取isp存放工具端下发坏点数据的地址 */
static ts_s32 exec_get_bp_addr(ts_void)
{
    ts_u32 *pu32_buf;
    ts_u32 u32_bp_cnt;
    ts_u08 *pu08_ptr;

    pu32_buf = (ts_u32 *)get_ptr_by_offset(TS_HBP_FT_DATA, TS_HBP_IGNORE_ATTR);
    u32_bp_cnt = *pu32_buf;

    pu08_ptr = (ts_u08 *)ts_isp_get_bp_pos_addr(u32_bp_cnt);
    if (ts_null == pu08_ptr) {
        send_packet(TS_HBP_CMD_ACK_ERROR, 0, ts_null);
    } else {
        *pu32_buf = (ts_u32)pu08_ptr;
        send_packet(TS_HBP_CMD_ACK_DONE, 4, ts_null);
    }

    return TS_OK;
}


TS_WEAK ts_void ts_isp_set_bp_multi_data(ts_void)
{
    /* do nothing */
    ts_cpl_log_warn("do nothing\n");
}


static ts_s32 exec_notic_bp_recv_complete(ts_void)
{
    ts_isp_set_bp_multi_data();

    send_packet(TS_HBP_CMD_ACK_DONE, 0, ts_null);

    return TS_OK;
}


TS_WEAK ts_void isp_auto_bp_det(ts_u32 u32_hist, ts_u32 u32_diff, ts_u32 u32_flt_size)
{
    ts_cpl_log_warn("%s do nothing\n",__FUNCTION__);

    return;
}

TS_WEAK ts_void isp_collect_bp_pos_core(ts_void)
{
    ts_cpl_log_warn("do nothing\n");

    return;
}

static ts_s32 exec_upd_bp_data(ts_void)
{
    ts_u32 u32_type, u32_hist, u32_diff, u32_flt_size;
    ts_u32 *pu32_buf;

    pu32_buf = (ts_u32 *)get_ptr_by_offset(TS_HBP_FT_DATA, TS_HBP_IGNORE_ATTR);

    u32_type = pu32_buf[0];
    u32_hist = pu32_buf[1];
    u32_diff = pu32_buf[2];
    u32_flt_size = pu32_buf[3];
    if (0 == u32_type) {
        isp_auto_bp_det(u32_hist, u32_diff, u32_flt_size);
    } else {
        //isp_collect_bp_pos_core();
    }

    send_packet(TS_HBP_CMD_ACK_DONE, 0, ts_null);

    return TS_OK;
}


static ts_s32 exec_sffs_open(hbp_rw_attr_s_t *pst_rw_attr)
{
    ts_void *pv_filename;
    ts_s32 s32_file_size;
    hbp_sffs_tail_s_t st_tail;

    if (ss32_sffs_hdl > 0)
        ts_lib_sfc_close(ss32_sffs_hdl);

    /* 获取文件名, 文件名不能大于 63 个字节 */
    pv_filename = (ts_void *)get_ptr_by_offset(TS_HBP_FT_DATA, TS_HBP_NEED_ATTR);
    ts_cpl_memset32((ts_u32 *)&sau08_sffs_name, 0, (HBP_SFFS_MAX_FN + 1) >> 2);
    ts_cpl_strncpy((ts_char *)&sau08_sffs_name, (ts_char *)pv_filename, HBP_SFFS_MAX_FN);

    ss32_sffs_hdl = ts_lib_sfc_open((ts_char *)&sau08_sffs_name);
    if (TS_ERR == ss32_sffs_hdl)
        return TS_ERR;

    /* 获取文件大小，受限协议包，文件大小不能大于 0xFFFFFF */
    s32_file_size = ts_lib_sfc_get_size(ss32_sffs_hdl);
    if ((s32_file_size > 0) && (s32_file_size < 0xFFFFFF)) {
        st_tail.u32_size = (ts_u32)s32_file_size;

        send_packet(TS_HBP_CMD_ACK_DONE, 0, (hbp_tail_s_t *)&st_tail);

        /* clear */
        sst_ctx.u32_sffs_offset = 0;
        sst_ctx.u32_sffs_size = 0;

        return TS_OK;
    }

    return TS_ERR;
}

#if 0
static ts_void exec_reboot(hbp_run_tail_s_t *pst_run_tail)
{
    ts_u32 u32_ms;

    u32_ms = pst_run_tail->u32_value;

    /* 数值不能太小，避免芯片陷于看门狗重复重启中，需留给系统启动到重新配置看门狗的时间 */
    if (u32_ms < 300)
        u32_ms = 300;

    send_packet(TS_HBP_CMD_ACK_DONE, 0, ts_null);

#if 0
    wdt_stop();
    wdt_start(u32_ms);
#endif

    while(1);
}
#endif


/*
 * 函数名 :  set_uart_hbp_cb
 * 描  述 :  设置协议解析回调函数
 * 输  入 :  参数        描述
 *           无         无
 * 输  出 :  参数        描述
 *           无         无
 * 返  回 :  返回值      描述
 *           TS_ERR         设置失败
 *           TS_OK          设置成功
 */
static ts_s32 set_uart_hbp_cb(ts_void)
{
    return ts_lib_uart_hbp_set_attr(sst_ctx.s32_hdev, hbp_parse_cb, TS_HBP_START, 1);
}


static ts_s32 hbp_drv_proc_show(ts_void *s, ts_void *v)
{
    uart_proto_s_t st_hbp_attr;
    ts_s32 s32_ret;

    s32_ret = ts_lib_uart_hbp_get_attr(sst_ctx.s32_hdev, &st_hbp_attr);
    if (TS_OK != s32_ret) {
        ts_cpl_seq_printf(s, "hbp info get fail!\n");

        return TS_ERR;
    }

    ts_cpl_seq_printf(s, "HBP info:\n");
    ts_cpl_seq_printf(s, "dev handel     : %d\n", sst_ctx.s32_hdev);
    ts_cpl_seq_printf(s, "parse cb addr  : %p\n", st_hbp_attr.fp_cb);
    ts_cpl_seq_printf(s, "frame head     : 0x%x\n", st_hbp_attr.u32_flg);
    ts_cpl_seq_printf(s, "enable status  : %d\n", st_hbp_attr.u32_en);
    ts_cpl_seq_printf(s, "err frame cnt  : %d\n", sst_ctx.u32_err_cnt);
    ts_cpl_seq_printf(s, "true frame cnt : %d\n", sst_ctx.u32_nrml_cnt);
    ts_cpl_seq_printf(s, "total frame cnt: %d\n", sst_ctx.u32_tot_frm);

    return 0;
}

static ts_cpl_seq_ops_s_t sst_hbp_seq_ops = {
    .show = hbp_drv_proc_show,
};

static ts_s32 hbp_proc_open(ts_void *inode, ts_void *file)
{
    return ts_cpl_seq_open(file, &sst_hbp_seq_ops);
}

static ts_ssize_t hbp_proc_write(ts_void *pv_file, const ts_char *pc_buf, ts_size_t sz_size, ts_loff_t *plof_pos)
{
    ts_char ac_buf[MAX_ARG_SIZE];
    ts_char *pc_cmd;
    ts_s32 s32_val;
    ts_s32 s32_ret;

    ts_cpl_strncpy((char *)&ac_buf, pc_buf, (MAX_ARG_SIZE - 1));
    if ('?' == ac_buf[0]) {
        ts_cpl_printk("hbp proc cmd help:\n");
        ts_cpl_printk("?: help\n");
        ts_cpl_printk("enable=[0 or 1]: hbp enable set 1 ,disable set 0\n");
    } else {
        pc_cmd = ts_cpl_split_args(ac_buf, &s32_val);
        if (pc_cmd != ts_null) {
            if (0 == ts_cpl_strncmp(pc_cmd, "enable", 16)) {
                s32_ret = ioctl(sst_ctx.s32_hdev, TS_IOCTRL_IOC_NR_UART_SET_HBP_EN, (ts_ulong)s32_val);
                if (TS_OK != s32_ret)
                    ts_cpl_printk("hbp info set fail!\n");
            }
        }
    }

    return sz_size;
}


static ts_cpl_fileops_s_t sst_hbp_proc_fops = {
    .open = hbp_proc_open,
    .write = hbp_proc_write,
};

static ts_cpl_proc_s_t sst_proc = {
    .pst_fops = &sst_hbp_proc_fops,
};


static ts_void hbp_proc_init(ts_void)
{
    ts_cpl_create_proc(&sst_proc, "/proc/hbp");

    sst_ctx.u32_nrml_cnt = 0;
    sst_ctx.u32_err_cnt = 0;
    sst_ctx.u32_tot_frm = 0;
}


/*
 * 函数名 :  hbp_lib_task
 * 描  述 :  协议执行，在此函数中循环检测命令并执行
 * 输  入 :  参数        描述
 *           无         无
 * 输  出 :  参数        描述
 *           无         无
 * 返  回 :  返回值      描述
 *           TS_ERR      任务启动失败
 * 
 */
static ts_s32 hbp_task(ts_void *pv_data)
{
    TS_UNUSED(pv_data);

    ts_u32 u32_cmd;
    ts_s32 s32_ret;
    hbp_head_s_t *pst_head;
    //hbp_tail_s_t st_tail;
    hbp_attr_s_t st_attr;
    
    /* 初始化信号量 */
    s32_ret = ts_cpl_semphr_init(&sst_ctx.st_sem, 0);
    if (TS_OK != s32_ret)
        return TS_ERR;

    s32_ret = ts_cpl_spinlock_init(&sst_ctx.st_spinlock);
    if (TS_OK != s32_ret)
        return TS_ERR;

    /* 初始化hbp解析参数*/
    clean_parse_info();

    /* 使用标准输入句柄，通过ioctrl来设置串口回调 */
    s32_ret = set_uart_hbp_cb();
    if (s32_ret < 0)
        return TS_ERR;

    while (1) {
        s32_ret = ts_cpl_down_interruptible(&sst_ctx.st_sem);
        if (TS_OK == s32_ret) {

			// ts_u16 u16_len;
			vot_uart_protocol((ts_u08 *)sau32_packet_buf);
			// if(u16_len > 0)
			// {
			// 	// 获取 vot uart protocol 数据，长度为u16_len
			// 	// ts_cpl_printk("vot_uart_protocol %d\n", u16_len);
			// }

            pst_head = (hbp_head_s_t *)&sau32_packet_buf;

            clone_attr(&st_attr);

            u32_cmd = (ts_u32)pst_head->u08_cmd;
            s32_ret = TS_ENOTSUP;
            switch (u32_cmd) {
            case TS_HBP_CMD_READ:
            case TS_HBP_CMD_WRITE:
                s32_ret = exec_read_write(u32_cmd, (hbp_rw_attr_s_t *)&st_attr);

                break;

            case TS_HBP_CMD_REBOOT:
                s32_ret = TS_HBP_CMD_ACK_UNSUP;
                //exec_reboot((hbp_run_tail_s_t *)&st_tail);

                break;

            case TS_HBP_CMD_QUIT:
                send_packet(TS_HBP_CMD_ACK_DONE, 0, 0);

                return TS_OK;

            case TS_HBP_CMD_SFFS_OPEN:
                s32_ret = exec_sffs_open((hbp_rw_attr_s_t *)&st_attr);

                break;

            case TS_HBP_CMD_GET_BP:
                s32_ret = exec_get_bp_pos();

                break;

            case TS_HBP_CMD_GET_BP_ADDR:
                s32_ret = exec_get_bp_addr();

                break;

            case TS_HBP_CMD_BP_COMPLETE_SIGN:
                s32_ret = exec_notic_bp_recv_complete();

                break;

            case TS_HBP_CMD_UPDATE_BP_DATA:
                s32_ret = exec_upd_bp_data();

                break;

            default:
                break;
            }

            if (TS_OK == s32_ret)
                continue;
            else if (TS_EBUSY == s32_ret)
                u32_cmd = TS_HBP_CMD_ACK_BUSY;
            else if (TS_ENOTSUP == s32_ret)
                u32_cmd = TS_HBP_CMD_ACK_UNSUP;
            else
                u32_cmd = TS_HBP_CMD_ACK_ERROR;
        } else {
            u32_cmd = TS_HBP_CMD_ACK_ERROR;
        }

        send_packet(u32_cmd, 0, ts_null);
    }
}


// ts_s32 ts_lib_uart_hbp_task(ts_s32 s32_hdev, ts_u32 u32_prio, ts_u32 u32_stack_size)
// {
//     ts_cpl_task_s_t st_task;

// #ifdef CONFIG_SUPPORT_PROCFS
//     hbp_proc_init();
// #endif

//     if ((s32_hdev < 0) || (u32_stack_size < 256)) {
//         ts_uapi_log_err("%s\n", TS_EINVAL_STR);

//         return -TS_EINVAL;
//     }

//     sst_ctx.s32_hdev = s32_hdev;
//     sau08_sffs_name[0] = '\0';

//     ts_cpl_kthread_create(&st_task, (ts_cpl_kthread_t)hbp_task, ts_null, "hbp", u32_prio, u32_stack_size);

//     return TS_OK;
// }
ts_s32 ts_lib_uart_hbp_task(ts_s32 s32_hdev, ts_u32 u32_prio, ts_u32 u32_stack_size, ts_u32 u32_flag)
{
    ts_cpl_task_s_t st_task;

#ifdef CONFIG_SUPPORT_PROCFS
    hbp_proc_init();
#endif

    if ((s32_hdev < 0) || (u32_stack_size < 256)) {
        ts_uapi_log_err("%s\n", TS_EINVAL_STR);

        return -TS_EINVAL;
    }

    sst_ctx.u32_flag = u32_flag;

    sst_ctx.s32_hdev = s32_hdev;
    sau08_sffs_name[0] = '\0';

    ts_cpl_kthread_create(&st_task, (ts_cpl_kthread_t)hbp_task, ts_null, "hbp", u32_prio, u32_stack_size);

    return TS_OK;
}


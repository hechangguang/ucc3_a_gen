/* 
 * i2c_tg_v100.c -- i2c_tg_v100 驱动实现
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#include <config.h>
#include <base/ts_type.h>
#include <base/ts_errno.h>
#include <cpl/ts_cpl.h>
#include <kapi/k_i2c_v100.h>
#include <linux/io.h>
#include <tsp.h>
#include "i2c_tg_v100.h"

#define I2C_DEFAULT_TMOUT       0x100000
#define I2C_SLV_BUF_MAX_SIZE    (1024 + 6)
#define I2C_SLV_NODE_NUM        (40)


static slv_req_s_t *slv_get_free_node(ts_i2c_dev_s_t *pst_dev)
{
    slv_req_s_t *pst_req;

    if (ts_cpl_list_empty((ts_cpl_list_head_s_t *)&pst_dev->st_slv_free_hdr) > 0)
        return ts_null;

    pst_req = (slv_req_s_t *)pst_dev->st_slv_free_hdr.next;
    ts_cpl_list_del_init((ts_cpl_list_head_s_t *)&pst_req->st_list);

    return pst_req;
}


static ts_void slv_put_free_node(ts_i2c_dev_s_t *pst_dev, slv_req_s_t *pst_req)
{
    if (ts_null == pst_req)
        return;

    ts_cpl_list_add_tail((ts_cpl_list_head_s_t *)&pst_req->st_list, (ts_cpl_list_head_s_t *)&pst_dev->st_slv_free_hdr);
}


static slv_req_s_t *slv_get_rdy_node(ts_i2c_dev_s_t *pst_dev)
{
    slv_req_s_t *pst_req;

    if (ts_cpl_list_empty((ts_cpl_list_head_s_t *)&pst_dev->st_slv_rdy_hdr) > 0)
        return ts_null;

    pst_req = (slv_req_s_t *)pst_dev->st_slv_rdy_hdr.next;
    ts_cpl_list_del_init((ts_cpl_list_head_s_t *)&pst_req->st_list);

    return pst_req;
}


static ts_void slv_put_rdy_node(ts_i2c_dev_s_t *pst_dev, slv_req_s_t *pst_req)
{
    if (ts_null == pst_req)
        return;

    ts_cpl_list_add_tail((ts_cpl_list_head_s_t *)&pst_req->st_list, (ts_cpl_list_head_s_t *)&pst_dev->st_slv_rdy_hdr);
}


ts_s32 write_cmd(ts_u32 u32_dev_regbase, ts_u32 u32_cmd)
{
    ts_u32 u32_status, u32_tmout;

    u32_tmout = 0;
    while (1) {
        u32_status = readl(u32_dev_regbase + I2C_TG_OFFSET_STATUS);
        if (I2C_BIT_DEV_TFNF == (u32_status & I2C_BIT_DEV_TFNF))
            break;

        u32_tmout++;
        if (u32_tmout > I2C_DEFAULT_TMOUT)
            return -TS_ETIMEDOUT;
    }

    writel(u32_cmd, (u32_dev_regbase + I2C_TG_OFFSET_DAT_CMD));

    //ts_cpl_printk("cmd = %#x\n", u32_cmd);

    return TS_OK;
}


static ts_void set_clk(ts_u32 u32_dev_regbase, ts_u32 u32_clk, ts_i2c_attr_s_t *pst_attr)
{
    ts_u32 u32_total;
    ts_u32 u32_hi, u32_lo, u32_dif;
    ts_u32 u32_sda_hold_clk;

    u32_total = u32_clk / pst_attr->u32_bus_hz;

    u32_hi = (u32_total * pst_attr->u08_duty) / 100;
    u32_lo = u32_total - u32_hi;
    if ((pst_attr->u08_duty_adj > 0) && (pst_attr->u08_duty_adj < 0x80)) {
        u32_dif = pst_attr->u08_duty_adj;
        u32_hi += u32_dif;
        u32_lo -= u32_dif;
    } else {
        u32_dif = pst_attr->u08_duty_adj;
        u32_dif &= 0x7F;
        u32_hi -= u32_dif;
        u32_lo += u32_dif;
    }

    writel(u32_hi, (u32_dev_regbase + I2C_TG_OFFSET_SCLCNT_H));
    writel(u32_lo, (u32_dev_regbase + I2C_TG_OFFSET_SCLCNT_L));

    /* 计算 sda hold */
    u32_sda_hold_clk = 1;
    if (pst_attr->u08_sda_hold_clk > 0) {
        u32_sda_hold_clk = pst_attr->u08_sda_hold_clk;
    } else {
        if (pst_attr->u08_sda_hold_us > 0) {
            u32_total = u32_clk / 1000 / 1000;
            u32_sda_hold_clk = u32_total * pst_attr->u08_sda_hold_us;
        }
    }

    writel(u32_sda_hold_clk, (u32_dev_regbase + I2C_TG_OFFSET_SDA_HOLD));
}


ts_s32 i2c_tdl_get_attr(ts_i2c_dev_s_t *pst_dev, ts_i2c_attr_s_t *pst_attr)
{
    ts_cpl_memcpy((ts_void *)pst_attr, &pst_dev->st_attr, sizeof(ts_i2c_attr_s_t));

    return TS_OK;
}

ts_s32 i2c_tdl_set_attr(ts_i2c_dev_s_t *pst_dev, const ts_i2c_attr_s_t *pst_attr)
{
    set_clk(pst_dev->u32_regbase, pst_dev->u32_clk, (ts_i2c_attr_s_t *)pst_attr);

    ts_cpl_memcpy(&pst_dev->st_attr, (ts_void *)pst_attr, sizeof(ts_i2c_attr_s_t));

    return TS_OK;
}


TS_STATIC_INLINE ts_void write_dev_addr(ts_u32 u32_dev_regbase, ts_u32 u32_dev_addr)
{
    writel((u32_dev_addr >> 1), (u32_dev_regbase + I2C_TG_OFFSET_TARGET));
}


TS_STATIC_INLINE ts_void disable_dev(ts_u32 u32_dev_regbase)
{
    ts_u32 u32_val;

    u32_val = readl(u32_dev_regbase + I2C_TG_OFFSET_ENABLE);
    u32_val &= ~(I2C_BIT_DEV_ENABLE);
    writel(u32_val, (u32_dev_regbase + I2C_TG_OFFSET_ENABLE));
}

TS_STATIC_INLINE ts_void enable_dev(ts_u32 u32_dev_regbase)
{
    ts_u32 u32_val;

    u32_val = readl(u32_dev_regbase + I2C_TG_OFFSET_ENABLE);
    u32_val |= I2C_BIT_DEV_ENABLE;
    writel(u32_val, (u32_dev_regbase + I2C_TG_OFFSET_ENABLE));
}


ts_s32 i2c_tdl_write(ts_i2c_dev_s_t *pst_dev, ts_i2c_msg_s_t *pst_msg)
{
    ts_u32 u32_regbase, u32_mode;
    ts_u32 u32_wr_cnt, u32_dat_16bit, u32_val, u32_wr_idx;
    ts_u08 *pu08_ptr;
    ts_s32 s32_ret;
    ts_u32 u32_tm_out;

    u32_regbase = pst_dev->u32_regbase;
    u32_mode = pst_msg->u08_mode;
    u32_wr_cnt = pst_msg->u32_buf_size;
    if ((I2C_BUS_RA08_D16 == u32_mode) || (I2C_BUS_RA16_D16 == u32_mode)) {
        u32_wr_cnt = u32_wr_cnt << 1;
        u32_dat_16bit = 1;
    } else {
        u32_dat_16bit = 0;
    }

    if (ts_null == pst_msg->pst_attr)
        u32_tm_out = pst_dev->st_attr.u08_time_out_jfs;
    else
        u32_tm_out = pst_msg->pst_attr->u08_time_out_jfs;

    s32_ret = i2c_tdl_wait_idle(u32_regbase, u32_tm_out);
    if (s32_ret != TS_OK)
        return s32_ret;

    disable_dev(u32_regbase);
    write_dev_addr(u32_regbase, pst_msg->u16_dev_addr);
    enable_dev(u32_regbase);

    /* 写入寄存器地址, 如果是 16bit 地址，则先发送高 8 bit */
    if (0 == pst_msg->u08_ignore_reg_addr) {
        u32_val = (ts_u32)pst_msg->u16_reg_addr;
        if ((I2C_BUS_RA16_D08 == u32_mode) || (I2C_BUS_RA16_D16 == u32_mode))
            write_cmd(u32_regbase, ((u32_val >> 8) & 0xFF));

        write_cmd(u32_regbase, ((u32_val >> 0) & 0xFF));
    }

    /* 在发送的过程中完成接收 */
    u32_wr_idx = 0;
    pu08_ptr = (ts_u08 *)pst_msg->pv_buf;
    while (1) {
        if (0 == u32_wr_cnt)
            break;

        /* 如果数据位 16bit ，需先写高位 */
        if (u32_dat_16bit > 0) {
            if (0 == (u32_wr_idx & 0x01))
                u32_val = (ts_u32)pu08_ptr[u32_wr_idx + 1];
            else
                u32_val = (ts_u32)pu08_ptr[u32_wr_idx - 1];
        } else {
            u32_val = (ts_u32)pu08_ptr[u32_wr_idx];
        }

        if (1 == u32_wr_cnt)
            u32_val |= I2C_BIT_CMD_STOP;

        s32_ret = write_cmd(u32_regbase, u32_val);
        if (s32_ret != TS_OK)
            return s32_ret;

        u32_wr_idx++;
        u32_wr_cnt--;
    }

    return TS_OK;
}


ts_s32 i2c_tdl_wait_idle(ts_u32 u32_dev_regbase, ts_u32 u32_time_out)
{
    ts_u32 u32_start, u32_now, u32_dif;
    ts_u32 u32_val;

    u32_start = ts_cpl_get_tickcount();
    while (1) {
        u32_val = readl(u32_dev_regbase + I2C_TG_OFFSET_STATUS);
        if ((1 << I2C_BIT_DEV_TFNF) == (u32_val & I2C_BIT_DEV_BUSY))
            return TS_OK;

        u32_now = ts_cpl_get_tickcount();
        if (u32_start > u32_now)
            u32_dif = (0xFFFFFFFF - u32_start) + u32_now;
        else
            u32_dif = u32_now - u32_start;

        if (u32_dif > u32_time_out)
            return -TS_EBUSY;
    }
}


/* 
 * 函数名 :  i2c_tdl_read
 * 描  述 :  i2c 读数据
 * 输  入 :  参数               描述
 *           pst_dev            i2c 设备
 *           pst_msg            本次 i2c 传输消息结构体
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           TS_OK              启动成功
 *           TS_ERR             启动失败
 */
ts_s32 i2c_tdl_read(ts_i2c_dev_s_t *pst_dev, ts_i2c_msg_s_t *pst_msg)
{
    ts_u32 u32_regbase, u32_mode, u32_dat_16bit;
    ts_u32 i, u32_clk_cnt, u32_rd_cnt, u32_pre_cnt, u32_val, u32_rd_idx;
    ts_u08 *pu08_ptr;
    ts_u08 u08_val;
    ts_u32 u32_tm_out;
    ts_u32 u32_start, u32_now;
    ts_s32 s32_ret;

    u32_regbase = pst_dev->u32_regbase;
    //ts_cpl_printk("reg = %#x\n", u32_regbase);

    if (ts_null == pst_msg->pst_attr)
        u32_tm_out = pst_dev->st_attr.u08_time_out_jfs;
    else
        u32_tm_out = pst_msg->pst_attr->u08_time_out_jfs;

    u32_mode = pst_msg->u08_mode;
    u32_clk_cnt = pst_msg->u32_buf_size;
    u32_rd_cnt = u32_clk_cnt;
    if ((I2C_BUS_RA08_D16 == u32_mode) || (I2C_BUS_RA16_D16 == u32_mode)) {
        u32_rd_cnt = u32_rd_cnt << 1;
        u32_dat_16bit = 1;
    } else {
        u32_dat_16bit = 0;
    }

    s32_ret = i2c_tdl_wait_idle(u32_regbase, u32_tm_out);
    if (s32_ret != TS_OK)
        return s32_ret;

    disable_dev(u32_regbase);
    write_dev_addr(u32_regbase, pst_msg->u16_dev_addr);
    enable_dev(u32_regbase);

    /* 写入寄存器地址, 如果是 16bit 地址，则先发送高 8 bit */
    if (0 == pst_msg->u08_ignore_reg_addr) {
        u32_val = (ts_u32)pst_msg->u16_reg_addr;
        if ((I2C_BUS_RA16_D08 == u32_mode) || (I2C_BUS_RA16_D16 == u32_mode))
            write_cmd(u32_regbase, ((u32_val >> 8) & 0xFF));

        write_cmd(u32_regbase, ((u32_val >> 0) & 0xFF));
    }

    /* 在发送的过程中完成接收 */
    u32_rd_idx = 0;
    pu08_ptr = (ts_u08 *)pst_msg->pv_buf;
    u32_start = ts_cpl_get_tickcount();
    while (1) {
        if (0 == u32_rd_cnt)
            break;

        if (u32_clk_cnt > 0) {
            /* 如果数据位 16bit ，需先读高位 */
            if (u32_dat_16bit > 0)
                write_cmd(u32_regbase, I2C_BIT_CMD_READ);

            if (u32_clk_cnt > 1)
                write_cmd(u32_regbase, I2C_BIT_CMD_READ);
            else
                write_cmd(u32_regbase, I2C_BIT_CMD_READ | I2C_BIT_CMD_STOP);

            /* 时钟个数都按照一次 */
            u32_clk_cnt --;
        }

        /* u32_rd_cnt = 0 必然退出循环，此处无需检测是否读完， 直接读 fifo 个数 */
        u32_pre_cnt = readl(u32_regbase + I2C_TG_OFFSET_FLR_RX);
        if (u32_pre_cnt > 0) {
            if (u32_pre_cnt > u32_rd_cnt)
                u32_pre_cnt = u32_rd_cnt;
            
            for (i = 0; i < u32_pre_cnt; i++) {
                u32_val = readl(u32_regbase + I2C_TG_OFFSET_DAT_CMD);
                u08_val = (ts_u08)(u32_val & 0xFF);

                /* 如果是 16bit 数据，首先读到的是高 8 bit */
                if (u32_dat_16bit > 0) {
                    if (0 == (u32_rd_idx & 0x01)) {
                        //ts_cpl_printk("hh u08_val = %#x\n", u08_val);

                        pu08_ptr[u32_rd_idx + 1] = u08_val;
                    } else {
                        //ts_cpl_printk("ll u08_val = %#x\n", u08_val);

                        pu08_ptr[u32_rd_idx - 1] = u08_val;
                    }
                } else {
                    pu08_ptr[u32_rd_idx] = u08_val;
                }

                u32_rd_idx++;
            }

            u32_rd_cnt -= u32_pre_cnt;

            /* 重置超时计数 */
            u32_start = ts_cpl_get_tickcount();
        } else {
            u32_now = ts_cpl_get_tickcount();
            //ts_cpl_printk("start = %d, now = %d\n", u32_start, u32_now);
            if (u32_start > u32_now)
                u32_val = (0xFFFFFFFF - u32_start) + u32_now;
            else
                u32_val = u32_now - u32_start;

            if (u32_val > u32_tm_out)
                return -TS_ETIMEDOUT;
        }
    }

    return TS_OK;
}

//TS_RELOC_ITCM
ts_void i2c_tdl_hirq(ts_i2c_dev_s_t *pst_dev)
{
    ts_u32 u32_regbase, u32_val, u32_r_len, u32_frm_done;
    ts_i2c_slv_attr_s_t *pst_slv_attr;
    ts_u32 i, k, u32_dev_reg_addr, u32_dev_val;
    ts_u16 *pu16_ptr;
    ts_u08 *pu08_ptr;
    slv_req_s_t *pst_slv_req;
    slv_req_s_t *pst_slv_next_req;

    u32_regbase = pst_dev->u32_regbase;
    pst_slv_attr = (ts_i2c_slv_attr_s_t *)&pst_dev->st_slv_attr;
    u32_val = ts_cpl_readl(u32_regbase + I2C_TG_OFFSET_RAW_STAT);

    /* clear irq */
    ts_cpl_readl(u32_regbase + I2C_TG_OFFSET_CLR_INTR);

    /* 处理读请求 */
    if ((u32_val & (1 << I2C_BIT_INTRPT_RD_REQ)) > 0) {
        u32_r_len = ts_cpl_readl(u32_regbase + I2C_TG_OFFSET_FLR_RX);

        /* 从设备内部寄存器按照主机发送的个数读取，最大 16bit */
        if (u32_r_len > 0) {
            u32_dev_reg_addr = ts_cpl_readl(u32_regbase + I2C_TG_OFFSET_DAT_CMD);
            u32_r_len--;

            /* 只要前两位 */
            if (pst_slv_attr->u08_mode > I2C_BUS_RA08_D16) {
                if (u32_r_len > 0) {
                    u32_r_len--;
                    u32_dev_reg_addr = u32_dev_reg_addr << 8;
                    u32_dev_reg_addr |= ts_cpl_readl(u32_regbase + I2C_TG_OFFSET_DAT_CMD);
                }
            }

            /* 读空 */
            for (i = 0; i < u32_r_len; i++)
                u32_dev_val = ts_cpl_readl(u32_regbase + I2C_TG_OFFSET_DAT_CMD);

            pst_dev->u16_reg_addr = (ts_u16)u32_dev_reg_addr;
            pst_dev->u08_slv_remain = 0;
            pst_dev->u08_slv_val = 0;
            if (pst_dev->u16_reg_addr >= pst_slv_attr->u16_reg_size)
                pst_dev->u16_reg_addr = 0;
        }

        /* 准备主机要求的数据，按照配置的寄存器、数据位宽处理 */
        if ((pst_slv_attr->pv_reg_map != ts_null) && (pst_slv_attr->u16_reg_size > 0)) {
            if ((I2C_BUS_RA08_D08 == pst_slv_attr->u08_mode) || (I2C_BUS_RA16_D08 == pst_slv_attr->u08_mode)) {
                pu08_ptr = (ts_u08 *)pst_slv_attr->pv_reg_map;
                pu08_ptr += pst_dev->u16_reg_addr;
    
                u32_dev_val = *pu08_ptr;
                pst_dev->u08_slv_remain = 0;
            } else {
                if (0 == pst_dev->u08_slv_remain) {
                    pu16_ptr = (ts_u16 *)pst_slv_attr->pv_reg_map;
                    pu16_ptr += pst_dev->u16_reg_addr;

                    u32_dev_val = *pu16_ptr;
                    pst_dev->u08_slv_remain = 1;
                    pst_dev->u08_slv_val = (ts_u08)u32_dev_val;
                    u32_dev_val = u32_dev_val >> 8;
                } else {
                    pst_dev->u08_slv_remain = 0;
                    u32_dev_val = (ts_u32)pst_dev->u08_slv_val;
                }
            }

            ts_cpl_writel((u32_dev_val & 0xFF), (u32_regbase + I2C_TG_OFFSET_DAT_CMD));
            if (0 == pst_dev->u08_slv_remain) {
                pst_dev->u16_reg_addr ++;
                if (pst_dev->u16_reg_addr >= pst_slv_attr->u16_reg_size)
                    pst_dev->u16_reg_addr = 0;
            }
        }
    } else {
        /* 循环处理写操作 */
        for (k = 0; k < 8; k++) {
            pst_slv_req = pst_dev->pst_slv_req;
            pu08_ptr = pst_slv_req->pu08_buf;
            u32_frm_done = 0;

            /* 不关心读的 stop 发生，在 stop 产生后，仅去查看主机写情况，即查询 I2C_TG_OFFSET_FLR_RX 状况 */
            u32_r_len = ts_cpl_readl(u32_regbase + I2C_TG_OFFSET_FLR_RX);
            if (u32_r_len > 0) {
                for (i = 0; i < u32_r_len; i++) {
                    u32_dev_val = ts_cpl_readl(u32_regbase + I2C_TG_OFFSET_DAT_CMD);
                    pu08_ptr[pst_slv_req->u32_w_idx] = (ts_u08)u32_dev_val;

                    /* 如果 u32_size 和 u32_w_idx 不相等，就说明出现缓存溢出 */
                    pst_slv_req->u32_w_idx++;
                    pst_slv_req->u32_size++;
                    if (pst_slv_req->u32_w_idx >= I2C_SLV_BUF_MAX_SIZE) {
                        /* 完成本帧传输 */
                        u32_frm_done = 1;
                        pst_dev->u32_rdy_cnt++;

                        break;
                    }
                }
            }

            if ((u32_val & (1 << I2C_BIT_INTRPT_STOP)) > 0) {
                if (pst_dev->u32_stop_cnt == pst_dev->u32_rdy_cnt) {
                    /* 一个数据未接收满的帧到达 */
                    u32_frm_done = 1;
                } else {
                    /* 同步 rdy 和 stop 计数, 无需关注 u32_frm_done */
                    pst_dev->u32_stop_cnt++;
                }
            }

            if (u32_frm_done > 0) {
                pst_slv_next_req = (slv_req_s_t *)slv_get_free_node(pst_dev);

                if (pst_slv_next_req != ts_null) {
                    slv_put_rdy_node(pst_dev, (ts_void *)pst_slv_req);
                } else {
                    pst_slv_next_req = pst_slv_req;
                }
    
                pst_slv_next_req->u32_size = 0;
                pst_slv_next_req->u32_w_idx = 0;
                pst_dev->pst_slv_req = pst_slv_next_req;
    
                //ts_cpl_printk("np=%x\n", (ts_u32)pst_slv_next_req->pu08_buf);

                /* 先唤醒，中断中不会执行 down 同步 */
                ts_cpl_up((ts_cpl_semphr_s_t *)&pst_dev->st_slv_sema);
            }

            if (0 == u32_r_len)
                break;

            /* 重新更新中断状态 */
            u32_val = ts_cpl_readl(u32_regbase + I2C_TG_OFFSET_RAW_STAT);
            ts_cpl_readl(u32_regbase + I2C_TG_OFFSET_CLR_INTR);
        }
    }
}


ts_s32 i2c_tdl_read_slv(ts_i2c_dev_s_t *pst_dev, ts_i2c_msg_s_t *pst_msg)
{
    ts_ulong flags;
    ts_s32 s32_ret;
    ts_u32 u32_tmout_ms;
    slv_req_s_t *pst_slv_req;

    u32_tmout_ms = pst_msg->u32_tmout_ms;
    if (u32_tmout_ms < 10)
        u32_tmout_ms = 10;

    while (1) {
        s32_ret = ts_cpl_down_timeout((ts_cpl_semphr_s_t *)&pst_dev->st_slv_sema, u32_tmout_ms);

        ts_cpl_spin_lock_irqsave(&pst_dev->st_slv_lock, (ts_ulong *)&flags);
        pst_slv_req = slv_get_rdy_node(pst_dev);
        ts_cpl_spin_unlock_irqrestore(&pst_dev->st_slv_lock, (ts_ulong *)&flags);

        if (pst_slv_req != ts_null) {
            if (0 == pst_slv_req->u32_size) {
                slv_put_free_node(pst_dev, pst_slv_req);

                return 0;
            }

            //ts_cpl_printk("gp=%x,%d\n", pst_slv_req->pu08_buf, pst_slv_req->u32_size);

            ts_cpl_memcpy(pst_msg->pv_buf, pst_slv_req->pu08_buf, pst_slv_req->u32_size);

            ts_cpl_spin_lock_irqsave(&pst_dev->st_slv_lock, (ts_ulong *)&flags);

            slv_put_free_node(pst_dev, pst_slv_req);

            ts_cpl_spin_unlock_irqrestore(&pst_dev->st_slv_lock, (ts_ulong *)&flags);

            return pst_slv_req->u32_size;
        }

        if (TS_OK == s32_ret) {
            /* 正常唤醒，却没有数据，继续休眠 */
            ts_cpl_printk("i2c slv node is null\n");
        } else {
            ts_cpl_printk("i2c slv read tmout\n");

            return -TS_ETIMEDOUT;
        }
    }
}


ts_s32 i2c_tdl_start_slv_attr(ts_i2c_dev_s_t *pst_dev, ts_i2c_slv_attr_s_t *pst_slv_attr)
{
    ts_i2c_attr_s_t *pst_attr;
    ts_u32 i, u32_regbase, u32_val;
    ts_void *pv_req;
    ts_u08 *pu08_ptr;
    slv_req_s_t *pst_req;

    u32_regbase = pst_dev->u32_regbase;
    pst_attr = (ts_i2c_attr_s_t *)&pst_dev->st_attr;
    pst_attr->u08_master = 0;

    disable_dev(u32_regbase);
    ts_cpl_writel(pst_slv_attr->u08_dev_addr >> 1, (u32_regbase + I2C_TG_OFFSET_SAR));

    u32_val = ts_cpl_readl(u32_regbase + I2C_TG_OFFSET_CTRL);

#if 0
    u32_val |= 0x20;
    u32_val &= ~0x01;
#else
    u32_val = 0x20;
#endif

    ts_cpl_writel(u32_val, (u32_regbase + I2C_TG_OFFSET_CTRL));

    /* 阈值为 fifo/2 */
    ts_cpl_writel((ts_u32)pst_slv_attr->u08_rfl, (u32_regbase + I2C_TG_OFFSET_TL_RX));

    /* 保存配置 */
    ts_cpl_memcpy((ts_void *)&pst_dev->st_slv_attr, (ts_void *)pst_slv_attr, sizeof(ts_i2c_slv_attr_s_t));

    /* 设置中断并开启中断 */
    u32_val = ts_cpl_readl(u32_regbase + I2C_TG_OFFSET_INTR_MASK);

    /* 开启读请求 */
    u32_val |= (1 << I2C_BIT_INTRPT_RD_REQ);

    /* 开启 stop 检测 */
    u32_val |= (1 << I2C_BIT_INTRPT_STOP);

    u32_val |= (1 << I2C_BIT_INTRPT_RX_OVER);
    u32_val |= (1 << I2C_BIT_INTRPT_RX_FULL);

    ts_cpl_writel(u32_val, (u32_regbase + I2C_TG_OFFSET_INTR_MASK));

    if (ts_null == pst_dev->pv_slv_mem) {
        pst_dev->pv_slv_mem = ts_cpl_tmm_malloc(ts_null, (I2C_SLV_BUF_MAX_SIZE * I2C_SLV_NODE_NUM), 32, "i2c_slv_mem");
        if (ts_null == pst_dev->pv_slv_mem) {
            ts_cpl_printk("malloc for slv mem fail\n");

            return -TS_ENOMEM;
        }

        /* 申请节点信息 */
        pst_dev->pv_lslv = ts_cpl_kmalloc((sizeof(slv_req_s_t) * I2C_SLV_NODE_NUM), TS_CPL_GFP_KERNEL);
        if (ts_null == pst_dev->pv_lslv) {
            ts_cpl_printk("malloc for slv node fail\n");
            //ts_cpl_tmm_free(pst_dev->pv_slv_mem);

            pst_dev->pv_slv_mem = ts_null;

            return -TS_ENOMEM;
        }

        ts_cpl_init_list_head((ts_cpl_list_head_s_t *)&pst_dev->st_slv_free_hdr);
        ts_cpl_init_list_head((ts_cpl_list_head_s_t *)&pst_dev->st_slv_rdy_hdr);

        /* 分配缓存 */
        pu08_ptr = (ts_u08 *)pst_dev->pv_slv_mem;
        pst_req = (slv_req_s_t *)pst_dev->pv_lslv;
        for (i = 0; i < I2C_SLV_NODE_NUM; i++) {
            ts_cpl_init_list_head((ts_cpl_list_head_s_t *)&pst_req->st_list);
            pst_req->pu08_buf = pu08_ptr;
            pst_req->u32_size = 0;
            pst_req->u32_w_idx = 0;

            slv_put_free_node(pst_dev, pst_req);

            pu08_ptr += I2C_SLV_BUF_MAX_SIZE;
            pst_req++;
        }
    } else {
        ts_cpl_printk("reinit\n");
    }

    /* 取出可能未被用户取走的节点 */
    for (i = 0; i < I2C_SLV_NODE_NUM; i++) {
        pv_req = (slv_req_s_t *)slv_get_rdy_node(pst_dev);
        if (ts_null == pv_req)
            break;

        slv_put_free_node(pst_dev, pv_req);
    }

    /* 获取一个空节点，用于接收 */
    pv_req = slv_get_free_node(pst_dev);
    if (ts_null == pv_req) {
        ts_cpl_printk("get free node fail\n");

        return -TS_ENOMEM;
    }

    pst_dev->pst_slv_req = (slv_req_s_t *)pv_req;
    pst_dev->pst_slv_req->u32_size = 0;
    pst_dev->pst_slv_req->u32_w_idx = 0;

    pst_dev->u32_rdy_cnt = 0;
    pst_dev->u32_stop_cnt = 0;

    /* 使能 i2c slave */
    enable_dev(u32_regbase);

    return TS_OK;
}


/* 
 * 函数名 :  i2c_tdl_init
 * 描  述 :  驱动层按照默认配置初始化。
 * 输  入 :  参数               描述
 *           pst_dev           i2c 设备
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           无                 无
 */
ts_void i2c_tdl_init(ts_i2c_dev_s_t *pst_dev)
{
    ts_i2c_attr_s_t *pst_attr;

    pst_attr = (ts_i2c_attr_s_t *)&pst_dev->st_attr;
    pst_attr->u08_duty = 48;
    pst_attr->u08_duty_adj = 0;
    pst_attr->u08_master = 1;
    pst_attr->u08_roll_thr = 32;
    pst_attr->u08_sda_hold_us = 0;
    pst_attr->u08_sda_hold_clk = 10;
    pst_attr->u08_sda_setup_us = 0;
    pst_attr->u08_time_out_jfs = 20;
    pst_attr->u32_bus_hz = 100000;

    set_clk(pst_dev->u32_regbase, pst_dev->u32_clk, pst_attr);

    writel(0x65, (pst_dev->u32_regbase + I2C_TG_OFFSET_CTRL));
    
    /* disable all interrupt */
    writel(0, (pst_dev->u32_regbase + I2C_TG_OFFSET_INTR_MASK));

    pst_dev->u16_reg_addr = 0;
    pst_dev->u08_slv_remain = 0;
    pst_dev->u08_slv_val = 0;

    ts_cpl_spinlock_init((ts_cpl_spinlock_s_t *)&pst_dev->st_slv_lock);
    ts_cpl_semphr_init((ts_cpl_semphr_s_t *)&pst_dev->st_slv_sema, 0);

    pst_dev->pst_slv_req = ts_null;
    pst_dev->pv_slv_mem = ts_null;
    pst_dev->pv_lslv = ts_null;
}


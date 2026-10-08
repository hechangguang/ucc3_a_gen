/* 
 * uart_v100.c -- uart v100 实现多串口设备管理
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2024 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#include <config.h>
#include <base/ts_type.h>
#include <base/ts_comm.h>
#include <cpl/ts_cpl.h>
#include <linux/fs.h>
#include <uapi/chip_def.h>
#include "kapi/k_console_v100.h"
#include "kapi/k_uart_v100.h"

/* 各个串口的接收缓冲有模块参数确定 */
ts_s32 uart0_rxsz = 256;
ts_s32 uart1_rxsz = 256;
ts_s32 uart2_rxsz = 256;
ts_s32 uart3_rxsz = 256;
ts_s32 uart4_rxsz = 256;

/* 各个串口接收超时参数 */
ts_s32 uart0_tm = -1;
ts_s32 uart1_tm = 1000;
ts_s32 uart2_tm = 1000;
ts_s32 uart3_tm = 1000;
ts_s32 uart4_tm = 1000;

#ifdef TS_UART_MAX_NUM
#define UART_MAX_DEV_NUM        TS_UART_MAX_NUM
#else
#define UART_MAX_DEV_NUM        (5)
#endif

static ts_cpl_dev_s_t *apst_cpl_devs[UART_MAX_DEV_NUM];


/* 仅开启了接收中断 */
static ts_cpl_irqreturn_en_t uart_irq_handler(ts_s32 s32_irq_no, ts_void *pv_dev)
{
    uart_dev_s_t *pst_uart_dev;

    pst_uart_dev = (uart_dev_s_t *)pv_dev;

    return pst_uart_dev->pst_ops->hirq(pst_uart_dev);
}


static ts_s32 uart_open(ts_void *pv_inode, ts_void *pv_file)
{
    struct inode *i_node;
    uart_dev_s_t *pst_uart_dev;

    i_node = (struct inode *)pv_inode;
    if (i_node->i_refs > 0) {
        tsp_log_err("%s: %s\n", i_node->i_name, TS_EBUSY_STR);

        return -TS_EBUSY;
    }

    i_node->i_refs++;
    pst_uart_dev = (uart_dev_s_t *)ts_cpl_get_dev_priv(pv_file);
    if (pst_uart_dev->pst_ops->setup != ts_null)
        pst_uart_dev->pst_ops->setup(pst_uart_dev);

    return TS_OK;
}


static ts_s32 uart_close(ts_void *pv_inode, ts_void *pv_file)
{
    struct inode *i_node;
    uart_dev_s_t *pst_uart_dev;

    i_node = (struct inode *)pv_inode;
    if (i_node->i_refs > 0) {
        i_node->i_refs--;

        pst_uart_dev = (uart_dev_s_t *)ts_cpl_get_dev_priv(pv_file);
        if (pst_uart_dev->pst_ops->close != ts_null)
            pst_uart_dev->pst_ops->close(pst_uart_dev);
    }

    return TS_OK;
}


static ts_long uart_ioctl(ts_void *pv_file, ts_s32 u32_cmd, ts_ulong ulg_arg)
{
    uart_dev_s_t *pst_uart_dev;
    ts_u32 u32_ioctl_nr;
    ts_long lg_ret;

    pst_uart_dev = (uart_dev_s_t *)ts_cpl_get_dev_priv((ts_void *)pv_file);
    if (ts_null == pst_uart_dev)
        return -TS_ENODEV;

    lg_ret = TS_OK;
    u32_ioctl_nr = _TS_CPL_IOC_NR(u32_cmd);
    switch (u32_ioctl_nr) {
        case IOC_NR_UART_HBP_SET_ATTR:
            /* 不锁是变量改变，不会出现致命错误，仅会丢失或者多处理数据 */
            ts_cpl_copy_from_user((ts_void *)&pst_uart_dev->st_hbp, (const ts_void *)ulg_arg, sizeof(uart_proto_s_t));

            /* 属性设置不频繁, 一般在程序运行前设置, 不影响数据处理 */
            pst_uart_dev->u32_proto_step = 0;

            break;

        case IOC_NR_UART_HBP_GET_ATTR:
            ts_cpl_copy_to_user((ts_void *)ulg_arg, (const ts_void *)&pst_uart_dev->st_hbp, sizeof(uart_proto_s_t));

            break;

        case IOC_NR_UART_HBP_SET_EN:
            pst_uart_dev->st_hbp.u32_en = (ts_u32)ulg_arg;
            pst_uart_dev->u32_proto_step = 0;

            break;

        case IOC_NR_UART_HIK_SET_ATTR:
            ts_cpl_copy_from_user((ts_void *)&pst_uart_dev->st_hikp, (const ts_void *)ulg_arg, sizeof(uart_proto_s_t));
            pst_uart_dev->u32_proto_step = 0;

            break;

        case IOC_NR_UART_HIK_GET_ATTR:
            ts_cpl_copy_to_user((ts_void *)ulg_arg, (const ts_void *)&pst_uart_dev->st_hikp, sizeof(uart_proto_s_t));

            break;

        case IOC_NR_UART_HIK_SET_EN:
            pst_uart_dev->st_hikp.u32_en = (ts_u32)ulg_arg;
            pst_uart_dev->u32_proto_step = 0;

            break;

        case IOC_NR_UART_SET_BAUDR:
            pst_uart_dev->pst_ops->set_speed(pst_uart_dev, (ts_u32)ulg_arg);

            break;

        case IOC_NR_UART_GET_ATTR:
            ts_cpl_copy_to_user((ts_void *)ulg_arg, (const ts_void *)&pst_uart_dev->st_attr, sizeof(uart_attr_s_t));

            break;

        case IOC_NR_UART_SET_ATTR:
            lg_ret = (ts_long)uart_set_attr(pst_uart_dev, (uart_attr_s_t *)ulg_arg);

            break;

        default:
            lg_ret = -TS_ENOTSUP;

            break;
    }

    return lg_ret;
}


static ts_ssize_t uart_read(ts_void *filep, char *buffer, ts_size_t buflen, ts_loff_t *pos)
{
    ts_u32 i, u32_chkc, u32_tmout;
    ts_ssize_t ssz_cnt;
    ts_u08 u08_val;
    uart_dev_s_t *pst_uart_dev;

    pst_uart_dev = ts_cpl_get_dev_priv((ts_void *)filep);
    if (ts_null == pst_uart_dev)
        return -1;

    ssz_cnt = 0;
    for (i = 0; i < buflen; i++) {
        /* 等待数据到达 */
        u32_tmout = 0;
        while (1) {
            u32_chkc = pst_uart_dev->pst_ops->chkc(pst_uart_dev);
            if (u32_chkc > 0)
                break;

            if (pst_uart_dev->u32_rx_tmout_ms > 0) {
                ts_cpl_msleep(10);
                u32_tmout += 10;
                if (u32_tmout >= pst_uart_dev->u32_rx_tmout_ms)
                    return ssz_cnt;
            } else {
                return ssz_cnt;
            }
        }

        u08_val = pst_uart_dev->pst_ops->getc(pst_uart_dev);
        buffer[i] = u08_val;

        ssz_cnt++;
    }

    return ssz_cnt;
}


static ts_ssize_t uart_write(ts_void *filep, const char *buffer, ts_size_t buflen, ts_loff_t *pos)
{
    ts_u32 i;
    ts_ssize_t ssz_cnt;
    ts_u08 u08_val;
    uart_dev_s_t *pst_uart_dev;
    ts_s32 s32_ret;

    pst_uart_dev = ts_cpl_get_dev_priv((ts_void *)filep);
    if (ts_null == pst_uart_dev)
        return -1;

    ssz_cnt = 0;
    for (i = 0; i < buflen; i++) {
        u08_val = (ts_u08)buffer[i];
        s32_ret = pst_uart_dev->pst_ops->putc(pst_uart_dev, u08_val);
        if (s32_ret != TS_OK)
            break;

        ssz_cnt++;
    }

    return ssz_cnt;
}


static ts_cpl_fileops_s_t sst_uart_ops = {
    .open = uart_open,
    .release = uart_close,
    .read = uart_read,
    .write = uart_write,
    .unlocked_ioctl = uart_ioctl,
};


static ts_s32 uart_probe(ts_cpl_platdev_s_t *pst_drv)
{
    ts_u32 i;
    ts_cpl_res_s_t st_res;
    ts_s32 s32_irq_no;
    ts_s32 s32_ret;
    ts_cpl_dev_s_t *pst_cpl_dev;
    uart_dev_s_t *pst_uart_dev;
    ts_char ac_name[TS_CPL_MAX_PATH_LEN + 1];
    ts_u32 au32_rxsz[5], au32_tmout[5];

    au32_rxsz[0] = (ts_u32)uart0_rxsz;
    au32_rxsz[1] = (ts_u32)uart1_rxsz;
    au32_rxsz[2] = (ts_u32)uart2_rxsz;
    au32_rxsz[3] = (ts_u32)uart3_rxsz;
    au32_rxsz[4] = (ts_u32)uart4_rxsz;

    au32_tmout[0] = (ts_u32)uart0_tm;
    au32_tmout[1] = (ts_u32)uart1_tm;
    au32_tmout[2] = (ts_u32)uart2_tm;
    au32_tmout[3] = (ts_u32)uart3_tm;
    au32_tmout[4] = (ts_u32)uart4_tm;

    for (i = 0; i < TS_UART_MAX_NUM; i++) {
        ts_cpl_sprintf((ts_char *)&ac_name, "/dev/ttyS%d", i);

        s32_ret = ts_cpl_plat_get_reg(pst_drv, (ts_cpl_res_s_t *)&st_res, i);
        if (s32_ret != TS_OK) {
            tsp_log_err("Failed to get regbase for %s\n", (ts_char *)&ac_name);

            break;
        }

        s32_irq_no = ts_cpl_plat_get_irq(pst_drv, i);
        if (s32_irq_no < 0) {
            tsp_log_err("Failed to get irq num for %s\n", (ts_char *)&ac_name);

            break;
        }

        pst_cpl_dev = ts_cpl_dev_register((ts_char *)&ac_name, (ts_cpl_fileops_s_t *)&sst_uart_ops, sizeof(uart_dev_s_t));
        if (ts_null == pst_cpl_dev) {
            tsp_log_err("Failed to register dev for %s\n", (ts_char *)&ac_name);

            break;
        }

        apst_cpl_devs[i] = pst_cpl_dev;
        pst_uart_dev = (uart_dev_s_t *)pst_cpl_dev->pv_private;

        pst_uart_dev->u32_regbase = (ts_u32)ts_cpl_ioremap(st_res.start, st_res.size);
        pst_uart_dev->s32_irq_no = s32_irq_no;
        s32_ret = ts_cpl_request_irq(s32_irq_no, uart_irq_handler, 0, pst_cpl_dev->pc_name + 5, pst_uart_dev);
        if (s32_ret != TS_OK) {
            tsp_log_err("Failed to register irq for %s\n", (ts_char *)&ac_name);

            break;
        }

        pst_uart_dev->u32_rx_tmout_ms = au32_tmout[i];
        tdl_uart_init(pst_uart_dev, au32_rxsz[i]);
    }

    return TS_OK;
}


static ts_cpl_of_devid_s_t sst_uart_drv_id = {
    .compatible = "tg,serial_v100",
};

static ts_cpl_plat_drv_s_t sst_uart_drv = {
    .probe = uart_probe,
    .driver = {
        .name = "uart",
        .of_match_table = &sst_uart_drv_id,
    }
};


ts_s32 uart_init(ts_void)
{
    return ts_cpl_plat_driver_register((ts_cpl_plat_drv_s_t *)&sst_uart_drv);
}

TS_FUNC_UNUSED static ts_void uart_exit(ts_void)
{

}

ts_cpl_ins_module_init(uart, uart_init);
ts_cpl_module_exit(uart_exit);

ts_cpl_ins_module_param(uart, uart0_rxsz, int, 0666);
ts_cpl_ins_module_param(uart, uart1_rxsz, int, 0666);
ts_cpl_ins_module_param(uart, uart2_rxsz, int, 0666);
ts_cpl_ins_module_param(uart, uart3_rxsz, int, 0666);
ts_cpl_ins_module_param(uart, uart4_rxsz, int, 0666);

ts_cpl_ins_module_param(uart, uart0_tm, int, 0666);
ts_cpl_ins_module_param(uart, uart1_tm, int, 0666);
ts_cpl_ins_module_param(uart, uart2_tm, int, 0666);
ts_cpl_ins_module_param(uart, uart3_tm, int, 0666);
ts_cpl_ins_module_param(uart, uart4_tm, int, 0666);


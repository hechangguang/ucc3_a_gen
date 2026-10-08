/* 
 * timer_v100.c -- timer_v100 驱动实现
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
#include <base/ts_compiler.h>
#include <cpl/ts_cpl.h>
#include <linux/fs.h>
#include <uapi/chip_def.h>
#include <kapi/k_timer_v100.h>
#include <uapi/timer_v100.h>


static ts_cpl_irqreturn_en_t hwt_handler(ts_s32 s32_irq_no, ts_void *pv_dev)
{
    hwt_ctx_s_t *pst_ctx;
    hwt_sub_dev_s_t *pst_sub_ctx;
    hwt_reg_s_t *pst_reg;
    volatile ts_u32 u32_eoi;
    ts_u32 i;

    TS_UNUSED(u32_eoi);

    pst_ctx = (hwt_ctx_s_t *)pv_dev;
    for (i = 0; i < HWT_MAX_NUM; ++i)  {
        pst_sub_ctx = &pst_ctx->ast_sub_dev[i];
        pst_reg = (hwt_reg_s_t *)pst_sub_ctx->u32_reg;

        if (pst_reg->un_intrpt.status == 1) {
            /* 读清中断 */
            u32_eoi = pst_reg->un_eoi.u32_val;

            /* 关闭定时器 */
            pst_reg->un_ctrl.u32_val = 0;

            if (pst_sub_ctx->fp_cb != ts_null)
                pst_sub_ctx->fp_cb(pst_sub_ctx->pv_param);
        }
    }

    return TS_CPL_IRQ_HANDLED;
}

static ts_s32 timer_v100_open(ts_void *pv_inode, ts_void *pv_file)
{
    struct inode *i_node;

    i_node = (struct inode *)pv_inode;
    if (i_node->i_refs > 0) {
        tsp_log_err("%s: %s\n", i_node->i_name, TS_EBUSY_STR);

        return -TS_EBUSY;
    }

    i_node->i_refs++;

    return TS_OK;
}


static ts_s32 timer_v100_close(ts_void *pv_inode, ts_void *pv_file)
{
    struct inode *i_node;

    i_node = (struct inode *)pv_inode;
    if (i_node->i_refs > 0)
        i_node->i_refs--;

    return TS_OK;
}


static ts_long timer_v100_ioctl(ts_void *pv_file, ts_s32 u32_cmd, ts_ulong ulg_arg)
{
    ts_cpl_dev_s_t *pst_cpl_dev;
    ts_u32 u32_ioctl_nr, *pu32_ctx;
    ts_long lg_ret;

    pst_cpl_dev = ts_cpl_get_dev(pv_file);
    if ((ts_null == pst_cpl_dev) || (ts_null == pst_cpl_dev->pv_private))
        return TS_ERR;

#ifdef CONFIG_HAVE_USER_MODE
    /* 内核态 用户态 数据拷贝 */
#endif

    lg_ret = TS_OK;
    u32_ioctl_nr = _TS_CPL_IOC_NR(u32_cmd);
    switch (u32_ioctl_nr) {
        case IOC_NR_TIMER_INIT:
            pu32_ctx = (ts_u32 *)ulg_arg;
            if (pu32_ctx != ts_null)
                *pu32_ctx = (ts_u32)pst_cpl_dev->pv_private;
            else
                lg_ret = -TS_EINVAL;

            break;
        case IOC_NR_TIMER_DEINIT:

            break;

        default :
            lg_ret = TS_ERR;

            break;
    }

    return lg_ret;
}


static ts_cpl_fileops_s_t sst_timer_v100_ops = {
    .open = timer_v100_open,
    .release = timer_v100_close,
    .read = ts_null,
    .write = ts_null,
    .unlocked_ioctl = timer_v100_ioctl,
};



#ifndef CONFIG_SUPPORT_PROCFS

ts_s32 timer_tdl_proc_show(ts_void *s, ts_void *v)
{
    ts_u32 i;
    ts_timer_dev_s_t *pst_timer_dev;
    ts_cpl_dev_s_t *pst_cpl_dev;

    ts_cpl_seq_printf(s, "index  regbase  irqno  period    cb          param        name\n");
    for (i = 0; i < TIMER_TG_MAX_SUB_DEV; i++) {
        pst_cpl_dev = apst_cpl_devs[i];
        if (ts_null == pst_cpl_dev)
            break;

        pst_timer_dev =  pst_cpl_dev->pv_private;
        if (ts_null == pst_timer_dev)
            break;

        pst_timer_dev->ac_name[TIMER_TG_MAX_SUB_DEV] = '\0';
        ts_cpl_seq_printf(s, "%d      %08x  %02d    %08d  %08x    %08x     %s\n", i, pst_timer_dev->u32_regbase, \
                pst_timer_dev->u32_irq_no, pst_timer_dev->u32_period_us, (ts_u32)pst_timer_dev->fp_callback, \
                (ts_u32)pst_timer_dev->pv_cb_param, (ts_char *)&pst_timer_dev->ac_name);
    }

    return 0;
}


static ts_cpl_seq_ops_s_t sst_timer_seq_ops = {
    .show = timer_tdl_proc_show,
};

static ts_s32 timer_proc_open(ts_void *inode, ts_void *file)
{
    return ts_cpl_seq_open(file, &sst_timer_seq_ops);
}


static ts_cpl_fileops_s_t sst_timer_proc_fops = {
    .open = timer_proc_open,
};

static ts_cpl_proc_s_t sst_proc = {
    .pst_fops = &sst_timer_proc_fops,
};


static ts_void timer_proc_init(ts_void)
{
    ts_cpl_create_proc(&sst_proc, "/proc/timer");
}
#endif



static ts_s32 timer_tdl_probe(ts_cpl_platdev_s_t *pst_drv)
{
    ts_u32 u32_regbase;
    ts_char ac_name[TS_CPL_MAX_PATH_LEN + 1];
    ts_cpl_res_s_t st_res;
    ts_s32 i, s32_ret, s32_clk_hz,s32_irq_no;
    ts_cpl_dev_s_t *pst_cpl_dev;
    hwt_ctx_s_t *pst_ctx;
    hwt_sub_dev_s_t *pst_sub_ctx;

    ts_cpl_sprintf((ts_char *)&ac_name, "/dev/hwt");

    s32_ret = ts_cpl_plat_get_reg(pst_drv, (ts_cpl_res_s_t *)&st_res, 0);
    if (s32_ret != TS_OK) {
        tsp_log_err("Failed to get regbase for %s\n", (ts_char *)&ac_name);

        return s32_ret;
    }

    u32_regbase = (ts_u32)ts_cpl_ioremap(st_res.start, st_res.size);

    /* 按照输入的时钟，提早计算 us 的时钟周期 */
    s32_clk_hz = ts_cpl_plat_get_clocks(pst_drv, 0);

    pst_cpl_dev = ts_cpl_dev_register((ts_char *)&ac_name,
                                        (ts_cpl_fileops_s_t *)&sst_timer_v100_ops, sizeof(hwt_ctx_s_t));
    if (ts_null == pst_cpl_dev) {
        tsp_log_err("Failed to register dev for %s\n", (ts_char *)&ac_name);

        return TS_ERR;
    }

    pst_ctx = (hwt_ctx_s_t *)pst_cpl_dev->pv_private;

    pst_ctx->u32_us_clk = s32_clk_hz / 1000000;

    s32_irq_no = (ts_u32)ts_cpl_plat_get_irq(pst_drv, 0);

    ts_cpl_mutex_init((ts_cpl_mutex_s_t *)&pst_ctx->st_mutex);

    s32_ret = ts_cpl_request_irq(s32_irq_no, hwt_handler, 0, "hwt", pst_ctx);
    if (s32_ret != TS_OK)
        return TS_ERR;

    for(i = 0; i < HWT_MAX_NUM; ++i) {
        pst_sub_ctx = &pst_ctx->ast_sub_dev[i];

        /* 子设备寄存器基地址 */
        pst_sub_ctx->u32_reg = u32_regbase;

        ts_cpl_spinlock_init((ts_cpl_spinlock_s_t *)&pst_sub_ctx->st_lock);

        /* 子设备初始化为空闲状态 */
        pst_sub_ctx->u32_busy = 0;
        pst_sub_ctx->fp_cb = ts_null;
        pst_sub_ctx->pv_param = ts_null;

        u32_regbase += sizeof(hwt_reg_s_t);
    }

    #ifndef CONFIG_SUPPORT_PROCFS
    timer_proc_init();
    #endif

    return TS_OK;
}


static ts_cpl_of_devid_s_t sst_timer_drv_id = {
    .compatible = "tg,timer_v100",
};


static ts_cpl_plat_drv_s_t sst_timer_drv = {
    .probe = timer_tdl_probe,
    .driver = {
        .name = "timer",
        .of_match_table = &sst_timer_drv_id,
    }
};




#ifdef CONFIG_PROC_FS
timer_sub_dev_s_t * timer_tdl_get_subdev(ts_u32 u32_index)
{
    if (u32_index >= TIMER_TG_MAX_SUB_DEV)
        return ts_null;
    else
        return (timer_sub_dev_s_t *)&sst_timer_priv.ast_sub_dev[u32_index];
}
#endif

ts_s32 timer_init(ts_void)
{
    return ts_cpl_plat_driver_register((ts_cpl_plat_drv_s_t *)&sst_timer_drv);
}

TS_FUNC_UNUSED static ts_void uart_exit(ts_void)
{

}

ts_cpl_ins_module_init(timer, timer_init);
ts_cpl_module_exit(timer_exit);


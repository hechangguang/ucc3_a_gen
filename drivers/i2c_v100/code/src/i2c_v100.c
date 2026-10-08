/* 
 * i2c_v100.c -- i2c v100 实现, 对外接口是标准设备驱动，内部使用固定接口调用，避免过多函数指针操作
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
#include <uapi/chip_def.h>
#include <cpl/ts_cpl.h>
#include <kapi/k_i2c_v100.h>

static ts_s32 i2c_v100_open(ts_void *pv_inode, ts_void *pv_file)
{
    ts_cpl_dev_s_t *pst_dev;
    ts_i2c_dev_s_t *pst_i2c_dev;

    pst_dev = ts_cpl_get_dev(pv_file);
    if ((ts_null == pst_dev) || (ts_null == pst_dev->pv_private))
        return TS_ERR;

    /* 按照默认值初始化 */
    pst_i2c_dev = pst_dev->pv_private;
    i2c_tdl_init(pst_i2c_dev);

    return TS_OK;
}


static ts_s32 i2c_v100_close(ts_void *pv_inode, ts_void *pv_file)
{
    return TS_OK;
}


static ts_long i2c_v100_ioctl(ts_void *pv_file, ts_s32 u32_cmd, ts_ulong ulg_arg)
{
    ts_cpl_dev_s_t *pst_dev;
    ts_i2c_dev_s_t *pst_i2c_dev;
    ts_i2c_msg_s_t *pst_msg;
    ts_long lg_ret;

    pst_dev = ts_cpl_get_dev(pv_file);
    if ((ts_null == pst_dev) || (ts_null == pst_dev->pv_private))
        return TS_ERR;

    pst_i2c_dev = pst_dev->pv_private;

#ifdef CONFIG_HAVE_USER_MODE
    /* 内核态 用户态 数据拷贝 */
#endif

    lg_ret = TS_OK;
    switch (u32_cmd) {
    case TS_I2C_IOCTL_SET_ATTR:
        lg_ret = (ts_long)i2c_tdl_set_attr(pst_i2c_dev, (const ts_i2c_attr_s_t *)ulg_arg);
        break;

    case TS_I2C_IOCTL_GET_ATTR:
        lg_ret = (ts_long)i2c_tdl_get_attr(pst_i2c_dev, (ts_i2c_attr_s_t *)ulg_arg);
        break;

    case TS_I2C_IOCTL_READ:
        pst_msg = (ts_i2c_msg_s_t *)ulg_arg;
        lg_ret = (ts_long)i2c_tdl_read(pst_i2c_dev, pst_msg);

        break;

    case TS_I2C_IOCTL_WRITE:
        pst_msg = (ts_i2c_msg_s_t *)ulg_arg;
        lg_ret = (ts_long)i2c_tdl_write(pst_i2c_dev, pst_msg);

        break;
    
    case TS_I2C_IOCTL_START_SLV:
        lg_ret = (ts_long)i2c_tdl_start_slv_attr(pst_i2c_dev, (ts_i2c_slv_attr_s_t *)ulg_arg);

        break;

    case TS_I2C_IOCTL_READ_SLV:
        lg_ret = (ts_long)i2c_tdl_read_slv(pst_i2c_dev, (ts_i2c_msg_s_t *)ulg_arg);

        break;
    
    default :
        lg_ret = TS_ERR;

        break;

    }

    return lg_ret;
}

/* 仅开启了接收中断 */
static ts_cpl_irqreturn_en_t i2c_irq_handler(ts_s32 s32_irq_no, ts_void *pv_dev)
{
    i2c_tdl_hirq((ts_i2c_dev_s_t *)pv_dev);

    return TS_CPL_IRQ_HANDLED;
}

static ts_cpl_fileops_s_t sst_i2c_v100_ops = {
    .open = i2c_v100_open,
    .release = i2c_v100_close,
    .read = ts_null,
    .write = ts_null,
    .unlocked_ioctl = i2c_v100_ioctl,
};

static ts_s32 i2c_v100_probe(ts_cpl_platdev_s_t *pst_drv)
{
    ts_u32 i, u32_fifo_depth, u32_clk;
    ts_cpl_res_s_t st_res;
    ts_cpl_dev_s_t *pst_cpl_dev;
    ts_i2c_dev_s_t *pst_i2c_dev;
    ts_s32 s32_irq_no;
    ts_char ac_name[16];

    u32_fifo_depth = ts_cpl_plat_get_value(pst_drv, "fifo-depth", 0, 16);
    u32_clk = ts_cpl_plat_get_clocks(pst_drv, 0);
    if ((0 == u32_fifo_depth) || (0 == u32_clk)) {
        tsp_log_err("Failed to get fifodep or clk \n");

        return TS_ERR;
    }

    for (i = 0; i < TS_I2C_MAX_NUM; i++) {
        ts_cpl_sprintf((ts_char *)&ac_name, "/dev/i2c%d", i);

        /* 不判断 dts 会出错以及注册设备会出错 */
        ts_cpl_plat_get_reg(pst_drv, (ts_cpl_res_s_t *)&st_res, i);
        s32_irq_no = ts_cpl_plat_get_irq(pst_drv, i);
        pst_cpl_dev = ts_cpl_dev_register((ts_char *)&ac_name, (ts_cpl_fileops_s_t *)&sst_i2c_v100_ops, sizeof(ts_i2c_dev_s_t));
        if (ts_null == pst_cpl_dev) {
            tsp_log_err("Failed to register dev for %s\n", (ts_char *)&ac_name);

            break;
        }

        pst_i2c_dev = (ts_i2c_dev_s_t *)pst_cpl_dev->pv_private;
        pst_i2c_dev->u16_id = (ts_u16)i;
        pst_i2c_dev->u32_regbase = (ts_u32)ts_cpl_ioremap(st_res.start, st_res.size);
        pst_i2c_dev->u16_fifo_depth = (ts_u16)u32_fifo_depth;
        pst_i2c_dev->u32_clk = u32_clk;

        ts_cpl_request_irq(s32_irq_no, i2c_irq_handler, 0, pst_cpl_dev->pc_name + 5, pst_i2c_dev);

        ts_cpl_printk("found %s@%#x\n", (ts_char *)&ac_name, pst_i2c_dev->u32_regbase);
    }

    return TS_OK;
}

static ts_cpl_of_devid_s_t sst_i2c_drv_id = {
    .compatible = "tg,i2c-v100",
};

static ts_cpl_plat_drv_s_t sst_i2c_drv = {
    .probe = i2c_v100_probe,
    .driver = {
        .name = "i2c",
        .of_match_table = &sst_i2c_drv_id,
    }
};

ts_s32 i2c_init(ts_void)
{
    ts_s32 s32_ret;

    s32_ret = ts_cpl_plat_driver_register((ts_cpl_plat_drv_s_t *)&sst_i2c_drv);

    return s32_ret;
}

ts_cpl_ins_module_init(i2c, i2c_init);

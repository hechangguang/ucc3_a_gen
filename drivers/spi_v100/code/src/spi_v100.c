/* 
 * spi_v100.c -- spi_v100 实现, 对外接口是标准设备驱动，内部使用固定接口调用，避免过多函数指针操作
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
#include <cpl/cpl_device.h>
#include <kapi/k_base_v100.h>
#include <kapi/k_spimst_v100.h>
#include <uapi/spi_v100.h>

static ts_u32 su32_spi_clk = 24000000;

static ts_s32 spi_v100_open(ts_void *pv_inode, ts_void *pv_file)
{
    return TS_OK;
}


static ts_s32 spi_v100_close(ts_void *pv_inode, ts_void *pv_file)
{
    return TS_OK;
}


static ts_long spi_v100_ioctl(ts_void *pv_file, ts_s32 u32_cmd, ts_ulong ulg_arg)
{
    ts_long lg_ret;
    ts_cpl_dev_s_t *pst_dev;
    spi_dev_s_t *pst_spi;
    ts_spi_msg_s_t *pst_msg;
    
    pst_dev = ts_cpl_get_dev(pv_file);
    if ((ts_null == pst_dev) || (ts_null == pst_dev->pv_private))
        return TS_ERR;
    
    pst_spi = pst_dev->pv_private;
    lg_ret = TS_OK;

#ifdef CONFIG_HAVE_USER_MODE
    /* 内核态 用户态 数据拷贝 */
#endif

    switch (u32_cmd) {
        case TS_SPI_IOCTL_SET_ATTR: {
            ts_cpl_memcpy((ts_void *)&pst_spi->st_attr, (ts_void *)ulg_arg, sizeof(ts_spi_attr_s_t));

            break;
        }

        case TS_SPI_IOCTL_GET_ATTR: {
            ts_cpl_memcpy((ts_void *)ulg_arg, (ts_void *)&pst_spi->st_attr, sizeof(ts_spi_attr_s_t));

            break;
        }

        case TS_SPI_IOCTL_XFER: {
            pst_msg = (ts_spi_msg_s_t *)ulg_arg;
            lg_ret = (ts_long)spi_tdl_xfer(pst_spi, pst_msg);
            break;
        }

        default : {
            lg_ret = TS_ERR;
        }

    }

	return lg_ret;
}

static ts_cpl_fileops_s_t sst_spi_v100_ops = {
    .open = spi_v100_open,
    .release = spi_v100_close,
    .read = ts_null,
    .write = ts_null,
    .unlocked_ioctl = spi_v100_ioctl,
};


static ts_s32 spimst_v100_probe(ts_cpl_platdev_s_t *pst_drv)
{
    ts_cpl_dev_s_t *pst_cpl_dev;
    spi_dev_s_t *pst_spi_dev;
    ts_s32 s32_ret, s32_irq_no;
    ts_u32 u32_regbase;
    ts_cpl_res_s_t st_res;
    ts_spi_attr_s_t *pst_attr;

    pst_cpl_dev = ts_cpl_dev_register("/dev/spimst", (ts_cpl_fileops_s_t *)&sst_spi_v100_ops, sizeof(spi_dev_s_t));
    if (ts_null == pst_cpl_dev) {
        ts_kapi_log_err("Failed to register dev for spi\n");

        return -TS_ENOMEM;
    }

    pst_spi_dev = (spi_dev_s_t *)pst_cpl_dev->pv_private;

    s32_ret = ts_cpl_plat_get_reg(pst_drv, (ts_cpl_res_s_t *)&st_res, 0);
    if (s32_ret != TS_OK)
        goto _probe_err;

    u32_regbase = (ts_u32)ts_cpl_ioremap(st_res.start, st_res.size);

    s32_irq_no = ts_cpl_plat_get_irq(pst_drv, 0);
    if (s32_irq_no < 0) {
        s32_ret = -TS_ENXIO;

        goto _probe_err;
    }

    /* 注册中断 */
    //ts_cpl_request_irq(s32_irq_no, spimst_handler, 0, "spi", pst_spi_dev);

    pst_spi_dev->u32_register_base = u32_regbase;
    pst_spi_dev->u08_irq_no = s32_irq_no;
    pst_spi_dev->u16_fifo_depth = (ts_u16)ts_cpl_plat_get_value(pst_drv, "fifo-depth", 0, 64);
    pst_spi_dev->u08_slv_cnt = 2;

    s32_ret = ts_cpl_mutex_init((ts_cpl_mutex_s_t *)&pst_spi_dev->st_mutex);
    if (s32_ret != TS_OK)
        goto _probe_err;

    pst_attr = (ts_spi_attr_s_t *)&pst_spi_dev->st_attr;
    pst_attr->u16_mode = SPI_MODE_0;
    pst_attr->u16_endian = SPI_ENDIAN_LE;
    pst_attr->u16_clk_div = 24;
    pst_attr->u16_rx_dly = 1000;

    return TS_OK;

_probe_err:
    ts_cpl_dev_unregister(pst_cpl_dev);

    return s32_ret;
}


static ts_cpl_of_devid_s_t sst_spimst_drv_id = {
    .compatible = "tg,spi_v100",
};

static ts_cpl_plat_drv_s_t sst_spimst_drv = {
    .probe = spimst_v100_probe,
    .driver = {
        .name = "spi_mst",
        .of_match_table = &sst_spimst_drv_id,
    }
};


ts_s32 spi_init(ts_void)
{
    ts_s32 s32_ret;

    s32_ret = ts_cpl_plat_driver_register((ts_cpl_plat_drv_s_t *)&sst_spimst_drv);

    return s32_ret;
}

ts_cpl_ins_module_init(spi, spi_init);

ts_cpl_ins_module_param(spi, su32_spi_clk, int, 0666);

/*
 * dmac.c -- dmac_v100 api 实现
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 *
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#include <base/ts_type.h>
#include <base/ts_errno.h>
#include <base/ts_bitop.h>

#include <uapi/base_v100.h>
#include <uapi/dmac_v100.h>
#include <kapi/k_dmac_v100.h>

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

/* DMAC 上下文 */
typedef struct libdma_ctx_s {
    /* 设备文件句柄 */
    ts_s32 s32_fd;

    /* 从驱动获取的驱动上下文 */
    dmac_dev_s_t *pst_ctx;
} libdma_ctx_s_t;

static libdma_ctx_s_t sst_ctx = {0};

#define libdma_chk_init()\
do{\
    if (ts_null == sst_ctx.pst_ctx) {\
        ts_uapi_log_err("%s\n", TS_ENOINIT_STR);\
        return ts_null;\
    }\
} while(0)


/*
 * 函数名 :  libdmac_start_chn
 * 描  述 :  开启DMAC通道传输
 * 输  入 :  参数               描述
 *           pv_chn             通道句柄
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           无                 无
 */
static ts_void libdmac_start_chn(ts_void *pv_chn)
{
    dmac_dev_s_t *pst_dev;
    dmac_chn_s_t *pst_chn;
    ts_u32 u32_reg, u32_id, u32_tmp;

    pst_chn = (dmac_chn_s_t *)pv_chn;
    pst_dev = (dmac_dev_s_t *)pst_chn->pv_priv;
    u32_id = (ts_u32)pst_chn->u16_id;

    u32_reg = pst_dev->u32_regbase + DMAC_TG_OFFSET_CHEN;
    u32_tmp = ts_cpl_readl(u32_reg);
    u32_tmp |= DMAC_BIT_CHN_EN(u32_id) | DMAC_BIT_CHN_EN_WE(u32_id);

    ts_cpl_writel(u32_tmp, u32_reg);
}


/*
 * 函数名 :  libdmac_stop_chn
 * 描  述 :  停止DMAC通道传输
 * 输  入 :  参数               描述
 *           pv_chn             通道句柄
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           无                 无
 */
static TS_FUNC_UNUSED ts_void libdmac_stop_chn(ts_void *pv_chn)
{
    dmac_dev_s_t *pst_dev;
    dmac_chn_s_t *pst_chn;
    ts_u32 u32_reg, u32_id, u32_tmp;

    pst_chn = (dmac_chn_s_t *)pv_chn;
    pst_dev = (dmac_dev_s_t *)pst_chn->pv_priv;

    u32_id = (ts_u32)pst_chn->u16_id;

    u32_reg = pst_dev->u32_regbase + DMAC_TG_OFFSET_CHEN;
    u32_tmp = ts_cpl_readl(u32_reg);
    u32_tmp &= ~(DMAC_BIT_CHN_EN(u32_id) | DMAC_BIT_CHN_EN_WE(u32_id));
    ts_cpl_writel(u32_tmp, u32_reg);
}


/*
 * 函数名 :  libdmac_chk_complete
 * 描  述 :  判断传输是否完成
 * 输  入 :  参数               描述
 *           pv_param           通道句柄
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           0                  传输未完成
 *           >0                 传输完成
 */
static ts_s32 libdmac_chk_complete(ts_void *pv_param)
{
    dmac_chn_s_t *pst_chn;
    ts_u32 u32_int_status;

    pst_chn = (dmac_chn_s_t *)pv_param;
    u32_int_status = pst_chn->u32_int_status;
    if ((u32_int_status & DMAC_BIT_CHN_TFR_DONE) > 0)
        return DMAC_BIT_CHN_TFR_DONE;
    else
        return 0;
}


/*
 * 函数名 :  libdmac_xfer
 * 描  述 :  DMAC传输参数配置以及开启传输
 * 输  入 :  参数               描述
 *           pv_chn             通道句柄
 *           dat_src_addr       源地址
 *           dat_dst_addr       目标地址
 *           u32_size           传输大小,单位byte
 *           u32_width          位宽
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           TS_OK              传输完成
 *           TS_ERR             传输失败
 */
static ts_s32 libdmac_xfer(ts_void *pv_chn, ts_dma_addr_t dat_src_addr, ts_dma_addr_t dat_dst_addr, ts_u32 u32_size, ts_u32 u32_width)
{
    dmac_chn_s_t *pst_chn;
    ts_u32 u32_reg_addr;
    ts_u32 u32_max_bytes, u32_sz;
    ts_cpl_wait_s_t *pst_wait;
    ts_s32 s32_ret;

    u32_sz = u32_size;
    pst_chn = (dmac_chn_s_t *)pv_chn;
    u32_reg_addr = pst_chn->u32_reg_addr;
    u32_max_bytes = 0x3FFFFF;

    if (16 == u32_width) {
        u32_sz = u32_size >> 1;
        u32_max_bytes *= 2;
    } else if (32 == u32_width) {
        u32_sz = u32_size >> 2;
        u32_max_bytes *= 4;
    } else if (64 == u32_width) {
        u32_sz = u32_size >> 3;
        u32_max_bytes *= 8;
    } else if (128 == u32_width) {
        u32_sz = u32_size >> 4;
        u32_max_bytes *= 16;
    }

    /* BLK_TS 22bit 最大0x3FFFFF */
    if ((u32_sz - 1 )> 0x3FFFFF) {
        ts_uapi_log_err(
            "Error: Transmission size exceeds BLK_TS (22-bit) limit!\n"
            "Current width: %d bit, Max allowed bytes: 0x%x bytes, Your requested bytes: 0x%x bytes.\n", u32_width, u32_max_bytes, u32_size);

        return TS_ERR;
    }

    /* 设置地址 */
    ts_cpl_writel(dat_src_addr, (u32_reg_addr + DMAC_CHN_OFFSET_SAR));
    ts_cpl_writel(dat_dst_addr, (u32_reg_addr + DMAC_CHN_OFFSET_DAR));
    ts_cpl_writel((u32_sz - 1), (u32_reg_addr + DMAC_CHN_OFFSET_BLK_TS));

    /* 清除中断状态 */
    pst_chn->u32_int_status = 0;

    libdmac_start_chn(pv_chn);

    /* wait for complete */
    pst_wait = (ts_cpl_wait_s_t *)&pst_chn->st_wait;

    s32_ret = ts_cpl_wait_timeout_uninterruptible(pst_wait, libdmac_chk_complete, pst_chn, 10000);
    if (s32_ret > 0)
        return TS_OK;
    else
        return TS_ERR;
}

/*
 * 函数名 :  tdl_dmac_enable
 * 描  述 :  使能 DMAC ， 并开启全局中断
 * 输  入 :  参数               描述
 *           pst_dev           DMAC 设备
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           无                 无
 */
static ts_void libdmac_enable(dmac_dev_s_t *pst_dev)
{
    ts_u32 u32_reg;
    ts_u32 u32_tmp;

    u32_reg = pst_dev->u32_regbase + DMAC_TG_OFFSET_CFG;
    u32_tmp = TS_BIT(DMAC_BIT_CFG_INT_EN) | TS_BIT(DMAC_BIT_CFG_DMAC_EN);

    ts_cpl_writel(u32_tmp, u32_reg);
}


/*
 * 函数名 :  tdl_dmac_disable
 * 描  述 :  关闭 DMAC ， 并关闭全局中断
 * 输  入 :  参数               描述
 *           pst_dev           DMAC 设备
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           无                 无
 */
static TS_FUNC_UNUSED ts_void libdmac_disable(dmac_dev_s_t *pst_dev)
{
    ts_u32 u32_reg;

    u32_reg = pst_dev->u32_regbase + DMAC_TG_OFFSET_CFG;

    ts_cpl_writel(0, u32_reg);
}


/*
 * 函数名 :  tdl_dmac_reset
 * 描  述 :  复位 DMAC
 * 输  入 :  参数               描述
 *           pst_dev           DMAC 设备
 *           u32_time_out       超时计数
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           TS_OK              初始化成功
 *           TS_ERR             初始化失败
 */
static ts_s32 libdmac_reset(dmac_dev_s_t *pst_dev, ts_u32 u32_time_out)
{
    ts_u32 i, u32_reg, u32_tmp;

    ts_cpl_writel(0, (pst_dev->u32_regbase + DMAC_TG_OFFSET_COMM_INT_STATUS_ENABLE));
    ts_cpl_writel(0, (pst_dev->u32_regbase + DMAC_TG_OFFSET_COMM_INT_SIGNAL_ENABLE));
    ts_cpl_writel(0, (pst_dev->u32_regbase + DMAC_TG_OFFSET_COMM_INT_CLEAR));

    u32_reg = pst_dev->u32_regbase + DMAC_TG_OFFSET_RESET;
    u32_tmp = ts_cpl_readl(u32_reg);
    u32_tmp |= TS_BIT(DMAC_BIT_RESET);
    ts_cpl_writel(u32_tmp, u32_reg);
    for (i = 0; i < u32_time_out; i++) {
        u32_tmp = ts_cpl_readl(u32_reg);
        if (0 == u32_tmp)
            return TS_OK;
    }

    return TS_ERR;
}


/*
 * 函数名 :  tdl_dmac_chn_reset
 * 描  述 :  DMAC 每个通道初始化
 * 输  入 :  参数               描述
 *           pst_dev           DMAC 设备
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           无                 无
 */
static ts_void libdmac_chn_reset(dmac_dev_s_t *pst_dev)
{
    ts_u32 i, u32_reg_base;

    u32_reg_base = pst_dev->u32_regbase + DMAC_TG_CHAN_OFFSET_BASE;
    for (i = 0; i < (ts_u32)pst_dev->u16_chn_num; i++) {

        /* 开启状态使能，如果 INT_SIGNAL 不使能，则不会产生中断, 可以用于查询 */
        ts_cpl_writel(0xffffffff, (u32_reg_base + DMAC_CHN_OFFSET_INT_STATUS_ENABLE));
        ts_cpl_writel(0xffffffff, (u32_reg_base + DMAC_CHN_OFFSET_INT_STATUS_ENABLE + 4));

        ts_cpl_writel(0xffffffff, (u32_reg_base + DMAC_CHN_OFFSET_INT_SIGNAL_ENABLE));
        ts_cpl_writel(0xffffffff, (u32_reg_base + DMAC_CHN_OFFSET_INT_SIGNAL_ENABLE + 4));

        ts_cpl_writel(0xffffffff, (u32_reg_base + DMAC_CHN_OFFSET_INT_CLEAR));
        ts_cpl_writel(0xffffffff, (u32_reg_base + DMAC_CHN_OFFSET_INT_CLEAR + 4));

        /* 对于不使用的高位寄存器等进行赋 0 操作 */
        ts_cpl_writel(0, (u32_reg_base + DMAC_CHN_OFFSET_SAR + 4));
        ts_cpl_writel(0, (u32_reg_base + DMAC_CHN_OFFSET_DAR + 4));
        ts_cpl_writel(0, (u32_reg_base + DMAC_CHN_OFFSET_LLP));
        ts_cpl_writel(0, (u32_reg_base + DMAC_CHN_OFFSET_LLP + 4));

        u32_reg_base += DMAC_TG_CHAN_OFFSET_BASE;
    }
}


/*
 * 函数名 :  libdmac_chn_set_attr
 * 描  述 :  为通道设置属性
 * 输  入 :  参数               描述
 *           pv_chn             DMAC 通道句柄
 *           pst_attr           传输属性
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           TS_ERR             属性错误
 *           TS_OK              属性设置成功
 */
static ts_s32 libdmac_chn_set_attr(ts_void *pv_chn, ts_dma_attr_s_t *pst_attr)
{
    dmac_chn_s_t *pst_chn;
    dmac_dev_s_t *pst_dev;
    ts_u32 u32_reg_chn_base, u32_val_lo, u32_val_hi;

    pst_chn = (dmac_chn_s_t *)pv_chn;
    pst_dev = (dmac_dev_s_t *)pst_chn->pv_priv;
    u32_reg_chn_base = pst_chn->u32_reg_addr;

    /* cfg reg */
    ts_cpl_writel(0, (u32_reg_chn_base + DMAC_CHN_OFFSET_CFG));
    ts_cpl_writel(((pst_attr->u32_tt_fc) << DMAC_BIT_CHN_TT_FC), (u32_reg_chn_base + DMAC_CHN_OFFSET_CFG + 4));

    /* ctl */
    if (TS_DMAC_XFER_ADDR_SINC_ONLY == pst_attr->u32_addr_type)
        u32_val_lo = DMAC_DADDR_FIXED;
    else if (TS_DMAC_XFER_ADDR_DINC_ONLY == pst_attr->u32_addr_type)
        u32_val_lo = DMAC_SADDR_FIXED;
    else if (TS_DMAC_XFER_ADDR_NO_CHANGE == pst_attr->u32_addr_type)
        u32_val_lo = DMAC_DADDR_FIXED | DMAC_SADDR_FIXED;
    else
        u32_val_lo = 0;

    if ((8 == pst_attr->u32_width) && (pst_dev->u16_bus_width >= DMAC_TR_WIDTH_BITS_8)) {
        u32_val_lo |= (DMAC_TR_WIDTH_BITS_8 << DMAC_BIT_CHN_CTL_SRC_TR_WIDTH);
        u32_val_lo |= (DMAC_TR_WIDTH_BITS_8 << DMAC_BIT_CHN_CTL_DST_TR_WIDTH);
    } else if ((16 == pst_attr->u32_width) && (pst_dev->u16_bus_width >= DMAC_TR_WIDTH_BITS_16)) {
        u32_val_lo |= (DMAC_TR_WIDTH_BITS_16 << DMAC_BIT_CHN_CTL_SRC_TR_WIDTH);
        u32_val_lo |= (DMAC_TR_WIDTH_BITS_16 << DMAC_BIT_CHN_CTL_DST_TR_WIDTH);
    } else if ((32 == pst_attr->u32_width) && (pst_dev->u16_bus_width >= DMAC_TR_WIDTH_BITS_32)) {
        u32_val_lo |= (DMAC_TR_WIDTH_BITS_32 << DMAC_BIT_CHN_CTL_SRC_TR_WIDTH);
        u32_val_lo |= (DMAC_TR_WIDTH_BITS_32 << DMAC_BIT_CHN_CTL_DST_TR_WIDTH);
    } else if ((64 == pst_attr->u32_width) && (pst_dev->u16_bus_width >= DMAC_TR_WIDTH_BITS_64)) {
        u32_val_lo |= (DMAC_TR_WIDTH_BITS_64 << DMAC_BIT_CHN_CTL_SRC_TR_WIDTH);
        u32_val_lo |= (DMAC_TR_WIDTH_BITS_64 << DMAC_BIT_CHN_CTL_DST_TR_WIDTH);
    } else if ((128 == pst_attr->u32_width) && (pst_dev->u16_bus_width >= DMAC_TR_WIDTH_BITS_128)) {
        u32_val_lo |= (DMAC_TR_WIDTH_BITS_128 << DMAC_BIT_CHN_CTL_SRC_TR_WIDTH);
        u32_val_lo |= (DMAC_TR_WIDTH_BITS_128 << DMAC_BIT_CHN_CTL_DST_TR_WIDTH);
    } else if ((256 == pst_attr->u32_width) && (pst_dev->u16_bus_width >= DMAC_TR_WIDTH_BITS_256)) {
        u32_val_lo |= (DMAC_TR_WIDTH_BITS_256 << DMAC_BIT_CHN_CTL_SRC_TR_WIDTH);
        u32_val_lo |= (DMAC_TR_WIDTH_BITS_256 << DMAC_BIT_CHN_CTL_DST_TR_WIDTH);
    } else {
        return TS_ERR;
    }

    u32_val_lo |= (pst_dev->u16_burst_len << DMAC_BIT_CHN_CTL_SRC_MSIZE);
    u32_val_lo |= (pst_dev->u16_burst_len << DMAC_BIT_CHN_CTL_DST_MSIZE);

    /* AXI 端口，因为有获取和释放，有很大可能被用户占用 port 不释放，动态去获取空闲
      port 并不一定能有效利用，这里只根据 id 来分配， 偶数 axi0， 计数 axi1 */
    if (pst_dev->u08_port_num > 1) {
        if ((pst_chn->u16_id & 0x01) > 0)
            u32_val_lo |= (1 << DMAC_BIT_CHN_CTL_SMS) | (1 << DMAC_BIT_CHN_CTL_DMS);
    }

    /* 限制burst长度 */
    u32_val_hi = ts_cpl_readl(u32_reg_chn_base + DMAC_CHN_OFFSET_CTL + 4);
    u32_val_hi |= (1 << DMAC_BIT_CHN_CTL_AWLEN_EN) | (8 << DMAC_BIT_CHN_CTL_AWLEN) |\
                    (1 << DMAC_BIT_CHN_CTL_ARLEN_EN) | (8 << DMAC_BIT_CHN_CTL_ARLEN);

    ts_cpl_writel(u32_val_lo, (u32_reg_chn_base + DMAC_CHN_OFFSET_CTL));
    ts_cpl_writel(u32_val_hi, (u32_reg_chn_base + DMAC_CHN_OFFSET_CTL + 4));

    return TS_OK;
}


/*
 * 函数名 :  libdmac_release_chn
 * 描  述 :  释放通道
 * 输  入 :  参数               描述
 *           pv_chn             DMAC通道句柄
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           无                 无
 */
static ts_void libdmac_release_chn(ts_void *pv_chn)
{
    dmac_chn_s_t *pst_chns;
    ts_cpl_mutex_s_t *pst_mutex;
    dmac_dev_s_t *pst_dev;

    pst_chns = (dmac_chn_s_t *)pv_chn;
    pst_dev = (dmac_dev_s_t *)pst_chns->pv_priv;
    if (pst_chns != ts_null) {
        pst_mutex = (ts_cpl_mutex_s_t *)&pst_dev->st_mutex;
        ts_cpl_mutex_lock(pst_mutex);
        pst_chns->u16_busy = DMAC_CHN_IDLE;
        ts_cpl_mutex_unlock(pst_mutex);
    }
}

/*
 * 函数名 :  libdmac_get_free_chn
 * 描  述 :  获取空闲通道
 * 输  入 :  参数               描述
 *           pst_dev           DMAC 设备
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           无                 无
 */
static ts_void * libdmac_get_free_chn(dmac_dev_s_t *pst_dev)
{
    dmac_chn_s_t *pst_chns;
    ts_s32 i, s32_chn_num;
    ts_cpl_mutex_s_t *pst_mutex;

    pst_chns = (dmac_chn_s_t *)pst_dev->pv_priv;
    if (ts_null == pst_chns)
        return pst_chns;

    s32_chn_num = (ts_s32)pst_dev->u16_chn_num;
    pst_mutex = (ts_cpl_mutex_s_t *)&pst_dev->st_mutex;

    ts_cpl_mutex_lock(pst_mutex);
    for (i = 0; i < s32_chn_num; i++) {
        if (DMAC_CHN_IDLE == pst_chns->u16_busy) {
            pst_chns->u16_busy = DMAC_CHN_BUSY;
            ts_cpl_mutex_unlock(pst_mutex);

            return pst_chns;
        }

        pst_chns++;
    }

    ts_cpl_mutex_unlock(pst_mutex);

    return ts_null;
}


/*
 * 函数名 :  libdmac_init
 * 描  述 :  初始化 DMAC
 * 输  入 :  参数               描述
 *           pst_dev           DMAC 设备
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           TS_OK              初始化成功
 *           TS_ERR             初始化失败
 */
static ts_s32 libdmac_init(dmac_dev_s_t *pst_dev)
{
    libdmac_reset(pst_dev, 0x500);
    libdmac_chn_reset(pst_dev);
    libdmac_enable(pst_dev);

    /* 避免多次判断，这里对 bus_width 和 burst_len 进行重新配置 */
    if (64 == pst_dev->u16_bus_width)
        pst_dev->u16_bus_width = (ts_u16)DMAC_TR_WIDTH_BITS_64;
    else if (128 == pst_dev->u16_bus_width)
        pst_dev->u16_bus_width = (ts_u16)DMAC_TR_WIDTH_BITS_128;
    else if (256 == pst_dev->u16_bus_width)
        pst_dev->u16_bus_width = (ts_u16)DMAC_TR_WIDTH_BITS_256;
    else if (512 == pst_dev->u16_bus_width)
        pst_dev->u16_bus_width = (ts_u16)DMAC_TR_WIDTH_BITS_512;
    else
        pst_dev->u16_bus_width = (ts_u16)DMAC_TR_WIDTH_BITS_32;

    if (64 == pst_dev->u16_burst_len)
        pst_dev->u16_burst_len = (ts_u16)DMAC_MSZIE_DATA_ITEM_64;
    else if (128 == pst_dev->u16_burst_len)
        pst_dev->u16_burst_len = (ts_u16)DMAC_MSZIE_DATA_ITEM_128;
    else if (256 == pst_dev->u16_burst_len)
        pst_dev->u16_burst_len = (ts_u16)DMAC_MSZIE_DATA_ITEM_256;
    else if (512 == pst_dev->u16_burst_len)
        pst_dev->u16_burst_len = (ts_u16)DMAC_MSZIE_DATA_ITEM_512;
    else if (1024 == pst_dev->u16_burst_len)
        pst_dev->u16_burst_len = (ts_u16)DMAC_MSZIE_DATA_ITEM_1024;
    else
        pst_dev->u16_burst_len = (ts_u16)DMAC_MSZIE_DATA_ITEM_32;

    return TS_ERR;
}


/*
 * 函数名 :  ts_lib_dma_do_memcpy
 * 描  述 :  DMAC 按照 属性配置 进行传输
 * 输  入 :  参数               描述
 *           pst_attr           传输属性
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           pv_ret             copy成功
 *           ts_null            copy失败
 */
static ts_void *ts_lib_dma_do_memcpy(ts_dma_attr_s_t *pst_attr)
{
    ts_void *pv_hdl;
    ts_void *pv_ret;
    ts_long lg_ret;

    if (ts_null == pst_attr)
        return ts_null;

    pv_hdl = libdmac_get_free_chn(sst_ctx.pst_ctx);
    if (ts_null == pv_hdl)
        return ts_null;

    pst_attr->pv_hdl = pv_hdl;
    lg_ret = TS_ERR;

    lg_ret = (ts_long)libdmac_chn_set_attr(pst_attr->pv_hdl, pst_attr);
    if (lg_ret != TS_OK)
        return ts_null;

    lg_ret = (ts_long)libdmac_xfer(pst_attr->pv_hdl, pst_attr->da_src_addr, \
                pst_attr->da_dst_addr, pst_attr->u32_size, pst_attr->u32_width);

    if (TS_OK == lg_ret)
        pv_ret = (ts_void *)pst_attr->da_dst_addr;
    else
        pv_ret = ts_null;

    libdmac_release_chn(pv_hdl);

    return pv_ret;
}


/*
 * 函数名 :  ts_lib_dma_memcpy
 * 描  述 :  DMAC 按照 32 bit 为单位进行传输
 * 输  入 :  参数               描述
 *           pv_dst             DMA 目标地址
 *           pv_src             DMA 源地址
 *           u32_count          DMA 拷贝个数， byte 为单位
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           = pv_dst           成功
 *           ts_null            失败
 */
ts_void *ts_lib_dma_memcpy(ts_void *pv_dst, const ts_void *pv_src, ts_u32 u32_count)
{
    ts_dma_attr_s_t st_attr;

    libdma_chk_init();

    if (((ts_u32)pv_dst & 0x03) != 0 || ((ts_u32)pv_src & 0x03) != 0) {
        ts_cpl_log_err("dma address is not 32 bit align\n");

        return ts_null;
    }

    st_attr.da_src_addr = (ts_dma_addr_t)pv_src;
    st_attr.da_dst_addr = (ts_dma_addr_t)pv_dst;
    st_attr.u32_size = u32_count;
    st_attr.u32_width = 32;
    st_attr.u32_addr_type = TS_DMAC_XFER_ADDR_NORMAL;
    st_attr.u32_tt_fc = TS_DMAC_XFER_TTFC_M2M_DMAC;

    return ts_lib_dma_do_memcpy((ts_dma_attr_s_t *)&st_attr);
}


/*
 * 函数名 :  ts_lib_dma_memcpy64
 * 描  述 :  DMAC 按照 64 bit 为单位进行传输
 * 输  入 :  参数               描述
 *           pv_dst             DMA 目标地址
 *           pv_src             DMA 源地址
 *           u32_count          DMA 拷贝个数， byte 为单位
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           = pv_dst           成功
 *           ts_null            失败
 */
ts_void *ts_lib_dma_memcpy64(ts_void *pv_dst, const ts_void *pv_src, ts_u32 u32_count)
{
    ts_dma_attr_s_t st_attr;

    libdma_chk_init();

    if (((ts_u32)pv_dst & 0x07) != 0 || ((ts_u32)pv_src & 0x07) != 0) {
        ts_cpl_log_err("dma address is not 64 bit align\n");

        return ts_null;
    }

    st_attr.da_src_addr = (ts_dma_addr_t)pv_src;
    st_attr.da_dst_addr = (ts_dma_addr_t)pv_dst;
    st_attr.u32_size = u32_count;
    st_attr.u32_width = 64;
    st_attr.u32_addr_type = TS_DMAC_XFER_ADDR_NORMAL;
    st_attr.u32_tt_fc = TS_DMAC_XFER_TTFC_M2M_DMAC;

    return ts_lib_dma_do_memcpy((ts_dma_attr_s_t *)&st_attr);
}


/*
 * 函数名 :  ts_lib_dma_memcpy16
 * 描  述 :  DMAC 按照 16 bit 为单位进行传输
 * 输  入 :  参数               描述
 *           pv_dst             DMA 目标地址
 *           pv_src             DMA 源地址
 *           u32_count          DMA 拷贝个数， byte 为单位
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           = pv_dst           成功
 *           ts_null            失败
 */
ts_void *ts_lib_dma_memcpy16(ts_void *pv_dst, const ts_void *pv_src, ts_u32 u32_count)
{
    ts_dma_attr_s_t st_attr;

    libdma_chk_init();

    if (((ts_u32)pv_dst & 0x01) != 0 || ((ts_u32)pv_src & 0x01) != 0) {
        ts_cpl_log_err("dma address is not 16 bit align\n");

        return ts_null;
    }

    st_attr.da_src_addr = (ts_dma_addr_t)pv_src;
    st_attr.da_dst_addr = (ts_dma_addr_t)pv_dst;
    st_attr.u32_size = u32_count;
    st_attr.u32_width = 16;
    st_attr.u32_addr_type = TS_DMAC_XFER_ADDR_NORMAL;
    st_attr.u32_tt_fc = TS_DMAC_XFER_TTFC_M2M_DMAC;

    return ts_lib_dma_do_memcpy((ts_dma_attr_s_t *)&st_attr);
}


/*
 * 函数名 :  ts_lib_dma_memcpy8
 * 描  述 :  DMAC 按照 8 bit 为单位进行传输
 * 输  入 :  参数               描述
 *           pv_dst             DMA 目标地址
 *           pv_src             DMA 源地址
 *           u32_count          DMA 拷贝个数， byte 为单位
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           = pv_dst           成功
 *           ts_null            失败
 */
ts_void *ts_lib_dma_memcpy8(ts_void *pv_dst, const ts_void *pv_src, ts_u32 u32_count)
{
    ts_dma_attr_s_t st_attr;

    libdma_chk_init();

    st_attr.da_src_addr = (ts_dma_addr_t)pv_src;
    st_attr.da_dst_addr = (ts_dma_addr_t)pv_dst;
    st_attr.u32_size = u32_count;
    st_attr.u32_width = 8;
    st_attr.u32_addr_type = TS_DMAC_XFER_ADDR_NORMAL;
    st_attr.u32_tt_fc = TS_DMAC_XFER_TTFC_M2M_DMAC;

    return ts_lib_dma_do_memcpy((ts_dma_attr_s_t *)&st_attr);
}


/*
 * 函数名 :  ts_lib_dmac_deinit
 * 描  述 :  DMAC 库去初始化
 * 输  入 :  参数           描述
 *           无             无
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          去初始化成功
 */
ts_s32 ts_lib_dmac_deinit(ts_void)
{
    if (sst_ctx.s32_fd >= 0) {
        close(sst_ctx.s32_fd);

        sst_ctx.s32_fd = -1;
        sst_ctx.pst_ctx = ts_null;
    }

    return TS_OK;
}


/*
 * 函数名 :  ts_lib_dmac_init
 * 描  述 :  初始化 DMAC 库
 * 输  入 :  参数           描述
 *           无             无
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          初始化成功
 *           TS_ERR        初始化失败
 */
ts_s32 ts_lib_dmac_init(ts_void)
{
    ts_s32 s32_ret;
    ts_u32 u32_dma_addr;
    if (ts_null == sst_ctx.pst_ctx) {
        sst_ctx.s32_fd = open("/dev/dmac", O_RDWR);
        if (sst_ctx.s32_fd >= 0) {
            s32_ret = (ts_s32)ioctl(sst_ctx.s32_fd, TS_IOCTL_IOC_NR_DMAC_INIT, (ts_long)&u32_dma_addr);
            if (TS_OK == s32_ret) {
                sst_ctx.pst_ctx = (dmac_dev_s_t *)u32_dma_addr;
                libdmac_init(sst_ctx.pst_ctx);

                return TS_OK;
            } else {
                ts_uapi_log_err("Failed to get dma context\n");

                ts_lib_dmac_deinit();
            }
        } else {
            ts_uapi_log_err("%s\n", TS_ENODEV_STR);
        }
    }

    return TS_ERR;
}


/* 
 * spi_tg_v100.c -- spi_tg_v100 实现
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
#include <uapi/spi_v100.h>
#include <kapi/k_spimst_v100.h>

#include "spi_tg_v100.h"

#define SPI_TG_TIMEOUT      (0xF0000)


ts_s32 spi_tdl_xfer(spi_dev_s_t *pv_dev, ts_spi_msg_s_t *pst_msg)
{
    ts_s32 s32_in_irq;
    ts_spi_attr_s_t *pst_attr;
    ts_u32 i, u32_val, u32_rx_cnt, u32_cmd_cnt, u32_tx_cnt, u32_cnt;
    ts_u32 *pu32_ptr;
    ts_u16 *pu16_ptr;
    ts_u08 *pu08_ptr;
    ts_u08 *pu08_buf;

    if (pst_msg->u08_cs >= pv_dev->u08_slv_cnt)
        return -TS_EINVAL;

    if ((pst_msg->u32_rx_size > 0) && (ts_null == pst_msg->pu08_rx_buf))
        return -TS_EINVAL;

    if ((pst_msg->u32_cmd_size > 0) && (ts_null == pst_msg->pu08_cmd_buf))
        return -TS_EINVAL;

    if ((pst_msg->u32_tx_size > 0) && (ts_null == pst_msg->pu08_tx_buf))
        return -TS_EINVAL;

    u32_rx_cnt = pst_msg->u32_rx_size;
    u32_cmd_cnt = pst_msg->u32_cmd_size;
    u32_tx_cnt = pst_msg->u32_tx_size;

    if ((u32_tx_cnt + u32_cmd_cnt) > pv_dev->u16_fifo_depth)
        return -TS_EINVAL;

    pst_attr = (ts_spi_attr_s_t *)&pv_dev->st_attr;

    /* 非中断，上锁保证本次传输完整 */
    s32_in_irq = ts_cpl_in_interrupt();
    if (0 == s32_in_irq)
        ts_cpl_mutex_lock((ts_cpl_mutex_s_t *)&pv_dev->st_mutex);

    ts_cpl_writel(SPI_TG_DISABLE, (pv_dev->u32_register_base + SPI_TG_OFFSET_ENR));

    /* 避免不同应用导致配置混乱，此处重新配置 */
    u32_val = pst_attr->u16_mode << SPI_TG_CTRL0_SCPHA;
    if (((u32_cmd_cnt + u32_tx_cnt) > 0) && (pst_msg->u32_rx_size > 0))
        u32_val |= SPI_TG_CTRL0_TMOD_EEPROM;
    else if (pst_msg->u32_rx_size > 0 && ((u32_cmd_cnt + u32_tx_cnt) <= 0))
        u32_val |= SPI_TG_CTRL0_TMOD_RX;
    else
        u32_val |= SPI_TG_CTRL0_TMOD_TX;

    u32_val |= (pst_msg->u08_dfs - 1) << SPI_TG_CTRL0_DFS;
    if (SPI_ENDIAN_BE == pst_attr->u16_endian)
        u32_val |= SPI_TG_CTRL0_ENDIAN_BE;

    ts_cpl_writel(u32_val, (pv_dev->u32_register_base + SPI_TG_OFFSET_CTRLR0));

    if (u32_rx_cnt > 0)
        ts_cpl_writel((u32_rx_cnt - 1), (pv_dev->u32_register_base + SPI_TG_OFFSET_CTRLR1));

    u32_val = pst_attr->u16_clk_div & 0xFFFE;
    if (0 == u32_val)
        u32_val = SPI_TG_DEFAULT_LCK_DIV;

    ts_cpl_writel(u32_val, (pv_dev->u32_register_base + SPI_TG_OFFSET_CLKDIV));

    ts_cpl_writel(SPI_TG_ENABLE, (pv_dev->u32_register_base + SPI_TG_OFFSET_ENR));

    /* cmd_buf -> tx_buf */
    u32_cnt = u32_cmd_cnt;
    pu08_buf = pst_msg->pu08_cmd_buf;
    for (i = 0; i < 2; i++) {
        if (u32_cnt > 0) {
            if (pst_msg->u08_dfs > 16 && pst_msg->u08_dfs <= 32) {
                pu32_ptr = (ts_u32 *)pu08_buf;
                for (i = 0; i < u32_cnt; i++) {
                    u32_val = *pu32_ptr++;
                    ts_cpl_writel(u32_val, (pv_dev->u32_register_base + SPI_TG_OFFSET_DR));

                }
            } else if (pst_msg->u08_dfs > 8 && pst_msg->u08_dfs <= 16) {
                pu16_ptr = (ts_u16 *)pu08_buf;
                for (i = 0; i < u32_cnt; i++) {
                    u32_val = *pu16_ptr++;
                    ts_cpl_writel(u32_val, (pv_dev->u32_register_base + SPI_TG_OFFSET_DR));
                }
            } else {
                pu08_ptr = (ts_u08 *)pu08_buf;
                for (i = 0; i < u32_cnt; i++) {
                    u32_val = *pu08_ptr++;
                    ts_cpl_writel(u32_val, (pv_dev->u32_register_base + SPI_TG_OFFSET_DR));
                }
            }
        }

        /* next to tx buf */
        u32_cnt = u32_tx_cnt;
        pu08_buf = pst_msg->pu08_tx_buf;
    }

    if ((u32_cmd_cnt + u32_tx_cnt) <= 0) {
        ts_cpl_writel(0xFF, (pv_dev->u32_register_base + SPI_TG_OFFSET_DR));
    }

    /* start transmission */
    ts_cpl_writel((1 << pst_msg->u08_cs), (pv_dev->u32_register_base + SPI_TG_OFFSET_SER));
    if (u32_rx_cnt > 0) {
        pu08_ptr = pst_msg->pu08_rx_buf;
        pu16_ptr = (ts_u16 *)pu08_ptr;
        pu32_ptr = (ts_u32 *)pu08_ptr;
        while (1) {
            u32_cnt = ts_cpl_readl(pv_dev->u32_register_base + SPI_TG_OFFSET_RXFLR);
            if (u32_cnt > 0) {

                if (u32_rx_cnt > u32_cnt)
                    u32_rx_cnt -= u32_cnt;
                else {
                    u32_cnt = u32_rx_cnt;
                    u32_rx_cnt = 0;
                }

                if (pst_msg->u08_dfs > 16 && pst_msg->u08_dfs <= 32) {
                    for (i = 0; i < u32_cnt; i++) {
                        *pu32_ptr++ = ts_cpl_readl(pv_dev->u32_register_base + SPI_TG_OFFSET_DR);
                    }
                } else if (pst_msg->u08_dfs > 8 && pst_msg->u08_dfs <= 16) {
                    for (i = 0; i < u32_cnt; i++) {
                        *pu16_ptr++ = (ts_u16)ts_cpl_readl(pv_dev->u32_register_base + SPI_TG_OFFSET_DR);
                    }
                } else {
                    for (i = 0; i < u32_cnt; i++) {
                        *pu08_ptr++ = (ts_u08)ts_cpl_readl(pv_dev->u32_register_base + SPI_TG_OFFSET_DR);
                    }
                }
            }

            if (0 == u32_rx_cnt)
                break;
        }
    }

    /* 检测 TX 是否完成  */
    for (i = 0; i < SPI_TG_TIMEOUT; i++) {
        u32_val = ts_cpl_readl(pv_dev->u32_register_base + SPI_TG_OFFSET_SR);
        if (SPI_TG_SR_IS_IDLE == (u32_val & SPI_TG_SR_IDLE_MASK))
            break;
    }

    /* done */
    ts_cpl_writel(SPI_TG_DISABLE, (pv_dev->u32_register_base + SPI_TG_OFFSET_SER));
    ts_cpl_writel(SPI_TG_DISABLE, (pv_dev->u32_register_base + SPI_TG_OFFSET_ENR));

    if (0 == s32_in_irq)
        ts_cpl_mutex_unlock((ts_cpl_mutex_s_t *)&pv_dev->st_mutex);

    return TS_OK;
}




/* 
 * gpio.c -- OPEN TDK 对 gpio 驱动的封装，实现读写 gpio 的功能,驱动代码除中断外，上移到用户层实现
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023-2025 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#include <uapi/base_v100.h>
#include <uapi/gpio_v100.h>
#include <uapi/iomux.h>
#include <kapi/k_gpio_v100.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

/* 管脚信息 */
typedef struct gpio_s {
    ts_u32 u32_grp_idx;
    ts_u32 u32_pin_idx;
} gpio_s_t;

/* GPIO 上下文 */
typedef struct libgpio_ctx_s {
    /* 设备文件句柄 */
    ts_s32 s32_fd;

    /* 从驱动获取的驱动上下文 */
    gpio_grp_s_t *pst_grp;

    /* 寄存器写保护锁 */
    ts_cpl_spinlock_s_t st_lock;
} libgpio_ctx_s_t;

static libgpio_ctx_s_t sst_ctx = {0};


#define libgpio_chk_init()\
do{\
    if (ts_null == sst_ctx.pst_grp) {\
        ts_uapi_log_err("%s\n", TS_ENOINIT_STR);\
        return -TS_ENOINIT;\
    }\
} while(0)



ts_void ts_lib_gpio_set_o_strong(ts_u32 u32_ioname, ts_u32 u32_str)
{
    ts_u32 u32_regbase, u32_e4_reg, u32_e8_reg, u32_cfg_grp, u32_cfg_idx;
    ts_u32 u32_val, u32_e4_val, u32_e8_val, u32_mask, u32_set;

    /* bt 高8bit 需要单独处理 */
    if (TS_IO_MIPI == u32_ioname) {
        u32_val = ts_cpl_readl(0x0034be30);
        if (u32_val != 0x04) {
            return;
        }

        if (1 == u32_str) {
            u32_set = 0xCC;
        } else if (2 == u32_str) {
            u32_set = 0xEE;
        } else if (3 == u32_str) {
            u32_set = 0xFF;
        } else {
            u32_set = 0x88;
        }

        ts_cpl_writel(u32_set, 0x0034be50);

        return;
    }

    u32_cfg_grp = TS_IO_GET_CFG_GRP(u32_ioname);
    u32_cfg_idx = TS_IO_GET_CFG_IDX(u32_ioname);
    if (TS_IO_ADC_CH_FLAG == u32_cfg_grp) {
        u32_regbase = TS_SUC_REGBASE + 0x38;
        u32_val = ts_cpl_readl(u32_regbase);

        u32_set = (1 << (12 + u32_cfg_idx)) | (1 << (16 + u32_cfg_idx));
        u32_mask = ~u32_set;
        u32_val &= u32_mask;
        if (1 == u32_str)
            u32_val |= (1 << (12 + u32_cfg_idx));
        else if (2 == u32_str)
            u32_val |= (1 << (16 + u32_cfg_idx));
        else if (3 == u32_str)
            u32_val |= u32_set;

        ts_cpl_writel(u32_val, u32_regbase);
    } else {
        if ((u32_cfg_grp <= TS_IO_CFG_MAX_GRP) && (u32_cfg_idx <= TS_IO_CFG_MAX_IDX) && (u32_str < 4)) {
            u32_e4_reg = TS_SUC_REGBASE + TS_SUC_CFG_E4_OFFSET;
            u32_e8_reg = TS_SUC_REGBASE + TS_SUC_CFG_E8_OFFSET;

            u32_e4_reg += u32_cfg_grp << 2;
            u32_e8_reg += u32_cfg_grp << 2;

            u32_set = 1 << u32_cfg_idx;
            u32_mask = ~u32_set;

            u32_e4_val = ts_cpl_readl(u32_e4_reg);
            u32_e8_val = ts_cpl_readl(u32_e8_reg);
            //ts_cpl_printk("rd[%x,%x] = %x,%x\n", u32_e4_reg, u32_e8_reg, u32_e4_val, u32_e8_val);

            u32_e4_val &= u32_mask;
            u32_e8_val &= u32_mask;
            if (1 == u32_str) {
                u32_e4_val |= u32_set;
            } else if (2 == u32_str) {
                u32_e8_val |= u32_set;
            } else if (3 == u32_str) {
                u32_e4_val |= u32_set;
                u32_e8_val |= u32_set;
            }

            ts_cpl_writel(u32_e4_val, u32_e4_reg);
            ts_cpl_writel(u32_e8_val, u32_e8_reg);
            //ts_cpl_printk("wr[%x,%x] = %x,%x\n", u32_e4_reg, u32_e8_reg, u32_e4_val, u32_e8_val);
        }
    }
}


ts_void ts_lib_gpio_set_sr(ts_u32 u32_ioname, ts_gpio_sr_en_t en_sr)
{
    ts_u32 u32_regbase, u32_cfg_grp, u32_cfg_idx;
    ts_u32 u32_val, u32_mask, u32_set;

    u32_cfg_grp = TS_IO_GET_CFG_GRP(u32_ioname);
    u32_cfg_idx = TS_IO_GET_CFG_IDX(u32_ioname);
    if ((u32_cfg_grp <= TS_IO_CFG_MAX_GRP) && (u32_cfg_idx <= TS_IO_CFG_MAX_IDX)) {
        u32_set = 1 << u32_cfg_idx;
        u32_mask = ~u32_set;

        u32_regbase = TS_SUC_REGBASE + TS_SUC_CFG_SR_OFFSET;
        u32_regbase += u32_cfg_grp << 2;
        u32_val = ts_cpl_readl(u32_regbase);
        //ts_cpl_printk("rd[%x] = %x\n", u32_regbase, u32_val);

        if (TS_GPIO_SR_SLOW == en_sr)
            u32_val &= u32_mask;
        else
            u32_val |= u32_set;

        ts_cpl_writel(u32_val, u32_regbase);
        //ts_cpl_printk("wr[%x] = %x\n", u32_regbase, u32_val);
    }
}


ts_void ts_lib_gpio_set_schmitt(ts_u32 u32_ioname, ts_option_en_t en_opt)
{
    ts_u32 u32_regbase, u32_cfg_grp, u32_cfg_idx;
    ts_u32 u32_val, u32_mask, u32_set;

    u32_cfg_grp = TS_IO_GET_CFG_GRP(u32_ioname);
    u32_cfg_idx = TS_IO_GET_CFG_IDX(u32_ioname);
    if (TS_IO_ADC_CH_FLAG == u32_cfg_grp) {
        u32_regbase = TS_SUC_REGBASE + 0x38;
        u32_val = ts_cpl_readl(u32_regbase);

        u32_set = (1 << (u32_cfg_idx + 24));
        u32_mask = ~u32_set;
        if (TS_OPT_ENABLE == en_opt)
            u32_val |= u32_set;
        else
            u32_val &= u32_mask;

        ts_cpl_writel(u32_val, u32_regbase);
    } else {
        if ((u32_cfg_grp <= TS_IO_CFG_MAX_GRP) && (u32_cfg_idx <= TS_IO_CFG_MAX_IDX)) {
            u32_set = 1 << u32_cfg_idx;
            u32_mask = ~u32_set;

            u32_regbase = TS_SUC_REGBASE + TS_SUC_CFG_SMT_OFFSET;
            u32_regbase += u32_cfg_grp << 2;

            u32_val = ts_cpl_readl(u32_regbase);
            //ts_cpl_printk("rd[%x] = %x\n", u32_regbase, u32_val);

            if (TS_OPT_ENABLE == en_opt)
                u32_val |= u32_set;
            else
                u32_val &= u32_mask;

            ts_cpl_writel(u32_val, u32_regbase);
            //ts_cpl_printk("wr[%x] = %x\n", u32_regbase, u32_val);
        }
    }
}


ts_void ts_lib_gpio_set_pull(ts_u32 u32_ioname, ts_elec_level_en_t en_lvl)
{
    ts_u32 u32_pd_reg, u32_pu_reg, u32_cfg_grp, u32_cfg_idx;
    ts_u32 u32_pd_val, u32_pu_val, u32_mask, u32_set;

    u32_cfg_grp = TS_IO_GET_CFG_GRP(u32_ioname);
    u32_cfg_idx = TS_IO_GET_CFG_IDX(u32_ioname);

    if (TS_IO_ADC_CH_FLAG == u32_cfg_grp) {
        u32_pd_reg = TS_SUC_REGBASE + 0x38;
        u32_pd_val = ts_cpl_readl(u32_pd_reg);

        u32_set = (1 << (8 + u32_cfg_idx)) | (1 << (20 + u32_cfg_idx));
        u32_mask = ~u32_set;
        u32_pd_val &= u32_mask;
        if (TS_ELEC_LVL_LO == en_lvl)
            u32_pd_val |= (1 << (8 + u32_cfg_idx));
        else if (TS_ELEC_LVL_HI == en_lvl)
            u32_pd_val |= (1 << (20 + u32_cfg_idx));

        ts_cpl_writel(u32_pd_val, u32_pd_reg);
    } else {
        if ((u32_cfg_grp <= TS_IO_CFG_MAX_GRP) && (u32_cfg_idx <= TS_IO_CFG_MAX_IDX) && (en_lvl < TS_ELEC_LVL_VIL)) {
            u32_pd_reg = TS_SUC_REGBASE + TS_SUC_CFG_PD_OFFSET;
            u32_pu_reg = TS_SUC_REGBASE + TS_SUC_CFG_PU_OFFSET;

            u32_pd_reg += u32_cfg_grp << 2;
            u32_pu_reg += u32_cfg_grp << 2;

            u32_set = 1 << u32_cfg_idx;
            u32_mask = ~u32_set;

            u32_pd_val = ts_cpl_readl(u32_pd_reg);
            u32_pu_val = ts_cpl_readl(u32_pu_reg);
            //ts_cpl_printk("rd[%x,%x] = %x,%x\n", u32_pd_reg, u32_pu_reg, u32_pd_val, u32_pu_val);

            u32_pd_val &= u32_mask;
            u32_pu_val &= u32_mask;

            if (TS_ELEC_LVL_LO == en_lvl)
                u32_pd_val |= u32_set;
            else if (TS_ELEC_LVL_HI == en_lvl)
                u32_pu_val |= u32_set;

            ts_cpl_writel(u32_pd_val, u32_pd_reg);
            ts_cpl_writel(u32_pu_val, u32_pu_reg);
            //ts_cpl_printk("wr[%x,%x] = %x,%x\n", u32_pd_reg, u32_pu_reg, u32_pd_val, u32_pu_val);
        }
    }
}


ts_void ts_lib_gpio_set_mux(ts_u32 u32_ioname, ts_u32 u32_func)
{
    ts_u32 u32_regbase, u32_mux_grp, u32_mux_idx;
    ts_u32 u32_val, u32_mask, u32_set;

    u32_mux_grp = TS_IO_GET_MUX_GRP(u32_ioname);
    u32_mux_idx = TS_IO_GET_MUX_IDX(u32_ioname);
    if (TS_IO_ADC_CH == u32_ioname) {
        u32_regbase = TS_SUC_REGBASE + 0x30;
        if (TS_IO_ADC_CH_FUN0 == u32_func) {
            /* ADC 功能 */
            ts_cpl_writel(1, u32_regbase);
        } else {
            /* GPIO 功能 */
            ts_cpl_writel(0, u32_regbase);
        }
    } else {
        if ((u32_mux_grp <= TS_IO_MUX_MAX_GRP) && (u32_mux_idx <= TS_IO_MUX_MAX_IDX) && (u32_func < 4)) {
            u32_mask = ~(TS_IO_MUX_MASK << u32_mux_idx);
            u32_set = u32_func << u32_mux_idx;

            u32_regbase = TS_SUC_REGBASE + TS_SUC_MUX_OFFSET;
            u32_regbase += u32_mux_grp << 2;
            u32_val = ts_cpl_readl(u32_regbase);
            //ts_cpl_printk("rd[%x] = %x\n", u32_regbase, u32_val);

            u32_val &= u32_mask;
            u32_val |= u32_set;
            ts_cpl_writel(u32_val, u32_regbase);
            //ts_cpl_printk("wr[%x] = %x\n", u32_regbase, u32_val);
        }
    }
}


static ts_s32 libgpio_get_pin(ts_u32 u32_pin_no, gpio_s_t *pst_gpio, const ts_char *pc_func, ts_u32 u32_line)
{
    ts_u32 u32_grp_idx;

    u32_grp_idx = u32_pin_no >> 5;
    if (u32_grp_idx >= TS_GPIO_MAX_GRP) {
        ts_cpl_log(TS_LOG_ERR, gs32_uapi_log_lvl, pc_func, u32_line, "%s, must be less than < %d\n", TS_EINVALGRPID_STR, TS_GPIO_MAX_GRP);

        return -TS_EINVALGRPID;
    }

    pst_gpio->u32_grp_idx = u32_grp_idx;
    pst_gpio->u32_pin_idx = u32_pin_no & 0x1F;

    return TS_OK;
}


ts_s32 ts_lib_gpio_write_grp(ts_u32 u32_grp_idx, ts_u32 u32_pin_idx, ts_u32 u32_val)
{
    gpio_dev_s_t *pst_dev;
    ts_u32 u32_rd_val, u32_wr_val;
    ts_ulong ul_flags;

    libgpio_chk_init();
    if (u32_grp_idx >= TS_GPIO_MAX_GRP) {
        ts_uapi_log_err("%s, must be less than < %d\n", TS_EINVALGRPID_STR, TS_GPIO_MAX_GRP);

        return -TS_EINVALGRPID;
    }

    pst_dev = (gpio_dev_s_t *)&sst_ctx.pst_grp->ast_dev[u32_grp_idx];

    ts_cpl_spin_lock_irqsave((ts_cpl_spinlock_s_t *)&sst_ctx.st_lock, (ts_ulong *)&ul_flags);
    u32_rd_val = ts_cpl_readl(pst_dev->u32_regbase + GPIO_TG_OFFSET_DR);
    u32_wr_val = u32_rd_val & ~u32_pin_idx;
    u32_wr_val |= u32_val;
    ts_cpl_writel(u32_wr_val, (pst_dev->u32_regbase + GPIO_TG_OFFSET_DR));
    ts_cpl_spin_unlock_irqrestore((ts_cpl_spinlock_s_t *)&sst_ctx.st_lock, (ts_ulong *)&ul_flags);

    return TS_OK;
}


ts_s32 ts_lib_gpio_read_grp(ts_u32 u32_grp_idx, ts_u32 u32_pin_idx, ts_u32 *pu32_val)
{
    gpio_dev_s_t *pst_dev;
    ts_u32 u32_val;

    libgpio_chk_init();
    uapi_chk_ptr(pu32_val, TS_ENULLPTR);

    if (u32_grp_idx >= TS_GPIO_MAX_GRP) {
        ts_uapi_log_err("%s, must be less than < %d\n", TS_EINVALGRPID_STR, TS_GPIO_MAX_GRP);

        return -TS_EINVALGRPID;
    }

    pst_dev = (gpio_dev_s_t *)&sst_ctx.pst_grp->ast_dev[u32_grp_idx];
    u32_val = ts_cpl_readl(pst_dev->u32_regbase + GPIO_TG_OFFSET_EXT_PORTA);
    *pu32_val = u32_val & u32_pin_idx;

    return TS_OK;
}


ts_s32 ts_lib_gpio_flip_grp(ts_u32 u32_grp_idx, ts_u32 u32_pin_idx)
{
    ts_s32 s32_ret;
    ts_u32 u32_val;

    s32_ret = ts_lib_gpio_read_grp(u32_grp_idx, u32_pin_idx, (ts_u32 *)&u32_val);
    if (TS_OK == s32_ret)
        u32_val = u32_val ^ u32_pin_idx;
    else
        return s32_ret;

    return ts_lib_gpio_write_grp(u32_grp_idx, u32_pin_idx, u32_val);
}


ts_s32 ts_lib_gpio_write_pin(ts_u32 u32_pin_no, ts_elec_level_en_t en_lvl)
{
    gpio_s_t st_gpio;
    gpio_dev_s_t *pst_dev;
    ts_u32 u32_val, u32_set;
    ts_s32 s32_ret;
    ts_ulong ul_flags;

    libgpio_chk_init();

    /* 根据 GPIO 序号确定 BANK */
    s32_ret = libgpio_get_pin(u32_pin_no, (gpio_s_t *)&st_gpio, __func__, __LINE__);
    if (s32_ret != TS_OK)
        return s32_ret;

    pst_dev = (gpio_dev_s_t *)&sst_ctx.pst_grp->ast_dev[st_gpio.u32_grp_idx];
    u32_set = (1 << st_gpio.u32_pin_idx);

    /* 检测此管脚方向是否处于输出状态 */
    u32_val = ts_cpl_readl(pst_dev->u32_regbase + GPIO_TG_OFFSET_DDR);
    if (0 == (u32_val & u32_set))
        ts_uapi_log_warn("Input pin(%d) for output!\n", u32_pin_no);

    ts_cpl_spin_lock_irqsave((ts_cpl_spinlock_s_t *)&sst_ctx.st_lock, (ts_ulong *)&ul_flags);
    u32_val = ts_cpl_readl(pst_dev->u32_regbase + GPIO_TG_OFFSET_DR);
    if (TS_ELEC_LVL_LO == en_lvl)
        u32_val &= ~u32_set;
    else
        u32_val |= u32_set;

    ts_cpl_writel(u32_val, (pst_dev->u32_regbase + GPIO_TG_OFFSET_DR));
    ts_cpl_spin_unlock_irqrestore((ts_cpl_spinlock_s_t *)&sst_ctx.st_lock, (ts_ulong *)&ul_flags);

    return TS_OK;
}


ts_s32 ts_lib_gpio_read_pin(ts_u32 u32_pin_no)
{
    gpio_s_t st_gpio;
    gpio_dev_s_t *pst_dev;
    ts_u32 u32_val, u32_mask;
    ts_s32 s32_ret;

    libgpio_chk_init();

    /* 根据 GPIO 序号确定 BANK */
    s32_ret = libgpio_get_pin(u32_pin_no, (gpio_s_t *)&st_gpio, __func__, __LINE__);
    if (s32_ret != TS_OK)
        return s32_ret;

    pst_dev = (gpio_dev_s_t *)&sst_ctx.pst_grp->ast_dev[st_gpio.u32_grp_idx];
    u32_mask = (1 << st_gpio.u32_pin_idx);
    u32_val = ts_cpl_readl(pst_dev->u32_regbase + GPIO_TG_OFFSET_EXT_PORTA);
    if ((u32_val & u32_mask) > 0)
        return 1;
    else
        return 0;
}


ts_s32 ts_lib_gpio_flip_pin(ts_u32 u32_pin_no)
{
    ts_s32 s32_ret;
    ts_elec_level_en_t en_lvl;

    s32_ret = ts_lib_gpio_read_pin(u32_pin_no);
    if (0 == s32_ret)
        en_lvl = TS_ELEC_LVL_HI;
    else if (1 == s32_ret)
        en_lvl = TS_ELEC_LVL_LO;
    else
        return s32_ret;

    return ts_lib_gpio_write_pin(u32_pin_no, en_lvl);
}


ts_s32 ts_lib_gpio_set_irq_attr(ts_gpio_irq_attr_s_t *pst_irq_attr)
{
    ts_u32 u32_val, u32_set, u32_int_en, u32_int_type, u32_int_pola;
    gpio_s_t st_gpio;
    gpio_dev_s_t *pst_dev;
    ts_s32 s32_ret;
    ts_ulong ul_flags;

    libgpio_chk_init();
    uapi_chk_ptr(pst_irq_attr, TS_ENULLPTR);

    /* 根据 GPIO 序号确定 BANK */
    s32_ret = libgpio_get_pin(pst_irq_attr->u32_pin_no, (gpio_s_t *)&st_gpio, __func__, __LINE__);
    if (s32_ret != TS_OK)
        return s32_ret;

    pst_dev = (gpio_dev_s_t *)&sst_ctx.pst_grp->ast_dev[st_gpio.u32_grp_idx];
    u32_set = 1 << st_gpio.u32_pin_idx;

    /* 首先关闭中断 */
    ts_cpl_spin_lock_irqsave((ts_cpl_spinlock_s_t *)&sst_ctx.st_lock, (ts_ulong *)&ul_flags);
    u32_int_en = ts_cpl_readl(pst_dev->u32_regbase + GPIO_TG_OFFSET_INTEN);
    u32_int_en &= ~u32_set;
    ts_cpl_writel(u32_int_en, (pst_dev->u32_regbase + GPIO_TG_OFFSET_INTEN));
    ts_cpl_spin_unlock_irqrestore((ts_cpl_spinlock_s_t *)&sst_ctx.st_lock, (ts_ulong *)&ul_flags);

    if (TS_OPT_ENABLE == pst_irq_attr->en_irqen) {
        uapi_chk_ptr(pst_irq_attr->fp_irqcb, TS_ENULLPTR);

        ts_cpl_spin_lock_irqsave((ts_cpl_spinlock_s_t *)&sst_ctx.st_lock, (ts_ulong *)&ul_flags);

        if (TS_ELEC_LVL_BEDGE == pst_irq_attr->en_irq_type) {
            u32_val = ts_cpl_readl(pst_dev->u32_regbase + GPIO_TG_OFFSET_INT_BOTHEDEG);
            u32_val |= u32_set;
            ts_cpl_writel(u32_val, (pst_dev->u32_regbase + GPIO_TG_OFFSET_INT_BOTHEDEG));
        } else {
            u32_val = ts_cpl_readl(pst_dev->u32_regbase + GPIO_TG_OFFSET_INT_BOTHEDEG);
            u32_val &= ~u32_set;
            ts_cpl_writel(u32_val, (pst_dev->u32_regbase + GPIO_TG_OFFSET_INT_BOTHEDEG));

            u32_int_type = ts_cpl_readl(pst_dev->u32_regbase + GPIO_TG_OFFSET_INTTYPE_LEVEL);
            u32_int_pola = ts_cpl_readl(pst_dev->u32_regbase + GPIO_TG_OFFSET_INT_POLARITY);

            if ((TS_ELEC_LVL_LO == pst_irq_attr->en_irq_type) || (TS_ELEC_LVL_HI == pst_irq_attr->en_irq_type))
                u32_int_type &= ~u32_set;
            else
                u32_int_type |= u32_set;

             if ((TS_ELEC_LVL_LO == pst_irq_attr->en_irq_type) || (TS_ELEC_LVL_FEDGE == pst_irq_attr->en_irq_type))
                u32_int_pola &= ~u32_set;
            else
                u32_int_pola |= u32_set;

            ts_cpl_writel(u32_int_type, (pst_dev->u32_regbase + GPIO_TG_OFFSET_INTTYPE_LEVEL));
            ts_cpl_writel(u32_int_pola, (pst_dev->u32_regbase + GPIO_TG_OFFSET_INT_POLARITY));
        }

        pst_dev->afp_cb[st_gpio.u32_pin_idx] = pst_irq_attr->fp_irqcb;

        u32_int_en |= u32_set;
        ts_cpl_writel(u32_int_en, (pst_dev->u32_regbase + GPIO_TG_OFFSET_INTEN));

        ts_cpl_spin_unlock_irqrestore((ts_cpl_spinlock_s_t *)&sst_ctx.st_lock, (ts_ulong *)&ul_flags);
    }

    return TS_OK;
}


ts_s32 ts_lib_gpio_set_out(ts_s32 s32_gpio_num, ts_elec_level_en_t en_o_def)
{
    ts_gpio_attr_s_t st_attr;

    st_attr.u32_pin_no = (ts_u32)s32_gpio_num;
    st_attr.en_dir = TS_GPIO_DIR_OUTPUT;
    if (TS_ELEC_LVL_LO == en_o_def)
        st_attr.en_lvl = TS_ELEC_LVL_LO;
    else
        st_attr.en_lvl = TS_ELEC_LVL_HI;

    return ts_lib_gpio_set_attr((ts_gpio_attr_s_t *)&st_attr);
}


ts_s32 ts_lib_gpio_set_in(ts_s32 s32_gpio_num, ts_option_en_t en_debounce_en)
{
    ts_gpio_attr_s_t st_attr;

    st_attr.u32_pin_no = (ts_u32)s32_gpio_num;
    st_attr.en_dir = TS_GPIO_DIR_INPUT;
    if (TS_OPT_DISABLE == en_debounce_en)
        st_attr.en_debounce_en = TS_OPT_DISABLE;
    else
        st_attr.en_debounce_en = TS_OPT_ENABLE;

    return ts_lib_gpio_set_attr((ts_gpio_attr_s_t *)&st_attr);
}


ts_s32 ts_lib_gpio_set_attr(ts_gpio_attr_s_t *pst_attr)
{
    ts_u32 u32_val, u32_set, u32_idx;
    gpio_s_t st_gpio;
    gpio_dev_s_t *pst_dev;
    ts_s32 s32_ret;
    ts_ulong ul_flags;

    libgpio_chk_init();
    uapi_chk_ptr(pst_attr, TS_ENULLPTR);

    /* 根据 GPIO 序号确定 BANK */
    s32_ret = libgpio_get_pin(pst_attr->u32_pin_no, (gpio_s_t *)&st_gpio, __func__, __LINE__);
    if (s32_ret != TS_OK)
        return s32_ret;

    pst_dev = (gpio_dev_s_t *)&sst_ctx.pst_grp->ast_dev[st_gpio.u32_grp_idx];
    u32_set = 1 << st_gpio.u32_pin_idx;

    ts_cpl_spin_lock_irqsave((ts_cpl_spinlock_s_t *)&sst_ctx.st_lock, (ts_ulong *)&ul_flags);

    if ((pst_attr->u32_pin_no > 25) && (pst_attr->u32_pin_no < 30)) {
        u32_val = ts_cpl_readl(TS_SUC_REGBASE + 0x38);
        u32_idx = pst_attr->u32_pin_no - 26;
        u32_val &= ~((1 << u32_idx) | (1 << (u32_idx + 4)));
        if (TS_GPIO_DIR_INPUT == pst_attr->en_dir)
            u32_val |= 1 << (u32_idx + 4);

        ts_cpl_writel(u32_val, (TS_SUC_REGBASE + 0x38));
    }

    /* 配置消抖 */
    u32_val = ts_cpl_readl(pst_dev->u32_regbase + GPIO_TG_OFFSET_DEBOUNCE);
    if (TS_OPT_DISABLE == pst_attr->en_debounce_en)
        u32_val &= ~u32_set;
    else
        u32_val |= u32_set;

    ts_cpl_writel(u32_val, (pst_dev->u32_regbase + GPIO_TG_OFFSET_DEBOUNCE));

    if (TS_GPIO_DIR_OUTPUT == pst_attr->en_dir) {
        u32_val = ts_cpl_readl(pst_dev->u32_regbase + GPIO_TG_OFFSET_DR);
        if (TS_ELEC_LVL_LO == pst_attr->en_lvl)
            u32_val &= ~u32_set;
        else
            u32_val |= u32_set;

        ts_cpl_writel(u32_val, (pst_dev->u32_regbase + GPIO_TG_OFFSET_DR));
    }

    u32_val = ts_cpl_readl(pst_dev->u32_regbase + GPIO_TG_OFFSET_DDR);
    if (TS_GPIO_DIR_INPUT == pst_attr->en_dir)
        u32_val &= ~u32_set;
    else
        u32_val |= u32_set;

    ts_cpl_writel(u32_val, (pst_dev->u32_regbase + GPIO_TG_OFFSET_DDR));

    ts_cpl_spin_unlock_irqrestore((ts_cpl_spinlock_s_t *)&sst_ctx.st_lock, (ts_ulong *)&ul_flags);

    return TS_OK;
}


/* 
 * 函数名 :  ts_lib_gpio_deinit
 * 描  述 :  gpio 库去初始化
 * 输  入 :  参数           描述
 *           无             无
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          去初始化成功
 */
ts_s32 ts_lib_gpio_deinit(ts_void)
{
    if (sst_ctx.s32_fd >= 0) {
        close(sst_ctx.s32_fd);

        sst_ctx.s32_fd = -1;
        sst_ctx.pst_grp = ts_null;
    }

    return TS_OK;
}


/* 
 * 函数名 :  ts_lib_gpio_init
 * 描  述 :  初始化 GPIO 库
 * 输  入 :  参数           描述
 *           无             无
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          初始化成功
 *           TS_ERR        初始化失败
 */
ts_s32 ts_lib_gpio_init(ts_void)
{
    ts_s32 s32_ret;
    ts_u32 u32_grp_addr;

    if (ts_null == sst_ctx.pst_grp) {
        sst_ctx.s32_fd = open("/dev/gpio", O_RDWR);
        if (sst_ctx.s32_fd >= 0) {
            s32_ret = (ts_s32)ioctl(sst_ctx.s32_fd, TS_IOCTL_IOC_NR_GPIO_INIT, (ts_long)&u32_grp_addr);
            if (TS_OK == s32_ret) {
                sst_ctx.pst_grp = (gpio_grp_s_t *)u32_grp_addr;

                ts_cpl_spinlock_init((ts_cpl_spinlock_s_t *)&sst_ctx.st_lock);

                return TS_OK;
            } else {
                ts_uapi_log_err("Failed to get gpio context\n");

                ts_lib_gpio_deinit();
            }
        } else {
            ts_uapi_log_err("%s\n", TS_ENODEV_STR);
        }
    }

    return TS_ERR;
}


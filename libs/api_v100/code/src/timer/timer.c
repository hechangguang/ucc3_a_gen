 /*
 * timer.c -- OPEN TDK 对 timer 驱动的封装，实现定时器 开始定时 的功能,驱动代码除中断外，上移到用户层实现
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023-2025 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 *
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */


#include <uapi/base_v100.h>
#include <uapi/timer_v100.h>
#include <kapi/k_timer_v100.h>
#include <kapi/k_base_v100.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>


extern ts_s32 interrupt_context(ts_void);

/* TIMER 上下文 */
typedef struct libtimer_ctx_s {
    /* 设备文件句柄 */
    ts_s32 s32_fd;

    /* 从驱动获取的驱动上下文 */
    hwt_ctx_s_t *pst_hwt_ctx;
} libtimer_ctx_s_t;

static libtimer_ctx_s_t sst_ctx = {0};

#define libtimer_chk_init()\
do{\
    if (ts_null == sst_ctx.pst_hwt_ctx) {\
        ts_uapi_log_err("%s\n", TS_ENOINIT_STR);\
        return -TS_ENOINIT;\
    }\
} while(0)


/*
 * 函数名 :  ts_lib_timer_get_free
 * 描  述 :  获取空闲子设备句柄
 * 输  入 :  参数               描述
 *           无                 无
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           TS_EBUSY           获取失败，暂无空闲设备
 *           >= 0               获取成功，返回空闲子设备句柄
 */
ts_s32 ts_lib_timer_get_free(ts_void)
{
    hwt_ctx_s_t *pst_ctx;
    hwt_sub_dev_s_t *pst_sub_ctx;
    ts_s32 i;
    ts_cpl_mutex_s_t *pst_mutex;

    libtimer_chk_init();

    pst_ctx = sst_ctx.pst_hwt_ctx;
    pst_mutex = (ts_cpl_mutex_s_t *)&pst_ctx->st_mutex;

    ts_cpl_mutex_lock(pst_mutex);
    for(i = 0; i < HWT_MAX_NUM; ++i) {
        pst_sub_ctx = &pst_ctx->ast_sub_dev[i];
        if (pst_sub_ctx->u32_busy == 0) {
            pst_sub_ctx->u32_busy = 1;
            pst_sub_ctx->fp_cb = ts_null;
            ts_cpl_mutex_unlock(pst_mutex);

            return i;
        }
    }

    ts_cpl_mutex_unlock(pst_mutex);

    return -TS_EBUSY;
}


/*
 * 函数名 :  ts_lib_timer_release
 * 描  述 :  释放占用的子设备
 * 输  入 :  参数               描述
 *           s32_handle         子设备句柄
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           TS_OK              释放成功
 *           TS_ENOINIT         定时器未初始化
 *           TS_EINVAL          参数错误
 */
ts_s32 ts_lib_timer_release(ts_s32 s32_handle)
{
    hwt_ctx_s_t *pst_ctx;
    hwt_sub_dev_s_t *pst_sub_ctx;
    hwt_reg_s_t *pst_reg;
    ts_ulong ul_flags;
    ts_s32 s32_ret;

    libtimer_chk_init();

    if (s32_handle < 0 || s32_handle >= HWT_MAX_NUM)
        return -TS_EINVAL;

    pst_ctx = sst_ctx.pst_hwt_ctx;
    pst_sub_ctx = &pst_ctx->ast_sub_dev[s32_handle];
    pst_reg = (hwt_reg_s_t *)pst_sub_ctx->u32_reg;
    s32_ret = interrupt_context();
    if (0 == s32_ret) {
        ts_cpl_spin_lock_irqsave((ts_cpl_spinlock_s_t *)&pst_sub_ctx->st_lock, (ts_ulong *)&ul_flags);
        pst_reg->un_ctrl.u32_val = 0;
        pst_sub_ctx->u32_busy = 0;
        ts_cpl_spin_unlock_irqrestore((ts_cpl_spinlock_s_t *)&pst_sub_ctx->st_lock, (ts_ulong *)&ul_flags);
    } else {
        pst_reg->un_ctrl.u32_val = 0;
        pst_sub_ctx->u32_busy = 0;
    }

    return TS_OK;
}


/*
 * 函数名 :  ts_lib_timer_stop
 * 描  述 :  定时器停止定时
 * 输  入 :  参数               描述
 *           s32_handle         子设备句柄
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           TS_OK              释放成功
 *           TS_ENOINIT         定时器未初始化
 *           TS_EINVAL          参数错误
 */
ts_s32 ts_lib_timer_stop(ts_s32 s32_handle)
{
    hwt_ctx_s_t *pst_ctx;
    hwt_reg_s_t *pst_reg;
    hwt_sub_dev_s_t *pst_sub_ctx;
    ts_ulong ul_flags;
    ts_s32 s32_ret;
    volatile ts_u32 u32_eoi;

    TS_UNUSED(u32_eoi);
    libtimer_chk_init();

    if (s32_handle < 0 || s32_handle >= HWT_MAX_NUM)
        return -TS_EINVAL;

    pst_ctx = sst_ctx.pst_hwt_ctx;
    pst_sub_ctx = &pst_ctx->ast_sub_dev[s32_handle];
    pst_reg = (hwt_reg_s_t *)pst_sub_ctx->u32_reg;

    s32_ret = interrupt_context();
    if (0 == s32_ret) {
        ts_cpl_spin_lock_irqsave((ts_cpl_spinlock_s_t *)&pst_sub_ctx->st_lock, (ts_ulong *)&ul_flags);
        pst_reg->un_ctrl.u32_val = 0;
        u32_eoi = pst_reg->un_eoi.u32_val;
        ts_cpl_spin_unlock_irqrestore((ts_cpl_spinlock_s_t *)&pst_sub_ctx->st_lock, (ts_ulong *)&ul_flags);
    } else {
        pst_reg->un_ctrl.u32_val = 0;
        u32_eoi = pst_reg->un_eoi.u32_val;
    }

    return TS_OK;
}


/*
 * 函数名 :  ts_lib_timer_start
 * 描  述 :  启动定时器
 * 输  入 :  参数               描述
 *           s32_handle         子设备句柄
 *           u32_dly_us         延时时间,单位us
 *           fp_cb              定时器结束后回调
 *           pv_cb_param        回调函数参数
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           TS_OK              定时器延时完成
 *           TS_ERR             失败
 *           TS_ETIMEDOUT       超时
 *           TS_ENOINIT         未初始化
 *           TS_EINVAL          参数错误
 */
ts_s32 ts_lib_timer_start(ts_s32 s32_handle, ts_u32 u32_dly_us)
{
    hwt_ctx_s_t *pst_ctx;
    hwt_sub_dev_s_t *pst_sub_ctx;
    hwt_reg_s_t *pst_reg;
    ts_u32 u32_us_clk,u32_cnt;
    hwt_ctrl_un_t un_ctrl;
    ts_ulong ul_flags;
    ts_s32 s32_ret;

    libtimer_chk_init();

    if (s32_handle < 0 || s32_handle >= HWT_MAX_NUM)
        return -TS_EINVAL;

    pst_ctx = sst_ctx.pst_hwt_ctx;
    pst_sub_ctx = &pst_ctx->ast_sub_dev[s32_handle];
    pst_reg = (hwt_reg_s_t *)pst_sub_ctx->u32_reg;
    u32_us_clk = pst_ctx->u32_us_clk;

    /* 计算延迟所需计数值 */
    u32_cnt = u32_us_clk * u32_dly_us;

    /* 定时器属性配置 */
    un_ctrl.u32_val = 0;
    un_ctrl.enable = 1;
    un_ctrl.mode = 1;

    s32_ret = interrupt_context();
    if (0 == s32_ret) {
        /* 开始前需停止定时 */
        ts_cpl_spin_lock_irqsave((ts_cpl_spinlock_s_t *)&pst_sub_ctx->st_lock, (ts_ulong *)&ul_flags);

        pst_reg->un_ctrl.u32_val = 0;
        pst_reg->un_load_count.u32_val = u32_cnt;
        pst_reg->un_ctrl.u32_val = un_ctrl.u32_val;

        ts_cpl_spin_unlock_irqrestore((ts_cpl_spinlock_s_t *)&pst_sub_ctx->st_lock, (ts_ulong *)&ul_flags);
    } else {
        pst_reg->un_ctrl.u32_val = 0;
        pst_reg->un_load_count.u32_val = u32_cnt;
        pst_reg->un_ctrl.u32_val = un_ctrl.u32_val;
    }

    return TS_OK;
}

/*
 * 函数名 :  ts_lib_timer_set_cb
 * 描  述 :  定时器设置回调
 * 输  入 :  参数           描述
 *           s32_handle      子设备句柄
 *           fp_cb           定时器结束后回调
 *           pv_param        回调函数参数
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          成功
 *           TS_ENOINIT     未初始化
 *           TS_EINVAL      参数错误
 */
ts_s32 ts_lib_timer_set_callback(ts_s32 s32_handle, ts_hwt_cb_fp_t fp_cb, ts_void *pv_param)
{
    hwt_ctx_s_t *pst_ctx;
    hwt_sub_dev_s_t *pst_sub_ctx;

    libtimer_chk_init();

    if (s32_handle < 0 || s32_handle >= HWT_MAX_NUM)
        return -TS_EINVAL;

    pst_ctx = sst_ctx.pst_hwt_ctx;
    pst_sub_ctx = &pst_ctx->ast_sub_dev[s32_handle];

    pst_sub_ctx->fp_cb = fp_cb;
    pst_sub_ctx->pv_param = pv_param;

    return TS_OK;
}

/*
 * 函数名 :  ts_lib_timer_deinit
 * 描  述 :  timer 库去初始化
 * 输  入 :  参数           描述
 *           无             无
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          去初始化成功
 */
ts_s32 ts_lib_timer_deinit(ts_void)
{
    if (sst_ctx.s32_fd >= 0) {
        close(sst_ctx.s32_fd);

        sst_ctx.s32_fd = -1;
        sst_ctx.pst_hwt_ctx = ts_null;
    }

    return TS_OK;
}


/*
 * 函数名 :  ts_lib_timer_init
 * 描  述 :  初始化 TIMER 库
 * 输  入 :  参数           描述
 *           无             无
 * 输  出 :  参数           描述
 *           无             无
 * 返  回 :  返回值         描述
 *           TS_OK          初始化成功
 *           TS_ERR        初始化失败
 */
ts_s32 ts_lib_timer_init(ts_void)
{
    ts_s32 s32_ret;
    ts_u32 u32_hwt_ctx_addr;

    if (ts_null == sst_ctx.pst_hwt_ctx) {
        sst_ctx.s32_fd = open("/dev/hwt", O_RDWR);
        if (sst_ctx.s32_fd >= 0) {
            s32_ret = (ts_s32)ioctl(sst_ctx.s32_fd, TS_IOCTL_IOC_NR_TIMER_INIT, (ts_long)&u32_hwt_ctx_addr);
            if (TS_OK == s32_ret) {
                sst_ctx.pst_hwt_ctx = (hwt_ctx_s_t *)u32_hwt_ctx_addr;

                return TS_OK;
            } else {
                ts_uapi_log_err("Failed to get timer context\n");

                ts_lib_timer_deinit();
            }
        } else {
            ts_uapi_log_err("%s\n", TS_ENODEV_STR);
        }
    }

    return TS_ERR;
}


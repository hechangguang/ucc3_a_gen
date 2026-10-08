/* 
 * snand_flash.c -- 实现 sfc-nand flash 应用级别功能,直接调用底层 sfc 接口
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023-2025 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#include <config.h>
#include <base/ts_type.h>
#include <base/ts_errno.h>
#include <bootup.h>
#include <lib/log.h>
#include <io.h>
#include <spi_flash.h>
#include <bootup_sign_api.h>
#include <app/snand_flash.h>


/* 区分 blk 读写操作 */
#define SNAND_OPS_READ_BLK          (0)
#define SNAND_OPS_WRITE_BLK         (1)

/* 超时设定 */
#define SNAND_MAX_TIMEOUT           (500000)

/* 仅保存 ECC 报错阈值 */
#define SNAND_ECC_MIN_THR           (4)
#define SNAND_ECC_MAX_THR           (7)

/* 最大坏块跳过数 */
#define SNAND_MAX_BAD_CNT           (4)

extern ts_void lib_sfc_erase(ts_u32 u32_erase_cmd, ts_u32 u32_flash_addr, ts_u32 u32_addr_size);


typedef struct snand_ctx_s {
    /* ecc 报错阈值 */
    ts_u32 u32_ecc_thr;

    /* 发生ecc纠正位数超过阈值标志 */
    ts_u32 u32_ecc_flg;

    /* 有风险坏块的块地址 考虑ecc超过阈值的概率不是很高，一次只允许调用者处理一个坏块 */
    ts_u32 u32_risk_addr;
} snand_ctx_s_t;

static snand_ctx_s_t sst_ctx = {
    .u32_ecc_thr = SNAND_ECC_MIN_THR,
    .u32_ecc_flg = 0,
    .u32_risk_addr = 0,
};

static TS_ALWAYS_INLINE ts_s32 snand_get_page0(ts_u32 u32_address)
{
    ts_u32 u32_blk_addr, u32_blk_page0_addr;

    if (u32_address >= SNAND_SIZE)
        return -TS_EINVAL;

    u32_blk_addr = u32_address >> SNAND_BLK_ADDR_SHIFT;
    u32_blk_page0_addr = u32_blk_addr << SNAND_PAGE0_ADDR_SHIFT;

    return (ts_s32)u32_blk_page0_addr;
}


static ts_s32 snand_get_ecc_status(ts_u08 u08_status)
{
    ts_u08 au08_cmd[4];
    ts_u08 au08_rbuf[4], u08_ecc_se, u08_ecc_thr;
    ts_s32 s32_ret;

    au08_cmd[0] = SFC_NAND_CMD_READ_FEATURE;
    au08_cmd[1] = SFC_NAND_STATUS_STATUS1;
    au08_rbuf[0] = 0;
    s32_ret = sfc_read_reg((ts_u08 *)&au08_cmd, 2, (ts_u08 *)&au08_rbuf, 1);
    if (s32_ret != TS_OK)
        return TS_ERR;
    
    if ((sst_ctx.u32_ecc_thr >= SNAND_ECC_MIN_THR) && (sst_ctx.u32_ecc_thr <= SNAND_ECC_MAX_THR))
        u08_ecc_thr = sst_ctx.u32_ecc_thr - 4;
    else
        u08_ecc_thr = 0;

    u08_ecc_se = au08_rbuf[0];
    u08_ecc_se = u08_ecc_se >> SNAND_F0H_ECCSE_OFFSET;
    u08_ecc_se &= 0x03;
    if (u08_ecc_se >= u08_ecc_thr) {
        return -TS_ESNANDCORR;
    } else {
        return TS_OK;
    }
}


/* 
 * 函数名 : hook_snand_get_status
 * 描  述 : 根据关注点获取当前 Flash 状态
 * 输  入 : 参数           描述
 *          u32_type        类型为:SNAND_STATUS_BUSY,SNAND_STATUS_PROG,SNAND_STATUS_READ
 * 输  出 : 参数           描述
 *           无             无
 * 返  回 : 返回值              描述
 *           TS_OK              获取状态为正常
 *           TS_ERR             SFC 执行错误
 *          -TS_ESNANDNOTCORR   ECC 无法纠错
 *          -TS_ESNANDCORR      ECC 出错，但是被纠正，且已经超过阈值
 *          -TS_ESNANDPROG      编程错误
 *          -TS_ETIMEDOUT       获取超时
 */
ts_s32 hook_snand_get_status(ts_u32 u32_type)
{
    ts_u08 au08_cmd[4];
    ts_u08 au08_rbuf[4], u08_status;
    ts_u32 i;
    ts_s32 s32_ret;

    au08_cmd[0] = SFC_NAND_CMD_READ_FEATURE;
    au08_cmd[1] = SFC_NAND_STATUS_STATUS0;
    au08_rbuf[0] = 0;

    /* 循环等待 Flash 完成，并得到最终关心的状态 */
    for (i = 0; i < SNAND_MAX_TIMEOUT; i++) {
        s32_ret = sfc_read_reg((ts_u08 *)&au08_cmd, 2, (ts_u08 *)&au08_rbuf, 1);
        if (s32_ret != TS_OK)
            return TS_ERR;

        u08_status = au08_rbuf[0];
        if (0 == (u08_status & SNAND_C0H_OIP))
            goto _chk_status;
    }

    return -TS_ETIMEDOUT;

_chk_status:
    if (SNAND_STATUS_READ == u32_type) {
        /* 如果发生 ECC 错误，需再次读取 F0H 寄存器，查看具体错误数量是否超过阈值 */
        if (0 == (u08_status & SNAND_C0H_ECCS)) {
            return TS_OK;
        } else if (SNAND_C0H_ECCS1 == (u08_status & SNAND_C0H_ECCS)) {
            return -TS_ESNANDNOTCORR;
        } else if (SNAND_C0H_ECCS == (u08_status & SNAND_C0H_ECCS)) {
            return -TS_ESNANDCORR;
        } else {
            return snand_get_ecc_status(u08_status);
        }
    } else if (SNAND_STATUS_PROG == u32_type) {
        if ((u08_status & SNAND_C0H_FAIL) > 0) {
            return -TS_ESNANDPROG;
        } else {
            return TS_OK;
        }
    } else {
        return TS_OK;
    }
}


/* 
 * 函数名 : snand_write_page
 * 描  述 : 在页范围内写数据，包括 oob 区域, 由调用者保证 flash 已经处于空闲状态
 * 输  入 : 参数           描述
 *          u32_page_ra     页地址 row addr
 *          u32_page_ca     页内地址， 0..2175, col addr
 *          pu08_buf        待写数据缓存
 *          u32_size        待写数据大小
 * 输  出 : 参数           描述
 *           无             无
 * 返  回 : 返回值         描述
 *          u32_size        写成功
 *          TS_ERR          sfc 执行错误
 *          -TS_ETIMEDOUT   获取状态超时
 *          -TS_EINVAL      参数错误
 *          -TS_ESNANDPROG  编程失败
 */
static ts_s32 snand_write_page(ts_u32 u32_page_ra, ts_u32 u32_page_ca, ts_u08 *pu08_buf, ts_u32 u32_size)
{
    ts_u08 au8_cmd_buf[4];
    ts_u32 u32_page_size;
    ts_s32 s32_ret;

    u32_page_size = SNAND_PAGE_SIZE + SNAND_SPARE_SIZE;

    /* 判断大小是否超过页大小 */
    if ((u32_page_ca + u32_size) > u32_page_size)
        return -TS_EINVAL;

    /* 写使能，并将 SFC 配置成单线写模式 */
    lib_sfc_write_en();

    /* hook 单线程，无中断，可以持续写 SFC */
    au8_cmd_buf[0] = SFC_NAND_CMD_PAGE_PROG;
    au8_cmd_buf[1] = u32_page_ca >> 8;
    au8_cmd_buf[2] = u32_page_ca >> 0;
    tdl_sfc_write_standard((ts_u08 *)&au8_cmd_buf, 3, pu08_buf, u32_size);

    au8_cmd_buf[0] = SFC_NAND_CMD_PROG_EXC;
    au8_cmd_buf[1] = u32_page_ra >> 16;
    au8_cmd_buf[2] = u32_page_ra >> 8;
    au8_cmd_buf[3] = u32_page_ra >> 0;
    tdl_sfc_write_standard((ts_u08 *)&au8_cmd_buf, 4, ts_null, 0);

    /* 确认是否写失败，所以此处需要等待完成 */
    s32_ret = hook_snand_get_status(SNAND_STATUS_PROG);
    if (s32_ret != TS_OK)
        return s32_ret;

    return u32_size;
}


/* 
 * 函数名 : hook_snand_mark_bad
 * 描  述 : 将当前地址所在的块标注成坏块
 * 输  入 : 参数           描述
 *          u32_address    坏块所在的块内任意地址
 * 输  出 : 参数           描述
 *           无             无
 * 返  回 : 返回值         描述
 *           TS_OK          标注成功
 *          TS_ERR          sfc 执行错误
 *          -TS_EBUSY       设备忙
 *          -TS_ETIMEDOUT   获取状态超时
 *          -TS_EINVAL      参数错误
 *          -TS_ESNANDPROG  编程失败
 */
ts_s32 hook_snand_mark_bad(ts_u32 u32_address)
{
    ts_u08 au08_oob_buf[8];
    ts_u32 *pu32_ptr;
    ts_u32 u32_blk_page0_addr, u32_col_addr;
    ts_s32 s32_ret;

    s32_ret = snand_get_page0(u32_address);
    if (s32_ret < 0)
        return s32_ret;
    else
        u32_blk_page0_addr = (ts_u32)s32_ret;

    /* 空闲判断 */
    s32_ret = hook_snand_get_status(SNAND_STATUS_BUSY);
    if (s32_ret != TS_OK)
        return -TS_EBUSY;

    u32_col_addr = 1 << SNAND_PAGE_ADDR_SHIFT;
    pu32_ptr = (ts_u32 *)&au08_oob_buf;
    pu32_ptr[0] = 0;
    s32_ret = snand_write_page(u32_blk_page0_addr, u32_col_addr, (ts_u08 *)&au08_oob_buf, 2);
    if (2 == s32_ret)
        return TS_OK;
    else
        return s32_ret;
}


/* 
 * 函数名 : hook_snand_chk_bad
 * 描  述 : 检测当前地址所在的块是否是坏块
 * 输  入 : 参数                描述
 *          u32_address         坏块所在的块内任意地址
 * 输  出 : 参数                描述
 *           无                 无
 * 返  回 : 返回值              描述
 *          TS_OK               好块
 *          TS_ERR              读出错
 *          -TS_EINVAL          参数错误
 *          -TS_ESNANDBADBLK    坏块
 */
ts_s32 hook_snand_chk_bad(ts_u32 u32_address)
{
    ts_s32 s32_ret;

    if (u32_address >= SNAND_SIZE)
        return -TS_EINVAL;

    /* 空闲判断 */
    s32_ret = hook_snand_get_status(SNAND_STATUS_BUSY);
    if (s32_ret != TS_OK)
        return -TS_EBUSY;

    s32_ret = sfc_nand_chk_blk(u32_address);
    if (-TS_EBADF == s32_ret)
        return -TS_ESNANDBADBLK;
    else
        return s32_ret;
}


ts_s32 hook_snand_enable_bp(ts_void)
{
    ts_u08 au08_cmd[4];

    au08_cmd[0] = SFC_NAND_CMD_SET_FEATURE;
    au08_cmd[1] = SFC_NAND_STATUS_BP;
    au08_cmd[2] = 0x38;

    return sfc_write_reg((ts_u08 *)&au08_cmd, 3);
}


ts_s32 hook_snand_disable_bp(ts_void)
{
    ts_u08 au08_cmd[4];

    au08_cmd[0] = SFC_NAND_CMD_SET_FEATURE;
    au08_cmd[1] = SFC_NAND_STATUS_BP;
    au08_cmd[2] = 0x00;

    return sfc_write_reg((ts_u08 *)&au08_cmd, 3);
}


/* 
 * 函数名 : hook_erase_blk
 * 描  述 : 擦除当前地址所在的块
 * 输  入 : 参数           描述
 *          u32_address     flash 线性地址，spare 区域不在其中
 * 输  出 : 参数           描述
 *           无             无
 * 返  回 : 返回值         描述
 *           TS_OK          擦除成功
 *          TS_ERR          sfc 执行错误
 *          -TS_EBUSY       设备忙
 *          -TS_ESNANDPROG  编程失败
 *          -TS_EINVAL      参数错误
 */

ts_s32 hook_erase_blk(ts_u32 u32_address)
{
    ts_s32 s32_ret;
    ts_u32 u32_page_addr;

    /* 超时较长，可能前级操作是擦除 */
    s32_ret = hook_snand_get_status(SNAND_STATUS_BUSY);
    if (s32_ret != TS_OK)
        return s32_ret;

    hook_snand_disable_bp();
    u32_page_addr = u32_address >> SNAND_PAGE_ADDR_SHIFT;
    lib_sfc_erase(SFC_NAND_CMD_BLK_ERASE, u32_page_addr, 3);
    hook_snand_enable_bp();

    return hook_snand_get_status(SNAND_STATUS_PROG);
}


/* 
 * 函数名 : hook_snand_set_ecc_thr
 * 描  述 : 设置 spi-nand flash 的 ecc 可纠正的阈值，即如果设置成 5， 则发生 ecc bit 错误小于等于 5 不会报错
 * 输  入 : 参数           描述
 *          u32_thr         报错阈值，范围 [4..7]
 * 输  出 : 参数           描述
 *           无             无
 * 返  回 : 返回值          描述
 *           TS_OK          设置成功
 *           TS_ERR         设置失败，配置的参数超过范围
 */
ts_s32 hook_snand_set_ecc_thr(ts_u32 u32_thr)
{
    if ((u32_thr >= SNAND_ECC_MIN_THR) && (u32_thr <= SNAND_ECC_MAX_THR)) {
        sst_ctx.u32_ecc_thr = u32_thr;

        return TS_OK;
    } else {
        return TS_ERR;
    }
}


/* 
 * 函数名 : hook_snand_get_ecc_thr
 * 描  述 : 返回 spi-nand flash 的 ecc 可纠正的阈值
 * 输  入 : 参数           描述
 *           无             无
 * 输  出 : 参数           描述
 *           无             无
 * 返  回 : 返回值          描述
 *           4..7          报错阈值，范围 [4..7]
 */
ts_u32 hook_snand_get_ecc_thr(ts_void)
{
    return sst_ctx.u32_ecc_thr;
}



static ts_s32 snand_preload_page(ts_u32 u32_page_addr)
{
    ts_u08 au08_cmd_buf[4];
    ts_s32 s32_ret;

    au08_cmd_buf[0] = SFC_NAND_CMD_PRELOAD;
    au08_cmd_buf[1] = u32_page_addr >> 16;
    au08_cmd_buf[2] = u32_page_addr >> 8;
    au08_cmd_buf[3] = u32_page_addr;

    sfc_write_reg((ts_u08 *)&au08_cmd_buf, 4);

    /* 等待完成 */
    s32_ret = hook_snand_get_status(SNAND_STATUS_READ);

    return s32_ret;
}


/* 
 * 函数名 : snand_read_page
 * 描  述 : hook 限定在双线模式；读取一页,由调用者保证 flash 已经处于空闲状态
 * 输  入 : 参数                描述
 *          u32_page_ra         页地址 row addr
 *          u32_page_ca         页内地址， 0..2175, col addr
 *          u32_size            读数据大小
 * 输  出 : 参数                描述
 *          pu08_buf            读数据缓存
 * 返  回 : 返回值              描述
 *          u32_size            读成功
 *          TS_ERR              sfc 执行错误
 *          -TS_ETIMEDOUT       获取状态超时
 *          -TS_EINVAL          参数错误
 *          -TS_ESNANDNOTCORR   ECC 无法纠错
 *          -TS_ESNANDCORR      ECC 出错，但是被纠正，且已经超过阈值
 */
ts_s32 snand_read_page(ts_u32 u32_page_ra, ts_u32 u32_page_ca, ts_u08 *pu08_buf, ts_u32 u32_size)
{
    ts_s32 s32_ret;
    ts_u32 u32_page_size;
    ts_u08 au8_tbuf[8];
    ts_u32 u32_fix_col_addr;

    u32_page_size = SNAND_PAGE_SIZE + SNAND_SPARE_SIZE;

    /* 判断大小是否超过页大小 */
    if ((u32_page_ca + u32_size) > u32_page_size)
        return -TS_EINVAL;

    s32_ret = snand_preload_page(u32_page_ra);
    if (s32_ret != TS_OK) {
        if (-TS_ESNANDCORR == s32_ret) {
            sst_ctx.u32_ecc_flg = 1;
            if (0 == sst_ctx.u32_risk_addr)
                sst_ctx.u32_risk_addr = u32_page_ra;
        } else {
            return s32_ret;
        }
    }

    /* dummy_shift */
    u32_fix_col_addr = u32_page_ca << 8;

    /* 配置成双线读模式 */
    tdl_sfc_config(SFC_IO_DUAL, SFC_OPS_READ, 0);
    au8_tbuf[3] = SFC_NAND_CMD_DUAL_READ_PAGE;
    au8_tbuf[5] = u32_fix_col_addr >> 16;
    au8_tbuf[6] = u32_fix_col_addr >> 8;
    au8_tbuf[7] = 0;
    s32_ret = tdl_sfc_xfer_multi((ts_u32 *)&au8_tbuf, 2, (ts_u32 *)pu08_buf, u32_size);
    if (TS_OK == s32_ret)
        return u32_size;
    else
        return s32_ret;
}



static ts_s32 snand_rw_blk(ts_u32 u32_address, ts_u08 *pu08_buf, ts_u32 u32_size, ts_u32 u32_rw)
{
    ts_u32 u32_page_size, u32_page_addr, u32_cur_addr, u32_col_addr;
    ts_u32 u32_cnt, u32_pre_cnt, u32_rw_cnt;
    ts_u08 *pu08_rw_buf;
    ts_s32 s32_ret;

    if (u32_address >= SNAND_SIZE)
        return -TS_EINVAL;

    u32_cur_addr = u32_address;
    u32_page_size = SNAND_PAGE_SIZE;
    u32_page_addr = u32_cur_addr >> SNAND_PAGE_ADDR_SHIFT;
    u32_col_addr = u32_cur_addr & (u32_page_size - 1);

    /* 不允许出现跨块 */
    if ((u32_col_addr + u32_size) > SNAND_BLK_SIZE)
        return -TS_EINVAL;

    /* 空闲判断 */
    s32_ret = hook_snand_get_status(SNAND_STATUS_BUSY);
    if (s32_ret != TS_OK)
        return -TS_EBUSY;

    if (SNAND_OPS_WRITE_BLK == u32_rw)
        hook_snand_disable_bp();

    u32_rw_cnt = 0;
    pu08_rw_buf = pu08_buf;
    u32_cnt = u32_size;
    while (1) {
        if ((0 == u32_cnt) || (u32_cur_addr >= SNAND_SIZE))
            break;

        if (u32_cnt > u32_page_size)
            u32_pre_cnt = u32_page_size;
        else
            u32_pre_cnt = u32_cnt;

        /* 涉及到页面缓冲，不允许跨页写入 */
        if (u32_col_addr > 0) {
            if ((u32_col_addr + u32_pre_cnt) > u32_page_size)
                u32_pre_cnt = u32_page_size - u32_col_addr;
        }

        if (SNAND_OPS_READ_BLK == u32_rw) {
            s32_ret = snand_read_page(u32_page_addr, u32_col_addr, pu08_rw_buf, u32_pre_cnt);
            if (s32_ret != u32_pre_cnt) {
                ts_log(TS_LOG_ERR, "%#x read fail, ret=%d(%x)\n", u32_cur_addr, s32_ret, s32_ret);

                return s32_ret;
            }
        } else {
            s32_ret = snand_write_page(u32_page_addr, u32_col_addr, pu08_rw_buf, u32_pre_cnt);
            if (s32_ret != (ts_s32)u32_pre_cnt) {
                hook_snand_enable_bp();
                ts_log(TS_LOG_ERR, "%#x write fail, ret=%d(%x)\n", u32_cur_addr, s32_ret, s32_ret);

                return s32_ret;
            }
        }

        u32_page_addr++;
        u32_col_addr = 0;
        u32_cnt -= u32_pre_cnt;
        pu08_rw_buf += u32_pre_cnt;
        u32_rw_cnt += u32_pre_cnt;
    }

    if (SNAND_OPS_WRITE_BLK == u32_rw)
        hook_snand_enable_bp();

    return u32_rw_cnt;
}


/* 
 * 函数名 : hook_snand_read_blk
 * 描  述 : 读一块数据
 * 输  入 : 参数                描述
 *          u32_address         flash 线性地址，spare 区域不在其中
 *          u32_size            读数据大小
 * 输  出 : 参数                描述
 *          pu08_buf            读数据缓存
 * 返  回 : 返回值              描述
 *          > 0                 读个数
 *          TS_ERR              sfc 执行错误
 *          -TS_ETIMEDOUT       获取状态超时
 *          -TS_EINVAL          参数错误
 *          -TS_EBUSY           设备忙
 *          -TS_ESNANDNOTCORR   ECC 无法纠错
 *          -TS_ESNANDCORR      ECC 出错，但是被纠正，且已经超过阈值
 */
ts_s32 hook_snand_read_blk(ts_u32 u32_address, ts_u08 *pu08_buf, ts_u32 u32_size)
{
    return snand_rw_blk(u32_address, pu08_buf, u32_size, SNAND_OPS_READ_BLK);
}


/* 
 * 函数名 : hook_snand_write_blk
 * 描  述 : 写一块数据
 * 输  入 : 参数           描述
 *          u32_address     flash 线性地址，spare 区域不在其中
 *          pu08_buf        待写数据缓存
 *          u32_size        待写数据大小
 * 输  出 : 参数           描述
 *           无             无
 * 返  回 : 返回值         描述
 *          > 0             写入个数
 *          TS_ERR          sfc 执行错误
 *          -TS_ETIMEDOUT   获取状态超时
 *          -TS_EINVAL      参数错误
 *          -TS_EBUSY       设备忙
 *          -TS_ESNANDPROG  编程失败
 */
ts_s32 hook_snand_write_blk(ts_u32 u32_address, ts_u08 *pu08_buf, ts_u32 u32_size)
{
    return snand_rw_blk(u32_address, pu08_buf, u32_size, SNAND_OPS_WRITE_BLK);
}


/* 
 * 函数名 : hook_snand_write
 * 描  述 : 写数据,带擦除，带校验
 * 输  入 : 参数           描述
 *          u32_address     flash 线性地址，spare 区域不在其中
 *          pu08_buf        待写数据缓存
 *          u32_size        待写数据大小
 * 输  出 : 参数           描述
 *           无             无
 * 返  回 : 返回值         描述
 *           0              写失败
 *            u32_size      写成功
 */
ts_u32 hook_snand_write(ts_u32 u32_address, ts_u08 *pu08_buf, ts_u32 u32_size, ts_u32 u32_bad_max_cnt)
{
    ts_u32 u32_cnt, u32_col_addr, u32_cur_addr;
    ts_u32 u32_wr_cnt, u32_pre_cnt, u32_ignore;
    ts_u08 *pu08_wr_buf;
    ts_s32 s32_ret;

    
    /* 不允许超过 flash 范围 */
    if (((u32_address + u32_size) > SNAND_SIZE) || (ts_null == pu08_buf))
        return -TS_EINVAL;

    /* 超时较长，可能前级操作是擦除 */
    s32_ret = hook_snand_get_status(SNAND_STATUS_BUSY);
    if (s32_ret != TS_OK)
        return 0;

    u32_cnt = u32_size;
    u32_cur_addr = u32_address;
    u32_col_addr = u32_address & (SNAND_BLK_SIZE - 1);
    u32_ignore = 0;
    pu08_wr_buf = pu08_buf;
    u32_wr_cnt = 0;
    while (1) {
        if (0 == u32_cnt)
            break;

        /* 查看是否是坏块 */
        s32_ret = sfc_nand_chk_blk(u32_cur_addr);
        if (s32_ret != TS_OK) {
            u32_cur_addr += SNAND_BLK_SIZE;
            u32_ignore++;
            if (u32_ignore > u32_bad_max_cnt)
                break;
            else
                continue;
        }

        if (u32_cnt > SNAND_BLK_SIZE)
            u32_pre_cnt = SNAND_BLK_SIZE;
        else
            u32_pre_cnt = u32_cnt;

        /* 涉及到坏块检测，不允许跨块操作 */
        if (u32_col_addr > 0) {
            if ((u32_col_addr + u32_pre_cnt) > SNAND_BLK_SIZE)
                u32_pre_cnt = SNAND_BLK_SIZE - u32_col_addr;
        }

        s32_ret = hook_snand_write_blk(u32_cur_addr, pu08_wr_buf, u32_pre_cnt);
        if (s32_ret != u32_pre_cnt)
            return s32_ret;
      
        u32_cnt -= u32_pre_cnt;
        u32_cur_addr += u32_pre_cnt;
        pu08_wr_buf += u32_pre_cnt;
        //u32_ignore = 0;
        u32_col_addr = 0;
        u32_wr_cnt += u32_pre_cnt;
    }

    return u32_wr_cnt;
}


/* 
 * 函数名 : hook_snand_read
 * 描  述 : 读数据
 * 输  入 : 参数           描述
 *          u32_address     flash 线性地址，spare 区域不在其中
 *          u32_size        读数据大小
 *          u32_max_cnt     读取时允许的最多坏块数
 *          u32_en          使能纠错位数超过阈值标记为坏块
 * 输  出 : 参数           描述
 *          pu08_buf        读数据缓存
 *          pu32_flg        读取时由于发生ecc纠正位数达到阈值而将块标记为坏块事件的标志
 * 返  回 : 返回值         描述
 *           0              读失败
 *          u32_size        读成功
 */
ts_u32 hook_snand_read(ts_u32 u32_address, ts_u08 *pu08_buf, ts_u32 u32_size)
{
    ts_u32 u32_cnt, u32_col_addr, u32_cur_addr;
    ts_u32 u32_rd_cnt, u32_pre_cnt, u32_ignore;
    ts_u08 *pu08_rd_buf;
    ts_s32 s32_ret;

    /* 不允许超过 flash 范围 */
    if (((u32_address + u32_size) > SNAND_SIZE) || (ts_null == pu08_buf))
        return -TS_EINVAL;

    /* 超时较长，可能前级操作是擦除 */
    s32_ret = hook_snand_get_status(SNAND_STATUS_BUSY);
    if (s32_ret != TS_OK)
        return 0;

    /* 清除ecc纠正位数超过阈值标志 和 风险块地址*/
    sst_ctx.u32_ecc_flg = 0;
    sst_ctx.u32_risk_addr = 0;

    u32_cnt = u32_size;
    u32_cur_addr = u32_address;
    u32_col_addr = u32_address & (SNAND_BLK_SIZE - 1);
    u32_ignore = 0;
    pu08_rd_buf = pu08_buf;
    u32_rd_cnt = 0;
    while (1) {
        if (0 == u32_cnt)
            break;

        /* 查看是否是坏块 */
        s32_ret = sfc_nand_chk_blk(u32_cur_addr);
        if (s32_ret != TS_OK) {
            u32_cur_addr += SNAND_BLK_SIZE;
            u32_ignore++;
            if (u32_ignore > SNAND_MAX_BAD_CNT)
                break;
            else
                continue;
        }

        if (u32_cnt > SNAND_BLK_SIZE)
            u32_pre_cnt = SNAND_BLK_SIZE;
        else
            u32_pre_cnt = u32_cnt;

        /* 涉及到坏块检测，不允许跨块操作 */
        if (u32_col_addr > 0) {
            if ((u32_col_addr + u32_pre_cnt) > SNAND_BLK_SIZE)
                u32_pre_cnt = SNAND_BLK_SIZE - u32_col_addr;
        }

        s32_ret = hook_snand_read_blk(u32_cur_addr, pu08_rd_buf, u32_pre_cnt);
        if (s32_ret != u32_pre_cnt)
            return s32_ret;

        u32_cnt -= u32_pre_cnt;
        u32_cur_addr += u32_pre_cnt;
        pu08_rd_buf += u32_pre_cnt;
        //u32_ignore = 0;
        u32_col_addr = 0;
        u32_rd_cnt += u32_pre_cnt;
    }

    return u32_rd_cnt;
}


/* 
 * 函数名 : hook_snand_get_ecc_flg
 * 描  述 : 查看是否有由于发生ecc纠正位数达到阈值而将块标记为坏块的情况
 * 输  入 : 参数           描述
 *          无              无 
 * 输  出 : 参数           描述
 *          无              无
 * 返  回 : 返回值         描述
 *           1              由于ecc位数超过阈值标记块为坏块
 *           0              未标记为坏块
 */
ts_u32 hook_snand_get_ecc_flg(ts_void)
{
    return sst_ctx.u32_ecc_flg;
}


/* 
 * 函数名 : hook_snand_get_risk_addr
 * 描  述 : 获取风险块的地址
 * 输  入 : 参数           描述
 *          无              无 
 * 输  出 : 参数           描述
 *          无              无
 * 返  回 : 返回值         描述
 *           1              由于ecc位数超过阈值标记块为坏块
 *           0              未标记为坏块
 */
ts_u32 hook_snand_get_risk_addr(ts_void)
{
    return sst_ctx.u32_risk_addr;
}


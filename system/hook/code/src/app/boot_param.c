/*
 * boot_param.c -- 关于启动参数读写接口
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2022-2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
*/


#include <config.h>
#include <base/ts_type.h>
#include <base/ts_errno.h>
#include <base/ts_log.h>
#include <lib/log.h>
#include <app/snand_flash.h>
#include <spi_flash.h>
#include <ts_string.h>
#include <bootup_api.h>
#include <crc.h>
#include <app/boot_param.h>


/* 
 * 函数名 : bt_param_read
 * 描  述 : nandflash中读取启动参数
 * 输  入 : 参数           描述
 *          u32_addr      启动参数在nand中的地址
 * 输  出 : 参数           描述
 *          pst_param      从nandflash中读取的启动参数
 * 返  回 : 返回值         描述
 *          TS_OK         读取成功
 *          TS_ERR        读取失败
 */
static ts_s32 bt_param_read(ts_u32 u32_addr, bt_param_s_t *pst_param)
{
    ts_u32 u32_cur_addr, u32_blk_addr, u32_blk_size;
    ts_u08 *pu08_rd_buf;
    ts_s32 s32_ret;
    ts_u32 u32_rd_size, u32_rd_cnt;
    bt_param_s_t st_r_param;

    u32_cur_addr = u32_addr;
    u32_rd_size = sizeof(bt_param_s_t);
    pu08_rd_buf = (ts_u08 *)&st_r_param;
    u32_blk_size = 1 << SNAND_BLK_ADDR_SHIFT;
    u32_blk_addr = u32_cur_addr & (~(u32_blk_size - 1));

    /* 将返回启动参数魔术字直接赋值，避免第一次读就读到0xffffffff */
    pst_param->u32_magic = 0xffffffff;

    /* 空闲判断 */
    s32_ret = hook_snand_get_status(SNAND_STATUS_BUSY);
    if (s32_ret != TS_OK)
        return -TS_EBUSY;

    while (1) {
        if (u32_cur_addr >= (u32_blk_addr + u32_blk_size))
            return TS_ERR;

        u32_rd_cnt = hook_snand_read(u32_cur_addr, pu08_rd_buf, u32_rd_size);
        if (u32_rd_cnt != u32_rd_size)
            return TS_ERR;

        if (0xffffffff == st_r_param.u32_magic) {

            /* 返回到上一页读取，多读一次，代替每次读到magic时都要拷贝一次 */
            u32_cur_addr -= SNAND_PAGE_SIZE;
            if (u32_cur_addr >= u32_blk_addr) {
                pu08_rd_buf = (ts_u08 *)pst_param;
                u32_rd_cnt = hook_snand_read(u32_cur_addr, pu08_rd_buf, u32_rd_size);
                if (u32_rd_cnt != u32_rd_size)
                    return TS_ERR;
                    
                return TS_OK;
            }
            
            return TS_ERR;
        }
        
        u32_cur_addr += SNAND_PAGE_SIZE;
    }

    return TS_OK;
}

/* 
 * 函数名 : bt_param_write
 * 描  述 : nandflash中写入启动参数
* 输  入 : 参数                 描述
 *          u32_addr            启动参数在nand中的地址
 * 输  出 : 参数                描述
 *          pst_boot_param      读取启动参数
 * 返  回 : 返回值              描述
 *          TS_OK               读取成功
 *          TS_ERR              读取失败
 */
static ts_s32 bt_param_write(ts_u32 u32_address, bt_param_s_t *pst_param)
{
    ts_u32 u32_cur_addr, u32_blk_addr, u32_blk_size;
    ts_u08 *pu08_rd_buf;
    ts_s32 s32_ret;
    ts_u32 u32_rw_size, u32_rw_cnt;
    bt_param_s_t st_r_param;

    u32_cur_addr = u32_address;
    u32_blk_size = 1 << SNAND_BLK_ADDR_SHIFT;
    u32_blk_addr = u32_cur_addr &(~(u32_blk_size - 1));
    u32_rw_size = sizeof(bt_param_s_t);

    /* 空闲判断 */
    s32_ret = hook_snand_get_status(SNAND_STATUS_BUSY);
    if (s32_ret != TS_OK)
        return -TS_EBUSY;

    hook_snand_disable_bp();

    while(1) {
        if (u32_cur_addr >= (u32_blk_addr + u32_blk_size)) {
            
            s32_ret = hook_erase_blk(u32_blk_addr);
            if (s32_ret != TS_OK) 
                hook_snand_mark_bad(u32_cur_addr);
            
            /* 不需要考虑地址偏移，nand的读写都会跳过坏块 */
            u32_cur_addr = u32_blk_addr;
        }

        pu08_rd_buf = (ts_u08 *)&st_r_param;
        u32_rw_cnt = hook_snand_read(u32_cur_addr, pu08_rd_buf, u32_rw_size);
        if (u32_rw_cnt != u32_rw_size) {
            hook_snand_enable_bp();

            return TS_ERR;
        }
            
        if (0xffffffff == st_r_param.u32_magic) {
            pu08_rd_buf = (ts_u08 *)pst_param;

            /* 将入参的启动参数写入 */
            u32_rw_cnt = hook_snand_write(u32_cur_addr, (ts_u08 *)pu08_rd_buf, u32_rw_size,1);
            if (u32_rw_cnt != u32_rw_size) {
                hook_snand_enable_bp();
    
                return TS_ERR;
            }

            hook_snand_enable_bp();

            return TS_OK;
        }

        u32_cur_addr += SNAND_PAGE_SIZE;
    }
}

/* 
 * 函数名 : get_bt_param
 * 描  述 : 获取启动参数
 * 输  入 : 参数           描述
 *          无             无  
 * 输  出 : 参数           描述
 *          pst_param      读取启动参数
 * 返  回 : 返回值         描述
 *          TS_OK         读取成功
 *          TS_ERR        读取失败
 */
ts_s32 get_bt_param(bt_param_s_t *pst_param)
{
    bt_param_s_t ast_param[2];
    ts_u32 au32_bt_param_addr[2];
    ts_u32 au32_status[2];
    ts_s32 s32_ret;
    ts_u08 *pu08_buf;
    ts_u32 u32_calc_crc, u32_verify_crc, u32_size;
    ts_u32 i;

    u32_size = sizeof (bt_param_s_t);
    au32_bt_param_addr[0] = BT_PARAM_A_ADDRESS;
    au32_bt_param_addr[1] = BT_PARAM_B_ADDRESS;
    for (i = 0;i < 2; ++i) {
        au32_status[i] = 0;
        s32_ret = bt_param_read(au32_bt_param_addr[i], (bt_param_s_t *)&ast_param[i]);
        if (s32_ret != TS_OK) 
            continue;

        /* 启动参数校验值计算时需要把校验值提前取出来，校验值不参与计算 */
        u32_verify_crc = ast_param[i].u32_crc;
        ast_param[i].u32_crc = 0;
        
        pu08_buf = (ts_u08 *)&ast_param[i];
        u32_calc_crc = verify_lib_calc_crc32(0x00, pu08_buf, u32_size);
        printf("read param calc crc %x read crc %x\n",u32_calc_crc, u32_verify_crc);
        if (u32_calc_crc == u32_verify_crc)
            au32_status[i] = 1;
        else
            au32_status[i] = 0;
    }

    if (1 == au32_status[0] && 1 == au32_status[1]) {
        ts_memcpy((ts_void *)pst_param, (const ts_void *)&ast_param[0], u32_size);

        return TS_OK;
    } else if (0 == au32_status[0] && 0 == au32_status[1]) {
        printf("crc fail double\n");
        
        return TS_ERR;
    } else {
        /* 为保证双启动参数保存的数据一致，需要将校验不通过的重写 */
        if(1 == au32_status[0]) {
            ts_memcpy((ts_void *)pst_param, (const ts_void *)&ast_param[0], u32_size);
            bt_param_write(au32_bt_param_addr[1], &ast_param[0]);
        } else {
            ts_memcpy((ts_void *)pst_param, (const ts_void *)&ast_param[1], u32_size);
            bt_param_write(au32_bt_param_addr[0], &ast_param[1]);
        }

        printf("occur one err,and cpy\n");
        return TS_OK;
    }
}


/* 
 * 函数名 : set_bt_param
 * 描  述 : 向FLASH中写入
 * 输  入 : 参数            描述
 *          pst_boot_param    写入nand-flash的启动参数
 * 输  出 : 参数            描述
 *          无              无
 * 返  回 : 返回值         描述
 *          TS_OK         设置成功
 *          TS_ERR        设置失败
 */
ts_s32 set_bt_param(bt_param_s_t *pst_boot_param)
{
    ts_s32 s32_ret;
    ts_u32 u32_crc;
    
    /* 写入前计算crc校验 */
    pst_boot_param->u32_crc = 0;
    u32_crc = verify_lib_calc_crc32(0x00, (ts_u08 *)pst_boot_param, sizeof(bt_param_s_t));
    pst_boot_param->u32_crc = u32_crc;

    ts_log(TS_LOG_INFO, "set_bt_param crc %#x\n", pst_boot_param->u32_crc);

    s32_ret = bt_param_write(BT_PARAM_A_ADDRESS, pst_boot_param);
    s32_ret |= bt_param_write(BT_PARAM_B_ADDRESS, pst_boot_param);

    return s32_ret;
}
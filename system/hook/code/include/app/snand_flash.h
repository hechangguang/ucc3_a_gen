/*
 * snand_flash.h -- nand_flash相关定义
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2022-2025 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
*/
#ifndef __SNAND_FLASH_H__
#define __SNAND_FLASH_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */


#include <base/ts_type.h>

/* spi-nand flash 页大小 */
#define SNAND_PAGE_SIZE             (2048)

/* 每页第 2048 byte 为工厂坏块占用，后 63B 为用户使用， 尾部 64B 为 ECC 使用 */
#define SNAND_SPARE_SIZE            (128)

/* spi-nand flash 块大小 */
#define SNAND_BLK_SIZE              (131072)

/* spi-nand flash 块内页个数 */
#define SNAND_PAGES_PRE_BLK         (64)

/* spi-nand flash 总大小 */
#define SNAND_SIZE                  (0x8000000)

/* spi-nand flash 页地址偏移 */
#define SNAND_PAGE_ADDR_SHIFT       (11)

/* spi-nand flash 块地址偏移 */
#define SNAND_BLK_ADDR_SHIFT        (17)

/* spi-nand flash 块地址到页地址转换偏移 */
#define SNAND_PAGE0_ADDR_SHIFT      (SNAND_BLK_ADDR_SHIFT - SNAND_PAGE_ADDR_SHIFT)

#define SNAND_BLK_NORMAL    (1)
#define SNAND_BLK_BAD       (2)

/* Function list */
ts_s32 hook_snand_get_status(ts_u32 u32_type);
ts_u32 hook_snand_write(ts_u32 u32_address, ts_u08 *pu08_buf, ts_u32 u32_size, ts_u32 u32_bad_max_cnt);
ts_u32 hook_snand_read(ts_u32 u32_address, ts_u08 *pu08_buf, ts_u32 u32_size);
ts_s32 hook_erase_blk(ts_u32 u32_address);
ts_s32 hook_snand_mark_bad(ts_u32 u32_address);
ts_s32 hook_snand_enable_bp(ts_void);
ts_s32 hook_snand_disable_bp(ts_void);
ts_u32 hook_snand_get_ecc_flg(ts_void);
ts_u32 hook_snand_get_risk_addr(ts_void);
ts_s32 hook_snand_chk_bad(ts_u32 u32_address);


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /*__SNAND_FLASH_H__ */


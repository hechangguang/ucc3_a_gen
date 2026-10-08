/* 
 * i2c_v100.h -- i2c v100 驱动接口头文件
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#ifndef __TDL_I2C_V100_H__
#define __TDL_I2C_V100_H__

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#include <config.h>
#include <base/ts_type.h>
#include <base/ts_errno.h>

#include <uapi/i2c_v100.h>
#include <cpl/ts_cpl.h>

typedef struct slv_req_s {
    ts_cpl_list_head_s_t st_list;

    ts_u08 *pu08_buf;
    ts_u32 u32_size;

    ts_u32 u32_w_idx;
} slv_req_s_t;

/* for slv */
typedef struct ts_i2c_dev_s {
    /* 寄存器基地址 */
    ts_u32 u32_regbase;

    /* 内部互斥锁 */
    ts_cpl_mutex_s_t st_mutex;

    ts_u16 u16_id;
    ts_u16 u16_fifo_depth;
    ts_u32 u32_clk;

    /* 存放当前配置 */
    ts_i2c_attr_s_t st_attr;

    /* 存放当前从设备的属性 */
    ts_i2c_slv_attr_s_t st_slv_attr;

    /* 从设备当前寄存器地址 */
    ts_u16 u16_reg_addr;

    /* 当前工作节点 */
    slv_req_s_t *pst_slv_req;
    ts_void *pv_lslv;

    /* 从设备 free 和 work 节点 */
    ts_cpl_list_head_s_t st_slv_free_hdr;
    ts_cpl_list_head_s_t st_slv_rdy_hdr;

    /* 用于接收完成和 stop 中断的同步 */
    ts_u32 u32_rdy_cnt;
    ts_u32 u32_stop_cnt;

    /* 从设备用于 16bit 数据处理 */
    ts_u08 u08_slv_remain;
    ts_u08 u08_slv_val;

    /* 挂读缓存 spin 锁 */
    ts_cpl_spinlock_s_t st_slv_lock;

    /* 读信号量 */
    ts_cpl_semphr_s_t st_slv_sema;

    /* 内部缓存基地址和大小 */
    ts_void *pv_slv_mem;
} ts_i2c_dev_s_t;


/* ioctl */
enum {
    /* i2c 设置属性 */
    IOC_NR_I2C_SET_ATTR = 0,

    /* i2c 获取属性 */
    IOC_NR_I2C_GET_ATTR,

    IOC_NR_I2C_READ,

    IOC_NR_I2C_WRITE,

    IOC_NR_I2C_START_SLV,

    IOC_NR_I2C_READ_SLV,

    IOC_NR_I2C_BUTT,
};

#define TS_I2C_IOCTL_SET_ATTR       TS_CPL_IOW(TS_IOC_MAGIC_IIC, IOC_NR_I2C_SET_ATTR, ts_i2c_attr_s_t)
#define TS_I2C_IOCTL_GET_ATTR       TS_CPL_IOW(TS_IOC_MAGIC_IIC, IOC_NR_I2C_GET_ATTR, ts_i2c_attr_s_t)
#define TS_I2C_IOCTL_READ           TS_CPL_IOW(TS_IOC_MAGIC_IIC, IOC_NR_I2C_READ, ts_i2c_msg_s_t)
#define TS_I2C_IOCTL_WRITE          TS_CPL_IOW(TS_IOC_MAGIC_IIC, IOC_NR_I2C_WRITE, ts_i2c_msg_s_t)
#define TS_I2C_IOCTL_START_SLV      TS_CPL_IOW(TS_IOC_MAGIC_IIC, IOC_NR_I2C_START_SLV, ts_i2c_slv_attr_s_t)
#define TS_I2C_IOCTL_READ_SLV       TS_CPL_IOW(TS_IOC_MAGIC_IIC, IOC_NR_I2C_READ_SLV, ts_i2c_msg_s_t)


/* 
 * 函数名 :  i2c_tdl_init
 * 描  述 :  驱动层按照默认配置初始化。
 * 输  入 :  参数               描述
 *           pst_dev           i2c 设备
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           无                 无
 */
ts_void i2c_tdl_init(ts_i2c_dev_s_t *pst_dev);

/* 
 * 函数名 :  i2c_tdl_xfer
 * 描  述 :  具体驱动实现传输功能
 * 输  入 :  参数               描述
 *           pv_dev             i2c 设备
 *           pst_msg            传输参数
 * 输  出 :  参数               描述
 *           无                 无
 * 返  回 :  返回值             描述
 *           TS_OK               正确传输
 *           TS_ERR             传输失败
 */
ts_s32 i2c_tdl_xfer(ts_i2c_dev_s_t *pv_dev, ts_i2c_msg_s_t *pst_msg);


ts_s32 i2c_tdl_read(ts_i2c_dev_s_t *pst_dev, ts_i2c_msg_s_t *pst_msg);
ts_s32 i2c_tdl_write(ts_i2c_dev_s_t *pst_dev, ts_i2c_msg_s_t *pst_msg);
ts_s32 i2c_tdl_wait_idle(ts_u32 u32_dev_regbase, ts_u32 u32_time_out);

ts_s32 i2c_tdl_get_attr(ts_i2c_dev_s_t *pst_dev, ts_i2c_attr_s_t *pst_attr);
ts_s32 i2c_tdl_set_attr(ts_i2c_dev_s_t *pst_dev, const ts_i2c_attr_s_t *pst_attr);

/* 中断处理 */
ts_void i2c_tdl_hirq(ts_i2c_dev_s_t *pst_dev);

/* 从设备接口 */
ts_s32 i2c_tdl_start_slv_attr(ts_i2c_dev_s_t *pst_dev, ts_i2c_slv_attr_s_t *pst_slv_attr);
ts_s32 i2c_tdl_read_slv(ts_i2c_dev_s_t *pst_dev, ts_i2c_msg_s_t *pst_msg);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* __TDL_I2C_V100_H__ */

/* 
 * i2c.c -- OPEN TDK 对 i2c 驱动的封装，实现读写 i2c 的功能
 * Created by : Top-Silicon Software Team
 * Copyright : (C) 2023 Hangzhou Top-Silicon Microelectronics Co., Ltd. All rights reserved.
 * 
 * This file is the property of Hangzhou Top-Silicon Microelectronics Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Hangzhou Top-Silicon Microelectronics Co., Ltd.
 */
#include <base/ts_type.h>
#include <base/ts_errno.h>
#include <cpl/ts_cpl.h>
#include <uapi/i2c_v100.h>
#include <uapi/base_v100.h>
#include <kapi/k_i2c_v100.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>


static ts_s32 i2c_do_read(ts_s32 s32_handle, ts_u32 u32_type, ts_u32 u32_dev_addr, \
            ts_u32 u32_reg_addr, ts_void *pv_rd_buf, ts_u32 u32_cnt)
{
    ts_i2c_msg_s_t st_msg;

    if ((s32_handle < 0) || (ts_null == pv_rd_buf) || (0 == u32_cnt))
        return -TS_EINVAL;

    st_msg.u16_dev_addr = (ts_u16)u32_dev_addr;
    st_msg.u16_reg_addr = (ts_u16)u32_reg_addr;
    st_msg.pv_buf = pv_rd_buf;
    st_msg.u32_buf_size = u32_cnt;

    st_msg.u08_mode = u32_type;
    st_msg.u08_ignore_reg_addr = 0;
    st_msg.pst_attr = ts_null;

    return (ts_s32)ioctl(s32_handle, TS_I2C_IOCTL_READ, (ts_long)&st_msg);
}


static ts_s32 i2c_do_write(ts_s32 s32_handle, ts_u32 u32_type, ts_u32 u32_dev_addr, \
                ts_u32 u32_reg_addr, ts_void *pv_wr_buf, ts_u32 u32_cnt)
{
    ts_i2c_msg_s_t st_msg;

    if ((s32_handle < 0) || (ts_null == pv_wr_buf) || (0 == u32_cnt))
        return -TS_EINVAL;

    st_msg.u16_dev_addr = (ts_u16)u32_dev_addr;
    st_msg.u16_reg_addr = (ts_u16)u32_reg_addr;
    st_msg.pv_buf = pv_wr_buf;
    st_msg.u32_buf_size = u32_cnt;

    st_msg.u08_mode = u32_type;
    st_msg.u08_ignore_reg_addr = 0;
    st_msg.pst_attr = ts_null;

    return (ts_s32)ioctl(s32_handle, TS_I2C_IOCTL_WRITE, (ts_long)&st_msg);
} 


ts_s32 ts_lib_i2c_ra08d08_read(ts_s32 s32_handle, ts_u32 u32_dev_addr, ts_u32 u32_reg_addr, \
                ts_u08 *pu08_rd_buf, ts_u32 u32_cnt)
{
    return i2c_do_read(s32_handle, I2C_BUS_RA08_D08, u32_dev_addr, u32_reg_addr, (ts_void *)pu08_rd_buf, u32_cnt);
}


ts_s32 ts_lib_i2c_ra08d08_readb(ts_s32 s32_handle, ts_u32 u32_dev_addr, ts_u32 u32_reg_addr, ts_u08 *pu08_val)
{
    return i2c_do_read(s32_handle, I2C_BUS_RA08_D08, u32_dev_addr, u32_reg_addr, (ts_void *)pu08_val, 1);
}

ts_s32 ts_lib_i2c_ra08d16_read(ts_s32 s32_handle, ts_u32 u32_dev_addr, ts_u32 u32_reg_addr, \
            ts_u16 *pu16_rd_buf, ts_u32 u32_cnt)
{
    return i2c_do_read(s32_handle, I2C_BUS_RA08_D16, u32_dev_addr, u32_reg_addr, (ts_void *)pu16_rd_buf, u32_cnt);
}

ts_s32 ts_lib_i2c_ra08d16_readw(ts_s32 s32_handle, ts_u32 u32_dev_addr, ts_u32 u32_reg_addr, ts_u16 *pu16_val)
{
    return i2c_do_read(s32_handle, I2C_BUS_RA08_D16, u32_dev_addr, u32_reg_addr, (ts_void *)pu16_val, 1);
}


ts_s32 ts_lib_i2c_ra16d08_read(ts_s32 s32_handle, ts_u32 u32_dev_addr, ts_u32 u32_reg_addr, \
                ts_u08 *pu08_rd_buf, ts_u32 u32_cnt)
{
    return i2c_do_read(s32_handle, I2C_BUS_RA16_D08, u32_dev_addr, u32_reg_addr, (ts_void *)pu08_rd_buf, u32_cnt);
}


ts_s32 ts_lib_i2c_ra16d08_readb(ts_s32 s32_handle, ts_u32 u32_dev_addr, ts_u32 u32_reg_addr, ts_u08 *pu08_val)
{
    return i2c_do_read(s32_handle, I2C_BUS_RA16_D08, u32_dev_addr, u32_reg_addr, (ts_void *)pu08_val, 1);
}


ts_s32 ts_lib_i2c_ra16d16_read(ts_s32 s32_handle, ts_u32 u32_dev_addr, ts_u32 u32_reg_addr, \
                        ts_u16 *pu16_rd_buf, ts_u32 u32_cnt)
{
    return i2c_do_read(s32_handle, I2C_BUS_RA16_D16, u32_dev_addr, u32_reg_addr, (ts_void *)pu16_rd_buf, u32_cnt);
}

ts_s32 ts_lib_i2c_ra16d16_readw(ts_s32 s32_handle, ts_u32 u32_dev_addr, ts_u32 u32_reg_addr, ts_u16 *pu16_val)
{
    return i2c_do_read(s32_handle, I2C_BUS_RA16_D16, u32_dev_addr, u32_reg_addr, (ts_void *)pu16_val, 1);
}


ts_s32 ts_lib_i2c_ra08d08_write(ts_s32 s32_handle, ts_u32 u32_dev_addr, ts_u32 u32_reg_addr, \
                ts_u08 *pu08_wr_buf, ts_u32 u32_cnt)
{
    return i2c_do_write(s32_handle, I2C_BUS_RA08_D08, u32_dev_addr, u32_reg_addr, (ts_void *)pu08_wr_buf, u32_cnt);
}


ts_s32 ts_lib_i2c_ra08d08_writeb(ts_s32 s32_handle, ts_u32 u32_dev_addr, ts_u32 u32_reg_addr, ts_u08 u08_val)
{
    ts_u08 au08_buf[4];

    au08_buf[0] = u08_val;
    return i2c_do_write(s32_handle, I2C_BUS_RA08_D08, u32_dev_addr, u32_reg_addr, (ts_void *)&au08_buf, 1);
}


ts_s32 ts_lib_i2c_ra08d16_write(ts_s32 s32_handle, ts_u32 u32_dev_addr, ts_u32 u32_reg_addr, \
                ts_u16 *pu16_wr_buf, ts_u32 u32_cnt)
{
    return i2c_do_write(s32_handle, I2C_BUS_RA08_D16, u32_dev_addr, u32_reg_addr, (ts_void *)pu16_wr_buf, u32_cnt);
}


ts_s32 ts_lib_i2c_ra08d16_writew(ts_s32 s32_handle, ts_u32 u32_dev_addr, ts_u32 u32_reg_addr, ts_u16 u16_val)
{
    ts_u16 au16_buf[2];

    au16_buf[0] = u16_val;
    return i2c_do_write(s32_handle, I2C_BUS_RA08_D16, u32_dev_addr, u32_reg_addr, (ts_void *)&au16_buf, 1);
}


ts_s32 ts_lib_i2c_ra16d08_write(ts_s32 s32_handle, ts_u32 u32_dev_addr, ts_u32 u32_reg_addr, ts_u08 *pu08_val, \
                            ts_u32 u32_cnt)
{
    return i2c_do_write(s32_handle, I2C_BUS_RA16_D08, u32_dev_addr, u32_reg_addr, (ts_void *)pu08_val, u32_cnt);
}


ts_s32 ts_lib_i2c_ra16d08_writeb(ts_s32 s32_handle, ts_u32 u32_dev_addr, ts_u32 u32_reg_addr, ts_u08 u08_val)
{
    ts_u08 au08_buf[4];

    au08_buf[0] = u08_val;
    return i2c_do_write(s32_handle, I2C_BUS_RA16_D08, u32_dev_addr, u32_reg_addr, (ts_void *)&au08_buf, 1);
}


ts_s32 ts_lib_i2c_ra16d16_write(ts_s32 s32_handle, ts_u32 u32_dev_addr, ts_u32 u32_reg_addr, \
                        ts_u16 *pu16_wr_buf, ts_u32 u32_cnt)
{
    return i2c_do_write(s32_handle, I2C_BUS_RA16_D16, u32_dev_addr, u32_reg_addr, (ts_void *)pu16_wr_buf, u32_cnt);
}


ts_s32 ts_lib_i2c_ra16d16_writew(ts_s32 s32_handle, ts_u32 u32_dev_addr, ts_u32 u32_reg_addr, ts_u16 u16_val)
{
    ts_u16 au16_buf[2];

    au16_buf[0] = u16_val;
    return i2c_do_write(s32_handle, I2C_BUS_RA16_D16, u32_dev_addr, u32_reg_addr, (ts_void *)&au16_buf, 1);
}


ts_s32 ts_lib_i2c_start_slave(ts_s32 s32_handle, ts_i2c_slv_attr_s_t *pst_slv_attr)
{
    if ((s32_handle < 0) || (ts_null == pst_slv_attr) || (0 == pst_slv_attr->u08_dev_addr)) {
        ts_uapi_log_err("%s:handle=%d, dev_addr=%x\n", TS_EINVAL_STR, s32_handle, pst_slv_attr->u08_dev_addr);

        return -TS_EINVAL;
    }

    /* 如果没有寄存器表，则仅出警告 */
    if ((ts_null == pst_slv_attr->pv_reg_map) || (0 == pst_slv_attr->u16_reg_size))
        ts_uapi_log_warn("The register map is not set, the host read will no be supported.\n");;

    if (pst_slv_attr->u08_rfl < 8)
        pst_slv_attr->u08_rfl = 8;

    if (pst_slv_attr->u08_rfl > 32)
        pst_slv_attr->u08_rfl = 32;

    return (ts_s32)ioctl(s32_handle, TS_I2C_IOCTL_START_SLV, (ts_long)pst_slv_attr);
}


ts_s32 ts_lib_i2c_slv_read_msg(ts_s32 s32_handle, ts_u08 *pu08_buf, ts_u32 u32_cnt, ts_u32 u32_tmout_ms)
{
    ts_i2c_msg_s_t st_msg;

    if ((s32_handle < 0) || (ts_null == pu08_buf) || (0 == u32_cnt)) {
        ts_uapi_log_err("%s:handle=%d, buf=%p, cnt=%d\n", TS_EINVAL_STR, s32_handle, pu08_buf, u32_cnt);

        return -TS_EINVAL;
    }

    st_msg.pv_buf = (ts_void *)pu08_buf;
    st_msg.u32_buf_size = u32_cnt;
    st_msg.u32_tmout_ms = u32_tmout_ms;

    return (ts_s32)ioctl(s32_handle, TS_I2C_IOCTL_READ_SLV, (ts_long)&st_msg);
}


ts_s32 ts_lib_i2c_get_attr(ts_s32 s32_handle, ts_i2c_attr_s_t *pst_attr)
{
    uapi_chk_ptr(pst_attr, TS_ENULLPTR);

    return (ts_s32)ioctl(s32_handle, TS_I2C_IOCTL_GET_ATTR, (ts_long)pst_attr);
}


ts_s32 ts_lib_i2c_set_attr(ts_s32 s32_handle, const ts_i2c_attr_s_t *pst_attr)
{
    uapi_chk_ptr(pst_attr, TS_ENULLPTR);

    return (ts_s32)ioctl(s32_handle, TS_I2C_IOCTL_SET_ATTR, (ts_long)pst_attr);
}


ts_s32 ts_lib_i2c_open(ts_char *pc_dev_name)
{
    return open(pc_dev_name, O_RDWR);
}


ts_void ts_lib_i2c_close(ts_s32 s32_handle)
{
    if (s32_handle >= 0)
        close(s32_handle);
}


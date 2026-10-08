/*
 * i2c.c -- tdk i2c 使用例子
 */
#include <base/ts_type.h>
#include <base/ts_bitop.h>
#include <cpl/ts_cpl.h>
#include <uapi/i2c_v100.h>
#include <uapi/gpio_v100.h>
#include <uapi/iomux.h>
#include <command.h>
#include <stdio.h>
#include <stdlib.h>

#define I2C_SLV_ADDR            (0x54)
#define I2C_SLV_REG_NUM         (512)
#define I2C_SLV_REG_SIZE        (I2C_SLV_REG_NUM * 2)
#define I2C_SLV_REG_MODE        I2C_BUS_RA08_D08

static ts_s32 i2c0_hdl = -1;
static ts_s32 i2c1_hdl = -1;
static ts_s32 i2c2_hdl = -1;
static ts_s32 i2c3_hdl = -1;
static ts_u16 *spu16_reg_map = ts_null;

static ts_void i2c_show_help(ts_void)
{
    printf("i2c help:\n");
    printf("i2c id r/w type dev_addr reg_addr wr_dat/cnt\n");
    printf("type:\n");
    printf("0: reg_addr_bit = 8; reg_dat_bit = 8\n");
    printf("1: reg_addr_bit = 8; reg_dat_bit = 16\n");
    printf("2: reg_addr_bit = 16; reg_dat_bit = 8\n");
    printf("3: reg_addr_bit = 16; reg_dat_bit = 16\n");
}


static ts_void i2c_slv_process(ts_void *pv_param)
{
    ts_u16 au16_buf[128];
    ts_u08 *pu08_ptr;
    ts_s32 s32_ret;
    ts_u32 i, u32_rd_cnt, u32_sum;
    //ts_u16 u16_val;

    while (1) {
        /* 读取 32 个数据 + 1 个地址， 每个数据和地址为 16bit， 所以需要乘以 2 */
        u32_rd_cnt = 129;
        s32_ret = ts_lib_i2c_slv_read_msg(i2c1_hdl, (ts_u08 *)&au16_buf, u32_rd_cnt, 0xfffffff);
        if (s32_ret > 0) {
            u32_rd_cnt = (ts_u32)s32_ret;
            pu08_ptr = (ts_u08 *)&au16_buf;
            u32_sum = 0;
            for (i = 0; i < u32_rd_cnt; i++) {
                /* 高低翻转 */
                u32_sum += pu08_ptr[i];

                //u16_val = be16_to_cpu(u16_val);

                #if 0
                printf("%02x ",  pu08_ptr[i]);
                if (0 == i) {
                    printf("\n");
                    continue;
                }

                j++;
                if (j >= 16) {
                    j = 0;
                    printf("\n");
                }
                #endif
            }

            printf("rsum=%x\n", u32_sum);
        }
    }
}


TS_DECLARE_CMD(i2c_slv)
{
    ts_u32 i;
    ts_i2c_slv_attr_s_t st_attr;
    ts_cpl_task_s_t st_task;

    if (i2c1_hdl >= 0) {
        printf ("slv intied!\n");

        return -1;
    }

    i2c1_hdl = ts_lib_i2c_open("/dev/i2c1");
    if (i2c1_hdl < 0) {
        printf("slv open fail!\n");

        return -1;
    }

    if (ts_null == spu16_reg_map) 
        spu16_reg_map = malloc(I2C_SLV_REG_SIZE);

    if (ts_null == spu16_reg_map) {
        printf("malloc for regmap fail!\n");

        return -1;
    } else {
        for (i = 0; i < I2C_SLV_REG_NUM; i++)
            spu16_reg_map[i] = i;
    }

    st_attr.u08_dev_addr = I2C_SLV_ADDR;
    st_attr.u08_mode = I2C_SLV_REG_MODE;
    st_attr.u16_reg_size = I2C_SLV_REG_NUM;
    st_attr.pv_reg_map = (ts_void *)spu16_reg_map;
    st_attr.u08_rfl = 16;

    ts_lib_i2c_start_slave(i2c1_hdl, (ts_i2c_slv_attr_s_t *)&st_attr);

    ts_cpl_kthread_create(&st_task, (ts_cpl_kthread_t)i2c_slv_process, ts_null, "i2c_slv", 14, 2048);

    return 0;
}


extern ts_s32 ts_lib_i2c_ra16d16_read(ts_s32 s32_handle, ts_u32 u32_dev_addr, ts_u32 u32_reg_addr, ts_u16 *pu16_rd_buf, ts_u32 u32_cnt);
TS_DECLARE_CMD(i2c_mst)
{
    ts_u32 u32_dev_addr, u32_reg_addr, u32_cnt, u32_sum;
    ts_u32 i;
    ts_s32 s32_ret;;
    ts_u16 au16_rd_buf[64];
    ts_u08 au08_wr_buf[256];

    if (i2c1_hdl < 0) {
        //ts_lib_gpio_set_mux(TS_IO_UART2_TXD, TS_IO_UART2_TXD_FUN2_I2C3_SDA);
        //ts_lib_gpio_set_mux(TS_IO_UART2_RXD, TS_IO_UART2_RXD_FUN2_I2C3_SCL);
    
        //ts_lib_gpio_set_pull(TS_IO_UART2_TXD, TS_ELEC_LVL_HI);
        //ts_lib_gpio_set_pull(TS_IO_UART2_RXD, TS_ELEC_LVL_HI);

        i2c1_hdl = ts_lib_i2c_open("/dev/i2c1");
    }

    u32_reg_addr = 0x120;
    u32_dev_addr = 0x54;
    u32_cnt = 30;

    if (argc > 1) {
        s32_ret = ts_lib_i2c_ra16d16_read(i2c1_hdl, u32_dev_addr, u32_reg_addr, (ts_u16 *)&au16_rd_buf, u32_cnt);
        if (TS_OK == s32_ret) {
            for (i = 0; i < u32_cnt; i++) {
                printf("i2c addr[%#x] : %#x\n", u32_reg_addr, au16_rd_buf[i]);
                u32_reg_addr++;
            }
        }
    } else {
        ts_cpl_srand(ts_cpl_get_now());
        u32_reg_addr = 0;
        u32_cnt = 128;
        u32_sum = 0;
        for (i = 0; i < u32_cnt; i++) {
            au08_wr_buf[i] = (ts_u08)ts_cpl_random();
            u32_sum += au08_wr_buf[i];

            #if 0
            printf("%02x ", au08_wr_buf[i]);
            j++;
            if (j >= 16) {
                j = 0;
                printf("\n");
            }
            #endif
        }

        printf("sum= %x\n", u32_sum);
    
        ts_lib_i2c_ra08d08_write(i2c1_hdl, u32_dev_addr, u32_reg_addr, (ts_u08 *)&au08_wr_buf, u32_cnt);
    }

    return 0;
}



TS_DECLARE_CMD(i2c)
{
    ts_u32 u32_ops, u32_type, u32_dev_addr, u32_reg_addr, u32_val, u32_cnt;
    ts_u32 i, u32_id;
    ts_s32 s32_ret, s32_cur_hdl;;
    ts_u16 u16_val;
    ts_u08 u08_val;

    if (argc != 7) {
        i2c_show_help();

        return 0;
    }

    u32_id = ts_cpl_str2num(argv[1]);

    if(0 == u32_id)
        s32_cur_hdl = i2c0_hdl;
    else if(1 == u32_id)
        s32_cur_hdl = i2c1_hdl;
    else if(2 == u32_id)
        s32_cur_hdl = i2c2_hdl;
    else
        s32_cur_hdl = i2c3_hdl;

    if (s32_cur_hdl < 0) {
        if (0 == u32_id) {
            i2c0_hdl = ts_lib_i2c_open("/dev/i2c0");
            s32_cur_hdl = i2c0_hdl;
        } else if (1 == u32_id) {
            i2c1_hdl = ts_lib_i2c_open("/dev/i2c1");
            s32_cur_hdl = i2c1_hdl;
        } else if (2 == u32_id) {
            i2c2_hdl = ts_lib_i2c_open("/dev/i2c2");
            s32_cur_hdl = i2c2_hdl;
        } else {
            i2c3_hdl = ts_lib_i2c_open("/dev/i2c3");
            s32_cur_hdl = i2c3_hdl;
        }

        if (s32_cur_hdl < 0) {
            printf("open i2c device fail!\n");

            return 0;
        }
    }

    u32_cnt = 0;
    if ('w' == argv[2][0]) {
        u32_ops = 1;
        u32_val = ts_cpl_str2num(argv[6]);
    } else if ('r' == argv[2][0]) {
        u32_ops = 0;
        u32_cnt = ts_cpl_str2num(argv[6]);
    } else {
        i2c_show_help();

        return 0;
    }

    u32_type = ts_cpl_str2num(argv[3]);
    if (u32_type > I2C_BUS_RA16_D16) {
        i2c_show_help();

        return 0;
    }

    u32_dev_addr = ts_cpl_str2num(argv[4]);
    u32_reg_addr = ts_cpl_str2num(argv[5]);

    if (0 == u32_ops) {
        if (I2C_BUS_RA16_D16 == u32_type) {
            for (i = 0; i < u32_cnt; i++) {
                s32_ret = ts_lib_i2c_ra16d16_readw(s32_cur_hdl, u32_dev_addr, u32_reg_addr, (ts_u16 *)&u16_val);
                if (s32_ret != TS_OK)
                    break;

                printf("i2c addr[%#x] : %#x\n", u32_reg_addr, u16_val);

                u32_reg_addr++;
            }
        } else if (I2C_BUS_RA08_D16 == u32_type) {
            for (i = 0; i < u32_cnt; i++) {
                s32_ret = ts_lib_i2c_ra08d16_readw(s32_cur_hdl, u32_dev_addr, u32_reg_addr, (ts_u16 *)&u16_val);
                if (s32_ret != TS_OK)
                    break;

                printf("i2c addr[%#x] : %#x\n", u32_reg_addr, u16_val);

                u32_reg_addr++;
            }
        }  else if (I2C_BUS_RA08_D08 == u32_type) {
            for (i = 0; i < u32_cnt; i++) {
                s32_ret = ts_lib_i2c_ra08d08_readb(s32_cur_hdl, u32_dev_addr, u32_reg_addr, (ts_u08 *)&u08_val);
                if (s32_ret != TS_OK)
                    break;

                printf("i2c addr[%#x] : %#x\n", u32_reg_addr, u08_val);

                u32_reg_addr++;
            }
        } else if (I2C_BUS_RA16_D08 == u32_type) {
            for (i = 0; i < u32_cnt; i++) {
                s32_ret = ts_lib_i2c_ra16d08_readb(s32_cur_hdl, u32_dev_addr, u32_reg_addr, (ts_u08 *)&u08_val);
                if (s32_ret != TS_OK)
                    break;

                printf("i2c addr[%#x] : %#x\n", u32_reg_addr, u08_val);

                u32_reg_addr++;
            }
        }
    } else {
        if (I2C_BUS_RA16_D16 == u32_type) 
            ts_lib_i2c_ra16d16_writew(s32_cur_hdl, u32_dev_addr, u32_reg_addr, (ts_u16)u32_val);
        else if (I2C_BUS_RA08_D16 == u32_type) 
            ts_lib_i2c_ra08d16_writew(s32_cur_hdl, u32_dev_addr, u32_reg_addr, (ts_u16)u32_val);
        else if (I2C_BUS_RA08_D08 == u32_type)
            ts_lib_i2c_ra08d08_writeb(s32_cur_hdl, u32_dev_addr, u32_reg_addr, (ts_u08)u32_val);
        else if (I2C_BUS_RA16_D08 == u32_type)
            ts_lib_i2c_ra16d08_writeb(s32_cur_hdl, u32_dev_addr, u32_reg_addr, (ts_u08)u32_val);
    }

    return 0;
}


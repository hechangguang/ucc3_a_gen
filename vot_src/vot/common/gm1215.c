#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
//#include <linux/i2c-dev.h>
#include <errno.h>
#include <string.h>
#include <cpl/ts_cpl.h>
#include <kapi/k_i2c_v100.h>
#include <uapi/gpio_v100.h>
#include <linux/io.h>
#include <tsp.h>
//#include "i2c_tg_v100.h"
#include "gm1215.h"
#include "snr_comm.h"

#if 1
#define IIC_DEBUG vision_log_debug
#else
#define IIC_DEBUG(...)
#endif


// GM1215 配置参数
#define GM1215_ADDR      (0x21)    // 7位I2C地址
// GM1215 官方寄存器
#define REG_UNLOCK       0xA9    // 解锁寄存器
#define UNLOCK_VAL       0xA1    // 解锁值
#define REG_SET_CURRENT  0x04   // SET电流配置寄存器
#define REG_APPLY        0x1E    // 生效寄存器
#define APPLY_VAL        0x01    // 生效值

// 电流参数
#define CURRENT_STEP     0.75f   // 每BIT 0.75μA
#define CURRENT_MIN      0.0f    // 最小电流(μA)
#define CURRENT_MAX      200.0f  // 最大200μA

#define use_i2c0    1  // 定义使用哪个I2C总线，0或1
//#define use_i2c1  1

#define gpio_for_i2c  0  // 0: 使用I2C控制器通信， 1: 使用GPIO模拟I2C通信

#if gpio_for_i2c
//使用GPIO模拟I2C通信
// ===================== GPIO操作宏定义 =====================
//1. 引脚定义
//  GPIO2 = SDA, GPIO3 = SCL（按实际修改）
#ifdef use_i2c0
#define SDA_PIN   0
#define SCL_PIN   1
#elif defined(use_i2c1)
#define SDA_PIN   2
#define SCL_PIN   3
#else
#error "请定义使用哪个GPIO模拟I2C"
#endif

// SCL输出高/低
#define I2C_SCL_OUT_H()     ts_lib_gpio_set_out(SCL_PIN, TS_ELEC_LVL_HI)
#define I2C_SCL_OUT_L()     ts_lib_gpio_set_out(SCL_PIN, TS_ELEC_LVL_LO)
#define I2C_SCL_H()         ts_lib_gpio_write_pin(SCL_PIN, TS_ELEC_LVL_HI)
#define I2C_SCL_L()         ts_lib_gpio_write_pin(SCL_PIN, TS_ELEC_LVL_LO)

// SDA输出高/低
#define I2C_SDA_OUT_H()     ts_lib_gpio_set_out(SDA_PIN, TS_ELEC_LVL_HI)
#define I2C_SDA_OUT_L()     ts_lib_gpio_set_out(SDA_PIN, TS_ELEC_LVL_LO)
#define I2C_SDA_H()         ts_lib_gpio_write_pin(SDA_PIN, TS_ELEC_LVL_HI)
#define I2C_SDA_L()         ts_lib_gpio_write_pin(SDA_PIN, TS_ELEC_LVL_LO)

// 读取SDA电平
#define I2C_SDA_IN()        ts_lib_gpio_set_in(SDA_PIN, TS_OPT_ENABLE) // 设置为输入
#define I2C_READ_SDA()      ts_lib_gpio_read_pin(SDA_PIN)
// =================================================================

//2. GPIO 初始化（SCL/SDA 设为开漏输出）
static void I2C_GPIO_Init(void)
{
    //  ts_lib_gpio_set_mux(TS_IO_I2C1_SDA, TS_IO_I2C1_SDA_FUN0);
    // ts_lib_gpio_set_mux(TS_IO_I2C1_SCL, TS_IO_I2C1_SCL_FUN0);
    // SCL/SDA 开漏输出
    #ifdef use_i2c0
    ts_lib_gpio_set_mux(TS_IO_I2C0_SDA, TS_IO_I2C0_SDA_FUN3_GPIO0);
    ts_lib_gpio_set_mux(TS_IO_I2C0_SCL, TS_IO_I2C0_SCL_FUN3_GPIO1);
    ts_lib_gpio_set_pull(TS_IO_I2C0_SDA, TS_ELEC_LVL_HI);
    ts_lib_gpio_set_pull(TS_IO_I2C0_SCL, TS_ELEC_LVL_HI);
    #elif defined(use_i2c1)
    ts_lib_gpio_set_mux(TS_IO_I2C1_SDA, TS_IO_I2C1_SDA_FUN3_GPIO2);
    ts_lib_gpio_set_mux(TS_IO_I2C1_SCL, TS_IO_I2C1_SCL_FUN3_GPIO3);
    ts_lib_gpio_set_pull(TS_IO_I2C1_SDA, TS_ELEC_LVL_HI);
    ts_lib_gpio_set_pull(TS_IO_I2C1_SCL, TS_ELEC_LVL_HI);
    #else       
        #error "请定义使用哪个GPIO模拟I2C"
    #endif
    ts_lib_gpio_set_out(SDA_PIN, TS_ELEC_LVL_HI);
    ts_lib_gpio_set_out(SCL_PIN, TS_ELEC_LVL_HI);
    // 空闲状态：SCL、SDA都为高电平
    // I2C_SCL_H();
    // I2C_SDA_H();
}
//3. 微秒延时（根据芯片修改实现）
static void I2C_Delay_us(uint32_t us)
{
    ts_cpl_udelay(us-1); 
}

//4. 基础 I2C 时序函数
// 1. I2C起始信号
static void I2C_Start(void)
{
    I2C_SDA_OUT_H();  // sda线输出高    
    I2C_SCL_OUT_H();  // SCL拉高
    I2C_Delay_us(4);  // 保持时序
    I2C_SDA_L();  // SCL高时，SDA下降沿 = 起始信号
   // I2C_Delay_us(4);
    I2C_SCL_L();  // 钳住SCL，准备传输数据
   // I2C_Delay_us(4);
}

// 2. I2C停止信号
static void I2C_Stop(void)
{
    I2C_SDA_OUT_L();  // sda线输出低
    I2C_SCL_OUT_H();  // SCL拉高
    I2C_Delay_us(4);
    I2C_SDA_H();  // SCL高时，SDA上升沿 = 停止信号
   // I2C_Delay_us(4);
    I2C_SCL_L();  // SCL拉低，结束传输
   // I2C_Delay_us(4);
}

// 3. 等待从机应答（返回值：1=应答失败，0=应答成功）
static uint8_t I2C_Wait_Ack(void)
{
    uint8_t retry = 0;
    I2C_SDA_OUT_H();  // 释放SDA线，等待从机拉低应答
    I2C_SCL_OUT_H();  // SCL拉高，读取SDA
    I2C_Delay_us(4);

    I2C_SDA_IN();  // SDA先设为输入
    // 等待SDA被从机拉低（应答）
    while(I2C_READ_SDA()!=0)
    {
        retry++;
        if(retry > 250)  // 超时判断
        {
            I2C_Stop();
           // IIC_DEBUG("I2C Wait Ack Failed! I2C_READ_SDA=%d\n", I2C_READ_SDA());
            /***********
             * I2C_READ_SDA()返回值说明：
            * 返  回 :  返回值         描述
            *           0              管脚上为低电平
            *           1              管脚上为高电平
            *          -TS_ENOINIT      GPIO 库未初始化
            *          -TS_EINVALGRPID  管脚编号错误，无对应 GPIO 组
            *******/
            return 1;
        }
    }
    I2C_SCL_OUT_L();  // SCL拉低
    return 0;
}

// 4. 主机发送应答信号 ACK
static void I2C_Ack(void)
{
    I2C_SDA_OUT_L();  // 设置SDA为输出，准备发送ACK
    I2C_SCL_OUT_H();
    I2C_Delay_us(4);
    I2C_SDA_L(); 
   // I2C_Delay_us(4);
    I2C_SCL_L();
   // I2C_Delay_us(4);
}

// 5. 主机发送非应答信号 NACK
static void I2C_NAck(void)
{
    I2C_SDA_OUT_H();  // 设置SDA为输出，保持高电平 = NACK
    I2C_SCL_OUT_H();
    I2C_Delay_us(4);
    I2C_SDA_H();
  //  I2C_Delay_us(4);
    I2C_SCL_L();
  //  I2C_Delay_us(4);
}

// 发送BIT0  
static void i2c_send_bit_0(void)
{
    //SDA=0;
   // gpio_direction_output(sda_pin, 0);
    //SCL=1;
   // gpio_direction_output(scl_pin, 1);
    //delay_us(5);
  //  udelay(5);
    //SCL=0;
   // gpio_set_value(scl_pin, 0);
    //delay_us(5);
   // udelay(5);

    I2C_SDA_OUT_L();  // 设置SDA为输出，拉低SDA
    I2C_SCL_OUT_H();  // SCL拉高，锁存数据
    I2C_Delay_us(4);
    I2C_SCL_L();  // SCL拉低，准备下一位
    //I2C_Delay_us(4);
}

// 发送BIT1 
static void i2c_send_bit_1(void)
{
    // //SDA=1;
    // gpio_direction_output(sda_pin, 1);
    // //SCL=1;
    // gpio_direction_output(scl_pin, 1);
    // //delay_us(5);
    // udelay(5);
    // //SCL=0;
    // gpio_set_value(scl_pin, 0);
    // //delay_us(5);
    // udelay(5);

    I2C_SDA_OUT_H();  // 设置SDA为输出，拉高SDA
    I2C_SCL_OUT_H();  // SCL拉高，锁存数据
    I2C_Delay_us(4);
    I2C_SCL_L();  // SCL拉低，准备下一位
   // I2C_Delay_us(4);
}

//5. 数据收发函数
// 1. 主机发送1个字节
static void I2C_Send_Byte(uint8_t data)
{
    uint8_t i;
    for (i=0; i<8; i++) 
    {
        if ((data<<i) & 0x80)
                i2c_send_bit_1();
            else
                i2c_send_bit_0();
    }
}

// 2. 主机读取1个字节
/*功能: 读1个字节，ack=1时，发送ACK，ack=0，发送nACK
* Return         : 1，有应答  0，无应答
*/
static uint8_t I2C_Read_Byte(uint8_t ack)
{
    uint8_t i, data = 0;
    int tmp;
    for(i=0; i<8; i++)  // 读取8位，高位先行
    {
        I2C_SDA_OUT_H();  // 释放SDA线，等待从机拉低应答
        I2C_SCL_OUT_H();  // SCL拉高，读取SDA
        I2C_Delay_us(4);
        I2C_SDA_IN();  // SDA设为输入
        tmp = I2C_READ_SDA();
        I2C_Delay_us(4);
        I2C_SCL_L(); // SCL拉低，准备下一位
        if (tmp == 1) 
        {
            data = data<<1;
            data = data | 0x01;
        } 
        else
            data = data<<1;
       
    }

    // 读完后发送应答/非应答
    if(ack) 
    {
        I2C_Ack();
    }
    else
    {
        I2C_NAck();
    }
    return data;


}

//6. 上层读写函数（常用：寄存器读写）

// 向I2C设备指定寄存器写入1个字节
// addr：设备7位I2C地址  reg：寄存器地址  data：数据
static int I2C_Write_Reg(uint8_t addr, uint8_t reg, uint8_t data)
{
    I2C_Start();
    I2C_Send_Byte((addr << 1) | 0);  // 地址+写位(0)
    if(I2C_Wait_Ack()) 
    {
        I2C_Stop();
        //IIC_DEBUG("Ack timeout,I2C Send addr Failed! addr=0x%02X\n", addr);
        return -1;
    }
    I2C_Send_Byte(reg);              // 寄存器地址
    if(I2C_Wait_Ack()) 
    {
        I2C_Stop();
       // IIC_DEBUG("Ack timeout,I2C Send reg Failed! reg=0x%02X\n", reg);
        return -1;
    }
    I2C_Send_Byte(data);             // 写入数据
    if(I2C_Wait_Ack()) 
    {
        I2C_Stop();
       // IIC_DEBUG("Ack timeout,I2C Send data Failed! data=0x%02X\n", data);
        return -1;
    }
    I2C_Stop();
    return 0;
}

// 从I2C设备指定寄存器读取1个字节
static int I2C_Read_Reg(uint8_t addr, uint8_t reg, uint8_t *data)
{
   
    I2C_Start();
    I2C_Send_Byte((addr << 1) | 0);  // 地址+写位
    if(I2C_Wait_Ack()) 
    {
        I2C_Stop();
        //IIC_DEBUG("Ack timeout,I2C Send addr Failed! addr=0x%02X\n", addr);
        return -1;
    }
    I2C_Send_Byte(reg);              // 寄存器地址
    if(I2C_Wait_Ack()) 
    {
        I2C_Stop();
        //IIC_DEBUG("Ack timeout,I2C Send reg Failed! reg=0x%02X\n", reg);
        return -1;
    }

    I2C_Start();                     // 重复起始
    I2C_Send_Byte((addr << 1) | 1);  // 地址+读位(1)
    if(I2C_Wait_Ack()) 
    {
        I2C_Stop();
        //IIC_DEBUG("Ack timeout,I2C Send addr Failed! data=0x%02X\n", addr);
        return -1;
    }
    *data = I2C_Read_Byte(0);         // 读取数据+NACK
    I2C_Stop();
    return 0;
}


int i2c_write_nLen(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, int len) 
{
    I2C_Start();
    // 发送设备地址（写操作）
    I2C_Send_Byte((dev_addr << 1) | 0);  // 地址+写位(0)
    if(I2C_Wait_Ack()) 
    {
        I2C_Stop();
        return -1;
    }
    // 发送寄存器地址
    I2C_Send_Byte((reg_addr));  // 寄存器地址
    if(I2C_Wait_Ack()) 
    {
        I2C_Stop();
        return -1;
    }
    // 发送数据
    for (int i = 0; i < len; i++) 
    {
        I2C_Send_Byte(data[i]);
        if(I2C_Wait_Ack())
        {
            I2C_Stop();
            return -1;
        }
    }
    I2C_Stop();
    return 0;
}

int i2c_read_nLen(uint8_t dev_addr, uint8_t reg_addr, uint8_t *buf, int len) 
{
    I2C_Start();
    // 先写寄存器地址（写操作）
    I2C_Send_Byte((dev_addr << 1) | 0);  // 地址+写位(0)
    if(I2C_Wait_Ack()) 
    {
        I2C_Stop();
        return -1;
    }
    I2C_Send_Byte((reg_addr));  // 寄存器地址
    if(I2C_Wait_Ack()) 
    {
        I2C_Stop();
        return -1;
    }
    
    I2C_Start();                     // 重复起始
    I2C_Send_Byte((dev_addr << 1) | 1);  // 地址+读位(1)
    if(I2C_Wait_Ack()) 
    {
        I2C_Stop();
        return -1;
    }
    
    for (int i = 0; i < len; i++) 
    {
        buf[i] = I2C_Read_Byte(1);
    }
    I2C_Stop();
    return 0;
}

#if 0
// 使用示例
// 以I2C 设备 0x50（AT24C02） 为例：
// // 初始化
// I2C_GPIO_Init();
// // 写入数据：向地址0x00寄存器写入0x12
// I2C_Write_Reg(0x50, 0x00, 0x12);
// // 读取数据：从地址0x00寄存器读取数据
// uint8_t val = I2C_Read_Reg(0x50, 0x00);
#endif

void gm1215_Init(void)
{
    I2C_GPIO_Init(); // 初始化GPIO引脚为I2C模式
}

/**
 * @brief  设置GM1215的SET引脚电流
 * @param  fd: I2C文件描述符
 * @param  current_ua: 目标电流(0~200μA)
 * @retval 0成功，-1失败
 */
int gm1215_set_current(int fd, float current_ua)
{
    // 1. 限幅
    if (current_ua < CURRENT_MIN || current_ua > CURRENT_MAX)
    {
        //IIC_DEBUG("电流超出范围！必须在0~200μA之间\n");
        IIC_DEBUG("Current out of range! Must be between 0~200μA\n");
        return -1;
    }

    // 2.计算寄存器值（四舍五入）
 /*********
 设置SET引脚电流 
0x0 = 100μA 
0x1 = 100.75μA 
… 
0x7F = 200μA 
0x80 = 0μA 
0x81 = 0.75μA 
0x82 = 1.5μA 
… 
0xFE = 98.5μA 
0xFF = 99.25μA 
*********/
    ts_u08 reg_val;

    if(current_ua >= 100 && current_ua <= CURRENT_MAX) {
        reg_val = (ts_u08)((current_ua - 100) / CURRENT_STEP + 0.5f);
    } else if(current_ua < 100 && current_ua >= 0) {
        reg_val = (ts_u08)(current_ua / CURRENT_STEP + 0.5f)+ 0x80;
    } else {
        reg_val = 0x80; // 0μA
    }

    //ts_u08 buf[1];
    // 写入寄存器：寄存器地址 + 数据
    // 3. 官方步骤：解锁
    // 写入数据：向地址0xA9寄存器写入0xA1（解锁后必须写入0xA1才能修改电流寄存器）
    if(I2C_Write_Reg(GM1215_ADDR, REG_UNLOCK, UNLOCK_VAL) != 0) {
        return -1;
    }

    // 4. 写电流配置
    if(I2C_Write_Reg(GM1215_ADDR, REG_SET_CURRENT, reg_val) != 0) {
        return -1;
    }

    // 5. 写入生效
    if(I2C_Write_Reg(GM1215_ADDR, REG_APPLY, APPLY_VAL) != 0) {
        return -1;
    }

    //IIC_DEBUG("设置电流成功: %.2f μA, 寄存器值: 0x%02X\n", current_ua, reg_val);
    IIC_DEBUG("Current set successfully: %.2f μA, register value: 0x%02X\n", current_ua, reg_val);
    return 0;
}

/**
 * @brief  读取GM1215当前SET电流
 * @param  fd: I2C文件描述符
 * @param  current_ua: 输出读取到的电流(μA)
 * @retval 0成功，-1失败
 */
int gm1215_get_current(int fd, float *current_ua)
{
  
// 读取数据：从地址0x00寄存器读取数据
    uint8_t reg_val = 0;
    if(I2C_Read_Reg(GM1215_ADDR, REG_SET_CURRENT, &reg_val) != 0) 
    {
        return -1;
    }

    if(reg_val <= 0x7F) {
        *current_ua = 100 + reg_val * CURRENT_STEP;
    } else {
        *current_ua = (reg_val - 0x80) * CURRENT_STEP;
    }
    IIC_DEBUG("Current read successfully: %.2f μA, register value: 0x%02X\n", *current_ua, reg_val);
    return 0;
}

/**
 * @brief  计算输出电压
 * @param  current_ua: SET电流(μA)
 * @param  rset_ohm: SET引脚外接电阻(Ω)
 * @retval 输出电压(V)
 */
float gm1215_calc_vout(float current_ua, float rset_ohm)
{
    return current_ua * rset_ohm / 1000000.0f;
}

int gm1215_run(int i2c_bus)
{
    int fd=1;
    float current, vout;
    float rset = 110000.0f;  // SET外接电阻 110kΩ，根据你的硬件修改
    float set_current_ua = 80.0f; // 目标电流，单位μA，根据需要修改
    // 1. 打开I2C


    // 2. 设置电流（示例：100μA）
    if (gm1215_set_current(fd, set_current_ua) != 0)    
    {
        return -1;
    }
    // 3. 读取电流
    if (gm1215_get_current(fd, &current) != 0)
    {
        return -1;
    }
    // 4. 计算输出电压
    vout = gm1215_calc_vout(current, rset);
   // IIC_DEBUG("外接Rset=%.0fΩ, 输出电压: %.3f V\n", rset, vout);
    IIC_DEBUG("External Rset=%.0fΩ, Output Voltage: %.3f V\n", rset, vout);

    return 0;
}

int gm1215_run_task(void)
{
    ts_cpl_msleep(1000);
    #ifdef use_i2c0
    return gm1215_run(0); // 使用I2C总线0
    #elif defined(use_i2c1)
    return gm1215_run(1); // 使用I2C总线1
    #else
    #error "请定义使用哪个GPIO模拟I2C"
    #endif
}
#else


//使用硬件I2C通信

typedef struct i2c_comm_ctx_s {
    ts_u32 u32_opened;
    ts_s32 s32_i2c_hdl;
    ts_u32 u32_dev_addr;
} i2c_comm_ctx_s_t;

static i2c_comm_ctx_s_t sst_i2c_ctx = {0};
static ts_u32 readCurrentFailed_times=0;

void gm1215_Init(void)
{
    #ifdef use_i2c0
    // ts_lib_gpio_set_mux(TS_IO_I2C0_SDA, TS_IO_I2C0_SDA_FUN0);
    // ts_lib_gpio_set_mux(TS_IO_I2C0_SCL, TS_IO_I2C0_SCL_FUN0);
    // ts_lib_gpio_set_pull(TS_IO_I2C0_SDA, TS_ELEC_LVL_HI);
    // ts_lib_gpio_set_pull(TS_IO_I2C0_SCL, TS_ELEC_LVL_HI);
    // ts_lib_gpio_set_mux(TS_IO_I2C0_SDA, TS_IO_I2C0_SDA_FUN1_X);
    // ts_lib_gpio_set_mux(TS_IO_I2C0_SCL, TS_IO_I2C0_SCL_FUN1_X);
    // ts_lib_gpio_set_pull(TS_IO_I2C0_SDA, TS_ELEC_LVL_HI);
    // ts_lib_gpio_set_pull(TS_IO_I2C0_SCL, TS_ELEC_LVL_HI);
    #elif defined(use_i2c1)
    // ts_lib_gpio_set_mux(TS_IO_I2C1_SDA, TS_IO_I2C1_SDA_FUN0);
    // ts_lib_gpio_set_mux(TS_IO_I2C1_SCL, TS_IO_I2C1_SCL_FUN0);
    // ts_lib_gpio_set_pull(TS_IO_I2C1_SDA, TS_ELEC_LVL_HI);
    // ts_lib_gpio_set_pull(TS_IO_I2C1_SCL, TS_ELEC_LVL_HI);
    // ts_lib_gpio_set_mux(TS_IO_I2C1_SDA, TS_IO_I2C1_SDA_FUN1_X);
    // ts_lib_gpio_set_mux(TS_IO_I2C1_SCL, TS_IO_I2C1_SCL_FUN1_X);
    // ts_lib_gpio_set_pull(TS_IO_I2C1_SDA, TS_ELEC_LVL_HI);
    // ts_lib_gpio_set_pull(TS_IO_I2C1_SCL, TS_ELEC_LVL_HI);
    #else
        #error "请定义使用哪个I2C控制器"
    #endif
}
/**
 * @brief  打开I2C设备
 * @param  i2c_bus: I2C总线号(如1对应/dev/i2c1)
 * @retval 文件描述符，失败返回-1
 */
int gm1215_open(int i2c_bus)
{
    char dev_path[20];
    int fd;
   // gm1215_Init();
   
    snprintf(dev_path, sizeof(dev_path), "/dev/i2c%d", i2c_bus);

    ts_i2c_attr_s_t st_i2c_attr;
   
        //sst_i2c_ctx.s32_i2c_hdl = ts_lib_i2c_open("/dev/i2c1");
        sst_i2c_ctx.s32_i2c_hdl = ts_lib_i2c_open(dev_path);
        if (sst_i2c_ctx.s32_i2c_hdl < 0) {
            IIC_DEBUG("open i2c device fail!\n");
            return TS_ERR;
        }
        fd=sst_i2c_ctx.s32_i2c_hdl;
        sst_i2c_ctx.u32_opened = 1;
        sst_i2c_ctx.u32_dev_addr =(ts_u32)(GM1215_ADDR<<1);// 8位设备地址

        ts_lib_i2c_get_attr(sst_i2c_ctx.s32_i2c_hdl, (ts_i2c_attr_s_t *)&st_i2c_attr);
        st_i2c_attr.u08_duty = 48;
        st_i2c_attr.u08_duty_adj = 0;
        st_i2c_attr.u08_master = 1;
        st_i2c_attr.u08_roll_thr = 32;
        st_i2c_attr.u08_sda_hold_us = 0;
        st_i2c_attr.u08_sda_hold_clk = 10;
        st_i2c_attr.u08_sda_setup_us = 0;
        st_i2c_attr.u08_time_out_jfs = 20;
        st_i2c_attr.u32_bus_hz = 100000;// 100kHz
        ts_lib_i2c_set_attr(sst_i2c_ctx.s32_i2c_hdl, (ts_i2c_attr_s_t *)&st_i2c_attr);
    
    return fd;
}

ts_s32 gm1215_i2c_getreg(ts_u08 u08_regaddr, ts_u08 *pu08_data)
{
    ts_s32 s32_ret = 1;

    if (0 == sst_i2c_ctx.u32_opened) {
        IIC_DEBUG("%s\n", TS_ENOTCFG_STR);

        return -TS_ENOTCFG;
    } else {
        s32_ret = ts_lib_i2c_ra08d08_readb(sst_i2c_ctx.s32_i2c_hdl, sst_i2c_ctx.u32_dev_addr, u08_regaddr, pu08_data);
        if (s32_ret != TS_OK)
            IIC_DEBUG("sensor_getreg err:ret=%d\n", s32_ret);

        return s32_ret;
    }
}


ts_void gm1215_i2c_setreg(ts_u08 u08_regaddr, ts_u08 u08_data)
{
    ts_s32 s32_ret;

    if (0 == sst_i2c_ctx.u32_opened) {
        vision_log_err("%s\n", TS_ENOTCFG_STR);
    } else {
        s32_ret = ts_lib_i2c_ra08d08_writeb(sst_i2c_ctx.s32_i2c_hdl, sst_i2c_ctx.u32_dev_addr, u08_regaddr, u08_data);
        if (s32_ret != TS_OK)
            IIC_DEBUG("snr_i2c_setreg err:ret=%d\n", s32_ret);
    }
}


/**
 * @brief  设置GM1215的SET引脚电流
 * @param  fd: I2C文件描述符
 * @param  current_ua: 目标电流(0~200μA)
 * @retval 0成功，-1失败
 */
int gm1215_set_current(int fd, float current_ua)
{
    // 1. 限幅
    if (current_ua < CURRENT_MIN || current_ua > CURRENT_MAX)
    {
        //IIC_DEBUG("电流超出范围！必须在0~200μA之间\n");
        IIC_DEBUG("Current out of range! Must be between 0~200μA\n");
        return -1;
    }

    // 2.计算寄存器值（四舍五入）
 /*********
 设置SET引脚电流 
0x0 = 100μA 
0x1 = 100.75μA 
… 
0x7F = 200μA 
0x80 = 0μA 
0x81 = 0.75μA 
0x82 = 1.5μA 
… 
0xFE = 98.5μA 
0xFF = 99.25μA 
*********/
    ts_u08 reg_val;
    if(current_ua >= 100 && current_ua <= CURRENT_MAX) {
        reg_val = (ts_u08)((current_ua - 100) / CURRENT_STEP);
        if(current_ua>=195.25)reg_val = 0x7F;
    } 
    else if(current_ua < 100 && current_ua >= 0) {
        reg_val = (ts_u08)(current_ua / CURRENT_STEP)+ 0x80;
        if(current_ua>=98.5)reg_val = 0xFF;
        else if(current_ua>=95.25&&current_ua<98.5)reg_val = 0xFE;
    } 
    else 
    {
        reg_val = 0x80; // 0μA
    }

    // 写入寄存器：寄存器地址 + 数据
    // 3. 官方步骤：解锁
    //IIC_DEBUG("i2c_ra08d08_writeb reg_addr[%#x] : %#x\n", REG_UNLOCK, UNLOCK_VAL);
    if (ts_lib_i2c_ra08d08_writeb(sst_i2c_ctx.s32_i2c_hdl, sst_i2c_ctx.u32_dev_addr, REG_UNLOCK, UNLOCK_VAL) != TS_OK)  
    {
        IIC_DEBUG("write Unlock failed, reg_addr[%#x] : %#x\n", REG_UNLOCK, UNLOCK_VAL);
        return -1;
    }  
   // gm1215_i2c_setreg(REG_UNLOCK, buf);
       
    // 4. 写电流配置
  
    //IIC_DEBUG("i2c_ra08d08_writeb reg_addr[%#x] : %#x\n", REG_SET_CURRENT, reg_val);
    if (ts_lib_i2c_ra08d08_writeb(sst_i2c_ctx.s32_i2c_hdl, sst_i2c_ctx.u32_dev_addr, REG_SET_CURRENT, reg_val)!= TS_OK)    
    {
        //IIC_DEBUG("写入电流失败, 错误: %s\n", strerror(errno));
        IIC_DEBUG("Failed to write current, reg_addr[%#x] : %#x\n", REG_SET_CURRENT, reg_val);
        return -1;
    }
    //gm1215_i2c_setreg(REG_SET_CURRENT, buf);
    // 5. 写入生效
    //IIC_DEBUG("i2c_ra08d08_writeb reg_addr[%#x] : %#x\n", REG_APPLY, APPLY_VAL);
    if (ts_lib_i2c_ra08d08_writeb(sst_i2c_ctx.s32_i2c_hdl, sst_i2c_ctx.u32_dev_addr, REG_APPLY, APPLY_VAL) != TS_OK)
    { //IIC_DEBUG("应用设置失败, 错误: %s\n", strerror(errno));
        IIC_DEBUG("Failed to apply settings,reg_addr[%#x] : %#x\n", REG_APPLY, APPLY_VAL);
        return -1;
    } 
    //gm1215_i2c_setreg(REG_APPLY, buf);
    //IIC_DEBUG("设置电流成功: %.2f μA, 寄存器值: 0x%02X\n", current_ua, reg_val);
    IIC_DEBUG("Current set successfully: %.2f μA, register value: 0x%02X\n", current_ua, reg_val);
    return 0;
}

/**
 * @brief  读取GM1215当前SET电流
 * @param  fd: I2C文件描述符
 * @param  current_ua: 输出读取到的电流(μA)
 * @retval 0成功，-1失败
 */
int gm1215_get_current(int fd, float *current_ua)
{
    int s32_ret;
    ts_u08 reg_val[1];
    s32_ret = ts_lib_i2c_ra08d08_readb(sst_i2c_ctx.s32_i2c_hdl, sst_i2c_ctx.u32_dev_addr, REG_SET_CURRENT, reg_val);
    if ((s32_ret)!= TS_OK)
    {
        //IIC_DEBUG("读取电流失败, 错误: %s\n", strerror(errno));
        IIC_DEBUG("Failed to read current, error: %d \n", s32_ret);
        readCurrentFailed_times++;
        return -1;
    }
    readCurrentFailed_times=0;
   // gm1215_i2c_getreg(REG_SET_CURRENT, &reg_val);

    if(reg_val[0] <= 0x7F) {
        *current_ua = 100 + reg_val[0] * CURRENT_STEP;
    } else {
        *current_ua = (reg_val[0] - 0x80) * CURRENT_STEP;
    }
    IIC_DEBUG("Current read successfully: %.2f μA, register value: 0x%02X\n", *current_ua, reg_val[0]);
    return 0;
}

/**
 * @brief  计算输出电压
 * @param  current_ua: SET电流(μA)
 * @param  rset_ohm: SET引脚外接电阻(Ω)
 * @retval 输出电压(V)
 */
float gm1215_calc_vout(float current_ua, float rset_ohm)
{
    return current_ua * rset_ohm / 1000000.0f;
}

int gm1215_run(int i2c_bus)
{
    //int i2c_bus = 1;        // 默认使用/dev/i2c1
    int fd;
    float current, vout;
    float rset = 115000.0f;  // SET外接电阻 115kΩ，根据你的硬件修改
    float set_current_ua = 70.0f; // 目标电流，单位μA，根据需要修改

    set_current_ua=(float)SystemParaMenu2.set_current_ua;
    // 1. 打开I2C
    fd = gm1215_open(i2c_bus);
    if (fd < 0) return -1;

    // 2. 设置电流
    if (gm1215_set_current(fd, set_current_ua) != 0)    
    {
        close(fd);
        return -1;
    }

    // 3. 读取电流
    if (gm1215_get_current(fd, &current) != 0)
    {
        close(fd);
        return -1;
    }

    // 4. 计算输出电压
    vout = gm1215_calc_vout(current, rset);
   // IIC_DEBUG("外接Rset=%.0fΩ, 输出电压: %.3f V\n", rset, vout);
    vision_log_warn("External Rset=%.0fΩ, Output Voltage: %.3f V\n", rset, vout);

    // 关闭设备
    close(fd);
    return 0;
}

int gm1215_run_task(void)
{
    static int run_status=-1;
    ts_cpl_msleep(1000);
    #ifdef use_i2c0
    if (run_status!=0&&readCurrentFailed_times<10)
    {
        run_status=gm1215_run(0);// 使用I2C总线0
    } 
    #elif defined(use_i2c1)
    if (run_status!=0&&readCurrentFailed_times<10)
    {
        run_status=gm1215_run(1);// 使用I2C总线1
    } 
    #else
    #error "请定义使用哪个I2C控制器"
    #endif
   
    return run_status;
}
#endif

static ts_s32 gm1215_run_process(ts_void *pv_param)
{

    gm1215_Init();
    ts_cpl_fix_msleep(1000);
    while (1)
    {
        gm1215_run_task();
    }
    return 0;
}
/**
 * @description: 创建gm1215线性稳压电源 新线程
 * @return {ts_void}
 */
ts_void create_gm1215_task_pthread(void)
{
    ts_cpl_task_s_t gm1215_run_task;
    // 阳光保护的优先级比消息线程的优先级低一点
    ts_cpl_kthread_tmm_create((ts_cpl_task_s_t *)&gm1215_run_task, (ts_cpl_kthread_t)gm1215_run_process, ts_null, "gm1215_run_process", 18, 2 * 2048, ts_null);
}

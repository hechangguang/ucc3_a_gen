## PWM

### 概述

PWM支持自定义占空比、频率等,支持用户中断回调设置。

支持磁编数据读取。

### API 说明

| 函数名 | 用途说明 |
| --- | --- |
| [ts_lib_pwm_open](#ts_lib_pwm_open) | 打开 pwm 设备 |
| [ts_lib_pwm_close](#ts_lib_pwm_close ) | 关闭 pwm 设备 |
| [ts_lib_pwm_set_mode](#ts_lib_pwm_set_mode ) | 设置 pwm 工作模式 |
| [ts_lib_pwm_set_freq_duty](#ts_lib_pwm_set_freq_duty ) | 设置 pwm 的频率和占空比 |
| [ts_lib_pwm_set_irqen](#ts_lib_pwm_set_irqen ) | 打开/关闭 pwm 指定通道的中断 |
| [ts_lib_pwm_set_callback](#ts_lib_pwm_set_callback ) | 设置 pwm 指定的通道的中断回调函数 |
| [ts_lib_pwm_start](#ts_lib_pwm_start ) | 不阻塞的启动 pwm 输出 |
| [ts_lib_pwm_start_timeout](#ts_lib_pwm_start_timeout ) | 停止 pwm 输出 |
| [ts_lib_pwm_stop](#ts_lib_pwm_stop ) | 写入输出信号 |
| [ts_lib_pwm_set_qdr_attr](#ts_lib_pwm_set_qdr_attr ) | 设置磁编模块的属性 |
| [ts_lib_pwm_set_qdr_compare_cnt](#ts_lib_pwm_set_qdr_compare_cnt ) | 设置磁编读数比较值 |
| [ts_lib_pwm_get_qdr_cnt](#ts_lib_pwm_get_qdr_cnt ) | 用户主动获取磁编读数 |

#### ts_lib_pwm_open

##### 描述

打开指定的 PWM 驱动设备，获得指定 PWM 设备句柄。

##### 语法

```c
ts_s32 ts_lib_pwm_open(ts_char *pc_dev_name)
```

##### 参数

| 参数        | 数据类型  | 描述           | 输入/输出 |
| --- | --- | --- | --- |
| pc_dev_name | ts_char * | 驱动设备文件名 | 输入      |

##### 返回值

| 返回值 | 描述         |
| --- | --- |
| < 0   | 设备打开失败 |
| >= 0    | 设备句柄     |

##### 版本差异

| 版本号 | 差异       |
| --- | --- |
| V100   | 支持此功能 |

##### 依赖

- 头文件 pwm_v100.h
- 库文件 ts_otdk.a

##### 注意

无

##### 举例

```c
ts_s32 s32_tp_pwm;
ts_u32 u32_channel, u32_freq, u32_duty;

insmod("pwm", ts_null);

s32_tp_pwm = ts_lib_pwm_open("/dev/tdk/pwm");
if (s32_tp_pwm >= 0) {
    u32_channel = 0;
    u32_freq = 10;
    u32_duty = 75;

    ts_lib_pwm_set_mode(s32_tp_pwm, u32_channel, TS_PWM_MODE_CONTINUOUS, TS_PWM_PFOUT_HIGH);
    ts_lib_pwm_set_freq_duty(s32_tp_pwm, u32_channel, u32_freq, u32_duty, TS_PWM_DUTY_ROUNDING);
    ts_lib_pwm_set_irqen(s32_tp_pwm, u32_channel, TS_PWM_IRQ_ENABLE);
    ts_lib_pwm_start(s32_tp_pwm, u32_channel, 0);

    ts_lib_pwm_close(s32_tp_pwm);
}

```

##### 相关主题

[ts_lib_pwm_close ](#ts_lib_pwm_close) 

#### ts_lib_pwm_close 

##### 描述

关闭打开的 PWM 驱动设备句柄

##### 语法

```c
ts_void ts_lib_pwm_close(ts_s32 s32_handle)
```

##### 参数

| 参数名称   | 数据类型 | 描述         | 输入/输出 |
| --- | --- | --- | --- |
| s32_handle | ts_s32   | 驱动设备句柄 | 输入      |

##### 返回值

无

##### 版本差异

| 版本号 | 差异       |
| --- | --- |
| V100   | 支持此功能 |

##### 依赖

- 头文件 pwm_v100.h
- 库文件 ts_otdk.a

##### 注意

无

##### 举例

无

##### 相关主题

[ts_lib_pwm_open](#ts_lib_pwm_open)

#### ts_lib_pwm_set_mode

##### 描述

设置指定 PWM 设备的工作模式(初始电平，以及波形个数类型)。

- 初始电平: 可选择高电平或低电平, 具体定义参见 : [ts_pwm_fpout_en_t](#ts_pwm_fpout_en_t)

- 波形个数类型: 可选择连续输出直至用户停止，或者指定输出 n 个波形, n 大于 1， 小于 4096；

  类型定义参见: [ts_pwm_mode_en_t](#ts_pwm_mode_en_t)

##### 语法

```c
ts_s32 ts_lib_pwm_set_mode(ts_s32 s32_handle, ts_u32 u32_channel, ts_pwm_mode_en_t en_mode,\
            ts_pwm_fpout_en_t en_fpout)
```

##### 参数

| 参数        | 数据类型 | 描述                     | 输入/输出 |
| --- | --- | --- | --- |
| s32_handle  | ts_s32   | 驱动设备句柄             | 输入      |
| u32_channel | ts_u32   | 指定的通道               | 输入      |
| en_mode     | ts_u32   | pwm 的工作模式           | 输入      |
| en_fpout    | ts_u32   | pwm 第一个相位的输出电平 | 输入      |

##### 返回值

| 返回值    | 描述             |
| --- | ---- |
| TS_OK     | 设置工作模式成功 |
| TS_ERR    | 其他错误         |
| TS_EINVAL | 参数错误         |
| TS_EBUSY  | pwm 设备忙       |

##### 版本差异

| 版本号 | 差异       |
| --- | --- |
| V100   | 支持此功能 |

##### 依赖

- 头文件 pwm_v100.h

- 库文件 ts_otdk.a

##### 注意

设置工作模式前 PWM 必须处于空闲状态，否则返回 TS_EBUSY

##### 举例

无

##### 相关主题

无

#### ts_lib_pwm_set_freq_duty

##### 描述

设置指定 PWM 设备对应通道的频率和占空比; 当指定频率计算有误差时，可以通过 en_duty_adj 

参数进行微调，以达到尽量符合用户的频率设定值。

##### 语法

```c
ts_s32 ts_lib_pwm_set_freq_duty(ts_s32 s32_handle, ts_u32 u32_channel, ts_u32 u32_freq,\
             ts_u32 u32_duty, ts_pwm_duty_adj_en_t en_duty_adj)
```

##### 参数

| 参数        | 数据类型             | 描述                          | 输入/输出 |
| --- | --- | --- | --- |
| s32_handle  | ts_s32               | 驱动设备句柄                  | 输入      |
| u32_channel | ts_u32               | 指定的通道                    | 输入      |
| u32_freq    | ts_u32               | pwm 输出波形的频率(单位：kHz) | 输入      |
| u32_duty    | ts_u32               | 占空比（0-100）               | 输入      |
| en_duty_adj | ts_pwm_duty_adj_en_t | 占空比有误差时的微调配置      | 输入      |

##### 返回值

| 返回值      | 描述         |
| --- | --- |
| TS_OK       | 设置成功     |
| TS_ERR      | 其他错误     |
| TS_EINVAL   | 参数错误     |
| TS_EBUSY    | pwm 设备忙   |
| TS_EPWMMODE | pwm 模式错误 |

##### 版本差异

| 版本号 | 差异       |
| --- | --- |
| V100   | 支持此功能 |

##### 依赖

- 头文件 pwm_v100.h

- 库文件 ts_otdk.a

##### 注意

参数 u32_freq 单位为 KHz

##### 举例

无

##### 相关主题

无

#### ts_lib_pwm_set_irqen

##### 描述

打开/关闭 PWM 指定通道的中断

##### 语法

```c
ts_s32 ts_lib_pwm_set_irqen(ts_s32 s32_handle, ts_u32 u32_channel, ts_pwm_irqen_en_t en_irqen)
```

##### 参数

| 参数        | 数据类型          | 描述               | 输入/输出 |
| --- | --- | --- | --- |
| s32_handle  | ts_s32            | 驱动设备句柄       | 输入      |
| u32_channel | ts_u32            | 指定的通道         | 输入      |
| en_irqen    | ts_pwm_irqen_en_t | 打开/关闭 pwm 中断 | 输入      |

##### 返回值

| 返回值    | 描述              |
| --- | --- |
| TS_OK     | 打开/关闭中断成功 |
| TS_ERR    | 其他错误          |
| TS_EINVAL | 参数错误          |

##### 版本差异

| 版本号 | 差异       |
| --- | --- |
| V100   | 支持此功能 |

##### 依赖

- 头文件 pwm_v100.h

- 库文件 ts_otdk.a

##### 注意

无

##### 举例

无

##### 相关主题

[ts_lib_pwm_set_callback](#ts_lib_pwm_set_callback ) 

#### ts_lib_pwm_set_callback

##### 描述

设置 PWM 指定的通道的中断回调函数, 通过 pv_priv 传递用户参数。

##### 语法

```c
ts_s32 ts_lib_pwm_set_callback(ts_s32 s32_handle, ts_u32 u32_channel, \
        ts_pwm_irqcb_fp_t fp_cb, ts_void *pv_priv)
```

##### 参数

| 参数        | 数据类型          | 描述                 | 输入/输出 |
| --- | --- | --- | --- |
| s32_handle  | ts_s32            | 驱动设备句柄         | 输入      |
| u32_channel | ts_u32            | 指定的通道           | 输入      |
| fp_cb       | ts_pwm_irqcb_fp_t | 回调函数             | 输入      |
| pv_priv     | ts_void *         | 回调函数用的私有参数 | 输入      |

##### 返回值

| 返回值    | 描述                 |
| --- | --- |
| TS_OK     | 设置中断回调函数成功 |
| TS_ERR    | 其他错误             |
| TS_EINVAL | 参数错误             |

##### 版本差异

| 版本号 | 差异       |
| --- | --- |
| V100   | 支持此功能 |

##### 依赖

- 头文件 pwm_v100.h

- 库文件 ts_otdk.a

##### 注意

只支持 RTOS 使用

##### 举例

无

##### 相关主题

[ts_lib_pwm_set_irqen](#ts_lib_pwm_set_irqen   ) 

#### ts_lib_pwm_start

##### 描述

启动 PWM 指定通道输出,不阻塞

##### 语法

```c
ts_s32 ts_lib_pwm_start(ts_s32 s32_handle, ts_u32 u32_channel, ts_u32 u32_period_count)
```

##### 参数

| 参数             | 数据类型 | 描述                                                        | 输入/输出 |
| ---- | --- | --- | --- |
| s32_handle       | ts_s32   | 驱动设备句柄                                                | 输入      |
| u32_channel      | ts_u32   | 指定的通道                                                  | 输入      |
| u32_period_count | ts_u32   | ONE-SHOT 模式输出的周期数（此参数在 CONTINUOUS 模式下无效） | 输入      |

##### 返回值

| 返回值      | 描述              |
| --- | --- |
| TS_OK       | 启动 pwm 输出成功 |
| TS_ERR      | 其他错误          |
| TS_EINVAL   | 参数错误          |
| TS_EPWMMODE | pwm 模式错误      |

##### 版本差异

| 版本号 | 差异       |
| --- | --- |
| V100   | 支持此功能 |

##### 依赖

- 头文件 pwm_v100.h

- 库文件 ts_otdk.a

##### 注意

参数 u32_period_count 在 CONTINUOUS 工作模式下无效

##### 举例

无

##### 相关主题

[ts_lib_pwm_start_timeout](#ts_lib_pwm_start_timeout ) 

[ts_lib_pwm_stop](#ts_lib_pwm_stop) 

#### ts_lib_pwm_start_timeout

##### 描述

PWM 以阻塞方式启动输出，一般用于等待指定个数的波形完成输出。连续模式不会阻塞。

##### 语法

```c
ts_s32 ts_lib_pwm_start_timeout(ts_s32 s32_handle, ts_u32 u32_channel, \
        ts_u32 u32_period_count, ts_u32 u32_timeout_ms)
```

##### 参数

| 参数             | 数据类型 | 描述                                                        | 输入/输出 |
| ---- | --- | --- | --- |
| s32_handle       | ts_s32   | 驱动设备句柄                                                | 输入      |
| u32_channel      | ts_u32   | 指定的通道                                                  | 输入      |
| u32_period_count | ts_u32   | ONE-SHOT 模式输出的周期数（此参数在 CONTINUOUS 模式下无效） | 输入      |
| u32_timeout_ms   | ts_u32   | pwm 超时时间（单位：ms）                                    | 输入      |

##### 返回值

| 返回值      | 描述              |
| --- | --- |
| TS_OK       | 启动 pwm 输出成功 |
| TS_ERR      | 其他错误          |
| TS_EINVAL   | 参数错误          |
| TS_EPWMMODE | pwm 模式错误      |

##### 版本差异

| 版本号 | 差异       |
| --- | --- |
| V100   | 支持此功能 |

##### 依赖

- 头文件 pwm_v100.h

- 库文件 ts_otdk.a

##### 注意

- 参数 u32_timeout_ms 单位为 ms

##### 举例

```c
ts_s32 s32_tp_pwm;
ts_u32 u32_channel, u32_freq, u32_duty, u32_period_count, u32_timeout_ms;

insmod("pwm", ts_null);

s32_tp_pwm = ts_lib_pwm_open("/dev/tdk/pwm");
if (s32_tp_pwm >= 0) {
    u32_channel = 0;
    u32_freq = 10;
    u32_duty = 75;
    u32_period_count = 100;
    u32_timeout_ms = 1000;


    ts_lib_pwm_set_mode(s32_tp_pwm, u32_channel, TS_PWM_MODE_ONESHOT, TS_PWM_PFOUT_HIGH);
    ts_lib_pwm_set_freq_duty(s32_tp_pwm, u32_channel, u32_freq, u32_duty, TS_PWM_DUTY_ROUNDING);
    ts_lib_pwm_set_irqen(s32_tp_pwm, u32_channel, TS_PWM_IRQ_ENABLE);
    ts_lib_pwm_start_timeout(s32_tp_pwm, u32_channel, u32_period_count, u32_timeout_ms);

    ts_lib_pwm_close(s32_tp_pwm);
}
```

##### 相关主题

[ts_lib_pwm_start](#ts_lib_pwm_start ) 

[ts_lib_pwm_stop](#ts_lib_pwm_stop) 

#### ts_lib_pwm_stop

##### 描述

停止 PWM 指定通道输出

##### 语法

```c
ts_s32 ts_lib_pwm_stop(ts_s32 s32_handle, ts_u32 u32_channel)
```

##### 参数

| 参数        | 数据类型 | 描述         | 输入/输出 |
| --- | --- | --- | --- |
| s32_handle  | ts_s32   | 驱动设备句柄 | 输入      |
| u32_channel | ts_u32   | 指定的通道   | 输入      |

##### 返回值

| 返回值    | 描述              |
| --- | --- |
| TS_OK     | 停止 pwm 输出成功 |
| TS_ERR    | 其他错误          |
| TS_EINVAL | 参数错误          |

##### 版本差异

| 版本号 | 差异       |
| --- | --- |
| V100   | 支持此功能 |

##### 依赖

- 头文件 pwm_v100.h

- 库文件 ts_otdk.a

##### 注意

无

##### 举例

无

##### 相关主题

[ts_lib_pwm_start](#ts_lib_pwm_start ) 

[ts_lib_pwm_start_timeout](#ts_lib_pwm_start_timeout)







#### ts_lib_pwm_set_qdr_attr

##### 描述

设置磁编模块的属性, 属性参数参见 : 

##### 语法

```c
ts_s32 ts_lib_pwm_set_qdr_attr(ts_s32 s32_handle, ts_pwm_qdr_attr_s_t *pst_qdr_attr)
```

##### 参数

| 参数        | 数据类型 | 描述         | 输入/输出 |
| --- | --- | --- | --- |
| s32_handle  | ts_s32   | PWM 驱动设备句柄 | 输入      |
| pst_qdr_attr | ts_pwm_qdr_attr_s_t *   | 参数指针   | 输入      |

##### 返回值

| 返回值    | 描述              |
| --- | --- |
| TS_OK     | 设置成功 |
| TS_ERR    | 设置失败          |
| TS_EINVAL | 入参无效          |

##### 版本差异

| 版本号 | 差异       |
| --- | --- |
| V100   | 支持此功能 |

##### 依赖

- 头文件 pwm_v100.h

- 库文件 ts_otdk.a

##### 注意

无

##### 举例

无

##### 相关主题

无



#### ts_lib_pwm_set_qdr_compare_cnt

##### 描述

设置磁编读数比较值，当磁编读数达到指定值时，会触发中断。

##### 语法

```c
ts_s32 ts_lib_pwm_set_qdr_compare_cnt(ts_s32 s32_handle, ts_u32 u32_qdr_id, ts_u32 u32_cnt)
```

##### 参数

| 参数        | 数据类型 | 描述         | 输入/输出 |
| --- | --- | --- | --- |
| s32_handle  | ts_s32   | PWM 驱动设备句柄 | 输入      |
| u32_qdr_id  | ts_u32   | 磁编通道 ID | 输入      |
| u32_cnt | ts_u32   | 磁编读数比较值   | 输入      |

##### 返回值

| 返回值    | 描述              |
| --- | --- |
| TS_OK     | 设置成功 |
| TS_ERR    | 设置失败          |
| TS_EINVAL | 入参无效          |

##### 版本差异

| 版本号 | 差异       |
| --- | --- |
| V100   | 支持此功能 |

##### 依赖

- 头文件 pwm_v100.h

- 库文件 ts_otdk.a

##### 注意

无

##### 举例

无

##### 相关主题

无



#### ts_lib_pwm_get_qdr_cnt

##### 描述

用户主动获取磁编读数

##### 语法

```c
ts_s32 ts_lib_pwm_get_qdr_cnt(ts_s32 s32_handle, ts_u32 u32_qdr_id, ts_u32 *pu32_cnt)
```

##### 参数

| 参数        | 数据类型 | 描述         | 输入/输出 |
| --- | --- | --- | --- |
| s32_handle  | ts_s32   | PWM 驱动设备句柄 | 输入      |
| u32_qdr_id  | ts_u32   | 磁编通道 ID | 输入      |
| u32_cnt | ts_u32 *   | 磁编读数   | 输出      |

##### 返回值

| 返回值    | 描述 |
| --- | --- |
| TS_OK     | 获取成功 |
| TS_ERR    | 获取失败 |
| TS_EINVAL | 入参无效 |

##### 版本差异

| 版本号 | 差异       |
| --- | --- |
| V100   | 支持此功能 |

##### 依赖

- 头文件 pwm_v100.h

- 库文件 ts_otdk.a

##### 注意

无

##### 举例

无

##### 相关主题

无















### 数据类型

| 数据类型 | 用途说明 |
| --- | --- |
| [ts_pwm_mode_en_t](#ts_pwm_mode_en_t) | pwm 工作模式枚举 |
| [ts_pwm_fpout_en_t](#ts_pwm_fpout_en_t) | pwm 初始相位枚举 |
| [ts_pwm_duty_adj_en_t](#ts_pwm_duty_adj_en_t) | pwm 占空比微调枚举 |
| [ts_pwm_irqen_en_t](#ts_pwm_irqen_en_t) | pwm 中断开关枚举 |
| [ts_pwm_mode_s_t](#ts_pwm_mode_s_t) | pwm 模式 |
| [ts_pwm_freq_duty_s_t](#ts_pwm_freq_duty_s_t) | pwm 频率、占空比配置 |
| [ts_pwm_irqen_s_t](#ts_pwm_irqen_s_t) | pwm 中断使能配置 |
| [ts_pwm_irqcb_attr_s_t](#ts_pwm_irqcb_attr_s_t) | pwm 中断回调属性 |
| [ts_pwm_output_s_t](#ts_pwm_output_s_t) | pwm 输出配置 |
| [ts_pwm_irqcb_fp_t](#ts_pwm_irqcb_fp_t) | pwm 中断回调函数定义 |

#### ts_pwm_mode_en_t

##### 说明

pwm 工作模式

##### 定义

```c
typedef enum ts_pwm_mode_en {
    TS_PWM_MODE_ONESHOT = 1,
    TS_PWM_MODE_CONTINUOUS,

    TS_PWM_MODE_BUTT
} ts_pwm_mode_en_t;
```

##### 成员

| 成员名称                   | 说明            |
| --- | --- |
| TS_PWM_MODE_ONESHOT    | ONE-SHOT 模式   |
| TS_PWM_MODE_CONTINUOUS | CONTINUOUS 模式 |

##### 注意

无

##### 相关数据类型与接口

[ts_lib_pwm_set_mode](#ts_lib_pwm_set_mode)

#### ts_pwm_fpout_en_t

##### 说明

PWM 第一个相位的输出电平

##### 定义

```c
typedef enum ts_pwm_fpout_en {
    TS_PWM_PFOUT_LOW = 0,
    TS_PWM_PFOUT_HIGH,

    TS_PWM_PFOUT_BUTT
} ts_pwm_fpout_en_t;
```

##### 成员

| 成员名称 | 说明 |
| --- | --- |
| TS_PWM_PFOUT_LOW  | 输出第一相位为低电平 |
| TS_PWM_PFOUT_HIGH | 输出第一相位为高电平 |

##### 注意

无

##### 相关数据类型与接口

[ts_lib_pwm_set_freq_duty](#ts_lib_pwm_set_freq_duty)

#### ts_pwm_duty_adj_en_t

##### 说明

占空比有误差时的微调配置

##### 定义

```c
typedef enum ts_pwm_duty_adj_en {
    TS_PWM_DUTY_ROUNDING = 0,
    TS_PWM_DUTY_ROUND_DOWN,
    TS_PWM_DUTY_ROUND_UP,

    TS_PWM_DUTY_BUTT
} ts_pwm_duty_adj_en_t;
```

##### 成员

| 成员名称 | 说明 |
| --- | --- |
| TS_PWM_DUTY_ROUNDING   | 四舍五入调整 |
| TS_PWM_DUTY_ROUND_DOWN | 向下取整调整 |
| TS_PWM_DUTY_ROUND_UP   | 向上取整调整 |

##### 注意

无

##### 相关数据类型与接口

[ts_lib_pwm_set_freq_duty](#ts_lib_pwm_set_freq_duty)

#### ts_pwm_irqen_en_t

##### 说明

打开/关闭 pwm 中断

##### 定义

```c
typedef enum ts_pwm_irqen_en {
    TS_PWM_IRQ_DISABLE = 0,
    TS_PWM_IRQ_ENABLE,

    TS_PWM_IRQ_EN_BUTT
} ts_pwm_irqen_en_t;
```

##### 成员

| 成员名称               | 说明          |
| --- | ---- |
| TS_PWM_IRQ_DISABLE | 打开 pwm 中断 |
| TS_PWM_IRQ_ENABLE  | 关闭 pwm 中断 |

##### 注意

无

##### 相关数据类型与接口

[ts_lib_pwm_set_irq](#ts_lib_pwm_set_irq)

#### ts_pwm_mode_s_t

##### 说明

pwm 模式

##### 定义

```c
typedef struct ts_pwm_mode_s {
    ts_u32 u32_channel;
    ts_pwm_mode_en_t en_mode;
    ts_pwm_fpout_en_t en_fpout;
} ts_pwm_mode_s_t;
```

##### 成员

| 成员名称    | 说明                     |
| --- | --- |
| u32_channel | pwm 通道                 |
| en_mode     | pwm 工作模式             |
| en_fpout    | pwm 第一个相位的输出电平 |

##### 注意

无

##### 相关数据类型与接口

[ts_lib_pwm_set_mode](#ts_lib_pwm_set_mode)

#### ts_pwm_freq_duty_s_t

##### 说明

pwm 频率、占空比

##### 定义

```c
typedef struct ts_pwm_freq_duty_s {
    ts_u32 u32_channel;
    ts_u32 u32_freq;
    ts_u32 u32_duty;
    ts_pwm_duty_adj_en_t en_duty_adj;
} ts_pwm_freq_duty_s_t;
```

##### 成员

| 成员名称    | 说明                         |
| --- | --- |
| u32_channel | pwm 通道                     |
| u32_freq    | pwm 输出波形的频率           |
| u32_duty    | pwm 占空比                   |
| en_duty_adj | pwm 占空比有误差时的微调配置 |

##### 注意

无

##### 相关数据类型与接口

[ts_lib_pwm_set_freq_duty](#ts_lib_pwm_set_freq_duty)

#### ts_pwm_irqen_s_t

##### 说明

pwm 中断使能配置

##### 定义

```c
typedef struct ts_pwm_irqen_s {
    ts_u32 u32_channel;
    ts_pwm_irqen_en_t en_irqen;
} ts_pwm_irqen_s_t;
```

##### 成员

| 成员名称    | 说明               |
| --- | --- |
| u32_channel | pwm 通道           |
| en_irqen    | 打开/关闭 pwm 中断 |

##### 注意

无

##### 相关数据类型与接口

[ts_lib_pwm_set_irqen](#ts_lib_pwm_set_irqen)

#### ts_pwm_irqcb_attr_s_t

##### 说明

pwm 中断回调属性

##### 定义

```c
typedef struct ts_pwm_irqcb_attr_s {
    ts_u32 u32_channel;
    ts_pwm_irqcb_fp_t fp_irqcb;
    void *pv_priv;
} ts_pwm_irqcb_attr_s_t;
```

##### 成员

| 成员名称 | 说明                       |
| --- | --- |
| fp_irqcb | pwm 中断回调函数           |
| pv_priv  | pwm 中断回调函数的私有参数 |

##### 注意

无

##### 相关数据类型与接口

[ts_lib_pwm_set_callback](#ts_lib_pwm_set_callback)

#### ts_pwm_output_s_t

##### 说明

pwm 输出配置

##### 定义

```c
typedef struct ts_pwm_output_s {
    ts_u32 u32_channel;
    ts_u32 u32_period_count;
    ts_u32 u32_timeout_ms;
} ts_pwm_output_s_t;
```

##### 成员

| 成员名称         | 说明                          |
| ---- | --- |
| u32_channel      | pwm 通道                      |
| u32_period_count | pwm ONE-SHOT 模式输出的周期数 |
| u32_timeout_ms   | pwm 超时时间（单位：ms）      |

##### 注意

无

##### 相关数据类型与接口

[ts_lib_pwm_start](#ts_lib_pwm_start)

[ts_lib_pwm_start_timeout](#ts_lib_pwm_start_timeout)

#### ts_pwm_irqcb_fp_t

##### 说明

中断回调函数定义

##### 定义

```c
typedef ts_void (*ts_pwm_irqcb_fp_t) (ts_u32, ts_void *);
```

##### 成员

无

##### 注意

无

##### 相关数据类型与接口

[ts_tdl_pwm_set_callback](#ts_tdl_pwm_set_callback)




#### ts_pwm_qdr_attr_s_t

##### 说明

磁编设置属性

##### 定义

```c
typedef struct ts_pwm_qdr_attr_s {
    ts_u32 u32_qdr_id;
    qdr_ctrl_un_t un_ctrl;
    ts_u32 u32_compare_cnt;
    ts_pwm_irqcb_fp_t fp_irqcb;
    ts_void *pv_priv;
} ts_pwm_qdr_attr_s_t;
```

##### 成员

| 成员名称 | 说明 |
| ---- | --- |
| u32_qdr_id | 磁编通道 ID |
| un_ctrl | 磁编工作控制配置, 具体参见 [qdr_ctrl_un_t](#qdr_ctrl_un_t) |
| u32_compare_cnt |  磁编读数比较值，当实际磁编计数达到或者超过此配置值时，将触发中断  |
| fp_irqcb | 磁编计数到达或超过指定值时，中断中将会调用此回调函数  |
| pv_priv | 传递给回调函数的用户数据  |

##### 注意

无

##### 相关数据类型与接口

[qdr_ctrl_un_t](#qdr_ctrl_un_t)
[ts_pwm_irqcb_fp_t](#ts_pwm_irqcb_fp_t)



#### qdr_ctrl_un_t

##### 说明

磁编工作控制配置，指定磁编模块的计数方式，中断使能等模式

##### 定义

```c
typedef union qdr_ctrl_un {
    struct {
        ts_u32 work_en:1;
        ts_u32 interrupt_en:1;
        ts_u32 work_mode:1;
        ts_u32 auto_clr_en:1;
        ts_u32 deglitch_en:1;
        ts_u32 pa_cnt_en:1;
        ts_u32 pb_cnt_en:1;
        ts_u32 pa_oppo_level:1;
        ts_u32 pa_posedge_en:1;
        ts_u32 pa_negedge_en:1;
        ts_u32 pb_oppo_level:1;
        ts_u32 pb_posedge_en:1;
        ts_u32 pb_negedge_en:1;
    };
    ts_u32 u32_val;
} qdr_ctrl_un_t;
```

##### 成员

| 成员名称 | 说明 |
| ---- | --- |
| work_en | 工作使能 |
| interrupt_en | 中断使能 |
| work_mode |  工作模式； 0 为单纯计数，此时中断无效； 1 为计数比较  |
| auto_clr_en | 使能后，进入磁编比较中断时，将会对磁编计数清零 |
| deglitch_en | 去毛刺使能  |
| pa_cnt_en | A相计数使能，高有效  |
| pb_cnt_en | B相计数使能，高有效  |
| pa_oppo_level | pa相反信号的电平选择；0x0：低电平；0x1：高电平  |
| pa_posedge_en | pa上升沿计数使能，高有效  |
| pa_negedge_en | pa下降沿计数使能，高有效  |
| pb_oppo_level | pb相反信号的电平选择；0x0：低电平；0x1：高电平  |
| pb_posedge_en | pb上升沿计数使能，高有效  |
| pb_negedge_en | pb下降沿计数使能，高有效  |

##### 注意

- interrupt_en 关闭时，磁编读数比较中断将无法触发，回调函数也不会被调用。

##### 相关数据类型与接口

[ts_pwm_qdr_attr_s_t](#ts_pwm_qdr_attr_s_t)




### 错误码

| 错误代码   | 宏定义      | 说明         |
| --- | --- | --- |
| 0x5B035001 | TS_EPWMMODE | pwm 模式错误 |

----
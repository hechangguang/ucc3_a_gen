支持640探测器和1280探测器固件编译：
640探测器编译：
1、修改 develop_dv220\ucc3_a_gen\sample\vision\Makefile文件，代码段为 snr ?= planck640c
2、修改 develop_dv220\ucc3_a_gen\Makefile文件，代码段为 
vot_config:
	@python3 generate_vot_config.py  $(WORK_DIR)/isp_config.bin
#	@python3 generate_vot_config_1280.py  $(WORK_DIR)/isp_config.bin
3、使用全编译命令： make vot_all  编译

1280探测器编译：
1、修改 develop_dv220\ucc3_a_gen\sample\vision\Makefile文件，代码段为 snr ?= planck1280c
2、修改 develop_dv220\ucc3_a_gen\Makefile文件，代码段为 
vot_config:
#	@python3 generate_vot_config.py  $(WORK_DIR)/isp_config.bin
	@python3 generate_vot_config_1280.py  $(WORK_DIR)/isp_config.bin
3、使用全编译命令： make vot_all  编译

#### 固件数据文件格式说明

VOT_ASIC_V000.1.0.02.100000.bin = [ts_vision.uimg + 64bytes 固定的校验信息]

64byte校验信息结构：
    数据结构：
    | 0-1字节 | 2-5字节       | 6-22字节       | 23-63字节    |
    |---------|--------------|----------------|--------------|
    | CRC16   | 日期(BCD编码) | 版本(ASCII)    | 动态填充     |

执行：
生成ts_vision.uimg
make vision 

生成VOT_ASIC_V000.1.0.02.100000.bin
make vision_package

会在tdk_release/images/目录下生成：VOT_ASIC_V000.1.0.02.100000.bin

ubuntu系统下，查看VOT_ASIC_V000.1.0.02.100000.bin的尾部64字节信息：


查看头部的16个字节数据
head -c 16 VOT_ASIC_V000.1.0.02.100000.bin  | xxd

查看尾部的64个字节数据
tail -c 64 VOT_ASIC_V000.1.0.02.100000.bin  | xxd



### 上位机soc升级程序流程简要说明
数据传输流程：
pc -> ir 开始
56 4F 54 00 06 00 AC 3A 01 A7 17

ir -> pc 开始应答
56 4F 54 00 06 00 CA 3A 01 B8 F7

pc -> ir 上位机应答
56 4F 54 00 06 00 AC 3A 02 A6 57

ir -> pc 
56 4F 54 00 06 00 CA 3A 02 B9 B7
 
pc -> ir 传输第一段
56 4F 54 05 09 00 AC 3A 00 00

ir -> pc 第一段应答
56 4F 54 00 06 00 CA 3A 06 7A B6

目前只有第一段数据才会有应答，第二段开始不应答
pc -> ir 传输第二段
56 4F 54 03 27 00 AC 3A 00 01

ir -> pc 第二段应答
56 4F 54 00 06 00 CA 3A 06 7A B6


pc -> ir 数据传输完成后，发送下面的指令
56 4F 54 00 06 00 AC 3A 03 66 96

ir -> pc  升级成功
56 4F 54 00 06 00 CA 3A 03 79 76

ir -> pc  数据校验失败
56 4F 54 00 06 00 CA 3A 04 BB 37

ir -> pc  程序版本不匹配
56 4F 54 00 06 00 CA 3A 05 7B F6

传完一个文件数据，上位机会在这段数据的最后面加上两个字节的CRC16
拿到数据后，需要
16bit
【16byte 头】 + ts_uimg + 【 2byte crc16-mobus】 


当前固件组成
ts_uimg + [2byte crc16-mobus] 


### git版本tags 打包指令简要说明：

对c4e47d8对应的版本，打上v000.1.0.01.100000的tag
git  tag  -a  v000.1.0.01.100000 -m "修复下发生产对应的bug" c4e47d8

推送到远端
git push origin v000.1.0.01.100000
git push origin  --tags
git tag -l


查看git  show   v000.1.0.01.100000
git  show   v000.1.0.01.100000


### 版本编译

1、根据用户定制类型修改products_rules.mk文件
变量名                   取值
CUSTOMER_TYPE           [VOT_NORMAL ]
HARD_PLATFORM_TYPE      [FPGA, ASIC, VS ]
DETECTOR_TYPE           [VOT, RTD ]
RESOLUTION_TYPE         [RES_640_512, RES_384_288 ]
VIDEO_OUTPUT_TYPE       [CVBS, LIBUSB, UVC, MIPI ]
FUNCTION_TYPE           [PICTURE_TYPE, TEMP_CALC_TYPE ]
FOCUS_TYPE              [FIXED_TYPE, AUTO_TYPE ]
FIRMWARE_SOFT_VERSION   [x.x.xx]


2、编译应用程序

make vision


3、打包应用程序

make vision_package


会在tdk_release/images/目录下生成：VOT_ASIC_V000.1.0.02.100000.bin
将该固件发布给客户使用。







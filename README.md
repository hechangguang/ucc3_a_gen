2026.09.20 
1、移植下发的编码器变焦程序功能 <000.1.2.0.132425.300.001>：
   针对编码器调焦、变倍的量程做±100冗余处理操作；

000.1.1.25.132425.300.006
QL:1、移植自动调焦、变焦相关功能逻辑，包括电位器版本和编码器版本，合并成一版:通过菜单二->System->Misc->Lens菜单项切换调焦/变焦功能，切换后需要重启机器才生效;
(PRIME--定焦机器，F_POT--调焦（电位器）机器，Z_POT--变焦（电位器）机器，F_ENC--调焦（编码器）机器，Z_ENC--变焦（编码器）机器)
2、版本号信息增加调焦类型机器版本说明，串口同步增加：.FP———调焦电位器版，.ZP———变焦电位器版，.FP———调焦编码器版，.ZP———变焦编码器版；
3、原有的T_RUL菜单位置变更为：System->Temp_Opt->T_RUL, GEN代表通用版测温规律，QC代表千从版测温规律，HAND3代表手持机384机器测温规律；
4、针对手持机384测温机器，增添测温规律公式：
   a.修改自动INT一二档规律；
   b.增加Sensor一档规律；
   c.增加打档操作自动调整GOF功能Auto_GOFSet_HAND(),仅限手持机384一档测温使用;

000.1.1.24.132414.300.003（千从项目）
QL：1、同步千从项目程序的功能改动，用QC_PROJECT宏定义区分，出具QC固件时注意要打开此宏定义，版本号中FUNCTION_TYPE同步修改为测温版本（FUNCTION_TYPE=INFRARED_THERMOMETRY_TYPE）;
2、测温版本默认初始化使用UYVY格式，即SystemParaMenu2.m_uvc_output_format = UVC_YUV_FORMAT_UYVY。
3、m_cvbs_display_mode和m_uvc_enlarger_mode开关测温版本默认是关闭状态。

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


000.1.1.3.100000.000.000：
QL:1、新增半自动校正坏点功能。
   2、修复cvbs菜单显示偏移问题
   3、修复镜像选择断电起来不生效问题

000.1.1.1.100000.000.000：
SXB：
1.支持上位机uvc视频源校正坏点；
2.为了增加用户坏点校正率，实现分离K表和B表存储和管理；
3.新增支持nuc k表开关控制，支持坏点开关控制，支持k=1.0矫正坏点；
4.支持uvc和cvsb同时现实菜单
5.支持新标定流程，一键det，一键occ，nuc，sffc，坏点矫正，更新sffc
6.修复开启画面卡20秒的问题


000.1.1.0.100000.000.004：
QL：1、坏点校正相关指令功能修改，坏点光标显示/隐藏指令已加，但功能待添加；

000.1.0.15.100000.000.003 ：
QL:1、串口协议指令功能与菜单功能进行核对；

000.1.0.01.100000.000.002 ：
ZY：1、FF指令中增加 保存NUC相关功能，兼容之前上位机的指令，在NUC校正相关函数中也有保存指令，后续尽量不要重复增加指令



000.1.0.01.100000.000.001 ：
QL：1、增添菜单二全功能串口指令功能；
    2、修复打开DET菜单会触发DET初始化操作的bug；
    3、NUC_Calc计算方式固定，左键采低温数据，右键采高温数据以及计算K值；
    4、“场景”菜单参数变量m_dev_env_mode调整至菜单一下UsermenuParameter{}结构体中；
    5、菜单一二的初始化、保存操作整理，菜单一只初始化/保存菜单一的相关参数，菜单二则会菜单一二全部参数一起初始化/保存；
    6、电子放大调整为无极变倍，目前是X1.0 - 10.0倍，步进为0.1；
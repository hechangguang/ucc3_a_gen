# 
# Makefile -- vision 顶层 Makefile, 通过调用 tsp 或者 linux 的模块编译 makefile
#			  进行编译
# 

# 小写，便于输入
os ?= rtt
ost ?= rtos
soc ?= T10x
hwv ?= n6
# lvgl
uvc ?= y
acm ?= y
libusb ?= y
#libusb2 ?= y
# DEV_SIM_MODE ?= y
# DSW_PRECENT_MODE_DEBUG ?= y

VOT_STRIP_MODE ?= y

# DEV_TESTBAR_MODE ?= y

# WLP_PLK640_DEV ?= y

# VOT_BT656_EN ?= y

# VOT_MIPI_EN ?= y

# VOT_RAW_YUV_EN ?= y

# DEV_RTDS123C_1280_MODE ?= y


# VOT_NUC_SFFC_TEST ?= y

# VOT_SFFC_TEST ?= y

# VOT_USER_INFO ?= y

# VOT_FIRE_HOT ?= y

# VOT_MIPI_RESET ?= y

# 工程编译选项

# 探测器选择planck640c planck384c planck1280c rv1280 hik2512 wy640 testbar sim ...
# 选择光智1280分辨率探测器planck1280c，编译时，打开此选项snr ?= planck1280c
# 选择光智640分辨率探测器planck640c，编译时，打开此选项snr ?= planck640c
snr ?= planck640c

# 指定探测器帧率
fps ?= 50

# 是否开启超分
sr ?= y
# 是否开启 AISP， 不能与 SR 一起开启
aisp ?= y

# 是否开启温补支持
sl ?= y

# 是否开启去固定横竖纹线程
snc ?= y

# 是否开启去坏斑算法
extbp ?= y

# 指定板子型号
brd ?= vot
bt ?=

# 指定 ISP 配置
ispcfg ?= def

# 指定 SR 模型: c4 c8 b lite lite2 def
srm ?= lite

# 客户板子代码
cus ?= vot

os_root ?= ../../tdk_release_v100

# 自动产生环境
MOD_DIR = $(shell if [ "$$PWD" != "" ]; then echo $$PWD; else pwd; fi)

# 内部指定
TARGET_NAME ?= vision

APP_NAME = ts_$(TARGET_NAME)


include ../../vot_config.mk
include ../../products_rules.mk


EXTRA_CFLAG += -DCUSTOMER_TYPE=\"$(subst ",,$(CUSTOMER_TYPE))\"
EXTRA_CFLAG += -DRESOLUTION_TYPE=\"$(subst ",,$(RESOLUTION_TYPE))\"
EXTRA_CFLAG += -DHARD_BOARD_PORT_TYPE=\"$(subst ",,$(HARD_BOARD_PORT_TYPE))\"
EXTRA_CFLAG += -DPRODUCTION_NAME=\"$(subst ",,$(PRODUCTION_NAME))\"
EXTRA_CFLAG += -DDETECTOR_TYPE_NAME=\"$(subst ",,$(DETECTOR_TYPE))\"
EXTRA_CFLAG += -DVOT_SOFTWARE_VER_FIXED_HEADER=\"$(subst ",,$(FIRMWARE_VERSION))\"
EXTRA_CFLAG += -DFOCUS_TYPE=\"$(subst ",,$(FOCUS_TYPE))\"
EXTRA_CFLAG += -DFUNCTION_TYPE=\"$(subst ",,$(FUNCTION_TYPE))\"
EXTRA_CFLAG += -DVIDEO_OUTPUT_TYPE=\"$(subst ",,$(VIDEO_OUTPUT_TYPE))\"

SDK_ROOT_DIR = $(MOD_DIR)/../../
VISION_DIR= $(MOD_DIR)/../../sample/vision
VOT_VISION_DIR= $(MOD_DIR)/../../vot_src



APP_SRC := $(VISION_DIR)/src
APP_INC :=  $(VISION_DIR)/inc

# 自动转换板级代码
#BRD_FILE := $(APP_SRC)/board/brd_$(strip $(brd)).c
#$(info brd=$(brd) -> using $(BRD_FILE))

# 自动转换板子类型配置
#EXTRA_CFLAG += $(if $(bt),-D$(shell echo "CONFIG_BT_$(subst =,_,$(bt))" | tr a-z A-Z),)

# 自动获取帧率
#EXTRA_CFLAG += $(if $(fps),-D$(shell echo "CONFIG_FPS_$(subst =,_,$(fps))" | tr a-z A-Z),)


EXTRA_CFLAG  += -I$(APP_INC)/ -I$(VISION_DIR)
EXTRA_CFLAG  += -I$(VISION_DIR)/src/inc

EXTRA_CFLAG  += -I$(VOT_VISION_DIR)/vot
EXTRA_CFLAG  += -I$(VOT_VISION_DIR)/inc
EXTRA_CFLAG  += -I$(VOT_VISION_DIR)/tlsf
EXTRA_CFLAG  += -I$(VOT_VISION_DIR)/log

APP_FILES = $(VISION_DIR)/src/main.c

APP_FILES += $(VOT_VISION_DIR)/tlsf/tlsf.c
APP_FILES += $(VOT_VISION_DIR)/tlsf/vot_tlsf.c


APP_FILES += $(VOT_VISION_DIR)/log/elog.c
APP_FILES += $(VOT_VISION_DIR)/log/elog_utils.c
APP_FILES += $(VOT_VISION_DIR)/log/elog_port.c

APP_FILES += $(VOT_VISION_DIR)/planck1280c_vot.c
APP_FILES += $(VOT_VISION_DIR)/planck640c_vot.c
APP_FILES += $(VOT_VISION_DIR)/fontlib_vot.c
APP_FILES += $(VOT_VISION_DIR)/snr_comm_vot.c

# comm
APP_FILES += $(APP_SRC)/comm/sffs_files.c
APP_FILES += $(APP_SRC)/comm/libupload.c
APP_FILES += $(APP_SRC)/comm/fontlib.c
APP_FILES += $(APP_SRC)/comm/tmm.c

# sensor
ifeq ($(snr),sim)
APP_FILES += $(APP_SRC)/sensor/viu_mem.c
EXTRA_CFLAG += -DCONFIG_SNR_SIM
else ifeq ($(snr),gst612)
APP_FILES += $(APP_SRC)/sensor/gd/gst612c2.c
EXTRA_CFLAG += -DCONFIG_SNR_GST612
else ifeq ($(snr),gst612wah)
APP_FILES += $(APP_SRC)/sensor/gd/gst612wah.c
EXTRA_CFLAG += -DCONFIG_SNR_GST612WAH
else ifeq ($(snr),gst412)
APP_FILES += $(APP_SRC)/sensor/gd/gst412.c
EXTRA_CFLAG += -DCONFIG_SNR_GST412
else ifeq ($(snr),gst212)
APP_FILES += $(APP_SRC)/sensor/gd/gst256.c
EXTRA_CFLAG += -DCONFIG_SNR_GST212
else ifeq ($(snr),timo212)
APP_FILES += $(APP_SRC)/sensor/gd/timo212.c
EXTRA_CFLAG += -DCONFIG_SNR_TIMO212
else ifeq ($(snr),gst1212)
APP_FILES += $(APP_SRC)/sensor/gd/gst1212.c
EXTRA_CFLAG += -DCONFIG_SNR_GST1212
else ifeq ($(snr),ak3212)
APP_FILES += $(APP_SRC)/sensor/ak/ak3212.c
EXTRA_CFLAG += -DCONFIG_SNR_AK3212
else ifeq ($(snr),va640)
APP_FILES += $(APP_SRC)/sensor/ak/va640.c
EXTRA_CFLAG += -DCONFIG_SNR_VA640
else ifeq ($(snr),ak612)
APP_FILES += $(APP_SRC)/sensor/ak/ak612.c
EXTRA_CFLAG += -DCONFIG_SNR_AK612
else ifeq ($(snr),elf256)
APP_FILES += $(APP_SRC)/sensor/iray/elf_256.c
EXTRA_CFLAG += -DCONFIG_SNR_ELF256
else ifeq ($(snr),rtd123c)
APP_FILES += $(APP_SRC)/sensor/iray/rtd_123c.c
EXTRA_CFLAG += -DCONFIG_SNR_RTD123C
else ifeq ($(snr),rtd6122)
APP_FILES += $(APP_SRC)/sensor/iray/rtd_6122.c
EXTRA_CFLAG += -DCONFIG_SNR_RTD6122
else ifeq ($(snr),ohle6081)
APP_FILES += $(APP_SRC)/sensor/iray/rtc_6081.c
EXTRA_CFLAG += -DCONFIG_SNR_OHLE6081
else ifeq ($(snr),ohle3123)
APP_FILES += $(APP_SRC)/sensor/iray/rtc_3123.c
EXTRA_CFLAG += -DCONFIG_SNR_OHLE3123
else ifeq ($(snr),rv1280)
APP_FILES += $(APP_SRC)/sensor/vot/rv1280.c
EXTRA_CFLAG += -DCONFIG_SNR_RV1280
else ifeq ($(snr),planck640c)
#APP_FILES += $(APP_SRC)/sensor/vot/planck640c.c
EXTRA_CFLAG += -DCONFIG_SNR_PLANCK640C
else ifeq ($(snr),planck384c)
#APP_FILES += $(APP_SRC)/sensor/vot/planck384c.c
EXTRA_CFLAG += -DCONFIG_SNR_PLANCK384C
else ifeq ($(snr),planck1280c)
#APP_FILES += $(APP_SRC)/sensor/vot/planck1280c.c
RESOLUTION_TYPE := RES_1280_1024
EXTRA_CFLAG += -DCONFIG_SNR_PLANCK1280C
else ifeq ($(snr),hik6412)
APP_FILES += $(APP_SRC)/sensor/hikmicro/hik_6412.c
EXTRA_CFLAG += -DCONFIG_SNR_HIK6412
else ifeq ($(snr),hik2512)
APP_FILES += $(APP_SRC)/sensor/hikmicro/hik_2512.c
EXTRA_CFLAG += -DCONFIG_SNR_HIK2512
else ifeq ($(snr),wy640)
APP_FILES += $(APP_SRC)/sensor/wy/wy640.c
EXTRA_CFLAG += -DCONFIG_SNR_WY640
else
# testbar
APP_FILES += $(APP_SRC)/sensor/testbar.c
EXTRA_CFLAG += -DCONFIG_SNR_TESTBAR
endif


APP_FILES += $(APP_SRC)/sensor/vot/planck1280c.c
APP_FILES += $(APP_SRC)/sensor/vot/planck640c.c
APP_FILES += $(APP_SRC)/sensor/vot/planck384c.c
APP_FILES += $(APP_SRC)/sensor/vot.c
APP_FILES += $(APP_SRC)/sensor/snr_comm.c
APP_FILES += $(APP_SRC)/sensor/testbar.c
APP_FILES += $(APP_SRC)/sensor/viu_mem.c
APP_FILES += $(APP_SRC)/sensor/rtd/rtd6122c.c
APP_FILES += $(APP_SRC)/sensor/rtd/rtds123c.c
APP_FILES += $(APP_SRC)/sensor/rtd_comm.c
# dbg
APP_FILES += $(APP_SRC)/debug/isp.c
APP_FILES += $(APP_SRC)/debug/i2c.c
# APP_FILES += $(APP_SRC)/debug/menu.c
APP_FILES += $(APP_SRC)/debug/vpo.c
# APP_FILES += $(APP_SRC)/debug/y16_get.c
# APP_FILES += $(APP_SRC)/debug/x16_get.c
APP_FILES += $(APP_SRC)/debug/templut.c
ifeq ($(VOT_FIRE_HOT),y)
APP_FILES += $(APP_SRC)/debug/fire_hot.c
endif

ifeq ($(sl),y)
APP_FILES += $(APP_SRC)/debug/sl.c
EXTRA_CFLAG += -DCONFIG_SL_EN
endif

ifeq ($(op),y)
EXTRA_CFLAG += -DCONFIG_OP_EN
APP_FILES += $(APP_SRC)/debug/op.c
endif

ifeq ($(lvgl),y)
APP_FILES += $(APP_SRC)/lvgl/lvgl.c
APP_FILES += $(APP_SRC)/lvgl/lv_demo_widgets.c
APP_FILES += $(APP_SRC)/lvgl/lv_demo_widgets_analytics.c
APP_FILES += $(APP_SRC)/lvgl/lv_demo_widgets_components.c
APP_FILES += $(APP_SRC)/lvgl/lv_demo_widgets_profile.c
APP_FILES += $(APP_SRC)/lvgl/lv_demo_widgets_shop.c
APP_FILES += $(APP_SRC)/lvgl/assets/img_lvgl_logo.c
APP_FILES += $(APP_SRC)/lvgl/assets/img_demo_widgets_avatar.c
APP_FILES += $(APP_SRC)/lvgl/assets/img_demo_widgets_needle.c
APP_FILES += $(APP_SRC)/lvgl/assets/img_clothes.c
EXTRA_CFLAG += -DCONFIG_LVGL
endif
APP_FILES += $(VOT_VISION_DIR)/vot/menu/votmain.c
APP_FILES += $(VOT_VISION_DIR)/vot/menu/display.c
APP_FILES += $(VOT_VISION_DIR)/vot/menu/ASCII.c
APP_FILES += $(VOT_VISION_DIR)/vot/menu/name.c
APP_FILES += $(VOT_VISION_DIR)/vot/menu/testmenu.c
APP_FILES += $(VOT_VISION_DIR)/vot/menu/usermenu.c
APP_FILES += $(VOT_VISION_DIR)/vot/menu/votmenu.c
APP_FILES += $(VOT_VISION_DIR)/vot/menu/IDE_tree.c
APP_FILES += $(VOT_VISION_DIR)/vot/menu/usermenufunction.c
APP_FILES += $(VOT_VISION_DIR)/vot/menu/testmenufunction.c
APP_FILES += $(VOT_VISION_DIR)/vot/menu/ispfunction.c
APP_FILES += $(VOT_VISION_DIR)/vot/menu/menu_save_flash.c
APP_FILES += $(VOT_VISION_DIR)/vot/common/common.c
APP_FILES += $(VOT_VISION_DIR)/vot/common/function.c
APP_FILES += $(VOT_VISION_DIR)/vot/common/spi_flash.c
APP_FILES += $(VOT_VISION_DIR)/vot/common/Zoom_Lens.c
APP_FILES += $(VOT_VISION_DIR)/vot/common/gm1215.c
APP_FILES += $(VOT_VISION_DIR)/vot/uart/uart_vot.c
APP_FILES += $(VOT_VISION_DIR)/vot/vot_config.c
APP_FILES += $(VOT_VISION_DIR)/vot/cJSON/cJSON.c
APP_FILES += $(VOT_VISION_DIR)/vot/common/TempCal.c



ifeq ($(VOT_MIPI_RESET),y)
EXTRA_CFLAG += -DVOT_MIPI_RESET
endif


ifeq ($(VOT_SFFC_TEST),y)
EXTRA_CFLAG += -DVOT_SFFC_TEST
endif

ifeq ($(DSW_PRECENT_MODE_DEBUG),y)
EXTRA_CFLAG += -DDSW_PRECENT_MODE_DEBUG
endif

ifeq ($(VOT_FIRE_HOT),y)
EXTRA_CFLAG += -DVOT_FIRE_HOT
endif

ifeq ($(DEV_SIM_MODE),y)
EXTRA_CFLAG += -DDEV_SIM_MODE
endif

ifeq ($(DEV_TESTBAR_MODE),y)
EXTRA_CFLAG += -DDEV_TESTBAR_MODE
endif

ifeq ($(VOT_NUC_SFFC_TEST),y)
EXTRA_CFLAG += -DVOT_NUC_SFFC_TEST
endif

# ifeq ($(VOT_USER_INFO),y)
# EXTRA_CFLAG += -DVOT_USER_INFO
# endif



# ifeq ($(DEV_RTDS123C_1280_MODE),y)
# EXTRA_CFLAG += -DDEV_RTDS123C_1280_MODE
# endif


ifeq ($(VOT_BT656_EN),y)
EXTRA_CFLAG += -DVOT_BT656_EN
endif


ifeq ($(VOT_MIPI_EN),y)
EXTRA_CFLAG += -DVOT_MIPI_EN
endif

ifeq ($(VOT_RAW_YUV_EN),y)
EXTRA_CFLAG += -DVOT_RAW_YUV_EN
endif


# ifeq ($(WLP_PLK640_DEV),y)
# EXTRA_CFLAG += -DWLP_PLK640_DEV
# endif


ifeq ($(sr),y)
EXTRA_CFLAG += -DCONFIG_SR_EN
endif

ifeq ($(snc),y)
EXTRA_CFLAG += -DCONFIG_FIX_SNC
endif

ifeq ($(extbp),y)
EXTRA_CFLAG += -DCONFIG_EXTBP_EN
endif

ifeq ($(sr),y)
ifeq ($(srm),c4)
EXTRA_CFLAG += -DCONFIG_SRM_C4
else ifeq ($(srm),lite)
EXTRA_CFLAG += -DCONFIG_SRM_LITE
else ifeq ($(srm),b)
EXTRA_CFLAG += -DCONFIG_SRM_B
else ifeq ($(srm),c8)
EXTRA_CFLAG += -DCONFIG_SRM_C8
else
EXTRA_CFLAG += -DCONFIG_SRM_BASE
endif
endif

# == usb ==
ifeq ($(uvc),y)
APP_FILES += $(APP_SRC)/usb/uvc/vcapture.c
APP_FILES += $(APP_SRC)/usb/class/usbd_video.c
EXTRA_CFLAG += -DCONFIG_UVC

# uvc+acm
ifeq ($(acm),y)
APP_FILES += $(APP_SRC)/usb/class/usbd_cdc_acm.c
EXTRA_CFLAG += -DCONFIG_UVC_WITH_ACM
endif
else
ifeq ($(acm),y)
APP_FILES += $(APP_SRC)/usb/class/usbd_cdc_acm.c
APP_FILES += $(APP_SRC)/usb/cdc/cdc_acm.c
EXTRA_CFLAG += -DCONFIG_CDC_ACM
endif
endif

ifeq ($(libusb),y)
APP_FILES += $(APP_SRC)/usb/class/libusb.c
APP_FILES += $(APP_SRC)/usb/libusb/usb.c
EXTRA_CFLAG += -DCONFIG_LIBUSB
endif

ifeq ($(libusb2),y)
APP_FILES += $(APP_SRC)/usb/class/libusb2.c
APP_FILES += $(APP_SRC)/usb/libusb2/usb.c
EXTRA_CFLAG += -DCONFIG_LIBUSB2
endif

# math
APP_FILES += $(APP_SRC)/math/exp.c
APP_FILES += $(APP_SRC)/math/pow.c
APP_FILES += $(APP_SRC)/math/sqrt.c
APP_FILES += $(APP_SRC)/math/log.c
APP_FILES += $(APP_SRC)/math/fabs.c
APP_FILES += $(APP_SRC)/math/floorf.c
APP_FILES += $(APP_SRC)/math/modff.c

EXTAR_INC := $(APP_INC)

binf := $(SDK_ROOT_DIR)/isp_config.bin

WHOLE_LIBS +=   $(SDK_ROOT_DIR)/tdk_release/lib/isp_config.bin.a

sinclude Makefile.$(ost)

.PHONY: all clr

all: build_$(ost)


clr:
	rm -rf build/*

deps_config := \
	src/driver/uart/Kconfig \
	src/driver/scpu/Kconfig \
	src/driver/efuse/Kconfig \
	src/driver/ddr/Kconfig \
	src/driver/clock/Kconfig \
	src/driver/Kconfig \
	src/ih/Kconfig \
	src/cmd/Kconfig \
	src/platform/Kconfig \
	/work/t10x_tdk/01.software/00.board/tmp/gitlab/ucc3_a_gen/system/hook/code/src/Kconfig

include//config/auto.conf: \
	$(deps_config)


$(deps_config): ;

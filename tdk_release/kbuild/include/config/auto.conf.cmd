deps_config := \
	klib/sh/Kconfig \
	klib/Kconfig \
	kdrv/serial/Kconfig \
	kdrv/icd/Kconfig \
	kdrv/systick/Kconfig \
	kdrv/cache/Kconfig \
	kdrv/Kconfig \
	fs/Kconfig \
	os/Kconfig \
	arch/Kconfig \
	/home/TOP-SILICON/10004/wksp/workday/local/tdk/tdk_ws/system/tsp/Kconfig

/home/TOP-SILICON/10004/wksp/workday/local/tdk/tdk_ws/system/tsp/include//config/auto.conf: \
	$(deps_config)


$(deps_config): ;

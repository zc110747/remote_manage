################################################################################
#
# Build for multi files.
# Copyright (c) 2020-2023
# Original Author: 1107473010@qq.com
#
################################################################################
dir := ${ENV_PATH_ROOT}/mod

subdirs := 	${dir}/01.led \
		${dir}/02.loopled \
		${dir}/03.beep \
		${dir}/04.i2c_ap/bus_i2c \
		${dir}/04.i2c_ap/regmap_i2c \
		${dir}/05.spi_icm/bus_spi \
		${dir}/05.spi_icm/regmap_spi \
		${dir}/06.key/key_async \
		${dir}/06.key/key_input \
		${dir}/06.key/key_rwio \
		${dir}/07.adc \
		${dir}/08.wire/dev_hx711 \
		${dir}/08.wire/iio_hx711 \
		${dir}/09.rtc/bus_rtc \
		${dir}/09.rtc/regmap_rtc \
		${dir}/10.rng/hw-rngc \
		${dir}/11.nvmem \
		${dir}/14.touch_key/goodix_ts_i2c \
		${dir}/17.pwm/pwm_consumer \
		${dir}/18.pwm/pwm_provider


with_kernel :=0
include $(ENV_PATH_ROOT)/build/buildmulti.mk

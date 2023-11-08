################################################################################
#
# THIS FILE IS BASIC MAKE FILE FOR
# Copyright (c) 2020-2021
# Original Author: 1107473010@qq.com
#
################################################################################

# This is object should be build and will link to generate
# the executables
CC = $(APP_CC_COMPILE)
CFLAGS := -O3 -std=c++17 -fno-strict-aliasing -lrt 

objects = 	main.o \
			remote/tcp_thread.o 	 \
			remote/uart_thread.o 		 \
			remote/protocol.o			 \
			module/system_config.o \
			module/calculate.o  \
			module/time_manage.o \
			module/fifo_manage.o \
			module/asio_server.o \
			module/cmd_process.o  \
			driver/rtc.o \
			driver/key.o \
			driver/spi_icm.o \
			driver/device_base.o \
			driver/driver.o \
			logger/logger.o \
			center/internal_process.o \
			center/device_manage.o   \
			center/center_manage.o


clear_dobject = *.d .*.o.d \
				module/*.d module/.*.o.d \
				driver/*.d driver/.*.o.d \
				event/*.d event/.*.o.d \
				lib/*.d lib/.*.o.d \
				logger/*.d logger/.*.o.d \
				remote/*.d remote/.*.o.d 

include_path = -I remote/ 				\
				-I driver/ 				\
				-I lib/ 				\
				-I include/ 			\
				-I module/ 				\
				-I lib/jsoncpp/include  \
				-I lib/asio/include		\
				-I logger/ 				\
				-I module/tools/ 		\
				-I event/				\
				-I center/

ifeq ($(CC), g++)		
	lib = lib/jsoncpp/x86_libjsoncpp.a
else
	lib = lib/jsoncpp/arm_libjsoncpp.a
endif

# This is the file name of the executables and path where 
# place the executables
executables = app_demo
executables_path = ../Executables

# Including this file allows access to the Build System.
include ../../buildtool/basic.mk

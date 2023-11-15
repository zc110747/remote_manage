################################################################################
#
# THIS FILE IS BASIC MAKE FILE FOR
# Copyright (c) 2020-2021
# Original Author: 1107473010@qq.com
#
################################################################################

# This is object should be build and will link to generate
# the executables
BUILD=e

CCFLAGS := -O3 -std=c++17 -fno-strict-aliasing -lrt 

cpp_objects = 	main.o \
			remote/tcp_thread.o 	 \
			remote/uart_thread.o 		 \
			remote/protocol.o			 \
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
			center/center_manage.o   \
			parameter/system_config.o

INCLUDES = 	-I remote/ 				\
			-I driver/ 				\
			-I include/ 			\
			-I module/ 				\
			-I logger/ 				\
			-I module/tools/ 		\
			-I event/				\
			-I parameter/			\
			-I center/				
				
INCLUDES +=	-I $(ENV_PATH_ROOT)/thirdparts/asio/include/ \
			-I $(ENV_PATH_ROOT)/thirdparts/jsoncpp/include/ 

ifeq ($(APP_CC_COMPILE),g++)
library = $(ENV_PATH_ROOT)/buildout/i386/libjsoncpp.a
else
library = $(ENV_PATH_ROOT)/buildout/arm/libjsoncpp.a
endif

# This is the file name of the executables and path where 
# place the executables
buildout = main_process

# Including this file allows access to the Build System.
include $(ENV_PATH_ROOT)/build/buildrules.mk

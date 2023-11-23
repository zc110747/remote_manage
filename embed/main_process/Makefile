################################################################################
#
# THIS FILE IS BASIC MAKE FILE FOR
# Copyright (c) 2020-2021
# Original Author: 1107473010@qq.com
#
################################################################################

# This is object should be build and will link to generate
# the executables
# build executable, static library, dynamic library
BUILD =e

# wheather support mqtt service
DEFINE_MQTT =1

CCFLAGS := -O3 -std=c++17 -fno-strict-aliasing -lrt 
CCFLAGS += -DMODULE_DEFINE_MQTT=${DEFINE_MQTT}

cpp_objects = 	main.o \
				remote/tcp_thread.o	\
				remote/protocol.o	\
				module/cmd_process.o  \
				driver/rtc.o \
				driver/key.o \
				driver/spi_icm.o \
				driver/device_base.o \
				driver/driver.o \
				center/internal_process.o \
				center/device_manage.o   \
				center/center_manage.o

INCLUDES = 	-I remote/ 				\
			-I driver/ 				\
			-I include/ 			\
			-I module/ 				\
			-I center/				\
			-I mqtt/
				
INCLUDES +=	-I $(ENV_PATH_ROOT)/thirdparts/asio/include/ \
			-I $(ENV_PATH_ROOT)/thirdparts/jsoncpp/include/ \
			-I $(ENV_PATH_ROOT)/thirdparts/mosquitto/include/ 

#根据编译版本增加不同库支持
ifeq ($(APP_CC_COMPILE),g++)
library = $(ENV_PATH_ROOT)/buildout/i386/libjsoncpp.a
ifeq (${DEFINE_MQTT},1)
cpp_objects += mqtt/mqtt_process.o
library += -lssl -lcrypto -lmosquitto -lmosquittopp
endif
else
library = $(ENV_PATH_ROOT)/buildout/arm/libjsoncpp.a
ifeq (${DEFINE_MQTT},1)
cpp_objects += mqtt/mqtt_process.o
library += -L ${ENV_PATH_ROOT}/rootfs/usr/lib -lssl -lcrypto -lmosquitto -lmosquittopp
endif
endif

# This is the file name of the executables and path where 
# place the executables
buildout = main_process

# Including this file allows access to the Build System.
include $(ENV_PATH_ROOT)/embed/common/common.mk
include $(ENV_PATH_ROOT)/build/buildrules.mk

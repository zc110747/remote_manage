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

cpp_objects =   main.o                  \
                remote/tcp_thread.o     \
                remote/protocol.o       \
                module/cmd_process.o    \
                module/device_process.o 

INCLUDES =  -I remote/                  \
            -I include/                 \
            -I module/                  \
            -I mqtt/
                
INCLUDES += -I $(ENV_PATH_ROOT)/thirdparts/asio/include/ \
            -I $(ENV_PATH_ROOT)/thirdparts/jsoncpp/include/ \
            -I $(ENV_PATH_ROOT)/thirdparts/mosquitto/include/ 

ifeq (${FIRMWARE_CURRENT_PLATFORMS},AARCH64)

#for aarch64, add library 
library = $(ENV_PATH_ROOT)/buildout/aarch64/libjsoncpp.a
ifeq (${DEFINE_MQTT},1)
cpp_objects += mqtt/mqtt_process.o
library += -L ${PROGRAM_INSTALL}/lib -lssl -lcrypto -lmosquitto -lmosquittopp
endif
else
#for arm, add library 
library = $(ENV_PATH_ROOT)/buildout/arm/libjsoncpp.a
ifeq (${DEFINE_MQTT},1)
cpp_objects += mqtt/mqtt_process.o
library += -L ${PROGRAM_INSTALL}/lib -lssl -lcrypto -lmosquitto -lmosquittopp
endif
endif

# This is the file name of the executables and path where 
# place the executables
buildout = main_process

# Including this file allows access to the Build System.
include $(ENV_PATH_ROOT)/embed/common/common.mk
include $(ENV_PATH_ROOT)/build/buildrules.mk

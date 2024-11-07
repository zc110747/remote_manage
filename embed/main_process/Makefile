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
                module/device_process.o \
                mb/port/portevent.o     \
                mb/port/portserial.o    \
                mb/port/porttcp.o       \
                mb/port/porttimer.o     \
                mb/mb_app.o
                
c_objects =     mb/ascii/mbascii.o          \
                mb/functions/mbfunccoils.o  \
                mb/functions/mbfuncdiag.o   \
                mb/functions/mbfuncdisc.o   \
                mb/functions/mbfuncholding.o \
                mb/functions/mbfuncinput.o  \
                mb/functions/mbfuncother.o  \
                mb/functions/mbutils.o      \
                mb/port/mbport.o            \
                mb/rtu/mbcrc.o              \
                mb/rtu/mbrtu.o              \
                mb/tcp/mbtcp.o              \
                mb/mb.o
                
INCLUDES =  -I remote/                  \
            -I include/                 \
            -I module/                  \
            -I mqtt/                    \
            -I mb/                      \
            -I mb/ascii/                \
            -I mb/include/              \
            -I mb/rtu/                  \
            -I mb/tcp/                  \
            -I mb/port/                 

INCLUDES += -I $(ENV_PATH_ROOT)/thirdparts/asio/include/ \
            -I $(ENV_PATH_ROOT)/thirdparts/jsoncpp/include/ \
            -I $(ENV_PATH_ROOT)/thirdparts/mosquitto/include/ 

library = $(SUPPORT_ENV_INSTALL)/lib/libjsoncpp.a

# support MQTT add
ifeq (${DEFINE_MQTT},1)
cpp_objects +=  mqtt/mqtt_process.o \
                mqtt/mqtt_protocol.o
library += -L ${SUPPORT_ENV_INSTALL}/lib -lssl -lcrypto -lmosquitto -lmosquittopp
endif


# This is the file name of the executables and path where 
# place the executables
buildout = main_process

# Including this file allows access to the Build System.
include $(ENV_PATH_ROOT)/embed/common/common.mk
include $(ENV_PATH_ROOT)/build/buildrules.mk

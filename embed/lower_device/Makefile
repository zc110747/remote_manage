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

cpp_objects =   main.o    \
                serial.o \
                asio_client.o \
                module/cmd_process.o  \
                

INCLUDES =  -I include/ \
            -I module/
                
INCLUDES += -I $(ENV_PATH_ROOT)/thirdparts/asio/include/ \
            -I $(ENV_PATH_ROOT)/thirdparts/jsoncpp/include/ 

library = $(SUPPORT_ENV_INSTALL)/lib/libjsoncpp.a

# This is the file name of the executables and path where 
# place the executables
buildout = lower_device

# Including this file allows access to the Build System.
include $(ENV_PATH_ROOT)/embed/common/common.mk
include $(ENV_PATH_ROOT)/build/buildrules.mk

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
			  	device_manage.o \
			  	driver/device_base.o \
			  	driver/driver.o \
				driver/key.o \
				driver/rtc.o \
				driver/spi_icm.o 
				
INCLUDES = 	-I include/ \
			-I driver/
				
INCLUDES +=	-I $(ENV_PATH_ROOT)/thirdparts/asio/include/ \
			-I $(ENV_PATH_ROOT)/thirdparts/jsoncpp/include/

ifeq ($(FIRMWARE_CURRENT_PLATFORMS),ARM)
library = $(ENV_PATH_ROOT)/buildout/arm/libjsoncpp.a
else
library = $(ENV_PATH_ROOT)/buildout/aarch64/libjsoncpp.a
endif

# This is the file name of the executables and path where 
# place the executables
buildout = local_device

# Including this file allows access to the Build System.
include $(ENV_PATH_ROOT)/embed/common/common.mk
include $(ENV_PATH_ROOT)/build/buildrules.mk

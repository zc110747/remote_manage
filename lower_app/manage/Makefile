################################################################################
#
# THIS FILE IS BASIC MAKE FILE FOR
# Copyright (c) 2020-2021
# Original Author: 1107473010@qq.com
#
################################################################################

# This is object should be build and will link to generate
# the executables
#HOST = arm-linux-gnueabihf-
#CPU = arm-
HOST=
CPU=x86-
CC = $(HOST)g++
CFLAGS := -O3 -std=c++17 -fno-strict-aliasing -lrt

objects = 	main.o \
			remote/SocketTcpThread.o 	 \
			remote/SocketUdpThread.o 	 \
			remote/UartThread.o 		 \
			application/DeviceManageThread.o   \
			application/SystemConfig.o \
			application/tools/CalcCrc16.o  \
			application/TimeManage.o \
			driver/rtc.o \
			driver/beep.o \
			driver/led.o \
			driver/spi_icm.o \
			driver/i2c_ap.o \
			driver/deviceBase.o \
			driver/driver.o \
			logger/logger.o \
			logger/cmdProcess.o 

include_path = -I remote/ 			\
				-I driver/ 			\
				-I lib/ 			\
				-I include/ 		\
				-I application/ 		\
				-I lib/jsoncpp/include  \
				-I logger/ 			\
				-I application/tools/ \
				-I event/
lib = lib/$(CPU)libjsoncpp.a

# This is the file name of the executables and path where 
# place the executables
executables = app_demo
executables_path = ../Executables

# Including this file allows access to the Build System.
include ../../Build/basic.mk

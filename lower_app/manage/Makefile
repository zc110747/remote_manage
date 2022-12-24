################################################################################
#
# THIS FILE IS BASIC MAKE FILE FOR
# Copyright (c) 2020-2021
# Original Author: 1107473010@qq.com
#
################################################################################

# This is object should be build and will link to generate
# the executables
HOST = arm-linux-gnueabihf-
CPU = arm-
#HOST =
#CPU = x86-
CC = $(HOST)g++
CFLAGS := -O3 -std=c++17 -fno-strict-aliasing -lrt 

objects = 	main.o \
			remote/SocketTcpThread.o 	 \
			remote/SocketUdpThread.o 	 \
			remote/UartThread.o 		 \
			module/DeviceManageThread.o   \
			module/SystemConfig.o \
			module/CalcCrc16.o  \
			module/TimeManage.o \
			module/FIFOManage.o \
			module/asio_server.o \
			module/cmdProcess.o  \
			driver/rtc.o \
			driver/beep.o \
			driver/led.o \
			driver/key.o \
			driver/spi_icm.o \
			driver/i2c_ap.o \
			driver/deviceBase.o \
			driver/driver.o \
			logger/logger.o \
			node_process/node_process.o


clear_dobject = *.d .*.o.d \
				module/*.d module/.*.o.d \
				driver/*.d driver/.*.o.d \
				event/*.d event/.*.o.d \
				lib/*.d lib/.*.o.d \
				logger/*.d logger/.*.o.d \
				remote/*.d remote/.*.o.d \
				node_process/*.d node_process/.*.o.

include_path = -I remote/ 			\
				-I driver/ 			\
				-I lib/ 			\
				-I include/ 		\
				-I module/ 		\
				-I lib/jsoncpp/include  \
				-I lib/asio/include		\
				-I logger/ 			\
				-I module/tools/ \
				-I event/		\
				-I node_process/		
				
lib = lib/$(CPU)libjsoncpp.a

# This is the file name of the executables and path where 
# place the executables
executables = app_demo
executables_path = ../Executables

# Including this file allows access to the Build System.
include ../../Build/basic.mk

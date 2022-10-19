#编译工具和编译选项
HOST = arm-linux-gnueabihf-
CPU = arm-
#HOST=
#CPU=x86-
CC = $(HOST)g++
CFLAGS  := -O3 -std=c++17 -lpthread -fno-strict-aliasing -lrt

#头文件路径
LOWERAPP_INC_ROOT = lib/jsoncpp/include

LIB = lib/$(CPU)libjsoncpp.a #链接的库

OBJS = 	main.o source/SystemConfig.o \
		source/ApplicationThread.o source/SocketTcpThread.o source/SocketUdpThread.o source/UartThread.o \
		source/GroupApp/CalcCrc16.o source/GroupApp/MqManage.o source/GroupApp/FIFOManage.o \
		source/GroupApp/MessageBase.o driver/rtc.o driver/beep.o driver/led.o driver/spi_icm.o driver/i2c_ap.o \
		driver/deviceBase.o driver/driver.o logger/logger.o logger/cmdProcess.o

APP = app_demo

LOWERAPP_INC= -I $(LOWERAPP_INC_ROOT)
INCLUDE_PATH = $(LOWERAPP_INC)

all : $(APP)

#生成.o文件 
%.o : %.cpp
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDE_PATH)

#生成文件
$(APP) : $(OBJS)
	$(CC) -o $(APP) $(OBJS) $(LIB) $(CFLAGS)
	rm -f $(OBJS)
	chmod 777 $(APP)

tags :
	ctags -R *

clean:
	rm -f $(OBJS) $(TARGET) $(APP)


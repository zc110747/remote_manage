#编译工具和编译选项
CC = arm-linux-gnueabihf-g++
CFLAGS  := -O3 -std=c++11 -lpthread

#头文件路径
LOWERAPP_INC_ROOT = lib/jsoncpp/include

OBJS = main.o source/ApplicationThread.o source/SocketThread.o source/UartThread.o source/UsrProtocol.o source/SystemConfig.o driver/Beep.o driver/Led.o 
LIB = lib/libjsoncpp.a #链接的库
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

tags :
	ctags -R *

clean:
	rm -f $(OBJS) $(TARGET) $(APP)


#编译工具和编译选项
CC = arm-linux-gnueabihf-g++
CFLAGS  := -O3 -std=c++11 -lpthread

#头文件路径
LOWERAPP_INC_ROOT = ../

OBJS = led_test.o 
APP = led_test

LOWERAPP_INC= -I $(LOWERAPP_INC_ROOT)
INCLUDE_PATH = $(LOWERAPP_INC)

all : $(APP)

#生成.o文件 
%.o : %.cpp
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDE_PATH)

#生成文件
$(APP): $(OBJS)
	$(CC) -o $(APP) $(OBJS) $(LIB) $(CFLAGS)
	rm -f $(OBJS)
tags :
	ctags -R *

clean:
	rm -f $(OBJS) $(APP) test_dest.xml

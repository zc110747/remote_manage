#编译工具和编译选项
CC = g++
CFLAGS  := -Wall -O3 -std=c++11

#头文件路径
JSONCPP_INC_ROOT = ../../jsoncpp/include

OBJS = json_reader_test.o
LIB = ../../libjsoncpp.a #链接的库
APP = json_test

JSONCPP_INC= -I $(JSONCPP_INC_ROOT)
INCLUDE_PATH = $(JSONCPP_INC)

all : $(APP)

#生成.o文件 
%.o : %.cpp
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDE_PATH)

#生成静态库
$(APP) : $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIB) -o $(APP)
	rm -f $(OBJS)

tags :
	ctags -R *

clean:
	rm -f $(OBJS) $(TARGET) $(APP)

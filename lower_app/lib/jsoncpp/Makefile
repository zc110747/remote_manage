#编译工具和编译选项
CC = g++
CFLAGS  := -Wall -O3 -std=c++11

#头文件路径
JSONCPP_INC_ROOT = include/

OBJS = json_reader.o json_value.o json_writer.o
LIB = libjsoncpp.a # 目标文件名 

JSONCPP_INC= -I $(JSONCPP_INC_ROOT)

INCLUDE_PATH = $(JSONCPP_INC)

all : $(LIB)

#生成.o文件 
%.o : %.cpp
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDE_PATH)

#生成静态库
$(LIB) : $(OBJS)
	rm -f $@
	ar cr $@ $(OBJS)
	rm -f $(OBJS)
	cp $(LIB) ../$(LIB)

tags :
	ctags -R *

clean:
	rm -f $(OBJS) $(TARGET) $(LIB)

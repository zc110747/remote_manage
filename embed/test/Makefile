################################################################################
#
# THIS FILE IS BASIC MAKE FILE FOR
# Copyright (c) 2020-2026
# Original Author: 公众号 <嵌入式技术总结>
#
# 服务器启动: gdbserver :1234 target
################################################################################

### 目标文件、源文件和编译控制
TARGET = target
OBJECTS = main.o
DEBUG = 1
STD_VERSION = -std=c99
CROSS_COMPILE = arm-none-linux-gnueabihf-

# 编译选项
CC = $(CROSS_COMPILE)gcc
LDFLAGS = -lm -lpthread
CFLAGS = -Wall -Werror $(STD_VERSION) $(INCLUDE_PATH)
ifeq ($(DEBUG), 1)
	CFLAGS += -g -o0
else
	CFLAGS += -O2
endif

# 编译和链接
all: $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

clean:
	rm -rf $(TARGET) $(OBJECTS)

.PHONY: all clean

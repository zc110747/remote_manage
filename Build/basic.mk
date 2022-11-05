################################################################################
#
# THIS FILE IS BASIC MAKE FILE FOR
# Copyright (c) 2020-2021
# Original Author: 1107473010@qq.com
#
################################################################################

# this place define the compile and flags, CC complie should be g++ and 
# option could be add in place
CC ?= g++
CFLAGS  += -lpthread -lm

# this is the rule how to complie file with type .cpp to middle file .o 
# then link the object and generate the executables.
all : $(executables)

%.o : %.cpp
	$(CC) $(CFLAGS) -c $< -o $@ $(include_path)

$(executables): $(objects)
	$(CC) -o $(executables) $(objects) $(lib) $(CFLAGS)
	rm -f $(objects)
	$(shell if [ -d $(executables_path) ]; then echo; else mkdir $(executables_path); fi)
	mv $(executables) $(executables_path)
tags :
	ctags -R *

# this is the rule how to clean all the file
clean:
	rm $(executables_path)/$(executables)

# this tags let the make can execute the executabls.
# command: make execute option="-h"
run:
	@make
	@echo ---- run complier executables ----
	@$(executables_path)/$(executables) $(option)

stop:
	@pkill -9 $(executables)

build:
	@make
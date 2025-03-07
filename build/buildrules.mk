################################################################################
#
# THIS FILE IS BASIC MAKE FILE FOR
# Copyright (c) 2020-2023
# Original Author: 1107473010@qq.com
#
################################################################################

#-------------------------------------------------------------------------------
#support c/c++ compile.
#build excutatble, static library or dynamic library.
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#							Build Config Region 
#-------------------------------------------------------------------------------

###buildout file name
buildout ?=

###build mode
#e: excutable
#s: static-library
#
BUILD ?=e

###compile tools
C_COMPILE ?= $(APP_C_COMPILE)
CC_COMPILE ?= $(APP_CC_COMPILE)
AR_COMPILE ?= $(APP_AR_COMPILE)

CFLAGS  += -lpthread -lm
CCFLAGS  += -lpthread -lm

###include Path
INCLUDES ?=

###include library
library ?= 

###c/cpp objects for build
object ?=
c_objects ?=
cpp_objects ?=

objects+=${c_objects}
objects+=${cpp_objects}

run_dist ?=$(BUILD_NFS_PATH)/home/sys/executable

###library for store the buildout
ifeq ($(FIRMWARE_CURRENT_PLATFORMS),ARM)
lib_dist ?= $(ENV_PATH_ROOT)/buildout/arm
else
lib_dist ?= $(ENV_PATH_ROOT)/buildout/aarch64
endif

# build information
ifeq ($(BUILD),e) 
$(info build excutable file)
endif
ifeq ($(BUILD),s) 
$(info build static library)
endif
ifeq ($(BUILD),d) 
CCFLAGS += -fPIC
$(info build dynamic library)
endif
$(info used c compiler:$(C_COMPILE), c++ compiler:$(CC_COMPILE))
$(info AR_COMPILE:$(AR_COMPILE))
#-------------------------------------------------------------------------------
#						End of Build Config Region 
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#						Build Rules Region 
#-------------------------------------------------------------------------------
%.o : %.cpp
	$(CC_COMPILE) $(CCFLAGS) -c $< -o $@ $(INCLUDES)

%.o : %.c
	$(C_COMPILE) $(CFLAGS) -c $< -o $@ $(INCLUDES)

all : $(buildout)

#build excutable file
ifeq ($(BUILD),e) 
$(buildout): $(objects)

ifeq ("${cpp_objects}", "")
	$(C_COMPILE) -o $(buildout) $(objects) $(library) $(CFLAGS)
else
	$(CC_COMPILE) -o $(buildout) $(objects) $(library) $(CCFLAGS) 
endif
	
	rm -rf $(objects)

	if [ ! -d $(run_dist) ]; then \
		mkdir -m 777 -p $(run_dist); \
	fi
	
	mv $(buildout) $(run_dist)/
endif

#build static library
ifeq ($(BUILD),s) 
$(buildout): $(objects)
	$(AR_COMPILE) -cr $(buildout).a $(objects)

	rm -rf $(objects)
	mv $(buildout).a $(SUPPORT_ENV_INSTALL)/lib/
endif

#build dynamic library
ifeq ($(BUILD),d) 
$(buildout): $(objects)
	$(CC_COMPILE) $(objects) $(CCFLAGS) -shared -o lib$(buildout).so

	rm -rf $(objects)
	rm -f $(objects)
endif

clean:
	rm -f $(objects)
	rm -f *.a
	rm -f *.so

tags :
	ctags -R *
#-------------------------------------------------------------------------------
#						End of Build Rules Region 
#-------------------------------------------------------------------------------
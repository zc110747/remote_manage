dir ?= ${ENV_PATH_ROOT}
subdirs ?= $(wildcard $(dir)/*)  
with_kernel ?=1

modules:
	set -e; 
	for dir in ${subdirs}; do \
		if [ -d $${dir} ] && [ -f $${dir}/Makefile ]; then  \
			if [ ${with_kernel} -eq 0 ]; then \
				make -j4 ARCH=arm CROSS_COMPILE=${NEW_KERNEL_CC} -C $${dir}; \
			else \
				make clean -C $${dir}; \
				make -j4 -C $${dir}; \
			fi \
		fi \
	done

clean:
	set -e; 
	for dir in ${subdirs}; do \
		if [ -d $${dir} ] && [ -f $${dir}/Makefile ]; then  \
			make clean -C $${dir}; \
		fi \
	done	

all : modules

.PHONY : all

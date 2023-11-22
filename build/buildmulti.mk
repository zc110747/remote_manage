
dir ?= ${ENV_PATH_ROOT}
subdirs := $(wildcard $(dir)/*)  

modules:
	set -e; 
	for dir in ${subdirs}; do \
		if [ -d $${dir} ] && [ -f $${dir}/Makefile ]; then  \
			make -j4 -C $${dir}; \
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

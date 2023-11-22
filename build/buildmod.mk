KERNELDIR := $(shell printenv ENV_KERNEL_DIR)
CURRENT_PATH := $(shell pwd)

build: kernel_modules
	mv *.ko ${APP_BUILD_DRIVER}/
	make clean

kernel_modules:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean

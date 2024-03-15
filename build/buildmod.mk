KERNELDIR := $(shell printenv SUPPORT_ENV_KERNEL_DIR)
CURRENT_PATH := $(shell pwd)

build: kernel_modules
	mkdir -m 755 -p ${APPLICATION_EMBED_MODUILES}/
	mv *.ko ${APPLICATION_EMBED_MODUILES}/
	make clean

kernel_modules:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean

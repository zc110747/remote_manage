KERNELDIR := $(shell printenv SUPPORT_ENV_KERNEL_DIR)
CURRENT_PATH := $(shell pwd)

ifeq ($(FIRMWARE_CURRENT_PLATFORMS),ARM)
ARCH=arm
else
ARCH=arm64
endif
CROSS_COMPILE=${NEW_KERNEL_CC}

build: kernel_modules
	mkdir -m 755 -p ${APPLICATION_EMBED_MODUILES}/
	mv *.ko ${APPLICATION_EMBED_MODUILES}/
ifeq ($(FIRMWARE_CLEAN),1)
	make clean
endif

kernel_modules:
	echo "platform:${ARCH}, COMPILE:${CROSS_COMPILE}"
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) modules

clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean

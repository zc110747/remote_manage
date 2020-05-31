KERNELDIR := /usr/code/linux
CURRENT_PATH := $(shell pwd)
obj-m := kernal_led.o

build: kernel_modules

kernel_modules:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) modules
clean:
	$(MAKE) -C $(KERNELDIR) M=$(CURRENT_PATH) clean

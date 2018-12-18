obj-m := kgex.o kgex_napi.o kgex-1.1.o
kgex-objs := kgex-main.o
kgex_napi-objs := kgex-main-napi.o
kgex-1.1-objs :=kgex-main-1.1.o

KDIR := $(EXYNOS_KERNEL_PATH)

PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean

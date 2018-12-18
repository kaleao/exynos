obj-m := kmax_pmu.o
KDIR := ../../system/exynos7420_kernel_4.4/
PWD := $(shell pwd)

default:
	$(MAKE) ARCH=arm64 -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean

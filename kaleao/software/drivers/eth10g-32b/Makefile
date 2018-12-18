obj-m := eth10g.o eth10gtest.o
eth10g-objs := eth10g-main.o eth10g-ethtool.o eth10g-proc.o
eth10gtest-objs := eth10g_test.o

KDIR := $(EXYNOS_KERNEL_PATH)

PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean

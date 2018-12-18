# obj-m := eth10g.o eth10g-1.1.o
obj-m := eth10g.o

eth10g-objs := eth10g_main.o eth10g_proc.o eth10g_ethtool.o
# eth10g-1.1-objs := eth10g_main_v1.1.o eth10g_proc.o eth10g_ethtool.o

KDIR := $(EXYNOS_KERNEL_PATH)

PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean

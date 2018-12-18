obj-m := kmax_pci.o

kmax_pci-objs := pci_enable.o

KDIR := ../../exynos7420_kernel_4.4

PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean

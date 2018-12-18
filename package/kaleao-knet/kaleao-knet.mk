KALEAO_KNET_VERSION = v0.1
KALEAO_KNET_STAGING = YES
KALEAO_KNET_SITE = 
KALEAO_KNET_SOURCE = 
KALEAO_KNET_INSTALL_TARGET = YES
DRIVER_ARCH = arm
DRIVER_DIR = $(TARGET_DIR)/root/drivers
TESTING_DRIVERS_DIR = $(TARGET_DIR)/root/testing/drivers/
DELETE = rm -rf 
BUILD_FOLDER_KNET = output/build/kaleao-knet-v0.1
BUILD_FOLDER_LINUX = output/build/linux*
KALEAO_KNET_DEPENDENCIES = linux

define KALEAO_KNET_BUILD_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) CC=$(TARGET_CC) LD=$(TARGET_LD) -C $(TOPDIR)/kaleao/drivers/knet
endef


define KALEAO_KNET_INSTALL_TARGET_CMDS
	$(INSTALL) -m 0755 -D kaleao/drivers/knet/kmax_pci.ko $(DRIVER_DIR)
	$(INSTALL) -m 0755 -D kaleao/drivers/knet/knet.ko $(DRIVER_DIR)
	$(INSTALL) -m 0755 -D kaleao/drivers/knet/kgex.ko $(DRIVER_DIR)

endef

kaleao-kmax-clean:
	$(DELETE) $(BUILD_FOLDER_KNET)
	$(TARGET_MAKE_ENV) $(MAKE) CC=$(TARGET_CC) LD=$(TARGET_LD) -C $(TOPDIR)/kaleao/drivers/knet clean
	$(DELETE) $(BUILD_FOLDER_LINUX)

$(eval $(generic-package))
$(eval $(host-generic-package))

KALEAO_UFS_VERSION = v0.1
KALEAO_UFS_STAGING = YES
KALEAO_UFS_SITE = 
KALEAO_UFS_SOURCE = 
KALEAO_UFS_INSTALL_TARGET = YES
DELETE = rm -rf 
BUILD_FOLDER_UFS = output/build/kaleao-UFS-v0.1
BUILD_FOLDER_LINUX = output/build/linux*
KALEAO_UFS_DEPENDENCIES = linux

define KALEAO_UFS_BUILD_CMDS
endef


define KALEAO_UFS_INSTALL_TARGET_CMDS
	mkdir -p $(TARGET_DIR)/ufs
	echo MALAKAKLAJSKANFDJANBDFISHFASNFDSANFSAFAS
	cp output/images/Image $(TARGET_DIR)/ufs/Image_noramfs
	cp output/images/set.bin.reva $(TARGET_DIR)/ufs
	cp kaleao/system/u-boot/kaleao/output/revb.set.bin $(TARGET_DIR)/ufs/set.bin.revb
	cp kaleao/system/u-boot/kaleao/output/revb-unsec.set.bin $(TARGET_DIR)/ufs/set.bin.revb-unsec
	cp output/images/kmax-revb-ufs.dtb $(TARGET_DIR)/ufs
	cp board/kaleao/write_ufs.sh $(TARGET_DIR)/ufs
	cp board/kaleao/write_ufs_128GB.sh $(TARGET_DIR)/ufs
	cp board/kaleao/write_ufs_unsecure.sh $(TARGET_DIR)/ufs
	cp board/kaleao/write_ufs_unsecure_128GB.sh $(TARGET_DIR)/ufs
	cp board/kaleao/rootfs.tar.gz $(TARGET_DIR)/ufs

endef

kaleao-ufs-clean:


$(eval $(generic-package))
$(eval $(host-generic-package))

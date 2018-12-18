TEST_VERSION = v0.1
TEST_INSTALL_STAGING = YES
TEST_SITE = 
TEST_SOURCE = 
TEST_INSTALL_TARGET = YES

define TEST_INSTALL_TARGET_CMDS
        $(INSTALL) -m 0755 -D package/test/testInit $(TARGET_DIR)/etc/init.d/testInit
endef

$(eval $(generic-package))
$(eval $(host-generic-package))

#############################################################
#		kmax Appliance
# 
#
#############################################################


KMAX_APPLIANCE_SITE = git@bitbucket.org:kaleao/microvisor.git
KMAX_APPLIANCE_SITE_METHOD = git
KMAX_APPLIANCE_INSTALL_STAGING=
KMAX_APPLIANCE_INSTALL_TARGET = YES

export BUILDROOT_ROOTDIR=$(TOPDIR)

define KMAX_APPLIANCE_BUILD_CMDS

endef

define KMAX_APPLIANCE_INSTALL_STAGING_CMDS

endef

define KMAX_APPLIANCE_INSTALL_TARGET_CMDS

endef


$(eval $(generic-package))
$(eval $(host-generic-package))


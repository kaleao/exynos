#############################################################
#	EXYNOSAGENT
# 
#
#############################################################



EXYNOS_AGENT_VERSION = master
EXYNOS_AGENT_PLATFORM=exynos
EXYNOS_AGENT_SYSTEM=exynosAgent
EXYNOS_AGENT_BUILD=debug

EXYNOS_AGENT_BUILDDIR=build/$(EXYNOS_AGENT_SYSTEM)/$(EXYNOS_AGENT_PLATFORM)/$(EXYNOS_AGENT_BUILD)
EXYNOS_AGENT_EXE=$(EXYNOS_AGENT_SYSTEM)_$(EXYNOS_AGENT_PLATFORM)


#work around build root defenitions
EXYNOS_AGENT_SITE = git@bitbucket.org:kaleao/applications.git
EXYNOS_AGENT_SITE_METHOD = git
EXYNOS_AGENT_INSTALL_STAGING=
EXYNOS_AGENT_INSTALL_TARGET = YES

export BUILDROOT_ROOTDIR=$(TOPDIR)

define EXYNOS_AGENT_BUILD_CMDS
	mkdir -p  ./output/host/opt/
	rm -rf  ./output/host/opt/ext-toolchain
        tar xf  ./output/build/kaleao_exynosAgent-master/tools/toolchains/arm64/linux/gcc-linaro-5.3.1-2016.05-x86_64_aarch64-linux-gnu.tar.xz -C ./output/host/opt/
        mv ./output/host/opt/gcc-linaro-5.3.1-2016.05-x86_64_aarch64-linux-gnu ./output/host/opt/ext-toolchain
	$(TARGET_MAKE_ENV) $(MAKE)  KALEAO_SYSTEM=$(EXYNOS_AGENT_SYSTEM) KALEAO_PLAT=$(EXYNOS_AGENT_PLATFORM) BUILD_TYPE=$(EXYNOS_AGENT_BUILD) -C $(@D)
endef

define EXYNOS_AGENT_INSTALL_STAGING_CMDS
	 $(INSTALL) -D -m 0755 $(@D)/$(EXYNOS_AGENT_BUILDDIR)/$(EXYNOS_AGENT_EXE) ./kaleao/system/misc/util/$(EXYNOS_AGENT_EXE)
endef

define EXYNOS_AGENT_INSTALL_TARGET_CMDS
	 $(INSTALL) -D -m 0755 $(@D)/$(EXYNOS_AGENT_BUILDDIR)/$(EXYNOS_AGENT_EXE) $(TARGET_DIR)/root/util/$(EXYNOS_AGENT_EXE)
	 $(INSTALL) -D -m 0755 $(@D)/$(EXYNOS_AGENT_BUILDDIR)/$(EXYNOS_AGENT_EXE) ./kaleao/system/misc/util/$(EXYNOS_AGENT_EXE)
endef

kaleao-$(EXYNOS_AGENT_SYSTEM)-clean:
	$(TARGET_MAKE_ENV) $(MAKE)  KALEAO_SYSTEM=$(EXYNOS_AGENT_SYSTEM) KALEAO_PLAT=$(EXYNOS_AGENT_PLATFORM) BUILD_TYPE=clean -C $($(EXYNOS_AGENT_SYSTEM)_SITE)

$(eval $(generic-package))
$(eval $(host-generic-package))



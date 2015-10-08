include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=xtstring
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define KernelPackage/xtstring
  SUBMENU:=Other modules
  TITLE:=example driver
  DEPENDS:=@LINUX_3_3
  FILES:=$(PKG_BUILD_DIR)/*.$(LINUX_KMOD_SUFFIX)
  KCONFIG:=
endef

define KernelPackage/xtstring/description
  Kernel module to xtstring
endef

EXTRA_KCONFIG:= \
	CONFIG_WHOVISIT=m

EXTRA_CFLAGS:= \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=m,%,$(filter %=m,$(EXTRA_KCONFIG)))) \
	$(patsubst CONFIG_%, -DCONFIG_%=1, $(patsubst %=y,%,$(filter %=y,$(EXTRA_KCONFIG)))) \

MAKE_OPTS:= \
	ARCH="$(LINUX_KARCH)" \
	CROSS_COMPILE="$(TARGET_CROSS)" \
	SUBDIRS="$(PKG_BUILD_DIR)" \
	EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
	$(EXTRA_KCONFIG)

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Build/Compile
	$(MAKE) -C "$(LINUX_DIR)" \
		$(MAKE_OPTS) \
		modules
endef

$(eval $(call KernelPackage,xtstring))


include $(TOPDIR)/rules.mk

PKG_NAME:=libdomlog
PKG_RELEASE:=1
PKG_VERSION:=1.0.0
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/libdomlog
	SECTION:=libs
	CATEGORY:=Libraries
	TITLE:=libdomlog
	DEPENDS:=+libuci
endef

define Package/libdomlog/description
	Custom logger
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib $(1)/usr/include/
	$(CP) $(PKG_BUILD_DIR)/*.so* $(1)/usr/lib/
	$(CP) $(PKG_BUILD_DIR)/*.h $(1)/usr/include/
endef

define Package/libdomlog/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(INSTALL_DIR) $(1)/etc/config
	$(CP) $(PKG_BUILD_DIR)/*.so* $(1)/usr/lib/
	$(INSTALL_BIN) ./files/domlog $(1)/etc/config/domlog
endef

$(eval $(call BuildPackage,libdomlog))
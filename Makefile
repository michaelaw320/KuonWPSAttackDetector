include $(TOPDIR)/rules.mk

PKG_NAME:=KuonWPSAttackDetector
PKG_RELEASE:=1

PKG_BUILD_BIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/KuonWPSAttackDetector
	SECTION:=network
	CATEGORY:=IDS
	DEPENDS:=+libpcap +screen +mailsend
	TITLE:=Kuon WPS Attack Detector
endef

define Package/KuonWPSAttackDetector/description
	WPS Attack detector and notifier
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

define Package/KuonWPSAttackDetector/install
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/root
	$(INSTALL_BIN) ./bin/KuonDetectorMain $(1)/bin/
	$(INSTALL_BIN) ./bin/KuonMailer $(1)/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/KuonPacketCapture $(1)/bin/
	$(INSTALL_BIN) ./bin/KuonSwitcher $(1)/bin/
	$(INSTALL_BIN) ./init/KuonWPSAttackDetector $(1)/etc/init.d/
	$(INSTALL_BIN) ./config/KuonConfig $(1)/root/
	$(INSTALL_BIN) ./config/notificationMail.txt $(1)/root/
endef

$(eval $(call BuildPackage,KuonWPSAttackDetector))

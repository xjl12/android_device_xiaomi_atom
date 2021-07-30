#
# Copyright (C) 2020 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

# Inherit from those products. Most specific first.
$(call inherit-product, $(SRC_TARGET_DIR)/product/core_64_bit.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base_telephony.mk)

# Inherit from atom device
$(call inherit-product, device/xiaomi/atom/device.mk)

# Inherit some common lineage stuff.
$(call inherit-product, vendor/lineage/config/common_full_phone.mk)

# Device identifier. This must come after all inclusions.
PRODUCT_NAME := aosp_atom
PRODUCT_DEVICE := atom
PRODUCT_BRAND := Redmi
PRODUCT_MODEL := Redmi 10X
PRODUCT_MANUFACTURER := Xiaomi
DEVICE_MAINTAINER := Xayah,Raspberry-Monster,Laulan56,XJLe

# PRODUCT_CHARACTERISTICS := nosdcard

PRODUCT_BUILD_PROP_OVERRIDES += \
    PRIVATE_BUILD_DISC="atom-user 11 RP1A.200720.011 V12.5.2.0.RJHCNXM release-keys"

BUILD_FINGERPRINT := "Redmi/atom/atom:11/RP1A.200720.011/V12.5.2.0.RJHCNXM:user/release-keys"

PRODUCT_PROPERTY_OVERRIDES += \
    ro.build.fingerprint=$(BUILD_FINGERPRINT)

PRODUCT_GMS_CLIENTID_BASE := android-xiaomi

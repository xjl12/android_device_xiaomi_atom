#
# Copyright (C) 2020 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

# Inherit from those products. Most specific first.
$(call inherit-product, $(SRC_TARGET_DIR)/product/core_64_bit.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base_telephony.mk)

# Inherit from cezanne device
$(call inherit-product, device/xiaomi/cezanne/device.mk)

# Inherit some common stuff.
$(call inherit-product, vendor/dot/config/common.mk)

# Device identifier. This must come after all inclusions.
PRODUCT_NAME := dot_cezanne
PRODUCT_DEVICE := cezanne
PRODUCT_BRAND := Redmi
PRODUCT_MODEL := M2006J10C
PRODUCT_MANUFACTURER := Xiaomi
DEVICE_MAINTAINER := nzlov

PRODUCT_CHARACTERISTICS := nosdcard

BUILD_FINGERPRINT := "Redmi/cezanne/cezanne:11/RP1A.200720.011/21.4.7:user/release-keys"

PRODUCT_GMS_CLIENTID_BASE := android-xiaomi

EXTRA_FOD_ANIMATIONS := true

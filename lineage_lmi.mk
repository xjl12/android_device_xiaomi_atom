#
# Copyright (C) 2020 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

# Inherit from those products. Most specific first.
$(call inherit-product, $(SRC_TARGET_DIR)/product/core_64_bit.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base_telephony.mk)

# Inherit from lmi device
$(call inherit-product, device/xiaomi/lmi/device.mk)

# Inherit some common Lineage stuff.
$(call inherit-product, vendor/lineage/config/common_full_phone.mk)

# Device identifier. This must come after all inclusions.
PRODUCT_NAME := lineage_lmi
PRODUCT_DEVICE := lmi
PRODUCT_BRAND := Xiaomi
PRODUCT_MODEL := Redmi K30 Pro
PRODUCT_MANUFACTURER := Xiaomi

PRODUCT_CHARACTERISTICS := nosdcard

BUILD_FINGERPRINT := "Redmi/lmi/lmi:10/QKQ1.191117.002/V12.0.8.0.QJKCNXM:user/release-keys"

PRODUCT_GMS_CLIENTID_BASE := android-xiaomi

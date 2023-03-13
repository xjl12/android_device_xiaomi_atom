#!/bin/sh
PATCH_LOC=$PWD/device/xiaomi/atom/patch
cd device/custom/sepolicy
git am $PATCH_LOC/device/custom/sepolicy/0001-Fix-up-kernel-failure.patch
cd ../../../framework/base
git am $PATCH_LOC/framework/base/0002-PE-Fingerprint-fix.patch
git am $PATCH_LOC/framework/base/0003-Mediatek-IMS-Fail.patch
git am $PATCH_LOC/framework/base/0004-UDFPS-better-user-experience-author-xjl12.patch
git am $PATCH_LOC/framework/base/0005-UdfpsView-patch-by-xjl12.patch
cd ../native
git am $PATCH_LOC/framework/native/0001-Fix-vibration.patch
cd ../../packages/modules/Bluetooth
git am $PATCH_LOC/packages/modules/Bluetooth/0003-audio_hal_interface-Optionally-use-sysbta-HAL.patch
cd ../../../system/sepolicy
git am $PATCH_LOC/system/sepolicy/0001-Fix-sepolicy-conflict.patch
cd ../../hardware/interfaces
git am $PATCH_LOC/hardware/interfaces/0001-Add-show-hide-udfps-view-callbacks-to-IBiometricsFin.patch

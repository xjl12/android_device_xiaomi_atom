#!/bin/sh
PATCH_LOC=$PWD/device/xiaomi/atom/patch
cd device/custom/sepolicy
git am $PATCH_LOC/0001-Fix-up-kernel-failure.patch
cd ../../../framework/base
git am $PATCH_LOC/0001-Fix-up-PixelExperience-permission.patch
git am $PATCH_LOC/0002-PE-Fingerprint-fix.patch
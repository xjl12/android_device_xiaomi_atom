#
# Copyright (C) 2017-2018 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
    $(call all-java-files-under, src)

LOCAL_MODULE := org.ifaa.android.manager
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := JAVA_LIBRARIES

include $(BUILD_JAVA_LIBRARY)

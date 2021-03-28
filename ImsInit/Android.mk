LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional

LOCAL_SRC_FILES := $(call all-java-files-under, src)
LOCAL_PACKAGE_NAME := ImsInit
LOCAL_PRIVATE_PLATFORM_APIS := true
LOCAL_CERTIFICATE := platform
LOCAL_PRIVILEGED_MODULE := true
LOCAL_VENDOR_MODULE := false

LOCAL_USE_AAPT2 := true

LOCAL_JAVA_LIBRARIES := \
    ims-common

include $(BUILD_PACKAGE)

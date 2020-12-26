LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := android.hardware.power-service.lmi
LOCAL_MODULE_TAGS := optional

LOCAL_MODULE_PATH := $(TARGET_OUT_PRODUCT)/vendor_overlay/$(PRODUCT_TARGET_VNDK_VERSION)/bin
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_MODULE_STEM := android.hardware.power-service

LOCAL_REQUIRED_MODULES := android.hardware.power-service.rc

LOCAL_SHARED_LIBRARIES := \
    liblog \
    libcutils \
    libdl \
    libxml2 \
    libbase \
    libutils \
    android.hardware.power-ndk_platform \
    libbinder_ndk

LOCAL_HEADER_LIBRARIES += libutils_headers libhardware_headers

LOCAL_SRC_FILES := \
    power-common.c \
    metadata-parser.c \
    utils.c \
    list.c \
    hint-data.c \
    powerhintparser.c \
    Power.cpp \
    main.cpp

LOCAL_C_INCLUDES := \
    external/libxml2/include \
    external/icu/icu4c/source/common

LOCAL_CFLAGS += -Wno-unused-parameter -Wno-unused-variable

ifeq ($(TARGET_USES_INTERACTION_BOOST),true)
    LOCAL_CFLAGS += -DINTERACTION_BOOST
endif

include $(BUILD_EXECUTABLE)

include $(CLEAR_VARS)

LOCAL_MODULE       := android.hardware.power-service.rc
LOCAL_MODULE_TAGS  := optional
LOCAL_MODULE_CLASS := ETC

LOCAL_MODULE_PATH  := $(TARGET_OUT_PRODUCT)/vendor_overlay/$(PRODUCT_TARGET_VNDK_VERSION)/etc/init
LOCAL_MODULE_STEM  := android.hardware.power-service.rc

LOCAL_SRC_FILES    := android.hardware.power-service.rc

include $(BUILD_PREBUILT)

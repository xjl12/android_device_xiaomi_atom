ifeq ($(BOARD_HAVE_BLUETOOTH), true)
LOCAL_PATH := $(call my-dir)

###########################################################################
# MTK BT CHIP INIT LIBRARY FOR BLUEDROID
###########################################################################
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	mtk.c \
	radiomgr.c \
	radiomod.c

LOCAL_SHARED_LIBRARIES := \
	liblog \
	libcutils \
	libnvram

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/include \
	system/bt/hci/include

ifeq ($(TARGET_BUILD_VARIANT), eng)
LOCAL_CFLAGS += -DBD_ADDR_AUTOGEN
endif

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libbluetooth_mtk
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)

###########################################################################
# MTK BT VENDOR DRIVER FOR BLUEDROID
###########################################################################
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	bt_drv.c

LOCAL_SHARED_LIBRARIES := \
	liblog \
	libbluetooth_mtk

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/include \
	system/bt/hci/include

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libbt-vendor
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_OWNER := mtk
LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)
endif

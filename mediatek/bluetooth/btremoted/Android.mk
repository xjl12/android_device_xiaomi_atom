#
#  Copyright (C) 2014 Amazon.com Inc. or its affiliates.  All rights reserved.
#

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

#btremoted-def := -D_METRICS

LOCAL_SRC_FILES:=   \
        btremoted.c \
        msbc.c      \
        sbcplc.c    \

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE:= btremoted

LOCAL_SHARED_LIBRARIES += libcutils liblog

LOCAL_CFLAGS := $(btremoted-def)

include $(BUILD_EXECUTABLE)

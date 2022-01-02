# Copyright 2010 The Android Open Source Project

LOCAL_PATH:= $(call my-dir)

#
# -- All host/targets excluding windows
#

include $(CLEAR_VARS)
LOCAL_MODULE := mke2fs.conf
LOCAL_SRC_FILES := $(LOCAL_MODULE)
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_IS_HOST_MODULE := true
include $(BUILD_PREBUILT)

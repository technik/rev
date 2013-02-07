#-----------------------------------------------------------
# Rev project
# Created by Carmelo J. Fdez-Agüera Tortos (a.k.a. Technik)
# On February 7th, 2013
#-----------------------------------------------------------
# Unit test android.mk makefile

ROOT_DIR := $(call my-dir)/../../..
CODE_DIR := $(ROOT_DIR)/src/code

LOCAL_PATH := $(CODE_DIR)

include $(CLEAR_VARS)

LOCAL_MODULE := testLog
LOCAL_SRC_FILES := main.cpp

LOCAL_LDLIBS    := -llog -landroid
LOCAL_STATIC_LIBRARIES := android_native_app_glue

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
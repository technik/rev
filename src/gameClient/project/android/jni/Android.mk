################################################################################
# Android makefile for rev gameClient
# Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
# on September 8th, 2011
################################################################################

# Local directory, root for searching sources
GC_ROOT_DIR:=../../..
GC_CODE_DIR:=$(GC_ROOT_DIR)/code

LOCAL_PATH:=$(call my-dir)/$(GC_CODE_DIR)

include $(CLEAR_VARS)

LOCAL_MODULE:=nativeGameClient

#-------------------------------------------------------------------------------
# Engine files
#-------------------------------------------------------------------------------
REV_CODE_DIR:=../../engine/code

# Rev Core
REVCORE_DIR:=$(REV_CODE_DIR)/revCore
REVCORE_SRC:=$(REVCORE_DIR)/codeTools/log/log.cpp \
             $(REVCORE_DIR)/component/component.cpp\
             $(REVCORE_DIR)/entity/entity.cpp\
             $(REVCORE_DIR)/entity/entityManager.cpp\
			 $(REVCORE_DIR)/file/file.cpp\
             $(REVCORE_DIR)/time/time.cpp

REVGAME_DIR:=$(REV_CODE_DIR)/revGame
REVGAME_SRC:=$(REVGAME_DIR)/gameClient/gameClient.cpp

REVVIDEO_DIR:=$(REV_CODE_DIR)/revVideo
REVVIDEO_SRC:=$(REVVIDEO_DIR)/video.cpp\
			  $(REVVIDEO_DIR)/color/color.cpp\
			  $(REVVIDEO_DIR)/renderer/directRenderer/directRenderer.cpp\
			  $(REVVIDEO_DIR)/videoDriver/videoDriver.cpp\
			  $(REVVIDEO_DIR)/videoDriver/android/videoDriverAndroid.cpp

REV_SRC_FILES:=$(REVCORE_SRC) $(REVGAME_SRC) $(REVVIDEO_SRC)

LOCAL_SRC_FILES:=android/main.cpp $(REV_SRC_FILES)

LOCAL_CFLAGS:=-D_DEBUG -Wall -Werror -Wextra -iquote $(LOCAL_PATH)/$(GC_CODE_DIR) -I$(LOCAL_PATH)/$(REV_CODE_DIR)

LOCAL_LDLIBS    := -lGLESv2

include $(BUILD_SHARED_LIBRARY)

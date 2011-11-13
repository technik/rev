################################################################################
# Android makefile for rev gameClient
# Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
# on September 8th, 2011
################################################################################

# Local directory, root for searching sources
GC_ROOT_DIR:=../../../..
GC_CODE_DIR:=$(GC_ROOT_DIR)/src/code
REV_CODE_DIR:=../../../../engine/src/code

LOCAL_PATH:=$(call my-dir)/$(GC_CODE_DIR)

# ----------------- zip -----------------------

include $(CLEAR_VARS)

LIBZIP_DIR:=$(REV_CODE_DIR)/libs/android/libzip

LOCAL_MODULE    := libzip
LOCAL_SRC_FILES :=\
	$(LIBZIP_DIR)/zip_add.c \
	$(LIBZIP_DIR)/zip_add_dir.c \
	$(LIBZIP_DIR)/zip_close.c \
	$(LIBZIP_DIR)/zip_delete.c \
	$(LIBZIP_DIR)/zip_dirent.c \
	$(LIBZIP_DIR)/zip_entry_free.c \
	$(LIBZIP_DIR)/zip_entry_new.c \
	$(LIBZIP_DIR)/zip_err_str.c \
	$(LIBZIP_DIR)/zip_error.c \
	$(LIBZIP_DIR)/zip_error_clear.c \
	$(LIBZIP_DIR)/zip_error_get.c \
	$(LIBZIP_DIR)/zip_error_get_sys_type.c \
	$(LIBZIP_DIR)/zip_error_strerror.c \
	$(LIBZIP_DIR)/zip_error_to_str.c \
	$(LIBZIP_DIR)/zip_fclose.c \
	$(LIBZIP_DIR)/zip_file_error_clear.c \
	$(LIBZIP_DIR)/zip_file_error_get.c \
	$(LIBZIP_DIR)/zip_file_get_offset.c \
	$(LIBZIP_DIR)/zip_file_strerror.c \
	$(LIBZIP_DIR)/zip_filerange_crc.c \
	$(LIBZIP_DIR)/zip_fopen.c \
	$(LIBZIP_DIR)/zip_fopen_index.c \
	$(LIBZIP_DIR)/zip_fread.c \
	$(LIBZIP_DIR)/zip_free.c \
	$(LIBZIP_DIR)/zip_get_archive_comment.c \
	$(LIBZIP_DIR)/zip_get_archive_flag.c \
	$(LIBZIP_DIR)/zip_get_file_comment.c \
	$(LIBZIP_DIR)/zip_get_num_files.c \
	$(LIBZIP_DIR)/zip_get_name.c \
	$(LIBZIP_DIR)/zip_memdup.c \
	$(LIBZIP_DIR)/zip_name_locate.c \
	$(LIBZIP_DIR)/zip_new.c \
	$(LIBZIP_DIR)/zip_open.c \
	$(LIBZIP_DIR)/zip_rename.c \
	$(LIBZIP_DIR)/zip_replace.c \
	$(LIBZIP_DIR)/zip_set_archive_comment.c \
	$(LIBZIP_DIR)/zip_set_archive_flag.c \
	$(LIBZIP_DIR)/zip_set_file_comment.c \
	$(LIBZIP_DIR)/zip_source_buffer.c \
	$(LIBZIP_DIR)/zip_source_file.c \
	$(LIBZIP_DIR)/zip_source_filep.c \
	$(LIBZIP_DIR)/zip_source_free.c \
	$(LIBZIP_DIR)/zip_source_function.c \
	$(LIBZIP_DIR)/zip_source_zip.c \
	$(LIBZIP_DIR)/zip_set_name.c \
	$(LIBZIP_DIR)/zip_stat.c \
	$(LIBZIP_DIR)/zip_stat_index.c \
	$(LIBZIP_DIR)/zip_stat_init.c \
	$(LIBZIP_DIR)/zip_strerror.c \
	$(LIBZIP_DIR)/zip_unchange.c \
	$(LIBZIP_DIR)/zip_unchange_all.c \
	$(LIBZIP_DIR)/zip_unchange_archive.c \
	$(LIBZIP_DIR)/zip_unchange_data.c

LOCAL_LDLIBS := -lz

include $(BUILD_STATIC_LIBRARY)

# ----------- End of zip -----------------------------
# ----------- Lib png --------------------------------

include $(CLEAR_VARS)

LOCAL_MODULE := libpng

LIBPNG_DIR := $(REV_CODE_DIR)/libs/android/libpng

LOCAL_SRC_FILES := \
	$(LIBPNG_DIR)/png.c \
	$(LIBPNG_DIR)/pngerror.c \
	$(LIBPNG_DIR)/pngget.c \
	$(LIBPNG_DIR)/pngmem.c \
	$(LIBPNG_DIR)/pngpread.c \
	$(LIBPNG_DIR)/pngread.c \
	$(LIBPNG_DIR)/pngrio.c \
	$(LIBPNG_DIR)/pngrtran.c \
	$(LIBPNG_DIR)/pngrutil.c \
	$(LIBPNG_DIR)/pngset.c \
	$(LIBPNG_DIR)/pngtrans.c \
	$(LIBPNG_DIR)/pngwio.c \
	$(LIBPNG_DIR)/pngwrite.c \
	$(LIBPNG_DIR)/pngwtran.c \
	$(LIBPNG_DIR)/pngwutil.c

LOCAL_LDLIBS := -lz

include $(BUILD_STATIC_LIBRARY)

# ----------- End of lib png -------------------------

include $(CLEAR_VARS)

LOCAL_MODULE:=nativeGameClient

#-------------------------------------------------------------------------------
# Engine files
#-------------------------------------------------------------------------------

# Rev Core
REVCORE_DIR:=$(REV_CODE_DIR)/revCore
REVCORE_SRC:=$(REVCORE_DIR)/codeTools/log/log.cpp \
			 $(REVCORE_DIR)/component/component.cpp\
			 $(REVCORE_DIR)/file/file.cpp\
			 $(REVCORE_DIR)/math/matrix.cpp\
			 $(REVCORE_DIR)/math/quaternion.cpp\
			 $(REVCORE_DIR)/math/vector.cpp\
			 $(REVCORE_DIR)/node/node.cpp\
			 $(REVCORE_DIR)/time/time.cpp\
			 $(REVCORE_DIR)/transform/transformSrc.cpp

REVGAME_DIR:=$(REV_CODE_DIR)/revGame
REVGAME_SRC:=$(REVGAME_DIR)/gameClient/gameClient.cpp\
			 $(REVGAME_DIR)/gui/guiElement.cpp\
			 $(REVGAME_DIR)/gui/panel.cpp

REVINPUT_DIR:=$(REV_CODE_DIR)/revInput
REVINPUT_SRC:=$(REVINPUT_DIR)/touchInput/touchInput.cpp\
			  $(REVINPUT_DIR)/touchInput/android/touchInputAndroid.cpp

REVVIDEO_DIR:=$(REV_CODE_DIR)/revVideo
REVVIDEO_SRC:=$(REVVIDEO_DIR)/video.cpp\
			  $(REVVIDEO_DIR)/camera/camera.cpp\
			  $(REVVIDEO_DIR)/camera/orthoCamera.cpp\
			  $(REVVIDEO_DIR)/color/color.cpp\
			  $(REVVIDEO_DIR)/material/basic/plainTextureMaterial.cpp\
			  $(REVVIDEO_DIR)/material/basic/raymarchingMaterial.cpp\
			  $(REVVIDEO_DIR)/material/basic/solidColorMaterial.cpp\
			  $(REVVIDEO_DIR)/renderer/directRenderer/directRenderer.cpp\
			  $(REVVIDEO_DIR)/scene/videoScene.cpp\
			  $(REVVIDEO_DIR)/scene/model/quad.cpp\
			  $(REVVIDEO_DIR)/scene/model/staticModel.cpp\
			  $(REVVIDEO_DIR)/scene/model/staticModelInstance.cpp\
			  $(REVVIDEO_DIR)/scene/renderableInstance.cpp\
			  $(REVVIDEO_DIR)/texture/texture.cpp\
			  $(REVVIDEO_DIR)/texture/android/androidpngutils.cpp\
			  $(REVVIDEO_DIR)/videoDriver/shader/pxlShader.cpp\
			  $(REVVIDEO_DIR)/videoDriver/shader/shader.cpp\
			  $(REVVIDEO_DIR)/videoDriver/shader/vtxShader.cpp\
			  $(REVVIDEO_DIR)/videoDriver/android/videoDriverAndroid.cpp\
			  $(REVVIDEO_DIR)/viewport/viewport.cpp

REV_SRC_FILES:=$(REVCORE_SRC) $(REVGAME_SRC) $(REVINPUT_SRC) $(REVVIDEO_SRC)

# - Build the engine library

LOCAL_SRC_FILES:=android/main.cpp $(REV_SRC_FILES)

LOCAL_CFLAGS:=-D_DEBUG -Wall -Werror -Wextra -iquote $(LOCAL_PATH) -I$(LOCAL_PATH)/$(REV_CODE_DIR) -I$(LOCAL_PATH)/$(LIBZIP_DIR)

LOCAL_LDLIBS    := -lGLESv2 -lz -llog

LOCAL_STATIC_LIBRARIES:=libzip libpng

include $(BUILD_SHARED_LIBRARY)

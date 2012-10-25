#-----------------------------------------------------------------------------------------------------------------------
# Revolution SDK
# Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
# On October 8th, 2012
#-----------------------------------------------------------------------------------------------------------------------
# Core module, make file

# Project configuration variables
MODULE_NAME := revvideo

# -- Folder structure --
ROOT_DIR := ../../..
SRC_DIR := $(ROOT_DIR)/src
CODE_DIR := $(SRC_DIR)/code
OUT_DIR := $(ROOT_DIR)/bin/linux

include $(REVSDK_HOME)/common/linux/make/linux-common.mk
INCLUDE_FLAGS := $(INCLUDE_FLAGS) -I$(REVSDK_HOME)/modules/baseLevel/core/src/code
include $(REVSDK_HOME)/common/linux/make/module-common.mk
LIBRARIES := -lX11
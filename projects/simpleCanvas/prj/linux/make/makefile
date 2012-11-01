#-----------------------------------------------------------------------------------------------------------------------
# Simple canvas
# Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
# On October 23rd, 2012
#-----------------------------------------------------------------------------------------------------------------------

# Project configuration variables
PRJ_NAME := simpleCanvas

# -- Folder structure --
ROOT_DIR := ../../..
SRC_DIR := $(ROOT_DIR)/src
CODE_DIR := $(SRC_DIR)/code
OUT_DIR := $(ROOT_DIR)/bin/linux

OUT_NAME := $(OUT_DIR)/$(PRJ_NAME)
OUTPUT := $(OUT_NAME)

include $(REVSDK_HOME)/common/linux/make/linux-common.mk

LINK_DIRS := $(LINK_DIRS) -L$(REVSDK_HOME)/modules/baseLevel/core/bin/linux \
 -L$(REVSDK_HOME)/modules/midLevel/video/bin/linux \
 -L$(REVSDK_HOME)/modules/highLevel/canvas/bin/linux
INCLUDE_FLAGS := $(INCLUDE_FLAGS) -I$(REVSDK_HOME)/modules/baseLevel/core/src/code \
 -I$(REVSDK_HOME)/modules/midLevel/video/src/code \
 -I$(REVSDK_HOME)/modules/highLevel/canvas/src/code

LIBRARIES := $(LIBRARIES) -lrevcanvas -lrevvideo -lrevcore -lGL -lX11

# -- Rules --
all: $(OUTPUT)

clean:
	rm -f $(OUTPUT) $(TEMP_FILES) $(OUT_NAME).elf \
	$(REVSDK_HOME)/modules/baseLevel/core/bin/linux/librevcore.a \
	$(REVSDK_HOME)/modules/midLevel/video/bin/linux/librevvideo.a
	cd $(REVSDK_HOME)/modules/highLevel/canvas/prj/linux/make && $(MAKE) clean
	cd $(REVSDK_HOME)/modules/baseLevel/core/prj/linux/make && $(MAKE) clean
	cd $(REVSDK_HOME)/modules/midLevel/video/prj/linux/make && $(MAKE) clean

run: $(OUTPUT)
	$(OUTPUT)

debug: $(OUTPUT)
	gdb $(OUTPUT)

%.cpp.o: %.cpp
	$(CXX) -c -o $@ $^ $(CXX_FLAGS)

$(OUT_NAME): $(CXX_OBJ)
	cd $(REVSDK_HOME)/modules/highLevel/canvas/prj/linux/make && $(MAKE)
	cd $(REVSDK_HOME)/modules/baseLevel/core/prj/linux/make/ && $(MAKE)
	cd $(REVSDK_HOME)/modules/midLevel/video/prj/linux/make/ && $(MAKE)
	$(CXX) -o $@ $^ $(CXX_FLAGS)

.PHONY: run clean

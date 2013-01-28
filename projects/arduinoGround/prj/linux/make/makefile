#-----------------------------------------------------------------------------------------------------------------------
# Generic template project based on Rev SDK
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

LIB_DIR := -L$(REVSDK_HOME)/modules/baseLevel/core/bin/linux -L$(REVSDK_HOME)/modules/midLevel/video/bin/linux
REV_INCLUDE := -I$(REVSDK_HOME)/modules/baseLevel/core/src/code -I$(REVSDK_HOME)/modules/midLevel/video/src/code

# -- Rules --
all: $(OUTPUT)

clean:
	rm -f $(OUTPUT) $(TEMP_FILES) $(OUT_NAME).elf \
	$(REVSDK_HOME)/modules/baseLevel/core/bin/linux/librevcore.a \
	$(REVSDK_HOME)/modules/midLevel/video/bin/linux/librevvideo.a

run: $(OUTPUT)
	$(OUTPUT)

debug: $(OUTPUT)
	gdb $(OUTPUT)

%.cpp.o: %.cpp
	$(CXX) -c -o $@ $^ $(CXX_FLAGS) $(REV_INCLUDE)

$(OUT_NAME): $(CXX_OBJ)
	cd $(REVSDK_HOME)/modules/baseLevel/core/prj/linux/make/ && $(MAKE)
	cd $(REVSDK_HOME)/modules/midLevel/video/prj/linux/make/ && $(MAKE)
	$(CXX) -o $@ $^ $(LIB_DIR) -lrevvideo -lrevcore

.PHONY: run clean

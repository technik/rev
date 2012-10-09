#-----------------------------------------------------------------------------------------------------------------------
# Rose project makefile for linux
# Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
# On October 6th, 2012
#-----------------------------------------------------------------------------------------------------------------------

# Project configuration variables
PRJ_NAME := rose

# -- Folder structure --
ROOT_DIR := ../../..
SRC_DIR := $(ROOT_DIR)/src
CODE_DIR := $(SRC_DIR)/code
OUT_DIR := $(ROOT_DIR)/bin/linux

OUT_NAME := $(OUT_DIR)/$(PRJ_NAME)
OUTPUT := $(OUT_NAME)

include $(REVSDK_HOME)/common/linux/make/linux-common.mk

LIB_DIR := -L$(REVSDK_HOME)/modules/core/bin/linux
REV_INCLUDE := -I$(REVSDK_HOME)/modules/core/src/

# -- Rules --
all: $(OUTPUT)

clean:
	rm -f $(OUTPUT) $(TEMP_FILES) $(OUT_NAME).elf $(REVSDK_HOME)/modules/core/bin/linux/librevcore.a

run: $(OUTPUT)
	$(OUTPUT)

debug: $(OUTPUT)
	gdb $(OUTPUT)

%.cpp.o: %.cpp
	$(CXX) -c -o $@ $^ $(CXX_FLAGS) $(REV_INCLUDE)

$(OUT_NAME): $(CXX_OBJ)
	cd $(REVSDK_HOME)/modules/core/prj/linux/make/ && $(MAKE)
	$(CXX) -o $@ $^ $(LIB_DIR) -lrevcore

.PHONY: run clean

#-----------------------------------------------------------------------------------------------------------------------
# Revolution SDK
# Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
# On October 7th, 2012
#-----------------------------------------------------------------------------------------------------------------------
# Core module, make file

# Project configuration variables
MODULE_NAME := revcore

# -- Folder structure --
ROOT_DIR := ../../..
SRC_DIR := $(ROOT_DIR)/src
CODE_DIR := $(SRC_DIR)
OUT_DIR := $(ROOT_DIR)/bin

include $(REVSDK_HOME)/common/arduino/make/arduino-common.mk

OUTPUT := $(OUT_DIR)/lib$(MODULE_NAME).a

# -- Rules --
all: $(OUTPUT)

clean:
	rm -f $(OUTPUT) $(TEMP_FILES)

$(OUTPUT): $(CXX_OBJ)
	$(AR) rcs $(OUTPUT) $(CXX_OBJ)

%.cpp.o: %.cpp
	$(CXX) -c -o $@ $^ $(CXX_FLAGS)

%.c.o : %.c
	$(CC) -o $@ $^ $(CC_FLAGS)

.PHONY: clean
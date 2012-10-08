#-----------------------------------------------------------------------------------------------------------------------
# Rose project makefile for linux
# Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
# On October 6th, 2012
#-----------------------------------------------------------------------------------------------------------------------

# Project configuration variables
PRJ_NAME := ciervaPilot

# -- Folder structure --
ROOT_DIR := ../../..
SRC_DIR := $(ROOT_DIR)/src
CODE_DIR := $(SRC_DIR)/code
OUT_DIR := $(ROOT_DIR)/bin

OUT_NAME := $(OUT_DIR)/$(PRJ_NAME)
OUTPUT := $(OUT_NAME).hex

include $(REVSDK_HOME)/common/arduino/make/arduino-common.mk

LIB_DIR := -L$(REVSDK_HOME)/modules/core/bin
REV_INCLUDE := -I$(REVSDK_HOME)/modules/core/src/

# -- Rules --
all: $(OUTPUT)

clean:
	rm -f $(OUTPUT) $(TEMP_FILES) $(OUT_NAME).elf

run: $(OUTPUT)
	avrdude -p atmega2560 -c stk500 -P /dev/ttyACM0 -U flash:w:$^:i

%.hex: %.elf
	$(OBJCOPY) -O ihex -R .eeprom -R .fuse -R .lock -R .signature $^ $@

$(OUT_NAME).elf: $(CXX_SRC) $(CORE_LIB)
	cd $(REVSDK_HOME)/modules/core/prj/arduino/make/ && $(MAKE)
	$(CXX) -o $@ $^ $(CXX_FLAGS) $(REV_INCLUDE) $(LIB_DIR) -lrevcore

readelf: $(OUT_NAME).elf
	$(READELF) -a $^

dump: $(OUT_NAME).elf
	$(OBJDUMP) -h $^

dwarf: $(OUT_NAME).elf
	$(OBJDUMP) --dwarf=info $^

assebly: (OUT_NAME).S

$(OUT_NAME).S: $(CXX_SRC)
	$(CXX) -S -o $@ $^ $(CXX_FLAGS)

.PHONY: readelf run clean
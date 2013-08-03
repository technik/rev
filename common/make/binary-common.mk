#-----------------------------------------------------------------------------------------------------------------------
# Revolution SDK
# Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
# On October 30th, 2012
#-----------------------------------------------------------------------------------------------------------------------
# Binary common makefile

include $(REVSDK_HOME)/common/make/$(PLATFORM)/toolchain.mk
include $(REVSDK_HOME)/common/make/$(PLATFORM)/platform.mk

CXX_SRC := $(shell find $(CODE_DIR) -name *.cpp)
CXX_OBJ := $(patsubst %.cpp, %.cpp.$(PLATFORM).o, $(CXX_SRC))

TEMP_FILES := $(CXX_OBJ)

# -- Build variables --
PREPROCESSOR_DEFINITIONS := $(PLATFROM_DEFINITIONS)
WARNING_FLAGS := -Wall -Werror
INCLUDE_FLAGS := -I$(CODE_DIR)
CXX_COMPILE_FLAGS := -std=c++0x -fno-access-control -fno-enforce-eh-specs\
 			-fno-rtti -fno-exceptions -ffunction-sections -fdata-sections
DEBUG_FLAGS := -g
OPTIMIZATION_FLAGS := # -O4
COMMON_C_FLAGS := $(PLATFORM_FLAGS) $(PREPROCESSOR_DEFINITIONS) $(WARNING_FLAGS)\
 			 $(INCLUDE_FLAGS) $(DEBUG_FLAGS) $(OPTIMIZATION_FLAGS)
CC_FLAGS := $(COMMON_C_FLAGS)
CXX_FLAGS := $(COMMON_C_FLAGS) $(CXX_COMPILE_FLAGS)

BUILD_DIR := build/$(MCU)/
ASM_DIR := asm/$(MCU)/
OUT_DIR := $(ROOT_DIR)/bin/$(MCU)

OUT_NAME := $(OUT_DIR)/$(PRJ_NAME)
OUTPUT := $(OUT_NAME).hex

# -- Rules --
all: $(OUTPUT)

clean:
	rm -f $(OUTPUT) $(TEMP_FILES) $(OUT_NAME).elf

run: $(OUTPUT)
	$(AVRDUDE) -p$(MCU) -c$(PROG_PROTOCOL) -b115200 -P$(PORT) -D -Uflash:w:$^:i

%.hex: %.elf
	$(OBJCOPY) -O ihex -R .eeprom -R .fuse -R .lock -R .signature $^ $@

$(OUT_NAME).elf: $(CXX_OBJ)
	$(CXX) -o $@ $^ $(CXX_FLAGS) $(REV_INCLUDE) $(LIB_DIR) -lc -lm -lc


$(ASM_DIR)%.cpp.s: $(CODE_DIR)%.cpp
	$(CXX) -S -o $@ $^ $(CXX_FLAGS) $(REV_INCLUDE) $(LIB_DIR)


%.cpp.$(PLATFORM).o: %.cpp
	$(CXX) -c -o $@ $^ $(CXX_FLAGS) $(REV_INCLUDE) $(LIB_DIR) -lc -lm -lc

readelf: $(OUT_NAME).elf
	$(READELF) -a $^

dump: $(OUT_NAME).elf
	$(OBJDUMP) -h $^

dwarf: $(OUT_NAME).elf
	$(OBJDUMP) --dwarf=info $^

assembly: (OUT_NAME).S

$(OUT_NAME).S: $(CXX_SRC)
	$(CXX) -S -o $@ $^ $(CXX_FLAGS)

.PHONY: readelf run clean
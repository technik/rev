# Template arduino project based on Rev SDK
# Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
# On October 6th, 2012

# Project configuration variables
PRJ_NAME := arduinoTemplate

# -- Global variables --
ATMEGA_TOOLCHAIN := $(AVR_HOME)/bin

# -- GNU Tools --
AS := $(ATMEGA_TOOLCHAIN)/avr-as # Assembler
CC := $(ATMEGA_TOOLCHAIN)/avr-gcc # C Compiler
CXX := $(ATMEGA_TOOLCHAIN)/avr-g++ # C++ Compiler
OBJCOPY := $(ATMEGA_TOOLCHAIN)/avr-objcopy # Obj copy
OBJDUMP := $(ATMEGA_TOOLCHAIN)/avr-objdump
READELF := $(ATMEGA_TOOLCHAIN)/avr-readelf

# -- Folder structure --
ROOT_DIR := ../../..
SRC_DIR := $(ROOT_DIR)/src
CODE_DIR := $(SRC_DIR)/code

CXX_SRC := $(shell find $(CODE_DIR) -name *.cpp)

# -- Build variables --
CXX_FLAGS := -mmcu=atmega2560 -fno-acess-control -fno-enforce-eh-specs -fno-rtti -g0 -O1 -fno-exceptions -ffunction-sections -fdata-sections
OUT_NAME := $(PRJ_NAME)
OUTPUT := $(OUT_NAME).hex

# -- Rules --
all: $(OUTPUT)

%.hex: %.elf
	$(OBJCOPY) -O ihex .eeprom -R .fuse -R .lock -R .signature $^ $@

$(OUT_NAME).elf: $(CXX_SRC)
	$(CXX) -o $@ $^ $(CXX_FLAGS)

readelf: $(OUT_NAME).elf
	$(READELF) -a $^

assebly: (OUT_NAME).S

$(OUT_NAME).S: $(CXX_SRC)
	$(CXX) -S -o $@ $^ $(CXX_FLAGS)

.PHONY: all readelf
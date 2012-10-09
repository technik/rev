#-----------------------------------------------------------------------------------------------------------------------
# Revolution SDK
# Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
# On October 7th, 2012
#-----------------------------------------------------------------------------------------------------------------------
# Arduino common makefile

# -- Global variables --
ATMEGA_TOOLCHAIN := $(AVR_HOME)/bin

# -- GNU Tools --
AS := $(ATMEGA_TOOLCHAIN)/avr-as # Assembler
AR := $(ATMEGA_TOOLCHAIN)/avr-ar # Archiver
CC := $(ATMEGA_TOOLCHAIN)/avr-gcc # C Compiler
CXX := $(ATMEGA_TOOLCHAIN)/avr-g++ # C++ Compiler
OBJCOPY := $(ATMEGA_TOOLCHAIN)/avr-objcopy # Obj copy
OBJDUMP := $(ATMEGA_TOOLCHAIN)/avr-objdump
READELF := $(ATMEGA_TOOLCHAIN)/avr-readelf

CXX_SRC := $(shell find $(CODE_DIR) -name *.cpp)
CXX_OBJ := $(patsubst %.cpp, %.cpp.o, $(CXX_SRC))

TEMP_FILES := $(CXX_OBJ)

# -- Build variables --
PREPROCESSOR_DEFINITIONS := -DATMEGA -DATMEGA2560
WARNING_FLAGS := -Wall -Werror
INCLUDE_FLAGS := -I$(CODE_DIR)
CXX_COMPILE_FLAGS := -std=c++0x -mmcu=atmega2560 -fno-access-control -fno-enforce-eh-specs\
 -fno-rtti -fno-exceptions -ffunction-sections -fdata-sections
DEBUG_FLAGS := -g
OPTIMIZATION_FLAGS := -O1
COMMON_C_FLAGS := $(PREPROCESSOR_DEFINITIONS) $(WARNING_FLAGS) $(INCLUDE_FLAGS) $(DEBUG_FLAGS) $(OPTIMIZATION_FLAGS
CC_FLAGS := $(COMMON_C_FLAGS)
CXX_FLAGS := $(COMMON_C_FLAGS) $(CXX_COMPILE_FLAGS)
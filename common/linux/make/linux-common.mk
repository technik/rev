#-----------------------------------------------------------------------------------------------------------------------
# Revolution SDK
# Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
# On October 7th, 2012
#-----------------------------------------------------------------------------------------------------------------------
# Linux common makefile


# -- GNU Tools --
AS := as # Assembler
AR := ar # Archiver
CC := gcc # C Compiler
CXX := g++ # C++ Compiler
OBJCOPY := objcopy # Obj copy
OBJDUMP := objdump
READELF := readelf

CXX_SRC := $(shell find $(CODE_DIR) -name *.cpp)
CXX_OBJ := $(patsubst %.cpp, %.cpp.o, $(CXX_SRC))

TEMP_FILES := $(CXX_OBJ)

# -- Build variables --
PREPROCESSOR_DEFINITIONS := 
WARNING_FLAGS := -Wall -Werror
INCLUDE_FLAGS := -I$(CODE_DIR)
CXX_COMPILE_FLAGS := -std=c++0x -fno-access-control -fno-enforce-eh-specs\
 -fno-rtti -g -O1 -fno-exceptions -ffunction-sections -fdata-sections
COMMON_C_FLAGS := $(PREPROCESSOR_DEFINITIONS) $(WARNING_FLAGS) $(INCLUDE_FLAGS)
CC_FLAGS := $(COMMON_C_FLAGS)
CXX_FLAGS := $(COMMON_C_FLAGS) $(CXX_COMPILE_FLAGS)
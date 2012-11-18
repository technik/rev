#-----------------------------------------------------------------------------------------------------------------------
# Revolution SDK
# Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
# On October 30th, 2012
#-----------------------------------------------------------------------------------------------------------------------
# Binary common makefile

include $(REVSDK_HOME)/common/make/$(PLATFORM)/toolchain.mk
include $(REVSDK_HOME)/common/make/$(PLATFORM)/platform.mk

CXX_SRC := $(shell find $(CODE_DIR) -name *.cpp)
CXX_OBJ := $(patsubst %.cpp, %.cpp.o, $(CXX_SRC))

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
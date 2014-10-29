#-------------------------------------------------------------------------------
# Base makefile for binary projects
# Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
# On 2014-10-27
#-------------------------------------------------------------------------------
include $(REVSDK_HOME)/common/make/linux/toolchain.mk

# ----- Build configuration -------
debug ?= false

# ----- Files and directories -----
SRC_DIR ?= .
CXX_SRC := $(shell find $(SRC_DIR) -name *.cpp)
CXX_OBJ = $(patsubst %.cpp, %.cpp.o, $(CXX_SRC))

TEMP_FILES = $(CXX_OBJ)

# ----- Build flags -----
WARNING_FLAGS = -Wall -Werror
INCLUDE_FLAGS = -I$(SRC_DIR)/ -I$(REVSDK_HOME)/engine/src/
LIBRARIES := $(LIBRARIES) -lrev
LINK_DIRS := -L$(REVSDK_HOME)/engine/bin
LINK_FLAGS = $(LINK_DIRS) $(LIBRARIES)
CXX_COMPILE_FLAGS = -std=c++11 -fno-rtti -fno-exceptions
ifeq (debug, true)
DEBUG_FLAGS = -g
endif
OPTIMIZATION_FLAGS = #-O1
COMMON_C_FLAGS = $(PREPROCESSOR_DEFINITIONS) $(WARNING_FLAGS) $(INCLUDE_FLAGS) \
 $(LINK_FLAGS) $(DEBUG_FLAGS) $(OPTIMIZATION_FLAGS)
CC_FLAGS = $(COMMON_C_FLAGS)
CXX_FLAGS = $(COMMON_C_FLAGS) $(CXX_COMPILE_FLAGS)

BUILD_DIR := ./build/
OUT_DIR ?= $(SRC_DIR)

OUT_NAME ?= $(OUT_DIR)/$(PRJ_NAME)
OUTPUT := $(OUT_NAME)

include $(REVSDK_HOME)/common/make/binary-rules.mk
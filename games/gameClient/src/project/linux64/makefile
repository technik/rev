################################################################################
# Revolution Engine client
# by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
# Created on August 20th, 2011
################################################################################
# makefile for linux x64 platform

################################################################################
# make arguments
################################################################################
CONFIG=debug	# build config (e.g. debug, release)
GEN_DEPEND=true	# Generate dependencies

################################################################################
# Project and folders
################################################################################
# Project name
PROJECT_NAME=gameClient
# Root directory
ROOT_FOLDER = ../../../..
# Install directory
ifdef REVOLUTION_ENGINE_HOME
INSTALL_FOLDER = $(REVOLUTION_ENGINE_HOME)/bin/$(PROJECT_NAME)/linux64
else
INSTALL_FOLDER = ROOT_FOLDER/bin/$(PROJECT_NAME)/linux64
endif
# Engine installation folder
ifdef REVOLUTION_ENGINE_HOME
ENGINE_HOME=$(REVOLUTION_ENGINE_HOME)
else
ENGINE_HOME=ROOT_FOLDER
endif
# Engine headers
ifdef $(USE_RELEASE_ENGINE)
ENGINE_INC_DIR=$(ENGINE_HOME)/include
else
ENGINE_INC_DIR=$(ENGINE_HOME)/src/engine/code
endif
ENGINE_LIB_DIR=$(ENGINE_HOME)/lib
# The directory for all sources of the project (not only code)
SRC_FOLDER = $(ROOT_FOLDER)/src/$(PROJECT_NAME)
# Source code folder
CODE_FOLDER = $(SRC_FOLDER)/code
# Output folder
OUT_FOLDER = $(ROOT_FOLDER)/build/$(PROJECT_NAME)/linux64

################################################################################
# Build options
################################################################################
INCLUDE_DIRS=-iquote $(CODE_FOLDER) -I$(ENGINE_INC_DIR)
CPP_WARNING_FLAGS = -Wall -Wextra -Werror
ifeq ($(CONFIG), debug)
CPP_PREPROCESSOR_DEFINITIONS = -D_DEBUG -D_linux
else ifeq ($(CONFIG), release)
CPP_PREPROCESSOR_DEFINITIONS = -D_RELEASE -D_linux
endif
CPP_COMPILER_FLAGS=$(CPP_WARNING_FLAGS) $(CPP_PREPROCESSOR_DEFINITIONS) \
                   -fno-rtti -fno-exceptions -g
CPP_COMPILER_OPTIONS=$(INCLUDE_DIRS) $(CPP_COMPILER_FLAGS)
LIB_DIRS=-L$(ENGINE_LIB_DIR)
ifeq ($(CONFIG), debug)
LIBS= -lrevd -lGL -lX11
ENGINE_LIB = $(REVOLUTION_ENGINE_HOME)/lib/librevd.a
else
LIBS= -lrev -lGL -lX11
ENGINE_LIB = $(REVOLUTION_ENGINE_HOME)/lib/librev.a
endif
LINKER_OPTIONS=$(LIB_DIRS) $(LIBS) $(LINKER_FLAGS)

################################################################################
# Files
################################################################################
# Main output
ifeq ($(CONFIG), debug)
OUTPUT_FILE = $(PROJECT_NAME)-dbg.elf
else
OUTPUT_FILE = $(PROJECT_NAME).elf
endif
OUTPUT = $(OUT_FOLDER)/$(OUTPUT_FILE)
# Source code files
CPP_SRC = $(shell find $(CODE_FOLDER) -name *.cpp)
# temporary object files
CPP_OBJ = $(patsubst %.cpp,%.o,$(CPP_SRC))
# temporary dependency files
ifeq ($(GEN_DEPEND), true)
# Only create dependencies for existing obj files
CPP_DEP = $(shell find $(CODE_FOLDER) -name *.o)
CPP_DEP = $(patsubst %.o, %.depend, $(CPP_DEP))
endif
# list of all temporary files
TEMP_FILES = $(CPP_OBJ) $(CPP_DEP)

################################################################################
# Makefile rules
################################################################################
# The default rule builds every thing
all: $(OUTPUT)

# ------------------------- Generic rules --------------------------------------
clean:
	rm -f $(OUTPUT) $(TEMP_FILES)

rebuild: clean all

run: $(OUTPUT)
	$(OUTPUT)

install: $(OUTPUT)
	cp -f $(OUTPUT) -t $(INSTALL_FOLDER)

# ------------------------- Other rules ----------------------------------------
# The main output
# Using g++ to link obj files links against c++ standard libraries.
$(OUTPUT): $(CPP_OBJ) $(ENGINE_LIB)
	g++ -o $@ $(CPP_OBJ) $(LINKER_OPTIONS)

$(ENGINE_LIB):
	cd $(REVOLUTION_ENGINE_HOME)/src/engine/project/linux64/ && $(MAKE) CONFIG=$(CONFIG)

# Dependency files
include $(CPP_DEP)

# Rule for building dependencies
%.depend: %.cpp
	g++ -MM -MP -MF $@ -MT $(patsubst %.cpp, %.o, $^) $^ $(CPP_COMPILER_OPTIONS)

# Rule for compiling cpp files
%.o: %.cpp
	g++ -c -o $@ $^ $(CPP_COMPILER_OPTIONS)

# Phony targets
.PHONY: all clean rebuild run install $(ENGINE_LIB)

# base engine makefile

SRC_DIR = ./src

CXX_SRC := $(shell find $(SRC_DIR) -name *.cpp)
CXX_OBJ = $(patsubst %.cpp, %.cpp.o, $(CXX_SRC))

TEMP_FILES = $(CXX_OBJ)

# ----- Build flags -----
WARNING_FLAGS = -Wall -Werror
INCLUDE_FLAGS = -I$(SRC_DIR)/
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

OUT_DIR = ./bin

OUT_NAME ?= $(OUT_DIR)/librev
OUTPUT := $(OUT_NAME)

# -- Rules --
all: $(OUTPUT)

clean:
	rm -f $(OUTPUT) $(TEMP_FILES)

run: $(OUTPUT)
	$(OUTPUT)

%.cpp.o: %.cpp
	$(CXX) -c -o $@ $^ $(CXX_FLAGS) -lc -lm -lc

$(OUTPUT): $(CXX_SRC)
	$(CXX) -o $@ $^ $(CXX_FLAGS) -lc -lm -lc

.PHONY: readelf run clean
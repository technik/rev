#-----------------------------------------------------------------------------------------------------------------------
# Revolution SDK
# Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
# On October 8th, 2012
#-----------------------------------------------------------------------------------------------------------------------
# Generic gnu module common makefile

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
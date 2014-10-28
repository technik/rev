#-------------------------------------------------------------------------------
# Common rules for binary files
# Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
# On 2014-10-27
#-------------------------------------------------------------------------------

# -- Rules --
all: $(OUTPUT)

clean:
	rm -f $(OUTPUT) $(TEMP_FILES) $(OUT_NAME).elf

run: $(OUTPUT)
	$(OUTPUT)

%.cpp.o: %.cpp
	$(CXX) -c -o $@ $^ $(CXX_FLAGS) $(REV_INCLUDE) $(LIB_DIR) -lc -lm -lc

$(OUTPUT): $(CXX_SRC)
	$(CXX) -o $@ $^ $(CXX_FLAGS) $(REV_INCLUDE) $(LIB_DIR) -lc -lm -lc

.PHONY: readelf run clean
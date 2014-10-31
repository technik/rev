#-------------------------------------------------------------------------------
# Common rules for binary files
# Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
# On 2014-10-27
#-------------------------------------------------------------------------------

# -- Rules --
all: $(OUTPUT)

clean:
	rm -f $(OUTPUT) $(TEMP_FILES)

run: $(OUTPUT)
	$(OUTPUT)

%.cpp.o: %.cpp
	$(CXX) -c -o $@ $^ $(CXX_FLAGS) -lc -lm -lc

$(REVSDK_HOME)/engine/bin/librev:
	cd $(REVSDK_HOME)/engine && $(MAKE)

$(OUTPUT): $(CXX_SRC) $(REVSDK_HOME)/engine/bin/librev.a
	$(CXX) -o $@ $^ $(CXX_FLAGS) -lc -lm -lc

.PHONY: readelf run clean $(REVSDK_HOME)/engine/bin/librev.a
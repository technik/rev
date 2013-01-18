##############################################################
# Terminal's temporary makefile
##############################################################


ROOT_DIR := ../..
CODE_DIR := $(ROOT_DIR)/src/code
FLEX_CXX := $(CODE_DIR)/lexer.cpp
BISON_C := $(CODE_DIR)/parser.c
BISON_CXX := $(BISON_C)pp
CXX_SRC := $(BISON_CXX) $(FLEX_CXX) \
			$(ROOT_DIR)/src/code/main.cpp
			

OUTPUT := $(ROOT_DIR)/bin/terminal.exe

all: $(OUTPUT)

run: $(OUTPUT)
	$(OUTPUT)
	
clean:
	rm -f $(OUTPUT) $(BISON_CXX) $(FLEX_CXX)
	
$(OUTPUT): $(CXX_SRC)
	g++ -o $(OUTPUT) $(CXX_SRC)
	
$(FLEX_CXX): $(ROOT_DIR)/src/flex/revscript.l
	flex -o $(FLEX_CXX) $(ROOT_DIR)/src/flex/revscript.l
	
$(BISON_CXX): $(ROOT_DIR)/src/bison/revscript.y
	bison -d -o $(BISON_C) $(ROOT_DIR)/src/bison/revscript.y
	mv $(BISON_C) $(BISON_CXX)
	
.PHONY: run clean
##############################################################
# RevScript project
# This makefile generates revScript lexer and parser's source
# Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
# On January 19th, 2013
##############################################################

ROOT_DIR := ../..
CODE_DIR := $(ROOT_DIR)/src/code/revScript
FLEX_CXX := $(CODE_DIR)/backend/lexer.cpp
BISON_C := $(CODE_DIR)/backend/parser.c
BISON_CXX := $(BISON_C)pp

all: bison flex

flex: $(FLEX_CXX)

bison: $(BISON_CXX)
	
clean:
	rm -f $(BISON_CXX) $(FLEX_CXX)
	
$(FLEX_CXX): $(ROOT_DIR)/src/flex/revscript.l
	flex -o $(FLEX_CXX) $(ROOT_DIR)/src/flex/revscript.l
	
$(BISON_CXX): $(ROOT_DIR)/src/bison/revscript.y
	bison -d -o $(BISON_C) $(ROOT_DIR)/src/bison/revscript.y
	mv $(BISON_C) $(BISON_CXX)
	
.PHONY: run clean
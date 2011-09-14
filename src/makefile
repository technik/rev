################################################################################
# Revolution Engine, Top makefile
# Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
# on August 21st, 2011
################################################################################
# Top level maefile. Can be used to build tools, libraries, plugins, etc...

################################################################################
# makefile arguments
################################################################################
CONFIG=release

################################################################################
# Variables
################################################################################
ifndef REVOLUTION_ENGINE_HOME
REVOLUTION_ENGINE_HOME=$(CURDIR)/..
endif
export CONFIG REVOLUTION_ENGINE_HOME

# -------------- Engine --------------------------------------------------------
ifeq ($(CONFIG), debug)
ENGINE_OUT=$(REVOLUTION_ENGINE_HOME)/lib/librevd.a
else
ENGINE_OUT=$(REVOLUTION_ENGINE_HOME)/lib/librev.a
endif
ENGINE_PRJ_DIR=$(CURDIR)/engine/project/linux64

# -------------- Game client ---------------------------------------------------
GAMECLIENT_OUT=$(REVOLUTION_ENGINE_HOME)/build/gameClient/gameClient.elf
GAMECLIENT_PRJ_DIR=$(REVOLUTION_ENGINE_HOME)/src/gameClient/project/linux64

################################################################################
# makefile rules 
################################################################################
# --- Default rule builds everything
all: engine gameClient

engine: $(ENGINE_OUT)

$(ENGINE_OUT):
	cd $(ENGINE_PRJ_DIR) && $(MAKE)

gameClient: $(GAMECLIENT_OUT)

$(GAMECLIENT_OUT):
	cd $(GAMECLIENT_PRJ_DIR) && $(MAKE)

clean:
	rm -f -r $(ENGINE_OUT) $(GAMECLIENT_OUT)

#-------------------------------------------------------------------------------
# Phony targets
.PHONY: all engine clean

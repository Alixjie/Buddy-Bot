HOME_TREE := ../

MODULE_NAME := $(notdir $(CURDIR))

include $(HOME_TREE)/mak_def.inc

CXXSRC += main.cpp
C_INCLUDES += -I$(CURDIR)/../sdk/include -I$(CURDIR)/../sdk/src

EXTRA_OBJ := 
LD_LIBS += -lstdc++ -lpthread

all: build_app

include $(HOME_TREE)/mak_common.inc

clean: clean_app

####
#### Generic Makefile for C or C++ projects
####
#### This file is public domain.
#### Jamie Bullock 2014 <jamie@jamiebullock.com>
####

###################################
### User configurable variables ###
###################################

#### It is best not to modify this file
#### Instead override these variables in a separate Make.config file if needed

# The name of the product to build (default uses parent directory name)
NAME ?= $(notdir $(CURDIR))
# The file suffix of source files, can be .c or .cpp
SUFFIX ?= .c
# List of directories containing source files to be compiled
DIRS ?= .
# Flags to pass to the compiler for release builds
FLAGS ?= -O3
# Flags to pass to the compiler for debug builds
DEBUG_FLAGS ?= -O0 -g
# Flags to pass to the linker
LDFLAGS ?=
# Type of product to build: "shared" for a shared library, "static" for a static library, empty for standalone
LIBRARY ?= static
# Prefix to the path that the "install" target will install into. libs to $(PREFIX)/lib, executables to $(PREFIX)/bin
PREFIX ?= /usr/local

##############################################
### Do not modify anything below this line ###
##############################################

ifeq ($(OS),Windows_NT)
else
    PLATFORM := $(shell uname -s)
endif

-include Make.config

OUT_DIR := .build
SRC := $(foreach dir, $(DIRS), $(wildcard $(dir)/*$(SUFFIX)))
OBJ_ := $(SRC:$(SUFFIX)=.o)
OBJ := $(addprefix $(OUT_DIR)/,$(OBJ_))
DEPS := $(OBJ:.o=.d)
SHARED_SUFFIX := dll
STATIC_SUFFIX := lib
INSTALL_DIR := $(PREFIX)/lib

ifeq "$(PLATFORM)" "Darwin"
    SHARED_SUFFIX := dylib
    STATIC_SUFFIX := a
endif

ifeq "$(PLATFORM)" "Linux"
    SHARED_SUFFIX := so
    STATIC_SUFFIX := a
endif

ifeq "$(LIBRARY)" "shared"
    OUT=lib$(NAME).$(SHARED_SUFFIX)
    LDFLAGS += -shared
else ifeq "$(LIBRARY)" "static"
    OUT=lib$(NAME).$(STATIC_SUFFIX)
else
    OUT=$(NAME)
    INSTALL_DIR := $(PREFIX)/bin
endif

ifeq "$(SUFFIX)" ".cpp"
    COMPILER := $(CXX)
else ifeq "$(SUFFIX)" ".c"
    COMPILER := $(CC)
endif

.SUFFIXES:
.PHONY: debug clean install uninstall

$(OUT): $(OBJ)
ifeq "$(LIBRARY)" "static"
	@$(AR) rcs $@ $^
else
	@$(COMPILER) $(LDFLAGS) $^ -o $@
endif

debug: FLAGS = $(DEBUG_FLAGS)
debug: $(OUT)

$(OUT_DIR)/%.o: %$(SUFFIX)
	@mkdir -p $(dir $@)
	@$(COMPILER) $(CXXFLAGS) $(FLAGS) -MMD -MP -fPIC -c $< -o $@

install: $(OUT)
	@install -d $(INSTALL_DIR)
	@install $(OUT) $(INSTALL_DIR)

uninstall:
	@$(RM) $(INSTALL_DIR)/$(OUT)

clean:
	@$(RM) -r $(OUT) $(OUT_DIR)

-include: $(DEPS)


LIBRARY ?= static
PREFIX ?= $(PWD)/dist

HPATH = include/xtract

export XTRACT_VERSION PREFIX LIBRARY

.PHONY: examples clean install doc src swig

all: src examples

src:
	@$(MAKE) -C $@

doc:
	@$(MAKE) -C $@

examples:
	@$(MAKE) -C $@

swig: src
	@$(MAKE) -C $@

check: src
	@$(MAKE) -C tests check

test: check

install:
	@$(MAKE) -C src install
	@$(MAKE) -C examples install
	@mkdir -p $(PREFIX)/$(HPATH)
	@cp $(HPATH)/* $(PREFIX)/$(HPATH)

clean:
	@$(MAKE) -C src clean
	@$(MAKE) -C examples clean
	@$(MAKE) -C swig clean
	@$(RM) -r dist

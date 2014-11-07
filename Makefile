
LIBRARY ?= static
PREFIX ?= $(PWD)/dist

HPATH = include/xtract

export XTRACT_VERSION PREFIX LIBRARY

.PHONY: examples clean install doc src

all: src examples

src:
	@$(MAKE) -C $@

doc:
	@$(MAKE) -C $@

examples:
	@$(MAKE) -C $@

install:
	@$(MAKE) -C src install
	@$(MAKE) -C examples install
	@mkdir -p $(PREFIX)/$(HPATH)
	@cp $(HPATH)/* $(PREFIX)/$(HPATH)

clean:
	@$(MAKE) -C src clean
	@$(MAKE) -C examples clean
	@$(RM) -r dist


PREFIX ?= $(PWD)/dist
HPATH = include/xtract

static: LIBRARY = static
shared: LIBRARY = shared

export XTRACT_VERSION PREFIX LIBRARY

.PHONY: examples clean static shared install doc

all: static examples

static shared:
	@$(MAKE) -C src

examples:
	@$(MAKE) -C $@

doc:
	@$(MAKE) -C $@

install:
	$(MAKE) -C src install
	$(MAKE) -C examples install
	@mkdir -p $(PREFIX)/$(HPATH)
	@cp $(HPATH)/* $(PREFIX)/$(HPATH)

clean:
	@$(MAKE) -C src clean
	@$(MAKE) -C examples clean
	@$(RM) -r dist

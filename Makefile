
PREFIX = dist

static: LIBTYPE = static
shared: LIBTYPE = shared

.PHONY: examples clean static shared install

all: static examples

static shared:
	@$(MAKE) -C src LIBRARY=$(LIBTYPE)

examples:
	@$(MAKE) -C examples

install:
	$(MAKE) -C src install PREFIX=$(PWD)/$(PREFIX)
	$(MAKE) -C examples install PREFIX=$(PWD)/$(PREFIX)
	@mkdir -p $(PREFIX)/include/xtract
	@cp include/xtract/* $(PREFIX)/include/xtract

clean:
	@$(MAKE) -C src clean
	@$(MAKE) -C examples clean

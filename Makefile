
static: LIBTYPE = static
shared: LIBTYPE = shared

.PHONY: examples clean static shared

all: static examples

static shared:
	@$(MAKE) -C src LIBRARY=$(LIBTYPE)

examples:
	@$(MAKE) -C examples

clean:
	@$(MAKE) -C src clean
	@$(MAKE) -C examples clean

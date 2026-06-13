
LIBRARY ?= static
PREFIX ?= $(PWD)/dist
XTRACT_VERSION := $(shell cat VERSION)

HPATH = include/xtract

export XTRACT_VERSION PREFIX LIBRARY

.PHONY: examples clean install doc src swig bench analyze check-asan cppcheck

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

analyze:
	@$(MAKE) -C src analyze

# Requires cppcheck; checks all preprocessor configurations of the
# first-party sources (vDSP and OOURA branches alike)
cppcheck:
	@cppcheck --enable=warning,performance,portability --std=c99 \
		--inline-suppr --error-exitcode=1 --quiet \
		-I include -I src src/*.c
	@echo "cppcheck clean"

# Rebuild the library and tests with AddressSanitizer and UBSan and run the
# suite; catches memory errors and undefined behaviour that static analysis
# cannot see (e.g. out-of-bounds access through opaque library calls).
# Cleans before and after so sanitised objects never mix with normal builds.
SANITIZE_FLAGS = -fsanitize=address,undefined -fno-sanitize-recover=all -fno-omit-frame-pointer -g
check-asan:
	@$(MAKE) -C src clean
	@$(MAKE) -C tests clean
	@$(MAKE) -C src EXTRA_FLAGS="$(SANITIZE_FLAGS)"
	@$(MAKE) -C tests check EXTRA_FLAGS="$(SANITIZE_FLAGS)"
	@$(MAKE) -C src clean
	@$(MAKE) -C tests clean
	@$(MAKE) -C src

bench: src
	@$(MAKE) -C bench bench

test: check

install:
	$(MAKE) -C src install
	$(MAKE) -C examples install
	mkdir -p $(PREFIX)/$(HPATH)
	cp $(HPATH)/* $(PREFIX)/$(HPATH)

clean:
	@$(MAKE) -C src clean
	@$(MAKE) -C examples clean
	@$(MAKE) -C swig clean
	@$(MAKE) -C bench clean
	@$(RM) -r dist

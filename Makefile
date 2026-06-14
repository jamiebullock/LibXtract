
LIBRARY ?= static
PREFIX ?= $(PWD)/dist
XTRACT_VERSION := $(shell cat VERSION)

HPATH = include/xtract

export XTRACT_VERSION PREFIX LIBRARY

.PHONY: examples clean install doc src swig bench analyze check-asan cppcheck coverage

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

# Rebuild the library and tests with gcov instrumentation, run the suite, and
# produce a line/branch coverage report for the first-party sources. Requires
# lcov (provides lcov/genhtml; uses gcov from the active toolchain). Third-party
# code (ooura, c-ringbuf, dywapitchtrack) keeps its upstream style and is out of
# scope for the test baseline, so it is filtered out of the report. Cleans
# before and after so instrumented objects never mix with normal builds.
COVERAGE_FLAGS = --coverage
COVERAGE_IGNORE = --ignore-errors unused,inconsistent,format,empty,gcov,unsupported
coverage:
	@$(MAKE) -C src clean
	@$(MAKE) -C tests clean
	@$(MAKE) -C src EXTRA_FLAGS="$(COVERAGE_FLAGS)"
	@$(MAKE) -C tests check EXTRA_FLAGS="$(COVERAGE_FLAGS)"
	@lcov --capture --directory src/.build --output-file coverage.info $(COVERAGE_IGNORE)
	@lcov --remove coverage.info '*/ooura/*' '*/c-ringbuf/*' '*/dywapitchtrack/*' \
		--output-file coverage.info $(COVERAGE_IGNORE)
	@lcov --list coverage.info $(COVERAGE_IGNORE)
	@genhtml coverage.info --output-directory coverage-html $(COVERAGE_IGNORE) >/dev/null
	@echo "Coverage report written to coverage-html/index.html"
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

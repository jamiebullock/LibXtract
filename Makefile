# Part of LibXtract
#
# SPDX-FileCopyrightText: 2014 Jamie Bullock
# SPDX-License-Identifier: Zlib

LIBRARY ?= static
PREFIX ?= $(PWD)/dist
XTRACT_VERSION := $(shell cat VERSION)

HPATH = include/xtract

export XTRACT_VERSION PREFIX LIBRARY

.PHONY: examples clean install doc src swig bench analyze check-asan cppcheck coverage fuzz mutation verify format format-check

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

# The targets below run from CMake presets, each in its own directory under
# build/, so an instrumented build never mixes with the normal one and only
# the changed files are rebuilt
CMAKE ?= cmake
CTEST ?= ctest

analyze:
	@$(CMAKE) --preset default >/dev/null && $(CMAKE) --build --preset default --target analyze

# Requires cppcheck
cppcheck:
	@$(CMAKE) --preset default >/dev/null && $(CMAKE) --build --preset default --target cppcheck

# Library and tests with AddressSanitizer and UBSan; catches memory errors and
# undefined behaviour that static analysis cannot see
check-asan:
	@$(CMAKE) --preset asan >/dev/null && $(CMAKE) --build --preset asan && $(CTEST) --preset asan

# Line and branch coverage of the first-party sources from the test suite.
# Requires lcov (provides lcov/genhtml; uses gcov from the active toolchain).
coverage:
	@$(CMAKE) --preset coverage >/dev/null && $(CMAKE) --build --preset coverage --target coverage

# The libFuzzer harnesses (one per feature header: scalar, delta, vector) with
# fuzzer coverage and the FUZZ_SAN sanitizers, each run for FUZZ_TIME seconds.
# Requires a clang with libFuzzer: Apple clang lacks the runtime, so on macOS
# install LLVM via Homebrew and pass FUZZ_CC=/opt/homebrew/opt/llvm/bin/clang.
# AddressSanitizer's runtime deadlocks during init on some macOS toolchains;
# pass FUZZ_SAN=undefined to fuzz with UBSan only there. CI runs the full set
# on Linux.
FUZZ_CC ?= clang
FUZZ_TIME ?= 60
FUZZ_SAN ?= address,undefined
fuzz:
	@$(CMAKE) --preset fuzz -DCMAKE_C_COMPILER=$(FUZZ_CC) -DXTRACT_FUZZ_SANITIZERS=$(FUZZ_SAN) -DXTRACT_FUZZ_TIME=$(FUZZ_TIME) >/dev/null \
	&& $(CMAKE) --build --preset fuzz && $(CTEST) --preset fuzz

# Mutation testing with Mull (https://github.com/mull-project/mull). The
# library is built with the Mull IR plugin on top of the sanitizer flags, so
# a mutant that reads past an array is detected rather than surviving, and
# the test binary is run once per mutant. mull.yml at the root scopes the
# mutants to the first-party sources. MULL_THRESHOLD is the minimum score
# out of 100 for the target to succeed.
MULL_LLVM ?= 18
MULL_CC ?= clang-$(MULL_LLVM)
MULL_PLUGIN ?= /usr/lib/mull-ir-frontend-$(MULL_LLVM)
MULL_RUNNER ?= mull-runner-$(MULL_LLVM)
MULL_THRESHOLD ?= 84
MULL_WORKERS ?= 4

mutation:
	@$(CMAKE) --preset mutation -DCMAKE_C_COMPILER=$(MULL_CC) -DXTRACT_MUTATION_PLUGIN=$(MULL_PLUGIN) \
		-DXTRACT_MULL_RUNNER=$(MULL_RUNNER) -DXTRACT_MUTATION_THRESHOLD=$(MULL_THRESHOLD) -DXTRACT_MUTATION_WORKERS=$(MULL_WORKERS) >/dev/null \
	&& $(CMAKE) --build --preset mutation && $(CMAKE) --build --preset mutation --target mutation

# Bounded model checking of every feature function with CBMC: one proof per
# function over every input of length up to VERIFY_MAX_N (see verify/Makefile).
# Requires cbmc and goto-cc. The proofs are independent, so `make -j verify`
# runs them in parallel; each needs up to a few GB of memory.
verify:
	@$(MAKE) -C verify

# clang-format over the first-party C sources (third-party, SWIG bindings and
# the C++ examples are excluded). The pinned CLANG_FORMAT version must match
# the one CI installs so a local `make format` and the enforced
# `make format-check` agree byte-for-byte.
CLANG_FORMAT ?= clang-format
FORMAT_FILES := $(shell git ls-files '*.c' '*.h' \
	':!:src/ooura/**' ':!:src/c-ringbuf/**' ':!:src/dywapitchtrack/**' \
	':!:swig/**' ':!:examples/**' \
	':!:tests/utest.h' ':!:bench/ubench.h')

format:
	@$(CLANG_FORMAT) -i $(FORMAT_FILES)

format-check:
	@$(CLANG_FORMAT) --dry-run --Werror $(FORMAT_FILES)

bench:
	@$(CMAKE) --preset default >/dev/null && $(CMAKE) --build --preset default --target bench

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
	@$(MAKE) -C verify clean
	@$(RM) -r dist build

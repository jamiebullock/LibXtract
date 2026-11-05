# Part of LibXtract
#
# SPDX-FileCopyrightText: 2026 Jamie Bullock
# SPDX-License-Identifier: Zlib

# Bounded model checking of the feature functions with CBMC. The library and
# the harnesses are compiled once into a goto binary; every function named
# verify_* in it is a proof, run as a separate CBMC invocation so that `make
# -j` runs them in parallel. A proof covers every input vector of length 0 to
# VERIFY_MAX_N, every argv value and a missing argv, and reports any out of
# bounds access, invalid pointer, integer division by zero, signed overflow,
# or loop that the bound cannot unwind. NaN and infinity are valid inputs and
# are not checked for. The portable (USE_OOURA) code path is checked on every
# host; the OOURA transform itself is replaced by fft_stub.c, which fills the
# buffer with arbitrary values, so the transform-based proofs cover the
# library's handling of any transform output and say nothing about the
# transform.
# math_stub.c does the same for pow, whose CBMC model needs a floating-point
# remainder the SMT backend cannot encode, and cbrt, which CBMC has no model
# of: both return any value.
# Requires cbmc, goto-cc and goto-instrument, and bitwuzla on the PATH.
CBMC ?= cbmc
GOTO_CC ?= goto-cc
GOTO_INSTRUMENT ?= goto-instrument
VERIFY_MAX_N ?= 32
# The proofs in SMALL_PROOFS run at this input bound, and each such harness
# draws its N from nondet_small_length(). Formula slicing cannot drop a
# floating-point sum that a branch tests (a loudness total, a filter's log
# floor, a band's zero amplitude), and the solver's cost in those sums grows
# too fast for VERIFY_MAX_N to be reachable.
VERIFY_SMALL_N ?= 8
SMALL_PROOFS = verify_hps verify_mcleod_f0 verify_sharpness \
	verify_tristimulus_1 verify_tristimulus_2 verify_tristimulus_3 \
	verify_mel_spectrogram verify_mfcc verify_gammatone_spectrogram verify_gfcc \
	verify_mmbses verify_spectral_subband_centroids
# The filterbank proofs take a filterbank of up to this many filters; the
# count multiplies the formula, and N is the bound the proofs are about.
VERIFY_MAX_FILTERS ?= 2

GOTO_BINARY = xtract.gb
PROOF_DIR = .proofs

LIB_SRC := $(filter-out ../src/ooura/fftsg.c, \
	$(wildcard ../src/*.c ../src/ooura/*.c ../src/c-ringbuf/*.c ../src/dywapitchtrack/*.c))
HARNESS_SRC := $(wildcard verify_*.c) fft_stub.c math_stub.c
HEADERS := verify.h $(wildcard ../include/xtract/*.h ../src/*.h ../src/ooura/*.h \
	../src/c-ringbuf/*.h ../src/dywapitchtrack/*.h)

CFLAGS = -U__APPLE__ -DUSE_OOURA -DVERIFY_MAX_N=$(VERIFY_MAX_N) -DVERIFY_SMALL_N=$(VERIFY_SMALL_N) \
	-DVERIFY_MAX_FILTERS=$(VERIFY_MAX_FILTERS) -std=c99 -I../include -I../src

# Every loop may run once per input element, and the bark band loop runs
# XTRACT_BARK_BANDS times whatever N is, so the bound is the larger of the two
# plus one for the exit check.
UNWIND := $(shell expr $(VERIFY_MAX_N) + 1)
ifeq ($(shell test $(UNWIND) -lt 27 && echo low), low)
    UNWIND := 27
endif
SMALL_UNWIND := $(shell expr $(VERIFY_SMALL_N) + 1)
ifeq ($(shell test $(SMALL_UNWIND) -lt 27 && echo low), low)
    SMALL_UNWIND := 27
endif
$(addprefix $(PROOF_DIR)/, $(addsuffix .proved, $(SMALL_PROOFS))): UNWIND = $(SMALL_UNWIND)

# The feature functions are floating-point loops, and the solver cost is in
# the doubles. Formula slicing drops the arithmetic that no property depends
# on, and Bitwuzla decides what remains in a fraction of the time and memory
# of the built-in SAT solver. CBMC_SOLVER= (empty) uses the built-in solver.
CBMC_SOLVER ?= --bitwuzla
CBMC_FLAGS = --unwind $(UNWIND) --unwinding-assertions --slice-formula $(CBMC_SOLVER) \
	--bounds-check --pointer-check --div-by-zero-check --signed-overflow-check \
	--malloc-may-fail --malloc-fail-null --drop-unused-functions

# The stamps hold the flags and the source list each product was made with, so
# that a changed bound, solver or source set rebuilds the binary or reruns the
# proofs.
CFLAGS_STAMP = $(PROOF_DIR)/cflags
CBMC_FLAGS_STAMP = $(PROOF_DIR)/cbmc_flags
HARNESS_LIST = $(PROOF_DIR)/harnesses

all: $(HARNESS_LIST)
	@$(MAKE) proofs HARNESSES="$$(tr '\n' ' ' < $(HARNESS_LIST))"

$(CFLAGS_STAMP): force
	@mkdir -p $(PROOF_DIR)
	@echo '$(CFLAGS) $(LIB_SRC) $(HARNESS_SRC)' > $@.new
	@if cmp -s $@ $@.new; then $(RM) $@.new; else mv $@.new $@; fi

$(CBMC_FLAGS_STAMP): force
	@mkdir -p $(PROOF_DIR)
	@echo '$(UNWIND) $(SMALL_UNWIND) $(CBMC_FLAGS)' > $@.new
	@if cmp -s $@ $@.new; then $(RM) $@.new; else mv $@.new $@; fi

$(GOTO_BINARY): $(LIB_SRC) $(HARNESS_SRC) $(HEADERS) $(CFLAGS_STAMP)
	$(GOTO_CC) $(CFLAGS) -o $@ $(LIB_SRC) $(HARNESS_SRC)

$(HARNESS_LIST): $(GOTO_BINARY)
	@$(GOTO_INSTRUMENT) --list-goto-functions $(GOTO_BINARY) > $@.functions
	@awk '/^verify_/ { print $$1 }' $@.functions > $@
	@test -s $@ || { echo "no verify_ function in $(GOTO_BINARY)"; $(RM) $@; exit 1; }

proofs: $(addprefix $(PROOF_DIR)/, $(addsuffix .proved, $(HARNESSES)))
	@test -n "$(HARNESSES)" || { echo "no harness given"; exit 1; }
	@echo "$(words $(HARNESSES)) proofs passed"

$(PROOF_DIR)/%.proved: $(GOTO_BINARY) $(CBMC_FLAGS_STAMP)
	@mkdir -p $(PROOF_DIR)
	@if $(CBMC) $(GOTO_BINARY) --function $* $(CBMC_FLAGS) > $(PROOF_DIR)/$*.log 2>&1; then \
		echo "$*: proved"; touch $@; \
	else \
		echo "$*: FAILED (verify/$(PROOF_DIR)/$*.log)"; \
		grep -E "FAILURE|unwinding assertion|^\*\*" $(PROOF_DIR)/$*.log | head -20; \
		exit 1; \
	fi

clean:
	@$(RM) -r $(GOTO_BINARY) $(PROOF_DIR)

.PHONY: all proofs clean force

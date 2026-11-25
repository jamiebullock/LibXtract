# Bounded model checking

`make -C verify` proves, with [CBMC](https://www.cprover.org/cbmc/), that no feature function has undefined behaviour on any input of up to `VERIFY_MAX_N` elements. The sanitizer, fuzz and mutation jobs sample the functions at realistic sizes. These proofs cover every input in their range.

## What a proof covers

Each function named `verify_*` in `verify_scalar.c`, `verify_vector.c`, `verify_delta.c`, `verify_multivariate.c` and `verify_stateful.c` is one proof, run as its own CBMC invocation (`make -j` runs them in parallel). A harness calls one feature function with:

- an input vector of any length from 0 to the proof's bound, with any values, allocated at exactly that length so a read past it is out of bounds;
- an argv that is either NULL or a buffer of any values, so both the missing and the supplied cases are covered; where argv selects the result length (`lpcc`'s order) that field is bounded to the input bound, since the harness must allocate the result;
- a result buffer of exactly the length the header documents for that call;
- for the features that take scratch, a context that either gives it, as an allocation of exactly the size asked for, or refuses;
- for the filterbank features, a filterbank of up to `VERIFY_MAX_FILTERS` filters, built for a transform of any even size from 2 to twice the proof's bound, whose gains and cosine table are allocated at exactly their sizes, the cosine table possibly NULL.

Two functions have no proof, each because the formula outgrows both solvers before N reaches 8:

- `xtract_subbands` reduces each band by calling through the `xtract[]` table, so its proof is the union of a proof for every permitted feature, once per band. Restricting the call site to the twelve permitted features with `goto-instrument --restrict-function-pointer` and bounding the band count to two was not enough.
- `xtract_features_from_subframes` dispatches through the same table with nearly every feature permitted.

The unit tests, built with the address and undefined behaviour sanitizers, remain the evidence for those two, and the fuzz job covers `xtract_subbands`. `xtract_features_from_subframes` has no fuzz harness.

CBMC then checks every array bound, pointer dereference, integer division, signed overflow and loop bound on every path. `malloc` may return NULL. NaN and infinity are valid inputs, so floating-point results are not checked. A passing proof means the function returns a status code for every such input without reading or writing outside its buffers.

## What is stubbed

The portable `USE_OOURA` code path is checked on every host (`__APPLE__` is undefined for the build, so the vDSP paths are not covered). The transform is replaced by a stub that fills the buffer with arbitrary values: the spectrum proofs cover the library's handling of any transform output and say nothing about the transform. The features reach it through the context, whose stub is in `context_stub.c`. `fft_stub.c` stands in for the OOURA functions that the context itself calls.

CBMC models the C maths library itself, and reports a call to any function it has no model of as a failure. A passing proof therefore depends on no unmodelled code. Nine functions are the exception, replaced in `math_stub.c`. `pow`, `cbrt`, `atan2`, `log1p`, `hypot`, `frexp` and `ldexp` return any value, and `frexp` writes any exponent, because CBMC's own `pow` model needs a floating-point remainder the SMT backend cannot encode and the other six have no model. A stub that returns any value covers every result the real function could give, which keeps the proofs sound, and they say nothing about what these seven functions return. `sqrt` has a model, which states its result with assumptions on two products of doubles. Formula slicing cannot drop an assumption, and in a loop that takes a square root per element those products are most of the formula. The library is therefore compiled with `sqrt` renamed to `stub_sqrt`, which calls `sqrt` unless the harness has set `stub_any_sqrt` and returns any value when it has. The proofs of `xtract_magnitude_spectrum`, `xtract_failsafe_f0`, `xtract_principal_variances` and `xtract_cooccurrence_correlation` set it: each is valid for every value a square root returns, and the flag cuts its solver memory by a factor of six or more. `__builtin_isfinite`, which glibc's `isfinite` expands to, has no model either and is implemented with CBMC's own finiteness test, so it behaves exactly as the real function. The library's own sort is replaced as well, in `sort_stub.c`, by a function that fills the buffer with arbitrary values, which covers every order the sort could leave it in. The proofs of the features that sort therefore say nothing about the sort, and do not pay for it. `verify_sort.c` compiles the sort itself under another name and proves it on its own, for every array of up to `VERIFY_MAX_N` values.

The context is replaced in the same way. In `context_stub.c` a take of scratch is an allocation of exactly the size asked for, which may fail, and giving it back frees it. A proof of a function that takes scratch therefore reports a write one element past what the function took, which the context's own single array would hide, and covers the context refusing. `verify_context.c` compiles the context itself under other names and proves it on its own: creating one of any size, and takes of any sizes for calls of any lengths, given back in order.

## Bounds

`VERIFY_MAX_N` (default 32) is the input length bound. The proofs listed in `SMALL_PROOFS` in the Makefile run at `VERIFY_SMALL_N` (default 8) instead. Formula slicing cannot drop a floating-point value that a branch tests, such as `sharpness`'s loudness total, the filterbank features' log floor or the comparisons that order the deque behind `moving_max` and `moving_min` or the elements of a window in `permutation_entropy`, or the threshold test on every element that `lempel_ziv_complexity` compares, or the tolerance test on every pair of elements that `sample_entropy` and `approximate_entropy` compare, or the sign test on every lag of the normalised square difference function that `mcleod_f0`, `periodicity` and `signal_confidence` scan for key maxima, or the band edge test on every bin frequency in `spectral_contrast`, or the range test and pitch class of every bin frequency in `chroma`, and the solver's cost in those values grows too fast for the full bound to be reachable. `covariance_matrix` runs at the small bound for its loops: one over the elements of a series inside two over the series, which unwind to the cube of the bound. The same effect decides whether a proof finishes at all: `xtract_mmbses` writes each band's result once, after the branch on the covariance determinant, so the determinant and the products behind it drop out of the formula; a write inside that branch keeps them in, and the proof does not finish. `VERIFY_MAX_FILTERS` (default 2) bounds the filterbank size, which multiplies the formula without bearing on the per-element code the proofs are about. `VERIFY_MAX_SWEEPS` (default 2) replaces the sweep limit of `principal_variances`, whose proof takes a matrix of up to three rows: every sweep makes the same accesses to a matrix of arbitrary values, so two sweeps cover any number of them, and with the function's own limit the proof does not finish. The harness of `coherence` sets its own bounds as well: an input of up to twelve elements and a transform of two or four, which reaches two segments at both sizes, each size in a call of its own so that the segment length is a constant in the loops it bounds. `cooccurrence_matrix` has two proofs, both with up to two levels: one of a grid of six elements as two rows of three and as three rows of two, each in a call of its own so that the width is a constant, and one of a grid of up to four elements with any width. `local_binary_patterns` has two in the same way: one of three rows of three, three rows of four and four rows of three, and one of a grid of up to eight elements with any width, which is too few for an element with a neighbour on every side. A transform of eight takes the solver past 7 GB of memory. The kernel `cqt` is given has `VERIFY_MAX_BANDS` (default 2) bands of up to `VERIFY_MAX_BAND_ENTRIES` (default 1) entries each, both set in the harness: a band count the solver has to choose, or more bands with chosen entry counts, multiplies the formula until the proof does not finish, while this kernel proves at the full bound. The loop unwinding bound is the proof's input bound plus one; for the full bound it has a floor of 27, because the bark band loop runs `XTRACT_BARK_BANDS` times whatever N is, and no small-bound proof reaches that loop.

The solver cost is in the doubles: every floating-point operation is bit-blasted, which makes the quadratic loops cost far more than the linear ones. Two settings keep them tractable. Formula slicing (`--slice-formula`) removes the arithmetic no property depends on, and Bitwuzla (`--bitwuzla`) decides what remains in a fraction of the time and memory of the built-in SAT solver. `make -C verify CBMC_SOLVER=` runs the built-in solver instead.

## Running

Requires `cbmc` and `goto-cc` (Homebrew `cbmc`, or on Ubuntu the `.deb` from the CBMC releases page) and `bitwuzla` on the PATH (Homebrew `bitwuzla`, or on Linux the static binary from the Bitwuzla releases page). The proofs of `higuchi_fractal_dimension` and `last_n` need about 7 GB of memory each. Every other proof needs less than 4 GB. This runs them one at a time:

```
make -C verify
```

On a machine with memory for more, `-j` runs several at once. CI has 16 GB, and runs the two large proofs alone and then the rest three at a time:

```
make -C verify .proofs/verify_higuchi_fractal_dimension.proved
make -C verify .proofs/verify_last_n.proved
make -C verify -j3
```

The build of the binary the proofs run on, `make -C verify xtract.gb`, takes a few seconds and needs no solver. It fails when the compiler warns, as it does for a call to a function that nothing declares, and when the binary calls a function that this repository declares and that has no body. CI runs it on every pull request, and the proofs on every push to main and develop.

Logs are written to `verify/.proofs/<proof>.log`; a proof that passes leaves `verify/.proofs/<proof>.proved`, and a later `make -C verify` reruns only the proofs whose inputs changed, where a changed bound or solver setting counts as a change to every proof.

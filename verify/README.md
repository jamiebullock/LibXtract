# Bounded model checking

`make -C verify` proves, with [CBMC](https://www.cprover.org/cbmc/), that no feature function has undefined behaviour on any input of up to `VERIFY_MAX_N` elements. The sanitizer, fuzz and mutation jobs sample the functions at realistic sizes; these proofs cover every input in their range.

## What a proof covers

Each function named `verify_*` in `verify_scalar.c`, `verify_vector.c`, `verify_delta.c` and `verify_stateful.c` is one proof, run as its own CBMC invocation (`make -j` runs them in parallel). A harness calls one feature function with:

- an input vector of any length from 0 to the proof's bound, holding any values, allocated at exactly that length so a read past it is out of bounds;
- an argv that is either NULL or a buffer of any values, so both the missing and the supplied cases are covered; where argv selects the result length (`lpcc`'s order) that field is bounded to the input bound, since the harness must allocate the result;
- a result buffer of exactly the length the header documents for that call;
- for the FFT features, a transform state that is either uninitialised or initialised for any power of two up to the bound;
- for the filterbank features, a filterbank of up to `VERIFY_MAX_FILTERS` filters whose rows are exactly as wide as the input, with any row, the row table or the DCT table possibly NULL.

Four functions have no proof, each because the formula outgrows both solvers before N reaches 8:

- `xtract_subbands` reduces each band by calling through the `xtract[]` table, so its proof is the union of a proof for every permitted feature, once per band. Restricting the call site to the twelve permitted features with `goto-instrument --restrict-function-pointer` and bounding the band count to two was not enough.
- `xtract_features_from_subframes` dispatches through the same table with nearly every feature permitted.
- `xtract_wavelet_f0` runs the bundled dywapitchtrack pitch tracker, whose loop counts do not follow N.
- `xtract_spectrum` tracks the running maximum of every coefficient for its normalisation, a branch on every element that keeps the square roots and logarithms in the formula; `xtract_autocorrelation_fft`, which shares the transform state and the stub, is proved.

The unit tests, built with the address and undefined behaviour sanitizers, remain the evidence for those four, and the fuzz jobs cover the three that are feature functions; `xtract_features_from_subframes` has no fuzz harness.

CBMC then checks every array bound, pointer dereference, integer division, signed overflow and loop bound on every path. `malloc` may return NULL. NaN and infinity are valid inputs, so floating-point results are not checked. A passing proof means the function returns a status code for every such input without reading or writing outside its buffers.

## What is stubbed

The portable `USE_OOURA` code path is checked on every host (`__APPLE__` is undefined for the build, so the vDSP paths are not covered). The OOURA transform itself is replaced by `fft_stub.c`, whose `rdft` fills the buffer with arbitrary values: the spectrum proofs cover the library's handling of any transform output and say nothing about the transform.

CBMC models the C maths library itself, and reports a call to any function it has no model of as a failure, so a passing proof depends on no unmodelled code. Three functions are the exception, replaced in `math_stub.c`. `pow` and `cbrt` return any value, because CBMC's own `pow` model needs a floating-point remainder the SMT backend cannot encode and `cbrt` has no model; a stub that returns any value covers every result the real function could give, so the proofs stay sound, and they say nothing about what `pow` or `cbrt` return. `__builtin_isfinite`, which glibc's `isfinite` expands to, has no model either and is implemented with CBMC's own finiteness test, so it behaves exactly as the real function. `qsort` has no model either and is replaced in `stdlib_stub.c`: the stub calls the comparator on each adjacent pair of elements, so the comparator is checked on arbitrary values, and then fills the buffer with arbitrary values, which covers every order the real sort could leave it in; the proofs of the features that sort say nothing about the sort.

## Bounds

`VERIFY_MAX_N` (default 32) is the input length bound. The proofs listed in `SMALL_PROOFS` in the Makefile run at `VERIFY_SMALL_N` (default 8) instead. Formula slicing cannot drop a floating-point value that a branch tests, such as `sharpness`'s loudness total, the filterbank features' log floor or the comparisons that order the deque behind `moving_max` and `moving_min` or the elements of a window in `permutation_entropy`, or the threshold test on every element that `lempel_ziv_complexity` compares, or the tolerance test on every pair of elements that `sample_entropy` and `approximate_entropy` compare, or the sign test on every lag of the normalised square difference function that `mcleod_f0`, `periodicity` and `signal_confidence` scan for key maxima, or the band edge test on every bin frequency in `spectral_contrast`, or the range test and pitch class of every bin frequency in `chroma`, and the solver's cost in those values grows too fast for the full bound to be reachable. The same effect decides whether a proof finishes at all: `xtract_mmbses` writes each band's result once, after the branch on the covariance determinant, so the determinant and the products behind it drop out of the formula; a write inside that branch keeps them in, and the proof does not finish. `VERIFY_MAX_FILTERS` (default 2) bounds the filterbank size, which multiplies the formula without bearing on the per-element code the proofs are about. The kernel `cqt` is given has `VERIFY_MAX_BANDS` (default 2) bands of up to `VERIFY_MAX_BAND_ENTRIES` (default 1) entries each, both set in the harness: a band count the solver has to choose, or more bands with chosen entry counts, multiplies the formula until the proof does not finish, while this kernel proves at the full bound. The loop unwinding bound is the proof's input bound plus one; for the full bound it has a floor of 27, because the bark band loop runs `XTRACT_BARK_BANDS` times whatever N is, and no small-bound proof reaches that loop.

The solver cost is in the doubles: every floating-point operation is bit-blasted, so the quadratic loops cost far more than the linear ones. Two settings keep them tractable. Formula slicing (`--slice-formula`) removes the arithmetic no property depends on, and Bitwuzla (`--bitwuzla`) decides what remains in a fraction of the time and memory of the built-in SAT solver. `make -C verify CBMC_SOLVER=` runs the built-in solver instead.

## Running

Requires `cbmc` and `goto-cc` (Homebrew `cbmc`; on Ubuntu the `.deb` from the CBMC releases page) and `bitwuzla` on the PATH (Homebrew `bitwuzla`; on Linux the static binary from the Bitwuzla releases page). A proof needs up to 7 GB of memory, so run them one at a time unless the machine has memory for more (`make -C verify -j2`):

```
make -C verify
```

Logs are written to `verify/.proofs/<proof>.log`; a proof that passes leaves `verify/.proofs/<proof>.proved`, and a later `make -C verify` reruns only the proofs whose inputs changed, where a changed bound or solver setting counts as a change to every proof.

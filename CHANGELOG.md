# Changelog

The notes of each LibXtract release, newest first. Version numbers follow [Semantic Versioning](https://semver.org); the [Versioning](README.md#versioning) section of the README states what is kept compatible between releases.

## 1.3.0 - 2026-11-10

LibXtract 1.3.0 replaces the Make build with CMake. One CMake project builds the library, the tests, the examples, the benchmarks, the fuzz harnesses and the language bindings; the install provides a CMake package and a pkg-config file; and Windows builds with MSVC without a POSIX layer. The C API is the same as in 1.2.0: no function, enumeration value or layout is added, removed or renumbered, and no feature's result changes.

### Build

- CMake 3.21 or later builds the library. The Makefiles are removed, so `make`, `make check` and `make install` are replaced by `cmake --preset default`, `cmake --build --preset default`, `ctest --preset default` and `cmake --install build/default`.
- `CMakePresets.json` defines the `default`, `asan`, `coverage`, `fuzz` and `mutation` presets, each with its own directory under `build/`. The default preset has the tool targets `analyze`, `cppcheck`, `format`, `format-check`, `bench` and `doc`.
- On Windows the default preset uses the Visual Studio generator, and Ninja from a Developer Command Prompt also works. MSYS2 is no longer needed.
- The build gives a static library by default and a shared library with `BUILD_SHARED_LIBS=ON`.
- The install places the library, the headers under `include/xtract/`, the CMake package and `xtract.pc` under the prefix. A CMake project calls `find_package(xtract)` and links `xtract::xtract`; any other build uses `pkg-config --cflags --libs xtract`.
- The Python, Java and Node.js bindings are built by CMake and need SWIG 4.2 or later. `XTRACT_BUILD_PYTHON`, `XTRACT_BUILD_JAVA` and `XTRACT_BUILD_NODE` are `AUTO` by default, which builds each binding whose toolchain is found; `ON` fails the configure when the toolchain is missing and `OFF` skips the binding. Each binding's test runs under CTest.
- The CBMC proofs keep their own GNU make build and are run with `make -C verify`, which replaces `make verify`.

### Shared library versioning

- On Linux and macOS the soname of the shared library carries the major and minor version: `libxtract.so.1.3` and `libxtract.1.3.dylib`. A program linked against the 1.3 shared library loads any 1.3.x release and no other minor release until it is recompiled and relinked. Releases up to 1.2.0 built the shared library with no version in its name.
- The name of the Windows DLL carries no version.

### Library

- The library no longer runs a constructor when it is loaded or a destructor at exit. The FFT state is zero-initialised thread-local storage, so initialisation is unchanged. Memory that `xtract_init_fft()` allocated on a thread still running at process exit is reclaimed with the process, and a thread that ends before the process must call `xtract_free_fft()` first, as the function's documentation now states.

### Examples and documentation

- `examples/finance/volatility.c` is the first example outside audio: it turns a daily price series into log returns and reports rolling and exponentially weighted volatility and the day a change of volatility regime is detected. It runs on a built-in synthetic series or on a CSV file, and `examples/finance/README.md` explains each step. `examples/README.md` lists the examples.
- The README describes the library as a feature extraction library for time series and other numeric data, with audio as one application. It gains a summary of the features by group and a "Using the library" section with the shared function signature, and its "Versioning" section states the compatibility policy in one paragraph.
- The code standard moves from the README to `CONTRIBUTING.md`.
- The main page of the API reference opens with the same description as the README.
- `CHANGELOG.md` holds the notes of every release from 1.1.0.

### Tooling

- CI builds, tests and installs with CMake on Linux, macOS and Windows with MSVC, builds and tests the three bindings on each, and builds a consumer project against the installed package.
- The sanitizer, coverage, fuzz, mutation, static analysis and format checks run from the CMake presets and targets, in CI and locally.
- A workflow tags a release `vX.Y.Z`, from the `VERSION` file, when `main` moves to it.

## 1.2.0 - 2026-11-09

LibXtract 1.2.0 adds 37 feature functions to the C API and implements two that were declared but not written, every one registered in the feature enumeration with a descriptor, tests, a fuzz harness and a CBMC proof, and makes the return code of every feature function part of the contract. Every change is additive: no function, enumeration value or layout is removed or renumbered, and `XTRACT_FEATURE_COUNT` grows to cover the new entries.

### API additions

Statistics and complexity of an arbitrary series (scalar, no sample rate):

- `xtract_median` and `xtract_median_absolute_deviation`, the latter taking the median in argv as `xtract_variance` takes the mean. NaN elements are ordered after every number.
- `xtract_hjorth_activity`, `xtract_hjorth_mobility` and `xtract_hjorth_complexity`.
- `xtract_time_reversal_asymmetry`, with the lag as an int in argv.
- `xtract_petrosian_fractal_dimension`, `xtract_katz_fractal_dimension` and `xtract_higuchi_fractal_dimension`, the last taking k_max in argv.
- `xtract_permutation_entropy` (order 2 to 8 and delay in argv) and `xtract_lempel_ziv_complexity` (a threshold in argv, the median by default), both normalised so that a random series reads about 1, and `xtract_sample_entropy` and `xtract_approximate_entropy` (template length and tolerance in argv), in nats.
- `xtract_periodicity` and `xtract_signal_confidence`, from the normalised square difference function that `xtract_mcleod_f0` computes: how strongly a series repeats, in [0, 1], and the height of the peak the f0 estimate was taken from. Neither takes an argument.

Envelope and transient shape of a series with a single peak:

- `xtract_attack_time` and `xtract_decay_time`, declared since 1.0 and returning `XTRACT_FEATURE_NOT_IMPLEMENTED`, are implemented. Attack time takes the sample rate and start and stop fractions in argv (defaults 0.2 and 0.9); decay time takes the sample rate and a threshold fraction (default 0.4). Both are in seconds, and `XTRACT_SECONDS` joins the unit enumeration.
- `xtract_decay_rate`, the least-squares slope of the log envelope after the peak, in `XTRACT_PER_SECOND`.
- `xtract_temporal_centroid`, `xtract_max_position`, `xtract_min_position` and `xtract_peak_asymmetry`, which take no argument and report positions as fractions of the length.

Moving-window and exponentially weighted features (vector, one value per window position):

- `xtract_moving_sum`, `xtract_moving_average`, `xtract_moving_variance` and `xtract_moving_standard_deviation`, with the window length as an int in argv, writing N - window + 1 values in O(N) whatever the window length. The variance uses Bessel's correction as `xtract_variance` does.
- `xtract_moving_max` and `xtract_moving_min`, by a monotonic deque, also O(N).
- `xtract_ewma_average` and `xtract_ewma_variance`, with the smoothing factor in argv, writing one value per input element.

Two-frame measures on the `xtract_flux` layout (the current frame in the first N/2 elements, the previous in the second):

- `xtract_cross_correlation`, N - 1 lags with lag 0 at index N/2 - 1 and a positive lag meaning the first series is delayed, as numpy, scipy, MATLAB and R define it; argv selects the biased sum or the correlation coefficient in [-1, 1]. `xtract_cross_correlation_lag` returns the lag of the largest coefficient.
- `xtract_kullback_leibler_divergence`, `xtract_itakura_saito_divergence` and `xtract_modified_kullback_leibler`, for onset and change detection alongside `xtract_flux`.

Spectral features on the standard spectral layout (N/2 coefficients followed by N/2 bin frequencies):

- `xtract_spectral_entropy`, the normalised Shannon entropy of a spectrum, in [0, 1].
- `xtract_spectral_contrast`, the octave-band contrast of Jiang et al. (2002), with the band count, lowest band edge and averaged fraction in argv, giving bands + 1 values.
- `xtract_chroma`, the twelve-element pitch class profile with C first, scaled so the largest class is 1, with the A4 frequency and the lowest and highest bin frequencies in argv. `XTRACT_PITCH_CLASSES` names the length and `XTRACT_CHROMA_COEFFS` the vector format.
- `xtract_init_cqt`, `xtract_free_cqt` and `xtract_cqt`: the constant-Q transform of Brown and Puckette (1992) as a kernel built once and applied to each magnitude/phase spectrum, giving band magnitudes and centre frequencies on the spectral layout, which `xtract_chroma` and `xtract_spectral_contrast` accept. `XTRACT_CQT_KERNEL` is the argv type and `XTRACT_INIT_CQT` the donor.

Registration:

- `xtract_peak`, `xtract_mmbses` and `xtract_spectral_subband_centroids` now have entries in `xtract_features_` and the dispatch table with full descriptors, so every feature function can be reached by id. `xtract_spectral_subband_centroids` takes N as the length of its input array like every other feature.
- `XTRACT_SPECTRAL_MAGNITUDE_PHASE` names the magnitude/phase layout for descriptor consumers.

### Return code contract

The return code of every feature function is part of the API from this release, as the README's API stability section states:

- `XTRACT_ARGUMENT_ERROR` means the caller broke the function's documented contract: N below the function's minimum, an odd N on a two-half layout, a NULL argv where one is required, or an argv value outside its range. A scalar result is set to 0 and a vector result is left untouched.
- `XTRACT_NO_RESULT` means the arguments were valid but the data has no answer (an all-zero spectrum, a constant series where a ratio is needed); a scalar result is 0.
- `XTRACT_BAD_STATE` is returned by `xtract_spectrum` and `xtract_autocorrelation_fft` when the FFT was not initialised or was initialised for another size.
- `XTRACT_BAD_ARGV`, `XTRACT_BAD_VECTOR_SIZE` and `XTRACT_DENORMAL_FOUND` are deprecated and no longer returned; they stay in the enumeration so that no value changes.
- The five functions that read or wrote out of bounds, or divided by zero, on zero-length input (`xtract_highest_value`, `xtract_peak`, `xtract_lpc`, `xtract_lpcc`, `xtract_subbands`) reject it. The diagnostic prints to stderr are removed; the return code is the diagnostic.
- A table-driven conformance test runs every feature against its descriptor, so the argument bounds the descriptors state are the ones the code enforces.

### Changed results

- `xtract_spectrum` with `XTRACT_MAGNITUDE_PHASE_SPECTRUM`: on Linux and Windows the phase changes sign. The OOURA transform returns the conjugate of the vDSP convention, so those builds reported every phase negated relative to macOS; the phase now has the same sign on every platform, with a cosine centred on a bin at 0 and a sine at -pi/2. `xtract_mmbses` is unaffected.
- `xtract_mcleod_f0`: the key maximum scan stops at lag N / 2, the half-window rule of McLeod and Wyvill. The function no longer reports a period longer than N / 2 samples and gives no result instead; white noise, which read a spurious key maximum near the last lag, now reads none.
- `xtract_flux` and `xtract_lnorm` with `XTRACT_NEGATIVE_SLOPE`: the filter now keeps only the negative differences, as `XTRACT_POSITIVE_SLOPE` keeps only the positive ones; it was previously ignored.
- `xtract_bark_coefficients`: the documented input is the first half of an `xtract_spectrum` result with DC included, so that element k is bin k, which is the grid the band limits from `xtract_init_bark` are on. Callers passing the default layout were summing every band one bin high and should switch.
- `xtract_features_from_subframes` accepts every scalar feature and the vector features whose result fits in a subframe, and rejects any other feature as an argument error.
- `xtract_failsafe_f0` returns `XTRACT_BAD_STATE` with the result untouched when its fallback needs an FFT that is not initialised for N on the calling thread; the FFT state is documented as belonging to the calling thread, and a second `xtract_init_fft` for a feature as replacing its FFT.

### Build and tooling

- `make verify` runs a CBMC bounded model checking harness for every feature function: nondeterministic data and arguments at every length up to the bound, with bounds, pointer, division, overflow and unwinding checks. CI runs the proofs on every push to main and develop and opens an issue on a failure. `verify/README.md` documents the bounds, the stubs and the functions without a proof.
- CI and CodeRabbit run on pull requests to both branches; the Doxygen site deploys from main.
- The CI tool downloads fail on an HTTP error, retry and are checked against pinned SHA-256 digests before they are installed; no job persists the workflow token; the mutation tests run with a cleared environment.

## 1.1.0 - 2026-09-30

LibXtract 1.1.0 is a correctness release. Every feature function was audited, fuzzed and pinned to hand-derived values, and a mutation-testing gate now runs in CI. Several features return different values from 1.0.0 as a result; each is listed below. The library is now licensed under the zlib licence.

### Licence

The library is relicensed from MIT to zlib (SPDX identifier `Zlib`). Bundled third-party code keeps its own licence; `DEPENDENCIES.md` lists each component and any obligation that follows binaries.

### API additions

- `xtract_init_dct` and `xtract_free_dct` precompute the DCT cosine table used by `xtract_mfcc`, `xtract_gfcc` and `xtract_dct`.
- `xtract_mel_filter` has a new `dct_table` field. Zero-initialise the structure, or set every field, before passing it to a feature function: NULL means the cosines are evaluated on each call. Code that allocates the structure with `malloc` and sets only `n_filters` and `filters` must now set `dct_table` as well.
- `XTRACT_MAGNITUDE_PHASE_SPECTRUM` is a new `xtract_spectrum` type: bin magnitudes in the first half of the output, the corresponding phases in the second.
- `XTRACT_UNIT_NONE`, `XTRACT_UNIT_ANY`, `XTRACT_UNIT_UNKNOWN` and `XTRACT_ACUM` are added to the unit enumeration, and every descriptor now carries a result range and unit derived from its computation.
- `XTRACT_F0_PEAK_CLIP` and `XTRACT_F0_CENTRE_CLIP` name the clipping thresholds `xtract_f0` uses.
- Both `xtract_spectrum` output layouts are documented: by default coefficient m is bin m + 1 and the DC bin is discarded; with the DC flag set, coefficient m is bin m.

### Removed: XTRACT_SPECTRUM_COEFFICIENTS

`XTRACT_SPECTRUM_COEFFICIENTS` is removed and `xtract_mmbses` now takes an `XTRACT_MAGNITUDE_PHASE_SPECTRUM` of the documented length N. The removed type was the library's only interleaved-complex format and required 2N input doubles while its header documented N, so a caller following the documentation read past the end of its buffer. The API policy batches removals into 2.0; this one is made in 1.1.0 as a one-off exception because the removed value could not be used safely and 1.0.0 was tagged without a published release. Any other removal or renaming waits for 2.0 as the policy states.

### Changed results

Under the API policy a correctness fix may change a feature's output. The following features return different values from 1.0.0:

- `xtract_spectrum`, all types: on macOS the absolute scale is halved to match the DFT convention already produced on Linux and Windows. Normalised results are unaffected.
- `xtract_mfcc` and `xtract_mel_spectrogram`: the mel filterbank samples each triangle at the bin frequencies, following Rabiner and Juang, and `XTRACT_EQUAL_AREA` normalises each filter by its own bandwidth. Every MFCC and mel-spectrogram value changes; `XTRACT_EQUAL_GAIN` filter shapes are unchanged apart from the sampling.
- `xtract_hps`: harmonics are indexed by the spectrum's bin layout, scored in the log domain and interpolated, and the harmonic count is settable through argv (2 to 16, default 3). Results on a default-layout spectrum move down by one bin.
- `xtract_lpcc`: coefficients beyond the LPC order were computed from one term too few and change.
- `xtract_mmbses`: uses the Gaussian entropy constant and consistent second moments from the cited formula, and takes the new input described above.
- `xtract_sharpness`: loudness-invariant and calibrated in acums, so values change substantially. `xtract_odd_even_ratio` changes only for inputs with sub-fundamental content.
- `xtract_init_bark`: band limits are rounded rather than truncated, which shifts some edges by one bin and slightly changes `xtract_bark_coefficients`, `xtract_loudness` and `xtract_sharpness` output for some sample rate and block size combinations.
- `xtract_mcleod_f0`: selects the key maximum of each NSDF region as McLeod and Wyvill specify, compared at sub-sample accuracy.
- `xtract_autocorrelation_fft`: both FFT backends are normalised to agree with `xtract_autocorrelation`; the vDSP path previously read and wrote past the caller's buffers.
- `xtract_spectral_subband_centroids`: reads the documented spectrum layout and weights numerator and denominator consistently.
- `xtract_subbands`: the final band is no longer zeroed when the bands tile the input exactly.
- `xtract_smoothed`: the last output element no longer depends on uninitialised memory.
- `xtract_flatness`: computed in the log domain, so it no longer underflows to zero on long inputs.
- `xtract_lpcc` reads its argv as an int, as the descriptor always did; the header previously documented a double.
- Edge cases: `xtract_harmonic_spectrum` and `xtract_hps` return `XTRACT_NO_RESULT` for a zero fundamental or an all-zero spectrum; `xtract_midicent` returns `XTRACT_ARGUMENT_ERROR` for a non-positive fundamental; `xtract_lowest_value` gives 0 on `XTRACT_NO_RESULT`; `xtract_sharpness` clamps N to the Bark band count.

### Build and tooling

- The library is built as C99 and formatted with clang-format; both are checked in CI.
- CI runs the test suite under AddressSanitizer and UndefinedBehaviorSanitizer, the clang static analyzer, cppcheck, CodeQL, libFuzzer harnesses for every feature function, and Mull mutation testing gated at a minimum score.
- The test suite is written in C with utest.h; `make coverage` produces a gcov/lcov report.
- The example reads its input with dr_wav; the Visual Studio 2012 project is removed.
- A `CONTRIBUTING.md`, pull request template and `SECURITY.md` are added.

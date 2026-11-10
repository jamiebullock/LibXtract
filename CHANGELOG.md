# Changelog

All notable changes to LibXtract are documented in this file. The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/), and versions follow [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.3.0] - 2026-11-10

### Added

- CMake build (3.21 or later) with presets, an installed CMake package and `xtract.pc`
- MSVC build on Windows without MSYS2
- Shared library soname with major.minor on Linux and macOS (`libxtract.so.1.3`)
- Python, Java and Node.js bindings built by CMake (SWIG 4.2 or later)
- Finance example in `examples/finance`

### Changed

- `make verify` is now `make -C verify`
- Code standard moved from the README to `CONTRIBUTING.md`

### Removed

- Make build; see the README for the CMake commands
- Load-time constructor and exit-time destructor; call `xtract_free_fft()` on a thread that exits early

## [1.2.0] - 2026-11-09

### Added

- `xtract_median` and `xtract_median_absolute_deviation`
- `xtract_hjorth_activity`, `xtract_hjorth_mobility` and `xtract_hjorth_complexity`
- `xtract_time_reversal_asymmetry`
- `xtract_petrosian_fractal_dimension`, `xtract_katz_fractal_dimension` and `xtract_higuchi_fractal_dimension`
- `xtract_permutation_entropy` and `xtract_lempel_ziv_complexity`
- `xtract_sample_entropy` and `xtract_approximate_entropy`
- `xtract_periodicity` and `xtract_signal_confidence`
- `xtract_attack_time` and `xtract_decay_time`, declared since 1.0, are implemented
- `xtract_decay_rate`
- `xtract_temporal_centroid`, `xtract_max_position`, `xtract_min_position` and `xtract_peak_asymmetry`
- `xtract_moving_sum`, `xtract_moving_average`, `xtract_moving_variance` and `xtract_moving_standard_deviation`
- `xtract_moving_max` and `xtract_moving_min`
- `xtract_ewma_average` and `xtract_ewma_variance`
- `xtract_cross_correlation` and `xtract_cross_correlation_lag`
- `xtract_kullback_leibler_divergence`, `xtract_itakura_saito_divergence` and `xtract_modified_kullback_leibler`
- `xtract_spectral_entropy`, `xtract_spectral_contrast` and `xtract_chroma`
- Constant-Q transform: `xtract_init_cqt`, `xtract_free_cqt` and `xtract_cqt`
- Feature enumeration entries for `xtract_peak`, `xtract_mmbses` and `xtract_spectral_subband_centroids`
- Units `XTRACT_SECONDS` and `XTRACT_PER_SECOND`
- CBMC proofs of every feature function, run with `make verify`

### Changed

- Return codes are part of the API contract (#221)
- Feature functions return `XTRACT_ARGUMENT_ERROR` for a missing argv and an N below their minimum (#218, #219)
- `xtract_spectrum`: phase sign on Linux and Windows now matches macOS (#246)
- `xtract_spectrum` and `xtract_autocorrelation_fft` return `XTRACT_BAD_STATE` when the FFT is not initialised for N (#219)
- `xtract_mcleod_f0`: no period longer than N / 2 samples is reported (#237)
- `xtract_flux` and `xtract_lnorm`: `XTRACT_NEGATIVE_SLOPE` keeps only negative differences; it was ignored (#241)
- `xtract_bark_coefficients`: documented input is a spectrum with DC included (#213)
- `xtract_features_from_subframes` accepts every scalar feature (#254)
- `xtract_failsafe_f0` returns `XTRACT_BAD_STATE` when its fallback FFT is not initialised (#252)
- `xtract_midicent` returns `XTRACT_NO_RESULT` for a zero fundamental (#218)
- `xtract_spectral_subband_centroids` takes N as the length of its input array (#220)

### Deprecated

- `XTRACT_BAD_ARGV`, `XTRACT_BAD_VECTOR_SIZE` and `XTRACT_DENORMAL_FOUND`, which are no longer returned (#218)

### Removed

- Diagnostic prints to stderr (#219)

### Fixed

- Out-of-bounds access on zero-length input in `xtract_highest_value`, `xtract_peak`, `xtract_lpc`, `xtract_lpcc` and `xtract_subbands` (#217)

## [1.1.0] - 2026-09-30

### Added

- `xtract_init_dct` and `xtract_free_dct` (#186)
- `dct_table` field in `xtract_mel_filter`; zero-initialise the structure or set every field (#187)
- `XTRACT_MAGNITUDE_PHASE_SPECTRUM` type for `xtract_spectrum` (#174)
- Result range and unit in every descriptor (#180)
- Units `XTRACT_UNIT_NONE`, `XTRACT_UNIT_ANY`, `XTRACT_UNIT_UNKNOWN` and `XTRACT_ACUM` (#180)
- `XTRACT_F0_PEAK_CLIP` and `XTRACT_F0_CENTRE_CLIP` (#181)
- Harmonic count argument for `xtract_hps`, 2 to 16 (#193)
- `CONTRIBUTING.md` and `SECURITY.md`

### Changed

- Licence changed from MIT to zlib (#189)
- Library is built as C99 (#147)
- `xtract_spectrum`: absolute scale halved on macOS to match Linux and Windows (#142)
- `xtract_mfcc` and `xtract_mel_spectrogram`: mel filterbank sampled at bin frequencies; every value changes (#204)
- `xtract_hps`: harmonics indexed by bin layout and scored in the log domain (#193)
- `xtract_lpcc`: coefficients beyond the LPC order corrected (#202)
- `xtract_lpcc`: argv is documented as an int, as the function reads it (#142)
- `xtract_mmbses`: takes a magnitude/phase spectrum of length N (#174)
- `xtract_sharpness`: loudness-invariant and in acums (#146)
- `xtract_odd_even_ratio`: sub-fundamental bins are excluded (#146)
- `xtract_init_bark`: band limits rounded, moving some edges by one bin (#178)
- `xtract_mcleod_f0`: selects the key maximum of each NSDF region (#145)
- `xtract_autocorrelation_fft`: both FFT backends match `xtract_autocorrelation` (#141)
- `xtract_spectral_subband_centroids`: reads the documented spectrum layout (#142)
- `xtract_harmonic_spectrum` returns `XTRACT_NO_RESULT` for a zero fundamental (#143)
- `xtract_hps` returns `XTRACT_NO_RESULT` for an all-zero spectrum (#143)
- `xtract_midicent` returns `XTRACT_ARGUMENT_ERROR` for a non-positive fundamental (#143)

### Removed

- `XTRACT_SPECTRUM_COEFFICIENTS`; use `XTRACT_MAGNITUDE_PHASE_SPECTRUM` (#174)
- Visual Studio 2012 project (#184)

### Fixed

- `xtract_autocorrelation_fft`: out-of-bounds read and write on macOS (#141)
- `xtract_smoothed`: last output element depended on uninitialised memory (#141)
- `xtract_subbands`: final band zeroed when the bands tile the input exactly (#141)
- `xtract_flatness`: underflow to zero on long inputs (#140)
- `xtract_sharpness`: out-of-range bands weighted when N exceeds the Bark band count (#143)
- `xtract_lowest_value`: result is 0 on `XTRACT_NO_RESULT` (#143)

[1.3.0]: https://github.com/jamiebullock/LibXtract/compare/v1.2.0...v1.3.0
[1.2.0]: https://github.com/jamiebullock/LibXtract/compare/v1.1.0...v1.2.0
[1.1.0]: https://github.com/jamiebullock/LibXtract/compare/v1.0.0...v1.1.0

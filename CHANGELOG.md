# Changelog

## [1.3.0] - 2026-11-10

### Changed

- Build the Python, Java and Node.js bindings with CMake, which needs SWIG 4.2 or later ([#258](https://github.com/jamiebullock/LibXtract/pull/258))
- Move the proofs from `make verify` to `make -C verify` ([#263](https://github.com/jamiebullock/LibXtract/pull/263))
- Move the code standard from the README to `CONTRIBUTING.md` ([#262](https://github.com/jamiebullock/LibXtract/pull/262))

### Added

- Add a CMake build (3.21 or later) with presets, an installed CMake package and `xtract.pc` ([#256](https://github.com/jamiebullock/LibXtract/pull/256), [#257](https://github.com/jamiebullock/LibXtract/pull/257))
- Add an MSVC build on Windows without MSYS2 ([#256](https://github.com/jamiebullock/LibXtract/pull/256), [#258](https://github.com/jamiebullock/LibXtract/pull/258))
- Add a major.minor soname, as in `libxtract.so.1.3`, to the shared library on Linux and macOS ([#256](https://github.com/jamiebullock/LibXtract/pull/256), [#262](https://github.com/jamiebullock/LibXtract/pull/262))
- Add a finance example in `examples/finance` ([#261](https://github.com/jamiebullock/LibXtract/pull/261))

### Removed

- Remove the Make build ([#259](https://github.com/jamiebullock/LibXtract/pull/259), [#263](https://github.com/jamiebullock/LibXtract/pull/263))
- Remove the load-time constructor and exit-time destructor, so a thread that exits early calls `xtract_free_fft()` ([#255](https://github.com/jamiebullock/LibXtract/pull/255))

## [1.2.0] - 2026-11-09

### Changed

- Make return codes part of the API contract ([#221](https://github.com/jamiebullock/LibXtract/pull/221))
- Check N and argv more strictly across the feature functions and document each function's contract in the header ([#218](https://github.com/jamiebullock/LibXtract/pull/218), [#219](https://github.com/jamiebullock/LibXtract/pull/219))
- Change the phase sign of `xtract_spectrum` on Linux and Windows to match macOS ([#246](https://github.com/jamiebullock/LibXtract/pull/246))
- Return `XTRACT_BAD_STATE` from `xtract_spectrum` and `xtract_autocorrelation_fft` when the FFT is not initialised for N ([#219](https://github.com/jamiebullock/LibXtract/pull/219))
- Return `XTRACT_BAD_STATE` from `xtract_failsafe_f0` when its fallback FFT is not initialised ([#252](https://github.com/jamiebullock/LibXtract/pull/252))
- Return `XTRACT_NO_RESULT` from `xtract_midicent` for a zero fundamental ([#218](https://github.com/jamiebullock/LibXtract/pull/218))
- Limit the period `xtract_mcleod_f0` reports to N / 2 samples ([#237](https://github.com/jamiebullock/LibXtract/pull/237))
- Keep only negative differences under `XTRACT_NEGATIVE_SLOPE` in `xtract_flux` and `xtract_lnorm`, which ignored it ([#241](https://github.com/jamiebullock/LibXtract/pull/241))
- Document the input of `xtract_bark_coefficients` as a spectrum with DC included ([#213](https://github.com/jamiebullock/LibXtract/pull/213))
- Accept every scalar feature in `xtract_features_from_subframes` ([#254](https://github.com/jamiebullock/LibXtract/pull/254))
- Take N as the length of the input array in `xtract_spectral_subband_centroids` ([#220](https://github.com/jamiebullock/LibXtract/pull/220))
- Deprecate `XTRACT_BAD_ARGV`, `XTRACT_BAD_VECTOR_SIZE` and `XTRACT_DENORMAL_FOUND`, which are no longer returned ([#218](https://github.com/jamiebullock/LibXtract/pull/218))

### Added

- Add `xtract_median` and `xtract_median_absolute_deviation` ([#229](https://github.com/jamiebullock/LibXtract/pull/229))
- Add `xtract_hjorth_activity`, `xtract_hjorth_mobility` and `xtract_hjorth_complexity` ([#230](https://github.com/jamiebullock/LibXtract/pull/230))
- Add `xtract_time_reversal_asymmetry` ([#231](https://github.com/jamiebullock/LibXtract/pull/231))
- Add `xtract_petrosian_fractal_dimension`, `xtract_katz_fractal_dimension` and `xtract_higuchi_fractal_dimension` ([#233](https://github.com/jamiebullock/LibXtract/pull/233))
- Add `xtract_permutation_entropy` and `xtract_lempel_ziv_complexity` ([#234](https://github.com/jamiebullock/LibXtract/pull/234), [#235](https://github.com/jamiebullock/LibXtract/pull/235))
- Add `xtract_sample_entropy` and `xtract_approximate_entropy` ([#236](https://github.com/jamiebullock/LibXtract/pull/236))
- Add `xtract_periodicity` and `xtract_signal_confidence` ([#237](https://github.com/jamiebullock/LibXtract/pull/237))
- Implement `xtract_attack_time` and `xtract_decay_time`, declared since 1.0 ([#238](https://github.com/jamiebullock/LibXtract/pull/238))
- Add `xtract_decay_rate` ([#238](https://github.com/jamiebullock/LibXtract/pull/238))
- Add `xtract_temporal_centroid`, `xtract_max_position`, `xtract_min_position` and `xtract_peak_asymmetry` ([#239](https://github.com/jamiebullock/LibXtract/pull/239))
- Add `xtract_moving_sum`, `xtract_moving_average`, `xtract_moving_variance` and `xtract_moving_standard_deviation` ([#225](https://github.com/jamiebullock/LibXtract/pull/225))
- Add `xtract_moving_max` and `xtract_moving_min` ([#226](https://github.com/jamiebullock/LibXtract/pull/226))
- Add `xtract_ewma_average` and `xtract_ewma_variance` ([#227](https://github.com/jamiebullock/LibXtract/pull/227))
- Add `xtract_cross_correlation` and `xtract_cross_correlation_lag` ([#240](https://github.com/jamiebullock/LibXtract/pull/240))
- Add `xtract_kullback_leibler_divergence`, `xtract_itakura_saito_divergence` and `xtract_modified_kullback_leibler` ([#241](https://github.com/jamiebullock/LibXtract/pull/241))
- Add `xtract_spectral_entropy`, `xtract_spectral_contrast` and `xtract_chroma` ([#232](https://github.com/jamiebullock/LibXtract/pull/232), [#242](https://github.com/jamiebullock/LibXtract/pull/242), [#243](https://github.com/jamiebullock/LibXtract/pull/243))
- Add the constant-Q transform as `xtract_init_cqt`, `xtract_free_cqt` and `xtract_cqt` ([#244](https://github.com/jamiebullock/LibXtract/pull/244))
- Add feature enumeration entries for `xtract_peak`, `xtract_mmbses` and `xtract_spectral_subband_centroids` ([#220](https://github.com/jamiebullock/LibXtract/pull/220))
- Add the units `XTRACT_SECONDS` and `XTRACT_PER_SECOND` ([#238](https://github.com/jamiebullock/LibXtract/pull/238))
- Add CBMC proofs of every feature function, run with `make verify` ([#222](https://github.com/jamiebullock/LibXtract/pull/222))

### Removed

- Remove the diagnostic prints to stderr ([#219](https://github.com/jamiebullock/LibXtract/pull/219))

### Fixed

- Fix out-of-bounds access on zero-length input in `xtract_highest_value`, `xtract_peak`, `xtract_lpc`, `xtract_lpcc` and `xtract_subbands` ([#217](https://github.com/jamiebullock/LibXtract/pull/217))

## [1.1.0] - 2026-09-30

### Changed

- Change the licence from MIT to zlib ([#189](https://github.com/jamiebullock/LibXtract/pull/189))
- Build the library as C99 ([#147](https://github.com/jamiebullock/LibXtract/pull/147))
- Match the absolute scale of `xtract_spectrum` on macOS to Linux and Windows, halving it ([#142](https://github.com/jamiebullock/LibXtract/pull/142))
- Change every value of `xtract_mfcc` and `xtract_mel_spectrogram` by sampling the mel filterbank at bin frequencies ([#204](https://github.com/jamiebullock/LibXtract/pull/204))
- Change `xtract_hps` to index harmonics by bin layout and score them in the log domain ([#193](https://github.com/jamiebullock/LibXtract/pull/193))
- Correct the `xtract_lpcc` coefficients beyond the LPC order ([#202](https://github.com/jamiebullock/LibXtract/pull/202))
- Document the argv of `xtract_lpcc` as an int, as the function reads it ([#142](https://github.com/jamiebullock/LibXtract/pull/142))
- Take a magnitude/phase spectrum of length N in `xtract_mmbses` ([#174](https://github.com/jamiebullock/LibXtract/pull/174))
- Make `xtract_sharpness` loudness-invariant and report it in acums ([#146](https://github.com/jamiebullock/LibXtract/pull/146))
- Exclude sub-fundamental bins from `xtract_odd_even_ratio` ([#146](https://github.com/jamiebullock/LibXtract/pull/146))
- Change `xtract_init_bark` to round band limits, moving some edges by one bin ([#178](https://github.com/jamiebullock/LibXtract/pull/178))
- Change `xtract_mcleod_f0` to select the key maximum of each NSDF region ([#145](https://github.com/jamiebullock/LibXtract/pull/145))
- Match `xtract_autocorrelation_fft` to `xtract_autocorrelation` on both FFT backends ([#141](https://github.com/jamiebullock/LibXtract/pull/141))
- Read the documented spectrum layout in `xtract_spectral_subband_centroids` ([#142](https://github.com/jamiebullock/LibXtract/pull/142))
- Return `XTRACT_NO_RESULT` from `xtract_harmonic_spectrum` for a zero fundamental ([#143](https://github.com/jamiebullock/LibXtract/pull/143))
- Return `XTRACT_NO_RESULT` from `xtract_hps` for an all-zero spectrum ([#143](https://github.com/jamiebullock/LibXtract/pull/143))
- Return `XTRACT_ARGUMENT_ERROR` from `xtract_midicent` for a non-positive fundamental ([#143](https://github.com/jamiebullock/LibXtract/pull/143))

### Added

- Add `xtract_init_dct` and `xtract_free_dct` ([#186](https://github.com/jamiebullock/LibXtract/pull/186))
- Add a `dct_table` field to `xtract_mel_filter`, which a caller zero-initialises or sets in full ([#187](https://github.com/jamiebullock/LibXtract/pull/187))
- Add the `XTRACT_MAGNITUDE_PHASE_SPECTRUM` type for `xtract_spectrum` ([#174](https://github.com/jamiebullock/LibXtract/pull/174))
- Add a result range and unit to every descriptor ([#180](https://github.com/jamiebullock/LibXtract/pull/180))
- Add the units `XTRACT_UNIT_NONE`, `XTRACT_UNIT_ANY`, `XTRACT_UNIT_UNKNOWN` and `XTRACT_ACUM` ([#180](https://github.com/jamiebullock/LibXtract/pull/180))
- Add `XTRACT_F0_PEAK_CLIP` and `XTRACT_F0_CENTRE_CLIP` ([#181](https://github.com/jamiebullock/LibXtract/pull/181))
- Add a harmonic count argument, 2 to 16, to `xtract_hps` ([#193](https://github.com/jamiebullock/LibXtract/pull/193))
- Add `CONTRIBUTING.md` and `SECURITY.md` ([#191](https://github.com/jamiebullock/LibXtract/pull/191))

### Removed

- **Breaking:** remove `XTRACT_SPECTRUM_COEFFICIENTS` in favour of `XTRACT_MAGNITUDE_PHASE_SPECTRUM` ([#174](https://github.com/jamiebullock/LibXtract/pull/174))
- Remove the Visual Studio 2012 project ([#184](https://github.com/jamiebullock/LibXtract/pull/184))

### Fixed

- Fix an out-of-bounds read and write in `xtract_autocorrelation_fft` on macOS ([#141](https://github.com/jamiebullock/LibXtract/pull/141))
- Fix the last output element of `xtract_smoothed`, which depended on uninitialised memory ([#141](https://github.com/jamiebullock/LibXtract/pull/141))
- Fix the final band of `xtract_subbands`, zeroed when the bands tile the input exactly ([#141](https://github.com/jamiebullock/LibXtract/pull/141))
- Fix underflow to zero in `xtract_flatness` on long inputs ([#140](https://github.com/jamiebullock/LibXtract/pull/140))
- Fix the weighting of out-of-range bands in `xtract_sharpness` when N exceeds the Bark band count ([#143](https://github.com/jamiebullock/LibXtract/pull/143))
- Fix `xtract_lowest_value` to give a result of 0 on `XTRACT_NO_RESULT` ([#143](https://github.com/jamiebullock/LibXtract/pull/143))

[1.3.0]: https://github.com/jamiebullock/LibXtract/compare/v1.2.0...v1.3.0
[1.2.0]: https://github.com/jamiebullock/LibXtract/compare/v1.1.0...v1.2.0
[1.1.0]: https://github.com/jamiebullock/LibXtract/compare/v1.0.0...v1.1.0

# LibXtract

[![CI](https://github.com/jamiebullock/LibXtract/actions/workflows/ci.yml/badge.svg)](https://github.com/jamiebullock/LibXtract/actions/workflows/ci.yml)

LibXtract is a portable C99 library of feature extraction functions for time series and other numeric data. Each function takes an array of doubles and returns a number or a vector that describes it: a statistic, a measure of spectral shape, a pitch estimate, a measure of complexity. The library has a full set of audio features, and its statistical, complexity and moving-window functions make no assumption about what the numbers represent, so they apply to prices, sensor readings and physiological signals as they do to audio.

It has no external dependencies.

## Features

The library has more than 100 features:

- **Statistics:** mean, variance, standard deviation, average deviation, skewness, kurtosis, median, median absolute deviation, extrema, RMS amplitude, zero-crossing rate
- **Moving windows:** moving sum, average, variance, standard deviation, maximum and minimum, and the exponentially weighted average and variance
- **Complexity:** permutation, sample and approximate entropy, Lempel-Ziv complexity, the Petrosian, Katz and Higuchi fractal dimensions, the Hjorth parameters
- **Spectrum:** magnitude and power spectra, peak and harmonic spectra, and the centroid, spread, flatness, rolloff, slope, contrast and entropy of a spectrum
- **Periodicity and pitch:** autocorrelation, the average magnitude and squared difference functions, a periodicity measure, and fundamental frequency by the AMDF, wavelet, McLeod and harmonic product spectrum methods
- **Change:** flux, the Kullback-Leibler and Itakura-Saito divergences between successive frames, and the cross-correlation of two series with its lag
- **Envelope:** attack time, decay time, decay rate, temporal centroid
- **Audio:** loudness, sharpness, Bark bands, MFCCs, gammatone cepstral coefficients, linear prediction coefficients, chroma, the constant-Q transform

The [API reference](https://www.jamiebullock.com/LibXtract/) lists every function with its arguments.

## Using the library

The feature functions share one signature:

```c
int xtract_feature(const double *data, const int N, const void *argv, double *result);
```

`data` points to `N` input values, `argv` points to the arguments the feature needs, and the function writes its output to `result` and returns a status code.

A feature that depends on another takes it as an argument. The variance takes the mean, the skewness takes the mean and the standard deviation, and a spectral feature takes a spectrum from `xtract_spectrum`:

```c
double mean, variance;

xtract_mean(data, N, NULL, &mean);
xtract_variance(data, N, &mean, &variance);
```

A value is therefore computed once however many features use it, and features can be combined: the irregularity of a set of MFCCs is a valid call. Functions that work from precomputed tables, such as the FFT, the mel filterbank and the constant-Q kernel, have an `xtract_init_` function to call first.

[`examples/`](examples) holds complete programs. [`finance`](examples/finance) computes the rolling and exponentially weighted volatility of a price series and finds a change of volatility regime; [`simpletest`](examples/simpletest) runs pitch tracking, MFCCs and onset detection over an audio file.

## Building

### Prerequisites

- A C99 compiler: gcc, clang, or Visual Studio 2019 or later on Windows
- [CMake](https://cmake.org) 3.21 or later

On macOS, the library uses Apple's Accelerate framework for the FFT. On Linux and Windows, it uses the bundled Ooura FFT.

### Build and test

```bash
cmake --preset default           # configure into build/default
cmake --build --preset default   # build the library, tests, examples and benchmarks
ctest --preset default           # run the tests
```

On Windows the default preset uses the Visual Studio generator; pass `-G Ninja` from a Developer Command Prompt to use Ninja instead. No MSYS2 or other POSIX layer is needed.

Each of the other presets builds under its own directory in `build/`. Configure and build one as above with its name in place of `default`, then run it:

| Preset | Run with | Purpose |
| --- | --- | --- |
| `default` | `ctest --preset default` | Release build of the library, tests, examples and benchmarks |
| `asan` | `ctest --preset asan` | The tests under AddressSanitizer and UBSan |
| `coverage` | `cmake --build --preset coverage --target coverage` | Line and branch coverage of the test suite (needs lcov) |
| `fuzz` | `ctest --preset fuzz` | The libFuzzer harnesses (needs a clang with libFuzzer) |
| `mutation` | `cmake --build --preset mutation --target mutation` | Mutation testing with Mull (needs clang-18 and mull-18; CI runs it) |

The default preset also has targets that run a tool, each built with `cmake --build --preset default --target <name>`:

| Target | Purpose |
| --- | --- |
| `analyze` | The clang static analyzer over the first-party sources |
| `cppcheck` | cppcheck over the first-party sources |
| `format`, `format-check` | Apply or check the clang-format style |
| `bench` | The benchmarks |
| `doc` | The API reference (needs Doxygen) |

The CBMC proofs of the feature functions have their own GNU make build, `make -C verify`, which CI runs on every push to main and develop; see [verify/README.md](verify/README.md).

### Install

```bash
cmake --install build/default                      # install to /usr/local
cmake --install build/default --prefix /somewhere  # install elsewhere
```

The install places the library, the headers, a CMake package and a pkg-config file under the prefix, so a downstream CMake project can call `find_package(xtract)` and link `xtract::xtract`, and any other build can use `pkg-config --cflags --libs xtract`. `BUILD_SHARED_LIBS=ON` builds a shared library.

## Python, Java and Node.js bindings

The bindings are generated with [SWIG](https://www.swig.org) 4.2 or later. The CMake options `XTRACT_BUILD_PYTHON`, `XTRACT_BUILD_JAVA` and `XTRACT_BUILD_NODE` are `AUTO` by default, which builds each binding whose toolchain is found: Python with its development headers, a JDK, or `node` with the Node.js headers and `node-addon-api` installed under `swig/` (`npm install --prefix swig node-addon-api`). Set an option to `ON` to fail when the toolchain is missing, or `OFF` to skip it:

```bash
cmake --preset default -DXTRACT_BUILD_PYTHON=ON -DXTRACT_BUILD_JAVA=OFF -DXTRACT_BUILD_NODE=OFF
cmake --build --preset default
ctest --preset default -R bindings
```

The modules are written under `build/default/swig/<language>`.

## Documentation

The API reference for the current release is published at <https://www.jamiebullock.com/LibXtract/>. It is generated from the [Doxygen](https://www.doxygen.nl) comments in the public headers and rebuilt from `main` by the `Docs` workflow on every push.

To build it locally, with Doxygen installed:

```bash
cmake --build --preset default --target doc
```

The output is `doc/html/index.html`.

## Versioning

LibXtract follows [Semantic Versioning](https://semver.org). The public API
is the set of headers installed under `include/xtract/`. Releases within 1.x
keep source compatibility: functions and `XTRACT_*` values are added, never
removed or changed, and breaking changes wait for 2.0. Binary compatibility
is not guaranteed between minor releases, so recompile and relink dependent
code when moving to a new one. A correctness fix may change a feature's
numerical output; the release notes say when.

## Contributing

[CONTRIBUTING.md](CONTRIBUTING.md) covers how to report a bug or propose a
change, the code standard, and the terms contributions are accepted under.
Report a security problem privately, as [SECURITY.md](SECURITY.md) describes.

## Licence

zlib. See `LICENSE`. The bundled third-party code under `src/` keeps its own
licences, listed in `DEPENDENCIES.md`; one of them (dywapitchtrack, MIT)
requires its copyright notice to accompany binary distributions.

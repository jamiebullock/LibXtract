# LibXtract

[![CI](https://github.com/jamiebullock/LibXtract/actions/workflows/ci.yml/badge.svg)](https://github.com/jamiebullock/LibXtract/actions/workflows/ci.yml)

LibXtract is a simple, portable, lightweight library of audio feature extraction functions. The purpose of the library is to provide a relatively exhaustive set of feature extraction primitives that are designed to be 'cascaded' to create extraction hierarchies.

For example, 'variance', 'average deviation', 'skewness' and 'kurtosis', all require the 'mean' of the input vector to be precomputed. However, rather than compute the 'mean' 'inside' each function, it is expected that the 'mean' will be passed in as an argument. This means that if the user wishes to use all of these features, the mean is calculated only once, and then passed to any functions that require it.

This philosophy of 'cascading' features is followed throughout the library, for example with features that operate on the magnitude spectrum of a signal vector (e.g. 'irregularity'), the magnitude spectrum is not calculated 'inside' the respective function, instead, a pointer to the first element in an array containing the magnitude spectrum is passed in as an argument.

Hopefully this not only makes the library more efficient when computing large numbers of features, but also makes it more flexible because extraction functions can be combined arbitrarily (one can take the irregularity of the Mel Frequency Cepstral Coefficients for example).

A complete list of features can be found by viewing the header files, or reading the doxygen documentation.

## Building

### Prerequisites

- A C99 compiler (gcc, clang, MinGW)
- make

On macOS, the library uses Apple's Accelerate framework for FFT. On Linux and Windows, it uses the bundled Ooura FFT.

On Windows, an MSYS2/MinGW environment is required to provide `make` and a POSIX-compatible shell. Install [MSYS2](https://www.msys2.org), then from the MinGW 64-bit shell run `pacman -S mingw-w64-x86_64-gcc make`.

### Build and test

```bash
make        # build library and examples
make check  # build and run tests
```

### Install

```bash
make install                        # install to /usr/local
make install PREFIX=/somewhere/else # install elsewhere
```

## Python bindings

To build the Python bindings, [SWIG](http://www.swig.org) and Python are required:

```bash
make swig
```

## Documentation

LibXtract headers are documented using [doxygen](https://www.doxygen.nl) comments. If you have doxygen installed:

```bash
make doc
```

The generated HTML documentation can then be viewed by opening `doc/html/index.html`.

## License

Copyright (C) 2012 Jamie Bullock

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.

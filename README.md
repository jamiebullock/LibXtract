# LibXtract

[![Build Status](https://travis-ci.org/jamiebullock/LibXtract.png?branch=master)](https://travis-ci.org/jamiebullock/LibXtract)

LibXtract is a simple, portable, lightweight library of audio feature extraction functions. The purpose of the library is to provide a relatively exhaustive set of feature extraction primatives that are designed to be 'cascaded' to create a extraction hierarchies.

For example, 'variance', 'average deviation', 'skewness' and 'kurtosis', all require the 'mean' of the input vector to be precomputed. However, rather than compute the 'mean' 'inside' each function, it is expected that the 'mean' will be passed in as an argument. This means that if the user wishes to use all of these features, the mean is calculated only once, and then passed to any functions that require it.

This philosophy of 'cascading' features is followed throughout the library, for example with features that operate on the magnitude spectrum of a signal vector (e.g. 'irregularity'), the magnitude spectrum is not calculated 'inside' the respective function, instead, a pointer to the first element in an array containing the magnitude spectrum is passed in as an argument.

Hopefully this not only makes the library more efficient when computing large numbers of features, but also makes it more flexible because extraction functions can be combined arbitrarily (one can take the irregularility of the Mel Frequency Cepstral Coefficients for example).

A complete list of features can be found by viewing the header files, or reading the doxygen documentation, available with this package.

## Downloading

The latest source code release for LibXtract can be downloaded from [https://github.com/jamiebullock/LibXtract/downloads](https://github.com/jamiebullock/LibXtract/downloads).

## Dependencies

To build the SWIG bindings [SWIG]() is required. For the Python bindings, a Python install is required. For the Java bindings a Java install is required.

## Installation

Type `make install` to build and install to `/usr/local`. Type `make install PREFIX=/somewhere/else` to install to `/somewhere/else`,


## Documentation

LibXtract headers are documented using [doxygen](http://www.stack.nl/~dimitri/doxygen/) comments.

If you have doxygen installed, the LibXtract build system should automatically detect this during `configure` and generate [LaTeX](http://www.latex-project.org) and HTML documentation in the `doc` directory when LibXtract is built with `make`.

The generated HTML documentation can then be viewed in a web browser by opening the file `doc/html/index.html`.

**Pre-generated documentation can be found on the [LibXtract website](http://jamiebullock.github.io/LibXtract/documentation)**


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



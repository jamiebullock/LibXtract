# LibXtract

LibXtract is a simple, portable, lightweight library of audio feature extraction functions. The purpose of the library is to provide a relatively exhaustive set of feature extraction primatives that are designed to be 'cascaded' to create a extraction hierarchies.

For example, 'variance', 'average deviation', 'skewness' and 'kurtosis', all require the 'mean' of the input vector to be precomputed. However, rather than compute the 'mean' 'inside' each function, it is expected that the 'mean' will be passed in as an argument. This means that if the user wishes to use all of these features, the mean is calculated only once, and then passed to any functions that require it.

This philosophy of 'cascading' features is followed throughout the library, for example with features that operate on the magnitude spectrum of a signal vector (e.g. 'irregularity'), the magnitude spectrum is not calculated 'inside' the respective function, instead, a pointer to the first element in an array containing the magnitude spectrum is passed in as an argument.

Hopefully this not only makes the library more efficient when computing large numbers of features, but also makes it more flexible because extraction functions can be combined arbitrarily (one can take the irregularility of the Mel Frequency Cepstral Coefficients for example).

A complete list of features can be found by viewing the header files, or reading the doxygen documentation, available with this package.

## Downloading

The latest source code release for LibXtract can be downloaded from [https://github.com/jamiebullock/LibXtract/downloads](https://github.com/jamiebullock/LibXtract/downloads).

## Dependencies

To build the PD external, the PD header 'm_pd.h' is required, this can be found in the [Pure Data source code release](http://puredata.info/downloads/pure-data).

If you are compiling LibXtract from a GIT clone, the **autotools** build system is required. This includes:

    automake >= 1.11
    autoconf >= 2.68
    libtool >= 2.4

## Installation

If you are installing from a GIT clone, first run:

    sh autogen.sh

If autogen.sh was successful, or you downloaded the source release, type:

    ./configure --enable-pd_example
    make
    sudo make install

There following configure flags are optional:

    --enable-pd_example (to build the PD example)
    --enable-simpletest (to build the simpletest example)

If you wish to build a Universal binary on OS X, you may need to do something like this:

    CFLAGS="-isysroot /Developer/SDKs/MacOSX10.4u.sdk -Wl,-syslibroot,/Developer/SDKs/MacOSX10.4u.sdk -arch i386 -arch ppc" \./configure --isable-dependency-tracking --your-options

## Building the MSP external

Type:

    cd examples/MSP
    make
    sudo make install

To build for a specific architecture:

    make intel
    or
    make ppc

## Building the Python bindings

To build the python bindings, add to your configure flags:

    --enable-swig --with-python

This requres additional dependencies to be resolved:

    swig >= 1.3
    python (with development files) >= 2.5

It might work with earlier versions of Python, but this has not been tested.

If you do not have the 'standard' version of Python on OS X, you might need to tell the configure script where the python library is e.g.:

    ./configure --your-flags LDFLAGS="-L/Library/Frameworks/Python.framework/Versions/2.5/lib/python2.5/config/"

To find your Python library type:

    locate libpython

NOTE: The python module will get installed under the main install prefix, so if your install prefix is set to /usr/local, then the python module will get installed to /usr/local/lib/python2.5/site-packages/libxtract/xtract. You may need to add this to your PYTHONPATH environment variable. For example in ~/.bash_profile add:
    export PYTHONPATH=/usr/local/lib/python2.7/site-packages


## Building the Java bindings

To build the java bindings, add to your configure flags:

    --enable-swig --with-java

This requres additional dependencies to be resolved:

    swig >= 1.3
    java (with development files) >= 2.0

It might work with other versions of Java, but this has not been tested.

On OS X, you will probably need to set your CLASSPATH environment variable before running ./configure

    export CLASSPATH=/System/Library/Frameworks/JavaVM.framework/Classes/classes.jar

On OS X, you also probably need to tell the configure script where to find your Java (JNI) headers.

    ./configure --flags  CFLAGS="-I/System/Library/Frameworks/JavaVM.framework/Headers"

To find out where your headers are:

    locate jni.h

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



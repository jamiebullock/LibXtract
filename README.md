# LibXtract

LibXtract is a simple, portable, lightweight library of audio feature extraction functions. The purpose of the library is to provide a relatively exhaustive set of feature extraction primatives that are designed to be 'cascaded' to create a extraction hierarchies.

For example, 'variance', 'average deviation', 'skewness' and 'kurtosis', all require the 'mean' of the input vector to be precomputed. However, rather than compute the 'mean' 'inside' each function, it is expected that the 'mean' will be passed in as an argument. This means that if the user wishes to use all of these features, the mean is calculated only once, and then passed to any functions that require it.

 This philosophy of 'cascading' features is followed throughout the library, for example with features that operate on the magnitude spectrum of a signal vector (e.g. 'irregularity'), the magnitude spectrum is not calculated 'inside' the respective function, instead, a pointer to the first element in an array containing the magnitude spectrum is passed in as an argument.

Hopefully this not only makes the library more efficient when computing large numbers of features, but also makes it more flexible because extraction functions can be combined arbitrarily (one can take the irregularility of the Mel Frequency Cepstral Coefficients for example).

A complete list of features can be found by viewing the header files, or reading the doxygen documentation, available with this package.

## Dependencies

To compile LibXtract, the following software is also needed:

    automake >= 1.6
    fftw3 (compiled with floating-point support)

To build the PD external, the PD header 'm_pd.h' is required

## Installation

Type:

    ./configure --enable-pd_example --enable-fft
    make
    sudo make install

There following configure flags are optional:

    --enable-pd_example (to build the PD example)
    --enable-fft (to enable functions that require fftw3)
    --enable-simpletest (to build the simpletest example)

If you wish to build a Universal binary on OS X:

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

## Disclaimer
 
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
 
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, 
USA.

#!/usr/bin/env python3
# Part of LibXtract
#
# SPDX-FileCopyrightText: 2007 Jamie Bullock
# SPDX-License-Identifier: Zlib

import math
import sys

try:
    import xtract
except ImportError as e:
    print('Failed to load the library "xtract": ' + str(e))
    sys.exit(1)

print('\nRunning libxtract Python bindings test...\n')

# The interface declares some headers function by function, so a function
# added to one of them is missing from the module until it is declared there
if len(sys.argv) > 1:
    import glob
    import re

    declared = set()
    for header in glob.glob(sys.argv[1] + '/*.h'):
        with open(header) as f:
            declared.update(re.findall(r'^\s+(?!\*)[\w \*]*?\b(xtract_\w+)\(.*\);', f.read(), re.MULTILINE))
    missing = sorted(name for name in declared if not hasattr(xtract, name))
    if missing:
        print('The module has no ' + ', '.join(missing))
        sys.exit(1)
    print('The module has all %d functions the headers declare' % len(declared))

len = 8

a = xtract.doubleArray(len)
temp = []

for i in range(0, len):
    a[i] = 2 * i
    temp.append(str(a[i]))

mean = xtract.xtract_mean(None, a,len,None)[1]

print('The mean of ' + ', '.join(temp) + ' is: %.2f' % mean)

argv = xtract.doubleArray(1)
argv[0] = mean

variance = xtract.xtract_variance(None, a, len, argv)[1]

print('The variance is %.2f' % variance)

print('Computing spectrum...')

argv = xtract.doubleArray(3)
argv[0] = 44100.0 / len
argv[1] = float(xtract.XTRACT_MAGNITUDE_SPECTRUM)
argv[2] = 0.0

ctx = xtract.xtract_context_new(len)

result = xtract.doubleArray(len)

xtract.xtract_spectrum(ctx, a,len,argv, result)
xtract.xtract_context_delete(ctx)


for i in range(len):
    print(result[i])

print('Computing the log energy a filter passes...')

gains = xtract.doubleArray(4)
coefficients = xtract.doubleArray(4)
for i, (gain, coefficient) in enumerate(zip([0.5, 0.25, 2.0, 1.0], [1.0, 2.0, 3.0, 4.0])):
    gains[i] = gain
    coefficients[i] = coefficient

filterbank = xtract.xtract_init_filterbank(8, 1, gains)
energy = xtract.doubleArray(1)
xtract.xtract_mel_spectrogram(None, coefficients, 4, filterbank, energy)
xtract.xtract_free_filterbank(filterbank)

print('The log energy is %.4f' % energy[0])

if not abs(energy[0] - math.log(11.0)) <= 1e-12:
    sys.exit(1)

print('Computing windowed subframes...')

for i in range(0, len):
    a[i] = 1.0

window = xtract.xtract_init_window(len // 2, xtract.XTRACT_HANN)
xtract.xtract_features_from_subframes(None, a, len, xtract.XTRACT_WINDOWED, window, result)

for i in range(len):
    print(result[i])

print('Computing the correlation of two series...')

pair = xtract.doubleArray(8)
for i, value in enumerate([1.0, 2.0, 3.0, 4.0, 2.0, 4.0, 6.0, 8.0]):
    pair[i] = value

correlation = xtract.xtract_pearson_correlation(None, pair, 8, None)[1]

print('The Pearson correlation is %.2f' % correlation)

if abs(correlation - 1.0) > 1e-12:
    sys.exit(1)

channels = xtract.intArray(1)
channels[0] = 2
covariance = xtract.doubleArray(4)
xtract.xtract_covariance_matrix(None, pair, 8, channels, covariance)

print('The covariance matrix is %.2f, %.2f, %.2f, %.2f' % tuple(covariance[i] for i in range(4)))

if abs(covariance[1] - 10.0 / 3.0) > 1e-12:
    sys.exit(1)

print('Testing stateful functions...')

N = 4
state = xtract.xtract_last_n_state_new(N)
result = xtract.doubleArray(N)

for i in range(N):
    result[i] = 0.0

data = xtract.doubleArray(1)
data[0] = 42.0
# N.B. data is a single sample; N is the window size, not the size of data
xtract.xtract_last_n(None, data, N, state, result)

print('last_n after one value: %.1f' % result[N - 1])

data[0] = 99.0
xtract.xtract_last_n(None, data, N, state, result)

print('last_n after two values: %.1f, %.1f' % (result[N - 2], result[N - 1]))

xtract.xtract_last_n_state_delete(state)

print('Reading a descriptor...')

feature = xtract.xtract_feature_by_name('spectral_centroid')
descriptor = xtract.xtract_descriptor(feature)
if feature != xtract.XTRACT_SPECTRAL_CENTROID or descriptor.name != 'spectral_centroid':
    print('The spectral centroid is not found by its name')
    sys.exit(1)
print('%s: %s' % (descriptor.title, descriptor.description))

print('\nFinished!\n')

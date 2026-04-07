#!/usr/bin/env python3

import sys

try:
    import xtract
except ImportError as e:
    print('Failed to load the library "xtract": ' + str(e))
    sys.exit(1)

print('\nRunning libxtract Python bindings test...\n')

len = 8

a = xtract.doubleArray(len)
temp = []

for i in range(0, len):
    a[i] = 2 * i
    temp.append(str(a[i]))

mean = xtract.xtract_mean(a,len,None)[1]

print('The mean of ' + ', '.join(temp) + ' is: %.2f' % mean)

argv = xtract.doubleArray(1)
argv[0] = mean

variance = xtract.xtract_variance(a, len, argv)[1]

print('The variance is %.2f' % variance)

print('Computing spectrum...')

argv = xtract.doubleArray(4)
argv[0] = 44100.0 / len
argv[1] = float(xtract.XTRACT_MAGNITUDE_SPECTRUM)
argv[2] = 0.0
argv[3] = 0.0

xtract.xtract_init_fft(len, xtract.XTRACT_SPECTRUM);

result = xtract.doubleArray(len)

xtract.xtract_spectrum(a,len,argv, result)


for i in range(len):
    print(result[i])


print('Computing windowed subframes...')

for i in range(0, len):
    a[i] = 1.0

window = xtract.xtract_init_window(len // 2, xtract.XTRACT_HANN)
xtract.xtract_features_from_subframes(a, len, xtract.XTRACT_WINDOWED, window, result)

for i in range(len):
    print(result[i])

print('Testing stateful functions...')

N = 4
state = xtract.xtract_last_n_state_new(N)
result = xtract.doubleArray(N)

for i in range(N):
    result[i] = 0.0

data = xtract.doubleArray(1)
data[0] = 42.0
# N.B. data is a single sample; N is the window size, not the size of data
xtract.xtract_last_n(state, data, N, None, result)

print('last_n after one value: %.1f' % result[N - 1])

data[0] = 99.0
xtract.xtract_last_n(state, data, N, None, result)

print('last_n after two values: %.1f, %.1f' % (result[N - 2], result[N - 1]))

xtract.xtract_last_n_state_delete(state)

print('\nFinished!\n')

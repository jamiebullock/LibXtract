#!/usr/bin/python

try:
    import xtract
except ImportError:
    print 'Failed to load the library "xtract"'

print '\nRunning libxtract Python bindings test...\n'

len = 8

a = xtract.doubleArray(len)
temp = []

for i in range(0, len):
    a[i] = 2 * i
    temp.append(str(a[i]))

mean = xtract.xtract_mean(a,len,None)[1]

print 'The mean of ' + ', '.join(temp) + ' is: %.2f' % mean

argv = xtract.doubleArray(1)
argv[0] = mean

variance = xtract.xtract_variance(a, len, argv)[1]

print 'The variance is %.2f' % variance

print 'Computing spectrum...'

argv = xtract.doubleArray(1)
argv[0] = 44100.0 / len  # Fake sample rate

xtract.xtract_init_fft(len, xtract.XTRACT_SPECTRUM);

result = xtract.doubleArray(len)

xtract.xtract_spectrum(a,len,argv, result)


for i in range(len):
    print result[i]


print 'Computing windowed subframes...'

for i in range(0, len):
    a[i] = 1.0

window = xtract.xtract_init_window(len / 2, xtract.XTRACT_HANN)
xtract.xtract_features_from_subframes(a, len, xtract.XTRACT_WINDOWED, window, result) 

for i in range(len):
    print result[i]

print '\nFinished!\n'

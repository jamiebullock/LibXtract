#!/usr/bin/python

try:
    import xtract
except ImportError:
    print 'Failed to load the library "xtract"'

print '\nRunning libxtract Python bindings test...\n'

len = 8

a = xtract.floatArray(len)
temp = []

for i in range(0, len):
    a[i] = 2 * i
    temp.append(str(a[i]))

print 'The mean of ' + ', '.join(temp) + ' is: %.2f' % \
	  xtract.xtract_mean(a,len,None)[1]

print 'Computing spectrum...'

argv = xtract.floatArray(1)
argv[0] = 44100.0 / len  # Fake sample rate

xtract.xtract_init_fft(len, xtract.XTRACT_SPECTRUM);

result = xtract.floatArray(len)

xtract.xtract_spectrum(a,len,argv, result)


for i in range(len):
    print result[i]

print '\nFinished!\n'

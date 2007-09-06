#!/usr/bin/python

try:
    import xtract
except ImportError:
    print 'Failed to load the library "jxtract"'

print '\nRunning libxtract Python bindings test...\n'

len = 5

a = xtract.floatArray(len)
temp = []

for i in range(0, len):
    a[i] = 2 * i
    temp.append(str(a[i]))

print 'The mean of ' + ', '.join(temp) + ' is: %.2f' % \
	  xtract.xtract_mean(a,len,None)[1]

print '\nFinished!\n'

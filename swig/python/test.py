#!/usr/bin/python

import xtract

len = 5

a = xtract.floatArray(len)

for i in range(0, len):
    a[i] = 2 * i

retval,result = xtract.xtract_mean(a,len,None)

print result


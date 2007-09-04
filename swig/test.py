#!/usr/bin/python

import xtract

len = 5

a = xtract.new_floatArray(len)

for i in range(0, len):
    xtract.floatArray_setitem(a, i, 2*i)

retval,result = xtract.xtract_mean(a,len,None)

print result





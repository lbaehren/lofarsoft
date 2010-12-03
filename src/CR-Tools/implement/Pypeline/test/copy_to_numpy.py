#! /usr/bin/env python

import pycrtools as cr
import numpy as np

print "Testing double"
a=cr.hArray([1.0,2.0,3.0,4.0,5.0])
b=np.empty(5,dtype='float')
cr.hCopy(b,a)
print b
print a.toNumpy()

print "\nTesting complex"
a=cr.hArray(complex, 5)
for i in range(5):
    a[i]=i
b=np.empty(5,dtype='complex')
cr.hCopy(b,a)
print b
print a.toNumpy()

print "\nTesting int"
a=cr.hArray([1,2,3,4,5])
b=np.empty(5,dtype='int')
cr.hCopy(b,a)
print b
print a.toNumpy()

print "\nTesting reverse"
b[0]=10
b[3]=11
cr.hCopy(a,b)
print b
print a

print "\nTesting looping"
a=cr.hArray([1,2,3,4,5])
b=np.empty(20,dtype='int')
cr.hCopy(b,a)
print a
print b

print "\nTesting looping for reverse copy"
a=cr.hArray([0 for i in range(20)])
b=np.arange(5,dtype='int')
cr.hCopy(a,b)
print b
print a


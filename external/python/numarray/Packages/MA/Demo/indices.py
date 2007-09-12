# sample of indexing behavior
from numarray.ma import *

a = array([1,2,3,4,5], Float)
a[1] = masked
print a
print "------"
for i in range(len(a)):
    print i, a[i]
print "------"
for x in a:
    print x
print "------"
print "filled with masked", filled(a, masked)
print "filled with 0.", filled(a, 0.)

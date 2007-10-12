__rcsid__="$Id: simpletest.py 39 2007-02-01 02:54:54Z phil $"

import simple_ext,numpy

print "\n--send arrays from C++ to python: --\n"
print simple_ext.testToPython()

print "\n--send array from python to C++:-- \n"

theArray=numpy.array([1,2,3,4],dtype=float)
theArray.shape=[2,2]
simple_ext.testFromPython(theArray)

print "\n--send the wrong type --\n"
from traceback import print_exc
theArray=numpy.array([1,2,3,4],dtype=int)
theArray.shape=[2,2]
try:
    simple_ext.testFromPython(theArray)
except TypeError:
    print_exc()

print "\n--send the wrong rank --\n"
theArray=numpy.array([1,2,3,4],dtype=float)
try:
    simple_ext.testFromPython(theArray)
except RuntimeError:
    print_exc()


print "\n--print docstrings --\n"
print "\nmodule rcsid:%s\n " % simple_ext.RCSID
print simple_ext.__doc__
print simple_ext.testToPython.__doc__
print simple_ext.testFromPython.__doc__
print simple_ext.testFromPython2.__doc__
print "numpy version: ",numpy.version.version

    





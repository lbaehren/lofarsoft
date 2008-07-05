#! /usr/bin/env python 
#
# This script times the 2D convolution code in various forms.

import time
from numarray import *

a=arange(10.0**6)
a.setshape(1000,1000)
c=a.copy()
b = a.copy()
b.byteswap()
d = a.copy()
d._aligned = 0

a1=arange(10.0**6)
c1=a1.copy()
b1 = a1.copy()
b1.byteswap()
d1 = a1.copy()
d1._aligned = 0

MIN_TIME_RESOLUTION = 0.001

def bench(bmark, ls=None, gs=None):
    if ls is None:
        ls = locals()
    if gs is None:
        gs = globals()
    t0 = time.time()
    r = eval(bmark, ls, gs)
    t = time.time()-t0
    if t > MIN_TIME_RESOLUTION:
        return t
    else:
        return MIN_TIME_RESOLUTION

def test():
    import numarray.examples
    import numarray.examples.convolve
    layer = [["High-level API:",
              "import numarray.examples.convolve.high_level as convolve"],
             ["Element-wise API:",
              "import  numarray.examples.convolve.element_wise as convolve"],
             ["One-dimensional API:",
              "import  numarray.examples.convolve.one_dimensional as convolve"],
             ["Numeric compatibility layer:",
              "import  numarray.examples.convolve.numpy_compat as convolve"]]
    for level in layer:
        print "\n" + level[0]
        exec(compile(level[1], '<string>', 'exec'))

        for ksize in [5, 10, 20]:
            k1 = ones(ksize, type=Float64)
            print "%-20s k = %2d: time = %7.3f s" % ("Normal Convolve1d", ksize,
                               bench("convolve.Convolve1d(k1, a1, c1)",
                                     locals()))
        for ksize in [5, 10, 20]:
            k1 = ones(ksize, type=Float64)
            print "%-20s k = %2d: time = %7.3f s" % ("Byteswap Convolve1d", ksize,
                               bench("convolve.Convolve1d(k1, b1, c1)",
                                     locals()))
        for ksize in [5, 10, 20]:
            k1 = ones(ksize, type=Float64)
            print "%-20s k = %2d: time = %7.3f s" % ("Align Convolve1d", ksize,
                               bench("convolve.Convolve1d(k1, d1, c1)",
                                     locals()))

        for ksize in [5, 10, 20]:
            k = identity(ksize, type=Float64)
            print "%-20s k = %2d: time = %7.3f s" % ("Normal Convolve2d", ksize,
                               bench("convolve.Convolve2d(k, a, c)", locals()))
        for ksize in [5, 10, 20]:
            k = identity(ksize, type=Float64)
            print "%-20s k = %2d: time = %7.3f s" % ("Byteswap Convolve2d", ksize,
                               bench("convolve.Convolve2d(k, b, c)", locals()))
        for ksize in [5, 10, 20]:
            k = identity(ksize, type=Float64)
            print "%-20s k = %2d: time = %7.3f s" % ("Align Convolve2d", ksize,
                               bench("convolve.Convolve2d(k, d, c)", locals()))

if __name__ == "__main__":
    test()


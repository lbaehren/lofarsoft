import sys
import gc
import numarray as _na
import time
from numarray.dotblas import USING_BLAS

# gc.set_threshold(1)

packages = [
    "numarray.numtest",
    "numarray.ieeespecial",
    "numarray.records",
    "numarray.strings",
    "numarray.memmap",
    "numarray.objects",
    "numarray.memorytest",
    "numarray.examples.convolve",
    "numarray.convolve",
    "numarray.fft",
    "numarray.linear_algebra",
    "numarray.image",
    "numarray.nd_image",
    "numarray.random_array",
    "numarray.ma",
    "numarray.matrix",
    "numarray.array_protocol",
    "numarray.dtype"
    ]

def test():
    if "gettotalrefcount" in dir(sys):
        DEBUG = "debug"
    else:
        DEBUG = "normal"

    if USING_BLAS:
        LINALG_KIND = "external"
    else:
        LINALG_KIND = "builtin"
    
    print "Testing numarray",_na.__version__,"on",DEBUG,"Python",sys.version_info, "on platform", sys.platform, "with", LINALG_KIND, "linear algebra support"
    total_t0 = time.clock()
    for p in packages:
        exec("import " + p)
        t0 = time.clock()
        result = eval(p+".test()")
        if result is not None:
            print ("%-40s%5.2f      %s" % (p + ":" , time.clock()-t0,  result))
    print "%-40s%5.2f" % ("Total time:", time.clock()-total_t0)
        
if __name__ == "__main__":
    test()

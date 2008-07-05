import numarray.libteacup
from numarray.libteacup import get_calibration, set_calibration, reset, calibrate

def report():
    items = numarray.libteacup.get_timings().items()
    items.sort()
    for item in items:
        print ("%-32s%-32s%s" % (item[0][0], item[0][1], item[1])), "cycles:", item[1].cycles


def profile_cached_function(f):
    f()
    f()
    reset()
    f()
    report()

def test_numarray_subscript():
    import numarray.numeric
    a  = numarray.numeric.arange(100)
    for i in xrange(100):
        for j in xrange(1000):
            x = a[i]
    report()


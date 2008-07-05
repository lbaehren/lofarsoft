'''This module contains GENERATED CODE which constructs numarray universal
functions from C-coded cfuncs contained in a sister extension module.

***************************** DO NOT EDIT **************************
'''

import _na_special
import numarray.ufunc as _uf

globals().update(_uf.make_ufuncs(_na_special))


def plot_airy(start=-10,stop=10,step=0.1,which=1):
    import matplotlib.pylab as mpl;

    a = mpl.arange(start, stop, step);
    mpl.plot(a, airy(a)[which]);

    b = 1.j*a + a
    ba = airy(b)[which]

    h = mpl.figure(2)
    mpl.plot(b.real, ba.real)

    i = mpl.figure(3)
    mpl.plot(b.imag, ba.imag)
    
    mpl.show()




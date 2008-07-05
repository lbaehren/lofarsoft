""" This setup demonstrates how to use the numarray code generation
facilities to define additional ufuncs at the user level.  Universal
functions apply operators or C-functions elementwise to all of the
elements of an array or pair of arrays. This setup generates code
defining three universal functions which are installed as the
numarray.foo package: cos, bessel, and bar.

The ufuncs are used like this:

>>> import numarray as na
>>> import numarray.foo as foo
>>> a = na.array([1,2,3,4], type='Int32')
>>> foo.Cos(a)
array([ 0.54030228, -0.41614684, -0.9899925 , -0.65364361 ], type=Float32)

Note that since a is an Int32 array, Cos(a) first does blockwise
conversion of a to Float32 before calling the cosine library function.
"""

import distutils, os
from distutils.core import setup
from numarray.numarrayext import NumarrayExtension
from numarray.codegenerator import UfuncModule, make_stub

# ======================================================================
#
# Generate the C-code for the numarray.foo._foo extension:
# 

m = UfuncModule("_foo")            


# Inline the code for a couple C functions to be turned into ufuncs.
# This method lets you define your function here in the setup.py.  An
# alternate approach would be to link with a libarary or additional C
# module.
m.add_code("""

static double c_bessel(double x, double y)
{
      double x2=x*x, y2=y*y, diff=x2-y2;
      return diff*diff/(x2+y2);
}

static UInt8  c_bar(UInt32 x )
{
      return (x >> 24) & 0xFF;
}


extern int airy ( double x, double *ai, double *aip, double *bi, double *bip );

""")

# Method add_ufunc() defines the name of a ufunc, it's corresponding C
# function which is applied element-wise to all elements of an array,
# The arity of the ufunc (unary or binary only), and the I/O type
# signatures which are directly supported by the ufunc.  Binary Ufunc
# "bessel" is implemented by the inline function "c_bessel" from the
# add_code() call above.

m.add_ufunc("bessel", "c_bessel", 2, [('Float32', 'Float32'),
                                      ('Float64', 'Float64')])

# Unary Ufunc "bar" only supports one builtin type signature:
# UInt32-->UInt8.  Other types are blockwise converted by the ufunc
# machinery to UInt32 before "c_bar" is called.

m.add_ufunc("bar", "c_bar", 1, [('UInt32','UInt8')])

# Unary Ufunc "cos" is implemented by the C standard library function
# "cos".  Given a Float32 array, it returns a Float32 array.  Given a
# Float64 array, it returns a Float64 array.  For other arrays,
# transparent conversion to one of the supported input types is performed
# by the ufunc machinery.

m.add_ufunc("cos", "cos", 1, [('Float32', 'Float32'),
                              ('Float64', 'Float64')])

# The generate() method writes out the complete extension module to the
# specified C file.
m.generate("Src/_foo.c")

# ======================================================================
# Create foo's __init__.py for defining UFuncs corresponding to CFuncs
# in _foo.  make_stub() emits boiler-plate which makes your extension
# a package.  The __init__ file imports all the public symbols from
# C extension _foo making them visible as numarray.foo.

make_stub("Lib/__init__", "_foo")  

# ======================================================================
#
# Standard distutils setup for the generated code.
#
setup(name = "foo",
      version = "0.1",
      maintainer = "Todd Miller",
      maintainer_email = "jmiller@stsci.edu",
      description = "foo universal functions for numarray",
      url = "http://www.stsci.edu/projects/foo/",
      packages = ["numarray.foo"],
      package_dir = { "numarray.foo":"Lib" },
      ext_modules = [ NumarrayExtension( 'numarray.foo._foo',
                                         ['Src/_foo.c', 'Src/airy.c',
                                          'Src/const.c', 'Src/polevl.c']
                                         # libraries = ['ttf'],
                                         # include_dirs = ['include'],
                                         # library_dirs = ['lib']
                                         )
                      ]
      )

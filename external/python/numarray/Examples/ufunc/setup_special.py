""" This setup demonstrates how to use the numarray code generation
facilities to define additional ufuncs at the user level.  Universal
functions apply operators or C-functions elementwise to all of the
elements of a set of input and output arrays.

This script is a concrete demonstration of how to implement an airy()
universal function for numarray.  Since this capability is likely to
be used first to translate ufuncs implemented for Numeric/scipy for
numarray, I'll first discuss how (one variant of) the airy function
was declared in scipy using the Numeric ufunc API.

========================================================================

I.  How scipy implements airy() using Numeric

The ufunc, airy(), has an associated table of C function pointers
which handle different kinds of array input signatures:

extern int airy ( double x, double *ai, double *aip, double *bi, double *bip );
extern int cairy_fake(Py_complex z, Py_complex *ai, Py_complex *aip, 
		      Py_complex *bi, Py_complex *bip);

static void * airy_data[] = { (void *)airy, 
			      (void *)airy, 
			      (void *)cairy_fake, 
			      (void *)cairy_fake,};

At the Python level, there is only one universal function, airy(), but
at the C level, there is both airy (for double precision real) and
cairy_fake (for double precision complex).  cairy_fake() is something
I kludged up to keep this demo simple; it calls airy on the real and
imaginary components of the parameter arrays in two seperate calls.

Also associated with the ufunc is a table of array types, here reformatted
to highlight the true organization of 4 sets of ufunc call signatures,
one per line:

static char cephes_5c_types[] = {
   PyArray_FLOAT,  PyArray_FLOAT,  PyArray_FLOAT, PyArray_FLOAT, PyArray_FLOAT,
   PyArray_DOUBLE,  PyArray_DOUBLE, PyArray_DOUBLE, PyArray_DOUBLE, PyArray_DOUBLE,
   PyArray_CFLOAT, PyArray_CFLOAT, PyArray_CFLOAT, PyArray_CFLOAT, PyArray_CFLOAT,
   PyArray_CDOUBLE, PyArray_CDOUBLE, PyArray_CDOUBLE, PyArray_CDOUBLE, PyArray_CDOUBLE,
};

Combined, cephes_5c_types and airy_data say that "airy" will be used
for FLOAT and DOUBLE arrays, and cairy_fake will be used for CFLOAT
and CDOUBLE arrays.

Given a set of actual array parameters to the ufunc, the ufunc
matching code looks through the type table for the first set of array
signatures which is greater than or equal to the signatures of the
actual parameters.  For this type ordering, integer types are ranked
lower than floating point types which in turn are ranked lower than
complex types, e.g.  PyArray_INT < PyArray_FLOAT < PyArray_CFLOAT.
Within a type class, types are ordered by bit precision, so
PyArray_FLOAT < PyArray_DOUBLE.  When a particular set of array
parameters doees not exactly match the table signature, temporary
copies are made to achieve an exact match.  The match is performed top
to bottom, left to right, so this scheme really only supports a subset
of all possible type code combinations.  i.e., It would not make sense
to have one signature begin with PyArray_CDOUBLE and the one right
below it to begin with a lesser type, say PyArray_FLOAT.  This is
because a match would occur before the second signature was ever
examined and a slower but "more powerful" function would be used
instead of a faster but "less powerful" function.  This consideration
applies to numarray as well,  but numarray sorts the signatures at
runtime.

Each function pointer and signature has a corresponding type
conversion function; in the scipy special module these are organized
into reusable sets.  For airy(), the set was called cephes_1c_4:

cephes1c_4_functions[0] = PyUFunc_f_ffff_As_d_dddd;
cephes1c_4_functions[1] = PyUFunc_d_dddd;
cephes1c_4_functions[2] = PyUFunc_F_FFFF_As_D_DDDD;
cephes1c_4_functions[3] = PyUFunc_D_DDDD;

Looking at the first conversion function, given 1 FLOAT (f) input
array, and 4 FLOAT output arrays, the type converter will convert to
DOUBLE (d) for both inputs and outputs before/after calling the C
library function airy().  In the case of the second signature, DOUBLE
inputs and outputs are passed through unchanged.  The final two
converters manage CFLOAT (F) and CDOUBLE (D) arrays in a similar
fashion.

Each converter function converts a set of array inputs and outputs for
a single invocation of the core airy or cairy_fake function using
appropriate casts on a call by call basis.  As such, no extra storage
is required.  In contrast, were a PyArray_INT array fed into the
airy() ufunc, the type matching code would force conversion of the
array to the minimal matching type, PyArray_FLOAT, in a blockwise
fashion, forcing use of a slower mode of ufunc execution.  Avoiding
this "bulk" array type conversion is the principle motivation behind
adding multiple signatures, but a balance must be struck between
object code bloat, frequency of use, etc.  It should also be noted
that other factors, such as aligment or byteswapping due to record
array layout (PyFITS) can force "slow mode".

Finally, at startup, Numeric/scipy ties all these defining pieces of
information together with a single Numeric API call which constructs a
ufunc callable object.  This object, f, is later inserted into the
dictionary of the defining extension module and is directly usable on
arrays with the import of that single module.

f = PyUFunc_FromFuncAndData(cephes1c_4_functions, airy_data,
    cephes_5c_types, 4, 1, 4, PyUFunc_None, "airy", airy_doc, 0);

That's the basics for Numeric; I'm leaving out the code necessary to
set up a C extension module and register the Ufunc discussed above in
the extension dictionary.

========================================================================

II.  How to use the numarray code generation facilities to generate
a numarray version of airy()

This script eventually generates two files: _na_airymodule.c and
__init__.py.  The former defines an extension module which creates
numarray cfuncs, c helpers for the numarray airy() ufunc.  The latter
file includes Python code which automatically constructs numarray
universal functions (ufuncs) from the cfuncs in _na_airymodule.c.

First, we do some imports for generating code, building extensions
and distributions, etc.
"""

import distutils, os, sys
from distutils.core import setup
from numarray.codegenerator import UfuncModule, make_stub
from numarray.numarrayext import NumarrayExtension

"""
The generation of _na_specialmodule.c begins by constructing a
UfuncModule object which supports the accumulation of Cfunc code and
eventually spewing it all out.  UfuncModule is passed the name of the
Python extension which will be created, not the C source file.
Strictly speaking, CfuncModule would have been a better class name
since the extension defines helpers called "cfuncs".  Since it's sole
purpose is to implement a portion of a ufunc, UfuncModule will do.
"""

m = UfuncModule("_na_special")


"""
Code generation continues by adding miscellaneous includes and wrapper
code.  You can inject whatever code you need here, and it will get
inserted into the module whereever you call add_code().  Here I inject
the include which provides the airy() library function prototype:
"""

m.add_code('#include "airy.h"')

"""
The C library function which is ultimately called must have all input
parameters listed first and all output parameters listed subsequently.
Any return value for the C function must either be ignored or
designated as the first of the ufunc output arrays.  If the parameter
order of a library function doesn't satisfy these constraints, it must
be wrapped to re-arrange it's parameters.  Such additional wrappers
can be injected into the beginning of_na_specialmodule.c using the
add_code() method just described.

The following code is used to define an airy() universal function.
Two independent C library functions are unified under a single Python
universal function.  Each C function is supported by helpers which
manage both single and double precision arithmetic without the use of
temporary arrays.

Here we define the support for real numbers:
"""

m.add_nary_ufunc(ufunc_name = "airy",
                 c_function  = "airy",    
                 signatures  =["dxdddd",
                               "fxffff"],
                 c_signature = "dxdddd")

"""
Here we define the support for complex numbers:
"""

m.add_nary_ufunc(ufunc_name = "airy",
                 c_function  ="cairy_fake",
                 signatures  =["DxDDDD",
                               "FxFFFF"],
                 c_signature = "DxDDDD")

"""
There are four required parameters used to define a numarray ufunc:

1. function -- The Python level name of the function, "airy".  This
also becomes the C library function called unless the c_function
parameter is also defined.

2. signatures -- The list of actual array function signatures for
which code will be generated.  Calls which match these signatures
can avoid the creation of type matching temporary arrays.

3. c_function -- The name of C library function onto which *all* the
designated signatures will be cast.

4. c_signature -- This is the signature to which actual array
signatures must ultimately be cast.

A signature is a string of Numeric type codes, with input and output
codes seperated by "x".  

An unofficial 1:1 mapping of numarray types to Numeric typecodes is:

    'Bool'      : "B",  
    'Int8'      : "1",
    'UInt8'     : "b",
    'Int16'     : "s",
    'UInt16'    : "w",
     'Long'     : "l", # Long is an alias for Int32 or Int64.  Order matters.
    'Int32'     : "i",
    'UInt32'    : "u",
    'Int64'     : "N",
    'UInt64'    : "U",
    'Float32'   : "f",
    'Float64'   : "d",
    'Complex32' : "F",
    'Complex64' : "D"

The definitive mapping is in numarray.codegenerator.ufunccode.typecode.
Conspicuously absent are typecodes for 'Object':'O' and
'Character:'c'.  numarray doesn't support these with ufuncs.

Having dedicated code means that the ufunc will generally run faster
for that type signature because the arrays will not need to be
converted to other types before/after calling the function; any
conversions will happen as casts from the Cfunc actual array types to
the library function parameter types.

-----------------------------------------------------------------------

Here's a more advanced example, demonstrating a ufunc which is
optimized to operate on a single array and a set of scalars through
the generation of cfuncs which can handle scalars:

The c function could be inlined in the module like this:
"""
    
m.add_code('''
static double
vsdemo(double v, double s1, double s2, double s3, double s4)
{
     return v + s1 + s2 + s3 + s4;
}
''')

"""
Although you can't see it in this definition, the idea is that
parameters s1 .. s4 will be constant across the entire array
computation and so are handled specially to avoid the creation of
large temporaries or the use of numarray's blocking "slow mode".

The "forms" parameter is used below to designate 3 things:

a. Vector cfuncs for which s1..s4 are arrays.

b. Scalar cfuncs for which s1..s4 are single values across the entire
ufunc computation.

c. The C function is a true function and so returns a value.
"""

m.add_nary_ufunc(ufunc_name = "vsdemo",
                 c_function  = "vsdemo",    
                 signatures  =["dddddxd",
                               "fffffxd"],
                 c_signature = "dddddxd",
                 forms       = ["vvvvvxf",
                                "vssssxf"])

"""
The scalar cfuncs are not currently exploited by the numarray ufunc
runtime machinery so you should always generate vector forms even if
you only want the scalar case.  This will change as the n-ary ufunc
machinery matures and replaces the current unary and binary machinery.

-----------------------------------------------------------------------

Once all the cfuncs have been specified,  it's time to spew code:
"""

m.generate("Src/_na_specialmodule.c")

"""
The generated C-code includes helper functions which have the function
signatures and forms encoded in their names, similar in spirit to c++
name mangling.  The Cfunc helper bodies generated for this example are
named "airy_dxdddd_vxvvvv" and "airy_fxffff_vxvvvv".  For the first
cfunc, "dxdddd" is called the signature and "vxvvvv" is called the
form.  dxidddd describes *actual* array parameters, not c library
function parameters.  The form is described in more detail below

Finally, a Python stub module is needed which will construct Python
ufuncs from extension cfuncs at runtime.  The stub module (__init__ or
the package it corresponds to) is what users actually import to get at
ufuncs.  Here make_stub creates Lib/__init__.py and injects the
function plot_airy() into it.  This setup creates __init__ because the
ultimate goal is to create the numarray.special package.
"""

extra_stub_code = '''
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

'''

make_stub("Lib/__init__", "_na_special", add_code=extra_stub_code)

""" Lastly, a distribution needs to be constructed which will install
the extension and stubfile.  Note that the setup references
pre-existing library files which actually implement the airy function.

I called the cfunc extension _na_special to designate that it is (a)
incomplete and therefore private: _ (b) built for numarray only: na.
"""

dist = setup(name = "na_special",
      version = "0.1",
      maintainer = "Todd Miller",
      maintainer_email = "jmiller@stsci.edu",
      description = "airy() universal function for numarray",
      url = "http://www.scipy.org/",
      packages = ["numarray.special"],
      package_dir = { "numarray.special":"Lib" },
      ext_modules = [ NumarrayExtension( 'numarray.special._na_special',
                                         ['Src/_na_specialmodule.c',
                                          'Src/airy.c',
                                          'Src/const.c',
                                          'Src/polevl.c']
                                        )
                     ]
     )

"""
=================================================================
III. OPTIONAL: add_nary_ufunc() has a parameter called "forms" which
enables generation of code with some extra properties.  It specifies
the list of function "forms" for which dedicated code will be
generated.  If you don't specify "forms", it defaults to a list of a
single form which specifies that all inputs and outputs corresponding
to the type signature are vectors:

["v"*n_inputs + "x" + "v"*n_outputs]  # Python code

Forms are denoted using a syntax very similar to, and symmetric with,
type signatures: forms have single character codes which correspond
one-for-one with signature typecodes.

Forms have the following grammar:     # Not Python Code

form ::=  <inputs> "x" <outputs>
inputs ::= ["v"|"s"]*
outputs ::= ["f"]?["v"]* | "A" | "R"

The form character values have different meanings than for type
signatures:

'v'  :   vector,  an array of input or output values
's'  :   scalar,  a single input value input
'f'  :   function,  the c_function returns a value
'R'  :   reduce,    this binary ufunc needs a reduction method
'A'  :   accumulate this binary ufunc needs an accumulate method
'x'  :   separator  delineates inputs from outputs

Examples

The form for a C function which takes 4 input values, returns one value,
and fills in 2 additional output values is:

"vvvsxfvv"

Forms use "v" (vector) to designate array parameters and "s" (scalar)
to designate single values.

Using "s" to designate scalar parameters is a useful performance
optimization for cases where it is known that only a single value is
passed in from Python to be used in all calls to the c function.  This
prevents the blockwise expansion of the scalar value into a vector which
forces numarray to use "slow mode".

Using "f" is necessary to specify that you want to keep the C function
return value; it must always be the first output and will therefore
appear as the first element of the result tuple.

For ufuncs of two parameters (binary ufuncs), two additional form
characters are possible: A (accumulate) and R (reduce).  Each of these
characters constitutes the *entire* ufunc form, so the form is denoted
"R" or "A".   For these kinds of cfuncs,  the type signature always
reads <t>x<t> where <t> is one of the type characters.  Thus,  the
cfunc name for an add reduction with two integer parameters would
be written as: "add_ixi_R" and add accumulate as "add_ixi_A".

The set of cfuncs generated is based on the signatures *crossed* with
the forms.  Since that's an N^2 relationship, it causes code bloat
fast and should be used with care.  It is also possible to call
add_nary_ufunc several times for the same function, each time
specifying only a subset of the total forms and signatures, thus
preventing a full N^2 blowup.

"""


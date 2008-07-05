"""This module does code generation for numarray's type converters.

WARNING: This module exists solely as a mechanism to generate a
portion of numarray and is not intended to provide any
post-installation functionality.  
"""

from basecode import CodeGenerator, template, hasUInt64, hasFloat128
from basecode import _ADDCFUNC_TEMPLATE     

_numarray_to_numarray = {
  ("Complex", "Complex"): "tout->r = tin->r; tout->i = tin->i;",
  ("Complex", "default"): "*tout = <desttypecast>(tin->r);",
  ("default", "Complex"): "tout->r = *tin; tout->i = 0;",
  ("default", "default"): "*tout = <desttypecast>(*tin);"
}

def _complex_type(t):
  if t in ["Complex32", "Complex64"]:
    return "Complex"
  else:
    return "default"

def numarray_to_numarray(t1, t2):
  return _numarray_to_numarray[_complex_type(t1), _complex_type(t2)]
    

AS_PyVALUE_TEMPLATE = \
'''
static PyObject *<typename>asPyValue(void *data) {
  return Py_BuildValue(<pybuildchar>, <pycast>*((<typename> *) data));
}

SELF_CHECKED_CFUNC_DESCR(<typename>asPyValue, CFUNC_AS_PY_VALUE);
'''

COMPLEX_AS_PyVALUE_TEMPLATE = \
'''
static PyObject *<typename>asPyValue(void *data) {
  return PyComplex_FromDoubles(((<typename> *) data)->r, 
                               ((<typename> *) data)->i);
}

SELF_CHECKED_CFUNC_DESCR(<typename>asPyValue, CFUNC_AS_PY_VALUE);
'''

# From_PyVALUE uses a (relatively) backwards convention for return status.
#   0 --> failure.  non-zero --> success.  This is actually my preferred
#   style as well, but *not* what the rest of numarray does.
#   Most ufuncs use the convention -1 for error and 0 for success.
FROM_PyVALUE_TEMPLATE = \
'''
static int <typename>fromPyValue(PyObject *value, void *dataptr) {
    <typename> *data = (<typename> *) dataptr;
    if (!PyNumber_Check(value))
        return 0;
    else {
        if (PyLong_Check(value)) {
            *data = <typecast>(PyLong_AsLongLong(value));
        } else if (PyInt_Check(value)) {
            *data = <typecast>(PyInt_AsLong(value));
        } else if (PyFloat_Check(value)) {
            *data = <typecast>(PyFloat_AsDouble(value));
        } else if (PyComplex_Check(value)) {
           *data = <typecast>(PyComplex_RealAsDouble(value));
        } else {
            return 0;
        }
        if (PyErr_Occurred())
           return 0;
        else
           return 1;
    }
}

SELF_CHECKED_CFUNC_DESCR(<typename>fromPyValue, CFUNC_FROM_PY_VALUE);
'''

COMPLEX_FROM_PyVALUE_TEMPLATE = \
'''
static int <typename>fromPyValue(PyObject *value, void *dataptr) {
    <typename> *data = (<typename> *) dataptr;
    if (!PyNumber_Check(value))
        return 0;
    else {
        if (PyLong_Check(value)) {
            data->r = PyLong_AsLong(value);
            data->i = 0;
        } else if (PyInt_Check(value)) {
            data->r = PyInt_AsLong(value);
            data->i = 0;
        } else if (PyFloat_Check(value)) {
            data->r = PyFloat_AsDouble(value);
            data->i = 0;
        } else if (PyComplex_Check(value)) {
           data->r = PyComplex_RealAsDouble(value);
           data->i = PyComplex_ImagAsDouble(value);
        } else {
            return 0;
        }
        if (PyErr_Occurred())
           return 0;
        else
           return 1;
    }
}

SELF_CHECKED_CFUNC_DESCR(<typename>fromPyValue, CFUNC_FROM_PY_VALUE);
'''

CONVERSION_TEMPLATE = \
'''
static int <typename>as<desttypename>(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
    <typename>     *tin  = (<typename> *)     buffers[0];
    <desttypename> *tout = (<desttypename> *) buffers[1];
    BEGIN_THREADS
    for (i=0; i<niter; i++, tout++, tin++) {
        <numarray_to_numarray>
    }
    END_THREADS
    return 0;
}

UFUNC_DESCR2(<typename>as<desttypename>, sizeof(<typename>), sizeof(<desttypename>));
'''

# ============================================================================
#          IMPORTANT:  no <>-sugared strings below this point

# translate <var> --> %(var)s in templates seen *so far*
template.sugar_dict(globals())  

# ============================================================================
    
#*********************************************************************#
#                   data lists for type conversion                    #
#*********************************************************************#

# These are used by the code to generate dictionaries with the keys
# defined in tdictfields and the corresponding values in typeconfig
# list items. There will be one dictionary for each element in
# typeconfig. It is done this way to make the table more readable
# and easier to edit.

typeconfig = [
    ["Bool",   "(Bool) isNonZERO",   "PY_BOOL_CHAR",   ""],
    ["Int8",   "(Int8)",             "PY_INT8_CHAR",   ""],
    ["Int16",  "(Int16)",            "PY_INT16_CHAR",  ""],
    ["Int32",  "(Int32)",            "PY_INT32_CHAR",  ""],
    ["UInt32",  "(UInt32)",          "PY_UINT32_CHAR",  ""],
    ["UInt8",  "(UInt8)",            "PY_UINT8_CHAR",  "(Int16)"],
    ["UInt16", "(UInt16)",           "PY_UINT16_CHAR", "(Int32)"],
    ["Float32","(Float32)",          "PY_FLOAT32_CHAR",""],
    ["Float64","(Float64)",          "PY_FLOAT64_CHAR",""],
    ["Complex32", "NUM_TO_COMPLEX",  "PY_COMPLEX32_CHAR",""],
    ["Complex64", "NUM_TO_COMPLEX",  "PY_COMPLEX64_CHAR",""],
    ]

typeconfig.append(["Int64", "(Int64)", "PY_LONG_CHAR", ""])
if hasUInt64():
  typeconfig.append(["UInt64", "(UInt64)", "PY_LONG_CHAR", ""])

if hasFloat128():
  typeconfig.append(["Float128", "(Float128)", "PY_FLOAT128_CHAR", ""])
  typeconfig.append(["Complex128", "NUM_TO_COMPLEX", "PY_COMPLEX128_CHAR", ""])

class ConvParams:
    def __init__(self, typename, typecast, pybuildchar, pycast):
        self.typename = typename
        self.typecast = typecast
        self.pybuildchar = pybuildchar
        self.pycast = pycast

class ConvCodeGenerator(CodeGenerator):
    def __init__(self, *components):
        CodeGenerator.__init__(self, *components)
        self.module = "_conv"
        self.qualified_module = "numarray._conv"
        
    def addcfunc(self, type, name):
        CodeGenerator.addcfunc(self, type+name, key=repr((type, name)))

    def gen_body(self):
        """Generates the repetitive sections of code for conversions"""

        # Iterate over the "from" datatype.
        for cfg1 in typeconfig:
            t1 = apply(ConvParams, cfg1)
            if t1.typename in ["Complex32","Complex64"]:
              frompy = COMPLEX_FROM_PyVALUE_TEMPLATE
              aspy = COMPLEX_AS_PyVALUE_TEMPLATE
            else:
              frompy = FROM_PyVALUE_TEMPLATE
              aspy = AS_PyVALUE_TEMPLATE
            self.codelist.append((self.separator + aspy + frompy) % \
                                 t1.__dict__)
            self.addcfunc(t1.typename, "asPyValue")
            self.addcfunc(t1.typename, "fromPyValue")
            
            # Iterate over the "to" datatype
            for cfg2 in typeconfig:
                t2 = apply(ConvParams, cfg2)
                if cfg1 == cfg2:
                    continue
                t1.desttypename = t2.typename
                t1.desttypecast = t2.typecast
                t1.numarray_to_numarray = numarray_to_numarray(t1.typename, t2.typename) % t1.__dict__
                self.codelist.append(CONVERSION_TEMPLATE % t1.__dict__)
                
                typetup = repr((t1.typename, t2.typename))
                name = t1.typename + "as" + t2.typename
                self.funclist.append(_ADDCFUNC_TEMPLATE % (typetup, name))

generate_conv_code  = ConvCodeGenerator()


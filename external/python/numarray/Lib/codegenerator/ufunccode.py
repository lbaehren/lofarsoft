from basecode import CodeGenerator, template, _TAIL, hasUInt64, all_types
import re

#*********************************************************************#
#                       data for _ufuncmodule.c                       #
#*********************************************************************#

UFUNC_HEADER = \
'''
#include <Python.h>
#include <stdio.h>
#include <math.h>
#include "libnumarray.h"

#ifdef MS_WIN32
#pragma warning(once : 4244)
#endif

#define logical_and(arg1, arg2) (arg1 != 0) & (arg2 != 0)
#define logical_or(arg1, arg2)  (arg1 != 0) | (arg2 != 0)
#define logical_xor(arg1, arg2) ((arg1 != 0) ^ (arg2 != 0)) & 1
#define ufmaximum(arg1, arg2) (((temp1=arg1) > (temp2=arg2)) ? temp1 : temp2)
#define ufminimum(arg1, arg2) (((temp1=arg1) < (temp2=arg2)) ? temp1 : temp2)

#define distance3d(x,y,z) sqrt(x*x + y*y + z*z)

'''

# Vector ufunc of M inputs and N outputs and void
UFUNC_VECTOR = \
'''
static int <cname>(long niter, long ninargs, long noutargs, void **buffers, long *bsizes) {
    long i;
<parameter_declarations><optional_declarations>
    BEGIN_THREADS
    for (i=0; i<niter; i++) {
        <input_assignments>
        <operator>
        <result_assigments>
	<pointer_increments>
    }
    END_THREADS
    return 0;
}

static CfuncDescriptor <cname>_descr =
{ "<cname>", (void *) <cname>, CFUNC_UFUNC, 0, CHECK_ALIGN, <inputs>, <outputs>,
  <sizes>, <vsmask> };
'''

UFUNC_REDUCE = \
'''
static void _<cname>(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    <intype0>  *tin0   = (<intype0> *) ((char *) input  + inboffset);
    <outtype0> *tout0  = (<outtype0> *) ((char *) output + outboffset);
    <outtype0> net;
    <optional_declarations>
    if (dim == 0) {
        net = *tout0;
        for (i=1; i<niters[dim]; i++) {
            tin0 = (<intype0> *) ((char *) tin0 + inbstrides[dim]);
            <operator>
        }
        *tout0 = net;
    }
    else {
        for (i=0; i<niters[dim]; i++) {
             _<cname>(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int <cname>(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
            _<cname>(dim, dummy, niters, input, inboffset, inbstrides,
                     output, outboffset, outbstrides );
            END_THREADS
            return 0;
}

STRIDING_DESCR2(<cname>, CHECK_ALIGN, sizeof(<intype0>), sizeof(<outtype0>));
'''

UFUNC_ACCUMULATE = \
'''
static void _<cname>(long dim, long dummy, maybelong *niters,
            void *input,  long inboffset,  maybelong *inbstrides,
            void *output, long outboffset, maybelong *outbstrides) {
    long i;
    <intype0> *tin0   = (<intype0> *) ((char *) input  + inboffset);
    <outtype0> *tout0 = (<outtype0> *) ((char *) output + outboffset);
    <outtype0> lastval;
    <optional_declarations>
    if (dim == 0) {
        for (i=1; i<niters[dim]; i++) {
            lastval = *tout0;
            tin0 = (<intype0> *) ((char *) tin0 + inbstrides[dim]);
            tout0 = (<outtype0> *) ((char *) tout0 + outbstrides[dim]);
            <operator>
        }
    }
    else {
        for (i=0; i<niters[dim]; i++) {
            _<cname>(dim-1, dummy, niters,
               input,  inboffset  + i*inbstrides[dim],  inbstrides,
               output, outboffset + i*outbstrides[dim], outbstrides);
        }
    }
}

static int  <cname>(long dim, long dummy, maybelong *niters,
                    void *input,  long inboffset,  maybelong *inbstrides,
                    void *output, long outboffset, maybelong *outbstrides) {
            BEGIN_THREADS
                _<cname>(dim, dummy, niters,
                        input, inboffset, inbstrides, output,
                        outboffset, outbstrides);
            END_THREADS
            return 0;
}

STRIDING_DESCR2(<cname>, CHECK_ALIGN, sizeof(<intype0>), sizeof(<outtype0>));
'''

# The following templates are the most complex. There are a number of
# different forms of templates, for unary and binary functions, and then
# different forms for the computation line (operator, function plus
# special versions for integer multiply and divide for overflow and
# divide by zero checking. More may be necesary in the future. These
# are defined here since most are used multiple times in the ufunc
# definition table.
# td stands for 'template dict'

def _combine(*dicts):
    r = {}
    for d in dicts:
        r.update(d)
    return r

macro1_td = {
    "operator_vxv"          :"<operator>(*tin0, *tout0);"
    }

macro2_cum_td = {
    "operator_R"            :"<operator>(net, *tin0, net);",
    "operator_A"            :"<operator>(lastval ,*tin0, *tout0);"
    }
macro2_nocum_td = {
    "operator_vvxv"         :"<operator>(*tin0, *tin1, *tout0);",
    "operator_vsxv"         :"<operator>(*tin0, tin1, *tout0);",
    "operator_svxv"         :"<operator>(tin0, *tin1, *tout0);",
    }
macro2_td = _combine(macro2_cum_td, macro2_nocum_td)

function1_td = {
    "operator_vxf"          :"*tout0 = <operator>(*tin0);"
    }

function2_cum_td = {
    "operator_R"            :"net   = <operator>(net, *tin0);",
    "operator_A"            :"*tout0 = <operator>(lastval ,*tin0);"
    }
function2_nocum_td = {
    "operator_vvxf"         :"*tout0 = <operator>(*tin0, *tin1);",
    "operator_vsxf"         :"*tout0 = <operator>(*tin0, tin1);",
    "operator_svxf"         :"*tout0 = <operator>(tin0,  *tin1);",
    }
function2_td = _combine(function2_cum_td, function2_nocum_td)

function3_td = {
    "operator_vvvxf"         :"*tout0 = <operator>(*tin0, *tin1, *tin2);",
    "operator_vssxf"         :"*tout0 = <operator>(*tin0, tin1, tin2);",
    }

operator1_td = {
    "operator_vxv"          :"*tout0 = <operator>*tin0;"
    }

operator2_nocum_td = {
    "operator_vvxv"         :"*tout0 =   *tin0 <operator> *tin1;",
    "operator_vsxv"         :"*tout0 =   *tin0 <operator> tin1;",
    "operator_svxv"         :"*tout0 =    tin0 <operator> *tin1;",
    }
operator2_cum_td = {
    "operator_R"            :"net    =     net <operator> *tin0;",
    "operator_A"            :"*tout0 = lastval <operator> *tin0;"
    }
operator2_td = _combine(operator2_cum_td, operator2_nocum_td)

int_divide_td = {
    "operator_vvxv"         :"*tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, *tin0) : *tin0 <operator> *tin1);",
    "operator_vsxv"         :"*tout0 = ((tin1==0) ? int_dividebyzero_error(tin1, *tin0) : *tin0 <operator> tin1);",
    "operator_svxv"         :"*tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, 0) : tin0 <operator> *tin1);",
    "operator_R"            :"net = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0) : net <operator> *tin0);",
    "operator_A"            :"*tout0 = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0) : lastval <operator> *tin0);"
    }

int_truedivide_td = {
    "operator_vvxv"         :"*tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, *tin0) : *tin0         <operator> (double) *tin1);",
    "operator_vsxv"         :"*tout0 = (( tin1==0) ? int_dividebyzero_error( tin1, *tin0) : *tin0         <operator> (double) tin1);",
    "operator_svxv"         :"*tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, 0)     : tin0          <operator> (double) *tin1);",
    "operator_R"            :"net    = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0)     : net           <operator> (double) *tin0);",
    "operator_A"            :"*tout0 = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0)     : lastval       <operator> (double) *tin0);"
    }

int_floordivide_td = {
    "operator_vvxv"         :"*tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, *tin0) : floor(*tin0   <operator> (double) *tin1));",
    "operator_vsxv"         :"*tout0 = (( tin1==0) ? int_dividebyzero_error( tin1, *tin0) : floor(*tin0   <operator> (double)  tin1));",
    "operator_svxv"         :"*tout0 = ((*tin1==0) ? int_dividebyzero_error(*tin1, 0)     : floor(tin0    <operator> (double) *tin1));",
    "operator_R"            :"net    = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0)     : floor(net     <operator> (double) *tin0));",
    "operator_A"            :"*tout0 = ((*tin0==0) ? int_dividebyzero_error(*tin0, 0)     : floor(lastval <operator> (double) *tin0));"
    }

int8_check = \
"""if (temp > 127) temp = int_overflow_error(127.);
if (temp < -128) temp = int_overflow_error(-128.);"""

uint8_check = \
"""if (temp > 255) temp = int_overflow_error(255.);"""

int16_check = \
"""if (temp > 32767) temp = int_overflow_error(32767.);
if (temp < -32768) temp = int_overflow_error(-32768);"""

uint16_check = \
"""if (temp > 65535) temp = int_overflow_error(65535.);"""

int32_check = \
"""if (temp > 2147483647) temp = (Float64) int_overflow_error(2147483647.);
if (temp < (-2147483648.)) temp = (Float64) int_overflow_error(-2147483648.);"""

uint32_check = \
"""if (temp > 4294967295U) temp = (Float64) int_overflow_error(4294967295.);"""

int64_check = \
"""if (smult64_overflow(<p1>, <p2>)) temp = (Float64) int_overflow_error(4611686018427387903.);"""

uint64_check = \
"""if (umult64_overflow(<p1>, <p2>)) temp = (Float64) int_overflow_error(9223372036854775807.);"""

# A template used below to generate multiple templates! (since they
# are all so similar. These are used by make_int_template_dict just below.
intX_td = {
    "operator_vvxv": ("*tin0",   "*tin1", "*tout0"),
    "operator_vsxv": ("*tin0",   "tin1",  "*tout0"),
    "operator_svxv": ("tin0",    "*tin1", "*tout0"),
    "operator_R"   : ("net",     "*tin0", "net"),
    "operator_A"   : ("lastval", "*tin0", "*tout0"),
    }

intX_mult_template = """temp = ((<uptype>) <p1>) %<operator> ((<uptype>) <p2>);
    <checkstr>
    <p3> = (<type>) temp;"""

def make_int_template_dict(uptype, sizestr, checkstr):
    td = {}
    for key in intX_td.keys():
        td[key] = intX_mult_template % \
                {"type": sizestr,
                 "checkstr": (checkstr % { "p1":intX_td[key][0],
                                           "p2":intX_td[key][1] }),
                 "uptype":uptype,
                 "p1":intX_td[key][0],
                 "p2":intX_td[key][1],
                 "p3":intX_td[key][2],
                 }
    return td

    
#*********************************************************************#
#                   data lists for ufunc generation                   #
#*********************************************************************#
# Some implicit rules:
# Bool never returned by math functions
# Bool always returned by comparison functions
# Math operators (+-*/**) generally return all types, double math functions
#  only return float types, special purpose functions (floor, ceil, etc)
#  only return one kind (int or float)
# Define various forms for signatures. New ufuncs may require new
# signatures lists. These are used in the ufunc definition table
# repeatedly.

operator_sigs = [
    ('Bool',    'Int8'),
    ('Int8',    'Int8'),
    ('Int16',   'Int16'),
    ('Int32',   'Int32'),
    ('UInt8',   'UInt8'),
    ('UInt16',  'UInt16'),
    ('UInt32',  'UInt32'),
    ('Float32', 'Float32'),
    ('Float64', 'Float64')
    ]

mathfunction_sigs = [
    ('Bool',    'Float64'),
    ('Int8',    'Float64'),
    ('Int16',   'Float64'),
    ('Int32',   'Float64'),
    ('UInt8',   'Float64'),
    ('UInt16',  'Float64'),
    ('UInt32',  'Float64'),
    ('Float32', 'Float32'),
    ('Float64', 'Float64')
    ]

comparison_sigs = [
    ('Bool',    'Bool'),
    ('Int8',    'Bool'),
    ('Int16',   'Bool'),
    ('Int32',   'Bool'),
    ('UInt8',   'Bool'),
    ('UInt16',  'Bool'),
    ('UInt32',   'Bool'),
    ('Float32', 'Bool'),
    ('Float64', 'Bool')
    ]

logical_sigs = [
    ('Bool','Bool')
    ]

int_sigs = [
    ('Bool',    'Bool'),
    ('Int8',    'Int8'),
    ('Int16',   'Int16'),
    ('Int32',   'Int32'),
    ('UInt8',   'UInt8'),
    ('UInt16',  'UInt16'),
    ('UInt32',  'UInt32')
    ]

truedivide_int_sigs = [
  ('Bool',    'Float32'),
  ('Int8',    'Float32'),
  ('Int16',   'Float32'),
  ('Int32',   'Float32'),
  ('UInt8',   'Float32'),
  ('UInt16',  'Float32'),
  ('UInt32',  'Float32')
  ]

operator_sigs.append(('Int64', 'Int64'))
mathfunction_sigs.append(('Int64', 'Float64'))
comparison_sigs.append(('Int64', 'Bool'))
int_sigs.append(('Int64', 'Int64'))
truedivide_int_sigs.append(('Int64', 'Float64'))


if hasUInt64():
  operator_sigs.append(('UInt64', 'UInt64'))
  mathfunction_sigs.append(('UInt64', 'Float64'))
  comparison_sigs.append(('UInt64', 'Bool'))
  int_sigs.append(('UInt64', 'UInt64'))
  truedivide_int_sigs.append(('UInt64', 'Float64'))

float_sigs = [
    ('Float32', 'Float32'),
    ('Float64', 'Float64')
    ]

complex_complex_sigs = [
    ('Complex32', 'Complex32'),
    ('Complex64', 'Complex64')
    ]

complex_real_sigs = [
    ('Complex32', 'Float32'),
    ('Complex64', 'Float64')
    ]

complex_bool_sigs = [
    ('Complex32', 'Bool'),
    ('Complex64', 'Bool')
    ]
    
opt_mult_decl = "Int32 temp;"
opt_mult32_decl = "Float64 temp;"
opt_mult64_decl = "Int64 temp;"
opt_minmax_decl = "<intype0> temp1, temp2;"   # !!! template expansion here.


# ============================================================================
#          IMPORTANT:  no <>-sugared strings below this point

# translate <var> --> %(var)s in templates seen *so far*
template.sugar_dict(globals())  

# ============================================================================
    
# Use function to define all variants of integer multiply.
multiply_int8_td   = make_int_template_dict("Int32",   "Int8",   int8_check)
multiply_uint8_td  = make_int_template_dict("Int32",   "UInt8",  uint8_check)
multiply_int16_td  = make_int_template_dict("Int32",   "Int16",  int16_check)
multiply_uint16_td = make_int_template_dict("Int32",   "UInt16", uint16_check)
multiply_uint32_td = make_int_template_dict("Float64", "UInt32", uint32_check)
multiply_int32_td  = make_int_template_dict("Float64", "Int32",  int32_check)
multiply_int64_td  = make_int_template_dict("Int64",   "Int64",  int64_check)
multiply_uint64_td = make_int_template_dict("UInt64",  "UInt64", uint64_check)

# These are used by the code to generate dictionaries with the keys
# defined in ufdictfields and the corresponding values in ufuncconfigs
# list items. There will be one dictionary for each element in
# ufuncconfigs. It is done this way to make the table more readable
# and easier to edit.

# Multiple entries for function names are permitted as long as the
# signatures have no overlaps.

# pyname          i  o  form,operator         sigs                  optional_decls

ufuncconfigs = [
["_distance3d",   (function3_td, "distance3d"),  float_sigs, ""],

["minus",         (operator1_td, "-"),        operator_sigs, ""],
["add",           (operator2_td, "+"),        operator_sigs,""],
["subtract",      (operator2_td, "-"),        operator_sigs, ""],
["multiply",      (multiply_int8_td,  "*"),  [('Int8', 'Int8')], opt_mult_decl],
["multiply",      (multiply_uint8_td,  "*"), [('UInt8', 'UInt8')], opt_mult_decl],
["multiply",      (multiply_int16_td,  "*"), [('Int16', 'Int16')], opt_mult_decl],
["multiply",      (multiply_uint16_td,  "*"),[('UInt16', 'UInt16')], opt_mult_decl],
["multiply",      (multiply_int32_td,  "*"), [('Int32', 'Int32')], opt_mult32_decl],
["multiply",      (multiply_uint32_td,  "*"), [('UInt32', 'UInt32')], opt_mult32_decl],
["multiply",      (multiply_int64_td,  "*"), [('Int64', 'Int64')], opt_mult64_decl],
["multiply",      (multiply_uint64_td,  "*"), [('UInt64', 'UInt64')], opt_mult64_decl],
["multiply",      (operator2_td, "*"),   float_sigs, ""],
["divide",        (int_divide_td,"/"),  int_sigs, ""],
["divide",        (operator2_td, "/"),   float_sigs, ""],
["floor_divide",  (int_floordivide_td,"/"),  int_sigs, ""],
["floor_divide",  (macro2_td, "NUM_FLOORDIVIDE"), float_sigs, ""],
["true_divide",   (int_truedivide_td,"/"),  truedivide_int_sigs, ""],
["true_divide",   (operator2_td, "/"),   float_sigs, ""],

# hack in template.Template makes @mod@ result in %

["remainder",     (int_divide_td, " @mod@ "), int_sigs, ""],
["remainder",     (function2_td, "fmod"),   float_sigs, ""],

["power",         (function2_td, "num_pow"),   operator_sigs, ""],
["abs",           (function1_td, "fabs"),  operator_sigs, ""],
["sin",           (function1_td, "sin"),  mathfunction_sigs, ""],
["cos",           (function1_td, "cos"),  mathfunction_sigs, ""],
["tan",           (function1_td, "tan"),  mathfunction_sigs, ""],
["arcsin",        (function1_td, "asin"), mathfunction_sigs, ""],
["arccos",        (function1_td, "acos"), mathfunction_sigs, ""],
["arctan",        (function1_td, "atan"), mathfunction_sigs, ""],
["arctan2",       (function2_td, "atan2"), mathfunction_sigs, ""],
["log",           (function1_td, "num_log"),  mathfunction_sigs, ""],
["log10",         (function1_td, "num_log10"),mathfunction_sigs, ""],
["exp",           (function1_td, "exp"),  mathfunction_sigs, ""],
["sinh",          (function1_td, "sinh"), mathfunction_sigs, ""],
["cosh",          (function1_td, "cosh"), mathfunction_sigs, ""],
["tanh",          (function1_td, "tanh"), mathfunction_sigs, ""],
["arcsinh",       (function1_td, "num_asinh"), mathfunction_sigs, ""],
["arccosh",       (function1_td, "num_acosh"), mathfunction_sigs, ""],
["arctanh",       (function1_td, "num_atanh"), mathfunction_sigs, ""],

["ieeemask",      (function2_nocum_td, "NA_IeeeMask32"), [(('Float32', 'Int32'), ('Bool',))], "", None, ['vvxf', 'vsxf'] ],
["ieeemask",      (function2_nocum_td, "NA_IeeeMask64"), [(('Float64', 'Int32'), ('Bool',))], "", None, ['vvxf', 'vsxf'] ],

["isnan",         (function1_td, "NA_isnan32"), [(('Float32',), ('Bool',))], "", None, ['vxf']  ],
["isnan",         (function1_td, "NA_isnan64"), [(('Float64',), ('Bool',))], "", None, ['vxf']  ],
["isnan",         (function1_td, "NA_isnanC32"), [(('Complex32',), ('Bool',))], "", None, ['vxf']  ],
["isnan",         (function1_td, "NA_isnanC64"), [(('Complex64',), ('Bool',))], "", None, ['vxf']  ],

["sqrt",          (function1_td, "sqrt"), mathfunction_sigs, ""],
["equal",         (operator2_nocum_td, "=="), comparison_sigs, ""],
["not_equal",     (operator2_nocum_td, "!="), comparison_sigs, ""],
["greater",       (operator2_nocum_td, ">"),  comparison_sigs, ""],
["greater_equal", (operator2_nocum_td, ">="),  comparison_sigs, ""],
["less",          (operator2_nocum_td, "<"),  comparison_sigs, ""],
["less_equal",    (operator2_nocum_td, "<="), comparison_sigs, ""],

["logical_and",   (function2_nocum_td, "logical_and"), comparison_sigs, ""],
["logical_or",    (function2_nocum_td, "logical_or"), comparison_sigs, ""],
["logical_xor",   (function2_nocum_td, "logical_xor"), comparison_sigs, ""],
["logical_and",   (function2_cum_td, "logical_and"), logical_sigs, ""],
["logical_or",    (function2_cum_td, "logical_or"), logical_sigs, ""],
["logical_xor",   (function2_cum_td, "logical_xor"), logical_sigs, ""],

["logical_not",   (operator1_td, "!"),  comparison_sigs, ""],

["bitwise_and",   (operator2_td, "&"),  int_sigs, ""],
["bitwise_or",    (operator2_td, "|"),  int_sigs, ""],
["bitwise_xor",   (operator2_td, "^"),  int_sigs, ""],

["bitwise_not",   (operator1_td, "~"),  int_sigs[1:], ""],
["bitwise_not",   (function1_td, "BOOLEAN_BITWISE_NOT"), int_sigs[0:1], ""],

["rshift",        (operator2_nocum_td, ">>"),  int_sigs, ""],
["lshift",        (operator2_nocum_td, "<<"),  int_sigs, ""],

["floor",         (function1_td, ""), int_sigs, ""],
["floor",         (function1_td, "floor"), float_sigs, ""],
["ceil",          (function1_td, ""), int_sigs, ""],
["ceil",          (function1_td, "ceil"), float_sigs, ""],
["maximum",       (function2_td, "ufmaximum"), operator_sigs, opt_minmax_decl],
["minimum",       (function2_td, "ufminimum"), operator_sigs, opt_minmax_decl],

["fabs",          (function1_td, "fabs"),  mathfunction_sigs, ""],
["_round",        (function1_td, "num_round"), mathfunction_sigs, ""],
["hypot",         (function2_td, "hypot"), mathfunction_sigs, ""],

# Complex ufuncs

["minus",         (macro1_td, "NUM_CMINUS"), complex_complex_sigs, ""],
["add",           (macro2_td, "NUM_CADD"), complex_complex_sigs, ""],
["subtract",      (macro2_td, "NUM_CSUB"), complex_complex_sigs, ""],
["multiply",      (macro2_td, "NUM_CMUL"),   complex_complex_sigs, ""],
["divide",        (macro2_td, "NUM_CDIV"),   complex_complex_sigs, ""],
["true_divide",   (macro2_td, "NUM_CDIV"),   complex_complex_sigs, ""],
["remainder",     (macro2_td, "NUM_CREM"), complex_complex_sigs, ""],
["power",         (macro2_td, "NUM_CPOW"),   complex_complex_sigs, ""],
["abs",           (function1_td, "NUM_CABS"), complex_real_sigs, ""],
["sin",           (macro1_td, "NUM_CSIN"),  complex_complex_sigs, ""],
["cos",           (macro1_td,    "NUM_CCOS"),  complex_complex_sigs, ""],
["tan",           (macro1_td, "NUM_CTAN"),  complex_complex_sigs, ""],

["arcsin",        (macro1_td, "NUM_CASIN"), complex_complex_sigs, ""],
["arccos",        (macro1_td, "NUM_CACOS"), complex_complex_sigs, ""],
["arctan",        (macro1_td, "NUM_CATAN"), complex_complex_sigs, ""],
["arcsinh",       (macro1_td, "NUM_CASINH"), complex_complex_sigs, ""],
["arccosh",       (macro1_td, "NUM_CACOSH"), complex_complex_sigs, ""],
["arctanh",       (macro1_td, "NUM_CATANH"), complex_complex_sigs, ""],

# ["arctan2",     (function2_td, "atan2"), mathfunction_sigs, ""],

["log",          (macro1_td, "NUM_CLOG"),  complex_complex_sigs, ""],
["log10",        (macro1_td, "NUM_CLOG10"), complex_complex_sigs, ""],
["exp",          (macro1_td, "NUM_CEXP"),  complex_complex_sigs, ""],
["sinh",         (macro1_td, "NUM_CSINH"), complex_complex_sigs, ""],
["cosh",         (macro1_td, "NUM_CCOSH"), complex_complex_sigs, ""],
["tanh",         (macro1_td, "NUM_CTANH"), complex_complex_sigs, ""],
["sqrt",         (macro1_td, "NUM_CSQRT"),  complex_complex_sigs, ""],

# Strictly speaking, these are implemented as macros as well, but they
# return real valued (Bool) expressions rather than fill in a 3rd
# Complex parameter.
["equal",        (function2_nocum_td, "NUM_CEQ"), complex_bool_sigs, ""],
["not_equal",    (function2_nocum_td, "NUM_CNE"), complex_bool_sigs, ""],
["greater",       (function2_nocum_td, "NUM_CGT"),  complex_bool_sigs, ""],
["greater_equal", (function2_nocum_td, "NUM_CGE"),  complex_bool_sigs, ""],
["less",          (function2_nocum_td, "NUM_CLT"),  complex_bool_sigs, ""],
["less_equal",    (function2_nocum_td, "NUM_CLE"), complex_bool_sigs, ""],

["logical_and",  (function2_nocum_td, "NUM_CLAND"), complex_bool_sigs, ""],
["logical_or",   (function2_nocum_td, "NUM_CLOR"), complex_bool_sigs, ""],
["logical_xor",  (function2_nocum_td, "NUM_CLXOR"), complex_bool_sigs, ""],
["logical_not",  (function1_td, "NUM_CLNOT"),  complex_bool_sigs, ""],
# ["bitwise_and",  (operator2_td, "&"),  int_sigs, ""],
# ["bitwise_or",   (operator2_td, "|"),  int_sigs, ""],
# ["bitwise_xor",  (operator2_td, "^"),  int_sigs, ""],
# ["bitwise_not",  (operator1_td, "~"),  int_sigs, ""],
# ["rshift",    (operator2_nocum_td, ">>"),  int_sigs, ""],
# ["lshift",    (operator2_nocum_td, "<<"),  int_sigs, ""],
["floor",       (macro1_td, "NUM_CFLOOR"), complex_complex_sigs, ""],
["ceil",        (macro1_td, "NUM_CCEIL"),   complex_complex_sigs, ""],
["maximum",     (function2_td, "NUM_CMAX"), complex_complex_sigs, ""],
["minimum",     (function2_td, "NUM_CMIN"), complex_complex_sigs, ""],
["fabs",        (macro1_td, "NUM_CFABS"),  complex_complex_sigs, ""],
["_round",      (macro1_td, "NUM_CROUND"), complex_complex_sigs, ""],
["hypot",       (macro2_td, "NUM_CHYPOT"), complex_complex_sigs, ""],
]

class UFuncParams:
    def __init__(self, pyname, td_opname, sigs,
                 opt_declarations, template = None, forms=None):
        self.pyname = pyname

        self.td, self.operator = td_opname

        self.opt_decls = opt_declarations

        # Select appropriate base template form.
        self.template = template

        # Extract forms from operator template keys
        self.forms = []
        self.sigs = {}
        for operator_key in self.td.keys():
            form = operator_key.split("_")[1]
            self.forms.append(form)
            self.sigs[form] = []

            if "x" in form:
                ins, outs = tuple(map(len, form.split("x")))
            elif form in ["R","A"]:
                ins, outs = 1, 1
            else:
                raise ValueError("Bad form in operator key of function '%s'" % (pyname,))            
            for s in sigs:

                # Normalize shorthand signatures to implied MxN form
                if len(s) == 2 and type(s[0]) == type(""):
                    s = ((s[0],)*ins, (s[1],)*outs)

                self.sigs[form].append(s)
            
            # for operator_vxv, operator_svxv, operator_vsxv, operator_vvxv...:
            #    Substitute actual operator/function for <operator>
            for key, value in self.td.items():
                self.__dict__[key] = value % self.__dict__ 
        
                
# =================================================================

# Operator encoding:

# The following single letter codes are used to encode the form
# of a ufunc.  For instance, what was formerly called the
# binary ufunc form "vector_vector" is now "vvxv".  Similarly,
# ufunc form "vector_scalar" is now "vsxv" and unary ufunc form
# "vector" is simply "vxv".

# x   == ufunc input / output delimiter
# R   == reduce i/o delimeter
# A   == accumulate i/o delimiter

# v   == value, dereferenced pointer
# V   == value no pointer increment

# p   == pointer, non-dereferenced pointer
# P   == pointer w/o increment

# f   == function return value, dereferenced pointer
# F   == function return value, dereferenced pointer, w/o increment

# s   == scalar, similar to V, except dereferenced at declaration time

typecode = {
    'Bool'      : "B",  
    'Int8'      : "1",
    'UInt8'     : "b",
    'Int16'     : "s",
    'UInt16'    : "w",
    # 'Long'    : "l", # Long is an alias for Int32 or Int64.  Order matters.
    'Int32'     : "i",
    'UInt32'    : "u",
    'Int64'     : "N",
    'UInt64'    : "U",
    'Float32'   : "f",
    'Float64'   : "d",
    'Complex32' : "F",
    'Complex64' : "D",
    'Object'    : "O"
    }

invtypecode = {}
for key,val in typecode.items():
    invtypecode[val] = key

_typecode_values = typecode.values() + ["x"]

_COMPLEX_TYPES = ["Complex32", "Complex64"]

def _typecodes(types):
    """Returns the Numeric single character type codes corresponding to a list
    of numarray types for use in encoding the parameter list in the C function
    name.
    """
    try:
        codes = [ typecode[ t ] for t in types ]
    except KeyError:
        raise TypeError, "Invalid numarray type in ufunc signature:" + repr(t)
    return "".join(codes)

def _encode_sig(signature):
    if isinstance(signature, tuple):
        return _typecodes(signature[0])+"x"+_typecodes(signature[1])
    else:        
        for t in signature:
            if t not in _typecode_values:
                raise TypeError, "Invalid typecode in ufunc signature"
        return signature

def _encode_cname(function, signature, form):
    """Returns the c function name for a given Python function name,
    signature, and form.
    """
    return "_".join([function, _encode_sig(signature), form])

def _make_parameter_list(intypes, lib_intypes, outtypes, lib_outtypes, form):
    "assembles a parameter list for a C function call"
    inputs, outputs = len(intypes), len(outtypes)
    sig = form.split("x")
    plist = "("
    for i in range(inputs):
        if sig[0][i] in "vV":
            if (intypes[i] != lib_intypes[i] and
                ((intypes[i] in _COMPLEX_TYPES) or
                 (lib_intypes[i] in _COMPLEX_TYPES))):
                plist += "input%d, " % (i,)
            else:
                plist += "*tin%d, " % (i,)
        else:
            if (intypes[i] in _COMPLEX_TYPES and intypes[i] != lib_intypes[i]):
                plist += "&input%d, " % (i,)
            else:
                plist += "tin%d, " % (i,)
    for i in range(outputs):
        if sig[1][i] not in "fF":
            if outtypes[i] != lib_outtypes[i]:
                plist += "&result%d, " % (i,)
            else:
                plist += "tout%d, " % (i,)			
    return plist[:-2] + ");"

def _make_pointer_increments(name, nargs, sig):
    """Increments array pointers following each function call."""
    inclist = ""
    for i in range(nargs):
        if sig[i] in "vpf":
            inclist += "++%s%d; " % (name, i,)
    return inclist

def _make_parameter_declarations(function, kind, types, sig, offset=0):
    """Generates variable declarations assigning elements of the ufunc buffer
    array to numbered local i/o pointers and values.
    """
    parameter_declarations = ""
    for i in range(len(types)):
        if sig[i] in "vVpPfF": # vector
            parameter_template = "    %-10s *%s%d =  (%s *) buffers[%d];\n" 
        elif sig[i] == "s": # scalar
            parameter_template = "    %-10s  %s%d = *(%s *) buffers[%d];\n"
        elif sig[i] in "AR":
            return ""
        else:
            raise ValueError("Bad form character '%s' in declaration of '%s'." % (sig[i], function,))
        parameter_declarations += parameter_template % \
                                  (types[i], kind, i, types[i], i+offset)
    return parameter_declarations

def _make_local_declarations(name, types, lib_types):
    locals = ""
    for i in range(len(types)):
        if types[i] != lib_types[i]:
            if (name != "input" or
                (types[i] in _COMPLEX_TYPES) or
                (lib_types[i] in _COMPLEX_TYPES)):
                locals += "    %s     %s%d;\n" % (lib_types[i], name, i)
    return locals

def _make_input_assigments(intypes, lib_intypes):
    assignments = ""
    for i in range(len(intypes)):
        if intypes[i] != lib_intypes[i]:
            if intypes[i] in _COMPLEX_TYPES:
                assignments += "        input%d.r = tin%d->r;\n" % (i,i)
                assignments += "        input%d.i = tin%d->i;\n" % (i,i)
            elif lib_intypes[i] in _COMPLEX_TYPES:
                if intypes[i] in _COMPLEX_TYPES:
                    assignments += "        input%d.r = tin%d->r;\n" % (i,i)
                else:
                    assignments += "        input%d.r = *tin%d;\n" % (i,i)                    
                assignments += "        input%d.i = 0;\n" % (i,)                
    return assignments

def _make_result_assigments(outtypes, lib_outtypes):
    assignments = ""
    for i in range(len(outtypes)):
        if outtypes[i] != lib_outtypes[i]:
            if outtypes[i] in _COMPLEX_TYPES:
                assignments += "        tout%d->r = result%d.r;\n" % (i,i)
                assignments += "        tout%d->i = result%d.i;\n" % (i,i)
            elif lib_outtypes[i] in _COMPLEX_TYPES:
                assignments += "        *tout%d = result%d.r;\n" % (i, i)
            else:
                assignments += "        *tout%d = result%d;\n" % (i,i)
    return assignments

def _typesizes(types):
    """Returns declaration for array of type sizes for use in the Cfunc
    declaration.  The Cfunc decl needs to know the element size of each
    incoming array buffer in order to do alignment and bounds checking.
    """
    sizes = "{ "
    for t in types:
        sizes += "sizeof(%s), " % (t,)
    return sizes[:-2] + " }"

def _vector_scalar_mask(form):
    """The vsmask marks buffers as either of length niter items or as
    pseudo buffers containing scalars which should not be length checked.
    """
    vsmask = "{ "
    for f in form:
        if f == "s":
            vsmask += "1, "
        else:
            vsmask += "0, "
    vsmask = vsmask[:-2] + " }"
    return vsmask

def _normalize_signature(sig):
    """Converts string form or tuple form to two type tuples + lengths"""
    if isinstance(sig, type("")):
        incodes, outcodes  = sig.split("x")
        intypes, outtypes = [], []
        for i in incodes:
            intypes.append(invtypecode[i])
        for o in outcodes:
            outtypes.append(invtypecode[o])
        return tuple(intypes), tuple(outtypes)
    else:
        return sig

def _make_function(function, signature, form=None, operator=None,
                   optional_declarations="", extra_template_vars={},
                   model=UFUNC_VECTOR, c_function=None, c_signature=None):
    # function -- ufunc name
    # c_function  -- c library function
    # cname -- cfunc helper function name
    intypes, outtypes  = _normalize_signature(signature)
    inputs, outputs = len(intypes), len(outtypes)

    if c_function is None:
        c_cfunction = function
        
    if c_signature is not None:
        c_intypes, c_outtypes = _normalize_signature(c_signature)
    else:  # default to
        c_intypes, c_outtypes = intypes, outtypes    
    
    if form is None:
        form = "v"*inputs + "x" + "v"*outputs
        
    sig = form.split("x")
	
    parameter_declarations = _make_parameter_declarations(
        function, "tin", intypes, sig[0])
    parameter_declarations += _make_parameter_declarations(
        function, "tout", outtypes, sig[1], offset=len(intypes))
    parameter_declarations += _make_local_declarations("input", intypes, c_intypes)
    parameter_declarations += _make_local_declarations("result", outtypes, c_outtypes)
    
    input_assignments = _make_input_assigments(intypes, c_intypes)
    pointer_increments = _make_pointer_increments("tin", inputs, sig[0])
    pointer_increments += _make_pointer_increments("tout", outputs, sig[1])
    result_assigments = _make_result_assigments(outtypes, c_outtypes)

    vsmask = _vector_scalar_mask( form )
    
    sizes = _typesizes(intypes+outtypes)
    
    cname = _encode_cname(function, signature, form)
    
    if operator is None:
        parameter_list = _make_parameter_list(
            intypes, c_intypes, outtypes, c_outtypes, form)
        operator = c_function + parameter_list
        if sig[1][0] in "fF":
            if outtypes[0] != c_outtypes[0]:
                operator = "result0 = " + operator
            else:
                operator = "*tout0 = " + operator

    extra_template_vars.update(locals())
    del extra_template_vars["extra_template_vars"]
    
    return str(template.Template(model, **extra_template_vars))


class UFuncCodeGenerator(CodeGenerator):

    _cfuncdict_header='''
static PyObject *init_funcDict(void) {
    PyObject *dict, *keytuple, *cfunc;
    dict = PyDict_New();
'''

    def __init__(self, *components):
        CodeGenerator.__init__(self, *components)

    def gen_function(self, function, signature, form=None, operator=None,
                     extra_vars={ "optional_declarations":""},
                     model=UFUNC_VECTOR):

        cname = _encode_cname(function, signature, form)
        extra_vars["cname"] = cname
        
        if "A" in form:
            code = str(template.Template(UFUNC_ACCUMULATE, **extra_vars))
        elif "R" in form:
            code = str(template.Template(UFUNC_REDUCE, **extra_vars))
        else:
            code = str(_make_function(function, signature, form, operator,
                                  extra_vars["optional_declarations"], extra_vars))            
        self.codelist.append(code)
        inform = re.split("x", form)[0]
        self.addcfunc(cname, function, inform, signature)

    def gen_body(self, Type):
        """Generates the repetitive sections of code.   Tuned to
        implementing the numarray ufunc configuration table rather
        than general purpose use.
        """
        self.module = "_ufunc" + Type
        self.qualified_module = "numarray._ufunc" + Type

        for cfg in self.config:  # Loop over ufunc table

            ufo = apply(UFuncParams, cfg)
            
            if self.separator is not None:
                self.codelist.append(self.separator % ufo.pyname)

            # for each form, add 
            for form in ufo.forms:

                # Loop over signatures for given ufunc
                for sig in ufo.sigs[form]:

                    ufo.ninputs = len(sig[0])
                    ufo.noutputs = len(sig[1])
                
                    for i in range(ufo.ninputs):
                        setattr(ufo, "intype%d" % i, sig[0][i]);
                    for i in range(ufo.noutputs):
                        setattr(ufo, "outtype%d" % i, sig[1][i]);

                    if ufo.intype0 != Type:
                        continue

                    # Expand opt_decls in terms of ufo vars
                    ufo.optional_declarations = ufo.opt_decls % ufo.__dict__

                    ufo.operator = getattr(ufo, "operator_"+form)
                    
                    self.gen_function(ufo.pyname, sig, form,
                                      ufo.operator, ufo.__dict__)

    def addcfunc(self, name, function, inform, signature):
        self.funclist.append('    /* %s */\n' % name)
        fmt='"ss(('
        fmt+='s'*len(signature[0])
        fmt+=')('
        fmt+='s'*len(signature[1])
        fmt+='))"'
        arglist=['"%s"' % i
                 for i in (function,inform)+signature[0]+signature[1] ]
        codestr='    keytuple=Py_BuildValue(%s,%s);\n'
        self.funclist.append(codestr % (fmt,",".join(arglist)))
        dictstr ='    cfunc = (PyObject*)NA_new_cfunc((void*)&%s_descr);\n' % name
        dictstr+='    PyDict_SetItem(dict,keytuple,cfunc);\n\n'
        dictstr+='    Py_DECREF(keytuple);\n'
        dictstr+='    Py_DECREF(cfunc);\n\n'
        self.funclist.append(dictstr)

#*********************************************************************#

generate_ufunc_code = UFuncCodeGenerator(
    UFUNC_HEADER, _TAIL, ufuncconfigs,
    "/*********************  %s  *********************/\n")

# =================================================================

class UfuncModule:
    """UfuncModule objects support the generation of numarray ufunc extension
    modules with methods and by maintaining the following state:
    
    _ufuncs   a list of ufunc configuration tuples.
    _code     a list of code snippets to insert into the module.
    
    """
    def __init__(self, module):
        self.module = module # module name
        self._ufuncs = [] # Ufunc configurations
        self._cfuncs = [] # NA_add_cfunc runtime registrations
        self._code = []   # inline code
        
    def add_code(self, code):
        """inserts the specified c-code into the module."""
        self._code.append(code)

    def _normalize(self, sig, unary_binary):
        """Converts (in,out) signature to (N*(in,), M*(out,)) form."""
        if unary_binary == 2:
            return ((sig[0],)*2,(sig[1],))
        else:
            return sig

    def add_ufunc(self, ufunc_name, c_name, unary_binary=1,
                  signatures="math", style="function"):
        """generates c-functions which handle a set of type signatures
        for a single ufunc:
        
        ufunc_name    the name of the ufunc being defined.
        
        c_name        the name of the C-function or macro to be applied elementwise.
        
        unary_binary  1 -- unary ufunc  2 -- binary ufunc  None -- nary
        
        signatures    Specifies a set of input,output type pairs for        
                      which to generate code.  When a ufunc is called,
                      type matching selects the "closest matching"
                      generated code to operate on the input types;
                      mis-matches result in blockwise type conversion
                      to the nearest type.
        
        There are pre-defined signature sets:
        
        set                     ins        out
        
        "float"    --         ('Float32', 'Float32')
                              ('Float64', 'Float64')
        
        "math"     --         ('Bool',    'Float64')
                              ('Int8',    'Float64')
                              ('Int16',   'Float64')
                              ('Int32',   'Float64')
                              ('UInt8',   'Float64')
                              ('UInt16',  'Float64')
                              ('UInt32',  'Float64')
                              ('Float32', 'Float32')
                              ('Float64', 'Float64')
        
        "int"              -- ('Bool',    'Bool')
                              ('Int8',    'Int8')
                              ('Int16',   'Int16')
                              ('Int32',   'Int32')
                              ('UInt8',   'UInt8')
                              ('UInt16',  'UInt16')
                              ('UInt32',  'UInt32')
        
        "complex_complex"  -- ('Complex32', 'Complex32')
                              ('Complex64', 'Complex64')

        "complex_real"     -- ('Complex32', 'Float32')
                              ('Complex64', 'Float64')
                                        
        "complex_bool"     -- ('Complex32', 'bool')
                              ('Complex64', 'bool')
                                        
        Alternately, a list of tuples of strings may be used:

                              [('Int32','Int32'), 'Float64','Float64')]

        style         "function" or "macro"
        
        "function" -- c_func is a function of 1 or 2 parameters
                      returning a single value.
        
        "macro"    -- c_func is a macro of 2 or 3 parameters,
                      returning nothing, where the last parameter
                      accepts the output value.
"""
        if style in ['function','macro']:   # original API
            style += str(unary_binary) + "_td"
            template_dict = globals()[style]
        elif type(style) == type(""):       # e.g. "operator_td"
            template_dict = globals()[style]
        elif type(style) == type({}):       # e.g.  ufunccode.operator2_td
            template_dict = style           # assumed to be user generated td.

        sigs_mapping = {
            "math"    : mathfunction_sigs,
            "comparison" : comparison_sigs,
            "int"     : int_sigs,
            "float"   : float_sigs,
            "complex_complex" : complex_complex_sigs,
            "complex_real" : complex_real_sigs,
            "complex_bool" : complex_bool_sigs,
            }
        
        if signatures in sigs_mapping.keys(): # named standard sig list
            sigs = [ self._normalize( sig, unary_binary ) \
                     for sig in sigs_mapping[signatures] ]
        else:
            sig0 = signatures[0]
            if (len(sig0[0]) == 2 and
                isinstance(sig0[0], tuple) and
                isinstance(sig0[1], tuple) and
                isinstance(sig0[0][0], type("")) and
                isinstance(sig0[1][0], type(""))):  # ((ins...), (outs...)) normalized
                sigs = signatures
            elif (len(sig0) == 2 and
                  isinstance(sig0[0], type("")) and
                  isinstance(sig0[1], type(""))):   # ((in,), (out,)) abbreviated
                sigs = [ self._normalize(sig, unary_binary) \
                         for sig in signatures ]
            else:
                raise ValueError("Supported signatures are:" + str(sigs_mapping.keys()))
                    
        cfg = ( ufunc_name, (template_dict, c_name), sigs, "" )
        self._ufuncs.append(cfg)

    def addcfunc(self, name, key):
        self._cfuncs = (name, key)

    def add_nary_ufunc(self, ufunc_name, signatures, forms=None,
                       c_function=None,c_signature=None):
        """Generates code for (signatures X forms) cfuncs."""
        sig0 = signatures[0]
        ins, outs = map(len, sig0.split("x"))
        for sig in signatures:
            cins, couts  = map(len, sig.split("x"))
            if cins != ins or couts != outs:
                raise ValueError("Inconsistent signature I/O counts")
                
        if forms is None:   # default vector form.
            if outs > 1: 
                forms = [ "v"*ins + "x" + "v"*outs ] 
            else:
                forms = [ "v"*ins + "x" + "f"*outs ] 

        for s in signatures:
            for f in forms:
                self.add_code(_make_function( ufunc_name, s, f,
                      c_function=c_function, c_signature=c_signature))
                cname = _encode_cname(ufunc_name, s, f)
                inform = re.split("x", f)[0]
                normsig = _normalize_signature(s)
                self._cfuncs.append((cname, ufunc_name, inform, normsig))

    def generate(self, filename):
        """emits the code for the entire module to the specified file."""
        gen = UFuncCodeGenerator(UFUNC_HEADER, _TAIL, self._ufuncs, None)
        # "/*********************  %s  *********************/\n")
        gen.gen_header()
        gen.codelist.extend(self._code)

        for cfg in self._ufuncs:
            ufo = UFuncParams(*cfg)

        for c in self._cfuncs:
            gen.addcfunc(*c)
        
        for t in all_types():
            gen.gen_body(t)
        gen.module = self.module
        gen.gen_trailer()
        gen.emit_code(filename)

def make_stub(module, c_module, add_code=""):
    """Generates Python code to define Ufuncs in 'module' corresponding to
    the Cfuncs in 'c_module'."""
    f = open(module+".py", "w")
    f.write("""'''This module contains GENERATED CODE which constructs numarray universal
functions from C-coded cfuncs contained in a sister extension module.

***************************** DO NOT EDIT **************************
'''

import %s
import numarray.ufunc as _uf

globals().update(_uf.make_ufuncs(%s))

%s

""" % (c_module, c_module, add_code))


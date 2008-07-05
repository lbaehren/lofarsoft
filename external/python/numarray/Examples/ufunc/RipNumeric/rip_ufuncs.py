"""This module is run as a utility to "rip" ufunc specifications from
Numeric source code.  It was used to generate the specifications for
the scipy.special port to numarray.  The regular expressions used to
"rip" the ufuncs are no doubt brittle and incomplete but better than
nothing.  They worked for me but are dependent on the regularity of
the source code being ripped so they may not work for you without
updating this module or reformatting your Numeric ufunc source.

This script is run as follows:

% python rip_ufuncs.py nc_cephesmodule.c  >na_cephes_table.py

Where nc_cephesmodule.c was the original Numeric ufunc source code.

It may help to reformat the Numeric source code a little to match
declarations like the following:

cephes3a_functions[0] = PyUFunc_fff_f_As_iid_d;
cephes3a_functions[1] = PyUFunc_ddd_d_As_iid_d;

static void * bdtrc_data[] = { (void *)bdtrc, (void *)bdtrc, };

PyUFunc_FromFuncAndData(cephes4_2_functions, oblate_radial2_nocv_data, cephes_6_types, 2, 4, 2, PyUFunc_None, "obl_rad2", obl_rad2_doc, 0);

"""

import re, sys
import warnings

space =  r"\s*"
field =  r"\s*(\w+)\s*"
comma =  r"\s*,\s*"
number = r"\s*(\d+)\s*"
quote =  r'"'
string = r'".*"'
field_or_string = "(" + field + "|" + string + ")"

ufunc = r"PyUFunc_FromFuncAndData" + space + r"\(" + field + comma + field + comma + field + comma + number + comma + number + comma + number + comma + field + comma + quote + field + quote + comma + field_or_string + comma + number + space + r"\)"

class RipException(Exception):
    pass

def devoid(fptrs):
    void = "\s*\(\s*void\s*\*\s*\)\s*(\w+)"
    results = []
    for f in fptrs:
        try:
            results.append(re.search(void, f).group(1))
        except:
            raise RipException("can't devoid:" + repr(f))
    return results    

def cfuncs(fname, fptr_data_array, ntypes):
    for l in open(fname).readlines():
        exp = "\s+" + fptr_data_array + "\s*\[\s*\]\s*=\s*\{" + \
              "\s*([^,}]+),?\s*"*ntypes
        m = re.search(exp, l)
        if m is not None:
            return devoid(m.groups())
    raise RipException("No match for fptr_data_array:", repr(fptr_data_array))
                              
def ufunc_types(fname, functions, ntypes):
    results = []
    for l in open(fname).readlines():
        m = re.search(functions + "\[\d+\]\s+=\s+(\w+)", l)
        if m is not None:
            generic_stub = m.group(1)
            results.append(generic_stub)
    return results

def getsigs(generic_stub):
    """'PyUFunc_fF_F_As_dD_D', 'PyUFunc_dD_D'"""
    sections = generic_stub.split("_")
    if len(sections) == 3:  # ttt_ttt
        ufunc_in_types = "x".join(sections[1:])
        cfunc_types = ufunc_in_types
    elif len(sections) == 6: # ttt_ttt_As_TTT_TTT
        ufunc_in_types = "x".join(sections[1:3])
        cfunc_types = "x".join(sections[4:])
    else:
        raise RipException("Unrecognized ufunc types:" + repr(generic_stub))
    return cfunc_types, ufunc_in_types

def table_tuples(ufunc, ntypes, nin, nout, generic_stubs, cfuncs):
    if nout == 1:
        forms = ["v"*nin + "x" + "f"]
        # if forms == ["vvxf"]:
        #    forms.extend(["vsxf","svxf"])
    else:
        forms = ["v"*nin + "x" + "v"*nout ]
    d = {}
    for i, f in enumerate(cfuncs):
        if d.has_key(f):
            d[f].append(generic_stubs[i])
        else:
            d[f] = [generic_stubs[i]]
    tuples = []
    for f in d.keys():
        cts, uts = [], []
        for stub in d[f]:
            ct, ut = getsigs(stub)
            cts.append(ct)
            uts.append(ut)
        for ct in cts:
            if ct != cts[0]:
                raise RipException("Inconsistent signatures for cfunction " + repr(f) + " in ufunc " + repr(ufunc))
        tuples.append( (ufunc, f, ct, forms, uts) )
    return tuples

def reformat(table):
    table.sort()
    print "### Table below: ufunc_name,c_function, c_signature, forms, signatures"
    print "func_tab = ["
    for r in table:
        s = "(%20s,%30s,%15s,%15s,%40s)," % (repr(r[0]), repr(r[1]), repr(r[2]), repr(r[3]), repr(r[4]))
        print s
    print "]"
        
def main(fname):
    table = []
    for l in open(fname).readlines():
        m = re.search(ufunc, l)
        if m is not None:
            # types derived here, both ufunc and c.
            signature_functions = m.group(1)
            # identifies ultimate c-function or wrapper
            c_functions = m.group(2)   
            ntypes = eval(m.group(4))
            nin = eval(m.group(5))
            nout = eval(m.group(6))
            ufuncname = m.group(8)
            try:
                uts = ufunc_types(fname, signature_functions, ntypes)
                cfs = cfuncs(fname, c_functions, ntypes)
                # print ufuncname, ntypes, nin, nout, uts, cfs
                if len(cfs) != ntypes:
                    raise RipException("Fewer C functions than types")
                table.extend(table_tuples(ufuncname, ntypes, nin, nout, uts, cfs))
            except RipException, s:
                warnings.warn("Problem with ufunc: " + repr(ufuncname) + " " + str(s))
    reformat(table)

main(sys.argv[1])

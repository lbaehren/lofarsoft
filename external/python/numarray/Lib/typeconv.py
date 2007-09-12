"""typeconv: Initialize tables of all type conversions

$Id: typeconv.py,v 1.2 2002/10/25 17:06:09 jaytmiller Exp $
"""

import _conv

# list of all supported types
numtypes = []

# Organize _conv functions to make intialization of TypeConverter simple.
typeconvfuncs = {}
for key in _conv.functionDict.keys():
    typename, functionKey = eval(key)
    if not typeconvfuncs.has_key(typename):
        numtypes.append(typename)
        typeconvfuncs[typename] = {}
    typeconvfuncs[typename][functionKey] = _conv.functionDict[key]

# Now check to see that the effective type/type matrix is complete.
# Perhaps a check is needed (but isn't done) to see if there are extra entries
for typename1 in numtypes:
    for typename2 in numtypes + ['asPyValue','fromPyValue']:
        if not typeconvfuncs[typename1].has_key(typename2):
            if typename1 != typename2:
                raise ValueError("Missing type conversion functions: %s to %s"  \
                    % (typename1,typename2))


class TypeConverter:

    """A container for all functionality to do type conversions for a given type

    Attributes are pointers to C functions:

    asPyValue: takes a C data pointer and returns a PyObject (value)
    fromPyValue: takes a PyObject (value) pointer and a C data pointer and places
                 the converted value at the data pointer
    """

    def __init__(self, name):
        self.name = name   # probably unnecessary
        tdict = typeconvfuncs[name]
        self.asPyValue = tdict['asPyValue']
        self.fromPyValue = tdict['fromPyValue']
        del tdict['asPyValue']
        del tdict['fromPyValue']
        self.astype = tdict

typeConverters = {}
for typename in numtypes:
    typeConverters[typename] = TypeConverter(typename)

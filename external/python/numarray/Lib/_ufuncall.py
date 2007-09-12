import numinclude

types = "Bool Int8 UInt8 Int16 UInt16 Int32 UInt32 Float32 Float64 Complex32 Complex64".split()

types.append("Int64")
if numinclude.hasUInt64:
    types.append("UInt64")

_modules = []

for t in types:
    m = "_ufunc" + t
    exec("import "+m)
    exec("_modules.append(%s)"  % m)

functionDict = {}
for m in _modules:
    functionDict.update(m.functionDict)

for t in types:
    m = "_ufunc" + t
    exec("del "+m)

del numinclude, m, _modules, t, types

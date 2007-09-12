from distutils.core import Extension

import sys, os, re
from os.path import normpath

VERSION = "1.5.2"
VERSION_HEX = "0x01050200"

NUMARRAY_PACKAGES = ["numarray", "numarray.codegenerator"]

def config_item(s):
    switch = "--" + s
    if switch in sys.argv:
	    sys.argv.remove(switch)
	    return True
    elif os.environ.has_key(s.upper()):
        return eval(os.environ[s.upper()])
    else:
        return False

def normalize(pathlist):
	if isinstance(pathlist, list):
		return [ normalize(p) for p in pathlist ]
	elif isinstance(pathlist, tuple):
		return tuple([ normalize(p) for p in pathlist ])
	elif isinstance(pathlist, dict):
		d = {}
		for key, value in pathlist.items():
			d[key] = normalize(value)
		return d
	elif isinstance(pathlist, str):
		return normpath(pathlist)
	else:
		return pathlist
		
NUMARRAY_PACKAGE_DIRS = {"numarray":"Lib",
                         "numarray.codegenerator":"Lib/codegenerator"}

MODULES = ["_conv",
           "_sort",
           "_bytes",
           "_ufunc",
           "_ufuncBool",
           "_ufuncInt8",
           "_ufuncUInt8",
           "_ufuncInt16",
           "_ufuncUInt16",
           "_ufuncInt32",
           "_ufuncUInt32",
           "_ufuncFloat32",
           "_ufuncFloat64",
           "_ufuncComplex32",
           "_ufuncComplex64",
           "_ndarray",
           "_numarray",
           "_chararray",
           "_objectarray",
           "memory",
           "_converter",
           "_operator",
           "_ufunc",
           "_numerictype",
           "libnumarray",
           "libnumeric",
           ]

NUMARRAY_DATA_FILES = [('numarray', ['LICENSE.txt',normpath('Lib/testdata.fits')])]

EXTRA_LINK_ARGS = ["-lm"]
EXTRA_COMPILE_ARGS = []

# compile c-modules with timing code which generally has to be
# added manually.
if "--timing" in sys.argv:
    MODULES.append("libteacup")
    EXTRA_COMPILE_ARGS.append("-DMEASURE_TIMING")

# Specify this to build numarray on an unthreaded Python.
if "--unthreaded" in sys.argv:
    EXTRA_COMPILE_ARGS.append("-DUNTHREADED")
    sys.argv.remove("--unthreaded")

# Specify this to try to exploit multiple processors.  A work
# in progress...
if "--smp" in sys.argv:
    EXTRA_COMPILE_ARGS.append("-DNA_SMP=1")
    sys.argv.remove("--smp")

USE_LAPACK = config_item("use_lapack")
USE_ABSOFT = config_item("use_absoft")
SELFTEST   = config_item("selftest")

GENCODE    = config_item("gencode")
GENUFUNCS  = GENCODE or config_item("genufuncs")
GENAPI     = GENCODE or config_item("genapi")

HAS_UINT64 = True

codeargs = ""
if sys.platform == "osf1V5":
    EXTRA_COMPILE_ARGS.extend(["-ieee"])
elif sys.platform == "linux2":
    if sys.maxint > 0x7FFFFFFF:
        pass
    else:
        pass
elif sys.platform == "sunos5":
    pass
elif sys.platform == "win32":
    if sys.version.find("MSC") >= 0 and sys.version.find("v.13") < 0:
        HAS_UINT64 = False
elif sys.platform == "cygwin":
    EXTRA_LINK_ARGS += ["-L/lib", "-lm", "-lc", "-lgcc", "-L/lib/mingw", "-lmingwex"]
elif sys.platform == "darwin":
    EXTRA_COMPILE_ARGS.extend(["-Ddarwin"])
elif sys.platform == "irix646":
    pass
elif sys.platform == "freebsd4-i386":
    pass
else:
    pass

MODULES.extend(["_ufuncInt64"]) 
if HAS_UINT64:
    codeargs = "--hasUInt64"
    MODULES.extend(["_ufuncUInt64"]) 

if (not os.path.exists("Include/numarray/numconfig.h") or
    not os.path.exists("Src/convmodule.c")):
    GENCODE = True

def _locate_headers():
    for a in sys.argv:
        if "--local" == a[:len("--local")]:
            words = a.split("=")
            return os.path.join(words[1].strip().rstrip(), "numarray")
    else:
        py_version_short = "%d.%d" % sys.version_info[:2]
        base = sys.exec_prefix
        template = {
            "win32"     : '%(base)s/Include/numarray',
            "darwin"    : '%(base)s/include/python%(py_version_short)s/numarray',
            "posix"     : '%(base)s/include/python%(py_version_short)s/numarray',
            }
        try:
            return normpath(template[sys.platform] % locals())
        except KeyError:
            return normpath(template["posix"] % locals())

def locate_headers():  # windows \n meta-coding fix
    return re.sub(r"\\", r"\\\\", _locate_headers())                  

EXTRA_COMPILE_ARGS = EXTRA_COMPILE_ARGS
EXTRA_LINK_ARGS = EXTRA_LINK_ARGS

class OurExtension(Extension):
    """OurBaseExtension is an Extension with implicit include_dirs,
    extra_compile_args, and extra_link_args.  Used to construct our
    c-api shared library.
    """
    def _set_default(self, keys, key, default):
	    if (not keys.has_key(key) or keys[key] is None):
		    keys[key] = default
		    
    def __init__(self, name, sources, **keys):
	    self._set_default(keys, "extra_compile_args", EXTRA_COMPILE_ARGS)
	    self._set_default(keys, "extra_link_args", EXTRA_LINK_ARGS)
	    self._set_default(keys, "include_dirs", [])
	    keys["include_dirs"].append("Include/numarray")
	    keys = normalize(keys)
	    Extension.__init__(self, name, normalize(sources), **keys)

NUMARRAY_EXTENSIONS = [ OurExtension("numarray."+m, ["Src/"+ m +"module.c"]) 
			for m in MODULES ]

def prepare(modules):
    print "Using EXTRA_COMPILE_ARGS =",EXTRA_COMPILE_ARGS

    # Generate numarray configuration header file
    
    f = open(normpath("Include/numarray/numconfig.h"),"w")
    f.write("""
/* This file is generated by setup.py.  DO NOT EDIT. */

#define NUMARRAY_VERSION "%s"
#define NUMARRAY_VERSION_HEX %s

""" % (VERSION, VERSION_HEX))
    f.close()
    del f

    for a in sys.argv:
	if a.startswith("--install-headers"):
	    INCLUDE_DIR = a.split("=")[1]
	    break
    else:
	INCLUDE_DIR = locate_headers()

    #  Generate the numinclude.py, the numarray configuration module.
    f = open("Lib/numinclude.py", "w")
    f.write("""
# This file is generated by setup.py.  DO NOT EDIT.

import _ndarray
import os

include_dir = '%s'
version     = '%s'
hasUInt64   = _ndarray.hasUInt64()
LP64        = _ndarray.lp64()

if not len(include_dir):   # default to same directory as numarray .py's
   import numinclude
   include_dir = os.sep.join(numinclude.__file__.split(os.sep)[:-1])
   
""" % ( INCLUDE_DIR, VERSION))
    f.close()

    python = sys.executable + " "
    if GENUFUNCS:
        os.system(python + " " + normpath("Lib/codegenerator/__init__.py") 
			 + " " + codeargs)
    if GENAPI:
        os.system(python + " " + normpath("Include/numarray/genapis.py") 
			 + " " + codeargs)

# if __name__ == "__main__":
prepare(MODULES)

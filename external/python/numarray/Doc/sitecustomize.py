import sys
import os
import glob

#
# This module should be placed somewhere on your PYTHONPATH,
# say the "root" directory for a number of user level packages.  It
# enables that one user directory to behave like a mini site-packages
# directory.
#
# This module processes all of the .pth files in the
# directory where it is located.  All of the relative paths in each .pth are
# extended by the absolute path of the "root" directory and then 
# added to sys.path, effectively extending PYTHONPATH to each path
# decribed somewhere in a .pth file.  Thus, by adding only the root
# directory to your PYTHONPATH, and by sticking this file there, you
# can import any of the packages installed there as well.
#
# e.g.
#
# /usr/ra
# /usr/ra/sitecustomize.py
# /usr/ra/numarray/...
# /usr/ra/numarray.pth
# /usr/ra/pyraf/...
# /usr/ra/pyraf.pth
#
# Here, only /usr/ra needs to be put on PYTHONPATH and both "numarray" and
# "pyraf" become importable.
#

modinfo = repr(sys.modules["sitecustomize"])
# print modinfo
modpath = modinfo.split(" ")[-1][1:-2]
# print modpath
where = os.path.dirname(modpath)

local_pths = glob.glob(os.path.join(where,"*.pth"))
for pth in local_pths:
    for d in open(pth).readlines():
        path = os.path.join(where, d.strip())
        if path not in sys.path:
            print "Adding", `path`, "to sys.path"
            sys.path.append(path)

# Do the "real" sitecustomize...
sys.path.remove(where)
del sys.modules["sitecustomize"]
import sitecustomize





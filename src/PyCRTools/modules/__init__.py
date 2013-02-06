"""A Python library for analyzing LOFAR TBB data.
"""

from math import *
import os
import sys
import global_space

# Make core functionality available in local namespace
from core import *

# Import open function from IO module
from io import open

# Now store the interactive namespace in tasks, so that we can assign to global variables in the interactive session
if "get_ipython" in __builtins__:
    print "IPython 0.11 detected - importing user namespace"
    global_space.globals = __builtins__["get_ipython"]().user_ns
elif "__IPYTHON__" in __builtins__:
    print "IPython 0.10 detected - importing user namespace"
    global_space.globals = __IPYTHON__.user_ns
else:
    if "__main__" in sys.modules and hasattr(sys.modules["__main__"], "__builtins__"):
        print "Python detected - importing user namespace"
        global_space.globals = sys.modules["__main__"].__builtins__.globals()
    else:
        print "Unknown Python version detected - not importing user namespace into tasks module."

import matplotlib
if "nogui" in global_space.globals and global_space.globals["nogui"]:
    print "PyCRTools: 'nogui=True' detected - using no plotting GUI!"
    matplotlib.use('Agg')
else:
    print "PyCRTools: Starting with plotting GUI. To run in batch mode use 'nogui=True' before importing pycrtools."

# Initialize tasks
import tasks
from pycrtools.tasks.commands import *

import rftools as rf
import shelve

tasks.globals = global_space.globals

taskdb = shelve.open(tasks.dbfile)
if "last_taskname" in taskdb:
    print "*** Restoring last task:", taskdb["last_taskname"]
    tload(taskdb["last_taskname"], quiet=True)
taskdb.close()

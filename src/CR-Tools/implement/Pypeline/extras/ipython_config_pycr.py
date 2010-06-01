# IPython profile for pycr
# To use (with ipython 0.11 or higher) copy this file to your ~/.ipython/
# directory and start ipython with
# ipython -p pycr

c = get_config()

# This can be used at any point in a config file to load a sub config
# and merge it into the current one.
load_subconfig('ipython_config.py')

lines = """
from pycrtools import *
"""

# You have to make sure that attributes that are containers already
# exist before using them.  Simple assigning a new list will override
# all previous values.
if hasattr(c.Global, 'exec_lines'):
    c.Global.exec_lines.append(lines)
else:
    c.Global.exec_lines = [lines]


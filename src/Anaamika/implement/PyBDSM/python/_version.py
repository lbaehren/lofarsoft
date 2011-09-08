"""Version module.

This module simply stores the version and svn revision numbers, as well
as a change log. The svn revision number will be updated automatically
whenever there is a change to this file."""

# Version number
__version__ = '1.0'

# Store svn Revision number. For this to work, one also
# needs to do: 
# "svn propset svn:keywords Revision src/Anaamika/implement/PyBDSM/python/_version.py" 
# from the LOFARSOFT directory. Then, the revision number is
# added automatically with each update to this file. 
__revision__ = filter(str.isdigit, "$Revision$")


# Change log
change_log = """
PyBDSM Change Log.
-------------------------------------------------------------------------------

2011/9/8 - Versioning system changed to use _version.py

"""


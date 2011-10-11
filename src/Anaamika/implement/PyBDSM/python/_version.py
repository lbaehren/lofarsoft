"""Version module.

This module simply stores the version and svn revision numbers, as well
as a change log. The svn revision number will be updated automatically
whenever there is a change to this file. However, if no change is made
to this file, the revision number will get out of sync. Therefore, one
must update this file with each (significant) update of the code: 
adding to the change log will naturally do this.
"""

# Version number
__version__ = '1.0'

# Store svn Revision number. For this to work, one also
# needs to do: 
# "svn propset svn:keywords Revision src/Anaamika/implement/PyBDSM/python/_version.py" 
# from the LOFARSOFT directory. Then, the revision number is
# added automatically with each update to this file. 
__revision__ = filter(str.isdigit, "$Revision$")


# Change log
def changelog():
    """
    PyBDSM Change Log.
    -----------------------------------------------------------------------
    
    2011/10/11 - Tweaked autocomplete to support IPython shell commands
                 (e.g., "!more file.txt").
    
    2011/10/05 - Added output of source lists. Changed name of write_gaul 
                 method to write_catalog (more general).
                 
    2011/10/04 - Added option to force source grouping by island 
                 (group_by_isl). Added saving of parameters to a PyBDSM 
                 save file to Op_output.
    
    2011/09/21 - Fixed issue with shapelet centering failing: it now falls
                 back to simple moment when this happens. Fixed issue with
                 plotresults when shapelets are fit.
                 
    2011/09/14 - Placed output column names and units in TC properties of
                 Gaussians. This allows easy standardization of the column
                 names and units.
                 
    2011/09/13 - Fixes to trim_box and resetting of Image objects in 
                 interface.process(). Changed thr1 --> thr2 in fit_iter in
                 guasfit.py, as bright sources are often "overfit" when 
                 using thr1, leading to large negative residuals. 
                 Restricted fitting of Gaussians to wavelet images to be 
                 only in islands found in the original image if 
                 opts.atrous_orig_isl is True. 
    
    2011/09/08 - Versioning system changed to use _version.py
    
    """
    pass
    
"""Version module.

This module simply stores the version and svn revision numbers, as well
as a changelog. The svn revision number will be updated automatically
whenever there is a change to this file. However, if no change is made
to this file, the revision number will get out of sync. Therefore, one
must update this file with each (significant) update of the code: 
adding to the changelog will naturally do this.
"""

# Version number
__version__ = '1.0'

# Store svn Revision number. For this to work, one also
# needs to do: 
# "svn propset svn:keywords Revision src/Anaamika/implement/PyBDSM/python/_version.py" 
# from the LOFARSOFT directory. Then, the revision number is
# added automatically with each update to this file. The line below does not
# need to be edited by hand.
__revision__ = filter(str.isdigit, "$Revision$")


# Change log
def changelog():
    """
    PyBDSM Changelog.
    -----------------------------------------------------------------------

    2010/02/02 - Improved polarisation module. Polarization quantities are
                 now calculated for Gaussians instead of sources.

    2012/01/27 - Fixed bug in psf_vary module that affected tesselation. 
                 Fixed many small typos in parameter descriptions.

    2012/01/18 - Fixed a bug that resulted in incorrect coordinates when
                 the trim_box option was used with a CASA image. Added 
                 option (blank_zeros) to blank pixels in the input image
                 that are exactly zero.

    2012/01/13 - Fixed minor bugs in the interactive shell and updated 
                 pybdsm.py to support IPython 0.12.
                 
    2011/12/21 - Fixed bug in gaul2srl module due to rare cases in which
                 an island has a negative rms value. Fixed a memory issue
                 in which memory was not released after using show_fit.

    2011/11/28 - Added option to have minpix_isl estimated automatically
                 as 1/3 of the beam area. This estimate should help 
                 exclude false islands that are much smaller than the
                 beam. This estimate is not let to fall below 4 pixels.
    
    2011/11/11 - Fixed bugs in source generation that would lead to
                 masking of all pixels for certain sources during
                 moment analysis. Adjusted calculation of jmax in
                 wavelet module to use island sizes (instead of image size)
                 if opts.atrous_orig_isl is True.

    2011/11/04 - Implemented new island fitting routine (enabled with the
                 peak_fit option) that can speed up fitting of large 
                 islands. Changed plotting of Gaussians in show_fit to 
                 use Ellipse artists to improve plotting speed.

    2011/11/03 - Altered reading of images to correctly handle 4D cubes.
                 Fixed bug in readimage that affected filenames.

    2011/10/26 - Extended psf_vary module to include fitting of stacked
                 PSFs with Gaussians, interpolation of the resulting
                 parameters across the image, and correction of the de-
                 convolved source sizes using the interpolated PSFs.
                 Changed plotting of Gaussians in show_fit() to use the
                 FWHM instead of sigma. Modified error calculation of M
                 sources to be more robust when sources are small. Fixed
                 spelling of "gaussian" in bbs_patches option list.

    2011/10/24 - Many small bug fixes to the psf_vary module. Fixed use of
                 input directory so that input files not in the current
                 directory are handled correctly.

    2011/10/14 - Added residual rms and mean values to sources and source
                 list catalogs. These values can be compared to background
                 rms and mean values as a quick check of fit quality.

    2011/10/13 - Modified deconvolution to allow 1-D Gaussians and sources. 
                 Added FREQ0, EQUINOX, INIMAGE keywords to output fits
                 catalogs. Fixed bug in source position angles. Adjusted
                 column names of output catalogs slightly to be more
                 descriptive.

    2011/10/12 - Added errors to source properties (using a Monte Carlo
                 method for M sources). Fixed bug in output column names.
    
    2011/10/11 - Tweaked autocomplete to support IPython shell commands
                 (e.g., "!more file.txt"). Fixed bug in gaul2srl that 
                 resulted in some very nearby Gaussians being placed into
                 different sources. Added group_tol option so that user
                 can adjust the tolerance of how Gaussians are grouped
                 into sources.
    
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
    
    2011/09/08 - Versioning system changed to use _version.py.
    
    """
    pass
    
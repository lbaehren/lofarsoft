"""Module image.

Instancess of class Image are a primary data-holders for all 
PyBDSM operations. They store the image itself together with
some meta-information (such as headers), options for processing
modules and all data generated during processing.

This module also defines class Op, which is used as a base class
for all PyBDSM operations.
"""

import numpy as N
from opts import *

class Image(object):
    """Image is the primary data container for PyBDSM.

    All the run-time data (such as image data, mask, etc.) 
    and the results of the analysis (like island lists,
    gaussian lists, etc) are stored here.

    A few convienience methods are provided for writing out
    lists and images:   
      write_gaul(): Writes Gaussian list in these formats:       
      write_srl(): Writes Source list in these formats:   
      write_shpl(): Writes Shapelet list in these formats:   
      write_rms_img(): Writes rms image to FITS file.         
      write_model_img(): Writes Gaussian and shapelet model
                         images to FITS file.    
      write_resid_img(): Writes Gaussian and shaplet resid
                         images to FITS file.

    """
    # A number of type-checked properties 
    # are defined for the most basic image attributes, such
    # as image data, mask, header, user options.
    # There is little sense in declaring all possible attributes
    # right here as it will introduce unneeded dependencies 
    # between modules, thus most other attributes (like island lists,
    # gaussian lists, etc) are inserted at run-time by the specific
    # PyBDSM modules.
    
    opts   = Instance(Opts, doc="User options")
    image  = NArray(doc="Image data, Stokes I")
    ch0    = NArray(doc="Channel-collapsed image data, Stokes I")
    ch0_Q  = NArray(doc="Channel-collapsed image data, Stokes Q")
    ch0_U  = NArray(doc="Channel-collapsed image data, Stokes U")
    ch0_V  = NArray(doc="Channel-collapsed image data, Stokes V")
    header = Any(doc="Image header")
    mask   = NArray(doc="Image mask (if present and attribute masked is set)")
    masked = Bool(False, doc="Flag if mask is present")

    def __init__(self, opts):
        self.opts = Opts(opts)

    def showfit(self):
        """Show ch0 image with Gaussians overlaid and residual image."""
        import plotresults
        plotresults.plotresults(self)

    def showrms(self):
        """Show rms image."""
        import plotresults
        plotresults.showrms(self)

    def showsrc(self):
        """Show ch0 image with sources overlaid."""
        import plotresults
        plotresults.showsrc(self)

    def write_resid_img(self, filename=None):
        """Write the residual Gaussian image to a fits file."""
        import output
        output.write_resid_img(self, filename=filename)

    def write_model_img(self, filename=None):
        """Write the model Gaussian image to a fits file."""
        import output
        output.write_model_img(self, filename=filename)

    def write_rms_img(self, filename=None):
        """Write the rms image to a fits file."""
        import output
        output.write_rms_img(self, filename=filename)

    def write_ch0_img(self, filename=None):
        """Write the compressed ch0 image (used for source detection) to a fits file."""
        import output
        output.write_ch0_img(self, filename=filename)
      
    def write_srl(self, filename=None, format='fits', srcroot=None):
        """Write the source list to a file.

        filename - name of resulting file
        format - format of output list
        srcroot - root for source names (BBS format only)
        Supported formats are:
            "fits"
            "ascii"
            "bbs" -- each source gets its own patch
        """
        import output
        output.write_srl(self, filename=filename, format=format)
        
    def write_shpl(self, filename=None, format='fits', srcroot=None):
        """Write the shapelet list to a file.

        filename - name of resulting file
        format - format of output list
        Supported formats are:
        srcroot - root for source names (BBS format only)
            "bbs"
            "fits"
            "ascii"
        """
        import output
        output.write_shpl(self, filename=filename, format=format)

    def write_gaul(self, filename=None, format='fits', srcroot=None, patches=False):
        """Write the Gaussian list to a file.

        filename - name of resulting file
        srcroot - root for source names (BBS format only)
        format - format of output list
        patches - If True, make one patch for each Gaussian 
        Supported formats are:
            "fits"
            "ascii"
            "bbs"
            "ds9"
        """
        import output
        output.write_gaul(self, filename=filename, format=format, srcroot=srcroot, patches=patches)
                

class Op(object):
    """Common base class for all PyBDSM operations.

    At the moment this class is empty and only defines placeholder
    for method __call__, which should be redefined in all derived
    classes.
    """
    def __call__(self, img):
        raise NotImplementedError("This method should be redefined")

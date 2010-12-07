"""Initialize PyBDSM namespace.

Import all standard operations, define default chain of
operations and provide function 'execute', which can
execute chain of operations properly.
"""
# Try simple check of whether pyrap is available
try:
    import pyrap.images as pim
    has_pyrap = True
except ImportError:
    has_pyrap = False

if has_pyrap:
    from read_image import Op_readimage
from FITS import Op_loadFITS
from preprocess import Op_preprocess
from rmsimage import Op_rmsimage
from threshold import Op_threshold
from islands import Op_islands
from collapse import Op_collapse
from gausfit import Op_gausfit
from make_residimage import Op_make_residimage
from output import Op_outlist
from shapefit import Op_shapelets
from gaul2srl import Op_gaul2srl
from spectralindex import Op_spectralindex
from polarisation import Op_polarisation
from wavelet_atrous import Op_wavelet_atrous
#from psf_vary import Op_psf_vary # this module is not in current USG repository, and _pytesselate.so does not import
import mylogger 

if has_pyrap:
    fits_chain = [Op_readimage(),
                  Op_collapse(),
                  Op_preprocess(),
                  Op_rmsimage(),
                  Op_threshold(), 
                  Op_islands(),
                  Op_gausfit(), 
                  Op_shapelets(),
                  Op_make_residimage(), 
                  Op_gaul2srl(), 
                  Op_spectralindex(),
                  Op_polarisation(),
                  Op_wavelet_atrous(),
                  #Op_psf_vary(),
                  Op_outlist()
                  ]
else:
    fits_chain = [Op_loadFITS(),
                  Op_collapse(),
                  Op_preprocess(),
                  Op_rmsimage(),
                  Op_threshold(), 
                  Op_islands(),
                  Op_gausfit(), 
                  Op_shapelets(),
                  Op_make_residimage(), 
                  Op_gaul2srl(), 
                  Op_spectralindex(),
                  Op_polarisation(),
                  Op_wavelet_atrous(),
                  #Op_psf_vary(),
                  Op_outlist()
                  ]
   

def execute(chain, opts):
    """Execute chain.

    Create new Image with given options and apply chain of 
    operations to it.
    """
    from image import Image
    from time import time
    from types import ClassType, TypeType
    import mylogger 

    mylog = mylogger.logging.getLogger("PyBDSM.init      ")
    mylog.info("Running PyBDSM on "+opts["filename"])
    
    img = Image(opts)
    img.log = ''

    ### make sure all op's are instances
    ops = []
    for op in chain:
        if isinstance(op, (ClassType, TypeType)):
            ops.append(op())
        else:
            ops.append(op)

    ### and run them all
    for op in ops:
        op.__start_time = time()
        op(img)
        op.__stop_time = time()

    if img.opts.print_timing:
        print "="*30
        for op in chain:
            print "%15s : %f" % (op.__class__.__name__, 
                                 (op.__stop_time - op.__start_time))
        print "="*30
        print "%15s : %f" % ("Total",
                             (chain[-1].__stop_time - chain[0].__start_time))

    return img


def process_image(input_file, beam=None, thresh_isl=3.0, thresh_pix=5.0, use_rms_map=None, extended=False, gaussian_maxsize=10.0, use_pyrap=True):
    #polarisation_do=False, spectralindex_do=False, shapelet_do=False,
    """
    Run a standard analysis and returns the associated Image object.

    
    The following options are available:
    
      beam = (major axis FWHM [deg], minor axis FWHM [deg], pos. angle [deg])
      polarisation_do = True/False; do polarisation analysis (default = False)?
      spectralindex_do = True/False; do spectral index analysis (default = False)?
      shapelet_do = True/False; do shapelet analysis (default = False)?
      thresh_isl = threshold for island detection in sigma (default = 3.0)
      thresh_pix = at least one pixel in island must exceed this threshold in sigma
                   (default = 5.0)
      use_rms_map = None/True/False; use 2D map of rms or constant (default = None =
                    determine automatically)
      gaussian_maxsize = maximum allowed Gaussian size in pixels (default = 10.0)
      extended = True/False; if significant portion of field has extended emission,
                 setting this flag to True may help. It sets rms map to a constant
                 (since the extended emission will bias map) and sets the maximum
                 Gaussian size to 100.0 pixels

                 
    Examples:
    
      - Find sources in an image:
        > img = bdsm.process_image('image.fits')
          --> process with default parameters
        > img = bdsm.process_image('image.fits', beam=(0.0138, 0.0066, -54.6))
          --> process with specified beam
        > img = bdsm.process_image('image.fits', isl_thresh=5.0)
          --> set threshold for island detection to 5-sigma

      - Write list of resulting Gaussians to a file:
        > img.write_gaul()
          --> write Gaussian list to "image.gaul.txt"
        > img.write_gaul(format='fits')
          --> write list in FITS format to "image.gaul.fits"
        > img.write_gaul(format='bbs')
          --> write list in BBS format to "image.gaul.sky"
        > img.write_gaul('im.sky', format='bbs')
          --> write to "im.sky"
        > img.write_gaul(format='ds9')
          --> write Gaussian list to "image.gaul.reg"

    """
    import sys
    
    # Handle options
    polarisation_do=False
    spectralindex_do=False
    shapelet_do=False
    if beam != None and (isinstance(beam, tuple) == False or len(beam) != 3):
        raise RuntimeError('Beam must be a Tuple of length 3: (maj [deg], min [deg], angle [deg]).')
    if isinstance(polarisation_do, bool) == False:
        raise RuntimeError('do_pol must be True or False')
    if isinstance(spectralindex_do, bool) == False:
        raise RuntimeError('do_spec must be True or False')
    if isinstance(shapelet_do, bool) == False:
        raise RuntimeError('do_shapelets must be True or False')
    if isinstance(thresh_isl, float) == False or thresh_isl <= 1.0:
        raise RuntimeError('isl_thresh must be greater than 1.0')
    if isinstance(thresh_pix, float) == False or thresh_pix <= 1.0:
        raise RuntimeError('pix_thresh must be greater than 1.0')
    # if (collapse_mode in ['average', 'single']) == False:
    #     raise RuntimeError('collapse_mode must be "average" or "single"')
    # if (collapse_wt in ['rms', 'unity']) == False:
    #     raise RuntimeError('collapse_wt must be "rms" or "unity"')
    # if threshold_method != None and (threshold_method in ['hard', 'fdr']) == False:
    #     raise RuntimeError('threshold_method must be None, "hard", or "fdr"')
    if use_rms_map != None and isinstance(use_rms_map, bool) == False :
        raise RuntimeError('use_rms_map must be None, True, or False')
    # if (use_mean_map in ['default', 'const', 'map']) == False:
    #     raise RuntimeError('use_mean_map must be "default", "const", or "map"')
    # if rms_box != None and (isinstance(rms_box, tuple) == False or len(rms_box) != 2):
    #     raise RuntimeError('rms_box not defined properly.')
    if isinstance(gaussian_maxsize, float) == False or gaussian_maxsize < 0.0:
        raise RuntimeError('gaussian_maxsize must be greater than 0.0')
    if isinstance(extended, bool) == False:
        raise RuntimeError('extended flag must be True or False')
    if extended == True:
        gaussian_maxsize = 100.0 # allow extended gaussians
        use_rms_map = False # ignore rms map, which may be biased by extended emission        
    if use_pyrap == True and has_pyrap == True:
        use_pyrap = True
    else:
        use_pyrap = False

    if use_pyrap:
        fits_chain = [Op_readimage(),
                      Op_collapse(),
                      Op_preprocess(),
                      Op_rmsimage(),
                      Op_threshold(), 
                      Op_islands(),
                      Op_gausfit(), 
                      Op_make_residimage(), 
                      Op_outlist()
                      ]
    else:
        fits_chain = [Op_loadFITS(),
                      Op_collapse(),
                      Op_preprocess(),
                      Op_rmsimage(),
                      Op_threshold(), 
                      Op_islands(),
                      Op_gausfit(), 
                      Op_make_residimage(),
                      Op_outlist()
                      ]
 
    # Build options dictionary
    opts = {'filename':input_file, 'fits_name':input_file, 'beam': beam, 'thresh_isl':thresh_isl, 'thresh_pix':thresh_pix, 'polarisation_do':polarisation_do, 'spectralindex_do':spectralindex_do, 'shapelet_do':shapelet_do, 'rms_map':use_rms_map, 'flag_maxsize_bm':gaussian_maxsize, 'use_pyrap':use_pyrap}

    # Run execute with the default fits_chain and given options
    img = execute(fits_chain, opts)
    return img

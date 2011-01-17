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


def process_image(input_file, beam=None, beam_spectrum=None, freq=None, thresh_isl=3.0, thresh_pix=5.0, thresh_gaus=None, use_rms_map=None, rms_box=None, extended=False, gaussian_maxsize=10.0, spectralindex_do=False, polarisation_do=False, shapelet_do=False, use_pyrap=True, collapse_mode='average', collapse_ch0=0, collapse_wt='rms', quiet=False):
    """
    Run a standard analysis and returns the associated Image object.

    The following options are available:    
      beam = None or (major axis FWHM [deg], minor axis FWHM [deg], pos. angle [deg])
      beam_spectrum = Synthesized beam per channel; None => all equal
      freq = None or frequency in Hz of input image (use a list if input image
             is a cube: e.g., [30e6, 35e6, 72e6])
      polarisation_do = True/False; do polarisation analysis (default = False)?
      spectralindex_do = True/False; do spectral index analysis (default = False)?
      shapelet_do = True/False; do shapelet analysis (default = False)?
      thresh_isl = threshold for island detection in sigma (default = 3.0)
      thresh_pix = at least one pixel in island must exceed this threshold in sigma
                   or island is ignored (default = 5.0)
      thresh_gaus = peak threshold in sigma for adding more Gaussians to an island
                    (default = None => determine automatically)
      collapse_mode = average/single; Average channels or take a single channel to 
                      perform source detection on
      collapse_ch0 = Number of the channel for source extraction, if collapse_mode =
                     "single"
      collapse_wt = unity/rms; Average channels with weights=1 or 1/rms^2 if 
                    collapse_mode="average"
      use_rms_map = None/True/False; use 2D map of rms or constant (default = None
                    => determine automatically)
      rms_box = None or (box_size [pixels], box_step [pixels])
      gaussian_maxsize = maximum allowed Gaussian size in number of beam areas
                         (default = 10.0)
      extended = True/False; if significant portion of field has extended emission,
                 setting this flag to True may help. It sets rms map to a constant
                 (since the extended emission will bias map) and sets the maximum
                 Gaussian area to 100.0 times the beam area.
      quiet - Suppress output to screen. Output is still sent to the logfile as usual.
      
    Examples:   
      Find sources in an image:
        > img = bdsm.process_image('image.fits')
          --> process with default parameters
        > img = bdsm.process_image('image.fits', beam=(0.0138, 0.0066, -54.6))
          --> process with specified beam
        > img = bdsm.process_image('image.fits', isl_thresh=5.0)
          --> set threshold for island detection to 5-sigma

      Write list of resulting Gaussians to a file:
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
    import os
    
    # Handle options
    if beam != None and (isinstance(beam, tuple) == False or len(beam) != 3):
        raise RuntimeError('beam must be a Tuple of length 3: (maj [deg], min [deg], angle [deg]).')
    if beam_spectrum != None and isinstance(beam_spectrum, list) == False:
        raise RuntimeError('beam_spectrum must be a list of Tuples of length 3: (maj [deg], min [deg], angle [deg]).')
    if freq != None and (isinstance(freq, float) == False and isinstance(freq, list) == False):
        raise RuntimeError('freq must be a float, a list of floats, or None')
    if isinstance(freq, float):
        freq = [freq] # make it a list
    if isinstance(polarisation_do, bool) == False:
        raise RuntimeError('polarisation_do must be True or False')
    if isinstance(spectralindex_do, bool) == False:
        raise RuntimeError('spectralindex_do must be True or False')
    if isinstance(shapelet_do, bool) == False:
        raise RuntimeError('shapelet_do must be True or False')
    if isinstance(thresh_isl, float) == False or thresh_isl < 0.0:
        raise RuntimeError('thresh_isl must be greater than 0.0')
    if isinstance(thresh_pix, float) == False or thresh_pix < 0.0:
        raise RuntimeError('thresh_pix must be greater than 0.0')
    if thresh_gaus != None and isinstance(thresh_gaus, float) == False:
        raise RuntimeError('thresh_gaus must be greater than 0.0 or None')
    if thresh_gaus == None:
        thresh_gaus = -1.0
    if (collapse_mode in ['average', 'single']) == False:
        raise RuntimeError('collapse_mode must be "average" or "single"')
    if isinstance(collapse_ch0, int) == False:
        raise RuntimeError('collapse_ch0 must be an integer')
    if (collapse_wt in ['rms', 'unity']) == False:
        raise RuntimeError('collapse_wt must be "rms" or "unity"')
    # if threshold_method != None and (threshold_method in ['hard', 'fdr']) == False:
    #     raise RuntimeError('threshold_method must be None, "hard", or "fdr"')
    if use_rms_map != None and isinstance(use_rms_map, bool) == False :
        raise RuntimeError('use_rms_map must be None, True, or False')
    # if (use_mean_map in ['default', 'const', 'map']) == False:
    #     raise RuntimeError('use_mean_map must be "default", "const", or "map"')
    if rms_box != None and (isinstance(rms_box, tuple) == False or len(rms_box) != 2):
        raise RuntimeError('rms_box must be entered as: (box_size [pixels], box_step [pixels])')
    if isinstance(gaussian_maxsize, float) == False or gaussian_maxsize < 0.0:
        raise RuntimeError('gaussian_maxsize must be greater than 0.0')
    if isinstance(extended, bool) == False:
        raise RuntimeError('extended flag must be True or False')
    if extended == True:
        gaussian_maxsize = 100.0 # allow very extended gaussians 
        use_rms_map = False # don't use rms map, which may be biased by extended emission        
    if use_pyrap == True and has_pyrap == True and os.path.splitext(input_file)[1] not in ['.fits', '.FITS']:
        use_pyrap = True
    else:
        use_pyrap = False

    if use_pyrap:
        if spectralindex_do:
            fits_chain = [Op_readimage(),
                          Op_collapse(),
                          Op_preprocess(),
                          Op_rmsimage(),
                          Op_threshold(), 
                          Op_islands(),
                          Op_gausfit(), 
                          Op_make_residimage(), 
                          Op_gaul2srl(), 
                          Op_spectralindex(),
                          Op_outlist()
                          ]
        else: # remove stuff needed only for specindx as they are buggy
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
        if spectralindex_do:
            fits_chain = [Op_loadFITS(),
                          Op_collapse(),
                          Op_preprocess(),
                          Op_rmsimage(),
                          Op_threshold(), 
                          Op_islands(),
                          Op_gausfit(), 
                          Op_make_residimage(),
                          Op_gaul2srl(), 
                          Op_spectralindex(),
                          Op_outlist()
                          ]
        else: # remove stuff needed only for specindx as they are buggy
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
    opts = {'filename':input_file, 'fits_name':input_file, 'beam': beam, 'beam_spectrum': beam_spectrum, 'frequency':freq, 'thresh_isl':thresh_isl, 'thresh_pix':thresh_pix, 'thresh_gaus':thresh_gaus, 'polarisation_do':polarisation_do, 'spectralindex_do':spectralindex_do, 'shapelet_do':shapelet_do, 'rms_map':use_rms_map, 'rms_box': rms_box, 'flag_maxsize_bm':gaussian_maxsize, 'use_pyrap':use_pyrap, 'thresh':'hard', 'collapse_mode':collapse_mode, 'collapse_ch0':collapse_ch0, 'collapse_wt':collapse_wt, 'quiet':quiet}

    # Run execute with the fits_chain and given options
    img = execute(fits_chain, opts)
    return img

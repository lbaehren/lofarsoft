"""Initialize PyBDSM namespace.

Import all standard operations, define default chain of
operations and provide function 'execute', which can
execute chain of operations properly.
"""

from FITS import Op_loadFITS
#from read_image import Op_readimage
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

fits_chain = [#Op_readimage(),
              Op_loadFITS(),
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
    mylog.info("Running PyBDSM on "+opts["fits_name"])
    
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


def process_image(input_file, beam=None, do_pol=False, do_spec=False, do_shapelets=False, isl_thresh=3.0, pix_thresh=5.0, collapse_mode='average', collapse_wt='rms', threshold_method=None, use_rms_map=None, use_mean_map='default', rms_box=None, extended=False, gaussian_maxsize=10.0):
    """
    Run a standard analysis on an image and returns the associated Image object.

    explain options, etc. here.
    """
    import sys
    
    # Handle options
    if beam != None and (isinstance(beam, tuple) == False or len(beam) != 3):
        raise RuntimeError('Beam must be a Tuple of length 3: (maj [deg], min [deg], angle [deg]).')
    if isinstance(do_pol, bool) == False:
        raise RuntimeError('do_pol must be True or False')
    if isinstance(do_spec, bool) == False:
        raise RuntimeError('do_spec must be True or False')
    if isinstance(do_shapelets, bool) == False:
        raise RuntimeError('do_shapelets must be True or False')
    if isinstance(isl_thresh, float) == False or isl_thresh <= 1.0:
        raise RuntimeError('isl_thresh must be greater than 1.0')
    if isinstance(pix_thresh, float) == False or pix_thresh <= 1.0:
        raise RuntimeError('pix_thresh must be greater than 1.0')
    if (collapse_mode in ['average', 'single']) == False:
        raise RuntimeError('collapse_mode must be "average" or "single"')
    if (collapse_wt in ['rms', 'unity']) == False:
        raise RuntimeError('collapse_wt must be "rms" or "unity"')
    if threshold_method != None and (threshold_method in ['hard', 'fdr']) == False:
        raise RuntimeError('threshold_method must be None, "hard", or "fdr"')
    if use_rms_map != None and isinstance(use_rms_map, bool) == False :
        raise RuntimeError('use_rms_map must be None, True, or False')
    if (use_mean_map in ['default', 'const', 'map']) == False:
        raise RuntimeError('use_mean_map must be "default", "const", or "map"')
    if rms_box != None and (isinstance(rms_box, tuple) == False or len(rms_box) != 2):
        raise RuntimeError('rms_box not defined properly.')
    if isinstance(gaussian_maxsize, float) == False or gaussian_maxsize < 0.0:
        raise RuntimeError('gaussian_maxsize must be greater than 0.0')
    if isinstance(extended, bool) == False:
        raise RuntimeError('extended flag must be True or False')
    if extended == True:
        gaussian_maxsize = 100.0 # allow extended gaussians
        use_rms_map = False # ignore rms map, which may be biased by extended emission        
    
    # Build options dictionary
    opts = {'fits_name':input_file, 'beam': beam, 'collapse_mode':collapse_mode , 'collapse_wt':collapse_wt, 'thresh':threshold_method, 'thresh_isl':isl_thresh, 'thresh_pix':pix_thresh, 'polarisation_do':do_pol, 'spectralindex_do':do_spec, 'shapelet_do':do_shapelets, 'rms_map':use_rms_map, 'mean_map':use_mean_map, 'rms_box':rms_box, 'flag_maxsize_bm':gaussian_maxsize}

    # Run execute with the default fits_chain and given options
    img = execute(fits_chain, opts)
    return img

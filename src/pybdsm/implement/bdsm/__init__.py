import time

from Image import Image, Opts
from FITS import Op_loadFITS
from bstat import Op_bstat, Op_thresholds
from islands import Op_islands
from gausfit import Op_gausfit
from output import Op_outlist

fits_chain = [Op_loadFITS(), Op_bstat(), Op_thresholds(), Op_islands(),
              Op_gausfit(), Op_outlist()]
bdsm_default_opts = {'rms_clip': 3,       ### sigma's for hard threshold
                     'isl_min_size': 4,   ### minimal island size, pixels
                     'isl_peak_clip': 5,  ### sigma's for island peak
                     'isl_clip': 3,       ### sigma's for island boundary
                     }



def execute(chain, opts):
    """Execute sequence of operations"""
    img = Image(bdsm_default_opts)
    img.opts._apply(opts)
    for op in chain:
        op.__start_time = time.time()
        op(img)
        op.__stop_time = time.time()

    print "="*30
    for op in chain:
        print "%15s : %f" % (op.__class__.__name__, 
                               (op.__stop_time - op.__start_time))
    print "="*30
    print "%15s : %f" % ("Total",
                           (chain[-1].__stop_time - chain[0].__start_time))

    return img

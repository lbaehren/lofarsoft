"""Initialize PyBDSM namespace.

Import all standard operations, define default chain of
operations and provide function 'execute', which can
execute chain of operations properly.
"""

from readimage import Op_readimage
from collapse import Op_collapse
from preprocess import Op_preprocess
from rmsimage import Op_rmsimage
from threshold import Op_threshold
from islands import Op_islands
from gausfit import Op_gausfit
from make_residimage import Op_make_residimage
from output import Op_outlist
from shapefit import Op_shapelets
from gaul2srl import Op_gaul2srl
from spectralindex import Op_spectralindex
from polarisation import Op_polarisation
from wavelet_atrous import Op_wavelet_atrous
from psf_vary import Op_psf_vary
from cleanup import Op_cleanup
import mylogger 

fits_chain = [Op_readimage(), Op_collapse(), Op_preprocess(),
              Op_rmsimage(), Op_threshold(), 
              Op_islands(),
              Op_gausfit(), 
              Op_shapelets(),
              Op_make_residimage(), 
              Op_gaul2srl(), 
              Op_spectralindex(),
              Op_polarisation(),
              Op_wavelet_atrous(),
              Op_psf_vary(),
              Op_outlist(),
              Op_cleanup()
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
    mylog.info("Running PyBDSM (test) on "+opts["fits_name"])

    img = Image(opts)
    img.log = ''
    stopat = img.opts.stop_at

    ### make sure all op's are instances
    ops = []
    for op in chain:
        if isinstance(op, (ClassType, TypeType)):
            ops.append(op())
        else:
            ops.append(op)
        if stopat == 'isl' and isinstance(op, Op_islands): break

    ### and run them all
    for op in ops:
        op.__start_time = time()
        op(img)
        op.__stop_time = time()

    if img.opts.print_timing:
        print "="*30
        for op in ops:#chain:
            print "%15s : %f" % (op.__class__.__name__, 
                                 (op.__stop_time - op.__start_time))
        print "="*30
        print "%15s : %f" % ("Total",
                             (ops[-1].__stop_time - ops[0].__start_time))
                             #(chain[-1].__stop_time - chain[0].__start_time))

    return img



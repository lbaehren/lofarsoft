"""Initialize PyBDSM namespace.

Import all standart operations, define default chain of
operations and provide function 'execute', which can
execute chain of operations properly.
"""

from FITS import Op_loadFITS
from preprocess import Op_preprocess
from rmsimage import Op_rmsimage
from islands import Op_islands
from gausfit import Op_gausfit
from fittedimage import Op_fittedimage
from output import Op_outlist

fits_chain = [Op_loadFITS(), Op_preprocess(),
              Op_rmsimage(), Op_islands(),
              Op_gausfit(), Op_fittedimage(),
              Op_outlist()]

def execute(chain, opts):
    """Execute chain.

    Create new Image with given options and apply chain of 
    operations to it.
    """
    from image import Image
    from time import time
    from types import ClassType, TypeType

    img = Image(opts)


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


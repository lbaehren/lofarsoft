
from Image import Image, Opts
from FITS import Op_loadFITS
from bstat import Op_bstat, Op_thresholds
from islands import Op_islands
from gausfit import Op_gausfit
from output import Op_outlist

fits_chain = [Op_loadFITS(), Op_bstat(), Op_thresholds(), Op_islands(),
              Op_gausfit(), Op_outlist()]
bdsm_default_opts = {}



def execute(chain, opts):
    """Execute sequence of operations"""
    img = Image(bdsm_default_opts)
    img.opts._apply(opts)
    for op in chain:
        op(img)

    return img

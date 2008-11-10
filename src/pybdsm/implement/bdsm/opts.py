"""PyBDSM options

Options are essentially user-controllable parametes passed into 
PyBDSM operations, and allow for end-users to control the exact
details of how calculations are done.

In general it's better to specify newly added options directly
in this file, so one can oversee them all. But it's also possible
to extend it at run-time, and under some circumstances (e.g. pybds 
installed system-wide, and there is no way to modify this file)
this might be the only option to do so. An example of such extension
follows:

==== file newmodule.py ====
from image import Op

class Op_new_op(Op):
    ## do something usefull here
    ## we need to add option my_new_opt
    pass

## this will extend Opts class at runtime and ensure that 
## type-checking works properly.
Opts.my_new_opt = Float(33, doc="docstring")
"""

from tc import Int, Float, Bool, String, Tuple, Enum, \
    Option, NArray, Instance, tInstance, List, Any, TCInit


class Opts(object):
    """Class Opts -- user-contollable parameters."""
    fits_name      = Option(None, String(), doc="input FITS file name")

    beam           = Option(None, Tuple(Float(), Float(), Float()),
                            doc = "Override beam shape from file header " \
                                "(bmaj, bmin, bpa)")

    rms_clip       = Float(3, doc="sigma's for hard threshold")
    rms_noise_map  = Bool(True, doc="mean/rms map(True) or constant")
    rms_box        = Tuple((50,25), doc="box size, step size for " \
                               "rms/mean map calculation")

    spline_rank    = Enum(1,3, doc="rank of interpolating function " \
                              "for rms/mean map")


    isl_clip       = Float(3, doc="sigma's for island detection")
    isl_peak_clip  = Float(5, doc="sigma's for island peak")
    isl_min_size   = Int(4, doc="minimal island size, pixels")


    fittedimage_clip=Float(.1, doc="sigma's for clipping gaussians " \
                                "while creating fittedimage")


    print_timing   = Bool(True, doc="print basic timing information")
    verbose_fitting= Bool(True, doc="print out extra information " \
                              "during fitting")


    def __init__(self, values=None):
        """Build an instance of Opts and (possibly) 
        initialize some variables.

        Parameters:
        values: dictionary of key->value for initialization
                of variables
        """
        TCInit(self)
        if values is not None:
            self.set_opts(values)

    def set_opts(self, opts):
        """Set multiple variables at once.

        opts should be dictionary of name->value
        """
        opts = dict(opts)
        for k, v in opts.iteritems():
            self.__setattr__(k,v)

    def info(self):
        """Pretty-print current values of options"""
        import tc
        ## enumerate all options
        opts=[]
        for k,v in self.__class__.__dict__.iteritems():
            if isinstance(v, tc.TC):
                opts.append((k,v))

        ## sort and pretty-print
        opts.sort()
        res = ""
        fmt = "%20s = %5s  ## %s\n"

        for k,v in opts:
            res += fmt % (k, str(self.__getattribute__(k)), str(v.doc()))

        return res

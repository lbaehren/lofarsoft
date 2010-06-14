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
    """Class Opts -- user-controllable parameters."""
    fits_name      = Option(None, String(), doc="input FITS file name")

    beam           = Option(None, Tuple(Float(), Float(), Float()),
                            doc="Override beam shape from file header: " \
                                "(bmaj, bmin, bpa)")
    beam_spectrum  = Option(None, List(Tuple(Float(), Float(), Float())), \
                            doc = "synthesized beam per channel; None=>all equal")
    beam_sp_derive = Bool(False, doc = "If true and beam_spectrum is None, then assume header beam is for \
                          median frequency and scales with frequency for channels.") 

    thresh         = Enum(None, "hard", "fdr", 
                          doc="hard/fdr thresholds.\n" \
                              "None => calculate inside program")
    fdr_alpha      = Float(0.05, doc="Alpha for FDR algorithm for thresholds")
    fdr_ratio      = Float(0.1, doc="For thresh==None; " \
                               "if #false_pix/#source_pix < fdr_ratio " \
                               "thresh='hard' else 'fdr'")

    kappa_clip     = Float(3, doc="kappa for clipped mean, rms")
    rms_map        = Option(None, Bool(), doc="True => use rms map; " \
                                "False => constant rms; " \
                                "None => calculate inside program whether to use or not")
    mean_map        = String('default', doc="default => calc whether to use or not, 'zero' => 0, \
                                'const' => clipped mean, 'map' => use map")
    rms_box        = Option(None, Tuple(Int(), Int()),
                            doc="box size, step size for " \
                                "rms/mean map calculation")

    bmpersrc_th    = Option(None, Float(), 
                            doc="Theoretical estimate of number of beams " \
                                "per source. None => calculate inside program")

    spline_rank    = Enum(1,3, doc="rank of the interpolating function " \
                              "for rms/mean map")

    thresh_isl     = Float(3, doc="pixel threshold in sigma, for the island boundary")
    thresh_pix     = Float(5, doc="pixel threshold in sigma, for island peak")
    minpix_isl     = Int(4, doc="minimal number of pixels with " \
                             "emission per island")
    ini_gausfit    = String('default', doc = "Initial guess for gaussians, 'default', 'fbdsm', or \
                     'nobeam'. See manual for details")

    flag_smallsrc   = Bool(False, doc = "Flag sources smaller than the flag_minarea times beam area")
    flag_minsnr = Float(0.9, doc = "Flag gaussian if peak less than flag_minsnr times thresh_pix times local rms")
    flag_maxsnr = Float(1.5, doc = "Flag gaussian if peak greater than flag_maxsnr times max value in island")
    flag_maxsize_isl = Float(1.0, doc = "Flag gaussian if x, y bounding box around sigma-contour is factor times \
                       island bbox")
    flag_bordersize = Int(0, doc = "Flag gaussian if centre is outside border-flag_bordersize pixels, default=0")
    flag_maxsize_bm = Float(10.0, doc = "Flag gaussian if area greater than flag_maxsizebm times beam area")
    flag_minsize_bm = Float(0.5, doc = "Flag gaussian if flag_smallsrc=True and area smaller than \
                      flag_minsize_bm times beam area")

    fittedimage_clip = Float(.1, doc="sigma's for clipping gaussians " \
                                "while creating fittedimage")
    check_outsideuniv = Bool(False, doc="Check for pixels outside the universe (takes time)")

    shapelet_do    = Bool(False, doc="Decompose island into shapelets (True/False)")
    shapelet_basis = Enum("cartesian", "polar", 
                          doc="basis set for shapelet decomposition.")
    shapelet_fitmode = Enum("fit", None,
                       doc="Calculate shapelet coeff.s by fitting (fit) or \
                       integrating (None)")

    polarisation_do = Bool(True, doc="Find polarization properties (True/False)")
    rm_do = Bool(False, doc="Find rotation measure properties (True/False)")

    collapse_mode = Enum('average', 'single', doc="Average channels or take single channel to \
                        perform source detection on")
    collapse_ch0 = Int(0, doc="Number of the channel for source extraction, if collapse_mode='single'")
    collapse_av = List(None, doc="List of channels to average if collapse_mode='average'; None=all")
    collapse_wt = Enum('unity', 'rms', doc="Average channels with weights=1 or 1/rms_clip^2 if \
                        collapse_mode='average'")

    savefits_rmsim  = Bool(False, doc="Save background rms image as fits file")
    savefits_meanim = Bool(False, doc="Save background mean image as fits file")
    savefits_rankim = Bool(False, doc="Save island rank image as fits file")

    spectralindex_do= Bool(True, doc="Calculate spectral indices for multi-channel image")
    spin_nchan0     = Int(40, doc='rough number of channels to average cube to, before \
                      attempting to fit spectral index.')
    spin_flux       = String('total', doc = "'total' or 'peak'; determines which flux to compute spectral indices for")
    flagchan_rms    = Bool(True, doc = "Flag channels before (averaging and) extracting spectral index, if their rms if \
                      more than 5 (clipped) sigma outside the median rms over all channel, but only if <= 10% of channels")
    specind_Msrc_exclude1 = Float(0.06, doc = "Exclude gaussians less than this factor*max(gaussian peaks) inside that M source \
                      while fitting for spectral index for each gaussian")
    specind_Msrc_exclude2 = Float(3, doc = "Exclude gaussians whose average SNR over channels is less than this")
    specind_kappa   = Float(7.5, doc = "Kappa for deciding Case I, II, III for spectral index")
    specind_minchan = Int(6, doc = "Min number of channels to average to, for a given source, before deciding to sum \
                      over pixels if still below specin_kappa*noise")
    specind_dolog   = Bool(False, doc = "Fit flux vs freq in log space or in real space")

    atrous_do       = Bool(False, doc = "Decompose gaussian residual image into a-trous wavelets")
    atrous_jmax     = Int(0, doc = 'J_max, max wavelength order, 0 => determine within')
    atrous_lpf      = String('b3', doc = 'Low pass filter, either b3 or tr for B3 spline or Triangle')
    atrous_bdsm_do  = Bool(True, doc = "Perform source extraction on each wvavelet scale")

    print_timing   = Bool(True, doc="print basic timing information")
    verbose_fitting= Bool(False, doc="print out extra information " \
                              "during fitting")
    debug_figs_1   = Bool(False, doc='Plot gaussian fit images for each source for each channel for spectralindex')
    debug_figs_2   = Bool(False, doc='Plot gaussian fit parameter plots for each source as a fn of channel')
    debug_figs_3   = Bool(False, doc='Plot flagged channels before and after averaging for spectralindex for image')
    debug_figs_4   = Bool(False, doc='Plot figs for Case I')
    debug_figs_5   = Bool(False, doc='Plot for spectral index for M-sources')
    debug_figs_6   = Bool(False, doc='Plot images for inigaus_nobeam')

    output_fbdsm   = Bool(False, doc="write out fBDSM format output files or not, for use in Anaamika")
    fbdsm_scratch  = String('/Users/mohan/anaamika/image/', doc="scratch directory for storing fBDSM files")


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

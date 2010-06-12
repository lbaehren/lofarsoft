"""Module gausfit.

This module does multi-gaussian fits for all detected islands.
At the moment fitting algorithm is quite simple -- we just add
gaussians one-by-one as long as there are pixels with emission
in the image, and do post-fitting flagging of the extracted
gaussians.

The fitting itself is implemented by the means of MGFunction 
class and a number of fitter routines in _cbdsm module.
MGFunction class implements multi-gaussian function and 
provides all functionality required by the specific fitters.
"""

from image import Op

class Op_gausfit(Op):
    """Fit a number of 2D gaussians to each island.

    The results of the fitting are stored in the Island
    structure itself as a list of Gaussian object (gaul),
    list of flagged gaussians (fgaul) and an MGFunction
    object which was used for fitting (mg_fcn).

    Additionally it adds generator which allows to traverse
    all gaussian lists together. An sample code for such
    traversal will read:

    img = <some Image with gausfit module run on it>
    for g in img.gaussians():
        <do something to g, where g is Gaussian instance>

    Prerequisites: module islands should be run first.
    """
    def __call__(self, img):
        opts = img.opts

        for idx, isl in enumerate(img.islands):
            if opts.verbose_fitting:
                print "Fitting isl #", idx

            gaul, fgaul = self.fit_island(isl, opts, img.pixel_beam)

            ### now convert gaussians into Gaussian objects and store
            gaul = [Gaussian(img, par, idx, gidx)
                        for (gidx, par) in enumerate(gaul)]

            fgaul= [Gaussian(img, par, idx, gidx + gidx2 + 1, flag)
                        for (gidx2, (flag, par)) in enumerate(fgaul)]

            isl.gaul = gaul
            isl.fgaul= fgaul

        ### create generator which allow to easily access all gaussians
        def gaussian_list():
            def list_generator():
                for isl in img.islands:
                    for g in isl.gaul:
                        yield g
            return list_generator()

        img.gaussians = gaussian_list

        return img

    def fit_island(self, isl, opts, beam):
        """Fit island with a set of 2D gaussians.

        Parameters:
        isl: island
        opts: Opts structure of the image
        beam: beam parameters which are used as an initial guess for
              gaussian shape

        Returns:
        Function returns 2 lists with parameters of good and flagged
        gaussians. Gaussian parameters are updated to be image-relative.
        """
        from _cbdsm import MGFunction
        fcn = MGFunction(isl.image, isl.mask_noisy, 1)
        thr = isl.mean + opts.isl_clip*isl.rms
        verbose = opts.verbose_fitting
        peak = fcn.find_peak()[0]
        dof = isl.size_active
        shape = isl.shape
        gaul = []
        iter = 0

        while iter < 5:
            iter += 1
            fitok = self.fit_iter(gaul, fcn, dof, beam, thr, verbose)
            gaul, fgaul = self.flag_gaussians(fcn.parameters, opts, 
                                              beam, thr, peak, shape)

            if fitok and len(fgaul) == 0:
                break

        ### return whatever we got
        isl.mg_fcn = fcn
        gaul  = [self.fixup_gaussian(isl, g) for g in gaul]
        fgaul = [(flag,self.fixup_gaussian(isl, g))
                                       for flag,g in fgaul]

        return gaul, fgaul

    def fit_iter(self, gaul, fcn, dof, beam, thr, verbose=1):
        """One round of fitting

        Parameters:
        gaul : list of initial gaussians
        fcn  : MGFunction object
        dof  : maximal number of fitted parameters
        beam : initial shape for newly added gaussians
               [bmaj, bmin, bpa] in pixels
        thr  : peak threshold for adding more gaussians
        verbose: whether to print fitting progress information
        """
        from _cbdsm import lmder_fit, dn2g_fit, dnsg_fit
        fit = lmder_fit
        beam = list(beam)

        ### first drop-in initial gaussians
        ### no error-checking here, they MUST fit
        fcn.reset()
        for g in gaul:
            self.add_gaussian(fcn, g, dof)

        ### do a round of fitting if any initials were provided
        fitok = True
        if len(gaul) != 0:
            fitok = fit(fcn, final=0, verbose=verbose)

        ### iteratively add gaussians while there are high peaks
        ### in the image and fitting converges
        while fitok:
            peak, coords = fcn.find_peak()

            if peak < thr:  ### no good peaks left
                break

            g = [peak, coords[0], coords[1]] + beam
            fitok &= self.add_gaussian(fcn, g, dof)

            fitok &= fit(fcn, final=0, verbose=verbose)

        ### and one last fit with higher precision
        ### make sure we return False when fitok==False due to lack
        ### of free parameters
        fitok &= fit(fcn, final=1, verbose=verbose)
        return fitok

    def add_gaussian(self, fcn, parameters, dof):
        """Try adding one more gaussian to fcn object.
        It's trying to reduce number of fitted parameters if
        there is not enough DoF left.

        Parameters:
        fcn: MGFunction object
        parameters: initial values for gaussian parameters
        dof: total possible number of fitted parameters
        """
        from _cbdsm import Gtype

        gtype = (Gtype.g3 if fcn.fitted_parameters() + 3 <= dof else None)
        gtype = (Gtype.g6 if fcn.fitted_parameters() + 6 <= dof else gtype)

        if gtype:
            fcn.add_gaussian(gtype, parameters)
            return True
        else:
            return False

    def flag_gaussians(self, gaul, opts, beam, thr, peak, shape):
        """Flag gaussians according to some rules.
        Splits list of gaussian parameters in 2, where the first
        one is a list of parameters for accepted gaussians, and
        the second one is a list of pairs (flag, parameters) for
        flagged gaussians.

        Parameters:
        gaul: input list of gaussians
        opts: Opts object to extract flagging parameters from
        beam: beam shape
        thr: threshold for pixels with signal
        peak: peak data value in the current island
        shape: shape of the current island
        """
        good = []
        bad  = []
        for g in gaul:
            flag = self._flag_gaussian(g, beam, thr, peak, shape)
            if flag:
                bad.append((flag, g))
            else:
                good.append(g)

        return good, bad

    def _flag_gaussian(self, g, beam, thr, peak, shape):
        """The actual flagging routine. See above for description.
        """
        from math import sqrt

        min_clip = 1
        max_clip = 2
        border_size = 1
        maxbeam_clip = 10
        minbeam_clip = .5

        A, x1, x2, s1, s2, th = g
        s1, s2 = map(abs, [s1, s2])
        flag = 0

        ### now check all conditions
        if A < min_clip*thr: flag += 1
        if A > max_clip*peak: flag += 2
        if x1 < -border_size or shape[0] - x1 < border_size-1: flag += 4
        if x2 < -border_size or shape[1] - x2 < border_size-1: flag += 8
        if s1 > sqrt(shape[0]*shape[1]) \
                or s1 > maxbeam_clip*sqrt(beam[0]*beam[1]): flag += 16
        if s1 < minbeam_clip*sqrt(beam[0]*beam[1]): flag += 32
        if s2 > sqrt(shape[0]*shape[1]) \
                or s2 > maxbeam_clip*sqrt(beam[0]*beam[1]): flag += 64
        if s2 < minbeam_clip*sqrt(beam[0]*beam[1]): flag += 128
        
        return flag

    def fixup_gaussian(self, isl, gaussian):
        """Normalize parameters by adjusting them to the 
        proper image coordinates and ensuring that all of 
        the implicit conventions (such as bmaj >= bmin) are met.
        """
        np = list(gaussian)

        ### update to the image coordinates
        np[1] += isl.origin[0]
        np[2] += isl.origin[1]

        ### shape values should be positive
        np[3] = abs(np[3])
        np[4] = abs(np[4])

        ### first extent is major
        if np[3] < np[4]:
            np[3:5] = np[4:2:-1]
            np[5] += 90
            
        ### clip position angle
        np[5] = divmod(np[5], 180)[1]
        
        return np


from image import *

class Gaussian(object):
    """Instances of this class are used to store information about
    extracted gaussians in a structured way.
    """
    gaussian_idx = Int(doc="Serial number of the gaussian within island")
    island_idx   = Int(doc="Serial number of the island")

    flag = Int(doc="Flag associated with gaussian")

    parameters  = List(Float(), doc="Raw gaussian parameters")

    ## should we use tuples for value + error ??
    total_flux  = Float(doc="Total flux, in Jy")
    total_fluxE = Float(doc="Total flux error")
    peak_flux   = Float(doc="Peak flux density, Jy/beam")
    peak_fluxE  = Float()
    center_sky  = List(Float(), doc="Sky coordinates of gaussian center")
    center_skyE = List(Float())
    center_pix  = List(Float(), doc="Pixel coordinates of gaussian center")
    center_pixE = List(Float())
    shape_sky   = List(Float(), doc="Shape of the gaussian FWHM, BPA, arcsec")
    shape_skyE  = List(Float())
    shape_pix   = List(Float(), doc="Shape of the gaussian, pixel units")
    shape_pixE  = List(Float())

    def __init__(self, img, gaussian, isl_idx, g_idx, flag=0):
        """Initialize Gaussian object from fitting data

        Parameters:
        img: PyBDSM image object
        gaussian: 6-tuple of fitted numbers
        isl_idx: island serial number
        g_idx: gaussian serial number
        flag: flagging (if any)
        """
        self.gaussian_idx = g_idx
        self.island_idx = isl_idx
        self.flag = flag
        self.parameters = gaussian

        p = gaussian
        self.peak_flux = p[0]
        self.center_pix = p[1:3]
        self.center_sky = img.pix2sky(p[1:3])
        self.shape_pix = p[3:6]
        self.shape_sky = img.pix2beam(p[3:6])


### Insert attributes into Island class
from islands import Island
Island.gaul = List(tInstance(Gaussian), doc="List of extracted gaussians")
Island.fgaul= List(tInstance(Gaussian), 
                   doc="List of extracted (flagged) gaussians")

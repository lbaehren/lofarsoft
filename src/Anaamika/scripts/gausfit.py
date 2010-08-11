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

from image import *
from copy import deepcopy as cp
import mylogger

gaus_num = Int(doc="Serial number of the gaussian for the image")
ngaus = Int(doc="Total number of gaussians extracted")

class Op_gausfit(Op):
    """Fit a number of 2D gaussians to each island.

    The results of the fitting are stored in the Island
    structure itself as a list of Gaussian object (gaul),
    list of flagged gaussians (fgaul) and an MGFunction
    object which was used for fitting (mg_fcn).

    Additionally it adds generator which allows to traverse
    all gaussian lists together. A sample code for such
    traversal will read:

    img = <some Image with gausfit module run on it>
    for g in img.gaussians():
        <do something to g, where g is Gaussian instance>

    Prerequisites: module islands should be run first.
    """
    def __call__(self, img):
        mylog = mylogger.logging.getLogger("PyBDSM."+img.log+"Gausfit   ")
        opts = img.opts
        for idx, isl in enumerate(img.islands):
          if opts.verbose_fitting:
            print "Fitting isl #", idx, '; # pix = ',N.sum(~isl.mask_active)

          gaul, fgaul = self.fit_island(isl, opts, img)

          ### now convert gaussians into Gaussian objects and store
          gaul = [Gaussian(img, par, idx, gidx)
                      for (gidx, par) in enumerate(gaul)]

          if len(gaul) == 0: gidx = 0
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

        ### put in the serial number of the gaussians for the whole image
        n = 0
        for isl in img.islands:
            m = 0
            for g in isl.gaul:
                n += 1; m += 1
                g.gaus_num = n
            isl.ngaus = m
        img.ngaus = n

        return img

    def fit_island(self, isl, opts, img):
        """Fit island with a set of 2D gaussians.

        Parameters:
        isl: island
        opts: Opts structure of the image
        beam: beam parameters which are used as an initial guess for
              gaussian shape

        Returns:
        Function returns 2 lists with parameters of good and flagged
        gaussians. Gaussian parameters are updated to be image-relative.

        My own notes (Niruj)
        fcn = MGFunction(im, mask, 1) makes an fcn object
        fcn.find_peak() finds peak and posn in im after subtracting all gaussians in fcn.parameters
        fcn.add_gaussian(gtype, (blah)) adds to fcn.parameters.
        fit(fcn, 0, 1) fits using fcn.parameters as initial guess and overwrites to fcn.parameters.
        fcn.reset() resets just the fcn.parameters. Image is still there.
        Atleast, thats what I think it is.

        """
        from _cbdsm import MGFunction
        fcn = MGFunction(isl.image, isl.mask_noisy, 1)
        beam = img.pixel_beam
        thr1 = isl.mean + opts.thresh_isl*isl.rms
        thr2 = isl.mean + opts.thresh_pix*isl.rms
        verbose = opts.verbose_fitting
        peak = fcn.find_peak()[0]
        dof = isl.size_active
        shape = isl.shape
        gaul = []
        iter = 0
        ng1 = 0

        if img.opts.ini_gausfit not in ['default', 'fbdsm', 'nobeam']: img.opts.ini_gausfit = 'default'
        if img.opts.ini_gausfit == 'default': 
          ngmax = 25
        if img.opts.ini_gausfit == 'fbdsm': 
          gaul, ng1, ngmax = self.inigaus_fbdsm(isl, thr2, beam, img)
        if img.opts.ini_gausfit == 'nobeam': 
          gaul = self.inigaus_nobeam(isl, thr2, beam, img)
          ng1 = len(gaul); ngmax = ng1+2
        while iter < 5:
            iter += 1
            fitok = self.fit_iter(gaul, ng1, fcn, dof, beam, thr1, iter, img.opts.ini_gausfit, ngmax, verbose)
            gaul, fgaul = self.flag_gaussians(fcn.parameters, opts, 
                                              beam, thr2, peak, shape)
            ng1 = len(gaul)
            if fitok and len(fgaul) == 0:
               break

        ### return whatever we got
        isl.mg_fcn = fcn
        gaul  = [self.fixup_gaussian(isl, g) for g in gaul]
        fgaul = [(flag, self.fixup_gaussian(isl, g))
                                       for flag, g in fgaul]

        return gaul, fgaul

    def inigaus_fbdsm(self, isl, thr, beam, img):
        """ initial guess for gaussians like in fbdsm """
        from math import sqrt
        from const import fwsig
        import functions as func

        im = isl.image; mask = isl.mask_active; av = img.clipped_mean 
        inipeak, iniposn, im1 = func.get_maxima(im, mask, thr, isl.shape, beam)
        if len(inipeak) == 0:
          av, stdnew, maxv, maxp, minv, minp = func.arrstatmask(im, mask)
          inipeak = [maxv]; iniposn = [maxp]
        nmulsrc1 = len(iniposn) 

        domore = True
        while domore:
          domore = False
          av, stdnew, maxv, maxp, minv, minp = func.arrstatmask(im1, mask)
          if stdnew > isl.rms and maxv >= thr and maxv >= isl.mean+2.0*isl.rms: 
            domore = True
            x1, y1 = N.array(iniposn).transpose()
            dumr = N.sqrt((maxp[0]-x1)*(maxp[0]-x1)+(maxp[1]-y1)*(maxp[1]-y1))
            distbm = dumr/sqrt(beam[0]*beam[1]*fwsig*fwsig)
            if N.any((distbm < 0.5) + (dumr < 2.2)): 
              domore = False
            if domore: 
              iniposn.append(N.array(maxp)); inipeak.append(maxv)
              im1 = func.mclean(im1, maxp, beam)

        inipeak = N.array(inipeak); iniposn = N.array(iniposn) 
        ind = list(N.argsort(inipeak)); ind.reverse()
        inipeak = inipeak[ind] 
        iniposn = iniposn[ind]
        gaul = []
        for i in range(len(inipeak)):
          g = (float(inipeak[i]), iniposn[i][0], iniposn[i][1]) + beam
          gaul.append(g)

        return gaul, nmulsrc1, len(inipeak)

    def inigaus_nobeam(self, isl, thr, beam, img):
        """ To get initial guesses when the source sizes are very different from the beam, and can also be elongated. 
            Mainly in the context of a-trous transform images. Need to arrive at a good guess of the sizes and hence need
            to partition the image around the maxima first. Tried the IFT watershed algo but with markers, it segments
            the island only around the minima and not the whole island. Cant find a good weighting scheme for tesselation 
            either. Hence will try this :

            Calulate number of maxima. If one, then take moment as initial guess.
            If more than one, then moment of whole island is one of the guesses if mom1 is within n pixels of one of 
            the maxima. Else dont take whole island moment. Instead, find minima on lines connecting all maxima and use
            geometric mean of all minima of a peak as the size of that peak.
            """
        from math import sqrt
        from const import fwsig
        import scipy.ndimage as nd
        import functions as func

        im = isl.image; mask = isl.mask_active; av = img.clipped_mean; thr1= -1e9
        inipeak, iniposn, im1 = func.get_maxima(im, mask, thr1, isl.shape, beam)
        npeak = len(iniposn) 
        gaul = []

        av, stdnew, maxv, maxp, minv, minp = func.arrstatmask(im, mask)
        mom = func.momanalmask_gaus(isl.image, isl.mask_active, 0, 1.0, True)
        if npeak <= 1:
          g = (float(maxv), int(round(mom[1])), int(round(mom[2])), mom[3]/fwsig, \
                                  mom[4]/fwsig, mom[5])
          gaul.append(g)

        if npeak > 1:                   # markers start from 1=background, watershed starts from 1=background
          watershed, markers = func.watershed(im, mask=isl.mask_active)
          nshed = N.max(markers)-1      # excluding background
          xm, ym = N.transpose([N.where(markers==i) for i in range(1,nshed+2)])[0]
          coords = [c for c in N.transpose([xm,ym])[1:]]
          alldists = [func.dist_2pt(c1, c2) for c1 in coords for c2 in coords if N.any(c1!=c2)]  # has double
          meandist = N.mean(alldists)    # mean dist between all pairs of markers
          compact = []; invmask = []
          for ished in range(nshed):
            shedmask = N.where(watershed==ished+2, False, True) + isl.mask_active # good unmasked pixels = 0
            imm = nd.binary_dilation(~shedmask, N.ones((3,3), int))   
            xbad, ybad = N.where((imm==1)*(im>im[xm[ished+1], ym[ished+1]]))
            imm[xbad, ybad] = 0
            invmask.append(imm); x, y = N.where(imm); xcen, ycen = N.mean(x), N.mean(y) # good pixels are now = 1
            dist = func.dist_2pt([xcen, ycen], [xm[ished+1], ym[ished+1]])
            if dist < max(3.0, meandist/4.0): 
              compact.append(True)  # if not compact, break source + diffuse
            else: 
              compact.append(False)
          if not N.all(compact):
           avsize = []
           ind = N.where(compact)[0]
           for i in ind: avsize.append(N.sum(invmask[i]))
           avsize = sqrt(N.mean(N.array(avsize)))
           for i in range(len(compact)):
             if not compact[i]:                         # make them all compact
               newmask = N.zeros(imm.shape, bool)
               newmask[max(0,xm[i+1]-avsize/2):min(im.shape[0],xm[i+1]+avsize/2), \
                       max(0,ym[i+1]-avsize/2):min(im.shape[1],ym[i+1]+avsize/2)] = True
               invmask[i] = invmask[i]*newmask
          resid = N.zeros(im.shape)                    # approx fit all compact ones
          for i in range(nshed): 
            mask1 = ~invmask[i]
            size = sqrt(N.sum(invmask))/fwsig
            xf, yf = coords[i][0], coords[i][1]
            p_ini = [im[xf, yf], xf, yf, size, size, 0.0]
            x, y = N.indices(im.shape)
            p, success = func.fit_gaus2d(im*invmask[i], p_ini, x, y)
            resid = resid + func.gaus_2d(p, x, y)
            gaul.append(p)
          resid = im - resid
          if not N.all(compact):                        # just add one gaussian to fit whole unmasked island
            maxv = N.max(resid)                         # assuming resid has only diffuse emission. can be false
            x, y = N.where(~isl.mask_active); xcen = N.mean(x); ycen = N.mean(y)
            invm = ~isl.mask_active 
            #bound = invm - nd.grey_erosion(invm, footprint = N.ones((3,3), int)) # better to use bound for ellipse fitting
            mom = func.momanalmask_gaus(invm, N.zeros(invm.shape, int), 0, 1.0, True)
            g = (maxv, xcen, ycen, mom[3]/fwsig, mom[4]/fwsig, mom[5]-90.)
            gaul.append(g)
            coords.append([xcen, ycen])
          if img.opts.debug_figs_6:
            import pylab as pl
            pl.figure(); pl.suptitle(img.imagename+' Island : '+str(isl.island_id))
            resid = N.zeros(im.shape); x, y = N.indices(im.shape); xp, yp = N.transpose(coords)
            for g in gaul:
              resid = resid + func.gaus_2d(g, x, y)
            pl.subplot(2,2,1); pl.imshow(N.transpose(im), interpolation='nearest', origin='lower')
            pl.subplot(2,2,2); pl.plot(xp, yp, '*k'); pl.imshow(N.transpose(watershed), interpolation='nearest', origin='lower')
            pl.subplot(2,2,3); pl.imshow(N.transpose(resid), interpolation='nearest', origin='lower')
            pl.subplot(2,2,4); pl.imshow(N.transpose(im-resid), interpolation='nearest', origin='lower')

        return gaul

    def fit_iter(self, gaul, ng1, fcn, dof, beam, thr, iter, inifit, ngmax, verbose=1):
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
        for ig in range(ng1):
          g = gaul[ig]
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
          if len(fcn.parameters) < ngmax and iter == 1 and inifit == 'fbdsm' and len(gaul) >= ng1+1: 
             ng1 = ng1 + 1
             g = gaul[ng1-1]
          else:
            if len(fcn.parameters) < ngmax and inifit in ['default', 'nobeam']:
              g = [peak, coords[0], coords[1]] + beam
            else:
              break
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
            flag = self._flag_gaussian(g, beam, thr, peak, shape, opts)
            if flag:
                bad.append((flag, g))
            else:
                good.append(g)

        return good, bad

    def _flag_gaussian(self, g, beam, thr, peak, shape, opts):
        """The actual flagging routine. See above for description.
        """
        from math import sqrt, sin, cos, log, pi
        from const import fwsig

        A, x1, x2, s1, s2, th = g
        s1, s2 = map(abs, [s1, s2])
        flag = 0

        if s1 < s2:   # s1 etc are sigma
          ss1=s2; ss2=s1; th1 = divmod(th+90.0, 180)[1]
        else:
          ss1=s1; ss2=s2; th1 = divmod(th, 180)[1]
        th1 = th1/180.0*pi
        if ss1 > 1e4 and ss2 > 1e4: 
          xbox = 1e9; ybox = 1e9
        else:
          xbox = 2.0*(abs(ss1*cos(th1)*cos(th1))+abs(ss2*ss2/ss1*sin(th1)*sin(th1)))/ \
                 sqrt(cos(th1)*cos(th1)+ss2*ss2/ss1/ss1*sin(th1)*sin(th1))
          ybox = 2.0*(abs(ss1*sin(th1)*sin(th1))+abs(ss2*ss2/ss1*cos(th1)*cos(th1)))/ \
                 sqrt(sin(th1)*sin(th1)+ss2*ss2/ss1/ss1*cos(th1)*cos(th1))

        ### now check all conditions
        border = opts.flag_bordersize
        if A < opts.flag_minsnr*thr: flag += 1
        if A > opts.flag_maxsnr*peak: flag += 2
        if N.isnan(x1) or x1 < border or x1 > shape[0] - border -1: flag += 4
        if N.isnan(x2) or x2 < border or x2 > shape[1] - border -1: flag += 8

        if xbox > opts.flag_maxsize_isl*shape[0]: flag += 16
        if ybox > opts.flag_maxsize_isl*shape[1]: flag += 32
        if s1*s2 > opts.flag_maxsize_bm*beam[0]*beam[1]: flag += 64
        if opts.flag_smallsrc:
          if s1*s2 < opts.flag_minsize_bm*beam[0]*beam[1]: flag += 128

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
    island_id    = Int(doc="Serial number of the island")

    flag = Int(doc="Flag associated with gaussian")

    parameters  = List(Float(), doc="Raw gaussian parameters")

    ## should we use tuples for value + error ??
    total_flux  = Float(doc="Total flux, in Jy")
    total_fluxE = Float(doc="Total flux error")
    peak_flux   = Float(doc="Peak flux density, Jy/beam")
    peak_fluxE  = Float()
    centre_sky  = List(Float(), doc="Sky coordinates of gaussian centre")
    centre_skyE = List(Float())
    centre_pix  = List(Float(), doc="Pixel coordinates of gaussian centre")
    centre_pixE = List(Float())
    size_sky   = List(Float(), doc="Shape of the gaussian FWHM, BPA, arcsec")
    size_skyE  = List(Float())
    size_pix   = List(Float(), doc="Shape of the gaussian FWHM, pixel units")
    size_pixE  = List(Float())

    def __init__(self, img, gaussian, isl_idx, g_idx, flag=0):
        """Initialize Gaussian object from fitting data

        Parameters:
        img: PyBDSM image object
        gaussian: 6-tuple of fitted numbers
        isl_idx: island serial number
        g_idx: gaussian serial number
        flag: flagging (if any)
        """
        import functions as func
        from const import fwsig

        self.gaussian_idx = g_idx
        self.island_id = isl_idx
        self.flag = flag
        self.parameters = gaussian

        p = gaussian
        self.peak_flux = p[0]
        self.centre_pix = p[1:3]
        size = p[3:6]
        size = func.corrected_size(size)
        self.size_pix = size
        self.size_sky = img.pix2beam(size)
        bm_pix = N.array([img.pixel_beam[0]*fwsig, img.pixel_beam[1]*fwsig, img.pixel_beam[2]])
        tot = p[0]*size[0]*size[1]/(bm_pix[0]*bm_pix[1])

        if flag == 0:
          errors = func.get_errors(img, p+[tot], img.islands[isl_idx].rms)
          self.centre_sky = img.pix2sky(p[1:3])
        else:
          errors = [0]*7
          self.centre_sky = [0., 0.]
        self.total_flux = tot
        self.total_fluxE = errors[6]

        self.peak_fluxE = errors[0]
        self.total_fluxE = errors[6]
        self.centre_pixE = errors[1:3]
        self.centre_skyE = img.pix2coord(errors[1:3])
        self.size_pixE = errors[3:6]
        self.size_skyE = img.pix2beam(errors[3:6])


### Insert attributes into Island class
from islands import Island
Island.gaul = List(tInstance(Gaussian), doc="List of extracted gaussians")
Island.fgaul= List(tInstance(Gaussian), 
                   doc="List of extracted (flagged) gaussians")

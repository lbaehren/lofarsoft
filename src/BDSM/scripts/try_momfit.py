
from image import *
from copy import deepcopy as cp

def momfit(img):
        from _cbdsm import MGFunction
        import functions as func
        from bdsm.const import fwsig
        from _cbdsm import MGFunction
        from gausfit import Op_gausfit as gg
        from bdsm.analysis import isl2border

        opts = img.opts
        resid = cp(img.ch0)
        verbose = opts.verbose_fitting

        from _cbdsm import lmder_fit, dn2g_fit, dnsg_fit
        fit = lmder_fit

        import pylab as pl
        import matplotlib.cm as cm
        pl.figure()

        for idx, isl in enumerate(img.islands):
            dof = isl.size_active
            mom = func.momanalmask_gaus(isl.image, isl.mask_active, 0, 1.0, True)
            im = cp(isl.image)

            fcn = MGFunction(isl.image, isl.mask_active, 1)
            g = [float(N.max(im)), int(round(mom[1])), int(round(mom[2])), mom[3]/fwsig, mom[4]/fwsig, mom[5]]
            c = cp(g); c[5] = c[5]+90.0
            if mom[3] > N.max(isl.shape) or mom[4] > N.max(isl.shape): print 'TOO BIG ',isl.island_id
            x2, y2 = func.drawellipse(c)
            x2 = x2 + isl.origin[0]; y2 = y2 + isl.origin[1]
            pl.plot(x2, y2, '-g')

            gg1 = gg()
            gg1.add_gaussian(fcn, g, dof)
            fit(fcn, final=0, verbose=verbose)

            A, x1, x2, s1, s2, th = fcn.parameters[0]
            s1, s2 = map(abs, [s1, s2])
            if s1 < s2:   # s1 etc are sigma
              ss1=s2; ss2=s1; th1 = divmod(th+90.0, 180)[1]
            else:
              ss1=s1; ss2=s2; th1 = divmod(th, 180)[1]
            c = [A, x1, x2, ss1, ss2, th1]
            x, y = N.indices(isl.shape)
            im = func.gaus_2d(c, x, y)
            resid[isl.bbox] -= im
            x2, y2 = func.drawellipse(c)
            x2 = x2 + isl.origin[0]; y2 = y2 + isl.origin[1]
            pl.plot(x2, y2, '-r')

            xb, yb = isl2border(img, isl) 
            pl.plot(xb, yb, 'x', color='#000000', markersize=8)

            #im = func.mclean(im, ((int(mom[1])), (int(mom[2]))), (mom[3]/fwsig, mom[4]/fwsig, mom[5]))

        import pyfits
        pyfits.writeto('momresid.fits', N.transpose(resid), img.header, clobber=True)

        from math import log10
        im_mean = img.clipped_mean
        im_rms = img.clipped_rms
        low = 1.1*abs(img.min_value)
        vmin = log10(im_mean-im_rms*4.0 + low)
        vmax = log10(im_mean+im_rms*10.0 + low)
        origin='lower'
        im = N.log10(img.ch0 + low)

        pl.imshow(N.transpose(im), origin=origin, interpolation='nearest',vmin=vmin, vmax=vmax, \
                    cmap=cm.gray)
        pl.colorbar()




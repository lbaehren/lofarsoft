import math
import numpy as N
import numpy.core.ma as M
import scipy.optimize as opt
from Image import Op

class Op_gausfit(Op):
    """Fit all islands with mutiple 2-d gaussians
    """

    def __call__(self, img):
        for idx, isl in enumerate(img.islands[0:10]):
            print "Fitting isl #", idx
            t = self.fit_island(isl, img.opts.isl_thresh)
            isl.gaus_fcn = t
            isl.gaul = t.parameters()

        return img

    def fit_island(self, isl, thr):
        fcn = gaus_fcn(isl)

        while True:
            peak, idx = find_peak(fcn.residuals())
            
            if peak <= thr: ## no good peaks left
                break

            if not fcn.add_gaussian(peak, idx): ## no more gaussians can be added
                break
            
            fcn.fit()

        fcn.fit()
        return fcn

class gaus_fcn:
    """Gaussian-fitting"""
    def __init__(self, isl):
        self.gaul = []  ## list of gaussians (idx_par, n_par)
        self.npar = 0   ## total number of parameters
        self.dof  = isl.size
        self.pars = []  ## parameters
        self.opts = isl.opts
        self.orig = isl.masked
        self.img = M.array(isl.masked, copy=True)

    def add_gaussian(self, peak, idx):
        if self.npar + 6 < self.dof:  ## add 6-parameter gaussian
            self.gaul.append((self.npar, 6))
            self.npar += 6
            self.pars.extend([peak, idx[0], idx[1]])
            self.pars.extend(self.opts.beam)
            return True
        if self.npar + 3 < self.dof:  ## add 3-parameter gaussian
            self.gaul.append((self.npar, 3))
            self.npar += 3
            self.pars.extend([peak, idx[0], idx[1]])
            return True
        return False

    def residuals(self, pars = None):
        img = self.img
        img.data.flat = self.orig.data
        if pars is None:
            p = self.pars
        else:
            p = pars
        for g in self.gaul:
            img -= N.fromfunction(self.gauss2d, img.shape, params=p[g[0]:g[0]+g[1]])

        return img

    def fcn(self, x):
        arr = self.residuals(x)
        arr.data[arr.mask]=0
        return arr.data.flat


    def fit(self):
        res = opt.leastsq(self.fcn, self.pars,
                          full_output=1, xtol=1e-3, ftol=1e-3)
        print 'Nfev %d  chisq %f ; opt.leastsq' % \
              (res[2]['nfev'], (res[2]['fvec']**2).sum())
        self.opars = self.pars
        self.pars = list(res[0])

    def gauss2d(self, x1, x2, params):
        if len(params) == 6:
            amp, x1o, x2o, sx1, sx2, theta = params
        elif len(params) == 3:
            amp, x1o, x2o = params
            sx1, sx2, theta = self.opts.beam
        else:
            raise RuntimeError("Don't know how to calculate this")
            
        theta = math.radians(theta)
        x1 -= x1o
        x2 -= x2o
        f1 = ( x1*math.cos(theta) + x2*math.sin(theta))/sx1
        f2 = (-x1*math.sin(theta) + x2*math.cos(theta))/sx2
        return amp * N.exp(-0.5*(f1**2 + f2**2))

    def parameters(self):
        l = []
        for g in self.gaul:
            p = self.pars[g[0]:g[0]+g[1]]
            if g[1] == 3:
                p.extend(self.opts.beam)
            l.append(p)
        return l

def find_peak(arr):
    idx = arr.argmax(axis=None)
    return (arr.flat[idx], N.unravel_index(idx, arr.shape))


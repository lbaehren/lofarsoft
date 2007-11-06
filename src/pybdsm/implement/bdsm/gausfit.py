from Image import Op
from _cbdsm import *

class Op_gausfit(Op):
    """Fit all islands with mutiple 2-d gaussians
    """

    def __call__(self, img):
        self.beam = img.opts.beam
        for idx, isl in enumerate(img.islands):
            print "Fitting isl #", idx
            t = self.fit_island(isl, img.opts.thresh_isl)
            isl.gaus_fcn = t
            self.store_parameters(isl, t.parameters)

        return img

    def store_parameters(self, isl, par):
        res = []
        for p in par:
            npar = list(p)
            npar[1] += isl.bbox[0]
            npar[2] += isl.bbox[2]
            res.append(npar)

        isl.gaul = res

    def fit_island(self, isl, thr):
        dof = isl.size # number of active pixels
        init_shape = list(self.beam)
        fcn = MGFunction(isl.masked.data, isl.masked.mask, 1)
        fit = lmder_fit

        while True:
            peak, idx = fcn.find_peak()

            if peak < thr: ## no good peaks left
                break

            gtype = (Gtype.g3 if fcn.fitted_parameters() + 3 <= dof else None)
            gtype = (Gtype.g6 if fcn.fitted_parameters() + 6 <= dof else gtype)

            if gtype:
                fcn.add_gaussian(gtype, [peak, idx[0], idx[1]] + init_shape)
            else:
                break
            
            if not fit(fcn, final=0, verbose=1):
                break

        fit(fcn, final=1, verbose=1)
        return fcn

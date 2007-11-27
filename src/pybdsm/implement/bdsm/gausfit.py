from image import Op

class Op_gausfit(Op):
    """Fit all islands with 2-d gaussians
    """

    def __call__(self, img):
        for idx, isl in enumerate(img.islands):
            print "Fitting isl #", idx
            t = self.fit_island(isl, img.opts.beam, img.opts.thresh_isl, img.opts.rms)
            isl.gaus_fcn = t
            isl.gaul = self._parameters_to_global(isl, t.parameters)

        return img

    def fit_island(self, isl, beam, thr, norm=1):
        """Fit island"""
        from _cbdsm import MGFunction, Gtype, lmder_fit

        beam = list(beam)
        dof = isl.size # number of active pixels
        fcn = MGFunction(isl.img, isl.noisy_mask, norm)
        fit = lmder_fit

        while True:
            peak, idx = fcn.find_peak()

            if peak < thr: ## no good peaks left
                break

            gtype = (Gtype.g3 if fcn.fitted_parameters() + 3 <= dof else None)
            gtype = (Gtype.g6 if fcn.fitted_parameters() + 6 <= dof else gtype)

            if gtype:
                fcn.add_gaussian(gtype, [peak, idx[0], idx[1]] + beam)
            else:
                break
            
            if not fit(fcn, final=0, verbose=1):
                break

        fit(fcn, final=1, verbose=1)
        return fcn

    def _parameters_to_global(self, isl, par):
        """Update parameters by adjusting them to the proper image coordinates"""
        res = []
        for p in par:
            npar = list(p)
            npar[1] += isl.bbox[0]
            npar[2] += isl.bbox[2]
            res.append(npar)

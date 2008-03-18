from image import Op

class Op_gausfit(Op):
    """Fit all islands with 2-d gaussians
    """

    def __call__(self, img):
        for idx, isl in enumerate(img.islands):
            if img.opts.verbose_fitting:
                print "Fitting isl #", idx
            t = self.fit_island(isl, img.opts.beam, img.opts.thresh_isl, img.opts.rms, img.opts.verbose_fitting)
            isl.gaus_fcn = t
            isl.gaul = self._normalize_parameters(isl, t.parameters)

        return img

    def fit_island(self, isl, beam, thr, norm=1, verbose=1):
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
            
            if not fit(fcn, final=0, verbose=verbose):
                break

        fit(fcn, final=1, verbose=verbose)
        return fcn

    def _normalize_parameters(self, isl, par):
        """Normalize parameters by adjusting them to the proper image coordinates
        and ensuring that all of the implicit conventions (such as bmaj >= bmin) are met.
        """
        res = []
        for p in par:
            np = list(p)

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

            ### normalize position angle
            np[5] = divmod(np[5], 180)[1]

            res.append(np)
        return res

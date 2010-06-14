"""Module make_residimage.

It calculates residual image from the list of gaussians and shapelets
"""

import numpy as N
from image import *
import pyfits
from shapelets import *
import mylogger

### Insert attribute into Image class for model image
Image.resid_gaus = NArray(doc="Residual image calculated from " \
                                "extracted gaussians")
Image.resid_shap = NArray(doc="Residual image calculated from " \
                                "shapelet coefficient")

class Op_make_residimage(Op):
    """Creates an image from the fitted gaussians
    or shapelets.

    The resulting model image is stored in the 
    resid_gaus or resid_shap attribute.

    Prerequisites: module gausfit or shapelets should 
    be run first.
    """

    def __call__(self, img):
        import functions as func

        mylog = mylogger.logging.getLogger("PyBDSM."+img.log+"ResidImage")
        mylog.info("Calculating residual image after subtracting reconstructed gaussians")
        shape = img.ch0.shape
        thresh= img.opts.fittedimage_clip

        fimg = N.zeros(shape, dtype=float)

        for g in img.gaussians():
            C1, C2 = g.centre_pix
            isl = img.islands[g.island_id]
            b = self.find_bbox(thresh*isl.rms, g)

            bbox = N.s_[max(0, int(C1-b)):min(shape[0], int(C1+b+1)),
                        max(0, int(C2-b)):min(shape[1], int(C2+b+1))]

            x_ax, y_ax = N.mgrid[bbox]
            fimg[bbox] += func.gaussian_fcn(g, x_ax, y_ax)

        img.resid_gaus = img.ch0 - fimg
        pyfits.writeto(img.imagename + '.resid_gaus.fits', N.transpose(img.resid_gaus), img.header, clobber=True)
        mylog.info('%s %s' % ('Wrote ', img.imagename+'.resid_gaus.fits'))

        ### residual rms and mean per island
        for isl in img.islands:
            resid = img.resid_gaus[isl.bbox]
            n, m = resid.shape

            ind = N.where(~isl.mask_active)
            resid = resid[ind]
            isl.gresid_rms = N.std(resid)
            isl.gresid_mean = N.mean(resid)

        # Now residual image for shapelets
        if img.opts.shapelet_do:
            mylog.info("Calculating residual image after subtracting reconstructed shapelets")
            shape = img.ch0.shape
            fimg = N.zeros(shape, dtype=float)

            for isl in img.islands:
              if isl.shapelet_beta > 0: # make sure shapelet has nonzero scale for this island
                mask=isl.mask_active
                cen=isl.shapelet_centre-N.array(isl.origin)
                basis, beta, nmax, cf = isl.shapelet_basis, isl.shapelet_beta, \
                                        isl.shapelet_nmax, isl.shapelet_cf
                image_recons=reconstruct_shapelets(isl.shape, mask, basis, beta, cen, nmax, cf)
                fimg[isl.bbox] += image_recons
           
            img.resid_shap = img.ch0 - fimg
            pyfits.writeto(img.imagename + '.resid_shap.fits', N.transpose(img.resid_shap), img.header, clobber=True)
            mylog.info('%s %s' % ('Wrote ', img.imagename+'.resid_shap.fits'))

            ### shapelet residual rms and mean per island
            for isl in img.islands:
                resid = img.resid_shap[isl.bbox]
                n, m = resid.shape
                ind = N.where(~isl.mask_active)
                resid = resid[ind]
                isl.sresid_rms = N.std(resid)
                isl.sresid_mean = N.mean(resid)

        return img

    def find_bbox(self, thresh, g):
        """Calculate bounding box for gaussian.

        This function calculates size of the box for evaluating
        gaussian, so that value of gaussian is smaller than threshold
        outside of the box.

        Parameters:
        thres: threshold
        g: Gaussian object
        """

        from math import ceil, sqrt, log
        A = g.peak_flux
        S = g.size_pix[0]
        return ceil(S*sqrt(-2*log(thresh/A)))



"""Module shapelets

This will do all the shapelet analysis of islands in an image
"""

from image import *
from islands import *
from shapelets import *

Island.shapelet_basis=String(doc="Coordinate system for shapelet decomposition (cartesian/polar)")
Island.shapelet_beta=Float(doc="Value of shapelet scale beta")
Island.shapelet_nmax=Int(doc="Maximum value of shapelet order")
Island.shapelet_centre=Tuple(Float(), Float(),doc="Centre for the shapelet decomposition")
Island.shapelet_cf=NArray(doc="Coefficient matrix of the shapelet decomposition")

class Op_shapelets(Op):
    """ Get the image and mask from each island and send it to 
    shapelet programs which can then also be called seperately """

    def __call__(self, img):
    
        for id, isl in enumerate(img.islands):
            arr=isl.image
	    mask=isl.mask_active + isl.mask_noisy
	    basis=img.opts.shapelet_basis
	    beam_pix=img.beam_deg2pix(img.beam)
            mode=img.opts.shapelet_fitmode
            if mode != 'fit': mode=''

	    fixed=(0,0,0)
	    (beta, centre, nmax)=self.get_shapelet_params(arr, mask, basis, beam_pix, fixed)

	    cf=decompose_shapelets(arr, mask, basis, beta, centre, nmax, mode)

	    image_recons=reconstruct_shapelets(arr.shape, mask, basis, beta, centre, nmax, cf)

	    isl.shapelet_beta=beta
	    isl.shapelet_centre=centre
	    isl.shapelet_nmax=nmax
	    isl.shapelet_basis=basis
	    isl.shapelet_cf=cf


    def get_shapelet_params(self, image, mask, basis, beam_pix, fixed, beta=None, cen=None, nmax=None):
         """ This takes as input an image, its mask (false=valid), basis="cartesian"/"polar", 
	     fixed=(i,j,k) where i,j,k =0/1 to calculate or take as fixed for (beta, centre, nmax),
	     beam_pix has the beam in (pix_fwhm, pix_fwhm, deg), 
	     beta (the scale), cen (centre of basis expansion), nmax (max order). The output
	     is an updated set of values of (beta, centre, nmax). If fixed is 1 and the value is not
	     specified as an argument, then fixed is taken as 0."""
	 from math import sqrt, log
	 import compute as comp

	 if fixed[0]==1 and beta==None: fixed[0]=0
	 if fixed[1]==1 and cen==None: fixed[1]=0
	 if fixed[2]==1 and nmax==None: fixed[2]=0

         if fixed[0]*fixed[1]==0:
             (m1, m2, m3)=comp.moment(image, mask)

         if fixed[0]==0:
             beta=sqrt(m3[0]*m3[1])*2.0
	 if fixed[1]==0:
             cen=m2
         if fixed[2]==0:
             (n, m)=image.shape
             nmax=int(round(sqrt(1.0*n*n+m*m)/beam_pix[1]))-1
             nmax=min(max(nmax*2+2,10),14)                      # totally ad hoc

         betarange=[0.5,sqrt(beta*max(n,m))]  # min, max

	 if fixed[1]==0:
             cen=shape_findcen(image, mask, basis, beta, nmax, beam_pix) # + check_cen_shapelet

         if fixed[0]==0:
             beta=shape_varybeta(image, mask, basis, beta, cen, nmax, betarange, plot=False)

	 if fixed[1]==0 and fixed[0]==0:
             cen=shape_findcen(image, mask, basis, beta, nmax, beam_pix) # + check_cen_shapelet

         return beta, cen, nmax

 
       


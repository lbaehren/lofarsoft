"""Module shapelets.

nmax => J = 0..nmax; hence nmax+1 orders calculated. 
ordermax = nmax+1; range(ordermax) has all the values of n
Order n => J=n, where J=0 is the gaussian.

this is what was written earlier. will instead use pybdsm one and save this.

"""

import numpy as N
import pyfits
from scipy.optimize import leastsq
import pylab as pl

def decompose_shapelets(image, mask, basis, beta, centre, nmax, mode):
    """ Decomposes image (with mask) and beta, centre (2-tuple) , nmax into basis
        shapelets and returns the coefficient matrix cf. 
	Mode is 'fit' or 'integrate' for method finding coeffs. If fit then integrated
	values are taken as initial guess.
	Centre is by normal convention, not for idiots who count from zero. """
	
    import compute_shapelet_coeff as csc

    #print ' DOESNT HANDLE MASKS YET !!!!! '
    bad = False
    if (beta < 0 or beta/max(image.shape) > 5 or \
       (max(N.abs(list(centre)))-max(image.shape)/2) > 10*max(image.shape)): bad = True

    hc = []
    if not bad:
      hc = csc.shapelet_coeff(nmax, basis)
    else:
      print ' Bad input params'
    ordermax=nmax+1

    Bset=N.zeros((ordermax, ordermax, image.shape[0], image.shape[1]))
    cf = N.zeros((ordermax,ordermax)) # coefficient matrix, will fill up only lower triangular part.
    index = [(i,j) for i in range(ordermax) for j in range(ordermax-i)]  # i=0->nmax, j=0-nmax-i
    for coord in index:
        B = shapelet_image(basis, beta, centre, hc, coord[0], coord[1], image.shape)
	if mode == 'fit': Bset[coord[0] , coord[1], ::] = B
	m = N.copy(mask)
	for i, v in N.ndenumerate(mask): m[i] = not v

	cf[coord] = N.sum(image*B*m)

    if mode == 'fit':
      npix = N.product(image.shape)-N.sum(mask)
      npara = (nmax+1)*(nmax+2)*0.5
      if npara >= npix:
        print ' Cant fit, too many parameters, take integrated values. '
      else:
        print ' Fitting the shapelet coefficients directly '
        cfnew = fit_shapeletbasis(image, mask, basis, beta, centre, nmax, cf, Bset)
        cfres = cf - cfnew
	print ' Max deviation : ',cfres.max(),' at ',N.unravel_index(N.argmax(cfres), cfres.shape) \
              ,'; rms : ',N.std(cfres)
	cf = cfnew

    return cf

def fit_shapeletbasis(image, mask, basis, beta, centre, nmax, cf0, Bset):
    """ Fits the image to the shapelet basis functions to estimate shapelet coefficients
    instead of integrating it out. This should avoid the problems of digitisation and hence
    non-orthonormality. """
    import functions as func
    
    cfshape = cf0.shape

    res=lambda p, image, Bset, cfshape : image.flatten()-func.shapeletfit(p, Bset, cfshape)
    (cf, flag)=leastsq(res, cf0.flatten(), args=(image, Bset, cfshape))
    cf = cf.reshape(cfshape)

    return cf

def reconstruct_shapelets(size, mask, basis, beta, centre, nmax, cf):
    """ Reconstructs a shapelet image of size, for pixels which are unmasked, for a given
    beta, centre, nmax, basis and the shapelet coefficient matrix cf. """
    import compute_shapelet_coeff as csc

    rimage = N.zeros(size)
    hc = []
    hc = csc.shapelet_coeff(nmax, basis)

    index = [(i,j) for i in range(nmax) for j in range(nmax-i)]
    for coord in index:
        B = shapelet_image(basis, beta, centre, hc, coord[0], coord[1], size)
        rimage += B*cf[coord]

    return rimage
 
def shapelet_image(basis, beta, centre, hc, nx, ny, size):
    """ Takes basis, beta, centre (2-tuple), hc matrix, x, y, size and returns the image of the shapelet of
    order nx,ny on an image of size size. Does what getcartim.f does in fBDSM. nx,ny -> 0-nmax
    Centre is by Python convention, for idiots who count from zero. """
    from math import sqrt,pi
    import scipy
    hcx = hc[nx,:]
    hcy = hc[ny,:]
    ind = N.array([nx,ny])
    fact = scipy.factorial(ind)
    dumr1 = N.sqrt((2.0**(ind))*sqrt(pi)*fact)

    x = (N.arange(size[0],dtype=float)-(centre[0]-1.))/beta
    y = (N.arange(size[1],dtype=float)-(centre[1]-1.))/beta

    dumr3 = N.zeros(size[0])
    for i in range(size[0]):
        for j in range(ind[0]+1):   
            dumr3[i] += hcx[j]*(x[i]**j)
    B_nx = N.exp(-0.50*x*x)*dumr3/dumr1[0]/sqrt(beta)

    dumr3 = N.zeros(size[1])
    for i in range(size[1]):
        for j in range(ind[1]+1):
            dumr3[i] += hcy[j]*(y[i]**j)
    B_ny = N.exp(-0.50*y*y)*dumr3/dumr1[1]/sqrt(beta)

    return N.outer(B_nx,B_ny)


def shape_findcen(image, mask, basis, beta, nmax, beam_pix): # + check_cen_shapelet
    """ Finds the optimal centre for shapelet decomposition. Minimising various
    combinations of c12 and c21, as in literature doesnt work for all cases. 
    Hence, for the c1 image, we find the zero crossing for every vertical line 
    and for the c2 image, the zero crossing for every horizontal line, and then 
    we find intersection point of these two. This seems to work even for highly 
    non-gaussian cases. """
    import compute_shapelet_coeff as csc
    import compute as comp
    import functions as func

    hc = []
    hc = csc.shapelet_coeff(nmax, basis)

    msk=N.zeros(mask.shape, dtype=bool)
    for i, v in N.ndenumerate(mask): msk[i] = not v

    n,m = image.shape
    cf12 = N.zeros(image.shape)
    cf21 = N.zeros(image.shape)
    index = [(i,j) for i in range(n) for j in range(m)]
    for coord in index:
        if msk[coord]:
            B12 = shapelet_image(basis, beta, coord, hc, 1, 2, image.shape)
	    cf12[coord] = N.sum(image*B12*msk)
            B21 = shapelet_image(basis, beta, coord, hc, 2, 1, image.shape)
	    cf21[coord] = N.sum(image*B21*msk)
        else:
	    cf12[coord] = None
	    cf21[coord] = None

    (xmax,ymax) = N.unravel_index(image.argmax(),image.shape)  #  FIX  with mask 
    if xmax in [1,n] or ymax in [1,m]:
        (m1, m2, m3) = comp.moment(mask)
	xmax,ymax = N.round(m2)

    # in high snr area, get zero crossings for each horizontal and vertical line for c1, c2 resp
    tr_mask=mask.transpose()
    tr_cf21=cf21.transpose()
    (x1,y1) = getzeroes_matrix(mask, cf12, ymax, xmax)         # y1 is array of zero crossings
    (y2,x2) = getzeroes_matrix(tr_mask, tr_cf21, xmax, ymax)    # x2 is array of zero crossings

    # find nominal intersection pt as integers
    xind=N.where(x1==xmax)
    yind=N.where(y2==ymax)
    xind=xind[0][0]
    yind=yind[0][0]

    # now take 2 before and 2 after, fit straight lines, get proper intersection
    ninter=5
    if xind<3 or yind<3 or xind>n-2 or yind>m-2:
        ninter = 3
    xft1 = x1[xind+2-ninter:xind+2]
    yft1 = y1[xind+2-ninter:xind+2]
    xft2 = x2[yind+2-ninter:yind+2]
    yft2 = y2[yind+2-ninter:yind+2]
    sig  = N.zeros(ninter, dtype=float)
    smask1=N.array([r == 0 for r in yft1])
    smask2=N.array([r == 0 for r in xft2])
    cen=[0.]*2
    if sum(smask1)>0 and sum(smask2)>0:
        c1, m1 = comp.fit_mask_1d(xft1, yft1, sig, smask1, func.poly, order=1)
        c2, m2 = comp.fit_mask_1d(xft2, yft2, sig, smask2, func.poly, order=1)
        cen[0]=(c1-c2)/(m2-m1)
	cen[1]=c1+m1*cen[0]

    # check if estimated centre makes sense
    error=shapelet_check_centre(image, mask, cen, beam_pix)
    if error > 0:
        (m1, m2, m3) = comp.moment(image, mask)
	cen = m2
    
    return cen

def getzeroes_matrix(mask, cf, cen, cenx):
    """ For a matrix cf, and a mask, this returns two vectors; x is the x-coordinate
    and y is the interpolated y-coordinate where the matrix cf croses zero. If there 
    is no zero-crossing, y is zero for that column x. """

    x = N.arange(cf.shape[0], dtype = float)
    y = N.zeros(cf.shape[0], dtype = float)

    for i in range(cf.shape[0]):
        l = [mask[i,j] for j in range(cf.shape[1])]
        npts = len(l)-sum(l)
     
	if npts > 3 and not N.isnan(cf[i,cen]):
 	    mrow=mask[i,:]
	    if sum(l) == 0:
               low=0
	       up=cf.shape[1]-1
            else:
                low = mrow.nonzero()[0][mrow.nonzero()[0].searchsorted(cen)-1]
                up  = mrow.nonzero()[0][mrow.nonzero()[0].searchsorted(cen)]
	    low += 1; up -= 1
	    npoint = up-low+1
	    xfn = N.arange(npoint)+low
	    yfn = cf[i,xfn]
	    root, error = shapelet_getroot(xfn, yfn, x[i], cenx, cen)
	    if error != 1:
                y[i] = root

    return x,y

def shapelet_getroot(xfn, yfn, xco, xcen, ycen):
    """ This finds the root for finding the shapelet centre. If there are multiple roots, takes 
    that which closest to the 'centre', taken as the intensity barycentre. This is the python
    version of getroot.f of anaamika."""
    import compute as comp
    import functions as func

    npoint=len(xfn)
    error=0
    if yfn.max()*yfn.min() >= 0.:
        error=1

    minint=0; minintold=0
    for i in range(1,npoint):
        if yfn[i-1]*yfn[i] < 0.:
            if minintold == 0:  # so take nearest to centre
	        if abs(yfn[i-1]) < abs(yfn[i]):
	            minint=i-1
                else:
	            minint=i
            else:
	        dnew=comp.dist_2pt([xco,xfn[i]], [xcen,ycen])
	        dold=comp.dist_2pt([xco,xfn[minintold]], [xcen,ycen])
		if dnew <= dold:
		    minint=i
                else:
		    minint=minintold
            minintold=minint
       
    if minint < 1 or minint > npoint: error=1
    if error != 1:
        low=minint-min(2,minint-1)
	up=minint+min(2,npoint-minint)
	nfit=up-low+1
	xfit=xfn[low:low+nfit]
	yfit=yfn[low:low+nfit]
	sig=N.ones(nfit)
	smask=N.zeros(nfit, dtype=bool)
	xx=[i for i in range(low,low+nfit)]

    c, m = comp.fit_mask_1d(xfit, yfit, sig, smask, func.poly, order=1)
    root=-c/m
    if root < xfn[low] or root > xfn[up]: error=1

    return root, error

def shapelet_check_centre(image, mask, cen, beam_pix):
    "Checks if the calculated centre for shapelet decomposition is sensible. """
    from math import pi

    error = 0
    n, m = image.shape
    x, y = round(cen[0]), round(cen[1])
    if x <= 0 or x >= n or y <= 0 or y >= m: error = 1
    if error == 0: 
        if not mask[int(round(x)),int(round(y))]: error == 2

    if error > 0:
        if (N.product(mask.shape)-sum(sum(mask)))/(pi*0.25*beam_pix[0]*beam_pix[1]) < 2.5:
	    error = error*10   # expected to fail since source is too small

    return error

def shape_varybeta(image, mask, basis, betainit, cen, nmax, betarange, plot):
    """ Shapelet decomposes and then reconstructs an image with various values of beta
    and looks at the residual rms vs beta to estimate the optimal value of beta. """

    betamin, betamax = betarange
    nbin=30; delta = (betamax-betamin)/(nbin-1.)
    beta_arr = N.arange(betamin, betamax+delta, delta)
    nbin = len(beta_arr)

    # same as fortran
    #nbin = 20
    #delta = (2.0*betainit-betainit/2.0)/nbin
    #beta_arr = betainit/4.0+N.arange(nbin)*delta

    res_rms=N.zeros(nbin)
    for i in range(len(beta_arr)):
        cf = decompose_shapelets(image, mask, basis, beta_arr[i], cen, nmax, mode='')
	im_r=reconstruct_shapelets(image.shape, mask, basis, beta_arr[i], cen, nmax, cf)
	im_res = image - im_r
	res_rms[i] = N.std(im_res)  # FIX for mask
    minind = N.argmin(res_rms)
    if minind > 1 and minind < nbin:
        beta = beta_arr[minind]
	error = 0
    else:
	beta = betainit
	error = 1

    if plot:
      pl.figure()
      pl.plot(beta_arr,res_rms,'*-')
      pl.xlabel('Beta')
      pl.ylabel('Residual rms')

    return beta, error




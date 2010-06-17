# some functions 

def poly(c,x):
    """ y = Sum { c(i)*x^i }, i=0,len(c)"""
    import numpy as N
    y=N.zeros(len(x))
    for i in range(len(c)):
        y += c[i]*(x**i)
    return y

def sp_in(c, x):
    """ Spectral index in freq-flux space """
    import numpy as N

    order = len(c)-1
    if order == 1:
      y = c[0]*N.power(x, c[1])
    else:
      if order == 2:
        y = c[0]*N.power(x, c[1])*N.power(x, c[2]*N.log(x))
      else:
        print 'Not yet implemented'

    return y

def wenss_fit(c,x):
    """ sqrt(c0*c0 + c1^2/x^2)"""
    import numpy as N
    y = N.sqrt(c[0]*c[0]+c[1]*c[1]/(x*x))
    return y

def nanmean(x):
    """ Mean of array with NaN """
    import numpy as N

    sum = N.nansum(x)
    n = N.sum(~N.isnan(x))

    if n > 0:
      mean = sum/n
    else:
      mean = float("NaN")

    return mean

def shapeletfit(cf, Bset, cfshape):
    """ The function """
    import numpy as N

    ordermax = Bset.shape[0]
    y = (Bset[0,0,::]).flatten()
    y = N.zeros(y.shape)
    index = [(i,j) for i in range(ordermax) for j in range(ordermax-i)]  # i=0->nmax, j=0-nmax-i
    for coord in index:
	linbasis = (Bset[coord[0], coord[1], ::]).flatten()
        y += cf.reshape(cfshape)[coord]*linbasis

    return y

def func_poly2d(ord,p,x,y):
    """ 2d polynomial.
    ord=0 : z=p[0]
    ord=1 : z=p[0]+p[1]*x+p[2]*y
    ord=2 : z=p[0]+p[1]*x+p[2]*y+p[3]*x*x+p[4]*y*y+p[5]*x*y 
    ord=3 : z=p[0]+p[1]*x+p[2]*y+p[3]*x*x+p[4]*y*y+p[5]*x*y+
              p[6]*x*x*x+p[7]*x*x*y+p[8]*x*y*y+p[9]*y*y*y"""

    if ord == 0:
        z=p[0]
    if ord == 1: 
        z=p[0]+p[1]*x+p[2]*y
    if ord == 2: 
        z=p[0]+p[1]*x+p[2]*y+p[3]*x*x+p[4]*y*y+p[5]*x*y
    if ord == 3: 
        z=p[0]+p[1]*x+p[2]*y+p[3]*x*x+p[4]*y*y+p[5]*x*y+\
          p[6]*x*x*x+p[7]*x*x*y+p[8]*x*y*y+p[9]*y*y*y
    if ord > 3: 
        print " We do not trust polynomial fits > 3 "
	z = None

    return z

def func_poly2d_ini(ord, av):
    """ Initial guess -- assume flat plane. """

    if ord == 0:
        p0 = N.asarray([av])
    if ord == 1:
        p0 = N.asarray([av] + [0.0]*2)
    if ord == 2:
        p0 = N.asarray([av] + [0.0]*5)
    if ord == 3:
        p0 = N.asarray([av] + [0.0]*9)
    if ord > 3:
        p0 = None

    return p0

def ilist(x):
    """ integer part of a list of floats. """

    fn = lambda x : [int(round(i)) for i in x]
    return fn(x)

def cart2polar(cart, cen):
    """ convert cartesian coordinates to polar coordinates around cen. theta is
    zero for +ve xaxis and goes counter clockwise. cart is a numpy array [x,y] where
    x and y are numpy arrays of all the (>0) values of coordinates."""
    import math

    polar = N.zeros(cart.shape)
    pi = math.pi
    rad = 180.0/pi

    cc = N.transpose(cart)
    cc = (cc-cen)*(cc-cen)
    polar[0] = N.sqrt(N.sum(cc,1))
    th = N.arctan2(cart[1]-cen[1],cart[0]-cen[0])*rad
    polar[1] = N.where(th > 0, th, 360+th)

    return polar


def polar2cart(polar, cen):
    """ convert polar coordinates around cen to cartesian coordinates. theta is
    zero for +ve xaxis and goes counter clockwise. polar is a numpy array of [r], [heta]
    and cart is a numpy array [x,y] where x and y are numpy arrays of all the (>0) 
    values of coordinates."""
    import math

    cart = N.zeros(polar.shape)
    pi = math.pi
    rad = 180.0/pi

    cart[0]=polar[0]*N.cos(polar[1]/rad)+cen[0]
    cart[1]=polar[0]*N.sin(polar[1]/rad)+cen[1]

    return cart

def gaus_pixval(g, pix):
    """ Calculates the value at a pixel pix due to a gaussian object g. """
    from const import fwsig, pi
    from math import sin, cos, exp

    cen = g.centre_pix
    peak = g.peak_flux
    bmaj_p, bmin_p, bpa_p = g.size_pix

    a4 = bmaj_p/fwsig; a5 = bmin_p/fwsig
    a6 = (bpa_p+90.0)*pi/180.0
    spa = sin(a6); cpa = cos(a6)
    dr1 = ((pix[0]-cen[0])*cpa + (pix[1]-cen[1])*spa)/a4
    dr2 = ((pix[1]-cen[1])*cpa - (pix[0]-cen[0])*spa)/a5
    pixval = peak*exp(-0.5*(dr1*dr1+dr2*dr2))

    return pixval
    
def atanproper(dumr, dx, dy):
    from math import pi
    
    ysign = (dy >= 0.0)
    xsign = (dx >= 0.0)
    if ysign and (not xsign): dumr = pi - dumr
    if (not ysign) and (not xsign): dumr = pi + dumr
    if (not ysign) and xsign: dumr = 2.0*pi - dumr

    return dumr

def gdist_pa(pix1, pix2, gsize):
    """ Computes FWHM in degrees in the direction towards second source, of an elliptical gaussian. """
    from math import atan, pi, sqrt, cos, sin, tan

    dx = pix2[0] - pix1[0]
    dy = pix2[1] - pix1[1]
    dumr = atan(abs(dy/dx))
    val = atanproper(dumr, dx, dy)

    psi = val - (gsize[2]+90.0)/180.0*pi
                                # convert angle to eccentric anomaly
    psi=atan(gsize[0]/gsize[1]*tan(psi))  
    dumr2 = gsize[0]*cos(psi)
    dumr3 = gsize[1]*sin(psi)
    fwhm = sqrt(dumr2*dumr2+dumr3*dumr3)
 
    return fwhm
  
def gaus_2d(c, x, y):
    """ x and y are 2d arrays with the x and y positions. """
    import math
    import numpy as N

    rad = 180.0/math.pi
    cs = math.cos(c[5]/rad)
    sn = math.sin(c[5]/rad)
    f1 = ((x-c[1])*cs+(y-c[2])*sn)/c[3]
    f2 = ((y-c[2])*cs-(x-c[1])*sn)/c[4]
    val = c[0]*N.exp(-0.5*(f1*f1+f2*f2))

    return val

def gaus_2d_itscomplicated(c, x, y, p_tofix, ind):
    """ x and y are 2d arrays with the x and y positions. c is a list (of lists) of gaussian parameters to fit, p_tofix 
    are gaussian parameters to fix. ind is a list with 0, 1; 1 = fit; 0 = fix. """

    import math
    import numpy as N

    val = N.zeros(x.shape)
    indx = N.array(ind)
    if len(indx) % 6 != 0:
      print " Something wrong with the parameters passed - need multiples of 6 !"
    else:
      ngaus = len(indx)/6
      params = N.zeros(6*ngaus)
      params[N.where(indx==1)[0]] = c
      params[N.where(indx==0)[0]] = p_tofix
      for i in range(ngaus):
        gau = params[i*6:i*6+6]
        val = val + gaus_2d(gau, x, y)

    return val

def g2param(g):
    """Convert gaussian object g to param list [amp, cenx, ceny, sigx, sigy, theta] """
    from const import fwsig
    from math import pi

    A = g.peak_flux
    cenx, ceny = g.centre_pix
    sigx, sigy, th = g.size_pix
    sigx = sigx/fwsig; sigy = sigy/fwsig; th = th+90.0
    params = [A, cenx, ceny, sigx, sigy, th]

    return params

def corrected_size(size):
    """ convert major and minor axis from sigma to fwhm and angle from horizontal to P.A. """

    from const import fwsig

    csize = [0,0,0]
    csize[0] = size[0]*fwsig 
    csize[1] = size[1]*fwsig  
    bpa = size[2]
    pa = bpa-90.0            
    pa = pa % 360
    if pa < 0.0: pa = pa + 360.0  
    if pa > 180.0: pa = pa - 180.0 
    csize[2] = pa
  
    return csize
 
def drawellipse(g):
    import math
    import numpy as N
    from gausfit import Gaussian

    rad = 180.0/math.pi
    if isinstance(g, Gaussian):
      param = g2param(g)
    else:
      if isinstance(g, list) and len(g)>=6:
        param = g
      else:
        raise RuntimeError("Input tp drawellipse neither Gaussian not list")

    x2 = []; y2 = []
    for th in range(0, 370, 10):
      x1=param[3]*math.cos(th/rad)
      y1=param[4]*math.sin(th/rad)
      x2.append(x1*math.cos(param[5]/rad)-y1*math.sin(param[5]/rad)+param[1])
      y2.append(x1*math.sin(param[5]/rad)+y1*math.cos(param[5]/rad)+param[2])
    x2 = N.array(x2); y2 = N.array(y2)

    return x2, y2

def mask_fwhm(g, fac1, fac2, delc, shap):
    """ take gaussian object g and make a mask (as True) for pixels which are outside (less flux) 
        fac1*FWHM and inside (more flux) fac2*FWHM. Also returns the values as well."""
    import math
    import numpy as N
    from const import fwsig

    x, y = N.indices(shap)
    params = g2param(g)
    params[1] -= delc[0]; params[2] -= delc[1]
    gau = gaus_2d(params, x, y)
    dumr1 = 0.5*fac1*fwsig
    dumr2 = 0.5*fac2*fwsig
    flux1= params[0]*math.exp(-0.5*dumr1*dumr1)
    flux2 = params[0]*math.exp(-0.5*dumr2*dumr2)
    mask = (gau <= flux1) * (gau > flux2)
    gau = gau * mask

    return mask, gau
        
def flatten(x): 
    """flatten(sequence) -> list
    Taken from http://kogs-www.informatik.uni-hamburg.de/~meine/python_tricks

    Returns a single, flat list which contains all elements retrieved
    from the sequence and all recursively contained sub-sequences
    (iterables).

    Examples:
    >>> [1, 2, [3,4], (5,6)]
    [1, 2, [3, 4], (5, 6)]
    >>> flatten([[[1,2,3], (42,None)], [4,5], [6], 7, MyVector(8,9,10)])
    [1, 2, 3, 42, None, 4, 5, 6, 7, 8, 9, 10]"""

    result = []
    for el in x:
        #if isinstance(el, (list, tuple)):
        if hasattr(el, "__iter__") and not isinstance(el, basestring):
            result.extend(flatten(el))
        else:
            result.append(el)
    return result

def moment(x,mask=None):
    """ 
    Calculates first 3 moments of numpy array x. Only those values of x 
    for which mask is False are used, if mask is given. Works for any 
    dimension of x. 
    """
    import numpy as N

    if mask == None:
        mask=N.zeros(x.shape, dtype=bool)
    m1=N.zeros(1)
    m2=N.zeros(x.ndim) 
    m3=N.zeros(x.ndim)
    for i, val in N.ndenumerate(x):
        if not mask[i]:
            m1 += val
	    m2 += val*N.array(i)
	    m3 += val*N.array(i)*N.array(i)
    m2 /= m1
    m3 = N.sqrt(m3/m1-m2*m2)
    return m1, m2, m3

def fit_mask_1d(x, y, sig, mask, funct, do_err, order=0, p0 = None):
    """ 
    Calls scipy.optimise.leastsq for a 1d function with a mask.
    Takes values only where mask=False.
    """
    from scipy.optimize import leastsq
    import functions as func
    from math import sqrt, pow
    import numpy as N
    #import pylab as pl

    ind=N.where(~N.array(mask))[0]
    if len(ind) > 1:
      n=sum(mask)

      if isinstance(x, list): x = N.array(x)
      if isinstance(y, list): y = N.array(y)
      if isinstance(sig, list): sig = N.array(sig)
      xfit=x[ind]; yfit=y[ind]; sigfit=sig[ind]

      if p0 == None:
        if funct == func.poly:
           p0=N.array([0]*(order+1))
           p0[1]=(yfit[0]-yfit[-1])/(xfit[0]-xfit[-1])
           p0[0]=yfit[0]-p0[1]*xfit[0]
        if funct == func.wenss_fit:
           p0=N.array([yfit[N.argmax(xfit)]] + [1.])
        if funct == func.sp_in:
           ind1 = N.where(yfit > 0.)[0]
           if len(ind1) >= 2:
             low = ind1[0]; hi = ind1[-1]
             sp = N.log(yfit[low]/yfit[hi])/N.log(xfit[low]/xfit[hi])
             p0=N.array([yfit[low]/pow(xfit[low], sp), sp] + [0.]*(order-1))
           else:
             p0=N.array([ind1[0], -0.8] + [0.]*(order-1))
      res=lambda p, xfit, yfit, sigfit: (yfit-funct(p, xfit))/sigfit
      (p, cov, info, mesg, flag)=leastsq(res, p0, args=(xfit, yfit, sigfit), full_output=True)
  
      if do_err: 
        if cov != None:
          if N.sum(sig != 1.) > 0:
            err = N.array([sqrt(cov[i,i]) for i in range(len(p))])
          else:
            chisq=sum(info["fvec"]*info["fvec"])
            dof=len(info["fvec"])-len(p)
            err = N.array([sqrt(cov[i,i]*chisq/dof) for i in range(len(p))])
        else:
          p, err = [0, 0], [0, 0]
      else: err = [0]
    else:
      p, err = [0, 0], [0, 0]

    return p, err

def dist_2pt(p1, p2):
    """ Calculated distance between two points given as tuples p1 and p2. """
    from math import sqrt
    dx=p1[0]-p2[0]
    dy=p1[1]-p2[1]
    dist=sqrt(dx*dx + dy*dy)

    return dist

def std(y):
    """ Returns unbiased standard deviation. """
    from math import sqrt
    import numpy as N

    l=len(y)
    s=N.std(y)
    return s*sqrt(float(l)/(l-1))

def imageshift(image, shift):
    """ Shifts a 2d-image by the tuple (shift). Positive shift is to the right and upwards. 
    This is done by fourier shifting. """
    import scipy
    from scipy import ndimage

    shape=image.shape

    f1=scipy.fft(image, shape[0], axis=0)
    f2=scipy.fft(f1, shape[1], axis=1)

    s=ndimage.fourier_shift(f2,shift, axis=0)

    y1=scipy.ifft(s, shape[1], axis=1)
    y2=scipy.ifft(y1, shape[0], axis=0)

    return y2.real

def trans_gaul(q):
    " transposes a tuple "
    y=[]
    for i in range(len(q[0])):
        elem=[]
        for j in range(len(q)):
            elem.append(q[j][i])
        y.append(elem)
    return y

def momanalmask_gaus(subim, mask, isrc, bmar_p, allpara=True):
    """ Compute 2d gaussian parameters from moment analysis, for an island with
        multiple gaussians. Compute only for gaussian with index (mask value) isrc.
        Returns normalised peak, centroid, fwhm and P.A. assuming North is top. """

    from math import sqrt, atan, pi
    from const import fwsig
    import functions as func
    import numpy as N

    m1 = N.zeros(2); m2 = N.zeros(2); m11 = 0.0; tot = 0.0
    mompara = N.zeros(6)
    n, m = subim.shape[0], subim.shape[1]
    index = [(i, j) for i in range(n) for j in range(m) if mask[i,j]==isrc]
    for coord in index:
        tot += subim[coord]
        m1 += N.array(coord)*subim[coord]
    mompara[0] = tot/bmar_p
    mompara[1:3] = m1/tot

    if allpara:
      for coord in index:
          co = N.array(coord)
          m2 += (co - mompara[1:3])*(co - mompara[1:3])*subim[coord]
          m11 += N.product(co - mompara[1:3])*subim[coord]

      mompara[3] = sqrt((m2[0]+m2[1]+sqrt((m2[0]-m2[1])*(m2[0]-m2[1])+4.0*m11*m11))/(2.0*tot))*fwsig
      mompara[4] = sqrt((m2[0]+m2[1]-sqrt((m2[0]-m2[1])*(m2[0]-m2[1])+4.0*m11*m11))/(2.0*tot))*fwsig
      dumr = atan(abs(2.0*m11/(m2[0]-m2[1])))
      dumr = func.atanproper(dumr, m2[0]-m2[1], 2.0*m11)
      mompara[5] = 0.5*dumr*180.0/pi - 90.0
      if mompara[5] < 0.0: mompara[5] += 180.0
  
    return mompara
     
def fit_gaus2d(data, p_ini, x, y, mask = None, err = None):
    """ Fit 2d gaussian to data with x and y also being 2d numpy arrays with x and y positions. 
        Takes an optional error array and a mask array (True => pixel is masked). """
    from scipy.optimize import leastsq
    import functions as func
    import numpy as N

    if mask != None and mask.shape != data.shape:
        print 'Data and mask array dont have the same shape, ignoring mask'
        mask = None
    if err != None and err.shape != data.shape: 
        print 'Data and error array dont have the same shape, ignoring error'
        err = None

    if mask == None: mask = N.zeros(data.shape, bool)
    g_ind = N.where(~N.ravel(mask))[0]
    if err == None:
        errorfunction = lambda p: N.ravel(func.gaus_2d(p, x, y) - data)[g_ind]
    else:  
        errorfunction = lambda p: N.ravel((func.gaus_2d(p, x, y) - data)/err)[g_ind]
    p, success = leastsq(errorfunction, p_ini)

    return p, success

def deconv(gaus_bm, gaus_c):
    """ Deconvolves gaus_bm from gaus_c to give gaus_dc. 
        Stolen shamelessly from aips DECONV.FOR. """
    from math import pi, cos, sin, atan, sqrt

    rad = 180.0/pi
    gaus_d = [0.0, 0.0, 0.0]

    phi_c = gaus_c[2]+900.0 % 180
    phi_bm = gaus_bm[2]+900.0 % 180
    maj2_bm = gaus_bm[0]*gaus_bm[0]; min2_bm = gaus_bm[1]*gaus_bm[1]
    maj2_c = gaus_c[0]*gaus_c[0]; min2_c = gaus_c[1]*gaus_c[1]
    theta=2.0*(phi_c-phi_bm)/rad
    cost = cos(theta)
    sint = sin(theta)

    rhoc = (maj2_c-min2_c)*cost-(maj2_bm-min2_bm)
    if rhoc == 0.0:
      sigic = 0.0
      rhoa = 0.0
    else:
      sigic = atan((maj2_c-min2_c)*sint/rhoc)   # in radians
      rhoa = ((maj2_bm-min2_bm)-(maj2_c-min2_c)*cost)/(2.0*cos(sigic))

    gaus_d[2] = sigic*rad/2.0+phi_bm
    dumr = ((maj2_c+min2_c)-(maj2_bm+min2_bm))/2.0
    gaus_d[0] = dumr-rhoa
    gaus_d[1] = dumr+rhoa
    error = 0
    if gaus_d[0] < 0.0: error += 1
    if gaus_d[1] < 0.0: error += 1

    gaus_d[0] = max(0.0,gaus_d[0])
    gaus_d[1] = max(0.0,gaus_d[1])
    gaus_d[0] = sqrt(abs(gaus_d[0]))
    gaus_d[1] = sqrt(abs(gaus_d[1]))
    if gaus_d[0] < gaus_d[1]:
      sint = gaus_d[0]
      gaus_d[0] = gaus_d[1]
      gaus_d[1] = sint
      gaus_d[2] = gaus_d[2]+90.0

    gaus_d[2] = gaus_d[2]+900.0 % 180
    if gaus_d[0] == 0.0:
      gaus_d[2] = 0.0
    else:
      if gaus_d[1] == 0.0:
        if (abs(gaus_d[2]-phi_c) > 45.0) and (abs(gaus_d[2]-phi_c) < 135.0):
          gaus_d[2] = gaus_d[2]+450.0 % 180

# errors
     #if rhoc == 0.0:
    #if gaus_d[0] != 0.0:
    #  ed_1 = gaus_c[0]/gaus_d[0]*e_1
    #else:
    #  ed_1 = sqrt(2.0*e_1*gaus_c[0])
    #if gaus_d[1] != 0.0:
    #  ed_2 = gaus_c[1]/gaus_d[1]*e_2
    #else:
    #  ed_2 = sqrt(2.0*e_2*gaus_c[1])
    #ed_3 =e_3
    #else:
    #  pass
      
    return gaus_d

def get_errors(img, p, stdav):

    import functions as func
    from const import fwsig
    from math import sqrt, log, pow, pi
    import mylogger
    import numpy as N

    mylog = mylogger.logging.getLogger("PyBDSM.Compute   ")

    if len(p) % 7 > 0: 
      mylog.error("Gaussian parameters passed have to have 7n numbers")
    ngaus = len(p)/7
    errors = []
    for i in range(ngaus):
      pp = p[i*7:i*7+7]
                                        ### Now do error analysis as in Condon (and fBDSM)
      size = pp[3:6]
      size = func.corrected_size(size)
      sq2 = sqrt(2.0)
      bm_pix = N.array([img.pixel_beam[0]*fwsig, img.pixel_beam[1]*fwsig, img.pixel_beam[2]])
      dumr = sqrt(abs(size[0]*size[1]/(4.0*bm_pix[0]*bm_pix[1])))
      dumrr1 = 1.0+bm_pix[0]*bm_pix[1]/(size[0]*size[0])
      dumrr2 = 1.0+bm_pix[0]*bm_pix[1]/(size[1]*size[1])
      dumrr3 = dumr*pp[0]/stdav
      d1 = sqrt(8.0*log(2.0))
      d2 = (size[0]*size[0]-size[1]*size[1])/(size[0]*size[0])

      e_peak = pp[0]*sq2/(dumrr3*pow(dumrr1,0.75)*pow(dumrr2,0.75))
      e_x0=size[0]/fwsig*sq2/(d1*dumrr3*pow(dumrr1,1.25)*pow(dumrr2,0.25))
      e_y0=size[1]/fwsig*sq2/(d1*dumrr3*pow(dumrr1,0.25)*pow(dumrr2,1.25))
      e_maj=size[0]*sq2/(dumrr3*pow(dumrr1,1.25)*pow(dumrr2,0.25))
      e_min=size[1]*sq2/(dumrr3*pow(dumrr1,0.25)*pow(dumrr2,1.25))  # in fw
      e_pa=2.0/(d2*dumrr3*pow(dumrr1,0.25)*pow(dumrr2,1.25))
      e_pa=e_pa*180.0/pi
      e_tot=pp[0]*sqrt(e_peak*e_peak/(pp[0]*pp[0])+(0.25/dumr/dumr)*(e_maj*e_maj/(size[0]*size[0])+\
            e_min*e_min/(size[1]*size[1])))
      if abs(e_pa) > 180.0: e_pa=180.0  # dont know why i did this
      errors = errors + [e_peak, e_x0, e_y0, e_maj, e_min, e_pa, e_tot]

    return errors

def fit_chisq(x, p, ep, mask, funct, order):
    import numpy as N

    ind = N.where(N.array(mask)==False)[0]

    if order == 0: 
      fit = [funct(p)]*len(p)
    else:
      fitpara, efit = fit_mask_1d(x, p, ep, mask, funct, True, order)
      fit = funct(fitpara, x)

    dev = (p-fit)*(p-fit)/(ep*ep)
    num = order+1
    csq = N.sum(dev[ind])/(len(fit)-num-1)

    return csq

def calc_chisq(x, y, ey, p, mask, funct, order):
    import numpy as N

    if order == 0: 
      fit = [funct(y)]*len(y)
    else:
      fit = funct(p, x)

    dev = (y-fit)*(y-fit)/(ey*ey)
    ind = N.where(~N.array(mask))
    num = order+1
    csq = N.sum(dev[ind])/(len(mask)-num-1)

    return csq

def get_windowsize_av(S_i, rms_i, chanmask, K, minchan):
    import numpy as N

    av_window = N.arange(2, int(len(S_i)/minchan)+1)
    win_size = 0
    for window in av_window:
      fluxes, vars, mask = variance_of_wted_windowedmean(S_i, rms_i, chanmask, window)
      minsnr = N.min(fluxes[~mask]/vars[~mask])
      if minsnr > K*1.1:                ### K*1.1 since fitted peak can be less than wted peak
        win_size = window  # is the size of averaging window
        break

    return win_size

def variance_of_wted_windowedmean(S_i, rms_i, chanmask, window_size):
    from math import sqrt
    import numpy as N

    nchan = len(S_i)
    nwin = nchan/window_size
    wt = 1/rms_i/rms_i
    wt = wt/N.median(wt)
    fluxes = N.zeros(nwin); vars = N.zeros(nwin); mask = N.zeros(nwin, bool)
    for i in range(nwin):
      strt = i*window_size; stp = (i+1)*window_size
      if i == nwin-1: stp = nchan
      ind = N.arange(strt,stp)
      m = chanmask[ind] 
      index = [arg for ii,arg in enumerate(ind) if not m[ii]]
      if len(index) > 0:
        s = S_i[index]; r = rms_i[index]; w = wt[index]
        fluxes[i] = N.sum(s*w)/N.sum(w)
        vars[i] = 1.0/sqrt(N.sum(1.0/r/r))
        mask[i] = N.product(m)
      else:
        fluxes[i] = 0
        vars[i] = 0
        mask[i] = True

    return fluxes, vars, mask 

def fit_mulgaus2d(image, gaus, x, y, mask = None, fitfix = None, err = None):
    """ fitcode : 0=fit all; 1=fit amp; 2=fit amp, posn; 3=fit amp, size """
    from scipy.optimize import leastsq
    import numpy as N
   
    if mask != None and mask.shape != image.shape:
        print 'Data and mask array dont have the same shape, ignoring mask'
        mask = None
    if err != None and err.shape != image.shape: 
        print 'Data and error array dont have the same shape, ignoring error'
        err = None
    if mask == None: mask = N.zeros(image.shape, bool)

    g_ind = N.where(~N.ravel(mask))[0]

    ngaus = len(gaus)
    if ngaus > 0: 
      p_ini = []
      for g in gaus:
        p_ini = p_ini + func.g2param(g)
      p_ini = N.array(p_ini)

      if fitfix == None: fitfix = [0]*ngaus
      ind = N.ones(6*ngaus)                                     # 1 => fit ; 0 => fix
      for i in range(ngaus):
        if fitfix[i] == 1: ind[i*6+1:i*6+6] = 0
        if fitfix[i] == 2: ind[i*6+3:i*6+6] = 0
        if fitfix[i] == 3: ind[i*6+1:i*6+3] = 0
      ind = N.array(ind)
      p_tofit = p_ini[N.where(ind==1)[0]]
      p_tofix = p_ini[N.where(ind==0)[0]]
      if err == None: err = N.ones(image.shape)

      errorfunction = lambda p, x, y, p_tofix, ind, image, err, g_ind: \
                     N.ravel((func.gaus_2d_itscomplicated(p, x, y, p_tofix, ind)-image)/err)[g_ind]
      p, success = leastsq(errorfunction, p_tofit, args=(x, y, p_tofix, ind, image, err, g_ind))
    else:
      p, sucess = None, 1

    para = N.zeros(6*ngaus)
    para[N.where(ind==1)[0]] = p
    para[N.where(ind==0)[0]] = p_tofix

    for igaus in range(ngaus):
      para[igaus*6+3] = abs(para[igaus*6+3])
      para[igaus*6+4] = abs(para[igaus*6+4])

    return para, success

def gaussian_fcn(g, x1, x2):
    """Evaluate Gaussian on the given grid.

    Parameters:
    x1, x2: grid (as produced by numpy.mgrid f.e.)
    g: Gaussian object
    """
    from math import radians, sin, cos
    from const import fwsig
    import numpy as N

    A = g.peak_flux
    C1, C2 = g.centre_pix
    S1, S2, Th = g.size_pix
    S1 = S1/fwsig; S2 = S2/fwsig; Th = Th + 90.0

    th = radians(Th)
    cs = cos(th)
    sn = sin(th)

    f1 = ((x1-C1)*cs + (x2-C2)*sn)/S1
    f2 = (-(x1-C1)*sn + (x2-C2)*cs)/S2

    return A*N.exp(-(f1*f1 + f2*f2)/2)

def mclean(im1, c, beam):
    """ Simple image plane clean of one gaussian at posn c and size=beam """
    import numpy as N

    amp = im1[c]
    b1, b2, b3 = beam
    b3 += 90.0
    para = [amp, c[0], c[1], b1, b2, b3]
    x, y = N.indices(im1.shape)

    im = gaus_2d(para, x, y)
    im1 = im1-im

    return im1
     
def arrstatmask(im, mask):
    """ Basic statistics for a masked array. dont wanna use numpy.ma """
    import numpy as N

    ind = N.where(~mask)
    im1 = im[ind]
    av = N.mean(im1)
    std = N.std(im1)
    maxv = N.max(im1)
    x, y = N.where(im == maxv)
    xmax = x[0]; ymax = y[0]
    minv = N.min(im1)
    x, y = N.where(im == minv)
    xmin = x[0]; ymin = y[0]

    return (av, std, maxv, (xmax, ymax), minv, (xmin, ymin))

def get_maxima(im, mask, thr, shape, beam):
    """ Gets the peaks in an image """
    from copy import deepcopy as cp
    import numpy as N

    im1 = cp(im)
    ind = N.array(N.where(~mask)).transpose()
    ind = [tuple(coord) for coord in ind if im[tuple(coord)] > thr]
    n, m = shape; iniposn = []; inipeak = []
    for c in ind:
      goodlist = [im[i,j] for i in range(c[0]-1,c[0]+2) for j in range(c[1]-1,c[1]+2) \
                   if i>=0 and i<n and j>=0 and j<m and (i,j) != c]
      peak = N.sum(im[c] > goodlist) == len(goodlist)
      if peak:
        iniposn.append(c); inipeak.append(im[c])
        im1 = mclean(im1, c, beam)

    return inipeak, iniposn, im1 

def watershed(image, mask=None, markers=None, beam=None, thr=None):
      import numpy as N
      from copy import deepcopy as cp
      import scipy.ndimage as nd
      import pylab as pl

      if thr==None: thr = -1e9
      if mask==None: mask = N.zeros(image.shape, bool)
      if beam==None: beam = (2.0, 2.0, 0.0)
      if markers==None:
        inipeak, iniposn, im1 = get_maxima(image, mask, thr, image.shape, beam)
        ng = len(iniposn); markers = N.zeros(image.shape, int)
        for i in range(ng): markers[iniposn[i]] = i+2
        markers[N.unravel_index(N.argmin(image), image.shape)] = 1
      
      im1 = cp(image)
      if im1.min() < 0.: im1 = im1-im1.min()
      im1 = 255 - im1/im1.max()*255
      opw = nd.watershed_ift(N.array(im1, N.uint8), markers)

      return opw, markers









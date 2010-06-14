
def linfit(x, y, sig):
    """ Code the linfit.f from numerical recipes to compare with optimize.leastsq 
    of scipy, especially the errors. """
    from math import sqrt
    import numpy as N

    ndata = len(x)
    wt = 1.0/(sig*sig)
    ss = N.sum(wt)
    sx = N.sum(x*wt)
    sy = N.sum(y*wt)

    sxoss = sx/ss
    t = (x-sxoss)/sig
    st2 = N.sum(t*t)
    b = N.sum(t*y/sig)

    b = b/st2
    a = (sy-sx*b)/ss
    siga = sqrt((1.0+sx*sx/(ss*st2))/ss)
    sigb = sqrt(1.0/st2)
    dum = (y-a-b*x)/sig
    chi2 = N.sum(dum*dum)
   
    return [a,b], [siga, sigb]
    


""" Need to test varybeta as well as findcen. Compare with fortran routine inside BDSM."""

import numpy as N
import pyfits
import libs
import shapelets as sh
import compute as comp
import pylab as pl
import _anaamika as A
import sys

rnd = 6

#####################
# read in fits file #
#####################
fitsfilename = '/data/anaamika/fits/scorpion.fits'
fitsfile = pyfits.open(fitsfilename)
hdr = fitsfile[0].header
image = fitsfile[0].data
image = image[0]
image = N.array(image.transpose(), order='C',dtype=image.dtype.newbyteorder('='))
fitsfile.close()

##################################
# calc initial centre by moments #
##################################
crms = 0.113 # Jy (from scorpion.fits)
kappa = 4.0  # to avoid false pixels
mask = image < crms*kappa
(m1, cen, m3)=func.moment(image, mask)
print
print 'Max pixel in image = ',N.unravel_index(N.argmax(image), image.shape)
print 'PYTHON : Moment 0 = ',N.round(m1, rnd)
print 'PYTHON : Initial centre from moment = ',N.round(cen, rnd)
print 'PYTHON : Moment 2 = ',N.round(m3, rnd)
print

#############################
# calc moments from fortran #
#############################

maskf = N.invert(mask)
maskf = N.array(maskf, dtype=int)
momf = A.momentmaskonly(image, maskf, image.shape[0], image.shape[1])
print 'FORTRAN: Moment 0 = ',N.round(momf[0], rnd)
print 'FORTRAN: Initial centre from moment = ',N.round(momf[1], rnd)
print 'FORTRAN: Moment 2 = ',N.round(momf[2], rnd)
print

#######################
# vary beta in python #
#######################
nmax = 14
basis = 'cartesian'
betainit = 3.0
betarange = [1.0, 10.0]
betapy, err = sh.shape_varybeta(image, mask, basis, betainit, cen, nmax, betarange, plot=False)
print 'PYTHON :Best beta = ',N.round(betapy, rnd)

########################
# vary beta in fortran #
########################
nbin = 71
betaf = A.varybeta_cart(image,maskf,momf[1][0],momf[1][1],nmax+1,betainit,\
                     2.5, 6.0, nbin)
print 'FORTRAN:Best beta = ',N.round(betaf, rnd)
#print 'Dispersion and mean of difference between the residual rms (F-Py) : ',N.std(resrmsp-resrmsf), \
#      '; ',N.mean(resrmsp-resrmsf)

#pl.subplot(211)
#pl.plot(betarrp,resrmsp,'-*r')
#pl.plot(betarrf,resrmsf,'-*b')
#pl.subplot(212)
#pl.plot(betarrf,(resrmsp-resrmsf)/resrmsf,'-*b')

#####################
# findcen in python #
#####################
bmaj = hdr['BMAJ']; bmin = hdr['BMIN']; bpa = hdr['BPA'];
cdelt1 = hdr['CDELT1']; cdelt2 = hdr['CDELT2']; 
beam_pix = N.array([abs(bmaj/cdelt1)/2.35482, abs(bmin/cdelt2)/2.35482, bpa])
cenp = sh.shape_findcen(image, mask, basis, betapy, nmax, beam_pix)
print
print 'PYTHON : Best centre = ',N.round(cenp, rnd)

######################
# findcen in fortran #
######################

cenf = A.findcen_cart(image,maskf,betaf,nmax+1,sav='n',runcode='aq')
print 'FORTRAN: Best centre = ',N.round(cenf, rnd)

pl.show()









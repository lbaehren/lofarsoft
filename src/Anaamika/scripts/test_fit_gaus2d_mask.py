
import numpy as N
import pylab as pl
import functions as func
import scipy


x2, y2 = scipy.mgrid[0:10:0.1, 0:10:0.1]
p = [1.0, 2.5, 7.25, 3.2, 2.23, 30.0]
print 'Original params   =  ',p

z2 = func.gaus_2d(p, x2, y2)

z2r = N.copy(N.ravel(z2))
bad_ind = N.random.random_integers(0, len(z2r), 100)
z2r[bad_ind] = 100.5

z2_bad = z2r.reshape(z2.shape)
mask = N.where(z2_bad > 10, True, False)

p0 = list(N.array(p)*0.7)
print 'Initial  params   =  ',p0, '\n'

p, ierr = func.fit_gaus2d(z2_bad, p0, x2, y2, mask = mask)
print 'Fitted   params   =  ',p, ierr, '\n'


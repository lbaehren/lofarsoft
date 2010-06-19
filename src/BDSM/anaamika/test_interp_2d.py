
from matplotlib import pylab as pl
import scipy
from scipy import interpolate
from scipy.interpolate import fitpack
import numpy as N
from math import pi
import nat

x,y=scipy.mgrid[0:100:10,0:100:10]
xpr=N.ravel(x)
ypr=N.ravel(y)

xpi=N.random.random(300)*90.0
ypi=N.random.random(300)*90.0

border1 = N.zeros(90)
border2 = N.arange(90)
border3 = N.array([90]*90)
#xpi=N.append(xpi, border2)
#ypi=N.append(ypi, border1)
#xpi=N.append(xpi, border2)
#ypi=N.append(ypi, border3)
#xpi=N.append(xpi, border1[1:])
#ypi=N.append(ypi, border2[1:])
#xpi=N.append(xpi, border3)
#ypi=N.append(ypi, border2)

xnew,ynew = scipy.mgrid[0:90,0:90]

# im_real is the actual function computed on the full grid
# z_reg is the interpolated function for a regular input grid
# z_irr is the interpolated function for an irregular input grid
im_real=0.3*N.sin(xnew*pi*1./100.0) + 0.3*N.cos(ynew*pi*0.7/100.0)
z_reg=0.3*N.sin(xpr*pi*1./100.0) + 0.3*N.cos(ypr*pi*0.7/100.0)
z_irr=0.3*N.sin(xpi*pi*1./100.0) + 0.3*N.cos(ypi*pi*0.7/100.0)

reg=fitpack.bisplrep(xpr,ypr,z_reg,kx=3,ky=3,s=0.)
irr=fitpack.bisplrep(xpi,ypi,z_irr,kx=3,ky=3,s=0.)

im_reg=fitpack.bisplev(xnew[:,0],ynew[0,:],reg)
im_irr=fitpack.bisplev(xnew[:,0],ynew[0,:],irr)

# now use natgrid from cdat
xo=N.arange(0.0,90.0,1.0)
yo=N.arange(0.0,90.0,1.0)
r = nat.Natgrid(xpi, ypi, xo, yo)
im_natgrid = r.rgrd(z_irr)


pl.figure(None)

pl.subplot(3,4,1)
pl.imshow(im_real,interpolation='nearest')
pl.title('True image')
pl.colorbar()

pl.subplot(3,4,2)
pl.imshow(im_reg,interpolation='nearest')
pl.title('Regular grid image')
pl.colorbar()

pl.subplot(3,4,3)
pl.imshow(im_irr,interpolation='nearest')
pl.title('Irregular grid image')
pl.colorbar()

pl.subplot(3,4,4)
pl.imshow(im_natgrid,interpolation='nearest')
pl.title('Irregular natgrid')
pl.colorbar()

pl.subplot(3,4,5)
pl.plot(xpi, ypi, 'x')
pl.title('Irregular grid points')
pl.colorbar()

pl.subplot(3,4,6)
pl.imshow((im_real-im_reg)[10:81,10:81],interpolation='nearest')
#pl.imshow((im_real-im_reg),interpolation='nearest')
pl.title('Diff reg grid image')
pl.colorbar()

pl.subplot(3,4,7)
pl.imshow((im_real-im_irr)[10:81,10:81],interpolation='nearest')
#pl.imshow((im_real-im_irr),interpolation='nearest')
pl.title('Diff irr grid image')
pl.colorbar()

pl.subplot(3,4,8)
pl.imshow((im_real-im_natgrid)[10:81,10:81],interpolation='nearest')
#pl.imshow((im_real-im_natgrid),interpolation='nearest')
pl.title('Diff irr natgrid')
pl.colorbar()

pl.subplot(3,4,9)
im_reg_dum = N.zeros(im_real.shape)
for i in range(len(xpr)):
    xd = int(round(xpr[i]))
    yd = int(round(ypr[i]))
    if xd < 90 and yd < 90:
        im_reg_dum[xd, yd] = z_reg[i]
pl.imshow(im_reg_dum,interpolation='nearest')
pl.title('Input reg grid image')
pl.colorbar()

pl.subplot(3,4,10)
im_irr_dum = N.zeros(im_real.shape)
im_irr_diff1 = N.zeros(im_real.shape)
im_natirr_diff1 = N.zeros(im_real.shape)
for i in range(len(xpi)):
    xd = int(round(xpi[i]))
    yd = int(round(ypi[i]))
    if xd < 90 and yd < 90:
        im_irr_dum[xd, yd] = z_irr[i]
        im_irr_diff1[xd, yd] = im_irr[xd,yd]-z_irr[i]
        im_natirr_diff1[xd, yd] = im_natgrid[xd,yd]-z_irr[i]
pl.imshow(im_irr_dum,interpolation='nearest')
pl.title('Input irr grid image')
pl.colorbar()

pl.subplot(3,4,11)
pl.imshow(im_irr_diff1,interpolation='nearest')
pl.title('Diff irr grid image')
pl.colorbar()

pl.subplot(3,4,12)
pl.imshow(im_natirr_diff1,interpolation='nearest')
pl.title('Diff natirr grid image')
pl.colorbar()




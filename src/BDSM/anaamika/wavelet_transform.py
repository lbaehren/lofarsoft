# wavelet transform. a trous.

import pyfits
import numpy as N
import scipy.signal as S

def sigclip(image, nsig=4):
 stdclip=image.std()
 return stdclip

def atrous(image, filtvec, lpf, j, std, kern):
  ff=filtvec[:]
  for i in range(1,len(filtvec)):
    ff[1+(2**(j-1))*(i-1):1+(2**(j-1))*(i-1)]=[0]*(2**(j-1)-1)
  kern=N.outer(ff,ff) 
  im_new=S.fftconvolve(image,kern,mode='same')
  return im_new
 

def a_trous_transform(fitsdir, fitsfilename, jmax, filter):
    """ Makes an a-trous transform of the image in fitsdir/fitsfilename and writes them
    out as FITS files. jmax is the maximum order of decomposition; w1-w_jmax and the
    d.c. image cJ are written out. filter is either 'tr' or 'b3' for triangle or
    B3 spline.""" 

    print ' Running a-trous transform of ' + fitsfilename

    fitsfile = pyfits.open(fitsdir+fitsfilename)
    hdr = fitsfile[0].header
    image = fitsfile[0].data
    image = image[0]
    image = N.array(image.transpose(), order='C',dtype=image.dtype.newbyteorder('='))
    fitsfile.close()
    if fitsfilename[-5:] in ['.fits', '.FITS']:
        filename=fitsfilename[0:-5]
    else:
        filename=fitsfilename


    if jmax < 1:
     jmax=1
     print 'Making Jmax = ',jmax
    elif jmax > 10:
     jmax=10
     print 'Making Jmax = ',jmax

    lpf=' '
    if lpf not in ['tr', 'b3']:
     lpf='b3'

    filter = {'tr':{'size':3,'vec':[1./4,1./2,1./4]}, 
          'b3':{'size':5, 'vec':[1./16,1./4,3./8,1./4,1./16]}}

    kern=[0]
    imm=image
    im_old=image
    for j in range(1,jmax+1):
      print ' Wavelet transform for scale ',j
      std=sigclip(im_old,4)
      im_new=atrous(im_old,filter[lpf]['vec'],lpf,j,std, kern)
      w=im_old-im_new
      im_old=im_new
      pyfits.writeto(fitsdir+filename+'.w'+`j`+'.fits',N.array(w.transpose(), \
   		     dtype=image.dtype.newbyteorder('=')) ,hdr, clobber=True)

    pyfits.writeto(fitsdir+filename+'.cJ.fits',N.array(im_new.transpose(), 
        	       dtype=image.dtype.newbyteorder('=')) ,hdr, clobber=True)

    print 
    print ' Written out ' + fitsdir + filename + '.w1 ... w' + `j` + ', cJ.fits'


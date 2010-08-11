from __future__ import with_statement
import _anaamika as A
import numpy as N
import os
import random

scratch = "/data/anaamika/image/"
fitsdir = "/data/anaamika/fits/"
plotdir = "/data/anaamika/plots/"
srldir = "/data/anaamika/image/"
runcode = "mv"
seed=random.randint(-10000,-1)

writearray_bin2d = A.writearray_bin2d
readarraysize = A.readarraysize
readarray_bin = A.readarray_bin

class arr2fname2:
	def __init__(self, f1, scratch=scratch, runcode='runcode'):
		self.scratch = scratch
		if isinstance(f1, N.ndarray):
			t = my_tmpname()
       			writearray_bin2d(f1,t,runcode)
			self.fname = t
			self.delete = True
		elif isinstance(f1, str):
			self.fname = f1
			self.delete = False
		else:
			raise RuntimeError, "Crap passed in"
	def __enter__(self):
		return self.fname
	def __exit__(self, *args):
		if self.delete:
			os.remove(self.scratch + self.fname + '.img')

def my_tmpname():
    """Generate random file name"""
    import tempfile
    t = tempfile.NamedTemporaryFile().name
    return t.rpartition("/")[2]

def readarray(f1, scratch=scratch):
    n,m,l=A.readarraysize(f1,'.img')
    arr=A.readarray_bin(n,m,n,m,f1,'.img')
    return arr

def ac(f1,f2):    
    "Auto-correlation function of an image. \n\
  f1=input file name or the numpy array. \n\
  f2=output file name \n"
    A.acf(f1,f2)

def ai(of,scratch=scratch,runcode=runcode):    
    "Adds multiple images. \n  of=name of output file. \n\n"
    A.addimages(of,scratch,runcode)

def mn(f1,f2,f3,f4):    
    "Makes mean, rms and median maps. \n  f1=name of input file or the numpy array. \n\
  f2=name of output mean file. \n  f3=name of output rms file. \n\
  f4=name of output median file. \n\n"
    with arr2fname2(f1, scratch) as fname:
        A.boxnoiseimage(fname,f2,f3,f4)

def gc(fitsname,fitsdir=fitsdir,scratch=scratch,runcode=runcode):    
    "Gets coordinates of pixels. \n\
  fitsname=name of fits file. \n"
    A.call_getcoords(fitsname,fitsdir,scratch,runcode)

def ft(f1,scratch=scratch,runcode=runcode):    
    "Computes fft of an image. \n\
  f1=input file name or the numpy array. \n"
    with arr2fname2(f1, scratch) as fname:
    	A.callfft(fname,scratch,runcode)

def fg(f1):    
    "Fits a single gaussian to the image \n\
  f1=input file name or the numpy array. \n"
    with arr2fname2(f1, scratch) as fname:
    	A.callfitgaussian(fname)

def gp(f1,code):    
    "Gets value at given pixel \n\
  f1=input file name or the numpy array. \n"
    with arr2fname2(f1, scratch) as fname:
    	A.callgetpixel(fname,code)

def mo(f1):    
    "Calculates first three moments of an image \n\
  f1=input file name or the numpy array. \n"
    with arr2fname2(f1, scratch) as fname:
    	A.callmoment(fname)

def at(f1,scratch=scratch,runcode=runcode,fitsdir=fitsdir):    
    "Calculates a trous wavelet transform images \n\
  f1=input file name or the numpy array. \n"
    with arr2fname2(f1, scratch) as fname:
    	A.callmwt(fname,scratch,runcode,fitsdir)

def pp(f1,f2,code):    
    "Changes values of pixels \n\
  f1=input file name or the numpy array. \n\
  f2=output file name or the numpy array. \n"
    with arr2fname2(f1, scratch) as fname:
	A.callputpixel(fname,f2,code)

def rf(fitsname,fitsdir=fitsdir,scratch=scratch,runcode=runcode):
    "Reads in a FITS file. \n\
  fitsname=full name of input FITS file \n"
    if fitsname[len(fitsname)-5:len(fitsname)] in ['.fits', '.FITS']:
	    filename=fitsname[0:len(fitsname)-5]
    else:
            filename=fitsname
    A.callreadfits(fitsname,filename,filename,fitsdir,scratch,runcode)

def rb(f1,f2,scratch=scratch,runcode=runcode):    
    "Changes pixels of a particular value to another. \n\
  f1=input file name or the numpy array. \n\
  f2=output file name \n."
    with arr2fname2(f1, scratch) as fname:
    	A.callremag(fname,f2,scratch,runcode)

def ta(seed=seed,scratch=scratch,srldir=srldir):
    "Test sub-menu. \n"
    A.calltest(seed,scratch,srldir)

def cl(f1,f2):    
    "Clips an image in/outside levels. \n\
  f1=input file name or the numpy array. \n\
  f2=output file name \n"
    with arr2fname2(f1, scratch) as fname:
    	A.clip(fname,f2)

def ci(f1,f2,f3,str1):    
    "Combine two images \n\
  f1=first input file name or the numpy array. \n\
  f2=second input file name or the numpy array. \n\
  f3=output file name  \n\
  str1=One of add/sub/mul/div \n"
    if str1 in ['add','sub','mul','div']:
      with arr2fname2(f1, scratch) as fname1:
    	with arr2fname2(f2, scratch) as fname2:
      	  A.combineimage(fname1,fname2,f3,str1)

def cg(f1,f2,filter,scratch=scratch,srldir=srldir):    
    "Convolves an image with gaussian or box \n\
  f1=input file name or the numpy array. \n\
  f2=output file name \n\
  filter='gaus' or 'boxf' \n"
    if filter in ['gaus','boxf']:
      with arr2fname2(f1, scratch) as fname:
      	A.convolveimage(fname,f2,filter,scratch,srldir)

def cc(f1,n,m,val):    
    "Creates a constant image \n\
  f1=output file name \n\
  n=size of image in X direction \n\
  m=size of image in Y direction \n\
  val=constant value of image \n"
    A.cr8constantim(f1,n,m,val)

def cm(f1,n,m,seed=seed,scratch=scratch,srldir=srldir):    
    "Creates an image with sinusoidal modulations \n\
  f1=output file name \n\
  n=size of image in X direction \n\
  m=size of image in Y direction \n"
    A.cr8modulationimage(f1,n,m,seed,scratch,srldir)

def cn(f1,n,m,sigmaJy,seed=seed,scratch=scratch,srldir=srldir):    
    "Creates a gaussian noise image \n\
  f1=output file name \n\
  n=size of image in X direction \n\
  m=size of image in Y direction \n\
  sigmaJy=dispersion of image in Jy \n"
    A.cr8noisemap(f1,n,m,sigmaJy,seed,scratch,srldir)

def cs(f1,seed=seed,scratch=scratch,srldir=srldir,fitsdir=fitsdir):    
    "Creates sources \n\
  f1=output file name \n"
    A.cr8sources(f1,seed,scratch,srldir,fitsdir)

def dd(f1,f2):    
    "Detects discontinuities image \n\
  f1=input filename or array. \n  f2=output file name. \n"
    with arr2fname2(f1, scratch) as fname:
         A.detectdiscont(fname,f2)        

def di(f1,scratch=scratch,srldir=srldir):    
    "Displays image \n\
  f1=input file name or the numpy array. \n"
    with arr2fname2(f1, scratch) as fname:
       A.displayim(fname,scratch,srldir)

def dt(f1,f2):    
    "Display two images, and their difference and ratio \n"
    with arr2fname2(f1, scratch) as fname1:
         with arr2fname2(f2, scratch) as fname2:
              A.displaytwoim(fname1,fname2)

def ex(f1,scratch=scratch,srldir=srldir):    
    "Experimental stuff \n\
  f1=input file name or the numpy array. \n"
    with arr2fname2(f1, scratch) as fname:
    	A.experiment(fname,scratch,srldir)

def fs(f1,scratch=scratch):    
    "FFT shifts the 2d-array \n\
  f1=input file name or the numpy array. \n"
    with arr2fname2(f1, scratch) as fname:
    	A.fftshift2(fname,scratch)

def ff(f1,scratch=scratch,fitsdir=fitsdir):    
    "Changes file format. \n\
  f1=file name including extension. "
    with arr2fname2(f1, scratch) as fname:
    	A.fileformat(fname,scratch,fitsdir)

def fn(fn,scratch=scratch):    
    "Plots functions of radius \n\
  f1=input file name or the numpy array. \n"
    with arr2fname2(f1, scratch) as fname:
    	A.fn_radius(fname,scratch)

def st(f1,kappa=4,scratch=scratch):    
    "Calculates image statistics \n\
  f1=input file name or the numpy array. \n\
  kappa=kappa for k-sigma clipped statistics \n"
    with arr2fname2(f1, scratch) as fname:
    	A.imagestats(fname,kappa,scratch)

def ms(f1,n,m,scratch=scratch):    
    "Makes shapes \n\
  f1=output file name \n\
  n=size of image in X direction \n\
  m=size of image in Y direction \n"
    with arr2fname2(f1, scratch) as fname:
    	A.makeshapes(fname,n,m,scratch)

def mf(ft,f1,f2):    
    "Median filters an image \n\
  ft=filter size (odd) \n\
  f1=input file name or the numpy array. \n\
  f2=output file name \n"
    if (ft % 2) == 0:
	ft += 1
    with arr2fname2(f1, scratch) as fname:
    	A.medianfilter(ft,fname,f2)

def mi(f1,f2,f3,scratch=scratch,srldir=srldir):    
    "Modulates an image \n\
  f1=input file name or the numpy array. \n\
  f2=file name or the numpy array of modulation image \n\
  f3=output file name \n"
    with arr2fname2(f1, scratch) as fname1:
      with arr2fname2(f2, scratch) as fname2:
    	A.modulate(fname1,fname2,f3,scratch,srldir)

def nb(f1,scratch=scratch,srldir=srldir):    
    "n-bits an image \n\
  f1=input file name or the numpy array. \n"
    with arr2fname2(f1, scratch) as fname:
    	A.nbit(fname,scratch,srldir)

def sc(f1,f2,nsig):    
    "n-sigma clip an image \n\
  f1=input file name or the numpy array. \n\
  f2=output file name \n"
    with arr2fname2(f1, scratch) as fname:
    	A.nsigmacut(fname,f2,nsig)

def oi(f1,f2):    
    "Do simple operations on an image \n\
  f1=input file name or the numpy array. \n\
  f2=output file name \n"
    with arr2fname2(f1, scratch) as fname:
    	A.operateimage(fname,f2)

def ps(f1,seed=seed,scratch=scratch,srldir=srldir):    
    "Populate an image with sources \n\
  f1=input file name or the numpy array. \n"
    with arr2fname2(f1, scratch) as fname:
    	A.populatesrcs(fname,seed,scratch,srldir)

def pb(f1,f2):    
    "Do primary beam correction \n\
  f1=input file name or the numpy array. \n\
  f2=output file name \n"
    with arr2fname2(f1, scratch) as fname:
    	A.primarybeam(fname,f2)

def sh(scratch=scratch,srldir=srldir,runcode=runcode):    
    "Shapelet sub-menu \n"
    A.shapelets(scratch,srldir,runcode)

def sl(f1):    
    "Plots vertical or horizontal slice plots \n\
  f1=input file name or the numpy array. \n"
    with arr2fname2(f1, scratch) as fname:
    	A.slice(fname)

def sm(scratch=scratch,srldir=srldir,runcode=runcode):    
    "BDSM task sub-menu \n"
    A.sm(scratch,srldir,runcode)

def sf(f1,f2,order):    
    "Computes structure function of an image. \n\
  f1=input file name or the numpy array. \n\
  f2=output file name \n\
  order=order of structure function \n"
    with arr2fname2(f1, scratch) as fname:
    	A.structfn(fname,f2,order)

def su(f1,f2,code):    
    "Cuts out part of an image. \n\
  f1=input file name or the numpy array. \n\
  f2=ouytput file name \n"
    with arr2fname2(f1, scratch) as fname:
    	A.subim(fname,f2,code)

def tr(f1,f2):    
    "Transposes an image. \n\
  f1=input file name or the numpy array. \n\
  f2=output file name \n"
    with arr2fname2(f1, scratch) as fname:
    	A.transposeim(fname,f2)

ac.__doc__ += A.acf.__doc__
ai.__doc__ += A.addimages.__doc__
mn.__doc__ += A.boxnoiseimage.__doc__
gc.__doc__ += A.call_getcoords.__doc__
ft.__doc__ += A.callfft.__doc__
fg.__doc__ += A.callfitgaussian.__doc__
gp.__doc__ += A.callgetpixel.__doc__
mo.__doc__ += A.callmoment.__doc__
at.__doc__ += A.callmwt.__doc__
pp.__doc__ += A.callputpixel.__doc__
rf.__doc__ += A.callreadfits.__doc__
rb.__doc__ += A.callremag.__doc__
ta.__doc__ += A.calltest.__doc__
cl.__doc__ += A.clip.__doc__
ci.__doc__ += A.combineimage.__doc__
cg.__doc__ += A.convolveimage.__doc__
cc.__doc__ += A.cr8constantim.__doc__
cm.__doc__ += A.cr8modulationimage.__doc__
cn.__doc__ += A.cr8noisemap.__doc__
cs.__doc__ += A.cr8sources.__doc__
dd.__doc__ += A.detectdiscont.__doc__
di.__doc__ += A.displayim.__doc__
dt.__doc__ += A.displaytwoim.__doc__
ex.__doc__ += A.experiment.__doc__
fs.__doc__ += A.fftshift2.__doc__
ff.__doc__ += A.fileformat.__doc__
fn.__doc__ += A.fn_radius.__doc__
st.__doc__ += A.imagestats.__doc__
ms.__doc__ += A.makeshapes.__doc__
mf.__doc__ += A.medianfilter.__doc__
mi.__doc__ += A.modulate.__doc__
nb.__doc__ += A.nbit.__doc__
sc.__doc__ += A.nsigmacut.__doc__
oi.__doc__ += A.operateimage.__doc__
ps.__doc__ += A.populatesrcs.__doc__
pb.__doc__ += A.primarybeam.__doc__
sh.__doc__ += A.shapelets.__doc__
sl.__doc__ += A.slice.__doc__
sm.__doc__ += A.sm.__doc__
sf.__doc__ += A.structfn.__doc__
su.__doc__ += A.subim.__doc__
tr.__doc__ += A.transposeim.__doc__
writearray_bin2d.__doc__ += A.writearray_bin2d.__doc__


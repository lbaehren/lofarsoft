
"""
        Compute a-trous wavelet transform of the gaussian residual image. 
        Do source extraction on this if asked.
"""

import numpy as N
from image import *
import mylogger
import pyfits
import pylab as pl
import _cbdsm
from math import log, floor, sqrt
import scipy.signal as S
from const import fwsig

from bdsm.FITS import Op_loadFITS
from bdsm.preprocess import Op_preprocess
from bdsm.rmsimage import Op_rmsimage
from bdsm.islands import Op_islands
from bdsm.gausfit import Op_gausfit
from bdsm.analysis import plotresults
from bdsm.gaul2srl import Op_gaul2srl
from bdsm.make_residimage import Op_make_residimage

jmax = Int(doc="Maximum order of a-trous wavelet decomposition")
lpf = String(doc="Low pass filter used for a-trous wavelet decomposition")
atrous_islands = List(Any(), doc = "")
atrous_gaussians = List(Any(), doc = "")
atrous_sources = List(Any(), doc = "")
n_pyrsrc = Int(0, doc = "Number of pyramidal sources")

class Op_wavelet_atrous(Op):
    """  """
    def __call__(self, img):

        mylog = mylogger.logging.getLogger("PyBDSM."+img.log+"Wavelet   ")
        if img.opts.atrous_do:
          mylog.info("Decomposing gaussian residual image into a-trous wavelets")
          dobdsm = img.opts.atrous_bdsm_do
          filter = {'tr':{'size':3,'vec':[1./4,1./2,1./4], 'name':'Triangle'}, 
                    'b3':{'size':5, 'vec':[1./16,1./4,3./8,1./4,1./16], 'name':'B3 spline'}}

          if dobdsm: wchain, wopts = self.setpara_bdsm(img)

          n, m = img.ch0.shape
          resid = img.resid_gaus
          lpf=img.opts.atrous_lpf
          if lpf not in ['b3', 'tr']: lpf = 'b3'
          jmax=img.opts.atrous_jmax
          l = len(filter[lpf]['vec'])             # 1st 3 is arbit and 2nd 3 is whats expected for a-trous
          if jmax < 1 or jmax > 20:                   # determine jmax 
            jmax = int(floor(log((min(resid.shape)/3.0*3.0-l)/(l-1)+1)/log(2.0)+1.0))
          img.wavelet_lpf = lpf
          img.wavelet_jmax = jmax
          mylog.info("Using "+filter[lpf]['name']+' filter with J_max = '+str(jmax))

          img.atrous_islands = []
          img.atrous_gaussians = []
          img.atrous_sources = []
          img.atrous_opts = []
    
          im_old=resid
          for j in range(1,jmax+1):
            mean, rms, cmean, std = _cbdsm.bstat(im_old, None, img.opts.kappa_clip)
            im_new=self.atrous(im_old,filter[lpf]['vec'],lpf,j,std)
            w=im_old-im_new
            im_old=im_new
            filename = img.imagename + '.atrous.w'+`j`+'.fits'
            pyfits.writeto(filename, w.transpose(), img.header, clobber=True)
            mylog.info('%s %s' % ('Wrote ', img.imagename + '.atrous.w'+`j`+'.fits'))
                                                        # now do bdsm on each wavelet image
            if dobdsm:
              wopts['fits_name'] = filename
              if img.opts.rms_box == None: box = 0
              else: box = img.opts.rms_box[0]
              y1 = (l+(l-1)*(2**(j-1)-1)); bs = max(10*y1, box)
              if bs > min(n,m)/2:
                wopts['rms_map'] = False
                wopts['rms_box'] = img.opts.rms_box # doesnt matter
              else:
                wopts['rms_box'] = (bs, bs/3)
              if j < 3: 
                wopts['ini_gausfit'] = 'default'
              else:
                wopts['ini_gausfit'] = 'nobeam'
              wid = (l+(l-1)*(2**(j-1)-1))/3.0; b1, b2 = img.pixel_beam[0:2]
              b1 = b1*fwsig; b2 = b2*fwsig
              cdelt = [abs(img.header['cdelt1']), abs(img.header['cdelt2'])]
              wopts['beam'] = (sqrt(wid*wid+b1*b1)*cdelt[0], sqrt(wid*wid+b2*b2)*cdelt[1], 0.0)

              #all wrong beams

              wimg = Image(wopts)
              wimg.pixel_beam = (wopts['beam'][0]/fwsig/cdelt[0], wopts['beam'][1]/fwsig/cdelt[1], wopts['beam'][2])
              wimg.log = 'Wavelet.'
              self.FITS_simple(wimg, img, w, '.atrous.w'+`j`)
              for op in wchain:
                op(wimg)
                img.atrous_opts.append(wimg.opts)
                if isinstance(op,Op_islands): img.atrous_islands.append(wimg.islands)
                if isinstance(op,Op_gausfit): img.atrous_gaussians.append(wimg.gaussians)
                if isinstance(op,Op_gaul2srl): img.atrous_sources.append(wimg.source)
              #plotresults(wimg)
              del wimg

          self.morphfilter_pyramid(img)

          pyfits.writeto(img.imagename + '.atrous.cJ.fits',im_new.transpose(), img.header, clobber=True)
          mylog.info('%s %s' % ('Wrote ', img.imagename + '.atrous.cJ.fits'))

#######################################################################################################
    def atrous(self, image, filtvec, lpf, j, std):

        ff=filtvec[:]
        for i in range(1,len(filtvec)):
          ii=1+(2**(j-1))*(i-1)
          ff[ii:ii]=[0]*(2**(j-1)-1)
        kern=N.outer(ff,ff) 
        im_new=S.fftconvolve(image,kern,mode='same')

        return im_new

#######################################################################################################
    def setpara_bdsm(self, img):
        from types import ClassType, TypeType

        chain=[Op_preprocess, Op_rmsimage(), Op_islands(), Op_gausfit(), Op_gaul2srl(), Op_make_residimage()]
        opts={'thresh':'hard'}
        opts['thresh_pix']=5.0
        opts['kappa_clip'] = 3.0
        opts['rms_map'] = True
        opts['thresh_isl'] = 3.0
        opts['minpix_isl'] = 4.0
        opts['takemeanclip'] = False
        opts['savefits_rmsim'] = False
        opts['savefits_meanim'] = False
        opts['savefits_rankim'] = False
        opts['output_fbdsm'] = False
        opts['polarisation_do'] = False

        opts['flag_smallsrc'] = False
        opts['flag_minsnr'] = 0.5
        opts['flag_maxsnr'] = 2.0
        opts['flag_maxsize_isl'] = 2.5
        opts['flag_bordersize'] = 0
        opts['flag_maxsize_bm'] = 15.0
        opts['flag_minsize_bm'] = 0.2

        ops = []
        for op in chain:
          if isinstance(op, (ClassType, TypeType)):
            ops.append(op())
          else:
            ops.append(op)

        return ops, opts

#######################################################################################################
    def FITS_simple(self, wimg, img, w, name):

        wimg.ch0 = w
        wimg.header = img.header
        wimg.filename = img.filename+name
        wimg.imagename = img.imagename+name+'.pybdsm'
        wimg.pix2sky = img.pix2sky; wimg.sky2pix = img.sky2pix; wimg.pix2beam = img.pix2beam
        wimg.beam2pix = img.beam2pix; wimg.pix2coord = img.pix2coord#; wimg.beam = img.beam

#######################################################################################################
    def morphfilter_pyramid(self, img):
        from math import ceil, floor

        jmax = len(img.atrous_islands)
        ind = [i for i, isl in enumerate(img.atrous_islands) if len(isl) > 0]
        ind.reverse()
        lpyr = []
        img.npyrsrc = -1
        if len(ind) > 0 :
          for i in ind:
            isls = img.atrous_islands[i]
            for isl in isls:
              if i != ind[0]:
                status = False; dumr = []
                for pyrsrc in lpyr:
                  belongs = pyrsrc.belongs(img, isl)
                  if belongs: dumr.append(pyrsrc.pyr_id)
                if len(dumr) > 1: 
                  raise RuntimeError("Source in lower wavelet level belongs to more than one higher level.")
                if len(dumr) == 1:
                  dumr = dumr[0]
                  pyrsrc = lpyr[dumr]
                  pyrsrc.add_level(img, i, isl)
                else:
                  pyrsrc = Pyramid_source(img, isl, i)
                  lpyr.append(pyrsrc)
              else: 
                pyrsrc = Pyramid_source(img, isl, i)
                lpyr.append(pyrsrc)
        img.pyrsrcs = lpyr

        pl.figure()
        a = ceil(sqrt(jmax)); b = floor(jmax/a)
        if a*b < jmax: b += 1
        colours = ['r', 'g', 'b', 'c', 'm', 'y', 'k']
        sh = img.ch0.shape
        for pyr in img.pyrsrcs:
          for iisl, isl in enumerate(pyr.islands):
            jj = pyr.jlevels[iisl]
            col = colours[pyr.pyr_id % 7]
            pl.subplot(a,b,jj+1)
            ind = N.where(~isl.mask_active)
            pl.plot(ind[0]+isl.origin[0], ind[1]+isl.origin[1], '.', color=col)
            pl.axis([0.0, sh[0], 0.0, sh[1]])
            pl.title('J = '+str(jj+1))
        pl.savefig(img.imagename+'.pybdsm.atrous.pyramidsrc.png')


#######################################################################################################

class Pyramid_source(object):
        """ Pyramid_source is a source constructed out of multiple wavelet transform images. """

        def __init__(self, img, island, level0):
            img.npyrsrc = img.npyrsrc + 1
            self.pyr_id = img.npyrsrc
            self.islands = [island]
            self.jlevels = [level0]

        def belongs(self, img, isl):
            import functions as func
                                                # get centroid of island (as integer)
            mom = func.momanalmask_gaus(isl.image, isl.mask_active, 0, 1.0, False)
            cen = N.array(mom[1:3]) + isl.origin
            icen = (int(round(cen[0])), int(round(cen[1])))
            belong = False
                                                # check if lies within any island of self
            for i, pyrisl in enumerate(self.islands):
              if N.sum([pyrisl.bbox[j].start <= cen[j] < pyrisl.bbox[j].stop for j in range(2)])==2:
                pix = tuple([cen[j] - pyrisl.origin[j] for j in range(2)])
                if not pyrisl.mask_active[pix]: 
                  belong = True
            return belong

        def add_level(self, img, level, isl):
            self.islands.append(isl)
            self.jlevels.append(level)


Image.pyrsrcs = List(tInstance(Pyramid_source), doc="List of Pyramidal sources")

    
    

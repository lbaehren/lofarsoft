
"""
        Compute a-trous wavelet transform of the gaussian residual image. 
        Do source extraction on this if asked.
"""

import numpy as N
from image import *
import mylogger
import os
import matplotlib.pyplot as pl
import _cbdsm
from math import log, floor, sqrt
import scipy.signal as S
from const import fwsig
from copy import deepcopy as cp
import functions as func

from readimage import Op_readimage
from preprocess import Op_preprocess
from rmsimage import Op_rmsimage
from threshold import Op_threshold
from islands import Op_islands
from gausfit import Op_gausfit
from gaul2srl import Op_gaul2srl
from make_residimage import Op_make_residimage
from output import Op_outlist
from interface import raw_input_no_history

jmax = Int(doc="Maximum order of a-trous wavelet decomposition")
lpf = String(doc="Low pass filter used for a-trous wavelet decomposition")
atrous_islands = List(Any(), doc = "")
atrous_gaussians = List(Any(), doc = "")
atrous_sources = List(Any(), doc = "")
n_pyrsrc = Int(0, doc = "Number of pyramidal sources")
Image.resid_wavelets = NArray(doc="Residual image calculated from " \
                                "gaussians fitted to wavelet sources")

class Op_wavelet_atrous(Op):
    """  """
    def __call__(self, img):

        mylog = mylogger.logging.getLogger("PyBDSM."+img.log+"Wavelet")
        if img.opts.atrous_do:
          mylog.info("Decomposing gaussian residual image into a-trous wavelets")
          bdir = img.basedir + '/wavelet/'
          if img.opts.output_all:
              os.mkdir(bdir)
              os.mkdir(bdir+'/residuals/')
              os.mkdir(bdir+'/models/')
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
          if jmax < 1 or jmax > 15:                   # determine jmax 
            # If opts.atrous_orig_isl is True, check if largest island size is 
            # smaller than 1/3 of image size. If so, use it to determine jmax.
            if img.opts.atrous_orig_isl:
                max_isl_shape = (0, 0)
                for isl in img.islands:
                    if isl.image.shape[0]*isl.image.shape[1] > max_isl_shape[0]*max_isl_shape[1]:
                        max_isl_shape = isl.image.shape
                if max_isl_shape != (0, 0) and min(max_isl_shape) < min(resid.shape)/3.0:
                    min_size = min(max_isl_shape)
                else:
                    min_size = min(resid.shape)
            else:
                min_size = min(resid.shape)
            jmax = int(floor(log((min_size/3.0*3.0-l)/(l-1)+1)/log(2.0)+1.0))+1
            if min_size*0.55 <= (l+(l-1)*(2**(jmax)-1)): jmax = jmax-1
          img.wavelet_lpf = lpf
          img.wavelet_jmax = jmax
          mylog.info("Using "+filter[lpf]['name']+' filter with J_max = '+str(jmax))

          img.atrous_islands = []
          img.atrous_gaussians = []
          img.atrous_sources = []
          img.atrous_opts = []
          img.resid_wavelets = cp(img.resid_gaus)
    
          im_old=resid
          total_flux = 0.0
          stop_wav = False
          pix_masked = N.where(N.isnan(resid)  == True)
          for j in range(1,jmax+1):#+1):  # extra +1 is so we can do bdsm on cJ as well
            mylogger.userinfo(mylog, "\nWavelet scale #" +str(j))
            mean, rms, cmean, std, cnt = _cbdsm.bstat(im_old, N.isnan(im_old), img.opts.kappa_clip) # why do i have this here ?
            if cnt > 198: cmean = mean; crms = rms
            im_new=self.atrous(im_old,filter[lpf]['vec'],lpf,j)
            im_new[pix_masked] = N.nan  # since fftconvolve wont work with blanked pixels
            w=im_old-im_new
            im_old=im_new
            suffix = 'w'+`j`
            filename = img.imagename + '.atrous.'+suffix+'.fits'
            if img.opts.output_all:
                func.write_image_to_file(img.use_io, filename, w.transpose(), img, bdir)
                mylog.info('%s %s' % ('Wrote ', img.imagename + '.atrous.'+suffix+'.fits'))
                                                        # now do bdsm on each wavelet image
            if dobdsm:
              wopts['filename'] = filename
              wopts['basedir'] = bdir
              if img.opts.rms_box == None: box = 0
              else: box = img.opts.rms_box[0]
              y1 = (l+(l-1)*(2**(j-1)-1)); bs = max(5*y1, box)  # changed from 10 to 5
              if bs > min(n,m)/2:
                wopts['rms_map'] = False
                wopts['rms_box'] = img.opts.rms_box # doesnt matter
              else:
                wopts['rms_box'] = (bs, bs/3)
              if not img.opts.rms_map: wopts['rms_map'] = False
              if not wopts['rms_map']: wopts['mean_map'] = 'const'
              if j <= 3: 
                wopts['ini_gausfit'] = img.opts.ini_gausfit
              else:
                wopts['ini_gausfit'] = 'nobeam'
              wid = (l+(l-1)*(2**(j-1)-1))/3.0; b1, b2 = img.pixel_beam[0:2]
              b1 = b1*fwsig; b2 = b2*fwsig
              cdelt = img.wcs_obj.acdelt[:2]
              wopts['beam'] = (sqrt(wid*wid+b1*b1)*cdelt[0], sqrt(wid*wid+b2*b2)*cdelt[1], 0.0)

              wimg = Image(wopts)
              wimg.pixel_beam = (wopts['beam'][0]/fwsig/cdelt[0], wopts['beam'][1]/fwsig/cdelt[1], wopts['beam'][2])
              wimg.pixel_beamarea = 1.1331*wimg.pixel_beam[0]*wimg.pixel_beam[1]*fwsig*fwsig
              wimg.orig_pixel_beam = img.pixel_beam
              wimg.log = 'Wavelet.'
              wimg.basedir = img.basedir
              wimg.extraparams['bbsprefix'] = suffix
              wimg.extraparams['bbsname'] = img.imagename+'.wavelet'
              wimg.extraparams['bbsappend'] = True
              wimg.bbspatchnum = img.bbspatchnum
              wimg.waveletimage = True
              wimg.use_wcs = img.use_wcs
              wimg.j = j             
              self.FITS_simple(wimg, img, w, '.atrous.'+suffix)
              for op in wchain:
                op(wimg)
                img.atrous_opts.append(wimg.opts)
                if isinstance(op,Op_islands): img.atrous_islands.append(wimg.islands)
                if isinstance(op,Op_gausfit): img.atrous_gaussians.append(wimg.gaussians)
                if isinstance(op,Op_gaul2srl): img.atrous_sources.append(wimg.sources)
              if hasattr(wimg, 'gaussians'): 
                img.resid_wavelets = self.subtract_wvgaus(img.opts, img.resid_wavelets, wimg.gaussians, wimg.islands)
              img.bbspatchnum = wimg.bbspatchnum
              total_flux += wimg.total_flux_gaus
              if img.opts.interactive and len(wimg.gaussians) > 0:
                  dc = '\033[34;1m'
                  nc = '\033[0m'
                  print dc + 'Displaying islands and rms image...' + nc
                  wimg.show_fit()
                  prompt = dc + "Press enter to continue or 'q' stop fitting wavelet images : " + nc
                  answ = raw_input_no_history(prompt)
                  while answ != '':
                      if answ == 'q':
                          img.wavelet_jmax = j
                          stop_wav = True
                          break
                      answ = raw_input_no_history(prompt)
              del wimg
              if stop_wav == True:
                  break

          pdir = img.basedir + '/misc/'
          self.morphfilter_pyramid(img, pdir)
          img.total_flux_gaus += total_flux
          mylogger.userinfo(mylog, "Total flux in model over all scales" , '%.3f Jy' % img.total_flux_gaus)
          if img.opts.output_all:
              func.write_image_to_file(img.use_io, img.imagename + '.atrous.cJ.fits',
                                       im_new.transpose(), img, bdir)
              mylog.info('%s %s' % ('Wrote ', img.imagename + '.atrous.cJ.fits'))
              func.write_image_to_file(img.use_io, img.imagename + '.resid_wavelets.fits',
                                       N.transpose(img.resid_wavelets),  img, bdir)
              mylog.info('%s %s' % ('Wrote ', img.imagename+'.resid_wavelets.fits'))
              func.write_image_to_file(img.use_io, img.imagename + '.model_wavelets.fits',
                                       N.transpose(img.resid_gaus-img.resid_wavelets), img, bdir)
              mylog.info('%s %s' % ('Wrote ', img.imagename+'.model_wavelets.fits'))
              func.write_image_to_file(img.use_io, img.imagename + '.model_all.fits',
                                       N.transpose(img.ch0-img.resid_wavelets), img, bdir)
              mylog.info('%s %s' % ('Wrote ', img.imagename+'.model_all.fits'))


#######################################################################################################
    def atrous(self, image, filtvec, lpf, j):

        ff=filtvec[:]
        for i in range(1,len(filtvec)):
          ii=1+(2**(j-1))*(i-1)
          ff[ii:ii]=[0]*(2**(j-1)-1)
        kern=N.outer(ff,ff) 
        unmasked = N.nan_to_num(image)
        im_new=S.fftconvolve(unmasked,kern,mode='same')
        if im_new.shape != image.shape:
            im_new = im_new[0:image.shape[0], 0:image.shape[1]]

        return im_new

#######################################################################################################
    def setpara_bdsm(self, img):
        from types import ClassType, TypeType

        chain=[Op_preprocess, Op_rmsimage(), Op_threshold(), Op_islands(),
               Op_gausfit(), Op_gaul2srl, Op_make_residimage()]

        opts={'thresh':'hard'}
        opts['thresh_pix'] = 5.0
        opts['kappa_clip'] = 3.0
        opts['rms_map'] = True
        opts['thresh_isl'] = 3.0
        opts['minpix_isl'] = 4.0
        opts['takemeanclip'] = False
        opts['savefits_rmsim'] = False
        opts['savefits_meanim'] = False
        opts['savefits_rankim'] = False
        opts['savefits_normim'] = False
        opts['output_fbdsm'] = False
        opts['polarisation_do'] = False
        opts['group_by_isl'] = img.opts.group_by_isl
        opts['quiet'] = img.opts.quiet

        opts['flag_smallsrc'] = False
        opts['flag_minsnr'] = 0.5
        opts['flag_maxsnr'] = 1.0
        opts['flag_maxsize_isl'] = 2.5
        opts['flag_bordersize'] = 2
        opts['flag_maxsize_bm'] = 15.0
        opts['flag_minsize_bm'] = 0.2
        opts['bbs_patches'] = img.opts.bbs_patches
        opts['filename'] = ''
        opts['output_all'] = img.opts.output_all
        opts['verbose_fitting'] = img.opts.verbose_fitting
        opts['split_isl'] = True
        opts['peak_fit'] = True
        opts['peak_maxsize'] = 30.0

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
        wimg.wcs_obj = img.wcs_obj
        wimg.parentname = img.filename
        wimg.filename = img.filename+name
        wimg.imagename = img.imagename+name+'.pybdsm'
        wimg.pix2sky = img.pix2sky; wimg.sky2pix = img.sky2pix; wimg.pix2beam = img.pix2beam
        wimg.beam2pix = img.beam2pix; wimg.pix2coord = img.pix2coord; wimg.beam = img.beam
        # Mask regions that are outside of islands in the original image.
        # This prevents fitting of Gaussians to wavelet images in these 
        # regions.
        if img.opts.atrous_orig_isl:
            mask = N.ones(img.ch0.shape, dtype=bool)
            for isl in img.islands:
                mask[isl.bbox] = isl.mask_noisy # This will be new mask for island finding and fitting
            if img.mask != None:
                mask[img.mask] = True # Make sure we pick up all masked pixels in img.mask as well
            wimg.rms_mask = img.mask # use original img.mask only for rms/mean map calculation
        else:
            mask = img.mask
        wimg.masked = True
        wimg.mask = mask
        wimg.use_io = img.use_io

#######################################################################################################
    def subtract_wvgaus(self, opts, residim, gaussians, islands):
        import functions as func
        from make_residimage import Op_make_residimage as opp

        dummy = opp()
        shape = residim.shape
        thresh= opts.fittedimage_clip

        for g in gaussians:#():
          C1, C2 = g.centre_pix
          isl = islands[g.island_id]
          b = opp.find_bbox(dummy, thresh*isl.rms, g)
          bbox = N.s_[max(0, int(C1-b)):min(shape[0], int(C1+b+1)),
                      max(0, int(C2-b)):min(shape[1], int(C2+b+1))]
          x_ax, y_ax = N.mgrid[bbox]
          ffimg = func.gaussian_fcn(g, x_ax, y_ax)
          residim[bbox] = residim[bbox] - ffimg

        return residim

#######################################################################################################
    def morphfilter_pyramid(self, img, bdir):
        from math import ceil, floor

        jmax = img.wavelet_jmax
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
                #if len(dumr) > 1: 
                #        raise RuntimeError("Source in lower wavelet level belongs to more than one higher level.")
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

        if img.opts.plot_pyramid:
            pl.figure()
            a = ceil(sqrt(jmax)); b = floor(jmax/a)
            if a*b < jmax: b += 1
            colours = ['r', 'g', 'b', 'c', 'm', 'y', 'k']
            sh = img.ch0.shape
            for pyr in img.pyrsrcs:
              for iisl, isl in enumerate(pyr.islands):
                jj = pyr.jlevels[iisl]
                col = colours[pyr.pyr_id % 7]
                pl.subplot(a,b,jj)
                ind = N.where(~isl.mask_active)
                pl.plot(ind[0]+isl.origin[0], ind[1]+isl.origin[1], '.', color=col)
                pl.axis([0.0, sh[0], 0.0, sh[1]])
                pl.title('J = '+str(jj))
            pl.savefig(bdir+img.imagename+'.pybdsm.atrous.pyramidsrc.png')
        
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
            self.jlevels.append(level + 1)


Image.pyrsrcs = List(tInstance(Pyramid_source), doc="List of Pyramidal sources")

    
    

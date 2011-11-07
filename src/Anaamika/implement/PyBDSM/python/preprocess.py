"""Module preprocess

Calculates some basic statistics of the image and sets up processing
parameters for PyBDSM.
"""

import numpy as N
import _cbdsm
from image import *
from math import pi, sqrt, log
import const
import functions as func
import mylogger

### Insert attributes into Image class
Image.raw_mean = Float(doc="Unclipped image mean")
Image.raw_rms  = Float(doc="Unclipped image rms")
Image.clipped_mean = Float(doc="Clipped image mean")
Image.clipped_rms  = Float(doc="Clipped image rms")
Image.clipped_mean_QUV = List(Float(), doc="Clipped image mean for Q, U, V")
Image.clipped_rms_QUV = List(Float(), doc="Clipped image rms for Q, U, V")
Image.blankpix  = Int(doc="Number of blanked pixels")
Image.noutside_univ  = Int(doc="Number of blanked pixels")

Image.maxpix_coord = Tuple(Int(), Int(),
                           doc="Coordinates of maximal pixel in the image")
Image.minpix_coord = Tuple(Int(), Int(),
                           doc="Coordinates of minimal pixel in the image")
Image.max_value = Float(doc="Maximal pixel in the image")
Image.min_value = Float(doc="Minimal pixel in the image")
Image.omega = Float(doc="Solid angle covered by the image")
confused = String(doc = 'confused image or not')

class Op_preprocess(Op):
    """Preprocessing -- calculate some basic statistics and set 
    processing parameters. Should assume that pixels outside the universe
    are blanked in QC ? """

    def __call__(self, img):
        mylog = mylogger.logging.getLogger("PyBDSM."+img.log+"Preprocess")
        if img.opts.polarisation_do:
          pols = ['I', 'Q', 'U', 'V']
          ch0images = [img.ch0, img.ch0_Q, img.ch0_U, img.ch0_V]
        else:
          pols = ['I'] # assume I is always present
          ch0images = [img.ch0]

        mask = img.mask
        opts = img.opts
        kappa = opts.kappa_clip
        for ipol, pol in enumerate(pols):
          image = ch0images[ipol]

          ### basic stats
          mean, rms, cmean, crms, cnt = _cbdsm.bstat(image, mask, kappa)
          if cnt > 198: cmean = mean; crms = rms
          if pol == 'I':
            if func.approx_equal(crms, 0.0, rel=None):
                raise RuntimeError('Clipped rms appears to be zero. Check for regions '\
                                       'with values of 0 and\nblank them (with NaNs) '\
                                       'or use trim_box to exclude them.')
            img.raw_mean    = mean
            img.raw_rms     = rms
            img.clipped_mean= cmean
            img.clipped_rms = crms
            mylog.info('%s %.4f %s %.4f %s ' % ("Raw mean (Stokes I) = ", mean*1000.0, \
                       'mJy and raw rms = ',rms*1000.0, 'mJy'))
            mylog.info('%d %s %.4f %s %d %s %.4f %s ' % (kappa,"sigma clipped mean (Stokes I) = ", cmean*1000.0, \
                       'mJy and ',kappa,'sigma clipped rms = ',crms*1000.0, 'mJy'))
          else:
            img.clipped_mean_QUV.append(cmean)
            img.clipped_rms_QUV.append(crms)
            mylog.info('%d %s %s %s %.4f %s %d %s %.4f %s ' % (kappa,"sigma clipped mean (Stokes ", pol, ") = ", cmean*1000.0, \
                       'mJy and ',kappa,'sigma clipped rms = ',crms*1000.0, 'mJy'))

        image = img.ch0
        # blank pixels; check if pixels are outside the universe
        if opts.check_outsideuniv:
          mylogger.userinfo(mylog, "Determining the pixels outside the universe")
          noutside_univ = self.outside_univ(img)
          img.noutside_univ = noutside_univ
          mylogger.userinfo(mylog, "Number of additional pixels blanked", str(noutside_univ))
        else:
          mylog.info("Not checking to see if there are pixels outside the universe")

        ### max/min pixel value & coordinates
        shape = image.shape[0:2]
        if mask != None:
            img.blankpix = N.sum(mask)
        if img.blankpix == 0:
          max_idx = image.argmax()
          min_idx = image.argmin()
        else:
          max_idx = N.nanargmax(image)
          min_idx = N.nanargmin(image)

        img.maxpix_coord = N.unravel_index(max_idx, shape)
        img.minpix_coord = N.unravel_index(min_idx, shape)
        img.max_value    = image.flat[max_idx]
        img.min_value    = image.flat[min_idx]

        ### Solid angle of the image
        cdelt = N.array(img.wcs_obj.acdelt[:2])
        img.omega = N.product(shape)*abs(N.product(cdelt))/(180.*180./pi/pi)

        ### Total flux in ch0 image
        if 'atrous' in img.filename:
            # Don't do this estimate for atrous wavelet images,
            # as it doesn't give the correct flux
            img.ch0_sum_jy = 0
        else:
            gfactor = 2.0 * N.sqrt(2.0 * N.log(2.0))
            pixels_per_beam = 2.0 * N.pi * (img.beam2pix(img.beam)[0]
                                            * img.beam2pix(img.beam)[1])\
                                            / gfactor**2
            im_flux = N.nansum(img.ch0)/pixels_per_beam # Jy
            img.ch0_sum_jy = im_flux
            mylogger.userinfo(mylog, 'Flux from sum of (non-blank) pixels',
                              '%.3f Jy' % (im_flux,))
        
        ### if image seems confused, then take background mean as zero instead
        alpha_sourcecounts = 2.5  # approx diff src count slope. 2.2? 
        if opts.bmpersrc_th is None:
          n = (image >= 5.*crms).sum()
          if n <= 0: 
            n = 1
            mylog.warning('No pixels in image > 5-sigma.')
            mylog.warning('Either clipped rms is wrong or the image is pure gaussian noise?')
            mylog.info('Taking number of pixels above 5-sigma as 1.')
          img.bmpersrc_th = N.product(shape)/((alpha_sourcecounts-1.)*n)
          mylog.info('%s %6.2f' % ('Estimated bmpersrc_th = ', img.bmpersrc_th))
        else:
          img.bmpersrc_th = opts.bmpersrc_th
          mylog.info('%s %6.2f' % ('Taking default bmpersrc_th = ', img.bmpersrc_th))

        confused = False
        if opts.mean_map == 'default':
          if opts.bmpersrc_th <= 25. or cmean/crms >= 0.1:
            confused = True
        img.confused = confused
        mylog.info('Parameter confused is '+str(img.confused))
          

        ### box size for rms/mean map calculations
        fwsig = const.fwsig
#        if opts.rms_map in [True, None] or opts.mean_map not in ['zero', 'const']:
        if opts.rms_box is None:
            # 'size' of brightest source
            kappa1 = 3.0
            brightsize = int(round(2.*img.beam[0]/cdelt[0]/fwsig* 
                                   sqrt(2.*log(img.max_value/(kappa1*crms)))))
            # atleast 4 boxes on each side
            largesize = int(round(min(shape)/4.))
            intersrcsep = int(round(sqrt(img.bmpersrc_th)*2.* \
                                        img.beam[0]/cdelt[0]))

            bsize = int(round(sqrt(brightsize*largesize*1.)))
            if intersrcsep > brightsize and intersrcsep < largesize:
                bsize = int(round(sqrt(intersrcsep*largesize*1.)))
            if bsize < 40:
                bsize = min(40,min(shape))
            bsize -= 1 # hack cos of WN40305HB, check later
            if bsize % 10 == 0: bsize += 1

            bstep = int(round(min(bsize/3., min(shape)/10.)))
            img.rms_box = (bsize, bstep)
            if (opts.rms_map is not False) and (opts.mean_map not in ['zero', 'const']):
                mylogger.userinfo(mylog, 'Derived rms_box (box size, step size)',
                                  '(' + str(img.rms_box[0]) + ', ' +
                                  str(img.rms_box[1]) + ') pixels')
        else:
            img.rms_box = opts.rms_box
            if (opts.rms_map is not False) and (opts.mean_map not in ['zero', 'const']):
                mylogger.userinfo(mylog, 'Using user-specified rms_box',
                                  '(' + str(img.rms_box[0]) + ', ' +
                                  str(img.rms_box[1]) + ') pixels')
            

        return img

    def outside_univ(self,img):
        """ Checks if a pixel is outside the universe and is not blanked, 
        and blanks it. (fits files written by CASA dont do this).  """
    
        noutside = 0
        n, m = img.ch0.shape
        for i in range(n):
          for j in range(m):
            out = False
            err = ''
            pix1 = (i,j)
            try:
              skyc = img.pix2sky(pix1)
              pix2 = img.sky2pix(skyc)
              if abs(pix1[0]-pix2[0]) > 0.5 or abs(pix1[1]-pix2[1]) > 0.5: out=True 
            except RuntimeError, err:
              pass
            if out or ("8" in str(err)):  
              noutside += 1
              img.ch0[pix1] = float("NaN")
        return noutside




"""Module collapse

Defines operation Op_collapse which collapses 3D image.

    collapse_mode = Enum('average', 'single', doc="Average channels or take single channel to \
                        perform source detection on")
    collapse_ch0 = Int(0, doc="Number of the channel for source extraction, if collapse_mode='single'")
    collapse_av = List(None, doc="list of channels to average if collapse_mode='average'")
    collapse_wt = Enum('unity', 'rms', doc="Average channels with weights=1 or 1/rms_clip^2 if \
                        collapse_mode='average'")

    Calculates and stores mean and rms (normal and clipped) per channel anyway for further use, even if 
    weights are unity.
"""

import numpy as N
from image import *
import _cbdsm
import pyfits
import mylogger

avspc_wtarr = NArray(doc = "Weight array for channel collapse")
channel_rms = NArray(doc = "RMS per channel")
channel_mean = NArray(doc = "Mean per channel")
channel_clippedrms = NArray(doc = "Clipped RMS per channel")
channel_clippedmean = NArray(doc = "Clipped mean per channel")

class Op_collapse(Op):
    """Collapse 3D image"""

    def __call__(self, img):

      mylog = mylogger.logging.getLogger("PyBDSM."+img.log+"Collapse  ")
      if img.opts.polarisation_do: pols = ['I', 'Q', 'U', 'V'] # make sure I is done first
      else: pols = ['I']                                       # assume I is always present

      if len(img.image.shape) == 3:
        mode = img.opts.collapse_mode
        chan0 = img.opts.collapse_ch0
        c_list = img.opts.collapse_av
        c_wts = img.opts.collapse_wt
        if c_list == []: c_list = N.arange(img.image.shape[0])
        ch0sh = img.image.shape[1:]
        img.ch0 = N.zeros(ch0sh)
        if img.opts.polarisation_do: 
          img.ch0_Q = N.zeros(ch0sh); img.ch0_U = N.zeros(ch0sh); img.ch0_V = N.zeros(ch0sh)
          ch0images = [img.ch0, img.ch0_Q, img.ch0_U, img.ch0_V]
        else:
          ch0images = [img.ch0]

        # assume all Stokes images have the same blank pixels as I:
        blank = N.isnan(img.image)
        hasblanks = blank.any()
        nch = len(c_list)
        kappa = img.opts.kappa_clip

        mean, rms, cmean, crms = chan_stats(img, kappa)
        img.channel_mean = mean; img.channel_rms = rms 
        img.channel_clippedmean = cmean; img.channel_clippedrms = crms

        for ipol, pol in enumerate(pols):
          if pol != 'I':
            # Read in the other Stokes image cubes. Checks that the files exist and
            # that the images have the same shape as I were done in FITS.py. Also,
            # assume (until true 4-D cubes are available) that input filename includes
            # '_I.fits' and that other Stokes cubes are named '*_Q.fits', etc.
            split_fitsname = img.opts.fits_name.split("_I.fits") # replace 'I' with 'Q', 'U', or 'V'
            fits_file = split_fitsname[0]+'_'+pol+'.fits'
            fits = pyfits.open(fits_file, mode="readonly")
            data = fits[0].data
            hdr = fits[0].header
            fits.close()

            ### now we need to transpose coordinates corresponding to RA & DEC
            axes = range(len(data.shape))
            axes[-1], axes[-2] = axes[-2], axes[-1]
            data = data.transpose(*axes)

            ### and make a copy of it to get proper layout & byteorder
            data = N.array(data, order='C',
                           dtype=data.dtype.newbyteorder('='))
                
          if mode == 'single':
            if pol == 'I': 
              img.ch0 = ch0 = img.image[chan0]
              mylog.info('%s %i' % ('Source extraction will be done on channel', chan0))
            else:
                    ch0images[ipol][:] = ch0[:] = data[chan0][:]

          if mode == 'average':
            if not hasblanks:
              if pol == 'I': 
                ch0, wtarr = avspc_direct(c_list, img.image, img.channel_clippedrms, c_wts)
              else:
                # use wtarr from the I image, which is always collapsed first
                ch0, wtarr = avspc_direct(c_list, data, img.channel_clippedrms, c_wts, wtarr=wtarr)
            else:
              if pol == 'I': 
                ch0, wtarr = avspc_blanks(c_list, img.image, img.channel_clippedrms, c_wts)
              else: 
                # use wtarr from the I image, which is always collapsed first
                ch0, wtarr = avspc_blanks(c_list, data, img.channel_clippedrms, c_wts, wtarr=wtarr)

            if pol == 'I':
              img.ch0 = ch0
              img.avspc_wtarr = wtarr
              mylog.info('%s %s' % ('Channels averaged with weights c_wts = ',c_wts))
              mylog.info('Source extraction will be done on averaged ch0 image')
              str1 = " ".join(str(n) for n in c_list)
              mylog.debug('%s %s' % ('Channels averaged : ', str1))
              str1 = " ".join(["%9.4e" % n for n in wtarr])
              mylog.debug('%s %s %s' % ('Channel weights : ', str1, '; unity=zero if c_wts="rms"'))
            ch0images[ipol][:] = ch0[:]

          pyfits.writeto(img.imagename+'.ch0_'+pol+'.fits', N.transpose(ch0), img.header, clobber=True)
          mylog.debug('%s %s ' % ('Writing file ', img.imagename+'.ch0_'+pol+'.fits'))

      else:
        img.ch0 = img.image
        if img.opts.polarisation_do: 
          for pol in pols[1:]:
            split_fitsname = img.opts.fits_name.split("_I.fits") 
            fits_file = split_fitsname[0]+'_'+pol+'.fits'
            fits = pyfits.open(fits_file, mode="readonly")
            data = fits[0].data
            fits.close()
            axes = range(len(data.shape)); axes[-1], axes[-2] = axes[-2], axes[-1]
            data = data.transpose(*axes)
            data = N.array(data, order='C', dtype=data.dtype.newbyteorder('='))
            if pol == 'Q': img.ch0_Q = data
            if pol == 'U': img.ch0_U = data
            if pol == 'V': img.ch0_V = data
      
      # create mask if needed (assume all pols have the same mask as I)
      mask = N.isnan(img.ch0)
      masked = mask.any()
      img.masked = masked
      if masked:
          img.mask = mask
 
########################################################################################

def chan_stats(img, kappa):

    if isinstance(img, Image): # check if img is an Image or just an ndarray
      nchan = img.image.shape[0]
    else:
      nchan = img.shape[0]

    mean = []; rms = []; cmean = []; crms = []
    for ichan in range(nchan):
      if isinstance(img, Image): # check if img is an Image or just an ndarray
        im = img.image[ichan]
      else:
        im = img[ichan]

      if N.any(im):
        immask = N.isnan(im)
        if immask.all():  
          m, r, cm, cr = 0, 0, 0, 0
        else:
          if immask.any():
            m, r, cm, cr = _cbdsm.bstat(im, immask, kappa)
          else:
            m, r, cm, cr = _cbdsm.bstat(im, None, kappa)
      else:
        m, r, cm, cr = 0, 0, 0, 0
      mean.append(m); rms.append(r); cmean.append(cm); crms.append(cr)

    return N.array(mean), N.array(rms), N.array(cmean), N.array(crms) 


########################################################################################

def avspc_direct(c_list, image, rmsarr, c_wts, wtarr=None):

    shape2 = image.shape[1:]
    ch0 = N.zeros(shape2)
    sumwts = 0.0
    if wtarr == None: 
      wtarr = N.zeros(len(c_list))
      for i, ch in enumerate(c_list):
        im = image[ch]
        r = rmsarr[ch]
        if c_wts == 'unity': wt = 1.0
        if c_wts == 'rms': wt = r
        if r != 0:
          wt = 1.0/(wt*wt) 
        else:
          wt = 0
        sumwts += wt
        ch0 += im*wt
        wtarr[i] = wt
    else:
      for i, ch in enumerate(c_list):
        im = image[ch]
        sumwts += wtarr[i]
        ch0 += im*wtarr[i]
    ch0 = ch0/sumwts

    return ch0, wtarr

########################################################################################
    
def avspc_blanks(c_list, image, rmsarr, c_wts, wtarr=None):

    shape2 = image.shape[1:]
    ch0 = N.zeros(shape2)
    sumwtim = N.zeros(shape2)
    if wtarr == None: 
      wtarr = N.zeros(len(c_list))
      for i, ch in enumerate(c_list):
        im = image[ch]
        r = rmsarr[ch]
        if c_wts == 'unity': wt = 1.0
        if c_wts == 'rms': wt = r
        if r != 0:
          wt = 1.0/(wt*wt) 
        else:
          wt = 0
        wtim = N.ones(shape2)*wt*(~N.isnan(im))
        sumwtim += wtim
        ch0 += N.nan_to_num(im)*wtim
        wtarr[i] = wt
    else:
      for i, ch in enumerate(c_list):
        im = image[ch]
        wtim = N.ones(shape2)*wtarr[i]*(~N.isnan(im))
        sumwtim += wtim
        ch0 += N.nan_to_num(im)*wtim
    ch0 = ch0/sumwtim

    return ch0, wtarr

########################################################################################
    
def windowaverage_cube(imagein, imageout, fac, chanrms, iniflags, c_wts, kappa, sbeam, freqin, calcrms_fromim):
    from math import sqrt

    nchan = imagein.shape[0]
    new_n = imageout.shape[0]
    beamlist = []
    avimage_flags = N.zeros(new_n, bool)
    crms_av = N.zeros(new_n)
    freq_av = N.zeros(new_n)
    for ichan in range(new_n):
      if ichan < new_n-1:
        strt = fac*ichan; stp = fac*ichan+fac
      else:
        strt = fac*ichan; stp = nchan
      subim = imagein[strt:stp,:,:]
      rmsarr = chanrms[strt:stp]
      flags = iniflags[strt:stp]
      c_list = N.arange(subim.shape[0])
      c_list = c_list[N.where(~flags)[0]]    # only use unflagged channels
      blank = N.isnan(subim)
      hasblanks = blank.any()
      if not hasblanks: 
          imageout[ichan], dum = avspc_direct(c_list, subim, rmsarr, c_wts)
      else:
          imageout[ichan], dum = avspc_blanks(c_list, subim, rmsarr, c_wts)
      beamlist.append(tuple(N.mean(sbeam[strt:stp][c_list], axis=0)))
      dumr = freqin[strt:stp]
      goodfreqs = dumr[N.where(~flags)[0]]
      freq_av[ichan] = N.mean(goodfreqs)
      avimage_flags[ichan] = N.product(flags)
      subnan = N.isnan(imageout[ichan])
      if not N.all(subnan):
        if calcrms_fromim:  # then calculate rms from the averaged image, assumed to be large enuff
          mean, rms, cmean, crms_av[ichan] = _cbdsm.bstat(imageout[ichan], N.isnan(imageout[ichan]), kappa)
        else:  # else just compute theoretical rms
          crms_av[ichan] = 1.0/sqrt(N.sum(1.0/rmsarr[c_list]/rmsarr[c_list]))
      else:
        crms_av[ichan] = 0

    return imageout, beamlist, freq_av, avimage_flags, crms_av 
        


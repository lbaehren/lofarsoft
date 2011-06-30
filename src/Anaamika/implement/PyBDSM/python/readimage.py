"""Module readimage.

Defines operation Op_readimage which loads image from FITS file or uses Pyrap

The current implementation tries to reduce input file to 2D if
possible, as this makes more sence atm. One more important thing
to note -- in it's default configuration pyfits will read data
in non-native format, so we have to convert it before usage.
"""

import numpy as N
from image import *
import mylogger 

Image.imagename = String(doc="Identifier name for output files")
Image.filename = String(doc="Name of input file without FITS extension")
Image.bbspatchnum = Int(doc="To keep track of patch number for bbs file for seperate patches per source")
Image.cfreq = Float(doc="Frequency in the header")
Image.use_io = String(doc="pyfits or pyrap")
Image.j = Int(doc="Wavelet order j, 0 for normal run")

class Op_readimage(Op):
    """Image file loader

    Loads fits file 'opts.fits_name' and configures
    wcslib machinery for it.
    """
    def __call__(self, img):
        import time, os

        has_pyfits = True
        try: import pyfits
        except ImportError: has_pyfits = False
        has_pyrap = True
        try: import pyrap.images as pim
        except ImportError: has_pyrap = False
        if not has_pyrap and not has_pyfits: raise RuntimeError("Neither Pyfits not Pyrap is available.")
        use_p = img.opts.use_pyrap

        use = None
        if use_p == None:
          if has_pyfits: 
            use = 'fits' 
          else: 
            use = 'rap'
        if use_p and has_pyrap: use = 'rap'
        if not use_p and has_pyfits: use = 'fits'
        if use == None: raise RuntimeError('Change the value of use_pyrap. Pyfits is '+has_pyfits+' and Pyrap is '+has_pyrap)
        img.use_io = use
                       
        mylog = mylogger.logging.getLogger("PyBDSM."+img.log+"Readimage ")

        # Open file(s). Check if there are other polarizations available
        # (assume for now that they are all separate fits files with *_I.fits
        # as the fits_name, and *_Q.fits, *_U.fits, and *_V.fits in the same
        # location). If so, make sure they exist, but do not store them (they
        # are loaded later by collapse.py).
        if img.opts.polarisation_do == True: 
            pols=['I','Q','U','V']
        else: 
            pols=['I'] # assume I is always present
        if pols[0] != 'I':
          mylog.error("First entry of pols has to be I")
          raise RuntimeError("First entry of pols has to be I")

        # Check for trailing "/" in filename (happens a lot, since MS images are directories)    ### KEEP
        if img.opts.fits_name[-1] == '/':
            img.opts.fits_name = img.opts.fits_name[:-1]

        for pol in pols:
            if pol == 'I':
                fits_file=img.opts.fits_name
            else:
                split_fitsname=img.opts.fits_name.split("_I.") # replace 'I' with 'Q', 'U', or 'V'
                fits_file=split_fitsname[0]+'_'+pol+split_fitsname[-1]
            if img.opts.indir != None: prefix = img.opts.indir + '/'
            else: prefix = ''
            try:
              if use == 'fits': fits = pyfits.open(prefix+fits_file, mode="readonly")
              if use == 'rap': fits = pim.image(prefix+fits_file)
            except:
              if pol == 'I':
                  mylog.critical("Cannot open file : "+prefix+fits_file+' - not in proper FITS format')
                  raise RuntimeError("Cannot open file : "+prefix+fits_file+' - not in proper FITS format')
              else:
                  img.opts.polarisation_do = False
                  mylog.warning('One or more of Q, U, V images not found. Polarisation module disabled.')
                  break
            mylog.info("Opened "+prefix+fits_file)

            if len(fits) != 1: print "WARNING: only the primary extent will be considered"

            if use == 'fits': 
              data = fits[0].data
              hdr = fits[0].header
              fits.close()
            if use == 'rap':
              data = fits.getdata()
              hdr = fits.info()

            ### try to reduce dimensionality of data
            if pol == 'I':
                mylog.info("Original image size : "+str(data.shape))
            if len(data.shape) >= 4: # 4d and more -- try to drop extra dimensions
                dims = data.shape[0:-3]
                allones = N.equal(dims, 1).all()
                if not allones:
                  raise RuntimeError("Data dimensionality too high, cannot interpret nontrivial dimensions > 4; Shape : "+data.shape)
                else:
                  data.shape = data.shape[-3:]
            if len(data.shape) == 3:
                if data.shape[0] == 1: # cut 3'd dimension if unity
                    data.shape = data.shape[-2:]

            if pol == 'I':
                ### now we need to transpose coordinates corresponding to RA & DEC
                axes = range(len(data.shape))
                axes[-1], axes[-2] = axes[-2], axes[-1]
                data = data.transpose(*axes)

                ### and make a copy of it to get proper layout & byteorder
                data = N.array(data, order='C',
                               dtype=data.dtype.newbyteorder('='))
                mylog.info("Final image size : "+str(data.shape))

                img.image = data
                img.header = hdr
                img.j = 0
            else:
                # Make sure all polarisations have the same shape as I
                if data.data.transpose(*axes).shape != img.image.shape:
                    img.opts.polarisation_do = False
                    mylog.warning('Shape of one or more of Q, U, V images does not match that of I. Polarisation module disabled.')
                    break

        ### initialize wcs conversion routines
        self.init_wcs(img)
        self.init_beam(img)
        self.init_freq(img)

        if img.opts.fits_name[-5:] in ['.fits', '.FITS']:
          fname = img.opts.fits_name[:-5]
          if fname[-2:] in '_I': fname = fname[:-2] # trim off '_I' as well
        else:
          fname = img.opts.fits_name
        if img.opts.indir == None: img.opts.indir = './'
        img.filename = fname
        img.parentname = fname
        img.imagename = fname+'.pybdsm'
        img.bbspatchnum = 0 # stupid, but kya karen

        basedir = './'+fname+'_pybdsm'
        opdir = img.opts.opdir_overwrite
        if opdir not in ['overwrite', 'append']: 
          img.opts.opdir_overwrite = 'append'
          mylog.info('Appending output files in directory '+basedir)
        img.basedir = basedir + '/'
        if img.opts.solnname != None: img.basedir += img.opts.solnname + '__'
        img.basedir += time.strftime("%d%b%Y_%H.%M.%S")

        if os.path.isfile(basedir): os.system("rm -fr "+basedir)
        if not os.path.isdir(basedir): os.mkdir(basedir)
        if opdir == 'overwrite': os.system("rm -fr "+basedir+"/*")
        os.mkdir(img.basedir)

        return img

    def init_wcs(self, img):
        """Initialize wcs pixel <=> sky conversion routines, and
        store them as img.pix2sky & img.sky2pix.

        Thanks to transpose operation done to image earlier we can use
        p2s & s2p transforms directly.
        """
        import wcslib
        from math import pi

        hdr = img.header
        t = wcslib.wcs()
        if img.use_io == 'fits':
          t.crval = (hdr['crval1'], hdr['crval2'])
          t.crpix = (hdr['crpix1'], hdr['crpix2'])
          t.cdelt = (hdr['cdelt1'], hdr['cdelt2'])
          t.acdelt = (abs(hdr['cdelt1']), abs(hdr['cdelt2']))
          t.ctype = (hdr['ctype1'], hdr['ctype2'])
          if hdr.has_key('crota1'): 
            t.crota = (hdr['crota1'], hdr['crota2'])
          if hdr.has_key('cunit1'): 
            t.cunit = (hdr['cunit1'], hdr['cunit2'])

        if img.use_io == 'rap':
          wcs_dict = hdr['coordinates']['direction0']
          coord_dict = {'degree' : 1.0, 'arcsec' : 1.0/3600, 'rad' : 180.0/pi}
          co_conv = [coord_dict[wcs_dict['units'][i]] for i in range(2)]
          n = 2
          t.crval = tuple([wcs_dict.get('crval')[i]*co_conv[i] for i in range(n)])
          t.crpix = tuple([wcs_dict.get('crpix')[i] for i in range(n)])
          t.cdelt = tuple([wcs_dict.get('cdelt')[i]*co_conv[i] for i in range(n)])
          t.acdelt = tuple([abs(wcs_dict.get('cdelt')[i])*co_conv[i] for i in range(n)])
          t.ctype = ('RA---' + wcs_dict.get('projection'), 'DEC--' + wcs_dict.get('projection'))
          if wcs_dict.has_key('crota1'):
            t.crota = tuple([wcs_dict.get('crota')[i] for i in range(n)])
          if wcs_dict.has_key('cunit1'): 
            t.cunit = tuple([wcs_dict.get('cunit')[i] for i in range(n)])

        t.wcsset()

        img.wcs_obj = t
        img.pix2sky = t.p2s
        img.sky2pix = t.s2p

    def init_beam(self, img):
        """Initialize beam parameters, and conversion routines
        to convert beam to/from pixel coordinates"""
        from const import fwsig

        ### FIXME: beam shape conversion should include rotation angle
        hdr = img.header
        cdelt1, cdelt2 = img.wcs_obj.acdelt[0:2]

        ### define beam conversion routines:
        def beam2pix(x):
            """ Converts beam in deg to pixels """
            bmaj, bmin, bpa = x
            s1 = abs(bmaj/cdelt1) 
            s2 = abs(bmin/cdelt2) 
            th = bpa ### FIXME: check conventions (th + 90)
            return (s1, s2, th)

        def pix2coord(pix):
            x, y = pix
            s1 = abs(x*cdelt1) 
            s2 = abs(y*cdelt2) 
            return (s1, s2)

        def pix2beam(x):
            s1, s2, th = x
            bmaj = abs(s1*cdelt1) 
            bmin = abs(s2*cdelt2) 
            bpa  = th ### FIXME: check conventions (th - 90)
            if bmaj < bmin:
                bmaj, bmin = bmin, bmaj
                bpa += 90
            bpa = divmod(bpa, 180)[1] ### bpa lies between 0 and 180
            return (bmaj, bmin, bpa)

        ### Get the beam information from the header
        if img.opts.beam is not None:
            beam = img.opts.beam
        else:
            try:
                beam = (hdr['BMAJ'], hdr['BMIN'], hdr['BPA'])
                img.opts.beam = beam
            except:
                ### try see if AIPS as put the beam in HISTORY as usual
                found=False
                for h in hdr.get_history():
                  if N.all(['BMAJ' in h, 'BMIN' in h, 'BPA' in h, 'CLEAN' in h]): 
                    dum, dum, dum, bmaj, dum, bmin, dum, bpa = h.split()
                    beam = (float(bmaj), float(bmin), float(bpa))
                    img.opts.beam = beam
                    found = True
                if not found: raise RuntimeError("FITS file error: no beam information")

        ### convert beam into pixels and make sure it's asymmetric
        pbeam = beam2pix(beam)
        pbeam = (pbeam[0]/fwsig, pbeam[1]/fwsig, pbeam[2])
        if abs(pbeam[0]/pbeam[1]) < 1.1:
            pbeam = (1.1*pbeam[0], pbeam[1], pbeam[2])
        
        ### and store it
        img.pix2beam = pix2beam
        img.beam2pix = beam2pix
        img.pix2coord = pix2coord
        img.beam = beam
        img.pixel_beam = pbeam
        img.pixel_restbeam = pbeam

    def init_freq(self, img):

        ### Freq in header
        nax = img.header['naxis']
        found  = False 
        if nax > 2:
          for i in range(nax):
            s = str(i+1)
            if img.header['ctype'+s][0:4] == 'FREQ':
              found = True
              crval, cdelt, crpix = img.header['CRVAL'+s], img.header['CDELT'+s], img.header['CRPIX'+s]
              ff = crval+cdelt*(1.-crpix)
        if found: 
          img.cfreq = ff
        else:
          img.cfreq = 50.0e6




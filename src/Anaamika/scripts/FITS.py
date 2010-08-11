"""Module FITS.

Defines operation Op_loadFITS which loads image from FITS file.

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

class Op_loadFITS(Op):
    """FITS file loader

    Loads fits file 'opts.fits_name' and configures
    wcslib machinery for it.
    """
    def __call__(self, img):
        import pyfits

        mylog = mylogger.logging.getLogger("PyBDSM."+img.log+"LoadFits  ")

        #if img.opts.fits_name is None:
        #    raise RuntimeError("FITS file name not specified")

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
          import sys
          sys.exit()

        for pol in pols:
            if pol == 'I':
                fits_file=img.opts.fits_name
            else:
                split_fitsname=img.opts.fits_name.split("_I.fits") # replace 'I' with 'Q', 'U', or 'V'
                fits_file=split_fitsname[0]+'_'+pol+'.fits'
            try:
              fits = pyfits.open(fits_file, mode="readonly")
            except:
              if pol == 'I':
                  mylog.critical("Cannot open file : "+fits_file)
                  raise RuntimeError("Cannot open file : "+fits_file)
              else:
                  img.opts.polarisation_do = False
                  mylog.warning('One or more of Q, U, V images not found. Polarisation module disabled.')
                  break

            mylog.info("Opened "+fits_file)
            #fits.info()

            if len(fits) != 1:
                print "WARNING: only the primary extent will be considered"

            data = fits[0].data
            hdr = fits[0].header
            fits.close()

            ### try to reduce dimensionality of data
            if pol == 'I':
                mylog.info("Original image size : "+str(data.shape))
            if len(data.shape) >= 4: # 4d and more -- try to drop extra dimensions
                dims = data.shape[0:-3]
                allones = N.equal(dims, 1).all()
                if not allones:
                    raise RuntimeError("Data dimensionality too high")
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
            else:
                # Make sure all polarisations have the same shape as I
                if data.data.transpose(*axes).shape != img.image.shape:
                    img.opts.polarisation_do = False
                    mylog.warning('Shape of one or more of Q, U, V images does not match that of I. Polarisation module disabled.')
                    break

        ### initialize wcs conversion routines
        self.init_wcs(img)
        self.init_beam(img)

        if img.opts.fits_name[-5:] in ['.fits', '.FITS']:
          fname = img.opts.fits_name[:-5]
          if fname[-2:] in '_I': fname = fname[:-2] # trim off '_I' as well
        else:
          fname = img.opts.fits_name
        img.filename = fname
        img.imagename = fname+'.pybdsm'

        return img

    def init_wcs(self, img):
        """Initialize wcs pixel <=> sky conversion routines, and
        store them as img.pix2sky & img.sky2pix.

        Thanks to transpose operation done to image earlier we can use
        p2s & s2p transforms directly.
        """
        import wcslib

        hdr = img.header
        t = wcslib.wcs()
        t.crval = (hdr['crval1'], hdr['crval2'])
        t.crpix = (hdr['crpix1'], hdr['crpix2'])
        t.cdelt = (hdr['cdelt1'], hdr['cdelt2'])
        if hdr.has_key('crota1'):
            t.crota = (hdr['crota1'], hdr['crota2'])
        t.ctype = (hdr['ctype1'], hdr['ctype2'])
        if hdr.has_key('cunit1'):
            t.cunit = (hdr['cunit1'], hdr['cunit2'])

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
        cdelt1 = hdr['cdelt1']
        cdelt2 = hdr['cdelt2']

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

        ### determine beam shape
        if img.opts.beam is not None:
            beam = img.opts.beam
        else:
            try:
                beam = (hdr['bmaj'], hdr['bmin'], hdr['bpa'])
            except:
                raise RuntimeError("FITS file error: no beam information")

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




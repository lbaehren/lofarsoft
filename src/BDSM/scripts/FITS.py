"""Module FITS.

Defines operation Op_loadFITS which loads image from FITS file.

The current implementation tries to reduce input file to 2D if
possible, as this makes more sence atm. One more important thing
to note -- in it's default configuration pyfits will read data
in non-native format, so we have to convert it before usage.
"""

import numpy as N
from image import Op

class Op_loadFITS(Op):
    """FITS file loader

    Loads fits file 'opts.fits_name' and configures
    wcslib machinery for it.
    """
    def __call__(self, img):
        import pyfits

        if img.opts.fits_name is None:
            raise RuntimeError("FITS file name not specified")

        ### open file
        fits = pyfits.open(img.opts.fits_name, mode="readonly")
        fits.info()

        if len(fits) != 1:
            print "WARNING: only the primary extent will be considered"

        data = fits[0].data
        hdr = fits[0].header
        fits.close()

        ### try to reduce dimensionality of data
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

        ### now we need to transpose coordinates corresponding to RA & DEC
        axes = range(len(data.shape))
        axes[-1], axes[-2] = axes[-2], axes[-1]
        data = data.transpose(*axes)

        ### and make a copy of it to get proper layout & byteorder
        data = N.array(data, order='C',
                       dtype=data.dtype.newbyteorder('='))

        ### create mask if needed
        mask = N.isnan(data)
        masked = mask.any()

        ### store all to img
        img.image  = data
        img.header = hdr
        img.masked = masked
        if masked:
            img.mask = mask

        ### initialize wcs conversion routines
        self.init_wcs(img)
        self.init_beam(img)

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
        ### FIXME: beam shape conversion should include rotation angle
        hdr = img.header
        cdelt1 = hdr['cdelt1']
        cdelt2 = hdr['cdelt2']

        ### define beam conversion routines:
        def beam2pix(x):
            bmaj, bmin, bpa = x
            s1 = abs(bmaj/cdelt1) / 2.35482
            s2 = abs(bmin/cdelt2) / 2.35482
            th = bpa ### FIXME: check conventions (th + 90)
            return (s1, s2, th)

        def pix2beam(x):
            s1, s2, th = x
            bmaj = abs(s1*cdelt1) * 2.35482
            bmin = abs(s2*cdelt2) * 2.35482
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
        if abs(pbeam[0]/pbeam[1]) < 1.1:
            pbeam = (1.1*pbeam[0], pbeam[1], pbeam[2])
        
        ### and store it
        img.pix2beam = pix2beam
        img.beam2pix = beam2pix
        img.beam = beam
        img.pixel_beam = pbeam

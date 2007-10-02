import pyfits, wcslib
import numpy as N
import numpy.core.ma as M
from Image import Op

class Op_loadFITS(Op):
    """FITS file loader

    Loads fits file 'opts.fits_name' and configures
    wcslib machinery for it.
    """
    
    def __call__(self, img):
        fits = pyfits.open(img.opts.fits_name, mode = "readonly")
        fits.info()
        
        ### now do some verification of the provided image
        hdr = fits[0].header
        if len(fits) != 1:
            print "WARNING: only the primary extent will be considered"
        if hdr['NAXIS'] != 2 and (hdr['NAXIS'] != 3 or hdr['NAXIS3'] != 1):
            raise RuntimeError('Incorrect image format: currently supported are NxM and NxMx1 images')

        img.fits_hdr = fits[0].header
        data    = fits[0].data
        if len(data.shape) == 3: ### cut out extra unity dimension
            data.shape = data.shape[1:]
        ### FIXME: forcibly convert to the native layout & byteorder
        data = N.array(data.transpose(), order='C',
                       dtype=data.dtype.newbyteorder('='))
        
        img.img = M.array(data, copy = False)
        fits.close()

        ### set up conversion routines
        self.init_wcs(img.opts, img.fits_hdr)

        ### set up beam parameters
        self.init_beam(img.opts, img.fits_hdr)
        
        return img

    def init_wcs(self, opts, hdr):
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

        opts.xy2radec = t.p2s
        opts.radec2xy = t.s2p

    def init_beam(self, opts, hdr):
        ### FIXME: beam shape conversion should include rotation angle
        ### first conversion routines:
        def beam2pix(x):
            bmaj, bmin, bpa = x
            s1 = abs(bmaj/hdr['cdelt1']) / 2.35482
            s2 = abs(bmin/hdr['cdelt2']) / 2.35482
            th = bpa ### FIXME: check conventions (th + 90)
            return (s1, s2, th)

        def pix2beam(x):
            s1, s2, th = x
            bmaj = abs(s1 * hdr['cdelt1']) * 2.35482
            bmin = abs(s2 * hdr['cdelt2']) * 2.35482
            bpa  = th ### FIXME: check conventions (th - 90)
            if bmaj < bmin:
                bmaj, bmin = bmin, bmaj
                bpa += 90
            bpa = divmod(bpa, 180)[1] ### bpa lies between 0 and 180
            return (bmaj, bmin, bpa)
        
        if not opts._has_key('beam'):
            if not hdr.has_key('bmaj'):
                raise RuntimeError('FITS header incomplete: no beam information')
            opts.beam = (hdr['bmaj'], hdr['bmin'], hdr['bpa'])

        opts.pix2beam = pix2beam
        ### now convert beam into pixels
        opts.beam = beam2pix(opts.beam)
        ### force asymmetric beam shape
        if abs(opts.beam[0]/opts.beam[1]) < 1.1:
            opts.beam = (opts.beam[0], 1.1*opts.beam[0], opts.beam[2])

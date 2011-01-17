"""Module FITS.

Defines operation Op_loadFITS which loads image from FITS file.

The current implementation tries to reduce input file to 2D if
possible, as this makes more sence atm. One more important thing
to note -- in it's default configuration pyfits will read data
in non-native format, so we have to convert it before usage.

Lastly, wcs and spectal information are stored in img.wcs_obj and
img.freq_pars to remove any FITS-specific calls to the header,
etc. in other modules.
"""

import numpy as N
from image import *
import mylogger 
import pyfits

Image.imagename = String(doc="Identifier name for output files")
Image.cfreq = Float(doc="Frequency in the header")
Image.freq_pars = Tuple((0.0, 0.0, 0.0), doc="Frequency prarmeters from the header: (crval, cdelt, crpix)")

class Op_loadFITS(Op):
    """FITS file loader

    Loads fits file 'opts.fits_name' and configures
    wcslib machinery for it.
    """
    def __call__(self, img):
        mylog = mylogger.logging.getLogger("PyBDSM."+img.log+"LoadFits  ")

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
            if img.opts.quiet == False:
                print "Opened "+fits_file

            if len(fits) != 1:
                mylog.warning("Mutliple extentions found. Only the primary extention will be considered")
                
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
                if img.opts.quiet == False:
                    if len(data.shape) == 3:
                        print "Image size : "+str(data.shape[-2:])+' pixels'
                    else:
                        print "Image size : "+str(data.shape)+' pixels'
                    
                img.image = data
                img.header = hdr
            else:
                # Make sure all polarisations have the same shape as I
                if data.transpose(*axes).shape != img.image.shape:
                    img.opts.polarisation_do = False
                    mylog.warning('Shape of one or more of Q, U, V images does not match that of I. Polarisation module disabled.')
                    break

        ### initialize wcs conversion routines, beam properties, and frequency info
        self.init_wcs(img)
        self.init_beam(img)
        self.init_freq(img)

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

    def init_freq(self, img):
        ### Place frequency info in img
        mylog = mylogger.logging.getLogger("PyBDSM."+img.log+"LoadFits  ")
        if img.opts.frequency != None:
            img.cfreq = img.opts.frequency[0]
            img.freq_pars = (0.0, 0.0, 0.0)
            mylog.info('Using user-specified frequency/frequencies instead of header values (if any).')
        else:
            found  = False
            hdr = img.header
            nax = hdr['naxis']
            if nax > 2:
              for i in range(nax):
                s = str(i+1)
                if hdr['ctype'+s][0:4] == 'FREQ':
                  found = True
                  crval, cdelt, crpix = hdr['CRVAL'+s], hdr['CDELT'+s], hdr['CRPIX'+s]
                  ff = crval+cdelt*(1.-crpix)
            if found: 
              img.cfreq = ff
              img.freq_pars = (crval, cdelt, crpix)
            else:
              img.cfreq = 50.0e6
              img.freq_pars = (0.0, 0.0, 0.0)
              mylog.warning('Could not determine frequency from the header. Using 50 MHz.')

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
        found_in_history = False
        if img.opts.beam is not None:
            beam = img.opts.beam
        else:          
            try:
                beam = (hdr['bmaj'], hdr['bmin'], hdr['bpa'])
            except:
                history = hdr.get_history()
                bmaj = None
                bmin = None
                bpa = None
                for h in history:
                    bmajindx = h.find('BMAJ')
                    bminindx = h.find('BMIN')
                    bpaindx = h.find('BPA')
                    if bmajindx != -1:
                        bmaj_list = h[bmajindx+5:].split(' ')
                        for bmaj_list_element in bmaj_list:
                            try:
                                bmaj = float(bmaj_list_element)
                                break
                            except:
                                pass
                    if bminindx != -1:
                        bmin_list = h[bminindx+5:].split(' ')
                        for bmin_list_element in bmin_list:
                            try:
                                bmin = float(bmin_list_element)
                                break
                            except:
                                pass
                    if bpaindx != -1:
                        bpa_list = h[bpaindx+4:].split(' ')
                        for bpa_list_element in bpa_list:
                            try:
                                bpa = float(bpa_list_element)
                                break
                            except:
                                pass
                    if bmaj != None and bmin != None and bpa != None:
                        if bmaj > 1.0:
                            bmaj = bmaj/3600.0
                            bmin = bmin/3600.0
                        beam = (bmaj, bmin, bpa)
                        found_in_history = True
                        if img.opts.quiet == False:
                            print "BMAJ, BMIN, and/or BPA keywords not found in header"
                            print "  Using beam parameters found in header history"
                        break
                if found_in_history == False:
                    raise RuntimeError("FITS file error: no beam information found in header")

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
        img.opts.beam = beam
        if img.opts.quiet == False:
            print "Beam shape (major, minor, pos angle) : (%4g, %4g, %4g) degrees" % (beam[0], beam[1], beam[2])



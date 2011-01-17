"""Module read_image.

Defines operation Op_readimage which loads image from a file.

The current implementation tries to reduce input file to 2D if
possible, as this makes more sence atm.

Lastly, wcs and spectal information are stored in img.wcs_obj and
img.freq_pars to remove any FITS-specific calls to the header,
etc. in other modules.
"""

import numpy as N
from image import *
import mylogger 
import time
import pyrap.images as pim

Image.imagename = String(doc="Identifier name for output files")
Image.cfreq = Float(doc="Frequency in the header")
Image.freq_pars = Tuple(doc="Frequency prarmeters from the header: (crval, cdelt, crpix)")

class Op_readimage(Op):
    """Image file loader

    Loads fits, CASA, or HDF5 file 'opts.filename' and configures
    wcslib machinery for it.
    """
    def __call__(self, img):
        mylog = mylogger.logging.getLogger("PyBDSM."+img.log+"LoadImage  ")

        # Open file(s) with pyrap, which can read FITS, CASA, and HDF5 files
        # or, if pyrap is unavailable, use pyfits, which can only read FITS files.
        # 
        # Check if there are other polarizations available
        # (assume for now that they are all separate files with *_I.ext
        # as the filename, and *_Q.ext, *_U.ext, and *_V.ext in the same
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
                image_file = img.opts.filename
            else:
                split_imname = img.opts.filename.split("_I.") # replace 'I' with 'Q', 'U', or 'V'
                image_file = split_imname[0] + '_' + pol + split_imname[-1]
            try:
                inputimage = pim.image(image_file)
            except:
              if pol == 'I':
                  mylog.critical("Cannot open file : "+image_file)
                  raise RuntimeError("Cannot open file : "+image_file)
              else:
                  img.opts.polarisation_do = False
                  mylog.warning('One or more of Q, U, V images not found. Polarisation module disabled.')
                  break

            mylog.info("Opened "+image_file)
            print "Opened "+image_file

            #if inputimage.shape()[0] != 1:
            #    mylog.warning("Mutliple extentions found. Only the primary extention will be considered")
                
            data = inputimage.getdata()
            hdr = inputimage.info()

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

                # ### and make a copy of it to get proper layout & byteorder
                data = N.array(data, order='C',
                               dtype=data.dtype.newbyteorder('='))
                mylog.info("Final image size : "+str(data.shape))
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

        ### initialize wcs conversion routines
        self.init_wcs(img)
        self.init_beam(img)
        self.init_freq(img)

        img.imagename = image_file+'.pybdsm'

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
        wcs_dict = hdr['coordinates']['direction0']
        ra_conversion = 1.0 # conversion to degrees for cdelt1 and crval1
        dec_conversion = 1.0 # conversion to degrees for cdelt2 and crval2
        if wcs_dict['units'][0] == 'arcsec':
            ra_conversion = 1.0 / 3600.0
        if wcs_dict['units'][0] == 'rad':
            ra_conversion = 180.0 / N.pi
        if wcs_dict['units'][1] == 'arcsec':
            dec_conversion = 1.0 / 3600.0
        if wcs_dict['units'][1] == 'rad':
            dec_conversion = 180.0 / N.pi                   
        t.crval = (wcs_dict.get('crval')[0] * ra_conversion, wcs_dict.get('crval')[1] * dec_conversion)
        t.crpix = (wcs_dict.get('crpix')[0], wcs_dict.get('crpix')[1])
        t.cdelt = (wcs_dict.get('cdelt')[0] * ra_conversion, wcs_dict.get('cdelt')[1] * dec_conversion)
        if wcs_dict.has_key('crota1'):
            t.crota = (wcs_dict.get('crota1')[0], wcs_dict.get('crota2')[1])
        t.ctype = ('RA---' + wcs_dict.get('projection'), 'DEC--' + wcs_dict.get('projection'))
        if wcs_dict.has_key('cunit1'):
            t.cunit = (wcs_dict.get('cunit')[0], wcs_dict.get('cunit')[1])
        t.wcsset()

        img.wcs_obj = t
        img.pix2sky = t.p2s
        img.sky2pix = t.s2p
        
    def init_freq(self, img):
        ### Place frequency info in img
        mylog = mylogger.logging.getLogger("PyBDSM."+img.log+"LoadImage  ")
        if img.opts.frequency != None:
            img.cfreq = img.opts.frequency[0]
            img.freq_pars = (0.0, 0.0, 0.0)
            mylog.info('Using user-specified frequency/frequencies instead of header values (if any).')
        else:
            found  = False
            hdr = img.header
            if hdr['coordinates'].has_key('spectral2'):
                found = True
                spec_dict = hdr['coordinates']['spectral2']['wcs']
                crval, cdelt, crpix = spec_dict.get('crval'), spec_dict.get('cdelt'), spec_dict.get('crpix')
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
        cdelt1 = img.wcs_obj.cdelt[0]
        cdelt2 = img.wcs_obj.cdelt[1]

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
            iminfo = hdr['imageinfo']
            if iminfo.has_key('restoringbeam'):
                beaminfo = iminfo['restoringbeam']
            else:
                raise RuntimeError("Error: no beam information found in header. Please specify beam.")
            if beaminfo.has_key('major') and beaminfo.has_key('minor') and beaminfo.has_key('major'):
                bmaj = beaminfo['major']['value']
                bmin = beaminfo['minor']['value']
                bpa = beaminfo['positionangle']['value']
                # make sure all values are in degrees
                if beaminfo['major']['unit'] == 'arcsec':
                    bmaj = bmaj / 3600.0
                if beaminfo['minor']['unit'] == 'arcsec':
                    bmin = bmin / 3600.0
                if beaminfo['major']['unit'] == 'rad':
                    bmaj = bmaj * 180.0 / N.pi
                if beaminfo['minor']['unit'] == 'rad':
                    bmin = bmin * 180.0 / N.pi
                beam = (bmaj, bmin, bpa) # all degrees
            else:
                raise RuntimeError("Error: incomplete beam information found in header. Please specify beam.")

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
        print "Beam shape (major, minor, pos angle) : (%4g, %4g, %4g) degrees" % (beam[0], beam[1], beam[2])



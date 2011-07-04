"""Module readimage.

Defines operation Op_readimage which loads image from FITS file or uses Pyrap

The current implementation tries to reduce input file to 2D if
possible, as this makes more sence atm. One more important thing
to note -- in it's default configuration pyfits will read data
in non-native format, so we have to convert it before usage. See
the read_image_from_file in functions.py for details.

Lastly, wcs and spectal information are stored in img.wcs_obj and
img.freq_pars to remove any FITS-specific calls to the header,
etc. in other modules.
"""

import numpy as N
from image import *
from functions import read_image_from_file
import mylogger
import sys

Image.imagename = String(doc="Identifier name for output files")
Image.filename = String(doc="Name of input file without FITS extension")
Image.bbspatchnum = Int(doc="To keep track of patch number for bbs file "\
                            "for seperate patches per source")
Image.cfreq = Float(doc="Frequency in the header")
Image.use_io = String(doc="pyfits or pyrap")
Image.j = Int(doc="Wavelet order j, 0 for normal run")
Image.freq_pars = Tuple((0.0, 0.0, 0.0),
                        doc="Frequency prarmeters from the header: (crval, cdelt, crpix)")
Image.waveletimage = Bool(doc="Whether a wavelet transform image of not")
Image.pixel_beamarea = Float(doc="Beam area in pixel")
Image.equinox = Float(doc='Equinox of input image from header')

class Op_readimage(Op):
    """Image file loader

    Loads fits file 'opts.fits_name' and configures
    wcslib machinery for it.
    """
    def __call__(self, img):
        import time, os
        mylog = mylogger.logging.getLogger("PyBDSM."+img.log+"Readimage")

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
            raise RuntimeError("First entry of pols has to be I")

        # Check for trailing "/" in filename (happens a lot, since MS images are directories)    ### KEEP
        if img.opts.filename == '':
            raise RuntimeError('Image file name not specified.')
        if img.opts.filename[-1] == '/':
            img.opts.filename = img.opts.filename[:-1]

        for pol in pols:
            if pol == 'I':
                image_file=img.opts.filename
            else:
                split_filename=img.opts.filename.split("_I") # replace 'I' with 'Q', 'U', or 'V'
                image_file=split_filename[0]+'_'+pol+split_filename[-1]

            result = read_image_from_file(image_file, img, img.opts.indir)
            if result == None:
                if pol == 'I':
                    raise RuntimeError("Cannot open file " + repr(image_file))
                else:
                    img.opts.polarisation_do = False
                    mylog.warning('Cannot open ' + pol + ' image. '\
                                      'Polarisation module disabled.')
                    break
            else:
                data, hdr = result

            if pol == 'I':
                # Store data and header in img, but only for pol == 'I'
                if len(data.shape) == 3:
                    mylogger.userinfo(mylog, 'Image size',
                                      str(data.shape[-2:])+' pixels')
                    mylogger.userinfo(mylog, 'Number of channels in image',
                                      '%i' % data.shape[0])
                else:
                    mylogger.userinfo(mylog, 'Image size',
                                      str(data.shape)+' pixels')
                    mylogger.userinfo(mylog, 'Number of channels in image',
                                      '1')
                img.image = data
                img.header = hdr
                img.j = 0                    
            else:
                # Make sure all polarisations have the same shape as I
                if data.shape != img.image.shape:
                    img.opts.polarisation_do = False
                    mylog.warning('Shape of one or more of Q, U, V images '\
                                      'does not match that of I. Polarisation '\
                                      'module disabled.')
                    break

        ### initialize wcs conversion routines
        self.init_wcs(img)
        self.init_beam(img)
        self.init_freq(img)
        year, code = self.get_equinox(img)
        img.equinox = year

        # Try to trim common extensions from filename
        root, ext = os.path.splitext(img.opts.filename)
        if ext in ['.fits', '.FITS']:
            fname = root
        elif ext == '.image':
            fname = root
        else:
            fname = img.opts.filename
        if fname[-2:] in '_I':
            fname = fname[:-2] # trim off '_I' as well
        if img.opts.indir == None: img.opts.indir = './'
        img.filename = img.opts.filename
        img.parentname = fname
        img.imagename = fname+'.pybdsm'
        img.bbspatchnum = 0 # stupid, but kya karen
        img.waveletimage = False
        if img.opts.output_all:
            # Set up directory to write output to
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
          if img.opts.trim_box != None:
              xmin, xmax, ymin, ymax = img.trim_box
              t.crpix = (hdr['crpix1']-xmin, hdr['crpix2']-ymin)
          else:
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
        mylog = mylogger.logging.getLogger("PyBDSM.InitBeam")

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
        found = False
        if img.opts.beam is not None:
            beam = img.opts.beam
        else:
            if img.use_io == 'rap':
                iminfo = hdr['imageinfo']
                if iminfo.has_key('restoringbeam'):
                    beaminfo = iminfo['restoringbeam']
                    found = True
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
                    found = True
            if img.use_io == 'fits':
                try:
                    beam = (hdr['BMAJ'], hdr['BMIN'], hdr['BPA'])
                    found = True
                except:
                    ### try see if AIPS as put the beam in HISTORY as usual
                    for h in hdr.get_history():
                      if N.all(['BMAJ' in h, 'BMIN' in h, 'BPA' in h, 'CLEAN' in h]): 
                        dum, dum, dum, bmaj, dum, bmin, dum, bpa = h.split()
                        beam = (float(bmaj), float(bmin), float(bpa))
                        img.opts.beam = beam
                        found = True
            if not found: raise RuntimeError("No beam information found in image header.")

        ### convert beam into pixels and make sure it's asymmetric
        pbeam = beam2pix(beam)
        pbeam = (pbeam[0]/fwsig, pbeam[1]/fwsig, pbeam[2])
        
        ### and store it
        img.pix2beam = pix2beam
        img.beam2pix = beam2pix
        img.pix2coord = pix2coord
        img.beam = beam
        img.pixel_beam = pbeam   # IN SIGMA UNITS
        img.pixel_beamarea = 1.1331*img.pixel_beam[0]*img.pixel_beam[1]*fwsig*fwsig
        img.pixel_restbeam = pbeam
        mylogger.userinfo(mylog, 'Beam shape (major, minor, pos angle)',
                          '(%s, %s, %s) degrees' % (round(beam[0],5),
                                                    round(beam[1],5),
                                                    round(beam[2],1)))

    def init_freq(self, img):
        """Initialize frequency parameters and store them"""
        mylog = mylogger.logging.getLogger("PyBDSM.InitFreq")
        if img.opts.frequency_sp != None and len(data.shape) == 3:
            # If user specifies multiple frequencies, then let
            # collapse.py do the initialization 
            img.cfreq = img.opts.frequency_sp[0]
            img.freq_pars = (0.0, 0.0, 0.0)
            mylog.info('Using user-specified frequency/frequencies.')
        elif img.opts.frequency != None:
            img.cfreq = img.opts.frequency
            img.freq_pars = (0.0, 0.0, 0.0)
            mylog.info('Using user-specified frequency/frequencies.')           
        else:
            found  = False
            hdr = img.header
            if img.use_io == 'rap':
                if hdr['coordinates'].has_key('spectral2'):
                    found = True
                    spec_dict = hdr['coordinates']['spectral2']['wcs']
                    crval, cdelt, crpix = spec_dict.get('crval'), \
                        spec_dict.get('cdelt'), spec_dict.get('crpix')
                    ff = crval+cdelt*(1.-crpix)

            if img.use_io == 'fits':
                nax = hdr['naxis']
                if nax > 2:
                    for i in range(nax):
                        s = str(i+1)
                        if hdr['ctype'+s][0:4] == 'FREQ':
                            found = True
                            crval, cdelt, crpix = hdr['CRVAL'+s], \
                                hdr['CDELT'+s], hdr['CRPIX'+s]
                            ff = crval+cdelt*(1.-crpix)
            if found: 
                img.cfreq = ff
                img.freq_pars = (crval, cdelt, crpix)
            else:
                raise RuntimeError('No frequency information found in image header.')

    def get_equinox(self, img):
        """Gets the equinox from the header.

        Returns float year with code, where code is:
        1 - EQUINOX, EPOCH or RADECSYS keyword not found in header
        0 - EQUINOX found as a numeric value
        1 - EPOCH keyword used for equinox (not recommended)
        2 - EQUINOX found as  'B1950'
        3 - EQUINOX found as  'J2000'
        4 - EQUINOX derived from value of RADECSYS keyword
            'ICRS', 'FK5' ==> 2000,  'FK4' ==> 1950
        """
        code = -1
        year = None
        if img.use_io == 'rap':
            hdr = img.header['coordinates']['direction0']
            code = -1
            year = None
            if hdr.has_key('system'):
                year = hdr['system']
                if isinstance(year, str):     # Check for 'J2000' or 'B1950' values
                    tst = year[:1]
                    if (tst == 'J') or (tst == 'B'):
                        year = float(year[1:])
                        if tst == 'J': code = 3
                        if tst == 'B': code = 2 
                else:
                    code = 0
        if img.use_io == 'fits':
            hdr = img.header
            if hdr.has_key('EQUINOX'):
                year = hdr['EQUINOX']
                if isinstance(year, str):     # Check for 'J2000' or 'B1950' values
                    tst = year[:1]
                    if (tst == 'J') or (tst == 'B'):
                        year = float(year[1:])
                        if tst == 'J': code = 3
                        if tst == 'B': code = 2 
                else:
                    code = 0
            else:
                if hdr.has_key('EPOCH'): # Check EPOCH if EQUINOX not found
                    year = hdr['EPOCH']
                    code = 1
                else:
                    if hdr.has_key('RADECSYS'):
                        sys = hdr['RADECSYS']
                        code = 4 
                        if sys[:3] == 'ICR': year = 2000.0
                        if sys[:3] == 'FK5': year = 2000.0
                        if sys[:3] == 'FK4': year = 1950.0
        return year, code

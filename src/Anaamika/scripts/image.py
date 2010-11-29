"""Module image.

Instancess of class Image are a primary data-holders for all 
PyBDSM operations. They store the image itself together with
some meta-information (such as headers), options for processing
modules and all data generated during processing.

This module also defines class Op, which is used as a base class
for all PyBDSM operations.
"""

import numpy as N
from opts import *

class Image(object):
    """Image is a primary data container for PyBDSM.

    All the run-time data (such as image data, mask, etc.) 
    is stored here. A number of type-checked properties 
    are defined for the most basic image attributes, such
    as image data, mask, header, user options.

    There is little sense in declaring all possible attributes
    right here as it will introduce unneeded dependencies 
    between modules, thus most other attributes (like island lists,
    gaussian lists, etc) are inserted at run-time by the specific
    PyBDSM modules.
    """
    opts   = Instance(Opts, doc="User options")

    image  = NArray(doc="Image data, Stokes I")
    ch0    = NArray(doc="Channel-collapsed image data, Stokes I")
    ch0_Q  = NArray(doc="Channel-collapsed image data, Stokes Q")
    ch0_U  = NArray(doc="Channel-collapsed image data, Stokes U")
    ch0_V  = NArray(doc="Channel-collapsed image data, Stokes V")
    header = Any(doc="Image header")
    mask   = NArray(doc="Image mask (if present and attribute masked is set)")
    masked = Bool(False, doc="Flag if mask is present")


    def __init__(self, opts):
        self.opts = Opts(opts)

    def plotfit(self):
        """Plot ch0 image with Gaussians overlaid and residual image."""
        import plotresults
        plotresults.plotresults(self)

    def write_resid_img(self, filename=None):
        """Write the residual Gaussian image to a fits file."""
        import pyfits
        if filename == None:
            filename = self.imagename + '.resid_gaus.fits'
        pyfits.writeto(filename, N.transpose(self.resid_gaus), self.header, clobber=True)
        print 'Wrote FITS file '+filename+'.'

    def write_model_img(self, filename=None):
        """Write the model Gaussian image to a fits file."""
        import pyfits
        if filename == None:
            filename = self.imagename + '.model_gaus.fits'
        pyfits.writeto(filename, N.transpose(self.model_gaus), self.header, clobber=True)
        print 'Wrote FITS file '+filename+'.'

    def write_rms_img(self, filename=None):
        """Write the rms image to a fits file."""
        import pyfits
        if filename == None:
            filename = self.imagename + '.rms.fits'
        pyfits.writeto(filename, N.transpose(self.rms), self.header, clobber=True)
        print 'Wrote FITS file '+filename+'.'

    def write_srl(self, filename=None, format='fits'):
        """Write the source list to a file.

        Supported formats are:
            "fits"
            "ascii"
            "bbs"
        """
        if (format in ['fits', 'ascii', 'bbs']) == False:
            raise RuntimeError('format must be "fits", "ascii", or "bbs"')       
        if filename == None:
            if format == 'fits':
                filename = self.imagename + '.srl.fits'
            if format == 'ascii':
                filename = self.imagename + '.srl.txt'
            if format == 'bbs':
                filename = self.imagename + '.srl.sky'
        print 'Sorry, this feature is not yet available. Use "write_gaul" instead.'
        

    def write_gaul(self, filename=None, format='fits'):
        """Write the Gaussian list to a file.

        Supported formats are:
            "fits"
            "ascii"
            "bbs"
        """
        if (format in ['fits', 'ascii', 'bbs']) == False:
            raise RuntimeError('format must be "fits", "ascii", or "bbs"')
        if filename == None:
            if format == 'fits':
                filename = self.imagename + '.gaul.fits'
            if format == 'ascii':
                filename = self.imagename + '.gaul.txt'
            if format == 'bbs':
                filename = self.imagename + '.gaul.sky'
        if format == 'fits':
            # Write as FITS binary table.
            import pyfits
            import fbdsm_fitstable_stuff as stuff
            from output import pybdsm2fbdsm
            
            cnames, cunit, cformat = stuff.cnames, stuff.cunit, stuff.cformat
            fbdsm_list = pybdsm2fbdsm(self)
            col_list = []
            for ind, col in enumerate(fbdsm_list):
                list1 = pyfits.Column(name=cnames[ind], format=cformat[ind], unit=cunit[ind], array=fbdsm_list[ind])
                col_list.append(list1)
            tbhdu = pyfits.new_table(col_list)
            tbhdu.writeto(filename, clobber=True)
            print '--> Wrote FITS file '+filename+'.'
            
        if format == 'ascii':
            # Write as ascii file.
            f = open(filename, "w")
            for g in self.gaussians():
                gidx = g.gaus_num-1  # python numbering
                iidx = g.island_id
                A = g.peak_flux
                ra, dec = g.centre_sky
                x, y = g.centre_pix
                shape = g.size_sky
                eA = g.peak_fluxE
                era, edec = g.centre_skyE
                ex, ey = g.centre_pixE
                eshape = g.size_skyE

                str1 = "%3d  %4d  %d    %10g %5g   %10g %5g   " \
                      "%10g %5g   %10g %5g   %10g %5g   %10g %5g   " \
                      "%10g %5g   %10g %5g   %10g %5g\n" % \
                      (gidx, iidx, 0,    0, 0,     A, eA, \
                       ra, era,     dec, edec,    x, ex,  y, ey, \
                       shape[0], eshape[0], shape[1], eshape[1],  shape[2], eshape[2])
                f.write(str1)
            f.close()
            print '--> Wrote ASCII file '+filename+'.'

        if format == 'bbs':
            # Write as a BBS sky model.
            import numpy as N
            from const import fwsig
            from output import ra2hhmmss
            from output import dec2ddmmss
            
            if self.opts.spectralindex_do: 
                freq = "%.5e" % self.freq0
            else:
                freq = "%.5e" % self.cfreq
            f = open(filename, 'w')
            str1 = "# (Name, Type, Ra, Dec, I, Q, U, V, ReferenceFrequency='"+freq+", SpectralIndexDegree='0', " \
                  + "SpectralIndex:0='0.0', MajorAxis, MinorAxis, Orientation) = format\n "
            f.write(str1)
            sep = ', '
            sname = self.imagename.split('.')[0]
            str_src = []
            total_flux = []
            bm_pix = N.array([self.pixel_beam[0]*fwsig, self.pixel_beam[1]*fwsig, self.pixel_beam[2]])
            bm_deg = self.pix2beam(bm_pix)
            for g in self.gaussians():
              src = sname + '_' + str(g.gaus_num)
              ra, dec = g.centre_sky
              ra = ra2hhmmss(ra)
              sra = str(ra[0]).zfill(2)+':'+str(ra[1]).zfill(2)+':'+str("%.3f" % (ra[2])).zfill(6)
              dec= dec2ddmmss(dec)
              decsign = ('-' if dec[3] < 0 else '+')
              sdec = decsign+str(dec[0]).zfill(2)+'.'+str(dec[1]).zfill(2)+'.'+str("%.3f" % (dec[2])).zfill(6)
              total_flux.append(g.total_flux)
              total = str("%.3e" % (g.total_flux))
              pol = '0.0, 0.0, 0.0, '
              deconv = g.deconv_size_sky
              if deconv[0] == 0.: deconv[0] = bm_deg[0]
              if deconv[1] == 0.: deconv[1] = bm_deg[1]
              if deconv[0] <= bm_deg[0] and deconv[1] <= bm_deg[1]:
                  stype = 'POINT'
                  deconv[0] = bm_deg[0]
                  deconv[1] = bm_deg[1]
              else:
                  stype = 'GAUSSIAN'
              deconv1 = str("%.5e" % (deconv[0])) 
              deconv2 = str("%.5e" % (deconv[1])) 
              deconv3 = str("%.5e" % (deconv[2])) 
              deconvstr = deconv1 + ', ' + deconv2 + ', ' + deconv3
              specin = '-0.8'
              if self.opts.spectralindex_do: 
                  specin = g.spin1[1]
                  total = str("%.3e" % (g.spin1[0]))
              str_src.append(src + sep + stype + sep + sra + sep + sdec + sep + total + sep + pol + \
                              freq + sep + '0' + sep + specin + sep + deconvstr + '\n')
            # sort by flux (largest -> smallest)
            flux_indx = range(len(str_src))
            flux_indx.sort(lambda x,y: cmp(total_flux[x],total_flux[y]), reverse=True)
            for i in flux_indx:
              f.write(str_src[i])
            f.close()
            print '--> Wrote BBS sky model '+filename+'.'
                 

class Op(object):
    """Common base class for all PyBDSM operations.

    At the moment this class is empty and only defines placeholder
    for method __call__, which should be redefined in all derived
    classes.
    """
    def __call__(self, img):
        raise NotImplementedError("This method should be redefined")

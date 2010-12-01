"""Module output.

Writes results of source detection in a variety of formats.
If called as during the fits chain (set with the
opts.output_all flag), all of the following are created:
    - BBS list
    - fbdsm gaussian list
    - star list
    - kvis annotations

Also include are the following functions:
  - write_gaul(img, filename=None, format='fits')
      Outputs Gaussian list in these formats:
       - BBS 
       - FITS
       - ASCII
  - write_srl(img, filename=None, format='fits')
      Writes Source list      
  - write_rms_img(img, filename=None)
      Writes rms image to FITS file.      
  - write_model_img(img, filename=None)
      Writes model image to FITS file. 
  - write_resid_img(img, filename=None)
      Writes resid image to FITS file.
"""

from image import Op
import os
import pyfits
import fbdsm_fitstable_stuff as stuff
import numpy as N

class Op_outlist(Op):
    """Write out list of gaussians

    Currently 4 output formats are supported:
    - BBS list
    - fbdsm gaussian list
    - star list
    - kvis annotations

    All output lists are generated atm.
    """
    def __call__(self, img):
        if img.opts.output_all:
            write_bbs(img)
            write_gaul(img)
            write_star(img)
            write_kvis_ann(img)
            write_gaul_FITS(img)
        return img


def write_star(img):
    fname = img.imagename + '.star'
    f = open(fname, 'w')

    for g in img.gaussians():
        A = g.peak_flux
        ra, dec = g.centre_sky
        shape = g.size_sky
        ### convert to canonical representation
        ra = ra2hhmmss(ra)
        dec= dec2ddmmss(dec)
        decsign = ('-' if dec[3] < 0 else '+')

        str = '%2i %2i %6.3f ' \
              '%c%2i %2i %6.3f ' \
              '%9.4f %9.4f %7.2f ' \
              '%2i %13.7f %10s\n' % \
               (ra[0], ra[1], ra[2], 
                decsign, dec[0], dec[1], dec[2],
                shape[0]*3600, shape[1]*3600, shape[2],
                4, A, '')

        f.write(str)
    f.close()

def write_kvis_ann(img):
    fname = img.imagename + '.kvis.ann'
    f = open(fname, 'w')
    f.write("### KVis annotation file\n\n")
    f.write("color green\n\n")

    for g in img.gaussians():
        iidx = g.island_id
        A = g.peak_flux
        ra, dec = g.centre_sky
        shape = g.size_sky
        cross = (3*img.header['cdelt1'],
                 3*img.header['cdelt2'])

        str = 'text   %10.5f %10.5f   %d\n' % \
            (ra, dec, iidx)
        f.write(str)
        str = 'cross   %10.5f %10.5f   %10.7f %10.7f\n' % \
            (ra, dec, abs(cross[0]), abs(cross[1]))
        #f.write(str)
        str = 'ellipse %10.5f %10.5f   %10.7f %10.7f %10.4f\n' % \
            (ra, dec, shape[0], shape[1], shape[2])
        f.write(str)
    f.close()

def write_srl(img, filename=None, format='fits'):
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
            filename = img.imagename + '.srl.fits'
        if format == 'ascii':
            filename = img.imagename + '.srl.txt'
        if format == 'bbs':
            filename = img.imagename + '.srl.sky'
    print 'Sorry, this feature is not yet available. Use "write_gaul" instead.'


def write_gaul(img, filename=None, format='fits'):
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
            filename = img.imagename + '.gaul.fits'
        if format == 'ascii':
            filename = img.imagename + '.gaul.txt'
        if format == 'bbs':
            filename = img.imagename + '.gaul.sky'
    if format == 'fits':
        # Write as FITS binary table.
        import pyfits
        import fbdsm_fitstable_stuff as stuff
        from output import pybdsm2fbdsm

        cnames, cunit, cformat = stuff.cnames, stuff.cunit, stuff.cformat
        fbdsm_list = pybdsm2fbdsm(img)
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
        f.write('#  gaul_id   island_id   flag   Total_Flux   Err_total_flux   Peak_flux  Err_peak_flux  RA   Err_RA    DEC    Err_DEC     Xpos        Err_xpos    Ypos      Err_ypos       Bmaj_fw      Err_bmaj  Bmin_fw   Err_bmin   Bpa     Err_bpa\n')
        for g in img.gaussians():
            gidx = g.gaus_num-1  # python numbering
            iidx = g.island_id
            F = g.flag
            A = g.peak_flux
            T = g.total_flux
            ra, dec = g.centre_sky
            x, y = g.centre_pix
            shape = g.size_sky
            eA = g.peak_fluxE
            eT = g.total_fluxE
            era, edec = g.centre_skyE
            ex, ey = g.centre_pixE
            eshape = g.size_skyE

            str1 = "%4d  %4d  %d    %10f %10f   %10f %10f  " \
                  "%10f %10f   %10f %10f   %10f %10f   %10f %10f   " \
                  "%10f %10f   %10f %10f   %10f %10f\n" % \
                  (gidx, iidx, F,    T, eT,     A, eA, \
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

        if img.opts.spectralindex_do: 
            freq = "%.5e" % img.freq0
        else:
            freq = "%.5e" % img.cfreq
        f = open(filename, 'w')
        str1 = "# (Name, Type, Ra, Dec, I, Q, U, V, ReferenceFrequency='"+freq+", SpectralIndexDegree='0', " \
              + "SpectralIndex:0='0.0', MajorAxis, MinorAxis, Orientation) = format\n "
        f.write(str1)
        sep = ', '
        sname = img.imagename.split('.')[0]
        str_src = []
        total_flux = []
        bm_pix = N.array([img.pixel_beam[0]*fwsig, img.pixel_beam[1]*fwsig, img.pixel_beam[2]])
        bm_deg = img.pix2beam(bm_pix)
        for g in img.gaussians():
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
          if img.opts.spectralindex_do: 
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

def write_resid_img(img, filename=None):
    """Write the residual Gaussian image to a fits file."""
    import pyfits
    if filename == None:
        filename = img.imagename + '.resid_gaus.fits'
    pyfits.writeto(filename, N.transpose(img.resid_gaus), img.header, clobber=True)
    print 'Wrote FITS file '+filename+'.'

def write_model_img(img, filename=None):
    """Write the model Gaussian image to a fits file."""
    import pyfits
    if filename == None:
        filename = img.imagename + '.model_gaus.fits'
    pyfits.writeto(filename, N.transpose(img.model_gaus), img.header, clobber=True)
    print 'Wrote FITS file '+filename+'.'

def write_rms_img(img, filename=None):
    """Write the rms image to a fits file."""
    import pyfits
    if filename == None:
        filename = img.imagename + '.rms.fits'
    pyfits.writeto(filename, N.transpose(img.rms), img.header, clobber=True)
    print 'Wrote FITS file '+filename+'.'
        
def ra2hhmmss(deg):
    """Convert RA coordinate (in degrees) to HH MM SS"""

    from math import modf
    if deg < 0:
        raise RuntimeError("Negative RA")
    x, hh = modf(deg/15.)
    x, mm = modf(x*60)
    ss = x*60

    return (int(hh), int(mm), ss)

def dec2ddmmss(deg):
    """Convert DEC coordinate (in degrees) to DD MM SS"""

    from math import modf
    sign = (-1 if deg < 0 else 1)
    x, dd = modf(abs(deg))
    x, ma = modf(x*60)
    sa = x*60

    return (int(dd), int(ma), sa, sign)

def pybdsm2fbdsm(img):
    import functions as func

    fbdsm = []
    for g in img.gaussians():
        gidx = g.gaus_num
        iidx = g.island_id+1
        F = g.flag
        A = g.peak_flux
        T = g.total_flux
        ra, dec = g.centre_sky
        x, y = g.centre_pix
        shape = g.size_sky
        eA = g.peak_fluxE
        eT = g.total_fluxE
        era, edec = g.centre_skyE
        ex, ey = g.centre_pixE
        eshape = g.size_skyE
        list1 = [gidx, iidx, F, T, eT, A, eA, ra, era, dec, edec, x, ex, y, ey, shape[0], eshape[0], shape[1], eshape[1], \
                 shape[2], eshape[2], 0.,0.,0.,0.,0.,0., 0.,0.,0.,0., 0.,0., iidx, 0,0,0,0, 0.,0.,0.,0.,0.,0.]
        fbdsm.append(list1)
    fbdsm = func.trans_gaul(fbdsm)

    return fbdsm



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

def write_gaul(img, filename=None, format='ascii', srcroot=None):
    """Write the Gaussian list to a file.

    filename - name of resulting file
    srcroot - root for source names
    format - format of output list
    Supported formats are:
        "fits"
        "ascii"
        "bbs"
        "ds9"
    """
    if (format in ['fits', 'ascii', 'bbs', 'ds9']) == False:
        raise RuntimeError('format must be "fits", "ascii", "ds9", or "bbs"')
    if filename == None:
        if format == 'fits':
            filename = img.imagename + '.gaul.fits'
        if format == 'ascii':
            filename = img.imagename + '.gaul.txt'
        if format == 'bbs':
            filename = img.imagename + '.gaul.sky'
        if format == 'ds9':
            filename = img.imagename + '.gaul.reg'
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
        print '--> Wrote FITS file ' + filename
        
    if format == 'ascii':
        # Write as ascii file.
        f = open(filename, "w")
        f.write('# PyBDSM Gaussian list for '+img.filename+'\n')
        f.write('# Refernce frequency : %.3e\n\n' % (img.freq0,))
        if img.opts.spectralindex_do:
            f.write('#  Gaul_id   Island_id   Flag   Total_flux   Err_total_flux   Peak_flux  Err_peak_flux Spec_indx Err_spec_indx  RA   Err_RA    DEC    Err_DEC     Xpos        Err_xpos    Ypos      Err_ypos       Bmaj_fw      Err_bmaj  Bmin_fw   Err_bmin   Bpa     Err_bpa\n')
        else:           
            f.write('#  Gaul_id   Island_id   Flag   Total_flux   Err_total_flux   Peak_flux  Err_peak_flux  RA   Err_RA    DEC    Err_DEC     Xpos        Err_xpos    Ypos      Err_ypos       Bmaj_fw      Err_bmaj  Bmin_fw   Err_bmin   Bpa     Err_bpa\n')
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
            if img.opts.spectralindex_do:
                spin1 = g.spin1
                espin1 = g.espin1
                if spin1 == None:
                    specin = 0.0
                    especin = 0.0
                else:                       
                    specin = spin1[1]
                    especin = espin1[1]
                # srcid = g.source_id
                # src = img.source[srcid]
                # for g_src in src.gaussians:
                #     if g_src.gaus_num == g.gaus_num:
                #         spin1 = g_src.spin1
                #         espin1 = g_src.espin1
                #         if spin1 == None:
                #             specin = 0.0
                #             especin = 0.0
                #         else:                       
                #             specin = spin1[1]
                #             especin = espin1[1]
                    # else:
                    #     specin = 0.0
                    #     especin = 0.0
                str1 = "%4d  %4d  %d    %10f %10f   %10f %10f  %10f %10f " \
                      "%10f %10f   %10f %10f   %10f %10f   %10f %10f   " \
                      "%10f %10f   %10f %10f   %10f %10f\n" % \
                      (gidx, iidx, F,    T, eT,     A, eA, specin, especin,\
                       ra, era,     dec, edec,    x, ex,  y, ey, \
                       shape[0], eshape[0], shape[1], eshape[1],  shape[2], eshape[2])
            else:                
                str1 = "%4d  %4d  %d    %10f %10f   %10f %10f  " \
                      "%10f %10f   %10f %10f   %10f %10f   %10f %10f   " \
                      "%10f %10f   %10f %10f   %10f %10f\n" % \
                      (gidx, iidx, F,    T, eT,     A, eA, \
                       ra, era,     dec, edec,    x, ex,  y, ey, \
                       shape[0], eshape[0], shape[1], eshape[1],  shape[2], eshape[2])
            f.write(str1)
        f.close()
        print '--> Wrote ASCII file ' + filename
        
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
              + "SpectralIndex:0='0.0', MajorAxis, MinorAxis, Orientation) = format\n"
        f.write(str1)
        sep = ', '
        if srcroot == None:
            sname = img.imagename.split('.')[0]
        else:
            sname = srcroot  
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
                spin1 = g.spin1
                if spin1 != None:
                    specin = str("%.3e" % (spin1[1]))
                # srcid = g.source_id
                # src_for_g = img.source[srcid]
                # for g_src in src_for_g.gaussians:
                #     if g_src.gaus_num == g.gaus_num:
                #         spin1 = g_src.spin1
                #         if spin1 != None:
                #             specin = str("%.3e" % (spin1[1]))
                       
            str_src.append(src + sep + stype + sep + sra + sep + sdec + sep + total + sep + pol + \
                          freq + sep + '0' + sep + specin + sep + deconvstr + '\n')
        # sort by flux (largest -> smallest)
        flux_indx = range(len(str_src))
        flux_indx.sort(lambda x,y: cmp(total_flux[x],total_flux[y]), reverse=True)
        for i in flux_indx:
            f.write(str_src[i])
        f.close()
        print '--> Wrote BBS sky model ' + filename
        
    if format == 'ds9':
        # write ds9 region file
        import numpy as N
        from const import fwsig
        out_region_file = open(filename, "w")
        region_comment = '# Region file format: DS9 version 4.0\nglobal color=green font="helvetica 10 normal" select=1 highlite=1 edit=1 move=1 delete=1 include=1 fixed=0 source\nfk5\n'
        out_region_file.write(region_comment)
        if srcroot == None:
            sname = img.imagename.split('.')[0]
        else:
            sname = srcroot  
        bm_pix = N.array([img.pixel_beam[0]*fwsig, img.pixel_beam[1]*fwsig, img.pixel_beam[2]])
        bm_deg = img.pix2beam(bm_pix)
        for g in img.gaussians():
            src = sname + '_' + str(g.gaus_num)
            ra, dec = g.centre_sky
            deconv = g.deconv_size_sky
            if deconv[0] == 0.: deconv[0] = bm_deg[0]
            if deconv[1] == 0.: deconv[1] = bm_deg[1]
            if deconv[0] <= bm_deg[0] and deconv[1] <= bm_deg[1]:
                stype = 'POINT'
                deconv[0] = bm_deg[0]
                deconv[1] = bm_deg[1]
                region = 'point('+str(ra)+','+str(dec)+') # point=cross width=2 text={'+src+'}\n'
            else:
                stype = 'GAUSSIAN'
                region = 'ellipse('+str(ra)+','+str(dec)+','+str(deconv[0]*3600.0)+'",'+str(deconv[1]*3600.0)+'",'+str(deconv[2]+90.0)+') # text={'+src+'}\n'
            out_region_file.write(region)
        out_region_file.close()
        print '--> Wrote ds9 region file ' + filename
        
def write_resid_img(img, filename=None):
    """Write the residual Gaussian image to a fits file."""
    if filename == None:
        filename = img.imagename + '.resid_gaus.fits'
    temp_im = make_fits_image(N.transpose(img.resid_gaus), img.wcs_obj)
    temp_im.writeto(filename, clobber=True)
    print 'Wrote FITS file ' + filename

def write_model_img(img, filename=None):
    """Write the model Gaussian image to a fits file."""
    if filename == None:
        filename = img.imagename + '.model_gaus.fits'
    temp_im = make_fits_image(N.transpose(img.model_gaus), img.wcs_obj)
    temp_im.writeto(filename, clobber=True)
    print 'Wrote FITS file ' + filename

def write_rms_img(img, filename=None):
    """Write the rms image to a fits file."""
    if img.opts.rms_map == True:
        if filename == None:
            filename = img.imagename + '.rms.fits'
        temp_im = make_fits_image(N.transpose(img.rms), img.wcs_obj)
        temp_im.writeto(filename, clobber=True)
        print 'Wrote FITS file ' + filename
    else:
        print 'RMS map is set to a constant. No image written.'
    
def write_ch0_img(img, filename=None):
    """Write the ch0 image (used for source detection) to a fits file."""
    if filename == None:
        filename = img.imagename + '.ch0.fits'
    temp_im = make_fits_image(N.transpose(img.ch0), img.wcs_obj)
    temp_im.writeto(filename, clobber=True)
    print 'Wrote FITS file ' + filename

def make_fits_image(imagedata, wcsobj):
    """Makes a simple FITS hdulist appropriate for images"""
    import pyfits
    hdu = pyfits.PrimaryHDU(imagedata)
    hdulist = pyfits.HDUList([hdu])
    header = hdulist[0].header
    header.update('CTYPE1', wcsobj.ctype[0])
    header.update('CTYPE2', wcsobj.ctype[1])
    header.update('CRVAL1', wcsobj.crval[0])
    header.update('CRVAL2', wcsobj.crval[1])
    header.update('CDELT1', wcsobj.cdelt[0])
    header.update('CDELT2', wcsobj.cdelt[1])
    header.update('CRPIX1', wcsobj.crpix[0])
    header.update('CRPIX2', wcsobj.crpix[1])
    header.update('CROTA1', wcsobj.crota[0])
    header.update('CROTA2', wcsobj.crota[1])
    hdulist[0].header = header
    return hdulist
    
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
        if img.opts.spectralindex_do:
            # if hasattr(g, 'spin1') == False:
            #     import pdb; pdb.set_trace()
            spin1 = g.spin1
            espin1 = g.espin1
            if spin1 == None:
                specin = 0.0
                especin = 0.0
            else:                       
                specin = spin1[1]
                especin = espin1[1]
            # srcid = g.source_id
            # src_of_g = img.source[srcid]
            # for g_src in src_of_g.gaussians:
            #     if g_src.gaus_num == g.gaus_num:
            #         if hasattr(g_src, 'spin1') == False:
            #             import pdb; pdb.set_trace()
            #         spin1 = g_src.spin1
            #         espin1 = g_src.espin1
            #         if spin1 == None:
            #             specin = 0.0
            #             especin = 0.0
            #         else:                       
            #             specin = spin1[1]
            #             especin = espin1[1]
        else:
            specin = 0.0
            especin = 0.0
  
        list1 = [gidx, iidx, F, T, eT, A, eA, ra, era, dec, edec, x, ex, y, ey, shape[0], eshape[0], shape[1], eshape[1], \
                 shape[2], eshape[2], 0.,0.,0.,0.,0.,0., 0.,0.,0.,0.,specin , especin, iidx, 0,0,0,0, 0.,0.,0.,0.,0.,0.]
        fbdsm.append(list1)
    fbdsm = func.trans_gaul(fbdsm)
    return fbdsm



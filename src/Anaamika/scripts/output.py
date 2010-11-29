"""Module output.

Dupms results of source detection in a variety of formats.
"""

from image import Op
import os
import pyfits
import fbdsm_fitstable_stuff as stuff

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
            self.write_bbs(img)
            self.write_gaul(img)
            self.write_star(img)
            self.write_kvis_ann(img)
            self.write_gaul_FITS(img)

        return img

    def write_bbs(self, img):
        """ Writes the gaussian list as a bbs-readable file"""
        import numpy as N
        from const import fwsig

        fname = img.imagename + '.sky_in'
        if img.opts.spectralindex_do: 
          freq = "%.5e" % img.freq0
        else:
          freq = "%.5e" % img.cfreq
        f = open(fname, 'w')
        str1 = "# (Name, Type, Ra, Dec, I, Q, U, V, ReferenceFrequency='"+freq+", SpectralIndexDegree='0', " \
              + "SpectralIndex:0='0.0', MajorAxis, MinorAxis, Orientation) = format\n "
        f.write(str1)

        sep = ', '
        sname = img.imagename.split('.')[0]
        str_src = []
        total_flux = []
        bm_pix = N.array([img.pixel_beam[0]*fwsig, img.pixel_beam[1]*fwsig, img.pixel_beam[2]])
        bm_deg = img.pix2beam(bm_pix)
        print bm_deg
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


    def write_gaul(self, img):
        fname = img.imagename + '.gaul'
        f = open(fname, "w")

        for g in img.gaussians():
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

            str = "%3d  %4d  %d    %10g %5g   %10g %5g   " \
                  "%10g %5g   %10g %5g   %10g %5g   %10g %5g   " \
                  "%10g %5g   %10g %5g   %10g %5g\n" % \
                  (gidx, iidx, 0,    0, 0,     A, eA, \
                   ra, era,     dec, edec,    x, ex,  y, ey, \
                   shape[0], eshape[0], shape[1], eshape[1],  shape[2], eshape[2])
            f.write(str)

        f.close()

    def write_star(self, img):
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

    def write_kvis_ann(self, img):
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

    def write_gaul_FITS(self, img):
        """ Write as FITS binary table. """

        cnames, cunit, cformat = stuff.cnames, stuff.cunit, stuff.cformat
        fbdsm_list = pybdsm2fbdsm(img)
        col_list = []
        for ind, col in enumerate(fbdsm_list):
          list1 = pyfits.Column(name=cnames[ind], format=cformat[ind], unit=cunit[ind], array=fbdsm_list[ind])
          col_list.append(list1)
        tbhdu = pyfits.new_table(col_list)
        tbhdu.writeto(img.imagename+'.gaul.FITS', clobber=True)


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
        A = g.peak_flux
        ra, dec = g.centre_sky
        x, y = g.centre_pix
        shape = g.size_sky
        eA = g.peak_fluxE
        era, edec = g.centre_skyE
        ex, ey = g.centre_pixE
        eshape = g.size_skyE
        list1 = [gidx, iidx, 0, 0., 0., A, eA, ra, era, dec, edec, x, ex, y, ey, shape[0], eshape[0], shape[1], eshape[1], \
                 shape[2], eshape[2], 0.,0.,0.,0.,0.,0., 0.,0.,0.,0., 0.,0., iidx, 0,0,0,0, 0.,0.,0.,0.,0.,0.]
        fbdsm.append(list1)
    fbdsm = func.trans_gaul(fbdsm)

    return fbdsm



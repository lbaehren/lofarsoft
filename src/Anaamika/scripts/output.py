"""Module output.

Dupms results of source detection in a variety of formats.
"""

from image import Op
import os
import pyfits
import fbdsm_fitstable_stuff as stuff

class Op_outlist(Op):
    """Write out list of gaussians

    Currently 3 output formats are supported:
    - fbdsm gaussian list
    - star list
    - kvis annotations

    All output lists are generated atm.
    """
    def __call__(self, img):
        self.write_gaul(img)
        self.write_star(img)
        self.write_kvis_ann(img)
        self.write_gaul_FITS(img)

        return img

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

    return (hh, mm, ss)

def dec2ddmmss(deg):
    """Convert DEC coordinate (in degrees) to DD MM SS"""

    from math import modf
    sign = (-1 if deg < 0 else 1)
    x, dd = modf(abs(deg))
    x, ma = modf(x*60)
    sa = x*60

    return (dd, ma, sa, sign)

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



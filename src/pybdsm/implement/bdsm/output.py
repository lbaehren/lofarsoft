"""Module output.

Dupms results of source detection in a variety of formats.
"""

from image import Op

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

        return img

    def write_gaul(self, img):
        fname = img.opts.fits_name + '.gaul'
        f = open(fname, "w")

        for g in img.gaussians():
            gidx = g.gaussian_idx
            iidx = g.island_idx
            A = g.peak_flux
            ra, dec = g.center_sky
            x, y = g.center_pix
            shape = g.shape_sky

            str = "%3d  %4d  %d    %10g %5g   %10g %5g   " \
                  "%10g %5g   %10g %5g   %10g %5g   %10g %5g   " \
                  "%10g %5g   %10g %5g   %10g %5g\n" % \
                  (gidx, iidx, 0,    0, 0,     A, 0, \
                   ra, 0,     dec, 0,    x, 0,  y, 0, \
                   shape[0], 0, shape[1], 0,  shape[2], 0)
            f.write(str)

        f.close()

    def write_star(self, img):
        fname = img.opts.fits_name + '.star'
        f = open(fname, 'w')

        for g in img.gaussians():
            A = g.peak_flux
            ra, dec = g.center_sky
            shape = g.shape_sky
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
        fname = img.opts.fits_name + '.kvis.ann'
        f = open(fname, 'w')
        f.write("### KVis annotation file\n\n")
	f.write("color green\n\n")

        for g in img.gaussians():
            iidx = g.island_idx
            A = g.peak_flux
            ra, dec = g.center_sky
            shape = g.shape_sky
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

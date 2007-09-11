from Image import Op

class Op_outlist(Op):
    """Write out list of gaussians"""

    def __call__(self, img):
        fname = img.opts.fits_name + '.gaul'
        f = open(fname, "w")
        
        for idx, isl in enumerate(img.islands):
            for gidx, gaus in enumerate(isl.gaul):
                ### convert to canonical units
                ra, dec = img.opts.xy2radec(*gaus[1:3])
                shape = img.opts.pix2beam(gaus[3:6])

                str = "%3d  %4d  %d    %10g %5g   %10g %5g   " \
                      "%10g %5g   %10g %5g   %10g %5g   %10g %5g   " \
                      "%10g %5g   %10g %5g   %10g %5g\n" % \
                      (gidx, idx, 0,    0, 0,     gaus[0], 0, \
                       ra, 0,     dec, 0,    gaus[1], 0,  gaus[2], 0, \
                       shape[0], 0, shape[1], 0,  shape[2], 0)
                
                f.write(str)

        f.close()

        fname = img.opts.fits_name + '.star'
        f = open(fname, 'w')

        for isl in img.islands:
            for gaus in isl.gaul:
                ### convert to canonical units
                ra, dec = img.opts.xy2radec(*gaus[1:3])
                shape = img.opts.pix2beam(gaus[3:6])

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
                       4, gaus[0], '')

                f.write(str)

        f.close()

def ra2hhmmss(deg):
    from math import modf
    if deg < 0:
        raise RuntimeError("Negative RA")
    x, hh = modf(deg/15.)
    x, mm = modf(x*60)
    ss = x*60
    return (hh, mm, ss)

def dec2ddmmss(deg):
    from math import modf
    sign = (-1 if deg < 0 else 1)
    x, dd = modf(abs(deg))
    x, ma = modf(x*60)
    sa = x*60
    return (dd, ma, sa, sign)

from Image import Op

class Op_outlist(Op):
    """Write out list of gaussians"""

    def __call__(self, img):
        fname = img.opts.fits_name + '.gaul'
        f = open(fname, "w")
        
        for idx, isl in enumerate(img.islands[0:10]):
            for gidx, gaus in enumerate(isl.gaul):
                ### convert to canonical units
                ra, dec = img.opts.xy2radec(*gaus[1:3])
                shape = img.opts.pix2beam(gaus[3:6])

                str = "%3d  %4d  %d    %10g %5g   %10g %5g" \
                      "%10g %5g   %10g %5g   %10g %5g   %10g %5g" \
                      "%10g %5g   %10g %5g   %10g %5g\n" % \
                      (gidx, idx, 0,    0, 0,     gaus[0], 0, \
                       ra, 0,     dec, 0,    gaus[1], 0,  gaus[1], 0, \
                       shape[0], 0, shape[1], 0,  shape[2], 0)
                
                f.write(str)

        f.close()

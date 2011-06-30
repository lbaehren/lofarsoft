
import numpy as N

def pytess_simple(tilecoord, pixel, wts, tess_sc, tess_fuzzy):
    """ Simple tesselation """
    
    xgen, ygen = tilecoord
    xgen = N.asarray(xgen)
    ygen = N.asarray(ygen)
    ngen = len(xgen)
    i,j = pixel
    dist = N.sqrt((i-xgen)*(i-xgen)+(j-ygen)*(j-ygen))/wts
    minind = dist.argmin()

    if tess_sc == 's':
      tilenum=minind
    else:
      print " Not yet implemented !!!! "

    return tilenum


def pytess_roundness(tilecoord, pixel, wts, tess_sc, tess_fuzzy):
    """ Simple tesselation """

    print " Not yet implemented !!!! "
    return 0


def pixintile(tilecoord, pixel, tess_method, wts, tess_sc, tess_fuzzy):
    """  This has routines to find out which tile a given pixel belongs to. """

    if tess_method == 'roundness': 
        tilenum = pytess_roundness(tilecoord, pixel, wts, tess_sc, tess_fuzzy)
    else:
        tilenum = pytess_simple(tilecoord, pixel, wts, tess_sc, tess_fuzzy)

    return tilenum





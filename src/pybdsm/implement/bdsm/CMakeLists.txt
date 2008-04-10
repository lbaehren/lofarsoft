## ----------------------------------------------------------------------
## $Id$
## ----------------------------------------------------------------------

set (bdsm_files __init__.py FITS.py image.py bstat.py gausfit.py
    		fittedimage.py islands.py output.py)


install (FILES ${bdsm_files} DESTINATION lib/python/bdsm)

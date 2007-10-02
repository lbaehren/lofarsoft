## ----------------------------------------------------------------------
## $Id$
## ----------------------------------------------------------------------

set (bdsm_files __init__.py FITS.py Image.py bstat.py gausfit.py
    		islands.py output.py)


install (FILES ${bdsm_files} DESTINATION lib/python/bdsm)

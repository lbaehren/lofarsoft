## ----------------------------------------------------------------------
## $Id$
## ----------------------------------------------------------------------

set (bdsm_files __init__.py image.py FITS.py collapse.py preprocess.py
	rmsimage.py islands.py gausfit.py fittedimage.py output.py)


install (FILES ${bdsm_files} DESTINATION lib/python/bdsm)

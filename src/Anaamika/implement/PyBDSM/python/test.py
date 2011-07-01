import sys
import numpy as N
import bdsm

sys.path.append('')

def plotim():
    """ Plots the image and overlays the island borders with the island number. Also draws the detected gaussians
        at their fwhm radius, with each source being a colour (and line style). """
    bdsm.analysis.plotresults(img)

def getisl(c):
    """ Plots the image and overlays the island borders with the island number. Also draws the detected gaussians
        at their fwhm radius, with each source being a colour (and line style). """
    islid = bdsm.analysis.getisland(img, c)
    return img.islands[islid]

def plot_morph_isl(img, isls=None, thr=None):
    bdsm.analysis.plot_morph_isl(img, isls, thr)



#img = bdsm.execute(bdsm.fits_chain,{'fits_name': "cg.FITS", 'beam' : (0.0020, 0.0020, 0.0), 'thresh' : "hard"})

#img = bdsm.execute(bdsm.fits_chain,{'fits_name': "subim.fits", 'beam' : (0.0015, 0.0015, 0.0), 'thresh':"hard"})

#img = bdsm.execute(bdsm.fits_chain,{'fits_name': "matteo_mfs.im.fits", 'beam' : (0.002, 0.002, 0.0), 'thresh':"hard"})

#img = bdsm.execute(bdsm.fits_chain,{'fits_name': "WN65341H.fits", 'beam': (.0165515, .01500, 0.0), 'thresh':"hard", 'atrous_do' : True})

img = bdsm.execute(bdsm.fits_chain,{'fits_name': "WN35078H.fits", 'beam': (.0261, .01500, 0.0), 'thresh':"hard", 'atrous_do' : True })

#img = bdsm.execute(bdsm.fits_chain,{'fits_name': "SST1cont.image.restored.fits", 'beam': (.008333, .008333, 0.0), 'thresh':"hard", 'atrous_do' : False})

#img = bdsm.execute(bdsm.fits_chain,{'fits_name': "bootbig.FITS", 'beam': (.00154, .00154, 0.0), 'thresh':"hard", \
#                'atrous_do' : True, 'atrous_bdsm_do' : False})

#img = bdsm.execute(bdsm.fits_chain,{'fits_name': "WN35060H", 'beam': (.0165515, .01500, 0.0), 'thresh':"hard"})

#img = bdsm.execute(bdsm.fits_chain,{'fits_name': "lock_cube1.fits", 'beam': (.0054, .0044, 0.0), \
#       'collapse_mode' : 'average', 'collapse_wt' : 'unity', 'beam_sp_derive' : True, 'debug_figs' : True})

#img = bdsm.execute(bdsm.fits_chain,{'fits_name': "newcube1.fits", 'beam': (.00389, .003056, 0.0), \
#                'collapse_mode' : 'average', 'collapse_wt' : 'rms', 'thresh' : 'hard'})

#img = bdsm.execute(bdsm.fits_chain,{'fits_name': "sim1.1.FITS", 'beam': (.00143, .00143, 0.0),\
#                'collapse_mode' : 'average', 'collapse_wt' : 'rms', 'thresh' : 'hard', 'thresh_pix' : '30'})

#img = bdsm.execute(bdsm.fits_chain,{'fits_name': "A2255_I.fits", 'beam': (.018, .014, 5.0), 'collapse_mode'
#        : 'average', 'collapse_wt' : 'rms', 'thresh' : 'hard', 'thresh_isl' : 20.0, 'thresh_pix' : 50.0,
#        'polarisation_do': True})

#img = bdsm.execute(bdsm.fits_chain,{'fits_name': "try.fits", 'beam': (.056, .028, 160.0), 'thresh':"hard", 'thresh_pix':20.})


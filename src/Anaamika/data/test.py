"""Simple script to test basic functionality of PyBDSM"""
import bdsm

img = bdsm.execute(bdsm.fits_chain,{'fits_name': "WN65341H.fits", 'beam': (.0165515, .01500, 0.0), 'collapse_mode' : 'average', 'collapse_wt' : 'rms', 'thresh':"hard", 'polarisation_do': False, 'spectralindex_do': False, 'shapelet_do': False, 'psf_vary_do': False})


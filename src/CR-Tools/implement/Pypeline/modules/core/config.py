"""Configuration and settings.
"""

import os

from hftools import *

LOFARSOFT=os.environ["LOFARSOFT"]
PYCRBIN=LOFARSOFT+"/release/bin/python/"
PYCR=LOFARSOFT+"/src/CR-Tools/implement/Pypeline/scripts/"

pydocpyfilename=LOFARSOFT +"/build/cr/implement/Pypeline/hftools.doc.py"
pydoctxtfilename=LOFARSOFT +"/build/cr/implement/Pypeline/hftools.doc.tex"

filename_lofar_onesecond=LOFARSOFT+"/data/lofar/RS307C-readfullsecond.h5"
filename_lofar_onesecond=LOFARSOFT+"/data/lofar/RS307C-readfullsecondtbb1.h5"
filename_lopes_test=LOFARSOFT+"/data/lopes/test.event"
filename_lofar=LOFARSOFT+"/data/lofar/rw_20080701_162002_0109.h5"
filename_lopes=LOFARSOFT+"/data/lopes/example.event"

DEFAULTDATAFILE=filename_lopes

# Read in the doc strings for the functions in hftools
execfile(pydocpyfilename)
f=open(pydoctxtfilename)
pycralldocstring=f.read()
f.close()

trackHistory.__doc__="""
Function for PYCRTOOLS module. Globally switch history tracking of
hArrays on (True) or off (False). Default at start-up is "on". This
setting is valid for all arrays created after the function is
called. If history tracking is on, you can use array.history() to see
a list of actions that were performed with the array."""


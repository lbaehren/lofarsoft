""" 
  Module rficlean.py. Part of the LOFAR Cosmic Ray pipeline.
  Works on a given CR event file, cleaning the spectrum from narrow-band interference.
  It can also do gain calibration, i.e. flattening the spectrum.
  Mostly taken from testrfi3.py.
  
  !! Working with event files opened by IO submodule of pycrtools. 
  !! This gives compatibility issues down the line in workspaces.py and harray.py
  
.. moduleauthor:: Arthur Corstanje <A.Corstanje@astro.ru.nl>
"""

import os
import time
from pycrtools import *

def cleanSpectrum(crfile):
    #------------------------------------------------------------------------
    #Creating Workspaces
    #------------------------------------------------------------------------
    ws=CRMainWorkSpace(datafile=crfile,doplot=True,verbose=True,modulename="ws")

    # note that the number of bins (e.g. nbins=256) is the number of bins after spline fitting
    ws.makeFitBaseline(ws,logfit=True,fittype="BSPLINE",nbins=256)#256) #fittype="POLY" or "BSPLINE"
    ws.makeAverageSpectrum(ws) 
    # ? average spectrum over all antennas or over all blocks for each antenna?

    #------------------------------------------------------------------------
    #Setting parameters
    #------------------------------------------------------------------------
    ws["blocksize"] = 2*65536
    ws["max_nblocks"]=2
    #ws["ncoeffs"]=45
    ws["ncoeffs"]=12
    if ws["datafile"]["Observatory"]=='LOFAR':
        ws["numin"]=12 #MHz
        ws["numax"]=82 #MHz
    if ws["datafile"]["Observatory"]=='LOPES':
        ws["numin"]=43 #MHz
        ws["numax"]=73 #MHz

    #------------------------------------------------------------------------
    #Begin Calculations
    #------------------------------------------------------------------------

    print 'workspace frequencies are: ', ws["frequency"]

    """
    Calculate a spectrum averaged over all blocks (fft and sum)
    """
    ws["spectrum"].craveragespectrum(ws["datafile"],ws)

    """
    Fit a polynomial/spline (as specified) to baseline of the (binned) average spectrum and return the coefficients.
    """
    ws["meanrms"]=ws["coeffs"].crfitbaseline(ws["frequency"],ws["spectrum"],ws)

    """
    Calculcate the baseline from the coefficients on the actual frequency grid.
    """
    print ws["frequency"]
    print ws["numin_i"]
    print ws["numax_i"]
    print ws["coeffs"]

    ws["baseline"].crcalcbaseline(ws["frequency"],ws["numin_i"],ws["numax_i"],ws["coeffs"],ws)

    if ws["verbose"]: print time.clock()-ws["t0"],"s: Applying gain calibration - flattening spectrum."

    """
    Apply the baseline and do a gain calibration on the spectrum
    """
    ws["cleanspec"].copy(ws.spectrum)
    ws.cleanspec /= ws.baseline
    if ws["doplot"]: ws.cleanspec[0].plot()

    """
    Identify spiky channels and replace them by the mean value (which is unity here).
    """
    ws["meanspec"]=ws["cleanspec"][...,ws["numin_i"]:ws["numax_i"]].meaninverse()
    ws["rfithreshold"] = (ws["meanrms"] * ws["rfi_nsigma"]) + ws["meanspec"]
    ws["nbad_channels"]=ws["bad_channels"][...].findgreaterthan(ws["cleanspec"][...],ws["rfithreshold"])

    ws["cleanspec"][...].set(ws["bad_channels"][...,[0]:ws["nbad_channels"]],ws["meanspec"])

    if ws["verbose"]: print time.clock()-ws["t0"],"s: Done calculating clean spectrum."
    if ws["doplot"]:
        ws.cleanspec[0].plot(clf=False)
#        plt.savefig("testrfi2-spectrum.pdf",format="pdf")
        raw_input("Plotted clean spectrum - press Enter to continue...")




    
# Execute doctests if module is executed as script
if __name__ == "__main__":
    import doctest
    doctest.testmod()


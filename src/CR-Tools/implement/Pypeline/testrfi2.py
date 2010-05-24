#------------------------------------------------------------------------
#Input section
#------------------------------------------------------------------------
filename=LOFARSOFT+"/data/lofar/rw_20080701_162002_0109.h5"
filename=filename_lopes_test
filename=filename_lofar_onesecond
#------------------------------------------------------------------------
#Setting Workspace
#------------------------------------------------------------------------

ws=CRMainWorkSpace(filename=filename,doplot=True,verbose=True,modulename="ws")  
ws.makeFitBaseline(ws,logfit=True,fittype="BSPLINE",nbins=256) #fittype="POLY" or "BSPLINE"
ws.makeAverageSpectrum(ws)

ws["blocksize"]=2**16
ws["max_nblocks"]=30
ws["ncoeffs"]=45
if ws["datafile"]["Observatory"]=='LOFAR':
    ws["numin"]=12 #MHz
    ws["numax"]=82 #MHz
elif ws["datafile"]["Observatory"]=='LOPES':
    ws["numin"]=43 #MHz
    ws["numax"]=73 #MHz

#Rounding off and setting all parameters to its default value
ws.initParameters()


#------------------------------------------------------------------------
#Begin Calculations
#------------------------------------------------------------------------

"""
Calculate a spectrum averaged over all blocks
"""
ws["spectrum"].craveragespectrum(ws.datafile,ws)

"""
Fit a polynomial to baseline of the (binned) average spectrum and return the coefficients.
"""
ws["meanrms"]=ws["coeffs"].crfitbaseline(ws.frequency,ws.spectrum,ws)

"""
Calculcate the baseline from the coefficients on the actual frequency grid.
"""
ws["baseline"].crcalcbaseline(ws["frequency"],ws["numin_i"],ws["numax_i"],ws["coeffs"],ws)

if ws["verbose"]: print time.clock()-ws["t0"],"s: Applying gain calibration - flattening spectrum."

"""
Apply the baseline and do a gain calibration on the spectrum
"""
ws.cleanspec.copy(ws.spectrum)
ws.cleanspec /= ws.baseline
if ws["doplot"]: ws.cleanspec[0].plot()

"""
Identify spiky channels and replace them by the mean value (which is unity here).
"""
ws.meanrms *= ws.rfi_nsigma

ws["nbad_channels"]=ws.bad_channels[...].findgreaterthan(ws.cleanspec[...],ws.meanrms+1.0)
ws.cleanspec[...].set(ws.bad_channels[...,[0]:ws["nbad_channels"]],1.0)

if ws["verbose"]: print time.clock()-ws["t0"],"s: Done."
if ws["doplot"]: ws.cleanspec[0].plot(clf=False)

#--------------------------------------------------------------------------------
#End of Calculations
#--------------------------------------------------------------------------------

"""
Input parameters used:
----------------------

#WorkSpace(Main):
ws["filename"] = /Users/falcke/LOFAR/usg/data/lofar/RS307C-readfullsecondtbb1.h5 # (local)
ws["blocksize"] = 65536 # (local)
ws["datafile"] = crfile('/Users/falcke/LOFAR/usg/data/lofar/RS307C-readfullsecondtbb1.h5') # (local)
ws["nofAntennas"] = 16 # (local)
ws["fftLength"] = 32769 # (local)
ws["frequency"] = hArray(float,[32769], name="Frequency") # len=32769, slice=[0:32769], vec -> [0.0,0.0030517578125,0.006103515625,0.0091552734375,0.01220703125,0.0152587890625,0.018310546875,0.0213623046875,...] # (local)
ws["numin"] = 12 # (local)
ws["numax"] = 82 # (local)
ws["numin_i"] = 3932 # (local)
ws["numax_i"] = 26869 # (local)
ws["verbose"] = True # (local)
ws["cleanspec"] = hArray(float,[16, 32769], name="Spectrum") # len=524304, slice=[0:524304], vec -> [1.0,0.00124850455294,0.000631638154174,0.000715415264217,0.000649151929872,0.000415066067646,0.000510754113443,0.000361641910164,...] # (local)
ws["bad_channels"] = hArray(int,[16, 32769], name="Bad Channels") # len=524304, slice=[0:524304], vec -> [0,3776,3785,3814,3817,3818,3822,3823,...] # (local)
ws["fft"] = hArray(complex,[16, 32769], name="FFT(E-Field)") # len=524304, slice=[0:524304], vec -> [(-274442+0j),(245.301752421+134.386972915j),(-45.3902736682+546.673909526j),(86.3575620375+542.382177548j),(-226.090654152-218.0407746j),(-248.462999369+208.515324508j),(95.2067140532-221.256523278j),(155.935851812+27.2885829908j),...] # (local)
ws["spectrum"] = hArray(float,[16, 32769], name="Spectrum") # len=524304, slice=[0:524304], vec -> [75393050597.7,252035.752124,127508.302224,144419.367734,131041.773732,83786.8488445,103101.446733,73000.3502569,...] # (local)
ws["t0"] = 3.046911 # (local)
ws["nbad_channels"] = Vec(int,16)=[778,980,774,985,682,...] # (local)
ws["fx"] = hArray(float,[16, 65536], name="E-Field") # len=1048576, slice=[0:1048576], vec -> [-12.0,-5.0,3.0,3.0,1.0,1.0,2.0,-7.0,...] # (local)
ws["ncoeffs"] = 45 # (local)
ws["doplot"] = True # (local)
ws["rfi_nsigma"] = 5 # (local)
ws["baseline"] = hArray(float,[16, 32769], name="Baseline") # len=524304, slice=[0:524304], vec -> [201870555.258,201870110.55,201869221.137,201867887.027,201866108.228,201863884.754,201861216.62,201858103.846,...] # (local)
ws["max_nblocks"] = 30 # (local)
ws["meanrms"] = Vec(float,16)=[0.958876600909,0.958510099893,0.958082971876,0.9578043747,0.957126813019,...] # (local)
#WorkSpace(FitBaseline):
ws.FitBaseline["nbins"] = 256 # (local)
ws.FitBaseline["ncoeffs"] = 45 # (parent)
ws.FitBaseline["polyorder"] = 44 # (local)
ws.FitBaseline["nofAntennas"] = 16 # (local)
ws.FitBaseline["freqs"] = hArray(float,[1, 256], name="Frequency") # len=256, slice=[0:256], vec -> [10.9405517578,11.2243652344,11.5081787109,11.7919921875,12.0758056641,12.3596191406,12.6434326172,12.9272460938,...] # (local)
ws.FitBaseline["spectrum"] = hArray(float,[16, 256], name="Binned Spectrum") # len=4096, slice=[0:4096], vec -> [12.1450864705,12.1813578646,12.2334895168,12.2672087637,12.2944396224,12.4026420821,12.4079874006,12.4450289961,...] # (local)
ws.FitBaseline["rms"] = hArray(float,[16, 256], name="RMS of Spectrum") # len=4096, slice=[0:4096], vec -> [35652.044937,34684.8737311,146261.655494,3501682.40996,658188.738859,35287.1723256,80771.9801297,41257.4227067,...] # (local)
ws.FitBaseline["rmsfactor"] = 2.0 # (local)
ws.FitBaseline["verbose"] = True # (parent)
ws.FitBaseline["selected_bins"] = hArray(int,[16, 256], name="Selected bins") # len=4096, slice=[0:4096], vec -> [0,1,5,7,8,14,20,22,...] # (local)
ws.FitBaseline["numax_i"] = 27459 # (local)
ws.FitBaseline["chisquare"] = Vec(int,16)=[522236304,522236304,522236304,522236304,522236304,...] # (local)
ws.FitBaseline["doplot"] = True # (parent)
ws.FitBaseline["numin"] = 12 # (parent)
ws.FitBaseline["ratio"] = hArray(float,[16, 256], name="RMS/Amplitude") # len=4096, slice=[0:4096], vec -> [0.035001680085,0.0330175725368,0.0279501398224,0.0379549439765,0.0410754285471,0.0461331450392,0.0323703741036,0.0786441626367,...] # (local)
ws.FitBaseline["xpowers"] = hArray(float,[16, 256, 45], name="Powers of Frequency") # len=184320, slice=[0:184320], vec -> [1.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,...] # (local)
ws.FitBaseline["bwipointer"] = 0 # (local)
ws.FitBaseline["nselected_bins"] = Vec(int,16)=[226,224,225,223,232,...] # (local)
ws.FitBaseline["clean_bins_y"] = hArray(float,[16, 256], name="Binned Spectrum") # len=4096, slice=[0:4096], vec -> [12.1450864705,12.1813578646,12.2334895168,12.2672087637,12.2944396224,12.4026420821,12.4079874006,12.4450289961,...] # (local)
ws.FitBaseline["clean_bins_x"] = hArray(float,[16, 256], name="Clean Frequencies") # len=4096, slice=[0:4096], vec -> [10.9405517578,11.2243652344,12.3596191406,12.9272460938,13.2110595703,14.9139404297,16.6168212891,17.1844482422,...] # (local)
ws.FitBaseline["baseline_x"] = hArray(float,[32769, 45], name="Powers of Frequency") # len=1474605, slice=[0:1474605], vec -> [0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,...] # (local)
ws.FitBaseline["numin_i"] = 3539 # (local)
ws.FitBaseline["covariance"] = hArray(float,[16, 45, 45]) # len=32400, slice=[0:32400], vec -> [0.0006022611158,-0.000687710105982,0.000309070804521,-0.000140185270957,6.87880008799e-05,-3.65784207838e-05,2.02561962107e-05,-1.61026435258e-05,...] # (local)
ws.FitBaseline["numax"] = 82 # (parent)
ws.FitBaseline["logfit"] = True # (local)
ws.FitBaseline["meanrms"] = 0 # (local)
ws.FitBaseline["fftLength"] = 32769 # (parent)
ws.FitBaseline["height_ends"] = hArray(float,[2, 16]) # len=32, slice=[0:32], vec -> [12.2231375098,12.1986436157,12.2426304397,12.2212455634,12.1255058606,12.2380744562,12.1310966275,12.1805350712,...] # (local)
ws.FitBaseline["extendfit"] = 0.1 # (local)
ws.FitBaseline["t0"] = 3.046911 # (parent)
ws.FitBaseline["weights"] = hArray(float,[16, 256], name="Fit Weights") # len=4096, slice=[0:4096], vec -> [0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,...] # (local)
ws.FitBaseline["coeffs"] = hArray(float,[16, 45]) # len=720, slice=[0:720], vec -> [12.1450864705,12.2358775212,12.1890251936,12.4711701933,12.3196306259,12.5544550439,12.2039334161,12.6608780707,...] # (local)
ws.FitBaseline["fittype"] = BSPLINE # (local)
ws.FitBaseline["powers"] = hArray(int,[16, 45]) # len=720, slice=[0:720], vec -> [0,1,2,3,4,5,6,7,...] # (local)

#WorkSpace(AverageSpectrum):
ws.AverageSpectrum["datafile"] = crfile('/Users/falcke/LOFAR/usg/data/lofar/RS307C-readfullsecondtbb1.h5') # (parent)
ws.AverageSpectrum["max_nblocks"] = 30 # (parent)
ws.AverageSpectrum["nblocks"] = 30 # (local)
ws.AverageSpectrum["blocks"] = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29] # (local)
ws.AverageSpectrum["verbose"] = True # (parent)
ws.AverageSpectrum["fx"] = hArray(float,[16, 65536], name="E-Field") # len=1048576, slice=[0:1048576], vec -> [-12.0,-5.0,3.0,3.0,1.0,1.0,2.0,-7.0,...] # (parent)
ws.AverageSpectrum["fft"] = hArray(complex,[16, 32769], name="FFT(E-Field)") # len=524304, slice=[0:524304], vec -> [(-274442+0j),(245.301752421+134.386972915j),(-45.3902736682+546.673909526j),(86.3575620375+542.382177548j),(-226.090654152-218.0407746j),(-248.462999369+208.515324508j),(95.2067140532-221.256523278j),(155.935851812+27.2885829908j),...] # (parent)
ws.AverageSpectrum["t0"] = 3.110615 # (local)
ws.AverageSpectrum["doplot"] = True # (parent)
"""

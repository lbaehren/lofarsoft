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
ws.makeFitBaseline(ws,logfit=True)
ws.makeAverageSpectrum(ws)

ws["blocksize"]=2**16
ws["max_nblocks"]=30
ws["ncoeffs"]=66
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
if ws["verbose"]: print time.clock()-ws.t0,"s: Setting up work arrays."


if ws["numin"]>0: ws["numin_i"]=ws.frequency.findlowerbound(ws.numin).val()
else: ws["numin_i"]=1
if ws["numax"]>0: ws["numax_i"]=ws.frequency.findlowerbound(ws.numax).val()
else: ws["numax_i"]=len(ws.frequency)

"""
Calculate a spectrum averaged over all blocks
"""
ws["spectrum"].craveragespectrum(ws.datafile,ws)

"""
Fit a polynomial to baseline of the average spectrum and return the coefficients.
"""
ws["meanrms"]=ws.FitBaseline.coeffs.crfitbaseline(ws.frequency,ws.spectrum,ws)

"""
Calculate a smooth baseline for the full (large) spectrum from the polynomial coefficients
"""
#ws.baseline.fill(0.0)
#ws.baseline[...,ws["numin_i"]:ws["numax_i"]].polynomial(ws.frequency[ws["numin_i"]:ws["numax_i"]],ws.FitBaseline.coeffs[...],ws.FitBaseline.powers[...])

#ws.baseline_x[ws["numin_i"]:ws["numax_i"]].bsplinefitxvalues(ws.frequency[ws["numin_i"]:ws["numax_i"]],ws.FitBaseline["bwipointer"])
#ws.baseline_x[0,ws["numin_i"]:ws["numax_i"]].bsplinefitxvalues(ws.frequency[ws["numin_i"]:ws["numax_i"]],ws.FitBaseline.clean_bins_x[0,0].val(),ws.FitBaseline.clean_bins_x[0,ws.FitBaseline.nselected_bins[0]].val(),ws["ncoeffs"])
ws.baseline_x[ws["numin_i"]:ws["numax_i"]].bsplinefitxvalues(ws.frequency[ws["numin_i"]:ws["numax_i"]],ws.FitBaseline.clean_bins_x[0,0].val(),ws.FitBaseline.clean_bins_x[0,-1].val(),ws["ncoeffs"])
#ws.baseline_x[ws["numin_i"]:ws["numax_i"]].bsplinefitxvalues(ws.frequency[ws["numin_i"]:ws["numax_i"]],ws.frequency[ws["numin_i"]].val(),ws.frequency[ws["numax_i"]].val(),ws["ncoeffs"])
ws.baseline[...,ws["numin_i"]:ws["numax_i"]].bspline(ws.baseline_x[ws["numin_i"]:ws["numax_i"]],ws.FitBaseline.coeffs[...])

#Now add nice ends (Hanning Filters) to the frequency range to suppress the noise outside the usuable bandwidth
heights=hArray(float,ws["nofAntennas"])
heights[...].copy(ws.baseline[...,ws["numin_i"]])
factor=hArray(float,ws["nofAntennas"],fill=6.9) # Factor 1000 in log
if not ws.FitBaseline.logfit: 
    factor.copy(heights)
    factor *= 1000.0

ws.baseline[...,0:ws["numin_i"]].gethanningfilterhalf(Vector(factor),Vector(heights),Vector(bool,heights,fill=True))

heights[...].copy(ws.baseline[...,ws["numax_i"]-2])
if not ws.FitBaseline.logfit: 
    factor.copy(heights)
    factor *= 1000.0

ws.baseline[...,ws["numax_i"]-1:].gethanningfilterhalf(Vector(factor),Vector(heights),Vector(bool,heights,fill=False))

if ws.FitBaseline.logfit: ws.baseline.exp()

if ws["verbose"]: print time.clock()-ws.t0,"s: Applying gain calibration - flattening spectrum."

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

if ws["doplot"]: ws.cleanspec[0].plot(clf=False)
if ws["verbose"]: print time.clock()-ws.t0,"s: Done."

#--------------------------------------------------------------------------------
#End of Calculations
#--------------------------------------------------------------------------------

"""
Input parameters used:
----------------------

ws["filename"] = /Users/falcke/LOFAR/usg/data/lofar/RS307C-readfullsecondtbb1.h5 # (local)
ws["blocksize"] = 65536 # (local)
ws["datafile"] = crfile('/Users/falcke/LOFAR/usg/data/lofar/RS307C-readfullsecondtbb1.h5') # (local)
ws["nofAntennas"] = 16 # (local)
ws["frequency"] = hArray(float,[32769], name="Frequency") # len=32769, slice=[0:32769], vec -> [0.0,0.0030517578125,0.006103515625,0.0091552734375,0.01220703125,0.0152587890625,0.018310546875,0.0213623046875,...] # (local)
ws["baseline"] = hArray(float,[16, 32769], name="Baseline") # len=524304, slice=[0:524304], vec -> [70815.3715391,70850.9537546,70886.4106311,70921.7421253,70956.9481949,70992.0287992,71026.9838987,71061.8134553,...] # (local)
ws["cleanspec"] = hArray(float,[16, 32769], name="Spectrum") # len=524304, slice=[0:524304], vec -> [1.0,1.0,9.2675562048,2.22403746024,6.85055364505,7.42014916007,0.324455680592,0.279554564561,...] # (local)
ws["bad_channels"] = hArray(int,[16, 32769], name="Bad Channels") # len=524304, slice=[0:524304], vec -> [0,1,13,21,261,441,870,1192,...] # (local)
ws["fft"] = hArray(complex,[16, 32769], name="FFT(E-Field)") # len=524304, slice=[0:524304], vec -> [(-271850+0j),(-192.699263445+617.49820137j),(386.380536933-263.99118465j),(217.764323203+71.8069398954j),(307.263285606+260.039868773j),(137.847102907-395.712846867j),(-1.58454364939+87.630999206j),(58.3920578782-56.6767355201j),...] # (local)
ws["spectrum"] = hArray(float,[16, 32769], name="Spectrum") # len=524304, slice=[0:524304], vec -> [221707267500.0,1255311.10448,656943.794681,157732.611232,486094.380098,526771.442866,23045.1084013,19865.6543174,...] # (local)
ws["t0"] = 3.069276 # (local)
ws["nbad_channels"] = Vec(int,16)=[498,2458,1460,1914,1583,...] # (local)
ws["fx"] = hArray(float,[16, 65536], name="E-Field") # len=1048576, slice=[0:1048576], vec -> [-11.0,0.0,1.0,-12.0,0.0,-5.0,-10.0,-2.0,...] # (local)
ws["doplot"] = True # (local)
ws["rfi_nsigma"] = 7 # (local)
ws["verbose"] = True # (local)
ws["max_nblocks"] = 3 # (local)
ws["polyorder"] = 17 # (local)
ws["meanrms"] = Vec(float,16)=[15.0049262074,15.0331848915,15.0589282951,15.0231420087,15.0132593054,...] # (local)
#WorkSpace(FitBaseline):
ws.FitBaseline["nbins"] = 256 # (local)
ws.FitBaseline["polyorder"] = 17 # (local)
ws.FitBaseline["ncoeffs"] = 18 # (local)
ws.FitBaseline["nofAntennas"] = 16 # (local)
ws.FitBaseline["freqs"] = hArray(float,[1, 256], name="Frequency") # len=256, slice=[0:256], vec -> [0.196838378906,0.587463378906,0.978088378906,1.36871337891,1.75933837891,2.14996337891,2.54058837891,2.93121337891,...] # (local)
ws.FitBaseline["spectrum"] = hArray(float,[16, 256], name="Binned Spectrum") # len=4096, slice=[0:4096], vec -> [11.1961956464,11.230210511,11.2394618395,11.2300710552,11.2080997626,11.179223227,11.1485123372,11.1203019391,...] # (local)
ws.FitBaseline["rms"] = hArray(float,[16, 256], name="RMS of Spectrum") # len=4096, slice=[0:4096], vec -> [238377.028391,144924.052984,181343.865359,183649.390091,161835.833421,149829.16238,179820.30256,137213.715552,...] # (local)
ws.FitBaseline["rmsfactor"] = 2.0 # (local)
ws.FitBaseline["ratio"] = hArray(float,[16, 256], name="RMS/Ampltude") # len=4096, slice=[0:4096], vec -> [0.834949528094,1.12620433322,1.08807866664,1.10032492548,1.60812196302,0.84224127119,1.04070916085,1.68426320056,...] # (local)
ws.FitBaseline["verbose"] = True # (parent)
ws.FitBaseline["selected_bins"] = hArray(int,[16, 256], name="Selected bins") # len=4096, slice=[0:4096], vec -> [0,1,2,3,4,5,6,7,...] # (local)
ws.FitBaseline["doplot"] = True # (parent)
ws.FitBaseline["covariance"] = hArray(float,[16, 18, 18]) # len=5184, slice=[0:5184], vec -> [0.0334207797104,-0.0555410866375,0.0302806304362,-0.00809062335968,0.00126363930837,-0.000127354494779,8.81135113149e-06,-4.35906569438e-07,...] # (local)
ws.FitBaseline["t0"] = 3.069276 # (parent)
ws.FitBaseline["logfit"] = True # (local)
ws.FitBaseline["coeffs"] = hArray(float,[16, 18]) # len=288, slice=[0:288], vec -> [11.1678313683,0.164937509685,-0.108591187661,0.0133954967973,0.00246752524118,-0.000749322293462,8.52501743122e-05,-5.77885034541e-06,...] # (local)
ws.FitBaseline["clean_bins_y"] = hArray(float,[16, 256], name="Binned Spectrum") # len=4096, slice=[0:4096], vec -> [11.1961956464,11.230210511,11.2394618395,11.2300710552,11.2080997626,11.179223227,11.1485123372,11.1203019391,...] # (local)
ws.FitBaseline["clean_bins_x"] = hArray(float,[1, 256], name="Frequency") # len=256, slice=[0:256], vec -> [0.196838378906,0.587463378906,0.978088378906,1.36871337891,1.75933837891,2.14996337891,2.54058837891,2.93121337891,...] # (local)
ws.FitBaseline["meanrms"] = 0 # (local)
ws.FitBaseline["powers"] = hArray(int,[16, 18]) # len=288, slice=[0:288], vec -> [0,1,2,3,4,5,6,7,...] # (local)
ws.FitBaseline["xpowers"] = hArray(float,[16, 256, 18], name="Powers of Frequency") # len=73728, slice=[0:73728], vec -> [1.0,0.196838378906,0.0387453474104,0.00762657137443,0.00150120194596,0.000295494157453,5.81645909293e-05,1.14490237883e-05,...] # (local)

#WorkSpace(AverageSpectrum):
ws.AverageSpectrum["datafile"] = crfile('/Users/falcke/LOFAR/usg/data/lofar/RS307C-readfullsecondtbb1.h5') # (parent)
ws.AverageSpectrum["max_nblocks"] = 3 # (parent)
ws.AverageSpectrum["nblocks"] = 3 # (local)
ws.AverageSpectrum["blocks"] = [0, 1, 2] # (local)
ws.AverageSpectrum["verbose"] = True # (parent)
ws.AverageSpectrum["fx"] = hArray(float,[16, 65536], name="E-Field") # len=1048576, slice=[0:1048576], vec -> [-11.0,0.0,1.0,-12.0,0.0,-5.0,-10.0,-2.0,...] # (parent)
ws.AverageSpectrum["fft"] = hArray(complex,[16, 32769], name="FFT(E-Field)") # len=524304, slice=[0:524304], vec -> [(-271850+0j),(-192.699263445+617.49820137j),(386.380536933-263.99118465j),(217.764323203+71.8069398954j),(307.263285606+260.039868773j),(137.847102907-395.712846867j),(-1.58454364939+87.630999206j),(58.3920578782-56.6767355201j),...] # (parent)
ws.AverageSpectrum["t0"] = 3.069276 # (parent)
ws.AverageSpectrum["doplot"] = True # (parent)
"""

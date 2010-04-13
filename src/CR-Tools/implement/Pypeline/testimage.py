#gdb --annotate=3 -quiet -f --args python -i /Users/falcke/LOFAR/usg/src/CR-Tools/implement/Pypeline/pycrtools.py -i /Users/falcke/LOFAR/usg/src/CR-Tools/implement/Pypeline/test.py

def p_(var): 
    if (type(var)==list): map(lambda x:p_(x),var)
    else: print " ",var,"=>",eval(var)

#from pycrtools import *

filename_sun=LOFARSOFT+"/data/lopes/example.event"
filename_biglofar=LOFARSOFT+"/data/lofar/rw_20080701_162002_0109.h5"
filename_lofar=LOFARSOFT+"/data/lofar/trigger-2010-02-11/triggered-pulse-2010-02-11-TBB1.h5"
figno=0

plt.clf()
#------------------------------------------------------------------------

file=crfile(filename_sun)
file["blocksize"]=2048
NyquistZone=2
size=5
center=[178.,28.,1]
FarField=True

size2=size/2
azrange=(center[0]-size2,center[0]+size2+1,1)
elrange=(center[1]-size2,center[1]+size2+1,1)
n_az=int((azrange[1]-azrange[0])/azrange[2])
n_el=int((elrange[1]-elrange[0])/elrange[2])
n_pixel=n_az*n_el

file_time=file["Time"].setUnit("\\mu","s")
file_frequencies=file["Frequency"]
file_efield=file["Fx"].setPar("xvalues",file_time)

file_fft=file["emptyFFT"].setPar("xvalues",file_frequencies)

file_efield[...].fft(file_fft[...],NyquistZone)

antenna_positions=file.getCalData("Position")
cal_delays=file.getCalData("Delay")
delays=hArray(float,dimensions=cal_delays)
weights=hArray(complex,dimensions=[n_pixel,file["nofSelectedAntennas"]],name="Complex Weights")

shifted_fft=hArray(complex,dimensions=weights)
beamformed_fft=hArray(complex,dimensions=[n_pixel,file["fftLength"]])


phase_center=hArray(antenna_positions[0].vec())
antenna_positions -= phase_center


azel=hArray(float,dimensions=[n_pixel,3])
cartesian=azel.new()

#------------------------------------------------------------------------

n=0
for az in range(*azrange):
    for el in range (*elrange):
        azel[n,0]=az
        azel[n,1]=el
        azel[n,2]= 1
        n+=1

hCoordinateConvert(azel[...],CoordinateTypes.AzElRadius,cartesian[...],CoordinateTypes.Cartesian,FarField)
hGeometricDelays(antenna_positions,cartesian,delays,FarField)
delays += cal_delays
hGeometricWeights(file_frequencies,antenna_positions,cartesian,weights,FarField)

file_fft.copy(shifted_fft)
shifted_fft *= weights

for n in range(n_pixels):
    beamformed_fft[n] = shifted_fft[n,0]
    shifted_fft[n,1:,...].addto(beamformed_fft[n])


"""

normalize by the number of antennas (which for some reason we don't
need to do after all), 
file_calfft_shifted_added /= file.nofSelectedAntennas

and then FFT back into the time domain:

"""
file_efield_shifted_added=hArray(float,dimensions=file_time,name="beamformed E-field",xvalues=file_time)
file_calfft_shifted_added.invfft(file_efield_shifted_added,2)

file_efield[0].plot()
file_efield_shifted_added.plot(xlim=(-2,0),clf=False,legend=["Reference Antenna","Beam"])
savefigure()
"""

In the plot one can see how the green line (beamformed) traces the
data in antenna one (which was our reference antenna). Of course, I
still do not understand why we do not need to normalize the beamformed
data by the number of antennas ...

(+) Appendix: Listing of all Functions:
=======================================

"""
help(all)
"""

"""

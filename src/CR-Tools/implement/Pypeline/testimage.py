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

print "t=",time.clock(),"s -","Reading in data and setting parameters"
file=crfile(filename_sun)
file["blocksize"]=128 #2**12
nblocks=10
NyquistZone=2
size=7
pixel_sep=1
center=[178.9,27.7,1]
center=[178.9,30.0,1]
center=[178.9,28.0,1]
FarField=True

size2=size/2
azrange=(center[0]-size2,center[0]+size2+1,pixel_sep)
elrange=(center[1]-size2,center[1]+size2+1,pixel_sep)
n_az=int((azrange[1]-azrange[0])/azrange[2])
n_el=int((elrange[1]-elrange[0])/elrange[2])
n_pixels=n_az*n_el

file_time=file["Time"].setUnit("\\mu","s")
file_frequencies=file["Frequency"]
file_efield=file["Fx"].setPar("xvalues",file_time)

file_fft=file["emptyFFT"].setPar("xvalues",file_frequencies)

file_efield[...].fft(file_fft[...],NyquistZone)

print "t=",time.clock(),"s -","Reading in Calibration Data"
antenna_positions=file.getCalData("Position")
cal_delays=file.getCalData("Delay")
delays=hArray(float,dimensions=[n_pixels,file["nofSelectedAntennas"]])

print "t=",time.clock(),"s -","Initializing results arrays"
phases=hArray(float,dimensions=[n_pixels,file["nofSelectedAntennas"],file["fftLength"]],name="Phases")
weights=hArray(complex,dimensions=[n_pixels,file["nofSelectedAntennas"],file["fftLength"]],name="Complex Weights")

shifted_fft=hArray(complex,dimensions=weights,name="FFT times Weights")
beamformed_fft=hArray(complex,dimensions=[n_pixels,file["fftLength"]],name="Beamformed FFT")
power=hArray(float,dimensions=beamformed_fft,name="Spectral Power",xvalues=file_frequencies,fill=0.0)

phase_center=hArray(antenna_positions[0].vec())
antenna_positions -= phase_center


azel=hArray(float,dimensions=[n_pixels,3])
cartesian=azel.new()

#------------------------------------------------------------------------

print "t=",time.clock(),"s -","Creating pximap GRID"
n=0
for el in np.arange (*elrange):
    for az in np.arange(*azrange):
        azel[n,0]=az
        azel[n,1]=el
        azel[n,2]= 1
        n+=1

print "t=",time.clock(),"s -","Calculating coordinates and delays"
hCoordinateConvert(azel[...],CoordinateTypes.AzElRadius,cartesian[...],CoordinateTypes.Cartesian,FarField)
hGeometricDelays(delays,antenna_positions,cartesian,FarField)
delays += cal_delays

print "t=",time.clock(),"s -","Calculating phases"
#phases=hArray(float,dimensions=sun_fft,name="Phases",xvalues=sun_frequencies)
phases.delaytophase(file_frequencies,delays)
#hGeometricPhases(sun_frequencies,antenna_positions,cartesian,phases,True)


print "t=",time.clock(),"s -","Calculating weights"
weights.phasetocomplex(phases)
#hGeometricWeights(file_frequencies,antenna_positions,cartesian,weights,FarField)


for block in range(nblocks):
    print "t=",time.clock(),"s -","Reading in Block #",block
    file["block"]=block
    file_efield.read(file,"Fx")
    file_efield[...].fft(file_fft[...],NyquistZone)
#
    print "t=",time.clock(),"s -","Applying weights"
    file_fft.copy(shifted_fft)
    shifted_fft *= weights
#
    print "t=",time.clock(),"s -","Beamforming"
    for n in range(n_pixels):
        beamformed_fft[n] = shifted_fft[n]
        shifted_fft[n,1:,...].addto(beamformed_fft[n])
#
    print "t=",time.clock(),"s -","Adding up spectral power"
    beamformed_fft.spectralpower(power)

print "t=",time.clock(),"s -","Binning and Normalizing"
intpower=np.array(power[...].sum())
maxval=intpower.max()
intpower /= maxval
intpower.resize([n_az,n_el])

print "t=",time.clock(),"s -","Plotting"
plt.clf()
plt.imshow(intpower,cmap=plt.cm.hot)
plt.savefig("sunimage2.pdf")
#print intpower
print "t=",time.clock(),"s -","Done"


"""
ccm=Vector(complex,file["nofSelectedAntennas"]*(file["nofSelectedAntennas"]-1)/2*file["fftLength"])
hCrossCorrelationMatrix(ccm,file_fft.vec(),file["fftLength"])
binning=Vector(int,1,fill=file["fftLength"])
image=Vector(float,int(size/pixel_sep)**2,fill=0)
hImageFromCCM(image,ccm,weights.vec(),binning.vec(),file["fftLength"])
"""

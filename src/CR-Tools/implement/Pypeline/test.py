#gdb --annotate=3 -quiet -f --args python -i /Users/falcke/LOFAR/usg/src/CR-Tools/implement/Pypeline/pycrtools.py -i /Users/falcke/LOFAR/usg/src/CR-Tools/implement/Pypeline/test.py
from pycrtools import *

filename_sun=LOFARSOFT+"/data/lopes/example.event"
filename_biglofar=LOFARSOFT+"/data/lofar/rw_20080701_162002_0109.h5"
filename_lofar=LOFARSOFT+"/data/lofar/trigger-2010-02-11/triggered-pulse-2010-02-11-TBB1.h5"

#------------------------------------------------------------------------sun=crfile(filename_sun)
sun=hFileOpen(filename_sun)
sun.filename=filename_sun

figno=0
"""

Now we read in the FFTed data

"""
sunfft=sun["FFT"]
"""

and make a complex scratch vector to hold an intermediated data
product which is a copy of the fft vector.

"""
crosscorr_cmplx=hArray(copy=sunfft)
"""

We then multiply the FFTs of all antennas with the complex conjugate
of a reference antenna (here antenna 0) and store that in the vector
crosscorr_cmplx, which is done by the following method.

"""
crosscorr_cmplx[...].crosscorrelatecomplex(sunfft[0])
"""

This vector will then actually hold the FFT of the cross correlation
of the antenna time series. Hence, what we now need to do is to FFT
back into the time domain and store it in a vector crosscorr.

"""
crosscorr=hArray(float,dimensions=[sun["nofSelectedAntennas"],sun["blocksize"]],name="Cross Correlation") 
crosscorr_cmplx[...].invfft(crosscorr[...],1)
"""

We can now plot this and use as the x-axis vector the Time Lags
conveniently provided by our cr file object.

"""
crosscorr.par.xvalues=sun["TimeLag"].setUnit("\\mu","")
#crosscorr[1].plot()
#figno+=1; plt.savefig("tutorial-fig"+str(figno))
"""

Note, plotting crosscorr[0].plot() will give the autocorrelation of
the first antenna (which we used as reference antenna in this example).

(++) Coordinates
---------------

We also have access to a few functions dealing with astronomical
coordinates. Assume, we have a source at an Azmiut/Elevation position
of (178 Degree,28 Degree) and we want to convert that into Cartesian
coordinates (which, for example, is required by our beamformer).

We first turn this into std vector and create a vector that is
supposed to hold the Cartesian coordinates. Note that the AzEL vector
is actually AzElRadius, where we need to set the radius to unity.

"""
azel=hArray([178.+90,28,1000])
cartesian=azel.new()

"""

We then do the conversion, using 

"""
hCoordinateConvert(azel,CoordinateTypes.AzElRadius,cartesian,CoordinateTypes.Cartesian,True)
"""

yielding the following output vector:

"""
cartesian
"""

Reading in Antennapositions:
"""
#antenna_positions_original=hArray(Vector().extendflat(hgetAntennaPositions(sun)),dimensions=[sun["nofAntennas"],3])
#antenna_positions2=hArray(copy=antenna_positions_original)

#antenna_positions2[...] -= antenna_positions_original[0]

obsdelays=hArray([0.0,0.05, -0.05 ,0, -0.125,-0.0375,0.125,0.175])

antenna_positions=hArray(hgetCalData(sun,"Positions"))

sun.Date=1265926154-6*3.15*10**7

#for i in range(8):
#    antenna_positions.vec()[i*3]=antenna_positions2.vec()[i*3+1]  
#    antenna_positions.vec()[i*3+1]=antenna_positions2.vec()[i*3]  

result=[]
#plt.clf()
"""
for j in range(29,30):
    x=[]; y=[]
    for i in range(177,178):
        azel[0]=float(i)
        azel[1]=float(j)
        hCoordinateConvert(azel,CoordinateTypes.AzElRadius,cartesian,CoordinateTypes.Cartesian,True)
        hGeometricDelays(antenna_positions,hArray(cartesian),delays,True)
        delays *= 10**6
        deviations=delays-obsdelays
        deviations.abs()
        sum=deviations.vec().sum()
        x.append(i); y.append(sum)
    result.append(y)

#    plt.plot(x,y)

#figno+=1; plt.savefig("tutorial-fig"+str(figno))

#plt.clf()
#plt.plot(x,result[0])
"""

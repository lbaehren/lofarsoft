# Test script to look at stability of RFI phases

from pycrtools import *
from pycrtools import srcfind as sf

deg2rad = 2*np.pi / 360.0
f = open('/Users/acorstanje/triggering/CR/L29590_D20110714T174749.986Z_CS003_R000_tbb.h5')

#f = open('/Users/acorstanje/triggering/CR/L28318_D20110613T051515.914Z_CS003_R000_tbb.h5')
#f = open('/Users/acorstanje/triggering/CR/L44792_D20120204T002716.906Z_CS003_R000_tbb.h5')
f["BLOCKSIZE"]=65536

plt.figure()
ph = np.zeros(14)
averagePhasePerAntenna = np.zeros(48)
n = 0
fchannel = 5058
freqs = f["FREQUENCY_DATA"]
freq = freqs[fchannel]
polarisation = 1
for i in range(14):

    f["BLOCK"] = i
    
    magspectrum = f["FFT_DATA"]
    spectrum = hArray(copy=magspectrum)
    magspectrum.abs()
    magspectrum.square()
    magspectrum.par.xvalues.fillrange(1, 1) # want channel numbers
   
    if i == 3:
        plt.figure()
        magspectrum[3].plot()
        plt.figure(1)
    #magspectrum.plot()

    # s[...].maxpos()
    #f["FREQUENCY_DATA"][5058]
    #f["FREQUENCY_DATA"][4538]

    
    print freqs[fchannel]
    print (0.1 / (2*np.pi)) / freqs[fchannel]
    phases = hArray(complex, [96])
    phases[...].copy(spectrum[..., fchannel]) # strongest rfi line at index 5058 with blocksize 65536

    y = phases.toNumpy()
    y = np.angle(y)
    ph[i] = y[27] # just to print differences across blocks
    y -= y[polarisation] # subtract reference phase y[0] or y[1] (polarisation)
    
    wrapped = y
    for k in range(len(y)): # must be a better way to do this...
        wrapped[k] = y[k] - 2*np.pi * round(y[k] / (2*np.pi))
    
    averagePhasePerAntenna += wrapped[polarisation::2]
    n+=1
    
    if i < 15:
        plt.plot(wrapped[polarisation::2])

    delta = y[32] - y[0]
    print delta -  2*np.pi * round(delta / (2*np.pi))

plt.figure()
plt.plot(np.unwrap(ph))

averagePhasePerAntenna /= n
#averagePhasePerAntenna = wrapped[polarisation::2] # hack, averaging may be ambiguous with the 2-pi wraparound (?)

# find direction of this source; plot image to check quality of fit.

print 'Find direction of source based on averaged phases per antenna...'
allpositions = f["ANTENNA_POSITIONS"].toNumpy()
positions = allpositions[polarisation::2].ravel() # only pol 'polarisation'
# NB. Indexing to the end is done by thisArray[start::step] !

(fitaz, fitel, minPhaseError) = sf.directionBruteForcePhases(positions, averagePhasePerAntenna, freq, elSteps = 9, showImage=True, verbose=True)

print 'Best fit az = %f, el = %f' % (fitaz / deg2rad, fitel / deg2rad)
print 'Phase error = %f' % minPhaseError

modelphases = sf.phasesFromDirection(positions, (fitaz, fitel), freq)
modelphases -= modelphases[0]
plt.figure()
plt.plot(modelphases)
plt.plot(averagePhasePerAntenna)
phaseDiff = averagePhasePerAntenna - modelphases

plt.figure()
plt.plot(sf.phaseWrap(phaseDiff))


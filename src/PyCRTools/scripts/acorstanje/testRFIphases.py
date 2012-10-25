# Test script to look at stability of RFI (radio transmitter) phases
# Created by Arthur Corstanje, May 2012

from pycrtools import *
from pycrtools import srcfind as sf

twopi = 2 * np.pi
deg2rad = twopi / 360.0
#f = open('/Users/acorstanje/triggering/CR/L29590_D20110714T174749.986Z_CS005_R000_tbb.h5')
#f = open('/Users/acorstanje/triggering/CR/L43181_D20120120T191949.209Z_CS003_R000_tbb.h5')
f = open('/Users/acorstanje/triggering/CR/L40797_D20111228T200122.223Z_CS007_R000_tbb.h5')
#f = open('/Users/acorstanje/triggering/CR/L57524_D20120428T050021.699Z_CS030_R000_tbb.h5')
#f = open('/Users/acorstanje/triggering/CR/L44792_D20120204T002716.906Z_CS003_R000_tbb.h5')
f["BLOCKSIZE"]=8192

# get calibration delays from file
caldelays = np.array(f["DIPOLE_CALIBRATION_DELAY"]) # f[...] outputs list!

plt.figure()
plt.title('Measured phases per antenna')
plt.xlabel('Antenna number (RCU/2)')
plt.ylabel('Phase [rad]')
nblocks = 11*8
ph = np.zeros(nblocks)
averagePhasePerAntenna = np.zeros(48) # do one polarisation
n = 0
fchannel = 8889/8 # known strong transmitter channel for this event (may vary +/- 5 channels?)
freqs = f["FREQUENCY_DATA"]
freq = freqs[fchannel]
calphases = (twopi * freq) * caldelays

polarisation = 1
measuredphases = np.zeros((nblocks, 96))

for i in range(nblocks):
# have to cut out the block with the pulse... autodetect is best? 
#    if i == 6:
#        f["BLOCK"] = 8
#    else:
    if i < 7:
        f["BLOCK"] = i
    else:
        f["BLOCK"] = i+1
    f["BLOCK"] = i
    
    magspectrum = f["FFT_DATA"] / f["BLOCKSIZE"] # normalize
    spectrum = hArray(copy=magspectrum)
    magspectrum.abs()
    magspectrum.square()
    magspectrum.par.logplot = 'y'
#    magspectrum.par.xvalues = freqs / 1.0e6
#    magspectrum.par.xvalues.fillrange(1, 1) # want channel numbers
 
    if i == 3:
        plt.figure()
        magspectrum[3].plot()
        plt.title('Power spectrum of one RCU')
#        plt.xlabel('Channel nr.')
        plt.xlabel('Frequency [MHz]')
        plt.ylabel('Power [a.u.]')
        plt.figure(1)

    # s[...].maxpos()
    #f["FREQUENCY_DATA"][5058]
    #f["FREQUENCY_DATA"][4538]
    
    print freqs[fchannel]
    print (0.1 / (2*np.pi)) / freqs[fchannel]
    phases = hArray(complex, [96])
    phases[...].copy(spectrum[..., fchannel]) # strongest rfi line at index 5058 with blocksize 65536

    y = phases.toNumpy()
    y = np.angle(y)
    y += calphases # subtract phases from delay calibration metadata (needs a + sign, why?)
    
    ph[i] = y[27] # just to print differences across blocks for one antenna
    y -= y[polarisation] # subtract reference phase y[0] or y[1] (polarisation)
    
#    wrapped = y
    wrapped = y - twopi * (y / twopi).round() # wrap into [-pi, +pi]
  #  wrapped[12] = 0.0
  
    # NB. Cannot average sequence of phases just like that; when having e.g. phi = +/- pi + noise, 
    # a spurious term -/+ k (2pi / n) will come in, depending on the number of phase wraps k. 
    # Therefore, force consecutive phases to have abs-difference < pi
    if n > 0:
        wrapped -= twopi * ((wrapped - lastwrapped) / twopi).round()
    measuredphases[i] = np.copy(wrapped)
    
    averagePhasePerAntenna += wrapped[polarisation::2]
    n+=1
    
    if i < 15:
        plt.plot(wrapped[polarisation::2])

    lastwrapped = np.copy(wrapped) # for disambiguating phases around wrapping points +/- pi (+ noise)

    delta = y[32] - y[0]
    print delta -  2*np.pi * round(delta / (2*np.pi))

plt.figure()
plt.plot(np.unwrap(ph))
plt.title('Unwrapped phase of one antenna from consecutive blocks')
plt.ylabel('Phase [rad]')
plt.xlabel('block nr.')

averagePhasePerAntenna /= n
#averagePhasePerAntenna = wrapped[polarisation::2] # hack, averaging may be ambiguous with the 2-pi wraparound (?)

# find direction of this source; plot image to check quality of fit.

print 'Find direction of source based on averaged phases per antenna...'
allpositions = f["ANTENNA_POSITIONS"].toNumpy()
positions = allpositions[polarisation::2].ravel() # only pol 'polarisation'
# NB. Indexing to the end is done by thisArray[start::step] !

(fitaz, fitel, minPhaseError) = sf.directionBruteForcePhases(positions, averagePhasePerAntenna, freq, azSteps = 2 * 360,elSteps = 18, showImage=True, verbose=True)

print 'Best fit az = %f, el = %f' % (fitaz / deg2rad, fitel / deg2rad)
print 'Phase error = %f' % minPhaseError

modelphases = sf.phasesFromDirection(positions, (fitaz, fitel), freq)
modelphases -= modelphases[0]
modelphases = sf.phaseWrap(modelphases) # have to wrap again as we subtracted the ref. phase

plt.figure()
plt.plot(modelphases, label='Modeled phases (plane wave)')
plt.plot(averagePhasePerAntenna, label='Avg. measured phases')
plt.legend()

phaseDiff = averagePhasePerAntenna - modelphases

plt.figure()
plt.plot(sf.phaseWrap(phaseDiff), label='Measured - expected phase')

#plt.figure()
rms_phase = measuredphases.std(axis=0) / np.sqrt(n)
plt.plot(rms_phase[polarisation::2], 'r', label='RMS phase noise')
plt.plot( - rms_phase[polarisation::2], 'r')
nanosecondPhase = twopi * freq * 1.0e-9
plt.title('Phase difference between measured and best-fit modeled phase\n1 ns = %1.3f rad' % nanosecondPhase)
plt.ylabel('Phase [rad]')
plt.xlabel('Antenna number (RCU/2)')
plt.legend()

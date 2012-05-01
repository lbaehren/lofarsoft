# Test script to look at stability of RFI phases

from pycrtools import *
f = open('/Users/acorstanje/triggering/CR/L29590_D20110714T174749.986Z_CS003_R000_tbb.h5')

f["BLOCKSIZE"]=65536

plt.figure()
for i in range(3):

    f["BLOCK"] = i
    
    magspectrum = f["FFT_DATA"]
    spectrum = f["FFT_DATA"]
    magspectrum.abs()
    magspectrum.square()
    #magspectrum.plot()

    # s[...].maxpos()
    #f["FREQUENCY_DATA"][5058]
    #f["FREQUENCY_DATA"][4538]

    sc = f["FFT_DATA"]
    phases = hArray(complex, [96])
    phases[...].copy(spectrum[..., 5058]) # strongest rfi line at index 5058 with blocksize 65536
    mags = hArray(complex, copy=phases)
    mags.abs()

    phases /= mags # better use np.angle
    phases.log()
    phases.imag() # now it contains phases as its real part, imag part = 0
    phasevalues = hArray(float, [96], copy=phases) # conversion OK?

    y = phasevalues.toNumpy()
    wrapped = y
    for i in range(len(y)): # must be a better way to do this...
        wrapped[i] = y[i] - np.pi * int(y[i] / np.pi)

    plt.plot(wrapped)
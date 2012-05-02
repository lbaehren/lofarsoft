# Test script to look at stability of RFI phases

from pycrtools import *
f = open('/Users/acorstanje/triggering/CR/L29590_D20110714T174749.986Z_CS003_R000_tbb.h5')

f["BLOCKSIZE"]=65536

plt.figure()
ph = np.zeros(14)
for i in range(14):

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
#    mags = hArray(complex, copy=phases)
#    mags.abs()

    y = phases.toNumpy()
    y = np.angle(y)
    ph[i] = y[4] # just to print differences across blocks
    y -= y[0] # subtract reference phase
    
    wrapped = y
    for k in range(len(y)): # must be a better way to do this...
        wrapped[k] = y[k] - 2*np.pi * round(y[k] / (2*np.pi))
    
    if i < 15:
        plt.plot(wrapped[0:-1:2])
        print 'plot'
    delta = y[4] - y[0]
    print delta -  2*np.pi * round(delta / (2*np.pi))

plt.figure()
plt.plot(np.unwrap(ph))
# Test script to look at stability of RFI phases

from pycrtools import *
f = open('/Users/acorstanje/triggering/CR/L29590_D20110714T174749.986Z_CS003_R000_tbb.h5')
#f = open('/Users/acorstanje/triggering/CR/L28318_D20110613T051515.914Z_CS003_R000_tbb.h5')
#f = open('/Users/acorstanje/triggering/CR/L44792_D20120204T002716.906Z_CS003_R000_tbb.h5')
f["BLOCKSIZE"]=65536

plt.figure()
ph = np.zeros(14)
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

    freqs = f["FREQUENCY_DATA"]
    print freqs[5058]
    print (0.1 / (2*np.pi)) / freqs[5058]
    phases = hArray(complex, [96])
    phases[...].copy(spectrum[..., 5058]) # strongest rfi line at index 5058 with blocksize 65536

    y = phases.toNumpy()
    y = np.angle(y)
    ph[i] = y[27] # just to print differences across blocks
    y -= y[0] # subtract reference phase
    
    wrapped = y
    for k in range(len(y)): # must be a better way to do this...
        wrapped[k] = y[k] - 2*np.pi * round(y[k] / (2*np.pi))
    
    if i < 15:
        plt.plot(wrapped[0:-1:2])

    delta = y[32] - y[0]
    print delta -  2*np.pi * round(delta / (2*np.pi))

plt.figure()
plt.plot(np.unwrap(ph))



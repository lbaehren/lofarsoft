import numpy as np
import matplotlib.pyplot as plt

twopi = 2*np.pi

phase = - np.pi + np.arange(100) * twopi / 100.0
fftphase = np.zeros(100)
for i in range(100):
#    x = np.cos(twopi * np.arange(i, 1024+i) * 16.33971234 / 1024)
    x = np.cos(phase[i] + twopi * np.arange(1024) * 173.73312 / 1024)
    # x *= np.hanning(len(x))
    y = np.fft.fft(x)
    fftphase[i] = np.angle(y[50])
    if fftphase[i] > np.pi:
        fftphase[i] -= twopi
    elif fftphase[i] < - np.pi:
        fftphase[i] += twopi

plt.figure()
plt.plot(phase)
plt.plot(fftphase, c = 'r')


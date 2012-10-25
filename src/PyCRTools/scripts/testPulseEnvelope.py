from pycrtools import *
f = open('/Users/acorstanje/triggering/CR/L40797_D20111228T200122.223Z_CS002_R000_tbb.h5')
f["BLOCKSIZE"] = 65536
f["BLOCK"] = 7
data = f["TIMESERIES_DATA"]
caldelays = f["DIPOLE_CALIBRATION_DELAY"]
y = data.toNumpy()
pulse_start = 34000
pulse_end = 34000 + 1024

pulseEnvelope = trerun("PulseEnvelope", "0", timeseries_data = data, pulse_start = pulse_start, pulse_end = pulse_end, resample_factor=2)
pulseEnvelope.delays -= hArray(caldelays)

y = pulseEnvelope.delays.toNumpy()
y2 = np.array([0.0] * 48) # has to be 0.0, 0 won't do
for i in range(48):
    y2[i] = y[2*i+1] - y[2*i]
    
y2 *= 1.0e9
plt.scatter(range(len(y2)), y2)

yy = pulseEnvelope.envelope.toNumpy()

plt.figure()
plt.plot(yy[0])
plt.plot(yy[1])

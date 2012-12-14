import pycrtools as cr
import numpy as np

# Test which integral of the spectrum can actually be used to normalize the data

galactic_noise= 7777
blocksize = 700

trace = np.random.randn(blocksize)

# For trace power we choose the convention that sqrt(power) should correspond to sigma, 
# i.e. the standard deviation (ignoring a possible non-zero mean).
# Alternatively you could sum up the power over the whole trace (a factor 'blocksize' larger)
trace_power = np.sum(trace ** 2) / blocksize
print 'Trace mean = %f, sigma = %f' % (trace.mean(), trace.std() )
print 'Trace power = %f' % trace_power
spec=np.fft.rfft(trace) # NB. For FFTW a factor 1/blocksize is needed for normalization of FFT.
print 'dimensions', spec.shape
power = (spec.real)**2+(spec.imag)**2 
power /= blocksize**2

amplitude = np.sqrt(power)

# options
cleaned_power_power = 2*np.sum(power) - power[0] - power[-1] # subtle: first and last element counted once, not twice! From complex -> real fft symmetry
#cleaned_power = np.sum(spec)
cleaned_power_amplitude = 2*np.sum(amplitude)
cleaned_power_rms = np.sqrt(cleaned_power_power)

#print "test of commutation", cleaned_power, np.sqrt(cleaned_power_power), cleaned_power/ np.sqrt(cleaned_power_power)

print 'Sum of amplitudes = %f, summed spectral power = %f, RMS spectral power (sqrt taken) = %f' % (cleaned_power_amplitude, cleaned_power_power, cleaned_power_rms )
print "Check for Parseval's theorem (trace power - spectral power = zero): %f" % (cleaned_power_power - trace_power)

print 'first power', cleaned_power_rms
inverse_power = 1./cleaned_power_rms
spec_normed = spec * inverse_power * galactic_noise
later_power = (spec_normed.real ** 2) + (spec_normed.imag) ** 2

later_power_power = 2 * np.sum(later_power) - later_power[0] - later_power[-1]
later_power_power /= blocksize ** 2 # again normalize to one-sample power
print "later power", later_power_power
print "later power RMS", np.sqrt(later_power_power)

new_trace = np.fft.irfft(spec_normed)


# second try with different random numbers
# just run the script again... :)
"""
trace1 = (np.random.randn(blocksize))*3.
print 'before2',trace1.mean(),trace1.std()
spec1=np.fft.rfft(trace1) / float(blocksize) # NB. Normalization for FFT.
power1 = (spec1.real)**2+(spec1.imag)**2
amplitude1 = np.sqrt(power1)

#options
cleaned_power_power1 = 2 *np.sum(power1)
#cleaned_power1 = np.sum(spec1)
cleaned_power1 =  2*np.sum(amplitude1)

print "test of commutation", cleaned_power1, np.sqrt(cleaned_power_power1),cleaned_power1/np.sqrt(cleaned_power_power1)

print 'first power', cleaned_power1
inverse_power1 = 1./cleaned_power1
spec_normed1 = spec1 * inverse_power1 * galactic_noise
later_power1 = np.sum((spec_normed1.real)**2+(spec_normed1.imag)**2)
print "later power1", later_power1
new_trace1 = np.fft.irfft(spec_normed1)

"""
# Comparison

cr.plt.figure()
cr.plt.plot(trace,color='b')
#cr.plt.plot(trace1,color="r")
cr.plt.plot(new_trace,color='g')
#cr.plt.plot(new_trace1,color='k')

print "end state"
print new_trace.mean(),new_trace.std()
#print new_trace1.mean(),new_trace1.std()
#cr.plt.figure()
#cr.plt.plot(np.sqrt((spec.real)**2+(spec.imag)**2))

cr.plt.show()


"""
Module documentation
====================

"""

from pycrtools.tasks import Task
from pycrtools.grid import CoordinateGrid
import numpy as np
import pycrtools as cr
import matplotlib.pyplot as plt


class PulseEnvelope(Task):
    """Calculate pulse envelope using Hilbert transform.

    Optionally the envelope will be (up/down)sampled using the *resample_factor* and the *delays* between the
    peak_amplitude will be computed with respect to a reference antenna that is either given or taken to be the one
    with the highest signal to noise.
    """

    parameters = dict(
        timeseries_data=dict(default=None,
            doc="Timeseries data for all antennas,  hArray of shape: [nAntennas,nSamples]"),
        nantennas=dict(default=lambda self: self.timeseries_data.shape()[0],
            doc="Number of antennas."),
        pulse_start=dict(default=0,
            doc="Start of pulse window."),
        pulse_end=dict(default=lambda self: self.timeseries_data.shape()[1],
            doc="End of pulse window."),
        pulse_width=dict(default=lambda self: self.pulse_end - self.pulse_start,
            doc="Width of pulse window."),
        window_start=dict(default=lambda self: max(self.pulse_start - self.pulse_width, 0),
            doc="Start of window."),
        window_end=dict(default=lambda self: min(self.pulse_end + self.pulse_width, self.timeseries_data.shape()[1]),
            doc="End of window."),
        window_width=dict(default=lambda self: self.window_end - self.window_start,
            doc="Width of full window."),
        resample_factor=dict(default=1,
            doc="Factor with which the timeseries will be resampled, needs to be integer > 0"),
        window_width_resampled=dict(default=lambda self: self.window_width * self.resample_factor,
            doc="Width of pulse window after resampling."),
        timeseries_data_resampled=dict(default=lambda self: cr.hArray(float, dimensions=(self.nantennas, self.window_width_resampled)),
            doc="Resampled timeseries data."),
        nfreq = dict(default=lambda self: self.window_width_resampled / 2 + 1,
            doc="Number of frequencies."),
        fft_data = dict(default=lambda self: cr.hArray(complex, dimensions=(self.nantennas, self.nfreq)),
            doc = "Fourier transform of timeseries_data_resampled."),
        sampling_frequency=dict(default = 200.e6,
            doc = "Sampling frequency of timeseries_data."),
        hilbertt=dict(default = lambda self: self.timeseries_data_resampled.new(), workarray = True,
            doc = "Hilbert transform of *fft_data*."),
        envelope=dict(default = lambda self: self.timeseries_data_resampled.new(), workarray = True,
            doc = "Envelope calculated using Hilbert transform."),
        fftwplan=dict(default = lambda self: cr.FFTWPlanManyDftR2c(self.window_width_resampled, 1, 1, 1, 1, 1, cr.fftw_flags.ESTIMATE)),
        ifftwplan=dict(default = lambda self: cr.FFTWPlanManyDftC2r(self.window_width_resampled, 1, 1, 1, 1, 1, cr.fftw_flags.ESTIMATE)),
        snr=dict(default = lambda self: cr.hArray(float, self.nantennas), output = True,
            doc = "Signal to noise ratio of pulse maximum."),
        rms=dict(default = lambda self: cr.hArray(float, self.nantennas), output = True,
            doc = "RMS of noise."),
        mean=dict(default = lambda self: cr.hArray(float, self.nantennas), output = True,
            doc = "Mean of noise."),
        peak_amplitude=dict(default = lambda self: cr.hArray(float, self.nantennas), output = True,
            doc = "Pulse maximum."),
        integrated_pulse_power=dict(default = lambda self: cr.hArray(float, self.nantennas), output = True,
            doc = "Integrated pulse power."),
        integrated_noise_power=dict(default = lambda self: cr.hArray(float, self.nantennas), output = True,
            doc = "Integrated noise power per sample."),
        maxpos=dict(default = lambda self: cr.hArray(int, self.nantennas), output = True,
            doc = "Position of pulse maximum relative to *pulse_start*."),
        maxpos_full=dict(default = lambda self: cr.hArray(int, self.nantennas), output = True,
            doc = "Position of pulse maximum relative to *pulse_start*."),
        best=dict(default = lambda self: cr.hMaxPos(self.snr),
            doc = "Antenna with highest signal to noise."),
        bestpos=dict(default = lambda self: self.maxpos[self.best],
            doc = "Position of pulse maximum relative to *pulse_start* in antenna with highest signal to noise."),
        maxdiff=dict(default = lambda self: cr.hMaxDiff(cr.hArray([self.maxpos[i] for i in self.antennas_with_significant_pulses]))[0],
            doc = "Maximum spread in position (in samples)."),
        pulse_maximum_time=dict(default = lambda self: cr.hArray(float, self.nantennas), output = True,
            doc = "Position of pulse maximum relative to data start in seconds."),
        refant=dict(default = lambda self: self.snr.maxpos().val(),
            doc = "Reference antenna for delays, taken as antenna with highest signal to noise."),
        delays=dict(default = lambda self: cr.hArray(float, self.nantennas), output = True,
            doc = "Delays corresponding to the position of the maximum of the envelope relative to the first antenna."),
        nsigma=dict(default = 3,
            doc = "Number of standard deviations that pulse needs to be above noise level in order to be accepted as found."),
        antennas_with_significant_pulses=dict(default = lambda self: [i for i in range(self.nantennas) if self.snr[i] > self.nsigma],
            doc = "Indices of antennas with pulses more than nsigma above the noise limit."),
        npolarizations=dict(default = 1,
            doc = "If this parameter > 1 then these are the number of antennas to be considered as one and pulse search is restricted to the one with the strongest SNR on average."),
        strongest_polarization=dict(default = None,
            doc = "Polarization that is strongest and is used to find pulse strength."),
        save_plots=dict(default = False,
            doc = "Store plots"),
        plot_prefix=dict(default = "",
            doc = "Prefix for plots"),
        plot_type=dict(default = "png",
            doc = "Plot type (e.g. png, jpeg, pdf)"),
        plotlist=dict(default = [],
            doc = "List of plots"),
        plot_antennas=dict(default = lambda self: range(self.nantennas),
            doc = "Antennas to create plots for."),
    )

    def run(self):
        """Run the task.
        """

        # Force recalculation of all task parameters
        self.update(True)

        # Resample singal
        cr.hFFTWResample(self.timeseries_data_resampled[...], self.timeseries_data[..., self.window_start:self.window_end])

        # Compute FFT
        cr.hFFTWExecutePlan(self.fft_data[...], self.timeseries_data_resampled[...], self.fftwplan)

        # Apply Hilbert transform
        cr.hApplyHilbertTransform(self.fft_data[...])

        # Get inverse FFT
        cr.hFFTWExecutePlan(self.hilbertt[...], self.fft_data[...], self.ifftwplan)
        self.hilbertt /= self.window_width_resampled

        # Get envelope
        self.envelope.fill(0)
        cr.hSquareAdd(self.envelope, self.hilbertt)
        cr.hSquareAdd(self.envelope, self.timeseries_data_resampled)
        cr.hSqrt(self.envelope)

        # Find signal to noise ratio, maximum, position of maximum and rms
        cr.hMaxSNR(self.snr[...], self.mean[...], self.rms[...], self.peak_amplitude[...], self.maxpos[...], self.envelope[...], (self.pulse_start - self.window_start) * int(self.resample_factor), (self.pulse_end - self.window_start) * int(self.resample_factor))

        if self.npolarizations > 1:

            # Figure out which polarization has the strongest pulse signal on average
            self.strongest_polarization = int(np.argmax(self.snr.toNumpy().reshape((self.nantennas / self.npolarizations, self.npolarizations)).mean(axis=0)))

            # Update snr and peak amplitude using position of maximum found in strongest polarization
            start = (self.pulse_start - self.window_start) * int(self.resample_factor)
            for i in range(self.nantennas / self.npolarizations):
                for j in range(self.npolarizations):
                    k = i*self.npolarizations
                    n = k + j
                    self.maxpos[n] = self.maxpos[k+self.strongest_polarization]
                    self.peak_amplitude[n] = self.envelope[n, start + self.maxpos[k+self.strongest_polarization]]
                    self.snr[n] = self.peak_amplitude[n] / self.rms[n]

        # Convert to delay
        self.delays[:] = self.maxpos[:]
        self.delays /= (self.sampling_frequency * self.resample_factor)

        # Calculate pulse maximum in seconds since start of datafile
        self.pulse_maximum_time[:] = self.window_start  # Number of samples before start of upsampled block
        self.pulse_maximum_time /= self.sampling_frequency  # Converted to seconds
        self.pulse_maximum_time += self.delays  # Add the number of seconds of the pulse in the upsampled block

        # Don't shift delays to be relative to reference antenna - get absolute time within given window
        #self.delays -= self.delays[self.refant]

        # Calculate integrated pulse power
        self.maxpos_full[...].copy(self.maxpos[...])
        self.maxpos_full /= self.resample_factor

        start = cr.hArray(int, self.maxpos_full.shape()[0])
        start.copy(self.maxpos_full)
        start += self.pulse_start - 5

        end = cr.hArray(int, self.maxpos_full.shape()[0])
        end.copy(self.maxpos_full)
        end += self.pulse_start + 5

        cr.hIntegratedPulsePower(self.integrated_pulse_power[...], self.integrated_noise_power[...], self.timeseries_data[...], start[...], end[...])

        self.integrated_pulse_power /= self.sampling_frequency
        self.integrated_noise_power /= self.sampling_frequency

        if self.save_plots:

            s = self.timeseries_data_resampled.toNumpy()
            y = self.envelope.toNumpy()
            x = 5.e-6 * self.resample_factor * np.arange(y.shape[1])

            # Single pulse envelope of first antenna
            for i in self.plot_antennas:
                plt.clf()

                if i==0:
                    l_signal = plt.plot(x, s[i], linestyle='-', color='#68C8F7', label="Signal")
                    l_envelope = plt.plot(x, y[i], linestyle='-', color='#B30424', label="Envelope")
                    l_rms = plt.plot(x, np.zeros(y.shape[1]) + self.mean[i] + self.rms[i], 'k--', label="RMS")
                else:
                    l_signal[0].set_ydata(s[i])
                    l_envelope[0].set_ydata(y[i])
                    l_rms[0].set_ydata(np.zeros(y.shape[1]) + self.mean[i] + self.rms[i])

                plt.annotate("pulse maximum", xy=(x[self.maxpos[i] + (self.pulse_start - self.window_start) * int(self.resample_factor)], self.peak_amplitude[i]), xytext = (0.13, 0.865), textcoords="figure fraction", arrowprops=dict(arrowstyle="->", connectionstyle="angle,angleA=0,angleB=90,rad=10"))

                p = self.plot_prefix + "pulse_envelope_envelope-{0:d}.{1}".format(i, self.plot_type)

                plt.xlabel(r"Time ($\mu s$)")
                plt.ylabel("Amplitude (ADU)")
                plt.legend()
                plt.title("Pulse envelope for antenna {0:d} {1:f} {2:f}".format(i, self.mean[i], self.rms[i]))
                plt.savefig(p)

                self.plotlist.append(p)

            # All pulse envelopes as stacked plot
            plt.clf()

            offset = 0

            color = ['#B30424', '#68C8F7', '#FF8C19']
            for i in range(y.shape[0]):

                plt.plot(x, y[i] + offset, linestyle='-', color=color[i % self.npolarizations])

                offset += self.peak_amplitude[i]

            p = self.plot_prefix + "pulse_envelope_envelope.{0}".format(self.plot_type)

            plt.gca().autoscale_view(tight=True)
            plt.xlabel(r"Time ($\mu s$)")
            plt.ylabel("Amplitude with offset (ADU)")
            plt.title("All pulse envelopes")
            plt.savefig(p)

            self.plotlist.append(p)

"""
Module documentation
====================

"""

from pycrtools.tasks import Task
import pycrtools as cr


class StokesParameters(Task):
    """Calculate Stokes parameters I, Q, U and V and the polarization angle.
    """

    parameters = dict(
        timeseries_data=dict(default=None,
            doc="Timeseries data."),
        nantennas=dict(default=lambda self: self.timeseries_data.shape()[0] / 3,
            doc="Number of antennas."),
        pulse_start=dict(default=0,
            doc="Start of pulse window."),
        pulse_end=dict(default=lambda self: self.timeseries_data.shape[1],
            doc="End of pulse window."),
        pulse_width=dict(default=lambda self: self.pulse_end - self.pulse_start,
            doc="Width of pulse window."),
        resample_factor=dict(default=1,
            doc="Resample factor."),
        pulse_width_resampled=dict(default=lambda self: self.pulse_width * self.resample_factor,
            doc="Width of pulse window after resampling."),
        timeseries_data_resampled=dict(default=lambda self: cr.hArray(float, dimensions=(self.nantennas, self.pulse_width_resampled)),
            doc="Resampled timeseries data."),
        fft_data = dict(default=lambda self: cr.hArray(complex, dimensions=(self.nantennas, self.pulse_width_resampled / 2 + 1)),
            doc = "Fourier transform of timeseries_data_resampled."),
        hilbertt=dict(default = lambda self: self.timeseries_data_resampled.new(), workarray = True,
            doc = "Hilbert transform of *fft_data*."),
        stokes=dict(default = lambda self: cr.hArray(float, dimensions=(self.nantennas, 4)),
            doc="Stokes parameters I, Q, U and V for each antenna."),
        polarization_angle=dict(default=lambda self: cr.hArray(float, self.nantennas),
            doc="Polarization angle ..math::`\phi=0.5 \atan(U/Q)` for each antenna."),
        fftwplan=dict(default=lambda self: cr.FFTWPlanManyDftR2c(self.pulse_width_resampled, 1, 1, 1, 1, 1, cr.fftw_flags.ESTIMATE)),
        ifftwplan=dict(default=lambda self: cr.FFTWPlanManyDftC2r(self.pulse_width_resampled, 1, 1, 1, 1, 1, cr.fftw_flags.ESTIMATE)),
    )

    def run(self):
        """Run the task.
        """

        # Resample singal
        cr.hFFTWResample(self.timeseries_data_resampled[...], self.timeseries_data[..., self.pulse_start:self.pulse_end])

        # Compute FFT
        cr.hFFTWExecutePlan(self.fft_data[...], self.timeseries_data_resampled[...], self.fftwplan)

        # Apply Hilbert transform
        cr.hApplyHilbertTransform(self.fft_data[...])

        # Get inverse FFT
        cr.hFFTWExecutePlan(self.hilbertt[...], self.fft_data[...], self.ifftwplan)
        self.hilbertt /= self.pulse_width_resampled

        # Calculate Stokes parameters
        cr.hStokesParameters(self.stokes[...], self.timeseries_data_resampled[0:3 * self.nantennas:3, ...], self.timeseries_data_resampled[1:3 * self.nantennas:3, ...], self.hilbertt[0:3 * self.nantennas:3, ...], self.hilbertt[1:3 * self.nantennas:3, ...])

        # Calculate polarization angle
        cr.hAtan2(self.polarization_angle[...], self.stokes[..., 2], self.stokes[..., 1])
        self.polarization_angle /= 2


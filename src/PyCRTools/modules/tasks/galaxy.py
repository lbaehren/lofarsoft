"""
Galaxy documentation
====================

"""

from pycrtools.tasks import Task
import numpy as np
import pytmf
import datetime


class GalacticNoise(Task):
    """Task to calculate Galactic noise level.

    Evaluates a partial Fourier series fit to the Galactic response as a function of Local Apparant Siderial Time.
    """

    parameters = dict(
        fft_data=dict(default=None, doc="FFT data to correct."),
        antenna_set=dict(default="LBA_OUTER", doc="Antenna set"),
        channel_width=dict(default=1., doc="Width of a single frequency channel in Hz"),
        galactic_noise_power=dict(default=(0, 0), doc="Galactic noise power per Hz", output=True),
        timestamp=dict(default=None, doc="Observation time"),
        longitude=dict(default=pytmf.deg2rad(6.869837540), doc="Observer longitude in radians"),
        coefficients_lba=dict(default=(np.array([2.10174906e-04, 8.35381607e-06, -6.49743724e-06, 8.93979789e-07, 3.25335297e-06]), np.array([2.21666835e-04, 6.60195679e-06, -5.24392154e-06, -1.02265106e-06, 3.31638392e-06])),
            doc="Tuple with coefficients for partial Fourier series describing galaxy response in Hz for polarization 0 and 1 respectively"),
        coefficients_hba=dict(default=(np.array([1.0]), np.array([1.0])),
            doc="Tuple with coefficients for partial Fourier series describing galaxy response in Hz for polarization 0 and 1 respectively"),
    )

    def fourier_series(self, x, p):
        """Evaluates a partial Fourier series

        ...math::

            F(x) \approx \frac{a_{0}}{2} + \sum_{n=1}^{\mathrm{order}} a_{n} \sin(nx) + b_{n} \cos(nx)

        """

        r = p[0] / 2

        order = (len(p) - 1) / 2

        for i in range(order):

            n = i + 1

            r += p[2*i + 1] * np.sin(n * x) + p[2*i + 2] * np.cos(n * x)

        return r

    def run(self):
        """Run.
        """

        # Convert timestamp to datetime object
        t = datetime.datetime.utcfromtimestamp(self.timestamp)

        # Calculate JD(UT1)
        ut = pytmf.gregorian2jd(t.year, t.month, float(t.day) + ((float(t.hour) + float(t.minute) / 60. + float(t.second) / 3600.) / 24.))

        # Calculate JD(TT)
        dtt = pytmf.delta_tt_utc(pytmf.date2jd(t.year, t.month, float(t.day) + ((float(t.hour) + float(t.minute) / 60. + float(t.second) / 3600.) / 24.)))
        tt = pytmf.gregorian2jd(t.year, t.month, float(t.day) + ((float(t.hour) + float(t.minute) / 60. + (float(t.second) + dtt / 3600.)) / 24.))

        # Calculate Local Apparant Sidereal Time
        self.last = pytmf.rad2circle(pytmf.last(ut, tt, self.longitude))

        # Evaluate Fourier series for calculated LST
        if "LBA" in self.antenna_set:
            self.galactic_noise_power = (self.fourier_series(self.last, self.coefficients_lba[0]), self.fourier_series(self.last, self.coefficients_lba[1]))
        elif "HBA" in self.antenna_set:
            self.galactic_noise_power = (self.fourier_series(self.last, self.coefficients_hba[0]), self.fourier_series(self.last, self.coefficients_hba[1]))
        else:
            raise ValueError("Unsupported antenna_set {0}".format(self.antenna_set))

        if self.fft_data is not None:

            # Calculate correction factor
            self.correction_factor = self.original_power.new()
            self.correction_factor.copy(self.original_power)

            ndipoles = self.correction_factor.shape()[0]

            cr.hInverse(self.correction_factor)
            cr.hMul(self.correction_factor[0:ndipoles:2, ...], self.galactic_noise_power[0] * self.channel_width)
            cr.hMul(self.correction_factor[1:ndipoles:2, ...], self.galactic_noise_power[1] * self.channel_width)
            cr.hSqrt(self.correction_factor)

            # Correct FFT data
            cr.hMul(fft_data[...], self.original_power)


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

    Evaluates the polynomial fit to the Galactic response as a function of Local Apparant Siderial Time.
    """

    parameters = dict(
        galactic_noise_power=dict(default=0, doc="Galactic noise power per Hz", output=True),
        timestamp=dict(default=None, doc="Observation time"),
        longitude=dict(default=pytmf.deg2rad(6.869837540), doc="Observer longitude in radians"),
        coefficients=dict(default=[7.56798970e-03, -2.29406950e-01, 2.94867516e+00, -2.04868891e+01, 8.13909736e+01, -1.82426264e+02, 2.22528946e+02, -1.44469831e+02, 1.91345039e+01, 7.72182912e+02], doc="Coefficients for polynomial describing galaxy response"),
    )

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

        # Evaluate polynomial for calculated LST
        p = np.poly1d(self.coefficients)
        self.galactic_noise_power = p(self.last)


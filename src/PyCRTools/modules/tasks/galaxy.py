"""
Galaxy documentation
====================

"""

from pycrtools.tasks import Task
import pycrtools as cr
import pytmf
import datetime

class GalacticNoise(Task):
    """Task to calculate Galactic noise level.

    Evaluates the polynomial fit to the Galactic response as a function of Local Apparant Siderial Time.
    """

    parameters = dict(
        galactic_noise = dict( default = cr.hArray(float, 1), doc = "Galactic noise for given paramters", output = True ),
        timestamp = dict( default = None, doc = "Observation time" ),
        longitude = dict( default = pytmf.deg2rad(6.869837540), doc = "Observer longitude in radians" ),
        coefficients = dict( default = [-3.48445802e-03, 1.08926968e-01, -1.33700575e+00, 8.66207005e+00, -3.39160283e+01, 8.52214323e+01, -1.24991383e+02, 7.59724604e+01, -2.49936341e+01, 7.70394797e+02], doc = "Coefficients for polynomial describing galaxy response" ),
        powers = dict( default = lambda self : range(len(self.coefficients)), doc = "Powers of the polynomial terms." )
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
        tt = pytmf.gregorian2jd(t.year, t.month, float(t.day) + ((float(t.hour) + float(t.minute) / 60. + (float(t.second) + dtt / 3600.)) / 24.));
        
        # Calculate Local Apparant Sidereal Time
        self.last = pytmf.rad2circle(pytmf.last(ut, tt, self.longitude));

        # Evaluate polynomial for calculated LST
        cr.hPolynomial(self.galactic_noise, cr.hArray(self.last), cr.hArray(self.coefficients), cr.hArray(self.powers))


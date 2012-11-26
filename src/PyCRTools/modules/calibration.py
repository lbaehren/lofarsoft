## This is a template Python module.
#  Use this template for each module in the Pypeline.
#  Note that a script is not the same as a module, the difference can
#  already be seen by the #! on the first line.
#  For scripts (e.g. files that you want to execute from the commandline)
#  you should use scripts/template.py instead.

"""Each module should have a documentation string describing it.

Style notes:
For Python, PEP 8  has emerged as the style guide that most projects adhere to; it promotes a very readable and eye-pleasing coding style. Every Python developer should read it at some point; here are the most important points extracted for you:

* Use 4-space indentation, and no tabs. 4 spaces are a good compromise between small indentation (allows greater nesting depth) and large indentation (easier to read). Tabs introduce confusion, and are best left out.

* Wrap lines so that they don't exceed 79 characters. This helps users with small displays and makes it possible to have several code files side-by-side on larger displays.

* Use blank lines to separate functions and classes, and larger blocks of code inside functions.

* When possible, put comments on a line of their own.

* Use docstrings.

* Use spaces around operators and after commas, but not directly inside bracketing constructs: a = f(1, 2) + g(3, 4).

* Name your classes and functions consistently; the convention is to use CamelCase for classes and lower_case_with_underscores for functions and methods. Always use self as the name for the first method argument (see A First Look at Classes for more on classes and methods).

* Don't use fancy encodings if your code is meant to be used in international environments. Plain ASCII works best in any case.
"""

## Imports
#  Only import modules that are actually needed and avoid
#  "from module import *" as much as possible to prevent name clashes.
import pycrtools as hf
import numpy as np
import metadata as md
import IO

## Examples


class AntennaCalibration():
    """Class documentation"""

    def __init__(self, files, antennaset, selection=None):
        """Constructor
        Initializes all the Antenna Calibration variables.
        This is a CableDelay, ClockOffset and station fine calibration,
        for as far as these are available.

        This needs to be updated for:
            Station Calibration for blocks not of size 1024
            Values obtained from HDF5 file instead of datafiles in the repository,

        *files*  List of crfiles, opened by IO.open()
        *antennaset* Antennaset corresponding to the files (e.g. "HBA", "LBA_OUTER" )
        """

        self.selection = selection
        # initialize used arrays
        self.antennaIDs = IO.get(files, "antennaIDs", False)

        timeinterval = IO.get(files, "sampleInterval", False)
        samplefrequency = IO.get(files, "sampleFrequency", False)
        self.frequencyValues = hf.hArray(IO.get(files, "frequencyValues", False))
        self.antennaset = antennaset
        self.totalCalibrationtemp = IO.get(files, "emptyFFT", False)
        self.totalShift = IO.get(files, "shift", False).new()

        nyquistZone = IO.get(files, "nyquistZone", True)
        if "HBA" in antennaset and 1 in nyquistZone:
            if IO.get(files, "sampleFrequency") > 180000000.0:
                nyquistZone = 2
            else:
                nyquistZone = 3
            IO.set(files, "nyquistZone", nyquistZone)
        else:
            nyquistZone = nyquistZone[0]

        # CableDelays
        CableDelays = md.get("CableDelays", self.antennaIDs, antennaset, True)

        self.CablePhaseDelays = CableDelays.new()

        self.CablePhaseDelays.fmod(CableDelays, timeinterval)

        self.CableTimeDelays = CableDelays.new()

        self.CableTimeDelays.sub(CableDelays, self.CablePhaseDelays)

        # Clock offset
        ClockDelays = md.get("ClockCorrection", self.antennaIDs, antennaset, True)

        self.ClockPhaseDelays = ClockDelays.new()

        self.ClockPhaseDelays.fmod(ClockDelays, timeinterval)

        self.ClockTimeDelays = ClockDelays.new()

        self.ClockTimeDelays.sub(ClockDelays, self.ClockPhaseDelays)

        # Station Phase Calibration

        self.StationCalibration = md.get("StationPhaseCalibration", self.antennaIDs, antennaset, True)

        if IO.get(files, "blocksize", False) != 1024:
            print "Blocksize not yet supported for station fine calibration."

            # interpolate frequencies
            # BeginFreq=IO.get(files,"frequencyRange",False)[0]
            # StationFrequencies=Vector(float,513)
            # StationFrequencies.fillrange(BeginFreq,samplefrequency/1024)
            self.StationCalibration.copy(self.totalCalibrationtemp)
            self.StationCalibration.fill(complex(1, 0))

        self.CalcShift()
        self.CalcDelay()

    def CalcShift(self):
        """Calculate the shift"""
        # Add that Cable Time delay is also optionally applied
        self.totalShift = self.ClockTimeDelays.new()
        self.totalShift.copy(self.ClockTimeDelays)

    def CalcDelay(self):
        """Calculate the delays"""
        self.totalDelays = self.ClockPhaseDelays.new()
        # a.sub(b,c) = a=b-c
        self.totalDelays.sub(self.ClockPhaseDelays, self.CablePhaseDelays)
        phases = self.totalDelays.new()
        phases.delaytophase(self.frequencyValues, self.totalDelays)
        phases.negate()
        self.totalCalibrationtemp.phasetocomplex(phases)
        self.totalCalibrationtemp.mul(self.StationCalibration)
        self.totalCalibration = IO.applySelection(self.selection, self.totalCalibrationtemp)

    def applyCalibration(self, fftdata):
        """Apply all the calibration set.

        *fftdata* Array with FFT data, data is return in this as well
        """

        fftdata.mul(self.totalCalibration)
        return True

    def applyShift(self, files):
        """Apply the calibration shift set in the calibration.

        *files* List of crfiles to apply the calibration too.
        """

        # if self.totalShift.max().val() == 0.0 and self.totalShift.min().val() = 0.0:
        #    # no shifts to be applied
        #    return True
        # else:
        # list with applied shifts
        nrAnts = IO.get(files, "nofSelectedAntennas", True)
        allinitialShift = hf.hArray(IO.get(files, "shift", False))
        allinitialShift.add(self.totalShift)

        shifts = []
        startAnt = 0
        endAnt = 0
        for num, nAnts in enumerate(nrAnts):
            startAnt = endAnt
            endAnt += nAnts
            shifts.append(allinitialShift[startAnt:endAnt].val())

        IO.set(files, "shiftVector", shifts)

        return True

## Executing a module should run doctests.
#  This examines the docstrings of a module and runs the examples
#  as a selftest for the module.
#  See http://docs.python.org/library/doctest.html for more information.
if __name__ == '__main__':
    import doctest
    doctest.testmod()

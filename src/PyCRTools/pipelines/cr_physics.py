"""CR pipeline.
"""

import numpy as np
import pycrtools as cr
from pycrtools import crdatabase as crdb

from optparse import OptionParser

# Parse commandline options
parser = OptionParser()
parser.add_option("-i", "--id", type="int", help="Event ID", default=1)
parser.add_option("-d", "--database", default="crdb", help="Filename of database")
parser.add_option("--maximum_nof_iterations", default = 5, help="Maximum number of iterations in antenna pattern unfolding loop.")

(options, args) = parser.parse_args()

db_filename = options.database
dbManager = crdb.CRDatabase(db_filename)
db = dbManager.db

# Get event from database
event = crdb.Event(db=db, id=options.id)

# Loop over all stations in event
stations = []
for f in event.datafiles:
    stations.extend(f.stations)

for station in stations:

    # Open file
    f = cr.open(station.datafile.settings.datapath+'/'+station.datafile.filename)

    # Select block containing pulse
    blocksize = station.polarisations[0].parameter["BLOCKSIZE"]
    block = station.polarisations[0].parameter["BLOCK"]
    f["BLOCKSIZE"] = blocksize
    f["BLOCK"] = block

    # Create FFTW plans
    fftplan = cr.FFTWPlanManyDftR2c(blocksize, 1, 1, 1, 1, 1, cr.fftw_flags.ESTIMATE)
    invfftplan = cr.FFTWPlanManyDftC2r(blocksize, 1, 1, 1, 1, 1, cr.fftw_flags.ESTIMATE)

    # Select antennas which are marked good for both polarizations
    names = f["DIPOLE_NAMES"]
    names_good = station.polarisations[0].parameter["antennas"].values() + station.polarisations[1].parameter["antennas"].values()

    selected_dipoles = []
    for i in range(len(names)/2):
        if names[2*i] in names_good and names[2*i+1] in names_good:
            selected_dipoles.extend([names[2*i], names[2*i+1]])

    f["SELECTED_DIPOLES"] = selected_dipoles

    # Read FFT data
    fft_data = f.empty("FFT_DATA")
    f.getFFTData(fft_data, block, False)
    frequencies = cr.hArray(f["FREQUENCY_DATA"])

    # Flag dirty channels (from RFI excission)
    dirty_channels = list(set(station.polarisations[0].parameter["dirty_channels"] + station.polarisations[1].parameter["dirty_channels"]))

    fft_data[..., dirty_channels] = 0

    # Apply calibration delays
    cabledelays = cr.hArray(f["DIPOLE_CALIBRATION_DELAY"])
    weights = cr.hArray(complex, dimensions=fft_data, name="Complex Weights")
    phases = cr.hArray(float, dimensions=fft_data, name="Phases", xvalues=frequencies)

    cr.hDelayToPhase(phases, frequencies, cabledelays)
    cr.hPhaseToComplex(weights, phases)

    fft_data.mul(weights)

    # Get timeseries data
    timeseries_data = f.empty("TIMESERIES_DATA")

    cr.hFFTWExecutePlan(timeseries_data[...], fft_data[...], invfftplan)
    timeseries_data /= blocksize

    # Get antennas positions
    antenna_positions = f["ANTENNA_POSITIONS"]

    # Get pulse window
    pulse_start = min(station.polarisations[0].parameter["pulse_start_sample"], station.polarisations[1].parameter["pulse_start_sample"])
    pulse_end = max(station.polarisations[0].parameter["pulse_end_sample"], station.polarisations[1].parameter["pulse_end_sample"])

    # Get first estimate of pulse direction
    pulse_direction = station.polarisations[0].parameter["pulse_direction"]

    # Start direction fitting loop
    n = 0
    converged = False
    while True:

        print "pulse_direction =", pulse_direction

        # Unfold antenna pattern
        antenna_response = cr.trun("AntennaResponse", direction = pulse_direction)

        # Calculate delays
        pulse_envelope = cr.trun("PulseEnvelope", timeseries_data = timeseries_data, pulse_start = pulse_start, pulse_end = pulse_end, resample_factor = 10)

        # Fit pulse direction
        direction_fit_plane_wave = cr.trun("DirectionFitPlaneWave", positions = antenna_positions, timelags = pulse_envelope.delays, good_antennas = pulse_envelope.antennas_with_significant_pulses)

        pulse_direction = direction_fit_plane_wave.meandirection_azel_deg

        n += 1
        if converged or n > options.maximum_nof_iterations:
            break # Exit fitting loop

# Project polarizations onto x,y,z frame

# Compute LDF


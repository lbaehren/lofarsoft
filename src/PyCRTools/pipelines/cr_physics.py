"""CR pipeline.
"""

import pytmf
import pycrtools as cr
from pycrtools import crdatabase as crdb
from pycrtools import metadata as md

from optparse import OptionParser

# Don't write output to all tasks
cr.tasks.task_write_parfiles = False

# Parse commandline options
parser = OptionParser()
parser.add_option("-i", "--id", type="int", help="Event ID", default=1)
parser.add_option("-d", "--database", default="cr.db", help="Filename of database")
parser.add_option("--maximum_nof_iterations", default = 5, help="Maximum number of iterations in antenna pattern unfolding loop.")

(options, args) = parser.parse_args()

db_filename = options.database
dbManager = crdb.CRDatabase(db_filename)
db = dbManager.db

# Get event from database
event = crdb.Event(db = db, id = options.id)

# Loop over all stations in event
stations = []
for f in event.datafiles:
    stations.extend(f.stations)

for station in stations:

    # Open file
    f = cr.open(station.datafile.settings.datapath+'/'+station.datafile.filename)

    # Set reference polarization to the one that had the best pulse
    if station.polarization['0']["pulse_height_incoherent"] > station.polarization['1']["pulse_height_incoherent"]:
        rp = '0'
    else:
        rp = '1'

    # Select block containing pulse
    blocksize = station.polarization[rp]["BLOCKSIZE"]
    block = station.polarization[rp]["BLOCK"]
    f["BLOCKSIZE"] = blocksize
    f["BLOCK"] = block

    # Create FFTW plans
    fftplan = cr.FFTWPlanManyDftR2c(blocksize, 1, 1, 1, 1, 1, cr.fftw_flags.ESTIMATE)
    invfftplan = cr.FFTWPlanManyDftC2r(blocksize, 1, 1, 1, 1, 1, cr.fftw_flags.ESTIMATE)

    # Select antennas which are marked good for both polarization
    names = f["DIPOLE_NAMES"]
    names_good = station.polarization['0']["antennas"].values() + station.polarization['1']["antennas"].values()

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
    dirty_channels = list(set(station.polarization['0']["dirty_channels"] + station.polarization['1']["dirty_channels"]))

    fft_data[..., dirty_channels] = 0

    # Apply calibration delays
    cabledelays = cr.hArray(f["DIPOLE_CALIBRATION_DELAY"])
    weights = cr.hArray(complex, dimensions=fft_data, name="Complex Weights")
    phases = cr.hArray(float, dimensions=fft_data, name="Phases", xvalues=frequencies)

    cr.hDelayToPhase(phases, frequencies, cabledelays)
    cr.hPhaseToComplex(weights, phases)

    fft_data.mul(weights)

    # Get expected galactic noise strength
    galactic_noise = cr.trun("GalacticNoise", timestamp = f["TIME"][0])

    # Get measured noise strength (using very ugly code needed because not all dipoles are selected and results are stored per polarization)
    antennas_spectral_power = dict(zip(
        station.polarization['0']["antennas"].values()+station.polarization['1']["antennas"].values(),
        station.polarization['0']["antennas_spectral_power"]+station.polarization['1']["antennas_spectral_power"]
        ))

    antennas_spectral_power_correction = cr.hArray([antennas_spectral_power[k] for k in selected_dipoles])

    # Correct to expected level
    cr.hInverse(antennas_spectral_power_correction)
    cr.hMul(antennas_spectral_power_correction, galactic_noise.galactic_noise)
    cr.hMul(fft_data[...], antennas_spectral_power_correction[...])

    # Get timeseries data
    timeseries_data = f.empty("TIMESERIES_DATA")
    nantennas = timeseries_data.shape()[0] / 2

    # Get antennas positions
    antenna_positions = f["ANTENNA_POSITIONS"]

    # Get pulse window
    pulse_start = station.polarization[rp]["pulse_start_sample"]
    pulse_end = station.polarization[rp]["pulse_end_sample"]

    # Get first estimate of pulse direction
    pulse_direction = station.polarization[rp]["pulse_direction"]
    print "database results for direction", pulse_direction

    fft = fft_data.toNumpy()

    # Start direction fitting loop
    n = 0
    converged = False
    while True:

        # Unfold antenna pattern
        antenna_response = cr.trun("AntennaResponse", normalize = False, fft_data = fft_data, frequencies = frequencies, direction = (0., 45.9999999999999))

        # Get timeseries data
        cr.hFFTWExecutePlan(timeseries_data[...], antenna_response.on_sky_polarization[...], invfftplan)
        timeseries_data /= blocksize

        # Calculate delays
        pulse_envelope = cr.trun("PulseEnvelope", timeseries_data = timeseries_data, pulse_start = pulse_start, pulse_end = pulse_end, resample_factor = 10)
        
        # Fit pulse direction
        direction_fit_plane_wave = cr.trun("DirectionFitPlaneWave", positions = antenna_positions, timelags = pulse_envelope.delays, good_antennas = pulse_envelope.antennas_with_significant_pulses,reference_antenna = pulse_envelope.refant, verbose=True)

        pulse_direction = direction_fit_plane_wave.meandirection_azel_deg

        n += 100
        if converged or n > options.maximum_nof_iterations or direction_fit_plane_wave.fit_failed:
            break # Exit fitting loop

    # Project polarization onto x,y,z frame
    xyz_timeseries_data = cr.hArray(float, dimensions = (3*nantennas, blocksize))
    cr.hProjectPolarizations(xyz_timeseries_data[0:3*nantennas:3,...], xyz_timeseries_data[1:3*nantennas:3,...], xyz_timeseries_data[2:3*nantennas:3,...], timeseries_data[0:2*nantennas:2,...], timeseries_data[1:2*nantennas:2,...], pytmf.deg2rad(pulse_direction[0]), pytmf.deg2rad(pulse_direction[1]))

    # Get Stokes parameters
    stokes_parameters = cr.trun("StokesParameters", timeseries_data = xyz_timeseries_data, pulse_start = pulse_start, pulse_end = pulse_end, resample_factor = 10)

# Beamform with all stations

# Compute LDF

# Compute footprint


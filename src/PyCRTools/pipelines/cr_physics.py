"""CR pipeline.
"""

import matplotlib
matplotlib.use("Agg")

import time
import pytmf
import numpy as np
import pycrtools as cr
from pycrtools import crdatabase as crdb
from pycrtools import metadata as md
from pycrtools import tools

from optparse import OptionParser

# Don't write output to all tasks
cr.tasks.task_write_parfiles = False

# Parse commandline options
parser = OptionParser()
parser.add_option("-i", "--id", type="int", help="Event ID", default=1)
parser.add_option("-d", "--database", default="cr.db", help="Filename of database")
parser.add_option("-o", "--output-dir", default="./", help="Output directory")
parser.add_option("--use-unity-ijm", action="store_true", default=False, help="Use unity inverse Jones matrix for unfolding the antenna pattern for testing purposes.")
parser.add_option("--maximum_nof_iterations", default = 5, help="Maximum number of iterations in antenna pattern unfolding loop.")

(options, args) = parser.parse_args()

db_filename = options.database
dbManager = crdb.CRDatabase(db_filename)
db = dbManager.db

start = time.clock()

# Get event from database
event = crdb.Event(db = db, id = options.id)

# Set the event status
event.status = "CR_PHYSICS_PROCESSING"
event.write(recursive=False, parameters=False)

# Loop over all stations in event
stations = []
for f in event.datafiles:
    stations.extend(f.stations)

combined_antenna_positions = []
combined_pulse_strength = []
combined_rms = []

for station in stations:

    try:
        print 80 * "-"
        print station
        print 80 * "-"

        # Open file
        f = cr.open(station.datafile.settings.datapath+'/'+station.datafile.filename)

        # Set reference polarization to the one that had the best pulse
        h0 = 0
        h1 = 0
        try:
            h0 = station.polarization['0']["pulse_height_incoherent"]
        except:
            pass

        try:
            h1 = station.polarization['1']["pulse_height_incoherent"]
        except:
            pass

        if h0 > h1:
            rp = '0'
        else:
            rp = '1'

        # Select block containing pulse
        blocksize = station.polarization[rp]["blocksize"]
        block = station.polarization[rp]["block"]
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
        try:
            cabledelays = cr.hArray(f["DIPOLE_CALIBRATION_DELAY"])
        except:
            print "Error when obtaining DIPOLE_CALIBRATION_DELAY skipping station", f["STATION_NAME"]

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

        fft = fft_data.toNumpy()

        # Start direction fitting loop
        n = 0
        direction_fit_converged = False
        while True:

            # Unfold antenna pattern
            antenna_response = cr.trun("AntennaResponse", normalize = False, fft_data = fft_data, frequencies = frequencies, direction = pulse_direction, test_with_unity_matrix = options.use_unity_ijm)

            # Get timeseries data
            cr.hFFTWExecutePlan(timeseries_data[...], antenna_response.on_sky_polarization[...], invfftplan)
            timeseries_data /= blocksize

            # Calculate delays
            pulse_envelope = cr.trun("PulseEnvelope", timeseries_data = timeseries_data, pulse_start = pulse_start, pulse_end = pulse_end, resample_factor = 10)

            # Use current direction if not enough significant pulses are found for direction fitting
            if len(pulse_envelope.antennas_with_significant_pulses) < 3:
                print "not enough antennas with significant pulses, using previous direction"
                break
            
            # Fit pulse direction
            direction_fit_plane_wave = cr.trun("DirectionFitPlaneWave", positions = antenna_positions, timelags = pulse_envelope.delays, good_antennas = pulse_envelope.antennas_with_significant_pulses,reference_antenna = pulse_envelope.refant, verbose=True)

            pulse_direction = direction_fit_plane_wave.meandirection_azel_deg

            n += 1
            if direction_fit_converged:
                print "fit converged"
                station["crp_pulse_direction"] = pulse_direction
                break

            if n > options.maximum_nof_iterations:
                print "maximum number of iterations reached"
                station["crp_pulse_direction"] = pulse_direction
                break

            if direction_fit_plane_wave.fit_failed:
                print "direction fit failed"
                break

        # Project polarization onto x,y,z frame
        xyz_timeseries_data = cr.hArray(float, dimensions = (3*nantennas, blocksize))
        cr.hProjectPolarizations(xyz_timeseries_data[0:3*nantennas:3,...], xyz_timeseries_data[1:3*nantennas:3,...], xyz_timeseries_data[2:3*nantennas:3,...], timeseries_data[0:2*nantennas:2,...], timeseries_data[1:2*nantennas:2,...], pytmf.deg2rad(pulse_direction[0]), pytmf.deg2rad(pulse_direction[1]))

        # Get Stokes parameters
        stokes_parameters = cr.trun("StokesParameters", timeseries_data = xyz_timeseries_data, pulse_start = pulse_start, pulse_end = pulse_end, resample_factor = 10)

        # Get pulse strength
        pulse_envelope_xyz = cr.trun("PulseEnvelope", timeseries_data = xyz_timeseries_data, pulse_start = pulse_start, pulse_end = pulse_end, resample_factor = 10)

        # Calculate time delay of pulse with respect to the start time of the file (e.g. f["TIME"])
        time_delays = pulse_envelope_xyz.pulse_maximum_time.toNumpy().reshape((nantennas,3))
        time_delays += float(block * blocksize + max(f["SAMPLE_NUMBER"])) / f["SAMPLE_FREQUENCY"][0] + f["CLOCK_OFFSET"][0]

        # Get xyz-polarization instance
        p = station.polarization['xyz']

        # Add parameters
        p["crp_itrf_antenna_positions"] = md.convertITRFToLocal(f["ITRFANTENNA_POSITIONS"]).toNumpy()
        p["crp_pulse_delays"] = time_delays
        p["crp_pulse_strength"] = cr.hArray(pulse_envelope_xyz.maxima).toNumpy().reshape((nantennas, 3))
        p["crp_rms"] = cr.hArray(pulse_envelope_xyz.rms).toNumpy().reshape((nantennas, 3))
        p["crp_stokes"] = stokes_parameters.stokes.toNumpy()
        p["crp_polarization_angle"] = stokes_parameters.polarization_angle.toNumpy()

        p.status = "OK"

    except Exception as e:
        print 80 * "-"
        print "Error occured when processing station", station.stationname
        print 80 * "-"
        print e
        print 80 * "-"

        p = station.polarization['xyz']

        p.status = "BAD"

# Create list of event level plots
plotlist = []

# Get combined parameters from (cached) database
all_station_antenna_positions = []
all_station_pulse_delays = []
all_station_pulse_strength = []
all_station_rms = []
all_station_direction = []

for station in stations:
    try:
        all_station_direction.append(station["crp_pulse_direction"])
        p = station.polarization["xyz"]
        all_station_antenna_positions.append(p["crp_itrf_antenna_positions"])
        all_station_pulse_delays.append(p["crp_pulse_delays"])
        all_station_pulse_strength.append(p["crp_pulse_strength"])
        all_station_rms.append(p["crp_rms"])
    except:
        print "Do not have all pulse parameters for station", station.stationname

all_station_antenna_positions = np.vstack(all_station_antenna_positions)
all_station_pulse_delays = np.vstack(all_station_pulse_delays)
all_station_pulse_strength = np.vstack(all_station_pulse_strength)
all_station_rms = np.vstack(all_station_rms)
all_station_direction = np.asarray(all_station_direction)

# Convert to contiguous array of correct shape
shape = all_station_antenna_positions.shape
all_station_antenna_positions = all_station_antenna_positions.reshape((shape[0] / 2, 2, shape[1]))[:,0]
all_station_antenna_positions = all_station_antenna_positions.copy()

# Calculate average direction and store it
average_direction = tools.averageDirectionLOFAR(all_station_direction[:,0], all_station_direction[:,1])
event["crp_average_direction"] = average_direction

# Beamform with all stations

# Compute LDF and footprint

core = list(stations[0].polarization['0']["pulse_core_lora"])+[0]
core_uncertainties = stations[0].polarization['0']["pulse_coreuncertainties_lora"].toNumpy()
direction_uncertainties = [3.,3.,0]

ldf = cr.trun("Shower", positions = all_station_antenna_positions, signals_uncertainties = all_station_rms, core = core, direction = average_direction, timelags = all_station_pulse_delays, core_uncertainties = core_uncertainties, signals = all_station_pulse_strength, direction_uncertainties = direction_uncertainties, ldf_enable = True, footprint_enable = True, save_plots = True, plot_prefix = options.output_dir+"/"+"cr_physics-"+str(options.id)+"-")

# Add LDF and footprint plots to list of event level plots
plotlist.extend(ldf.plotlist)

# Add list of event level plots to event
event["plotfiles"] = ["/"+p.lstrip("./") for p in plotlist]

# Update event status
event.status = "CR_ANALYZED"
event.write()

print "[cr_physics] completed in {0:.3f} s".format(time.clock() - start)


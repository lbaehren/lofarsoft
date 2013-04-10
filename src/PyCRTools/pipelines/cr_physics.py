"""CR pipeline.
"""

import logging

# Log everything, and send it to stderr.
logging.basicConfig(level=logging.DEBUG)

import matplotlib
matplotlib.use("Agg")

import os
import sys
import time
import datetime
import pytmf
import numpy as np
import matplotlib.pyplot as plt
import pycrtools as cr
from pycrtools import crdatabase as crdb
from pycrtools import metadata as md
from pycrtools import tools
from pycrtools import lora

from pycrtools.tasks import antennaresponse
from pycrtools.tasks import findrfi
from pycrtools.tasks import shower
from pycrtools.tasks import galaxy
from pycrtools.tasks import minibeamformer
from pycrtools.tasks import directionfitplanewave
from pycrtools.tasks import pulseenvelope
#from pycrtools.tasks import stokesparameters
from pycrtools.tasks import wavefront
from pycrtools.tasks import directionfitbf

from optparse import OptionParser
from contextlib import contextmanager

# Error handling
class PipelineError(Exception):
    """Base class for pipeline exceptions."""

    def __init__(self, message, category="OTHER"):
        self.message = message
        self.category = category

class EventError(PipelineError):
    """Raised when an unhandlable error occurs at event level."""
    pass

class StationError(PipelineError):
    """Raised when an unhandlable error occurs at station level."""
    pass

class PolarizationError(PipelineError):
    """Raised when an unhandlable error occurs at polarization level."""
    pass

class Skipped(PipelineError):
    """Base class for everything that needs to lead to a skipped state."""
    pass

class EventSkipped(Skipped):
    """Raised when event is skipped."""
    pass

class StationSkipped(Skipped):
    """Raised when station is skipped."""
    pass

class PolarizationSkipped(Skipped):
    """Raised when polarization is skipped."""
    pass

@contextmanager
def process_event(event):
    start = time.clock()

    print "-- event {0}".format(event._id)

    event.status = "PROCESSING"
    event.statusmessage = ""
    event["crp_plotfiles"] = []

    for station in event.stations:
        station.status = "NEW"
        station.statusmessage = ""
        station["crp_plotfiles"] = []

        for p in station.polarization.keys():
            station.polarization[p].status = "NEW"
            station.polarization[p].statusmessage = ""
            station.polarization[p]["crp_plotfiles"] = []

    try:
        yield event
    except EventSkipped as e:
        logging.info("event skipped because: {0}".format(e.message))
        event.status = "SKIPPED"
        event.statusmessage = e.message
    except EventError as e:
        logging.exception(e.message)
        event.status = "ERROR"
        event.statusmessage = e.message
    except Exception as e:
        event.status = "ERROR"
        event.statusmessage = e.message
        raise
    except BaseException as e:
        event.status = "ERROR"
        event.statusmessage = "sigterm recieved"
        raise
    finally:
        event["last_processed"] = datetime.datetime.utcnow()
        event.write()
        print "-- event {0} completed in {1:.3f} s".format(event._id, time.clock() - start)

@contextmanager
def process_station(station):
    start = time.clock()

    print "-- station {0}".format(station.stationname)

    station.status = "PROCESSING"
    station.statusmessage = ""
    station.statuscategory = ""
    station["crp_plotfiles"] = []

    try:
        yield station
    except StationSkipped as e:
        logging.info("station skipped because: {0}".format(e.message))
        station.status = "SKIPPED"
        station.statusmessage = e.message
    except Skipped as e:
        logging.info("station processing aborted due to container skipping")
        station.status = "ABORTED"
        station.statusmessage = e.message
        station.statuscategory = e.category
        raise
    except StationError as e:
        logging.exception(e.message)
        station.status = "ERROR"
        station.statusmessage = e.message
    except Exception as e:
        station.status = "ERROR"
        station.statusmessage = e.message
        raise
    except BaseException as e:
        station.status = "ERROR"
        station.statusmessage = "sigterm recieved"
        raise
    finally:
        print "-- station {0} completed in {1:.3f} s".format(station.stationname, time.clock() - start)

@contextmanager
def process_polarization(polarization, *args):
    start = time.clock()

    print "-- polarization",
    for p in args:
        print p,
    print "\n"

    for p in args:
        polarization[p].status = "PROCESSING"
        polarization[p].statusmessage = ""
        polarization[p]["crp_plotfiles"] = []

    try:
        yield polarization
    except PolarizationSkipped as e:
        logging.info("polarization skipped because: {0}".format(e.message))
        for p in args:
            polarization[p].status = "SKIPPED"
            polarization[p].statusmessage = e.message
    except Skipped as e:
        logging.info("polarization processing aborted due to container skipping")
        for p in args:
            polarization[p].status = "ABORTED"
            polarization[p].statusmessage = e.message
        raise
    except PolarizationError as e:
        logging.exception(e.message)
        for p in args:
            polarization[p].status = "ERROR"
            polarization[p].statusmessage = e.message
    except Exception as e:
        for p in args:
            polarization[p].status = "ERROR"
            polarization[p].statusmessage = e.message
        raise
    except BaseException as e:
        for p in args:
            polarization[p].status = "ERROR"
            polarization[p].statusmessage = "sigterm recieved"
        raise
    finally:
        print "-- polarization",
        for p in args:
            print p,
        print "completed in {0:.3f} s".format(time.clock() - start)

# Don't write output to all tasks
cr.tasks.task_write_parfiles = False

# Parse commandline options
parser = OptionParser()
parser.add_option("-i", "--id", type="int", help="event ID", default=1)
parser.add_option("-b", "--blocksize", type="int", default=2 ** 16)
parser.add_option("-d", "--database", default="crdb.sqlite", help="filename of database")
parser.add_option("-o", "--output-dir", default="./", help="output directory")
parser.add_option("-s", "--station", action="append", help="only process given station")
parser.add_option("-a", "--accept_snr", type="int", default=5, help="accept pulses with snr higher than this in the beamformed timeseries")
parser.add_option("--maximum_nof_iterations", type="int", default=5, help="maximum number of iterations in antenna pattern unfolding loop")
parser.add_option("--maximum_angular_diff", type="float", default=0.5, help="maximum angular difference in direction fit iteration (in degrees), corresponds to angular resolution of a LOFAR station")
parser.add_option("--maximum_allowed_residual_delay", type="float", default=9e-8, help="average delay that is still allowed for a station to be called good")
parser.add_option("--maximum_allowed_outliers", type="float", default=4, help="number of outliers that can be ignored when calculating the average residuals")
parser.add_option("--broad_search_window_width", type="int", default=2 ** 14, help="width of window around expected location for first pulse search")
parser.add_option("--narrow_search_window_width", type="int", default=2 ** 7, help="width of window around expected location for subsequent pulse search")
parser.add_option("-l", "--lora_directory", default="./", help="directory containing LORA information")
parser.add_option("--lora_logfile", default="LORAtime4", help="name of LORA logfile with timestamps")
parser.add_option("--host", default=None, help="PostgreSQL host.")
parser.add_option("--user", default=None, help="PostgreSQL user.")
parser.add_option("--password", default=None, help="PostgreSQL password.")
parser.add_option("--dbname", default=None, help="PostgreSQL dbname.")
parser.add_option("--plot-type", default="png", help="Plot type (e.g. png, jpeg, pdf.")
parser.add_option("--use-cc-delay", default=False, action="store_true", help="Use cross correlation delays instead of Hilbert transform maxima when calculating direction.")
parser.add_option("--use-hanning-window", default=False, action="store_true", help="Apply Hanning window before FFT.")
parser.add_option("--debug", default=False, action="store_true", help="Generate additional plots for debugging.")

(options, args) = parser.parse_args()

db_filename = options.database
dbManager = crdb.CRDatabase(db_filename, host=options.host, user=options.user, password=options.password, dbname=options.dbname)
db = dbManager.db

start = time.clock()

# Get event from database
with process_event(crdb.Event(db=db, id=options.id)) as event:

    # Create output directory
    directory = os.path.join(options.output_dir, str(options.id))
    if not os.path.exists(directory):
        os.makedirs(directory)

    event_plot_prefix = os.path.join(directory, "cr_physics-{0}-".format(options.id))

    cr_found = False

    # Get first estimate of pulse direction
    try:
        lora_direction = list(event["lora_direction"])
        pulse_direction = lora_direction
    except KeyError:
        raise EventSkipped("have no lora_direction")

    # Create FFTW plans
    fftwplan = cr.FFTWPlanManyDftR2c(options.blocksize, 1, 1, 1, 1, 1, cr.fftw_flags.ESTIMATE)
    ifftwplan = cr.FFTWPlanManyDftC2r(options.blocksize, 1, 1, 1, 1, 1, cr.fftw_flags.ESTIMATE)

    # Loop over all stations in event
    stations = []
    for f in event.datafiles:
        stations.extend(f.stations)

    combined_antenna_positions = []
    combined_pulse_peak_amplitude = []
    combined_rms = []

    # Process only given stations if explicitly requested
    for station in stations:

        if options.station and station.stationname not in options.station:
            continue

        with process_station(station) as station:
            #Reset to LORA direction for initial values
            pulse_direction = lora_direction

            station_plot_prefix = event_plot_prefix + "{0}-".format(station.stationname)

            # Open file
            f = cr.open(station.datafile.settings.datapath + '/' + station.datafile.filename)

            # Check if we are dealing with LBA or HBA observations
            if "HBA" in f["ANTENNA_SET"]:
                hba = True
            elif "LBA" in f["ANTENNA_SET"]:
                hba = False
            else:
                raise EventSkipped("unsupported antennaset {0}".format(f["ANTENNA_SET"]))

            # Read LORA information
            tbb_time = f["TIME"][0]
            tbb_sample_number = max(f["SAMPLE_NUMBER"])

            print "reading LORA data"
            try:
                (tbb_time_sec, tbb_time_nsec) = lora.nsecFromSec(tbb_time, logfile=os.path.join(options.lora_directory, options.lora_logfile))

                (block_number_lora, sample_number_lora) = lora.loraTimestampToBlocknumber(tbb_time_sec, tbb_time_nsec, tbb_time, tbb_sample_number, blocksize=options.blocksize)
            except Exception:
                raise StationError("could not get expected block number from LORA data")
            print "have LORA data"

            # Center readout block around LORA pulse location
            shift = sample_number_lora - (options.blocksize / 2)
            pulse_search_window_start = (options.blocksize / 2) - (options.broad_search_window_width / 2)
            pulse_search_window_end = (options.blocksize / 2) + (options.broad_search_window_width / 2)

            print "shifting block by {0} samples to center lora pulse at sample {1}".format(shift, options.blocksize / 2)

            # Set file parameters to match LORA block
            f["BLOCKSIZE"] = options.blocksize
            f["BLOCK"] = block_number_lora
            try:
                f.shiftTimeseriesData(shift)
            except ValueError as e:
                raise StationError("{0}, signal is at edge of file".format(e.message))

            # Make plot of timeseries data in both polarizations of first selected antenna
            try:
                raw_data = f["TIMESERIES_DATA"].toNumpy()
            except RuntimeError as e:
                raise StationError("Cannot get the raw data, hdf5 problem detected: {0}".format(e.message))

            if raw_data.shape[0] < 2:
                raise StationError("File of station contains less than 2 antennas.")

            plt.subplot(2,1,1)
            plt.plot(raw_data[0])
            plt.subplot(2,1,2)
            plt.plot(raw_data[1])
            plotfile = station_plot_prefix + "raw_data.{0}".format(options.plot_type)
            plt.savefig(plotfile)
            station["crp_plotfiles"].append(plotfile)

            with process_polarization(station.polarization, '0', '1') as polarization:

                # Find RFI and bad antennas
                try:
                    if hba:
                        findrfi = cr.trun("FindRFI", f=f, nofblocks=10, freq_range=(110, 190), save_plots=True, plot_prefix=station_plot_prefix, plot_type=options.plot_type, plotlist=[], apply_hanning_window=options.use_hanning_window)
                    else:
                        findrfi = cr.trun("FindRFI", f=f, nofblocks=10, freq_range=(30, 80), save_plots=True, plot_prefix=station_plot_prefix, plot_type=options.plot_type, plotlist=[], apply_hanning_window=options.use_hanning_window)
                except ZeroDivisionError as e:
                    raise StationError("findrfi reports NaN in file {0}".format(e.message))

                print "blocks_with_dataloss", findrfi.blocks_with_dataloss

                for plot in findrfi.plotlist:
                    station["crp_plotfiles"].append(plot)

                # Select antennas which are marked good for both polarization
                fft_data = f.empty("FFT_DATA")
                f.getFFTData(fft_data, block_number_lora, options.use_hanning_window, datacheck=True)

                dipole_names = f["DIPOLE_NAMES"]

                station["crp_median_cleaned_spectrum"] = findrfi.median_cleaned_spectrum

                selected_dipoles = []
                for i in range(len(dipole_names) / 2):
                    if dipole_names[2 * i] in findrfi.good_antennas and dipole_names[2 * i + 1] in findrfi.good_antennas and f.nof_consecutive_zeros[2 * i] < 512 and f.nof_consecutive_zeros[2 * i + 1] < 512:
                        selected_dipoles.extend([dipole_names[2 * i], dipole_names[2 * i + 1]])

                try:
                    f["SELECTED_DIPOLES"] = selected_dipoles
                except ValueError as e:
                    raise StationError("Data problem, cannot read selected dipoles: {0}".format(e.message))

                station["crp_selected_dipoles"] = selected_dipoles

                # Read FFT data (without Hanning window)
                fft_data = f.empty("FFT_DATA")
                f.getFFTData(fft_data, block_number_lora, options.use_hanning_window, datacheck=True)

                print "NOF consecutive zeros", f.nof_consecutive_zeros

                # Get corresponding frequencies
                frequencies = cr.hArray(f["FREQUENCY_DATA"])

                # Normalize spectrum
                fft_data /= f["BLOCKSIZE"]

                # Reject DC component
                fft_data[..., 0] = 0.0

                # Also reject 1st harmonic (gives a lot of spurious power with Hanning window)
                fft_data[..., 1] = 0.0

                # Flag dirty channels (from RFI excission)
                fft_data[..., cr.hArray(findrfi.dirty_channels)] = 0
                station["crp_dirty_channels"] = findrfi.dirty_channels

                # Get integrated power / amplitude spectrum for each antenna
                antennas_cleaned_sum_amplitudes = cr.hArray([findrfi.antennas_cleaned_sum_amplitudes[i] for i in f["SELECTED_DIPOLES_INDEX"]])
                antennas_cleaned_power = cr.hArray([findrfi.antennas_cleaned_power[i] for i in f["SELECTED_DIPOLES_INDEX"]])

                station["crp_antennas_cleaned_sum_amplitudes"] = antennas_cleaned_sum_amplitudes
                station["crp_antennas_cleaned_power"] = antennas_cleaned_power

                # Normalize recieved power to that expected for a Galaxy dominated reference antenna
                galactic_noise = cr.trun("GalacticNoise", fft_data=fft_data, channel_width=f["SAMPLE_FREQUENCY"][0] / f["BLOCKSIZE"], timestamp=tbb_time, antenna_set=f["ANTENNA_SET"], original_power=antennas_cleaned_power)

                station["crp_galactic_noise"] = galactic_noise.galactic_noise_power

                # Apply calibration delays
                try:
                    cabledelays = cr.hArray(f["DIPOLE_CALIBRATION_DELAY"])
                except Exception:
                    raise StationSkipped("do not have DIPOLE_CALIBRATION_DELAY value")

                weights = cr.hArray(complex, dimensions=fft_data, name="Complex Weights")
                phases = cr.hArray(float, dimensions=fft_data, name="Phases", xvalues=frequencies)

                cr.hDelayToPhase(phases, frequencies, cabledelays)
                cr.hPhaseToComplex(weights, phases)

                fft_data.mul(weights)

                # Get timeseries data
                timeseries_data = f.empty("TIMESERIES_DATA")
                nantennas = timeseries_data.shape()[0] / 2

                # Get antennas positions
                antenna_positions = f["ANTENNA_POSITIONS"]

                # Beamform in LORA direction for both polarizations
                fft_data_0 = cr.hArray(complex, dimensions=(nantennas, options.blocksize / 2 + 1))
                fft_data_1 = cr.hArray(complex, dimensions=(nantennas, options.blocksize / 2 + 1))

                fft_data_0[...].copy(fft_data[0:nantennas:2, ...])
                fft_data_1[...].copy(fft_data[1:nantennas:2, ...])

                antenna_positions_one = cr.hArray(float, dimensions=(nantennas, 3))
                antenna_positions_one[...].copy(antenna_positions[0:nantennas:3, ...])

                mb0 = cr.trun("MiniBeamformer", fft_data=fft_data_0, frequencies=frequencies, antpos=antenna_positions_one, direction=pulse_direction)
                mb1 = cr.trun("MiniBeamformer", fft_data=fft_data_1, frequencies=frequencies, antpos=antenna_positions_one, direction=pulse_direction)

                beamformed_timeseries = cr.hArray(float, dimensions=(2, options.blocksize))

                print "calculating inverse FFT"

                cr.hFFTWExecutePlan(beamformed_timeseries[0], mb0.beamformed_fft, ifftwplan)
                cr.hFFTWExecutePlan(beamformed_timeseries[1], mb1.beamformed_fft, ifftwplan)

                print "starting pulse envelope"

                # Look for significant pulse in beamformed signal
                pulse_envelope_bf = cr.trun("PulseEnvelope", timeseries_data=beamformed_timeseries, pulse_start=pulse_search_window_start, pulse_end=pulse_search_window_end, nsigma=options.accept_snr, save_plots=True, plot_prefix=station_plot_prefix+"bf-", plot_type=options.plot_type, plotlist=[])
                polarization['0']['crp_plotfiles'].append(pulse_envelope_bf.plotlist[0])
                polarization['1']['crp_plotfiles'].append(pulse_envelope_bf.plotlist[1])

                polarization['0']['crp_bf_peak_amplitude'] = pulse_envelope_bf.peak_amplitude[0]
                polarization['1']['crp_bf_peak_amplitude'] = pulse_envelope_bf.peak_amplitude[1]

                polarization['0']['crp_bf_rms'] = pulse_envelope_bf.rms[0]
                polarization['1']['crp_bf_rms'] = pulse_envelope_bf.rms[1]

                polarization['0']['crp_bf_mean'] = pulse_envelope_bf.mean[0]
                polarization['1']['crp_bf_mean'] = pulse_envelope_bf.mean[1]

                cr_found_in_station = False
                if 0 in pulse_envelope_bf.antennas_with_significant_pulses:
                    cr_found_in_station = True
                    polarization['0'].status = "GOOD"
                    polarization['0'].statusmessage = ""
                else:
                    polarization['0'].status = "BAD"
                    polarization['0'].statusmessage = "no significant pulse found in beamformed signal"

                if 1 in pulse_envelope_bf.antennas_with_significant_pulses:
                    cr_found_in_station = True
                    polarization['1'].status = "GOOD"
                    polarization['1'].statusmessage = ""
                else:
                    polarization['1'].status = "BAD"
                    polarization['1'].statusmessage = "no significant pulse found in beamformed signal"

            # skip this station for further processing when no cosmic ray signal is found in the beamformed timeseries
            # in the LORA direction for at least one of the polarizations
            if cr_found_in_station:
                station.status = "GOOD"
                station.statusmessage = ""
            else:
                station.status = "BAD"
                station.statusmessage = "no significant pulse found in beamformed signal for either polarization"
                station.statuscategory = "no_bf_pulse"

                continue

            # Get pulse window
            station['crp_bf_pulse_position'] = int(pulse_envelope_bf.bestpos)
            pulse_start = pulse_search_window_start + int(pulse_envelope_bf.bestpos) - options.narrow_search_window_width / 2
            pulse_end = pulse_search_window_start + int(pulse_envelope_bf.bestpos) + options.narrow_search_window_width / 2

            print "now looking for pulse in narrow range between samples {0:d} and {1:d}".format(pulse_start, pulse_end)

            # Optionally plot raw data after calibrating
            if options.debug:
                td = timeseries_data.toNumpy()
                for i in range(td.shape[0]):
                    plt.figure()
                    plt.plot(td[i, pulse_start, pulse_end])
                    plt.title("Timeseries cut to pulse after calibration dipole {0}".format(i))
                    plotfile = station_plot_prefix + "calibrated_timeseries-{0}.{1}".format(i, options.plot_type)
                    plt.savefig(plotfile)
                    station["crp_plotfiles"].append(plotfile)

            # Start direction fitting loopim
            n = 0
            direction_fit_converged = False
            direction_fit_successful = True
            while True:

                # Unfold antenna pattern
                if hba:
                    # Get timeseries data
                    cr.hFFTWExecutePlan(timeseries_data[...], fft_data[...], ifftwplan)

                else:
                    antenna_response = cr.trun("AntennaResponse", instrumental_polarization=fft_data, frequencies=frequencies, direction=pulse_direction)

                    # Get timeseries data
                    cr.hFFTWExecutePlan(timeseries_data[...], antenna_response.on_sky_polarization[...], ifftwplan)

                # Run beamforming direction finder once
                # not to find the direction but to at least have one point for outer stations
                if n==0 and not hba:
                    fft_data_0[...].copy(antenna_response.on_sky_polarization[0:nantennas:2, ...])
                    fft_data_1[...].copy(antenna_response.on_sky_polarization[1:nantennas:2, ...])

                    dbf_theta = cr.trun("DirectionFitBF", fft_data=fft_data_0, frequencies=frequencies, antpos=antenna_positions, start_direction=lora_direction)
                    dbf_phi = cr.trun("DirectionFitBF", fft_data=fft_data_1, frequencies=frequencies, antpos=antenna_positions, start_direction=lora_direction)

                    timeseries_data_dbf = cr.hArray(float, dimensions=(2,dbf_theta.beamformed_timeseries.shape()[0]))
                    timeseries_data_dbf[0].copy(dbf_theta.beamformed_timeseries)
                    timeseries_data_dbf[1].copy(dbf_phi.beamformed_timeseries)
                
                    pulse_envelope_dbf = cr.trun("PulseEnvelope", timeseries_data=timeseries_data_dbf, pulse_start=pulse_start, pulse_end=pulse_end, resample_factor=16, save_plots=True, plot_prefix=station_plot_prefix+"dbf-", plot_type=options.plot_type, plotlist=station["crp_plotfiles"], extra=True)

                    station["crp_integrated_pulse_power_dbf"] = cr.hArray(pulse_envelope_dbf.integrated_pulse_power).toNumpy()
                    station["crp_integrated_noise_power_dbf"] = cr.hArray(pulse_envelope_dbf.integrated_noise_power).toNumpy()

                # Calculate delays using Hilbert transform
                if options.debug:
                    pulse_envelope = cr.trun("PulseEnvelope", timeseries_data=timeseries_data, pulse_start=pulse_start, pulse_end=pulse_end, resample_factor=16, npolarizations=2, save_plots=True, plot_prefix=station_plot_prefix+"pe-", plot_type=options.plot_type, plotlist=station["crp_plotfiles"])
                else:
                    pulse_envelope = cr.trun("PulseEnvelope", timeseries_data=timeseries_data, pulse_start=pulse_start, pulse_end=pulse_end, resample_factor=16, npolarizations=2)

                delays = pulse_envelope.delays

                # Use current direction if not enough significant pulses are found for direction fitting
                if len(pulse_envelope.antennas_with_significant_pulses) < 3:
                    if n == 0:
                        logging.info("less than 3 antennas with significant pulses in first iteration")
                        raise StationError("less than 3 antennas with significant pulses")
                    else:
                        logging.info("less than 3 antennas with significant pulses, using previous direction")
                        station["crp_pulse_direction"] = pulse_direction
                        station.statusmessage = "less than 3 antennas in last iteration"
                        break

                # Fit pulse direction

                direction_fit_plane_wave = cr.trun("DirectionFitPlaneWave", positions=antenna_positions, timelags=delays, good_antennas=pulse_envelope.antennas_with_significant_pulses, reference_antenna=pulse_envelope.refant, verbose=True)

                pulse_direction = direction_fit_plane_wave.meandirection_azel_deg

                print "Hilbert envelope direction:", direction_fit_plane_wave.meandirection_azel_deg

                # Check if fitting was succesful
                if direction_fit_plane_wave.fit_failed:
                    break

                # Check for convergence of iterative direction fitting loop
                if n > 0:
                    angular_diff = np.rad2deg(tools.spaceAngle(np.deg2rad((90 - last_direction[1])), np.deg2rad((90 - last_direction[0])), np.deg2rad((90 - pulse_direction[1])), np.deg2rad((90 - pulse_direction[0]))))

                    if angular_diff < options.maximum_angular_diff:
                        direction_fit_converged = True

                last_direction = pulse_direction

                n += 1
                if direction_fit_converged:
                    print "fit converged"
                    station["crp_pulse_direction"] = pulse_direction
                    break

                # Check if maximum number of iterations is reached (will avoid infinite loop)
                if n > options.maximum_nof_iterations:
                    print "maximum number of iterations reached"
                    station["crp_pulse_direction"] = pulse_direction
                    station.statusmessage = "maximum number of iterations reached"
                    break

                if hba:
                    station["crp_pulse_direction"] = pulse_direction
                    break

            # Check if result of planewave fit is reasonable
            residual_delays = direction_fit_plane_wave.residual_delays.toNumpy()
            residual_delays = np.abs(residual_delays)

            average_residual = residual_delays.mean()

            # Plot residual delays
            plt.clf()
            plt.plot(direction_fit_plane_wave.residual_delays.toNumpy(), "ro")
            plt.xlabel("Antenna number")
            plt.ylabel("Residual delay (s)")
            plotfile = station_plot_prefix + "residual_delay.{0}".format(options.plot_type)
            plt.savefig(plotfile)
            station["crp_plotfiles"].append(plotfile)

            # Add parameters
            station["crp_pulse_delay"] = delays.toNumpy().reshape((nantennas, 2))[:,pulse_envelope.strongest_polarization]
            print 'Strongest polarization = %d' % pulse_envelope.strongest_polarization
            station["crp_pulse_delay"] += float(block_number_lora * options.blocksize + max(f["SAMPLE_NUMBER"]) + shift + pulse_start) / f["SAMPLE_FREQUENCY"][0] + f["CLOCK_OFFSET"][0]
            print 'Station: %s' % station.stationname
            print 'BLock nr Lora = %d, max-min sample nr = %d, shift = %d, pulse_start = %d, clock offset = %f' % (block_number_lora, max(f["SAMPLE_NUMBER"]) - min(f["SAMPLE_NUMBER"]), shift, pulse_start, f["CLOCK_OFFSET"][0])
            station["crp_pulse_delay_fit_residual"] = direction_fit_plane_wave.residual_delays.toNumpy()

            itrf_antenna_positions = cr.hArray(f["ANTENNA_POSITION_ITRF"])
            itrf_antenna_positions.reshape((itrf_antenna_positions.shape()[0] / 3, 3))

            station["itrf_antenna_positions"] = itrf_antenna_positions.toNumpy()
            station["local_antenna_positions"] = md.convertITRFToLocal(itrf_antenna_positions).toNumpy()
            station["clock_offset"] = f["CLOCK_OFFSET"][0]

            if direction_fit_plane_wave.fit_failed:
                station.status = "BAD"
                station.statusmessage = "direction fit failed"
                station.statuscategory = "fit_failed"
                pulse_direction = list(event["lora_direction"])
                direction_fit_successful = False


            if direction_fit_plane_wave.goodcount < nantennas / 2:
                station.status = "BAD"
                station.statusmessage = "goodcount {0} < nantennas / 2 [= {1}]".format(direction_fit_plane_wave.goodcount, nantennas / 2)
                station.statuscategory = "good_count_ant"
                pulse_direction = list(event["lora_direction"])
                direction_fit_successful = False


            if average_residual > options.maximum_allowed_residual_delay:
                limited_residuals = residual_delays
                limited_residuals.sort()
                limited_residuals = limited_residuals[:-1.*options.maximum_allowed_outliers]
                limited_residuals_mean = limited_residuals.mean()
                if limited_residuals_mean > options.maximum_allowed_residual_delay:
                    print "direction fit residuals too large, average_residual = {0}, limited = {1}".format(average_residual,limited_residuals_mean)
                    station.status = "BAD"
                    station.statusmessage = "average_residual = {0}, limited = {1}".format(average_residual,limited_residuals_mean )
                    station.statuscategory = "average_residual"
                    pulse_direction = list(event["lora_direction"])
                    direction_fit_successful = False

                else:
                    print "direction fit limited residuals ok, average_residual = {0}, limited = {1}".format(average_residual,limited_residuals_mean)
            else:
                print "direction fit residuals ok, average_residual = {0}".format(average_residual)

            with process_polarization(station.polarization, 'xyz') as polarization:

                xyz_timeseries_data = cr.hArray(float, dimensions=(3 * nantennas, options.blocksize))
                cr.hProjectPolarizations(xyz_timeseries_data[0:3 * nantennas:3, ...], xyz_timeseries_data[1:3 * nantennas:3, ...], xyz_timeseries_data[2:3 * nantennas:3, ...], timeseries_data[0:2 * nantennas:2, ...], timeseries_data[1:2 * nantennas:2, ...], pytmf.deg2rad(pulse_direction[0]), pytmf.deg2rad(pulse_direction[1]))

                # Get Stokes parameters
#                stokes_parameters = cr.trun("StokesParameters", timeseries_data=xyz_timeseries_data, pulse_start=pulse_start, pulse_end=pulse_end, resample_factor=16)

                # Get pulse strength
                pulse_envelope_xyz = cr.trun("PulseEnvelope", timeseries_data=xyz_timeseries_data, pulse_start=pulse_start, pulse_end=pulse_end, resample_factor=16, npolarizations=3, save_plots=True, plot_prefix=station_plot_prefix, plot_type=options.plot_type, plotlist=polarization['xyz']["crp_plotfiles"], extra=True)

                # Do noise characterization
#                noise = cr.trun("Noise", timeseries_data=xyz_timeseries_data, histrange=(-3 * pulse_envelope_xyz.rms[0], 3 * pulse_envelope_xyz.rms[0]), save_plots=True, plot_prefix=station_plot_prefix, plot_type=options.plot_type, plotlist=polarization['xyz']["crp_plotfiles"])

                # Calculate time delay of pulse with respect to the start time of the file (e.g. f["TIME"])
                time_delays = pulse_envelope_xyz.pulse_maximum_time.toNumpy().reshape((nantennas, 3))
                time_delays += float(block_number_lora * options.blocksize + max(f["SAMPLE_NUMBER"])) / f["SAMPLE_FREQUENCY"][0] + f["CLOCK_OFFSET"][0]

                station["crp_pulse_time"] = time_delays

                polarization['xyz']["crp_pulse_peak_amplitude"] = cr.hArray(pulse_envelope_xyz.peak_amplitude).toNumpy().reshape((nantennas, 3))
                polarization['xyz']["crp_integrated_pulse_power"] = cr.hArray(pulse_envelope_xyz.integrated_pulse_power).toNumpy().reshape((nantennas, 3))
                polarization['xyz']["crp_integrated_noise_power"] = cr.hArray(pulse_envelope_xyz.integrated_noise_power).toNumpy().reshape((nantennas, 3))
                polarization['xyz']["crp_rms"] = cr.hArray(pulse_envelope_xyz.rms).toNumpy().reshape((nantennas, 3))
                polarization['xyz']["crp_stokes"] = pulse_envelope_xyz.stokes.toNumpy()
                polarization['xyz']["crp_polarization_angle"] = pulse_envelope_xyz.polarization_angle.toNumpy()

                # Exclude failed fits and average residuals and set polarization status
                if  direction_fit_successful == False:
                    polarization['xyz'].status = "BAD"
                    polarization['xyz'].statusmessage = "direction fit not successful"

                else:
                    polarization['xyz'].status = "GOOD"
                    polarization['xyz'].statusmessage = ""

                    cr_found = True

    if cr_found:

        # Get combined parameters from (cached) database
        all_station_antenna_positions = []
        all_station_pulse_delays = []
        all_station_fit_residuals = []
        all_station_pulse_peak_amplitude = []
        all_station_integrated_pulse_power = []
        all_station_integrated_noise_power = []
        all_station_rms = []
        all_station_direction = []
        all_station_names = []
        all_station_antennas_stationnames = []
        all_station_polarization_angle = []

        for station in stations:
            if station.status == "GOOD":
                try:
                    all_station_direction.append(station["crp_pulse_direction"])
                    all_station_pulse_delays.append(station["crp_pulse_delay"])# - station["clock_offset"])
                    all_station_fit_residuals.append(station["crp_pulse_delay_fit_residual"])
                    all_station_antenna_positions.append(station["local_antenna_positions"])
                    all_station_pulse_peak_amplitude.append(station.polarization['xyz']["crp_pulse_peak_amplitude"])
                    all_station_integrated_pulse_power.append(station.polarization['xyz']["crp_integrated_pulse_power"]*10**9) #scaling for plotting
                    all_station_integrated_noise_power.append(station.polarization['xyz']["crp_integrated_noise_power"]*11*10**9) #scaling for plotting and number of integrated samples
                    all_station_rms.append(station.polarization['xyz']["crp_rms"])
                    all_station_names.append(station.stationname)
                    all_station_antennas_stationnames.extend([station.stationname] * len(station["crp_pulse_delay"]) ) # gives station name for every antenna in the combined array
                    all_station_polarization_angle.append(station.polarization['xyz']["crp_polarization_angle"])
                except Exception as e:
                    raise EventError("{0} when attempting to obtain parameters for station {1}".format(e.message, station.stationname))

        all_station_antenna_positions = np.vstack(all_station_antenna_positions)
        all_station_pulse_delays = np.hstack(all_station_pulse_delays)
        all_station_pulse_delays -= all_station_pulse_delays.min() # Subtract global offset
        all_station_fit_residuals = np.hstack(all_station_fit_residuals)
        all_station_fit_residuals = all_station_fit_residuals[0::2] # only one delay per antenna, i.e. same for odd/even rcus
        all_station_pulse_peak_amplitude = np.vstack(all_station_pulse_peak_amplitude)
        all_station_integrated_pulse_power = np.vstack(all_station_integrated_pulse_power)
        all_station_integrated_noise_power = np.vstack(all_station_integrated_noise_power)
        all_station_rms = np.vstack(all_station_rms)
        all_station_direction = np.asarray(all_station_direction)
        all_station_polarization_angle = np.hstack(all_station_polarization_angle)
        all_station_antennas_stationnames = np.array(all_station_antennas_stationnames)

        # Convert to contiguous array of correct shape
        shape = all_station_antenna_positions.shape
        all_station_antenna_positions = all_station_antenna_positions.reshape((shape[0] / 2, 2, shape[1]))[:, 0]
        all_station_antenna_positions = all_station_antenna_positions.copy()

        # Calculate average direction and store it
        average_direction = tools.averageDirectionLOFAR(all_station_direction[:, 0], all_station_direction[:, 1])
        event["crp_average_direction"] = average_direction

        # Beamform with all stations

        # Compute LDF and footprint
        core = list(event["lora_core"])
        core_uncertainties = event["lora_coreuncertainties"].toNumpy()
        direction_uncertainties = [3., 3., 0]
        lora_positions = np.array(zip(event["lora_posx"],event["lora_posy"],event["lora_posz"]))
        lora_signals = event["lora_particle_density__m2"]
        lora_direction = list(event["lora_direction"])

        ldf = cr.trun("Shower", positions=all_station_antenna_positions, signals_uncertainties=all_station_rms, core=core, direction=average_direction, timelags=all_station_pulse_delays, core_uncertainties=core_uncertainties, signals=all_station_pulse_peak_amplitude, direction_uncertainties=direction_uncertainties, all_directions=all_station_direction, all_stations=all_station_names, ldf_enable=True, footprint_enable=True, footprint_use_background=True, skyplot_of_directions_enable=True, lora_direction=lora_direction, lora_positions=lora_positions, lora_signals=lora_signals, save_plots=True, plot_prefix=event_plot_prefix, plot_type=options.plot_type, plotlist=event["crp_plotfiles"])

        ldf_total = cr.trun("Shower", positions=all_station_antenna_positions, signals_uncertainties=all_station_rms, core=core, direction=average_direction, core_uncertainties=core_uncertainties, signals=all_station_pulse_peak_amplitude, direction_uncertainties=direction_uncertainties, all_directions=all_station_direction, all_stations=all_station_names, ldf_enable=True, ldf_total_signal=True, save_plots=True, plot_prefix=event_plot_prefix, plot_type=options.plot_type, plotlist=event["crp_plotfiles"])

        ldf_power = cr.trun("Shower", positions=all_station_antenna_positions, signals_uncertainties=all_station_integrated_noise_power, core=core, direction=average_direction, timelags=all_station_pulse_delays, core_uncertainties=core_uncertainties, signals=all_station_integrated_pulse_power, direction_uncertainties=direction_uncertainties, ldf_enable=True, ldf_integrated_signal=True, ldf_logplot=False, ldf_remove_outliers=False, footprint_enable=False, save_plots=True, plot_prefix=event_plot_prefix, plot_type=options.plot_type, plotlist=event["crp_plotfiles"])

        polarization_footprint = cr.trun("Shower",positions=all_station_antenna_positions,signals=all_station_pulse_peak_amplitude,footprint_enable=False,ldf_enable = False ,direction= average_direction,core = core,polarization_angle=all_station_polarization_angle,azimuth_in_distance_bins_enable=False,footprint_polarization_enable=True,save_plots=True, plot_prefix=event_plot_prefix, plot_type=options.plot_type, plotlist=event["crp_plotfiles"])

        # Plot wavefront shape using arrival times (from all_station_pulse_delays)
        # filter out all channels with > +/- 3 ns fit residual. This removes all 5-ns glitches as well as bad pulse position estimates.
        # Later we'll try to correct for glitches by collecting database values of fit residuals.
        noGlitchIndices = np.where( abs(all_station_fit_residuals) < 3e-9 )
#        print 'no glitch indices arE:'
#        print noGlitchIndices
        try:
            wavefront = cr.trun("Wavefront", arrivaltimes=all_station_pulse_delays[noGlitchIndices], positions=all_station_antenna_positions[noGlitchIndices], stationnames=all_station_antennas_stationnames[noGlitchIndices], loracore=core, save_plots=True, plot_prefix=event_plot_prefix,plot_type=options.plot_type, plotlist=event["crp_plotfiles"])
            # put into database: wavefront.fitPlanar = (az, el, mse) and wavefront.fitPointSource = (az, el, R, mse)
            event["wavefront_fit_planar"] = wavefront.fitPlaneWave
            event["wavefront_fit_pointsource"] = wavefront.fitPointSource
            event["wavefront_curvature_radius"] = wavefront.fitPointSource[2]
        except:
           print "wavefront returned problem"

        event.status = "CR_FOUND"
        event.statusmessage = ""

    else:

        event.status = "CR_NOT_FOUND"

print "[cr_physics] completed in {0:.3f} s".format(time.clock() - start)


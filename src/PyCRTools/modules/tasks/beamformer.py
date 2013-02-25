"""
Calculate complex beams towards multiple directions. Also calculates
the average spectrum in each beam and for an incoherent beam.

.. moduleauthor:: Name of the module author <email address of the module author>

Example::

  file=cr.open(LOFARSOFT+"/data/lopes/example.event")
  tpar antenna_positions=dict(zip(file["antennaIDs"],file.getCalData("Position")))
  tpar pointings=[dict(az=178.9*deg,el=28*deg),dict(az=0*deg,el=90*deg,r=1)]
  tpar cal_delays=dict(zip(file["antennaIDs"],file.getCalData("Delay")))
  tpar phase_center=[-84.5346,36.1096,0]
  tpar FarField=True
  tpar NyquistZone=2
  tpar randomize_peaks=False

  #file=cr.open(LOFARSOFT+"/data/lopes/2004.01.12.00:28:11.577.event")
  #file["SelectedAntennasID"]=[0]
  #fx0=file["TIMESERIES_DATA"]


  # ------------------------------------------------------------------------
  tload "BeamFormer"
  file=cr.open(LOFARSOFT+"/data/lopes/2004.01.12.00:28:11.577.event")
  tpar filefilter="$LOFARSOFT/data/lopes/2004.01.12.00:28:11.577.event"
  tpar antenna_positions=dict(map(lambda x: (x[0],x[1].array()),zip(file["antennaIDs"],file.getCalData("Position"))))
  tpar pointings=[dict(az=41.9898208*deg, el=64.70544*deg,r=1750),dict(az=0*deg,el=90*deg,r=100000)]
  tpar cal_delays=dict(zip(file["antennaIDs"],[0,-2.3375e-08,-2.75e-09,-3.75e-09,-2.525e-08,-2.575e-08,1.3125e-08,-1.6875e-08]))
  tpar phase_center=[-22.1927,15.3167,0]
  tpar FarField=False
  tpar NyquistZone=2
  tpar randomize_peaks=False
  # ------------------------------------------------------------------------
  antenna pos: cr.hArray(float, [8, 3], fill=[-84.5346,36.1096,0,-52.6146,54.4736,-0.0619965,-34.3396,22.5366,-0.131996,-2.3706,40.6976,-0.00299835,41.0804,1.97557,-0.0769958,22.7764,34.1686,-0.0549927,-20.8546,72.5436,-0.154999,11.1824,90.8196,-0.221992]) # len=24 slice=[0:24])

  self=Task
  self.beams[...,0].nyquistswap(self.NyquistZone)
  fxb=cr.hArray(float,[2,self.blocklen],name="TIMESERIES_DATA"); fxb[...].saveinvfftw(self.beams[...,0],1);  fxb.abs()
  fxb[...].plot(clf=True); cr.plt.show()

"""

# KNOWN BUGS (EE):
#
#   1) The modified tparameters are not saved after exiting python. (antennaIDs,cal_delays,..) .... the saved parameters are not read back when opening beamforming iteractibly.
#


import pycrtools as cr
from pycrtools import tasks
from pycrtools import qualitycheck
import time
import pytmf
import math
import os
import sys
import numpy as np
from pycrtools import metadata as md

import pdb
# pdb.set_trace()

"""
"""

cr.tasks.__raiseTaskDeprecationWarning(__name__)

# Defining the workspace parameters
deg = math.pi / 180.
pi2 = math.pi / 2.

# def makeGrid(AZ,EL,Distance,offset=5*deg):
#    return [dict(az=AZ-offset, el=EL+offset,r=Distance),dict(az=AZ, el=EL+offset,r=Distance),dict(az=AZ+offset, el=EL+offset,r=Distance),
#            dict(az=AZ-offset, el=EL,r=Distance),dict(az=AZ, el=EL,r=Distance),dict(az=AZ+offset, el=EL,r=Distance),
#            dict(az=AZ-offset, el=EL-offset,r=Distance),dict(az=AZ, el=EL-offset,r=Distance),dict(az=AZ+offset, el=EL-offset,r=Distance)]


def getfile(ws):
    """
    To produce an error message in case the file does not exist.
    """
    if ws.file_start_number < len(ws.filenames):
        f = cr.open(ws.filenames[ws.file_start_number])
        return f
    else:
        print "ERROR: File " + ws.filefilter + " not found!"
        return None

"""
  =========== ===== ========================================================
  *default*         contains a default value or a function that will be
                    assigned when the parameter is accessed the first time
                    and no value has been explicitly set. The function has
                    the form ``lambda ws: functionbody``, where ws is the
                    worspace itself, so that one can access other
                    parameters. E.g.: ``default:lambda ws: ws.par1+1`` so
                    that the default value is one larger than the value in
                    ``par1`` in the workspace.
  *doc*             A documentation string describing the parameter.
  *unit*            The unit of the value (for informational purposes only)
  *export*    True  If ``False`` do not export the parameter with
                    ``ws.parameters()`` or print the parameter
  *workarray* False If ``True`` then this is a workarray which contains
                    large amount of memory and is listed separately and
                    not written to file.
  =========== ===== ========================================================
"""


class BeamFormer(tasks.Task):
    """
    The function will calculate multiple beams for a list of files and
    a series of antennas (all integrated into one complex spectrum per
    beam).

    The task will do a basic quality check of each time series data
    set and only integrate good blocks.

    The desired frequency resolution is provided with the parameter
    delta_nu, but by default this will be rounded off to the nearest
    value using channel numbers of a power of 2. This will then set
    the block size for reading in the data. Multiple blocks can be
    read in one go (at least as soon as the new data reader supports
    this) that fit into one chunk of memory. The maximum length of the
    chunk can be set. If the filesize and number of blocks to read is
    larger than the chunksize, the chunk will be written to disk (and
    read back for adding the next antenna, which obviously is a slower
    process).

    The resulting beam is stored in the array ``Task.beam`` and written to
    disk as an cr.hArray with parameters stored in the header dict (use
    ``getHeader('BeamFormer')`` to retrieve this.)

    The incoherent and beamed average spectra are stored in
    ``Task.avspec_incoherent`` and ``Task.avspec`` respectively. They
    are also available as attributes to Task.bf.par (also when stored
    to disk).

    The beam can be FFTed back to time using ``Task.tcalc`` viewed with
    ``Task.tplot``.

    A dynamic spectrum can be calculated with ``Task.dyncalc``and viewed
    with ``Task.dynplot``.

    To avoid the spectrum being influenced by spikes in the time
    series, those spikes can be replaced by random numbers, before the
    FFT is taken (see 'randomize_peaks').

    The quality information (RMS, MEAN, flagged blocks per antennas)
    is stored in a data 'quality database' in text and python form and
    is also available as Task.quality. (See also:
    Task.antennacharacteristics, Task.mean, Task.mean_rms, Task.rms,
    Task.rms_rms, Task.npeaks, ``Task.npeaks_rms``,
    ``Task.homogeneity_factor`` - the latter is printed and is a useful
    hint if something is wrong)

    Flagged blocks can be easily inspeced using the task method
    ``Task.qplot`` (``qplot`` for quality plot).

    If you see an outputline like this::

        # Start antenna = 92 (ID= 17011092) - 4 passes:
        184 - Mean=  3.98, RMS=  6.35, Npeaks=  211, Nexpected=256.00 (Npeaks/Nexpected=  0.82), nsigma=  2.80, limits=( -2.80,   2.80)
        185 - Mean=  3.97, RMS=  6.39, Npeaks=  200, Nexpected=256.00 (Npeaks/Nexpected=  0.78), nsigma=  2.80, limits=( -2.80,   2.80)
        186 - Mean=  3.98, RMS=  6.40, Npeaks=  219, Nexpected=256.00 (Npeaks/Nexpected=  0.86), nsigma=  2.80, limits=( -2.80,   2.80)
        - count= 186, Block   514: mean=  0.25, rel. rms=   2.6, npeaks=   16, spikyness=  15.00, spikeexcess= 16.00   ['rms', 'spikeexcess']

    this will tell you that Antenna 17011092 was worked on (the 92nd
    antenna in the data file) and the 186th chunk (block 514)
    contained some suspicious data (too many spikes). If you want to
    inspect this, you can call::

        >>> Task.qplot(186)

    This will plot the chunk and highlight the first flagged block
    ``(#514)`` in that chunk::

        >>> Task.qplot(186,1)

    would highlight the second flagged block (which does not exist here).

    If the chunks are too long to be entirely plotted, use::

        >>> Task.qplot(186,all=False).

    CurrentModuleDeveloper:: Emilio Enriquez <e.enriquez 'at' astro.ru.nl>

    """

    parameters = dict(
# Beamformer parameters:
#------------------------------------------------------------------------
        pointings=dict(default=[dict(az=178.9 * deg, el=28 * deg), dict(az=0 * deg, el=90 * deg, r=1)],
                       doc="List of coordinate dicts (``{'az':azimuth,'el':elevation value}``) containing pointing directions for each beam on the sky."),

        cal_delays=dict(default={},
                        doc="A dict containing 'cable' delays for each antenna in seconds as values. Key is the antenna ID. Delays will be added to geometrical delays.",
                        unit="s"),

        phase_center=dict(default=[0, 0, 0],
                          doc="List or vector containing the *x*, *y*, *z* positions of the phase center of the array.",
                          unit="m"),

        FarField=dict(default=True,
                      doc="Form a beam towards the far field, i.e. no distance."),

        NyquistZone=dict(default=lambda self: self.datafile['NYQUIST_ZONE'][0],
                         doc="In which Nyquist zone was the data taken (e.g. ``NyquistZone = 2`` if data is from 100-200 MHz for 200 MHz sampling rate)."),

#------------------------------------------------------------------------
# Some standard parameters
#------------------------------------------------------------------------
        filefilter=dict(default="$LOFARSOFT/data/lofar/VHECR_example_short.h5",
                        doc="Unix style filter (i.e., with ``*``, ~, ``$VARIABLE``, etc.), to describe all the files to be processed."),

        file_start_number=dict(default=0,
                               doc="Integer number pointing to the first file in the ``filenames`` list with which to start. Can be changed to restart a calculation."),

        datafile=dict(default=getfile,
                      export=False,
                      doc="Data file object pointing to raw data."),

        doplot=dict(default=False,
                    doc="Plot current spectrum while processing."),

        dohanning=dict(default=True,
                       doc="Apply a hanning filter to the time series data before ffting, to avoid spectral leakage."),

        plotlen=dict(default=2 ** 12,
                     doc="How many channels ``+/-`` the center value to plot during the calculation (to allow progress checking)."),

        plotskip=dict(default=1,
                      doc="Plot only every ``plotskip``-th spectrum, skip the rest (should not be smaller than 1)."),

        plot_center=dict(default=0.5,
                         doc="Center plot at this relative distance from start of vector (0=left end, 1=right end)."),

        plot_pause=dict(default=True,
                          doc="Pause after every plot?"),

        plot_start=dict(default=lambda self: max(int(self.speclen * self.plot_center) - self.plotlen, 0),
                        doc="Start plotting from this sample number."),

        plot_end=dict(default=lambda self: min(int(self.speclen * self.plot_center) + self.plotlen, self.speclen),
                      doc="End plotting before this sample number."),

        delta_nu=dict(default=1,
                      doc="Desired frequency resolution - will be rounded off to get powers of 2x ``blocklen``. Alternatively set blocklen directly.",
                      unit="Hz"),

        maxnantennas=dict(default=96,
                          doc="Maximum number of antennas per file to sum over (also used to allocate some vector sizes)."),

        maxchunklen=dict(default=2 ** 20,
                         doc="Maximum length of one chunk of data that is kept in memory.",
                         unit="Samples"),

        maxnchunks=dict(default=lambda self: int(math.floor(self.filesize / self.sectlen)),
                        doc="Maximum number of spectral chunks to calculate."),

        maxblocksflagged=dict(default=2,
                              doc="Maximum number of blocks that are allowed to be flagged before the entire spectrum of the chunk is discarded."),

        stride=dict(default=1,
                    doc="If ``stride > 1`` skip (``stride - 1``) blocks."),

        file_ext=dict(default=".beam",
                      doc="Extension of filename",
                      export=False),

        filenames=dict(default=lambda self: cr.listFiles(self.filefilter),
                       doc="List of filenames of data file to read raw data from."),

        output_dir=dict(default="",
                        doc="Directory where output file is to be written to."),

        detail_name=dict(default='',
                         doc="Name extension to descrive a particular analysis (e.g., used if ``stride > 1``.)"),

        output_filename=dict(default=lambda self: (os.path.split(self.filenames[0])[1].strip('.h5') if len(self.filenames) > 0 else "unknown") + self.detail_name + self.file_ext,
                             doc="Filename (without directory, see ``output_dir``) to store the final spectrum."),

        spectrum_file=dict(default=lambda self: os.path.join(os.path.expandvars(os.path.expanduser(self.output_dir)), self.output_filename),
                           doc="Complete filename including directory to store the final spectrum."),

        qualitycheck=dict(default=False,
                          doc="Perform basic qualitychecking of raw data and flagging of bad data sets."),

        quality_db_filename=dict(default="qualitydatabase",
                                 doc="Root filename of log file containing the derived antenna quality values (uses '.py' and '.txt' extension)."),

        quality=dict(default=[],
                     doc="A list containing quality check information about every large chunk of data that was read in. Use ``Task.qplot(Entry#,flaggedblock=nn)`` to plot blocks in question.",
                     export=False,
                     output=True),

        antennacharacteristics=dict(default={},
                                    doc="A dict with antenna IDs as key, containing quality information about every antenna.",
                                    export=False,
                                    output=True),

        mean_antenna=dict(default=lambda self: (cr.hArray(float, [self.maxnantennas], name="Mean per Antenna") if self.qualitycheck else None),
                          doc="Mean value of time series per antenna.",
                          output=True),

        rms_antenna=dict(default=lambda self: (cr.hArray(float, [self.maxnantennas], name="RMS per Antenna") if self.qualitycheck else None),
                         doc="RMS value of time series per antenna.",
                         output=True),

        npeaks_antenna=dict(default=lambda self: (cr.hArray(float, [self.maxnantennas], name="Number of Peaks per Antenna") if self.qualitycheck else None),
                            doc="Number of peaks of time series per antenna.",
                            output=True),

        mean=dict(default=0,
                  doc="Mean of mean time series values of all antennas.",
                  output=True),

        mean_rms=dict(default=0,
                      doc="RMS of mean of mean time series values of all antennas.",
                      output=True),

        npeaks=dict(default=0,
                    doc="Mean of number of peaks all antennas.",
                    output=True),

        npeaks_rms=dict(default=0,
                        doc="RMS of ``npeaks`` over all antennas.",
                        output=True),

        rms=dict(default=0,
                 doc="Mean of RMS time series values of all antennas.",
                 output=True),

        rms_rms=dict(default=0,
                     doc="RMS of rms of mean time series values of all antennas.",
                     output=True),

        homogeneity_factor=dict(default=0,
                                doc="``=1-(rms_rms/rms+ npeaks_rms/npeaks)/2`` - this describes the homogeneity of the data processed. A ``homogeneity_factor = 1`` means that all antenna data were identical, a low factor should make one wonder if something went wrong.",
                                output=True),

        spikeexcess=dict(default=20,
                         doc="Set maximum allowed ratio of detected over expected peaks per block to this level (1 is roughly what one expects from Gaussian noise)."),

        rmsfactor=dict(default=2,
                       doc="Factor by which the RMS is allowed to change within one chunk of time series data before it is flagged."),

        meanfactor=dict(default=3,
                        doc="Factor by which the mean is allowed to change within one chunk of time series data before it is flagged."),

        randomize_peaks=dict(default=True,
                             doc="Replace all peaks in time series data which are ``rmsfactor`` above or below the mean with some random number in the same range."),

        peak_rmsfactor=dict(default=5,
                            doc="At how many sigmas above the mean will a peak be randomized."),

        nantennas_start=dict(default=lambda self: (self.antenna_list[0]  if isinstance(self.antenna_list, list) else 0),
                             doc="Start with the *n*-th antenna in each file (see also ``nantennas_stride``). Can be used for selecting odd/even antennas."),

        nantennas_stride=dict(default=1,
                              doc="Take only every *n*-th antenna from antennas list (see also ``nantennas_start``). Use 2 to select odd/even."),

        nspectraadded=dict(default=lambda self: cr.hArray(int, [self.nchunks], fill=0, name="Spectra added"),
                           doc="Number of spectra added per chunk.",
                           output=True),

        nspectraflagged=dict(default=lambda self: cr.hArray(int, [self.nchunks], fill=0, name="Spectra flagged"),
                             doc="Number of spectra flagged per chunk.",
                             output=True),

        antennas_used=dict(default=lambda self: set(),
                           doc="A set of antenna names that were actually included in the average spectrum, excluding the flagged ones.",
                           output=True),

        nantennas_total=dict(default=0,
                             doc="Total number of antennas that were processed (flagged or not) in this run.",
                             output=True),

        header=dict(default=lambda self: self.datafile.getHeader() if self.datafile else {},
                    doc="Header of datafile",
                    export=False),

        # Not in use.
        #        antenna_positions = dict(default=lambda self:dict(map(lambda x: (x[0],x[1].array()),zip(self.datafile["DIPOLE_NAMES"],self.datafile["ANTENNA_POSITION_ITRF"]))),
        #                                 doc="A dict containing *x*, *y*, *z*-Antenna positions for each antenna in seconds as values. Key is the antenna ID.",
        #                                 unit="m"),

        #*** Need to make this parameter an input parameter.
        blocklen=dict(default=lambda self: min(2 ** int(math.floor(math.log(1. / self.delta_nu / self.sample_interval, 2))), 2 ** int(math.floor(math.log(min(self.maxchunklen, self.filesize / self.stride))))),
                      doc="The size of a block used for the FFT, limited by filesize.",
                      unit="Sample"),

        test_beam_per_antenna=dict(default=False,
                                   doc="Save the all de antennas separatelly. Temporal parameter while task is under developement. NOTE: Some values are hardcoded!"),

        sample_offset=dict(default=0,
                           doc="Sample offset applied to time series. (ie. to set different stations to a common time.)",
                           unit="Sample"),

        #------------------------------------------------------------------------
        # Derived parameters

        nbeams=dict(default=lambda self: len(self.pointings),
                    doc="Number of beams to calculate."),

        phase_center_array=dict(default=lambda self: cr.hArray(list(self.phase_center), name="Phase Center", units=("", "m")),
                                doc="List or vector containing the *x*, *y*, *z* positions of the phase center of the array.",
                                unit="m"),

        antpos = dict(default=lambda self: cr.hArray(float, [3], units=("", "m")),
                      doc="Cartesian coordinates of the current antenna relative to the phase center of the array.",
                      unit="m"),

        stationpos = dict(default = lambda self: (md.getStationPositions(self.datafile['STATION_NAME'][0], self.antenna_set, True, coordinatesystem='ITRF') if self.station_centered else md.ITRFCS002),
                          doc = "ITRF coordinates of the phace center (usually center of current station).",
                          unit = "m"),

        block_duration=dict(default=lambda self: self.sample_interval * self.blocklen,
                            doc="The length of a block in time units.",
                            unit="s"),

        speclen=dict(default=lambda self: self.blocklen / 2 + 1,
                     doc="Length of one spectrum.",
                     unit="Channels"),

        sample_interval=dict(default=lambda self: self.datafile["SAMPLE_INTERVAL"][self.nantennas_start],
                             doc="Length in time of one sample in raw data set.",
                             unit="s"),

        filesize=dict(default=lambda self: self.datafile["MAXIMUM_READ_LENGTH"],
                      doc="Length of file for one antenna.",
                      unit="Samples"),

        fullsize=dict(default=lambda self: self.nblocks * self.blocklen * self.nchunks,
                      doc="The full length of the raw time series data used for the dynamic spectrum.",
                      unit="Samples"),

        delta_nu_used=dict(default=lambda self: 1 / (self.sample_interval * self.blocklen),
                           doc="Actual frequency resolution of dynamic spectrum",
                           unit="Hz"),

        max_nblocks=dict(default=lambda self: int(math.floor(self.filesize / self.stride / self.blocklen)),
                         doc="Maximum number of blocks in file."),

        chunklen=dict(default=lambda self: min(self.filesize / self.stride, self.maxchunklen),
                      doc="Length of one chunk of data treated in memory.",
                      unit="Samples"),

        nblocks=dict(default=lambda self: int(min(max(round(self.chunklen / self.blocklen), 1), self.max_nblocks)),
                     doc="Number of blocks of length blocklen integrated per spectral chunk. The blocks are also used for quality checking."),

        sectlen=dict(default=lambda self: self.chunklen * self.stride,
                     doc="Length of one section of data used to extract one chunk of data treated in memory.",
                     unit="Samples"),

        sectduration=dict(default=lambda self: self.sectlen * self.sample_interval,
                          doc="Length in time units of one section of data used to extract one chunk, i.e. on time step in dynamic spectrum.",
                          unit="s"),

        end_time=dict(default=lambda self: self.sectduration * self.nchunks,
                      doc="End of time axis.",
                      unit="s"),

        start_time=dict(default=lambda self: 0,
                        doc="Start of time axis.",
                        unit="s"),

        blocks_per_sect=dict(default=lambda self: self.nblocks * self.stride,
                             doc="Number of blocks per section of data."),

        nchunks=dict(default=lambda self: min(int(math.floor(self.filesize / self.sectlen)), self.maxnchunks),
                     doc="Maximum number of spectral chunks to average."),

        start_frequency=dict(default=lambda self: self.datafile["FREQUENCY_RANGE"][self.nantennas_start][0],
                             doc="Start frequency of spectrum.",
                             unit="Hz"),

        end_frequency=dict(default=lambda self: self.datafile["FREQUENCY_RANGE"][self.nantennas_start][1],
                           doc="End frequency of spectrum.",
                           unit="Hz"),

        delta_frequency=dict(default=lambda self: (self.end_frequency - self.start_frequency) / (self.speclen - 1.0),
                             doc="Separation of two subsequent channels in final spectrum.",
                             unit="Hz"),

        pointingsXYZ=dict(default=lambda self: cr.hArray(float, [self.nbeams, 3], fill=[item for sublist in [cr.convert(coords, "CARTESIAN") for coords in self.pointings] for item in sublist], name="Beam Direction XYZ"),
                          doc="Array of shape ``[nbeams,3]`` with *x*, *y*, *z* positions for each beam on the sky."),

        antenna_set = dict(default = lambda self: self.datafile['ANTENNA_SET'],
                           doc="Antenna Set",
                           unit=""),

        antenna_list=dict(default=lambda self: {},
                          doc="List of antenna indices used as input from each filename.",
                          output=True),

        rfi_channels=dict(default=None,
                          doc="RFI channels detected by previous code.",
                          output=True),

        #------------------------------------------------------------------------
        # Now define all the work arrays used internally

        data=dict(workarray=True,
                  doc="Main input array of raw data.",
                  default=lambda self: cr.hArray(float, [self.nblocks, self.blocklen], name="data", header=self.header)),

        fftdata=dict(workarray=True,
                     doc="Main input array of raw data.",
                     default=lambda self: cr.hArray(complex, [self.nblocks, self.speclen], name="fftdata", header=self.header)),

        #        avspec = dict(workarray=True,
        #                      doc="Average spectrum in each beam.",
        #                      default=lambda self:cr.hArray(float,[self.nblocks,self.nbeams,self.speclen],name="Average Spectrum",header=self.header,par=dict(logplot="y"),xvalues=self.frequencies)),

        #        avspec_incoherent = dict(workarray=True,
        #                                 doc="The average spectrum of all blocks in an incoherent beam (i.e. squaring before adding).",default=lambda self:
        #                                 cr.hArray(float,[self.speclen],name="Incoherent Average Spectrum",header=self.header,par=dict(logplot="y"),xvalues=self.frequencies)),

        #        tbeam_incoherent = dict(workarray=True,
        #                                doc="Contains the power as a function of time of an incorehent beam of all antennas (simply the sqaure of the ADC values added).",
        #                                default=lambda self: cr.hArray(float,[self.blocklen*self.nblocks],name="Incoherent Time Beam",header=self.header)),

        beams=dict(workarray=True,
                   doc="Output array containing the FFTed data for each beam.",
                   default=lambda self: cr.hArray(complex, [self.nblocks, self.nbeams, self.speclen], name="Beamed FFT", header=self.header, xvalues=self.frequencies)),

        phases=dict(workarray=True,
                    doc="Complex phases for each beam and each freqeuncy channel used to calculate complex weights for beamforming.",
                    default=lambda self: cr.hArray(float, [self.nbeams, self.speclen], name="Phases", header=self.header)),

        weights=dict(workarray=True,
                     doc="Complex weights for each beam and each freqeuncy channel used to calculate beams.",
                     default=lambda self: cr.hArray(complex, [self.nbeams, self.speclen], name="Weights", header=self.header)),

        delays=dict(workarray=True,
                    doc="Contains the geometric delays for the current antenna for each beam.",
                    default=lambda self: cr.hArray(float, [self.nbeams], name="Delays", header=self.header)),

        frequencies=dict(workarray=True,
                         doc="Frequency axis for the final power spectrum.",
                         default=lambda self: cr.hArray(float, [self.speclen], name="Frequency", units=("", "Hz"), header=self.header)),

        times=dict(workarray=True,
                   doc="Time axis for the final dynamic spectrum.",
                   default=lambda self: cr.hArray(float, [self.sectduration * self.nchunks / self.block_duration], name="Time", units=("", "s"), header=self.header)),


        nantennas=dict(workarray=True,
                       default=lambda self: len(self.antennas),
                       doc="The actual number of antennas available for calculation in the file (``< maxnantennas``)."),

        antennas=dict(workarray=True,
                      default=lambda self: cr.hArray(range(min(self.datafile["NOF_DIPOLE_DATASETS"], self.maxnantennas))),
                      doc="Antennas from which to select initially for the current file."),

        antennaIDs=dict(workarray=True,
                        default=lambda self: cr.ashArray(cr.hArray(self.datafile["DIPOLE_NAMES"])[self.antennas]),
                        doc="Antenna IDs to be selected from for current file."),

        newfigure=dict(workarray=True,
                       default=True,
                       doc="Create a new figure for plotting for each new instance of the task."),

        figure=dict(workarray=True,
                    default=None,
                    doc="The matplotlib figure containing the plot",
                    output=True),

        plotspec=dict(workarray=True,
                      default=True,
                      doc="If **True** plot the beamformed average spectrum at the end, otherwise the time series."),

        station_centered = dict(workarray = True,
                                default = True,
                                doc = "if True using the 'ANTENNA_POSITONS' (phase center at the center of the station), otherwise 'ANTENNA_POSITION'(phase center at the LOFAR center), info elsewere on their meanings.")
    )

    def run(self):
        """Run the program.
        """

        if len(self.pointings) > 1:
            sys.exit('Bugs using multiple beams, need to check code.')

        if len(self.filenames) > 1:
            sys.exit('The calibration needed for beamforming using multiple stations (files) is not complete.')

        self.quality = []
        self.antennas_used = set()
        self.antennacharacteristics = {}
        self.spectrum_file_bin = os.path.join(self.spectrum_file, "data.bin")
        self.dostride = (self.stride > 1)
        self.nspectraflagged.fill(0)
        self.nspectraadded.fill(0)
        self.count = 0
        self.NOF_DIPOLE_DATASETS = 0
        self.nantennas_total = 0
        self.beams.setHeader(FREQUENCY_INTERVAL=self.delta_frequency)
#        self.beams.par.avspec=self.avspec
#        self.beams.par.avspec_incoherent=self.avspec_incoherent
#        self.beams.par.tbeam_incoherent=self.tbeam_incoherent
        self.frequencies.fillrange((self.start_frequency), self.delta_frequency)
        self.updateHeader(self.beams, BEAM_NSPECTRAADDED="nspectraadded", BEAM_FILENAMES="filenames", BEAM_ANTENNAS_USED="antennas_used", BEAM_NCHUNKS="nchunks", BEAM_FREQUENCIES="frequencies", FREQUENCY_INTERVAL="delta_nu_used", FFTSIZE="speclen", BLOCKSIZE="blocklen", BEAM_FILENAME="spectrum_file")
        self.datafile['ANTENNA_SET'] = self.antenna_set

        dataok = True
        clearfile = True
        t0 = time.clock()  # ; print "Reading in data and doing a double FFT."
        fftplan = cr.FFTWPlanManyDftR2c(self.blocklen, 1, 1, 1, 1, 1, cr.fftw_flags.ESTIMATE)

        if self.doplot:
            cr.plt.ioff()
            if self.newfigure and not self.figure:
                self.figure = cr.plt.figure()

        # Quick.n.dirty code here, need to be able to automatically update parameters.
        if self.sample_offset:
            self.datafile.shiftTimeseriesData(sample_offset=self.sample_offset)  # This resets some parameters, used for now for recalculating self.nchunks
        if isinstance(self.antenna_list, list):
            self.datafile["SELECTED_DIPOLES"] = self.antenna_list
        self.filesize = self.datafile["MAXIMUM_READ_LENGTH"]
        self.nchunks = int(math.floor(self.filesize / self.sectlen))

        original_file_start_number = self.file_start_number
        self.beams.fill(0)
#        self.avspec.fill(0)
#        self.avspec_incoherent.fill(0)
#        self.tbeam_incoherent.fill(0)
        if self.dohanning:
            hannfilter = cr.hArray(float, self.blocklen)
            hannfilter.gethanningfilter()

        for fname in self.filenames[self.file_start_number:]:
            print "# Start File", str(self.file_start_number) + ":", fname
            self.ws.update(workarrays=False)  # Since the file_start_number was changed, make an update to get the correct file
            self.datafile["BLOCKSIZE"] = self.blocklen  # Setting initial block size
            if not self.antenna_list:
                self.antenna_list[fname] = range(self.nantennas_start, self.nantennas, self.nantennas_stride)
            if self.test_beam_per_antenna:
                test_beam = cr.hArray(Type=complex, dimensions=[len(self.antenna_list[fname]), self.speclen], name="test_BEAM")
                test_tbeam = cr.hArray(float, dimensions=[len(self.antenna_list[fname]), self.blocklen], name="test_TIMESERIES_DATA")
                count = 0
            if isinstance(self.antenna_list, list):
                self.antenna_list = {fname:self.antenna_list}
            for iantenna in self.antenna_list[fname]:
                antenna = self.antennas[iantenna]
                rms = 0
                mean = 0
                npeaks = 0
                self.antennaIDs = list(cr.ashArray(cr.hArray(self.datafile["DIPOLE_NAMES"])[self.antennas]))
                antennaID = self.antennaIDs[iantenna]
                self.datafile["SELECTED_DIPOLES"] = [antennaID]
                if self.sample_offset:
                    self.datafile.shiftTimeseriesData(sample_offset=self.sample_offset)  # Need to calculate this each time after having a new antenna selection.
                print "# Start antenna =", antenna, "(ID=", str(antennaID) + "):"
                if self.station_centered:
                    self.antpos = self.datafile.getRelativeAntennaPositionsNew()
                else:
                    self.antpos = md.convertITRFToLocal(self.datafile['ITRFANTENNA_POSITIONS'],reflonlat=None)
                self.antpos -= self.phase_center_array
                # print "Relative antenna position =",self.antpos

                # Calculate the geometrical delays needed for beamforming
                cr.hGeometricDelays(self.delays, self.antpos, self.pointingsXYZ, self.FarField)

                # Add the cable/calibration delay
                cal_delay = self.cal_delays.setdefault(antennaID, 0.0)
                # print "Delay =",cal_delay
                self.delays += cal_delay
                # print "Total delay =",self.delays

                self.phases.delaytophase(self.frequencies, self.delays)
                self.weights.phasetocomplex(self.phases)
                if self.test_beam_per_antenna:
                    self.nchunks = 19
                for nchunk in range(self.nchunks):
                    if self.test_beam_per_antenna:
                        nchunk = 18
                    print ' Working on chunk {0} out of {1} \r'.format(nchunk, self.nchunks),  # Prints the progress.
                    sys.stdout.flush()
                    blocks = range(nchunk * self.blocks_per_sect, (nchunk + 1) * self.blocks_per_sect, self.stride)
                    for block in blocks:
                        self.data[block - self.nblocks * nchunk].read(self.datafile, "TIMESERIES_DATA", [block])
                    if self.qualitycheck:
                        self.count = len(self.quality)
                        self.quality.append(qualitycheck.CRQualityCheckAntenna(self.data, datafile=self.datafile, normalize=False, spikeexcess=self.spikeexcess, spikyness=100000, rmsfactor=self.rmsfactor, meanfactor=self.meanfactor, chunk=self.count, blockoffset=nchunk * self.blocks_per_sect))
                        if not self.quality_db_filename == "":
                            qualitycheck.CRDatabaseWrite(self.quality_db_filename + ".txt", self.quality[self.count])
                        mean += self.quality[self.count]["mean"]
                        rms += self.quality[self.count]["rms"]
                        npeaks += self.quality[self.count]["npeaks"]
                        dataok = (self.quality[self.count]["nblocksflagged"] <= self.maxblocksflagged)
                    if not dataok:
                        print " # Data flagged!"
                        self.nspectraflagged[nchunk] += 1
                    else:
                        self.nspectraadded[nchunk] += 1
                        if not antennaID in self.antennas_used:
                            self.antennas_used.add(antennaID)
                            self.NOF_DIPOLE_DATASETS += 1
                        if self.qualitycheck and self.randomize_peaks and self.quality[self.count]["npeaks"] > 0:
                            lower_limit = self.quality[self.count]["mean"] - self.peak_rmsfactor * self.quality[self.count]["rms"]
                            upper_limit = self.quality[self.count]["mean"] + self.peak_rmsfactor * self.quality[self.count]["rms"]
                            self.data.randomizepeaks(lower_limit, upper_limit)
                        if self.dohanning:
                            self.data[...] *= hannfilter
                        cr.hFFTWExecutePlan(self.fftdata[...], self.data[...], fftplan)
                        self.fftdata[...].nyquistswap(self.NyquistZone)
#                        self.avspec_incoherent.spectralpower2(self.fftdata[...])
#                        self.tbeam_incoherent.squareadd(self.data)
                        self.fftdata /= float(self.nspectraadded[nchunk])  # Used for averanging the antennas. 1/N part.   (the x/(x+1) part comes from a few lines below)
                        if self.NOF_DIPOLE_DATASETS == 1:
                            self.beams.fill(0)
                        elif self.nchunks > 1:  # If not working on the 1st antenna, and if using several chunks, need to load self.beams with the values from previous antennas.
                            self.beams.readfilebinary(self.spectrum_file_bin, nchunk * self.speclen * self.nbeams * self.nblocks)
                        self.beams *= (self.nspectraadded[nchunk] - 1.0) / (self.nspectraadded[nchunk])  # Used for averanging the antennas. N-1/N part.
                        self.beams[...].muladd(self.weights, self.fftdata[...])  # Used for averanging the antennas. (1/N) + (N-1/N), and apply weights to fftdata.
#                        if iantenna == len(self.antenna_list[fname])-1:
#                        self.avspec[...].spectralpower2(self.beams[...]) #Debug# This is now done in self.dyncalc, no need for it here...
                        self.beams.write(self.spectrum_file, nblocks=self.nchunks, block=nchunk, writeheader=False, clearfile=clearfile, ext=self.file_ext)
                        clearfile = False
                        if self.test_beam_per_antenna and nchunk == 18:
                            test_beam[count] = self.beams[238]  # 238 since was checking in CR file (block 5998, file: L28348_D20110612T231913.199Z_CS002_R000_tbb.h5)
                            count += 1
                        # print "#  Time:",time.clock()-t0,"s for processing this chunk. Number of spectra added =",self.nspectraadded
#                        if self.doplot>2 and self.nspectraadded[nchunk]%self.plotskip==0:
#                            self.avspec[...,self.plot_start:self.plot_end].plot()
#                            print "RMS of plotted spectra=",self.avspec[...,self.plot_start:self.plot_end].stddev()
#                        if self.doplot>1:
#                            self.avspec[...,0,:].plot();self.plotpause()
                     # End for nchunk
                print "# End   antenna =", antenna, " Time =", time.clock() - t0, "s  nspectraadded =", self.nspectraadded.sum(), "nspectraflagged =", self.nspectraflagged.sum()
                if self.qualitycheck:
                    mean /= self.nchunks
                    rms /= self.nchunks
                    self.mean_antenna[self.nantennas_total] = mean
                    self.rms_antenna[self.nantennas_total] = rms
                    self.npeaks_antenna[self.nantennas_total] = npeaks
                    if not self.quality_db_filename == "":
                        self.antennacharacteristics[antennaID] = {"mean": mean, "rms": rms, "npeaks": npeaks, "quality": self.quality[-self.nchunks:]}
                        print "#          mean =", mean, "rms =", rms, "npeaks =", npeaks
                        f = open(self.quality_db_filename + ".py", "a")
                        f.write('antennacharacteristics["' + str(antennaID) + '"]=' + str(self.antennacharacteristics[antennaID]) + "\n")
                        f.close()
                self.nantennas_total += 1
                # End for iantenna
            self.datafile["SELECTED_DIPOLES"] = list(self.antennas_used)
            self.header = self.datafile.getHeader()
            self.updateHeader(self.beams, BEAM_NSPECTRAADDED="nspectraadded", BEAM_FILENAMES="filenames", BEAM_ANTENNAS_USED="antennas_used")
            self.beams.writeheader(self.spectrum_file, nblocks=self.nchunks, block=nchunk, clearfile=clearfile, ext=self.file_ext)
            print "# End File", str(self.file_start_number) + ":", fname
#            self.avspec /= self.nspectraadded
#            self.avspec_incoherent /= self.nspectraadded
#            self.tbeam_incoherent /= self.nspectraadded
            self.file_start_number += 1
        self.file_start_number = original_file_start_number  # Resets to original value, so that the parameter file is correctly written.
        if self.qualitycheck:
            self.mean = cr.asvec(self.mean_antenna[:self.nantennas_total]).mean()
            self.mean_rms = cr.asvec(self.mean_antenna[:self.nantennas_total]).stddev(self.mean)
            self.rms = cr.asvec(self.rms_antenna[:self.nantennas_total]).mean()
            self.rms_rms = cr.asvec(self.rms_antenna[:self.nantennas_total]).stddev(self.rms)
            self.npeaks = cr.asvec(self.npeaks_antenna[:self.nantennas_total]).mean()
            self.npeaks_rms = cr.asvec(self.npeaks_antenna[:self.nantennas_total]).stddev(self.npeaks)
            self.homogeneity_factor = 1 - (self.npeaks_rms / self.npeaks + self.rms_rms / self.rms) / 2. if self.npeaks > 0 else 1 - (self.rms_rms / self.rms)
            print "Mean values for all antennas: Task.mean =", self.mean, "+/-", self.mean_rms, "(Task.mean_rms)"
            print "RMS values for all antennas: Task.rms =", self.rms, "+/-", self.rms_rms, "(Task.rms_rms)"
            print "NPeaks values for all antennas: Task.npeaks =", self.npeaks, "+/-", self.npeaks_rms, "(Task.npeaks_rms)"
            print "Quality factor =", self.homogeneity_factor * 100, "%"
        print "Finished - total time used:", time.clock() - t0, "s."
        print "To inspect flagged blocks, used 'Task.qplot(Nchunk)', where Nchunk is the first number in e.g. '184 - Mean=  3.98, RMS=...'"
        print "To read back the beam formed data type: bm=cr.hArrayRead('" + self.spectrum_file + ',ext=' + self.file_ext + "')"
        print "To calculate or plot the invFFTed times series of one block, use 'Task.tcalc(bm)' or 'Task.tplot(bm)'."
        if self.doplot:
            self.tplot(plotspec=self.plotspec)
            self.plotpause()
            cr.plt.ion()
        if self.test_beam_per_antenna:
            test_tbeam[...].invfftw(test_beam[...])
            test_tbeam /= self.blocklen
            test_tbeam[...].plot()
        self.antenna_list = {}

    def tplot(self, beams=None, block=0, chunk=0, NyquistZone=1, doabs=True, smooth=0, mosaic=True, plotspec=False, xlim=None, ylim=None, recalc=False):
        """
        Take the result of the BeamForm task, i.e. an array of
        dimensions ``[self.nblocks,self.nbeams,self.speclen]``, do a
        NyquistSwap, if necessary, and then calculate an inverse
        FFT. If the time series (``self.tbeams``) has already been
        caclulated it will only be recalculated if explicitly asked
        for with ``recalc=True``.

        ============= ===== ===================================================================
        *beams*       None  Input array. Take self.beams from task, if None.
        *block*       0     Which  block to plot, or Ellipsis ('...') for all.
        *chunk*       0     Which chunk to plot. If new chunk, need "recalc=True".
        *NyquistZone* 1     NyquistZone=2,4,... means flip the data left to right before FFT.
        *doabs*       True  Take the absolute of the timeseries before plotting.
        *smooth*      0     If > 0 smooth data with a Gaussian kernel of that size.
        *mosaic*      True  Plots each beam in a different plot, False to overplot.
        *plotspec*    False If True plot the average spectrum instead.
        *xlim*              Tuple with minimum and maximum limits for the *x*-axis.
        *ylim*              Tuple with minimum and maximum limits for the *y*-axis.
        *recalc*      False If true force a recalculation of the time series beam if it exists.
        ============= ===== ===================================================================

        The final time series (all blocks) is stored in ``Task.tbeams``.
        """

        if block > self.nblocks - 1 or block < 0:
            raise ValueError('Block value out of range.')
        if chunk > self.maxnchunks - 1 or chunk < 0:
            raise ValueError('Chunk value out of range.')
        self.spectrum_file_bin = os.path.join(self.spectrum_file, "data.bin")
        if not beams:
            self.beams.readfilebinary(self.spectrum_file_bin, chunk * self.speclen * self.nbeams * self.nblocks)
        else:
            self.beams = beams
        hdr = self.beams.getHeader()
        if plotspec:
            spec = cr.hArray(float, [self.nbeams, self.speclen], name="Frequency Spectrum", header=hdr, par=dict(logplot="y"), xvalues=self.frequencies)
            spec.spectralpower2(self.beams[block, ...])
        if "BeamFormer" in hdr:
            if not NyquistZone:
                NyquistZone = hdr["BeamFormer"]["NyquistZone"]
        if not plotspec and (not hasattr(self, "tbeams") or recalc):
            self.tcalc(beams=self.beams, NyquistZone=NyquistZone, doabs=doabs, smooth=smooth)

        if not ylim and not plotspec:
            ylim = (self.tbeams[block, ...].min().val(), self.tbeams[block, ...].max().val())
        if mosaic and self.nbeams > 1:
            npanels = self.beams.shape()[-2]
            width = int(math.ceil(math.sqrt(npanels)))
            height = int(math.ceil(npanels / float(width)))
            cr.plt.clf()
            for n in range(npanels):
                cr.plt.subplot(width, height, n + 1)
                if plotspec:
                    spec[n].plot(clf=False, title=str(n) + ".", xlim=xlim, ylim=ylim)
                else:
                    self.tbeams[block, n].plot(clf=False, title=str(n) + ".", xlim=xlim, ylim=ylim)
        else:
            if plotspec:
                spec[...].plot(clf=True, xlim=xlim, ylim=ylim)
            else:
                self.tbeams[block, ...].plot(clf=True, xlim=xlim, ylim=ylim)

    def tcalc(self, beams=None, block=0, chunk=0, NyquistZone=1, doabs=False, smooth=0):
        """
        Calculate the time series after beamforming, i.e. take the
        result of the BeamForm task, i.e. an array of dimensions
        [self.nblocks,self.nbeams,self.speclen], do a NyquistSwap, if
        necessary, and then an inverse FFT.

        ============= ===== ===================================================================
        *beams*       None  Input array. Take self.beams from task, if None.
        *NyquistZone* 1     NyquistZone=2,4,... means flip the data left to right before FFT.
        *doabs*       False Take the absolute of the timeseries, if True.
        *smooth*      0     If > 0 smooth data with a Gaussian kernel of that size.
        ============= ===== ===================================================================

        The final time series (all blocks) is stored in ``Task.tbeams`` and returned.
        """

        if block > self.nblocks - 1 or block < 0:
            raise ValueError('Block value out of range.')
        if chunk > self.maxnchunks - 1 or chunk < 0:
            raise ValueError('Chunk value out of range.')
        if not beams:
            self.beams.readfilebinary(self.spectrum_file_bin, chunk * self.speclen * self.nbeams * self.nblocks)
            beams = self.beams
        hdr = beams.getHeader()
        if "BeamFormer" in hdr:
            if not NyquistZone:
                NyquistZone = hdr["BeamFormer"]["NyquistZone"]
        dim = beams.shape()
        blocklen = (dim[-1] - 1) * 2
        self.beamscopy = cr.hArray(dimensions=[dim[-3] * dim[-2], dim[-1]], copy=beams)
        self.beamscopy[...].nyquistswap(self.NyquistZone)
        self.tbeams2 = cr.hArray(float, [dim[-3] * dim[-2], blocklen], name="TIMESERIES_DATA")
        self.tbeams = cr.hArray(self.tbeams2.vec(), [dim[-3], dim[-2], blocklen], name="TIMESERIES_DATA")
        self.tbeams2[...].invfftw(self.beamscopy[...])
        self.tbeams /= blocklen
        if doabs:
            self.tbeams.abs()
        if smooth > 0:
            self.tbeams[...].runningaverage(smooth, cr.hWEIGHTS.GAUSSIAN)
        if "SAMPLE_INTERVAL" in hdr:
            dt = beams.getHeader("SAMPLE_INTERVAL")[self.nantennas_start]
            self.tbeams.par.xvalues = cr.hArray(float, [blocklen], name="Time", units=("", "s"))
            self.tbeams.par.xvalues.fillrange(-(blocklen / 2) * dt, dt)
            self.tbeams.par.xvalues.setUnit("mu", "")
        return self.tbeams

    def dynplot(self, dynspec=None, cleandynspec=None, plot_cleanspec=False, dmin=None, dmax=None, cmin=None, cmax=None, from_file=False, filename=None):
        """
        Plot the dynamic beamformed spectrum. Provide the dynamic spectrum of the beamformed.

        ================ ===== ===================================================================
        *dynspec*        None  Array with dynamic spectrum.
        *cleandynspec*   None  Array with cleaned dynamic spectrum.
        *plot_cleanspec* False If True it plots the cleaned spectrum in linear.
        *dmin*                 Minimum z-value (intensity) in dynamic spectrum to plot
        *dmax*                 Maximum z-value (intensity) in dynamic spectrum to plot
        *cmin*                 Minimum z-value (intensity) in clean dynamic spectrum to plot
        *cmax*                 Maximum z-value (intensity) in clean dynamic spectrum to plot
        *from_file*      False Read from file.
        *filename*       None  Filename of the beam.
        ================ ===== ===================================================================

        Example::

            tpar filenames=['File_path/File_name_tbb.h5']
            tpar blocklen=2**10
            tpar maxchunklen=2**16*5
            Task.dynplot(dmin=-10e1,dmax=10e2)

        or::

            tpar filenames=['File_path/File_name_tbb.h5']
            tpar blocklen=2**10
            tpar maxchunklen=2**16*5
            dynspec=Task.dyncalc()
            Task.dynplot(dynspec=dyspec,dmin=-10e1,dmax=10e2)

        """

        if filename:
            self.spectrum_file = filename

        if cleandynspec:
            plot_cleanspec = True

        if not dynspec:
            if from_file:
                self.spectrum_file_bin = os.path.join(self.spectrum_file, "dynspec")
                self.dynspec = cr.hArrayRead(self.spectrum_file_bin, ext=self.file_ext)
                if plot_cleanspec:
                    self.spectrum_file_bin = os.path.join(self.spectrum_file, "clean_dynspec")
                    self.cleandynspec = cr.hArrayRead(self.spectrum_file_bin, ext=self.file_ext)
            else:
                print 'CAUTION: Using the default options in self.dyncalc()'
                if plot_cleanspec:
                    self.dyncalc(clean=True)
                else:
                    self.dyncalc()

            dynspec = self.dynspec
            if plot_cleanspec:
                cleandynspec = self.cleandynspec
        else:
            self.dynspec = dynspec
            if plot_cleanspec:
                if not cleandynspec:
                    raise ValueError('Need to provide cleandynspec if plot_clenspec=True.')
                self.cleandynspec = cleandynspec

        cr.plt.clf()
        if plot_cleanspec:
            cr.plt.subplot(1, 2, 1)
        extra = '[log10]'
        cr.plt.title("Dynamic Spectrum %s " % extra)
        cr.plt.imshow(np.log10(cr.hArray_toNumpy(dynspec)), aspect='auto', origin='lower', vmin=dmin, vmax=dmax, cmap=cr.plt.cm.hot, extent=(self.start_time, self.end_time, self.start_frequency / 10 ** 6, self.end_frequency / 10 ** 6))
        cr.plt.ylabel("Frequency [MHz]")
        cr.plt.xlabel("+/- Time [s]")

        if plot_cleanspec:
            cr.plt.subplot(1, 2, 2)
            cr.plt.imshow(cr.hArray_toNumpy(cleandynspec), aspect='auto', origin='lower', vmin=cmin, vmax=cmax, cmap=cr.plt.cm.hot, extent=(self.start_time, self.end_time, self.start_frequency / 10 ** 6, self.end_frequency / 10 ** 6))
            extra = '[linear]'
            cr.plt.title("Cleaned Dynamic Spectrum %s " % extra)
            cr.plt.ylabel("Frequency [MHz]")
            cr.plt.xlabel("+/- Time [s]")
            print "cleandynspec: min=", self.cleandynspec.min().val(), "max=", self.cleandynspec.max().val(), "rms=", self.cleandynspec.stddev().val()

    def dynDM(self, cleandynspec=None, DM=None, Ref_Freqs=None, from_file=False, verbose=False, save_file=False):
        """
        Do dedispersion by integer shifting.
        Calculate dedispersed time series.

        ============== ===== ===================================================================
        *cleandynspec* None  Array with cleaned dynamic spectrum.
        *DM*           None  Dispersion Measure.
        *Ref_Freqs*    None  List with the begin and end of reference frequencies. [f1,f2] in Hz
        *from_file*    False Read cleandynspec from file.
        *verbose*      False If true then prints extra information, plot the dedispersed dynspec, and calculates/plots the time series.
        *save_file*    False Saves a file in hArray format with additional information besides the array values.
        ============== ===== ===================================================================

        Example::

            dedispersed_dynspec = Task.dynDM(cleandynspec=cleandynspec,DM=26.83,Ref_Freqs=[151e6,170e6])

        or::

            Task.dynDM(DM=26.83,Ref_Freqs=[151e6,170e6],from_file=True,verbose=True)

        Needs a cleaned dynamic spectrum as input file.
        """
        #------------------------
        # Checking imput is corret.
        if not cleandynspec:
            if from_file:
                self.spectrum_file_bin = os.path.join(self.spectrum_file, "clean_dynspec")
                self.cleandynspec = cr.hArrayRead(self.spectrum_file_bin, ext=self.file_ext)
                self.times = self.cleandynspec.par.xvalues
                self.frequencies = self.cleandynspec.par.yvalues
            else:
                print 'CAUTION: Using the default options in self.dyncalc()'
                self.dyncalc(clean=True)
            cleandynspec = self.cleandynspec
        else:
            self.cleandynspec = cleandynspec

        if not Ref_Freqs:
            Ref_Freqs = [self.frequencies[int(len(self.frequencies) / 4)], self.frequencies[int(3 * len(self.frequencies) / 4)]]  # Taking the middle half.
        else:
            if not isinstance(Ref_Freqs, type([])) or len(Ref_Freqs) != 2 or Ref_Freqs[0] > Ref_Freqs[1] or Ref_Freqs[0] < 0 or Ref_Freqs[1] < 0:
                raise ValueError('Need a list of lenght 2, with first element "<" or "=" second element. Both elements positive.')

        #------------------------
        # Some more parameters and variables.
        dedispersed_dynspec = cr.hArray(float, cleandynspec, fill=0)
        dt = self.block_duration * self.cleandynspec.par.tbin

        # Channel of reference frequency
        nref = self.frequencies.Find(">", Ref_Freqs[0])[0]
        nref2 = self.frequencies.Find(">", Ref_Freqs[1])[0]
        if verbose:
            print "Reference channels=", nref, nref2

        # Calculate the relative shifts in samples per frequency channels
        # Constant value comes from "Handbook of Pulsar Astronomy - by Duncan Ross Lorimer , Section 4.1.1, pagina 86 (in google books)"
        shifts = (4.148808e-3 * DM / dt) * 1e9 ** 2 * (Ref_Freqs[0] ** -2 - self.frequencies ** -2)

        # Integer offsets to reference frequency (shift to center)
        offsets = cr.Vector(int, self.frequencies, fill=shifts)
        offsets += self.times.shape()[0] / 2

        # Print roughly the shift at the end of the pulse around Ref_Freqs[1]
        if verbose:
            print "Sample shift at reference frequency (", Ref_Freqs[0], "):", shifts[nref]
            print "Sample shift at 2nd reference frequency (", Ref_Freqs[1], "):", shifts[nref2]

        # Now do the actual dedispersion by integer shifting ... that's all
        dedispersed_dynspec[...].shift(cleandynspec[...], offsets.vec())

        if verbose:
            # Plotting of dedispersed dynamic spectrum
            cr.plt.clf()
            cr.plt.title("Dedispersed Dynamic Spectrum")
            cr.plt.imshow(cr.hArray_toNumpy(dedispersed_dynspec), aspect='auto', cmap=cr.plt.cm.hot, origin='lower', vmin=1e-5, vmax=0.003, extent=(self.start_time, self.end_time, self.start_frequency / 10 ** 6, self.end_frequency / 10 ** 6))
            cr.plt.ylabel("Frequency [MHz]")
            cr.plt.xlabel("Time [s]")
            k = raw_input("Press return to continue ...")

            # Calculate dedispersed time series by collapsing all frequencies onto time axis
            dedispersed_timeseries = cr.hArray(float, self.times, fill=0, name="Power", xvalues=self.times)
            dedispersed_dynspec[...].addto(dedispersed_timeseries)

            # plotting of time series
            dedispersed_timeseries.plot(clf=True)

        # Adding parameters
        dedispersed_dynspec.par.yvalues = self.frequencies
        dedispersed_dynspec.par.xvalues = self.times
        if save_file:
            dedispersed_dynspec.write(os.path.join(self.spectrum_file, "dedispersed_dynspec"), nblocks=1, block=0, clearfile=True, ext=self.file_ext)
            print 'Saving binary in %s' % os.path.join(self.spectrum_file, "dedispersed_dynspec" + self.file_ext)

        self.cleandynspec = cleandynspec
        return dedispersed_dynspec

    def dyn_DM_SNR(self, dedispersed_dynspec, pulse_time=None, pulse_period=None, Ref_Freqs=None, verbose=False):
        """
        Calculates SNR for each pulse in a dedispersed time series.

        ===================== ===== ===================================================================
        *dedispersed_dynspec*       Array with cleaned dynamic spectrum.
        *pulse_time*          None  Time for each pulse. [t1,t2,t3,...]
        *Ref_Freqs*           None  List with the begin and end of reference frequencies. [f1a,f1b,f2a,f2b,f3a,f3b,....] in Hz.
        *verbose*             False If true then prints the SNR information, and plots a time series, for each pulse.
                                    Otherwise just returns the pulse_timeseries as an hArray.
        *pulse_period*        None  The actual pulse period of the pulsar. [s]
        ===================== ===== ===================================================================

        Example::

            pulse_timeseries = Task.dyn_DM_SNR(dedispersed_dynspec,pulse_time=[.1,.5,.8],Ref_Freqs=[120e6,145e6,150e6,170e6,140e6,160e6])

        or::

            Task.dyn_DM_SNR(dedispersed_dynspec,pulse_time=[.1,.5,.8],pulse_period=0.71452,Ref_Freqs=[120e6,145e6,150e6,170e6,140e6,160e6],verbose=True)

        """

        min_pulse_period = self.times[10]

        #------------------------
        # Checking imput is corret.

        if not isinstance(Ref_Freqs, type([])) or np.remainder(len(Ref_Freqs), 2):
            raise ValueError('Need a list of lenght 2n.')

        if not isinstance(pulse_time, type([])) or len(pulse_time) != len(Ref_Freqs) / 2:
            raise ValueError('Need a list of lenght n.')

        if pulse_period < min_pulse_period:
            raise ValueError('Pulse period < %2.3f [s], code upgraded needed if the period is correct.')

        #------------------------
        # Inicialating some variables.
        npulses = len(pulse_time)
        dedispersed_dynspec = cr.hArray_toNumpy(dedispersed_dynspec)
        pulse_timeseries = np.zeros([npulses, len(self.times)])
        Stats = np.zeros([4, npulses])  # The order is [Mean,Stddev,Max,SNR]  by npulses.
        mask = np.ones(len(pulse_timeseries[0]), dtype=bool)
        anti_mask = np.zeros(pulse_timeseries.shape, dtype=bool)

        if verbose:
            print 'Calculating time series for %i pulses.' % npulses

        # Region around the pulse to calculate the max from (useful if several pulses in time series).
        dtime = self.times.Find(">", max(pulse_period / 16, min_pulse_period))[0]

        # Creating mask on top of all pulses.
        for npulse in range(npulses):
            # Channel of reference time
            ntime = self.times.Find(">", pulse_time[npulse])[0]
            if verbose:
                print "Pulse time = ", self.times[ntime]
            mask[ntime - dtime:ntime + dtime] = False
            anti_mask[npulse, ntime - dtime:ntime + dtime] = True

        for npulse in range(npulses):
            # Channel of reference frequency
            nfreq1 = self.frequencies.Find(">", Ref_Freqs[2 * npulse])[0]
            nfreq2 = self.frequencies.Find(">", Ref_Freqs[2 * npulse + 1])[0]
            if verbose:
                print "Reference channels=", nfreq1, nfreq2
                print "Reference frequencies=", self.frequencies[nfreq1], self.frequencies[nfreq2]

            dedispersed_pulse = dedispersed_dynspec[nfreq1:nfreq2]
            pulse_timeseries[npulse] = np.sum(dedispersed_pulse, axis=0)

            Stats[0, npulse] = pulse_timeseries[npulse, mask].mean()           # Mean0

            pulse_timeseries[npulse] = pulse_timeseries[npulse] - Stats[0, npulse]

            Stats[1, npulse] = pulse_timeseries[npulse, mask].std()           # Stddev
            Stats[2, npulse] = pulse_timeseries[npulse, anti_mask[npulse]].max()           # Max
            Stats[3, npulse] = Stats[2, npulse] / Stats[1, npulse]  # SNR

            if verbose:
                print '--------'
                print 'Pulse ', npulse + 1, ':'
                print '     Mean  = %2.3f , Stddev  = %2.3f ' % (Stats[0, npulse], Stats[1, npulse])
                print '     Max  = %2.3f, SNR = %2.3f' % (Stats[2, npulse], Stats[3, npulse])
                print '--------'

        pulse_timeseries = cr.hArray(pulse_timeseries)

        if verbose:
            cr.plt.clf()
            cr.plt.rcParams['legend.loc'] = 'best'
            cr.plt.rcParams['font.size'] = 20
#            cr.plt.plot(cr.hArray_toNumpy(self.times),np.transpose(pulse_timeseries))
            cr.plt.plot(self.times[0:260].vec(), pulse_timeseries[0, 0:260].vec())
            cr.plt.plot(self.times[260:2 * 260].vec(), pulse_timeseries[1, 260:2 * 260].vec())
            cr.plt.plot(self.times[2 * 260:-1].vec(), pulse_timeseries[2, 2 * 260:-1].vec())
            cr.plt.xlabel("+/- Time [s]")
            cr.plt.title("Normalized Time_Series")

#        pulse_timeseries = cr.hArray(pulse_timeseries)
        pulse_timeseries.par.xvalues = self.times

        return pulse_timeseries, Stats

    def dyncalc(self, beams=None, nbeam=0, save_file=False, fraction=None, tbin=1, clean=False):
        '''
        Calculates the dynamic spectrum.

        =============== ===== ===================================================================
        *beams*         None  Input array. Take self.beams from task, if None.
        *nbeam*         0     Beam to work with, if (self.)beams has stored multiple ones.
        *save_file*     False Saves a file in hArray format with additional information besides the array values.
        *fraction*      None  If not None, then a list of the form [x,y] such that extracting the fraction x/y of the data. with x>-1, and y>=x.
        *tbin*          1     If >1 integrates over this number of blocks. Or time binning.
        *clean*         False If True it calculates the cleaned spectrum.
        =============== ===== ===================================================================

        Example::

            tpar filenames = ['File_path/File_name_tbb.h5']
            tpar blocklen = 2**10
            tpar maxchunklen = 2**16*5
            dynspec = Task.dyncalc()

        or::

            dynspec,cleandynspec = Task.dyncalc(tbin=16,clean=True,save_file=True)

        The regular and clean dynamic spectra (all blocks) are returned and stored in ``Task.dynspec`` and ``Task.cleandynspec`` respectively.
        '''
        t0 = time.clock()
        if beams != None:
            self.beams = beams

        self.spectrum_file_bin = os.path.join(self.spectrum_file, "data.bin")

        if not fraction:
            fraction = [1]
            chunk_fraction = self.nchunks
        else:
            if not isinstance(fraction, type([])) or len(fraction) != 2 or fraction[0] > fraction[1] or fraction[0] < 0 or fraction[1] < 0:
                raise ValueError('Need a list of lenght 2, with first element "<" or "=" second element. Both elements positive.')
            if fraction[0] == 0:
                fraction[0] = 1
            chunk_fraction = int(self.nchunks / fraction[1])
            self.start_time = (fraction[0] - 1) * (self.sectduration * self.nchunks) / fraction[1]
            self.end_time = fraction[0] * (self.sectduration * self.nchunks) / fraction[1]

        dynspec = cr.hArray(float, [self.nblocks * chunk_fraction, self.speclen])
        chunk_range = range((fraction[0] - 1) * chunk_fraction, (fraction[0]) * chunk_fraction)

        # Reading beams.
        if not beams:
            for chunk in chunk_range:
                self.beams.readfilebinary(self.spectrum_file_bin, chunk * self.speclen * self.nbeams * self.nblocks)
                for block in range(self.nblocks):
                    dynspec[(chunk - chunk_range[0]) * self.nblocks + block].spectralpower2(self.beams[block, nbeam])

        # Time integration.
        if tbin > 1:
            dynspec = cr.hArray_toNumpy(dynspec)
            dynspec = dynspec.reshape((dynspec.shape[0] / tbin, tbin, dynspec.shape[1]))
            dynspec = np.sum(dynspec, axis=1)
            dynspec = cr.hArray(dynspec)

        # Cleaning dynamic spectrum.
        if clean:
            avspec = cr.hArray(float, self.speclen, fill=0.0)
            dynspec[...].addto(avspec)
            cleandynspec = dynspec / avspec

        # Transposing arrays.
        dynspec = dynspec.Transpose()
        if clean:
            cleandynspec = cleandynspec.Transpose()

        # Create a frequency vector
        self.frequencies.fillrange((self.start_frequency), self.delta_frequency)

        # Create a time vector
        self.times = cr.hArray(float, int(round((self.end_time - self.start_time) / (self.block_duration * tbin))), name="Time", units=("", "s"))
        self.times.fillrange(self.start_time, self.block_duration * tbin)

        # Adding parameters
        dynspec.par.yvalues = self.frequencies
        dynspec.par.xvalues = self.times
        dynspec.par.tbin = tbin
        if clean:
            cleandynspec.par.yvalues = self.frequencies
            cleandynspec.par.xvalues = self.times
            cleandynspec.par.tbin = tbin

        # Saving file(s).
        if save_file:
            dynspec.write(os.path.join(self.spectrum_file, "dynspec"), nblocks=1, block=0, clearfile=True, ext=self.file_ext)
            print 'Saving binary in %s' % os.path.join(self.spectrum_file, "dynspec" + self.file_ext)
            if clean:
                cleandynspec.write(os.path.join(self.spectrum_file, "clean_dynspec"), nblocks=1, block=0, clearfile=True, ext=self.file_ext)
                print 'Saving binary in %s' % os.path.join(self.spectrum_file, "clean_dynspec" + self.file_ext)

        print "Finished - total time used:", time.clock() - t0, "s."

        self.dynspec = dynspec
        if clean:
            self.cleandynspec = cleandynspec
            return self.dynspec, self.cleandynspec
        else:
            return self.dynspec

    def dyncalc_multi_Beam(self, beams=None, nbeam=0, save_file=False, fraction=None, tbin=1, clean=False):
        '''
        Add beams toguether. Calculates the dynamic spectrum.

        =============== ===== ===================================================================
        *beams*         None  Input array. Take self.beams from task, if None.
        *nbeam*         0     Beam to work with, if (self.)beams has stored multiple ones.
        *save_file*     False Saves a file in hArray format with additional information besides the array values.
        *fraction*      None  If not None, then a list of the form [x,y] such that extracting the fraction x/y of the data. with x>-1, and y>=x.
        *tbin*          1     If >1 integrates over this number of blocks. Or time binning.
        *clean*         False If True it calculates the cleaned spectrum.
        =============== ===== ===================================================================

        Example::

            tpar filenames = ['File_path/File_name_tbb.h5']
            tpar blocklen = 2**10
            tpar maxchunklen = 2**16*5
            dynspec = Task.dyncalc_multi_Beam()

        or::

            TAB,TAB_Clean=Task.dyncalc_multi_Beam(tbin=16,clean=True)

        The regular and clean dynamic spectra (all blocks) are returned and stored in ``Task.dynspec`` and ``Task.cleandynspec`` respectively.
        '''

        t0 = time.time()
        tp0 = time.clock()

        if beams != None:
            self.beams = beams

        nbeams = len(self.spectrum_file)
        print 'WARNING: The results obtained from this program will be off if the beams are not the same size. A fix may not be supported here in the future, but new code is beeing added to Beam_Tools.py'

        self.spectrum_file_bin = self.spectrum_file
        for i, name in enumerate(self.spectrum_file):
            self.spectrum_file_bin[i] = os.path.join(name, "data.bin")

        if not fraction:
            fraction = [1]
            chunk_fraction = self.nchunks
        else:
            if not isinstance(fraction, type([])) or len(fraction) != 2 or fraction[0] > fraction[1] or fraction[0] < 0 or fraction[1] < 0:
                raise ValueError('Need a list of lenght 2, with first element "<" or "=" second element. Both elements positive.')
            if fraction[0] == 0:
                fraction[0] = 1
            chunk_fraction = int(self.nchunks / fraction[1])
            self.start_time = (fraction[0] - 1) * (self.sectduration * self.nchunks) / fraction[1]
            self.end_time = fraction[0] * (self.sectduration * self.nchunks) / fraction[1]

        dynspec = cr.hArray(float, [self.nblocks * chunk_fraction, self.speclen])
        chunk_range = range((fraction[0] - 1) * chunk_fraction, (fraction[0]) * chunk_fraction)
        bm = cr.hArray(complex, self.beams, self.beams)

        # Reading beams.
        if not beams:
            print 'Calculating the Dynamic Spectrum from the following beams: '
            for file in self.spectrum_file_bin:
                print file
            for chunk in chunk_range:
                print ' Calculation at {0:.2%}  \r'.format(float(chunk) / len(chunk_range)),
                sys.stdout.flush()
                for nb in range(nbeams):
#                    print self.spectrum_file_bin[nb]
                    bm.readfilebinary(self.spectrum_file_bin[nb], chunk * self.speclen * self.nbeams * self.nblocks)
                    bm /= float(nb + 1.)
                    if nb > 1:
                        self.beams *= (nb) / (nb + 1.)
                    self.beams += bm
                    bm *= 0
                for block in range(self.nblocks):
                    dynspec[(chunk - chunk_range[0]) * self.nblocks + block].spectralpower2(self.beams[block, nbeam])
                self.beams *= 0

        # Time integration.
        if tbin > 1:
            dynspec = cr.hArray_toNumpy(dynspec)
            dynspec = dynspec.reshape((dynspec.shape[0] / tbin, tbin, dynspec.shape[1]))
            dynspec = np.sum(dynspec, axis=1)
            dynspec = cr.hArray(dynspec)

        # Cleaning dynamic spectrum.
        if clean:
            avspec = cr.hArray(float, self.speclen, fill=0.0)
            dynspec[...].addto(avspec)
            cleandynspec = dynspec / avspec

        # Transposing arrays.
        dynspec = dynspec.Transpose()
        if clean:
            cleandynspec = cleandynspec.Transpose()

        # Create a frequency vector
        self.frequencies.fillrange((self.start_frequency), self.delta_frequency)

        # Create a time vector
        self.times = cr.hArray(float, int(round((self.end_time - self.start_time) / (self.block_duration * tbin))), name="Time", units=("", "s"))
        self.times.fillrange(self.start_time, self.block_duration * tbin)

        # Adding parameters
        dynspec.par.yvalues = self.frequencies
        dynspec.par.xvalues = self.times
        dynspec.par.tbin = tbin
        if clean:
            cleandynspec.par.yvalues = self.frequencies
            cleandynspec.par.xvalues = self.times
            cleandynspec.par.tbin = tbin

        # Saving file(s).
        if save_file:
            dynspec.write(os.path.join(self.spectrum_file, "dynspec"), nblocks=1, block=0, clearfile=True, ext=self.file_ext)
            print 'Saving binary in %s' % os.path.join(self.spectrum_file, "dynspec" + self.file_ext)
            if clean:
                cleandynspec.write(os.path.join(self.spectrum_file, "clean_dynspec"), nblocks=1, block=0, clearfile=True, ext=self.file_ext)
                print 'Saving binary in %s' % os.path.join(self.spectrum_file, "clean_dynspec" + self.file_ext)

        print "Finished - total processor time used:", time.clock() - tp0, "s."
        print "Finished - total time used:", time.time() - t0, "s."

        self.dynspec = dynspec
        if clean:
            self.cleandynspec = cleandynspec
            return self.dynspec, self.cleandynspec
        else:
            return self.dynspec

    def qplot(self, entry=0, flaggedblock=0, block=-1, all=True):
        """
        If you see an output line like this::

          # Start antenna = 92 (ID= 17011092) - 4 passes:
          184 - Mean=  3.98, RMS=  6.35, Npeaks=  211, Nexpected=256.00 (Npeaks/Nexpected=  0.82), nsigma=  2.80, limits=( -2.80,   2.80)
          185 - Mean=  3.97, RMS=  6.39, Npeaks=  200, Nexpected=256.00 (Npeaks/Nexpected=  0.78), nsigma=  2.80, limits=( -2.80,   2.80)
          186 - Mean=  3.98, RMS=  6.40, Npeaks=  219, Nexpected=256.00 (Npeaks/Nexpected=  0.86), nsigma=  2.80, limits=( -2.80,   2.80)
          - Block   514: mean=  0.25, rel. rms=   2.6, npeaks=   16, spikyness=  15.00, spikeexcess= 16.00   ['rms', 'spikeexcess']

        this will tell you that Antenna 17011092 was worked on (the 92nd
        antenna in the data file) and the 186th chunk (block 514)
        contained some suspicious data (too many spikes). If you want to
        inspect this, you can call::

          >>> Task.qplot(186)

        This will plot the chunk and highlight the first flagged block
        (#514) in that chunk::

          >>> Task.qplot(186,1)

        would highlight the second flagged block (which does not exist here).

        If the chunks are too long to be entirely plotted, use::

          >>> Task.qplot(186,all=False).
        """

        print 'WARNING: Out of Order. Need to be debuged.'
        stop

        quality_entry = self.quality[entry]
        filename = quality_entry["filename"]
        datafile = cr.open(filename)
# EE        iantenna=datafile["CHANNEL_ID"].find(quality_entry["antenna"])
        iantenna = datafile["CHANNEL_ID"].index(quality_entry["antenna"])
        datafile["selectedAntennas"] = [iantenna]
        if block < 0 and flaggedblock < len(quality_entry["flaggedblocks"]):
            block = quality_entry["flaggedblocks"][flaggedblock]
            s = "flaggedblock # " + str(flaggedblock) + "/" + str(len(quality_entry["flaggedblocks"]) - 1)
        else:
            s = ""
        print "Filename:", filename, "block =", block, "BLOCKSIZE =", quality_entry["BLOCKSIZE"], s
        if all:
            datafile["BLOCKSIZE"] = quality_entry["size"]
            datafile["BLOCK"] = quality_entry["offset"] * quality_entry["BLOCKSIZE"] / quality_entry["size"]
            y0 = datafile["TIMESERIES_DATA"]
            y0.par.xvalues = datafile["Time"]
            y0.par.xvalues.setUnit("mu", "")
            y0.plot()
        datafile["BLOCKSIZE"] = quality_entry["BLOCKSIZE"]
        datafile["BLOCK"] = block
        y = datafile["TIMESERIES_DATA"]
        y.par.xvalues = datafile["Time"]
        y.par.xvalues.setUnit("mu", "")
        y.plot(clf=not all)


"""
Replacements for new tbb.py (deprecated)
========================================

Text will disappear soon. Just reminder for myself ...

::

   (query-replace "nofAntennas" "NOF_DIPOLE_DATASETS" nil (point-min) (point-max))
   (query-replace "AntennaIDs" "DIPOLE_NAMES" nil (point-min) (point-max))
   (query-replace "sampleInterval" "SAMPLE_INTERVAL" nil (point-min) (point-max))
   (query-replace "datafile.hdr" "datafile.getHeader()" nil (point-min) (point-max))
   (query-replace "fftLength" "FFTSIZE" nil (point-min) (point-max))
   (query-replace "blocksize" "BLOCKSIZE" nil (point-min) (point-max))
   (query-replace "block" "BLOCK" 1 (point-min) (point-max))
   (query-replace "Fx" "TIMESERIES_DATA" nil (point-min) (point-max))
   (query-replace "Time" "TIME_DATA" nil (point-min) (point-max))
   (query-replace "selectedAntennasID" "SELECTED_DIPOLES" nil (point-min) (point-max))

   del-> freqs

           start_frequency = dict(default=lambda self:self.datafile["FREQUENCY_RANGE"][0][0],
                                  doc="Start frequency of spectrum",unit="Hz"),
           end_frequency = dict(default=lambda self:self.datafile["FREQUENCY_RANGE"][0][1],
                                doc="End frequency of spectrum",unit="Hz"),
           filesize = dict(default=lambda self:self.datafile["DATA_LENGTH"][0],
                           doc="Length of file for one antenna.",
                           unit="Samples"),

           self.beams.setHeader("FREQUENCY_INTERVAL"=self.delta_frequency)
           self.datafile["SELECTED_DIPOLES"]=[antennaID]

   getfile:

   ************************************************************************
   Example:
   file=cr.open(LOFARSOFT+"/data/lopes/example.event")
   tpar antenna_positions=dict(zip(file["antennaIDs"],file.getCalData("Position")))
   tpar pointings=[dict(az=178.9*deg,el=28*deg),dict(az=0*deg,el=90*deg,r=1)]
   tpar cal_delays=dict(zip(file["antennaIDs"],file.getCalData("Delay")))
   tpar phase_center=[-84.5346,36.1096,0]
   tpar FarField=True
   tpar NyquistZone=2
   tpar randomize_peaks=False

   #file=cr.open(LOFARSOFT+"/data/lopes/2004.01.12.00:28:11.577.event")
   #file["SelectedAntennasID"]=[0]
   #fx0=file["TIMESERIES_DATA"]


   ------------------------------------------------------------------------
   tload "BeamFormer"
   file=cr.open(LOFARSOFT+"/data/lopes/2004.01.12.00:28:11.577.event")
   tpar filefilter="$LOFARSOFT/data/lopes/2004.01.12.00:28:11.577.event"
   tpar antenna_positions=dict(map(lambda x: (x[0],x[1].array()),zip(file["antennaIDs"],file.getCalData("Position"))))
   tpar pointings=[dict(az=41.9898208*deg, el=64.70544*deg,r=1750),dict(az=0*deg,el=90*deg,r=100000)]
   tpar cal_delays=dict(zip(file["antennaIDs"],[0,-2.3375e-08,-2.75e-09,-3.75e-09,-2.525e-08,-2.575e-08,1.3125e-08,-1.6875e-08]))
   tpar phase_center=[-22.1927,15.3167,0]
   tpar FarField=False
   tpar NyquistZone=2
   tpar randomize_peaks=False
   ------------------------------------------------------------------------
   antenna pos: cr.hArray(float, [8, 3], fill=[-84.5346,36.1096,0,-52.6146,54.4736,-0.0619965,-34.3396,22.5366,-0.131996,-2.3706,40.6976,-0.00299835,41.0804,1.97557,-0.0769958,22.7764,34.1686,-0.0549927,-20.8546,72.5436,-0.154999,11.1824,90.8196,-0.221992]) # len=24 slice=[0:24])

   self=Task
   self.beams[...,0].nyquistswap(self.NyquistZone)
   fxb=cr.hArray(float,[2,self.blocklen],name="TIMESERIES_DATA"); fxb[...].saveinvfftw(self.beams[...,0],1);  fxb.abs()
   fxb[...].plot(clf=True); cr.plt.show()
"""

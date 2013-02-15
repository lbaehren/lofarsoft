"""
Task: Average spectrum
======================


"""

# ************************************************************************
# Replacements for new tbb.py
# ************************************************************************
# (query-replace "nofAntennas" "NOF_DIPOLE_DATASETS" nil (point-at-eol) (point-max))
# (query-replace "AntennaIDs" "DIPOLE_NAMES" nil (point-at-eol) (point-max))
# (query-replace "sampleInterval" "SAMPLE_INTERVAL" nil (point-at-eol) (point-max))
#  datafile["SAMPLE_INTERVAL"] -> datafile["SAMPLE_INTERVAL"][0]
# (query-replace "datafile.hdr" "datafile.getHeader()" nil (point-at-eol) (point-max))
# (query-replace "fftLength" "FFTSIZE" nil (point-at-eol) (point-max))
# (query-replace "blocksize" "BLOCKSIZE" nil (point-at-eol) (point-max))
# (query-replace "block" "BLOCK" 1 (point-at-eol) (point-max))
# (query-replace "Fx" "TIMESERIES_DATA" nil (point-at-eol) (point-max))
# (query-replace "Time" "TIME_DATA" nil (point-at-eol) (point-max))
# (query-replace "selectedAntennasID" "SELECTED_DIPOLES" nil (point-at-eol) (point-max))
# (query-replace "selectedAntennas" "SELECTED_DIPOLES" nil (point-at-eol) (point-max))
# (query-replace "[\"Frequency\"]" "[\"FREQUENCY_DATA\"]" nil (point-at-eol) (point-max))
# (query-replace "datafile.filesize" "datafile[\"DATA_LENGTH\"][0]" nil (point-at-eol) (point-max))
# in averagespectrum_getfile(ws): crfile -> open

import pycrtools as cr
from pycrtools import tasks
from pycrtools import qualitycheck
import numpy as np
import time
import os
import math

# Defining the workspace parameters


def make_frequencies(spectrum, offset=-1, frequencies=None, setxvalues=True):
    """Calculates the frequencies for the calculated spectrum (outdated)
    """
    hdr = spectrum.par.hdr
    if offset < 0 and "nbands" in hdr:
        mult = hdr["nbands"]
    else:
        mult = 1
    if offset < 0:
        offset = 0
    if "subspeclen" in hdr:
        l = hdr["subspeclen"]
        if hdr["stride"] == 1:
            l /= 2
    else:
        l = len(spectrum)
    if frequencies == None:
        frequencies = cr.hArray(float, [l * mult], name="Frequency", units=("M", "Hz"), header=hdr)
    frequencies.fillrange((hdr["start_frequency"] + offset * hdr["delta_band"]) / 10 ** 6, hdr["delta_frequency"] / 10 ** 6)
    if setxvalues:
        spectrum.par.xvalues = frequencies
    return frequencies

# def averagespectrum_calcfrequencies(self):
#    """Calculates the frequencies for the calculated spectrum.
#    """


def averagespectrum_getfile(current_file_number, filenames, nfiles, filefilter):
    """
    To produce an error message in case the file does not exist
    """
    if current_file_number < nfiles:
        f = cr.open(filenames[current_file_number])
        return f
    else:
        print "ERROR: File " + filefilter + " not found!"
        return None

"""
=========== ===== ========================================================
*default*         contains a default value or a function that will be
                  assigned when the parameter is accessed the first time
                  and no value has been explicitly set. The function has
                  the form ``lambda self: functionbody``, where self is the
                  worspace itself, so that one can access other
                  parameters. E.g.: ``default:lambda self: self.par1+1`` so
                  that the default value is one larger than the value in
                  ``par1`` in the workspace.
*doc*             A documentation string describing the parameter.
*unit*            The unit of the value (for informational purposes only)
*export*    True  If ``False`` do not export the parameter with
                  ``self.parameters()`` or print the parameter
*workarray* False If ``True`` then this is a workarray which contains
                  large amount of memory and is listed separately and
                  not written to file.
=========== ===== ========================================================

"""


class WorkSpace(tasks.WorkSpace(taskname="AverageSpectrum")):
    """
    """
    parameters = dict(
        filefilter=dict(default="$LOFARSOFT/data/lofar/RS307C-readfullsecondtbb1.h5",
                        doc="Unix style filter (i.e., with ``*``, ~, ``$VARIABLE``, etc.), to describe all the files to be processed."),

        file_start_number=dict(default=0,
                               doc="Integer number pointing to the first file in the ``filenames`` list with which to start. Can be changed to restart a calculation."),

        current_file_number=dict(default=0,
                                 doc="Integer number pointing to the current file in the ``filenames`` list.",
                                 output=True),

        load_if_file_exists=dict(default=False,
                                 doc="If average spectrum file (``spectrumfile``) already exists, skip calculation and load existing file."),

        datafile=dict(default=lambda self: averagespectrum_getfile(self.current_file_number, self.filenames, self.nfiles, self.filefilter),
                      doc="Data file object pointing to raw data.",
                      export=False),

        addantennas=dict(default=True,
                         doc="If True, add all antennas into one average spectrum, otherwise keep them separate in memory per file."),

        doplot=dict(default=False,
                    doc="Produce output plots. If doplot>1 plot while processing."),

        plotlen=dict(default=2 ** 12,
                     doc="How many channels ``+/-`` the center value to plot during the calculation (to allow progress checking)."),

        plotskip=dict(default=1,
                      doc="Plot only every 'plotskip'-th spectrum, skip the rest (should not be smaller than 1)."),

        plot_finish=dict(default=lambda self: cr.plotfinish(filename=self.plot_filename, doplot=self.doplot, plotpause=True),
                         doc="Function to be called after each plot to determine whether to pause or not (see ::func::plotfinish)"),

        plot_name=dict(default="",
                       doc="Extra name to be added to plot filename."),

        plot_filename=dict(default="",
                           doc="Base filename to store plot in."),

        plot_antenna=dict(default=0,
                          doc="Which antenna to plot?"),

        plot_zoom_x1=dict(default=0,
                          doc="Use this to plot an additional graph zoomed into the region between plot_zoom_x1 and plot_zoom_x2"),

        plot_zoom_x2=dict(default=0,
                          doc="Use this to plot an additional graph zoomed into the region between plot_zoom_x1 and plot_zoom_x2"),

        plot_zoom_x=dict(default=lambda self: (self.plot_zoom_x1, self.plot_zoom_x2) if self.plot_zoom_x1 and self.plot_zoom_x2 else False,
                         doc="(nu1,nu2) - If a tuple with two numbers is provided use this to plot an additional graph zoomed into the region between these frequencies on the x-axis"),

        plot_zoom_slice=dict(default=lambda self: slice(self.frequencies.findlowerbound(self.plot_zoom_x[0]).val(),
                                                        self.frequencies.findlowerbound(self.plot_zoom_x[1]).val()) if isinstance(self.plot_zoom_x, tuple) else False,
                             doc="slice(n1,n2) - If a slice is provided use this to plot an additional graph zoomed into the region between these channels along the x-axis"),

        stride_n=dict(default=0,
                      doc="if >0 then divide the FFT processing in ``n=2**stride_n blocks``. This is slower but uses less memory."),

        doublefft=dict(default=False,
                       doc="if True split the FFT into two, thereby saving memory."),

        calc_incoherent_sum=dict(default=False,
                                 doc="Calculate the incoherent sum of all antennas (doublefft=False). See incoherent_sum for result."),

        ntimeseries_data_added_per_chunk=dict(default=lambda self: cr.hArray(int, [self.nchunks]),
                                              doc="Number of chunks added in each bin for incoherent sum"),

        delta_nu=dict(default=1000,
                      doc="Frequency resolution",
                      unit="Hz"),

        blocklen=dict(default=lambda self: 2 ** int(math.floor(math.log(self.chunksize_estimated, 2)) / 2),
                      doc="The size of a block being read in if stride>1, otherwise ``self.blocklen*self.nblocks`` is the chunksize."),

        maxnchunks=dict(default=-1,
                        doc="Maximum number of chunks of raw data to integrate spectrum over (-1 = all)."),

        maxblocksflagged=dict(default=0,
                              doc="Maximum number of blocks that are allowed to be flagged before the entire spectrum of the chunk is discarded. If =0 then flag a chunk if any block is deviant."),

        stride=dict(default=lambda self: 2 ** self.stride_n if self.doublefft else 1,
                    doc="If ``stride>1`` divide the FFT processing in ``n=stride`` blocks."),

        fileext=dict(default=".pcr",
                     doc="Extension of filename for temporary data files (e.g., used if ``stride > 1``.)"),

        infileext=dict(default=".h5",
                       doc="Extension of the input filename (later to be removde from the filename)"),

        tmpfilename=dict(default="tmp",
                         doc="Root filename for temporary data files."),

        filename=dict(default=lambda self: self.filenames[self.current_file_number],
                      doc="Index number of the current file in the list of filenames of raw data files.",
                      output=True),

        filenames=dict(default=lambda self: cr.listFiles(self.filefilter),
                       doc="List of filenames of data file to read raw data from."),

        nfiles=dict(default=lambda self: len(self.filenames),
                    doc="Number of data files to process."),

        output_dir=dict(default="",
                        doc="Directory where output file is to be written to (look for filename.dir there)."),

        output_subdir=dict(default=lambda self: os.path.join(self.output_dir, self.root_filename + ".dir"),
                           doc="Directory where output data files are being written to."),

        root_filename=dict(default=lambda self: (os.path.split(self.filenames[self.current_file_number])[1].replace(self.infileext, "")
                                                 if self.nfiles > self.current_file_number else "unknown"),
                           doc="Root filename for output without directory and extensions."),

        spectrum_file=dict(default=lambda self: os.path.join(self.output_subdir, self.root_filename + "-spec" + self.fileext),
                           doc="Complete filename including directory to store the final spectrum."),

        dynspec_file=dict(default=lambda self: os.path.join(self.output_subdir, self.root_filename + "-dynspec" + self.fileext),
                          doc="Complete filename including directory to store the final dynamic spectrum."),

        all_dynspec_file=dict(default=lambda self: os.path.join(self.output_dir, self.root_filename + "-alldynspec" + self.fileext),
                              doc="Complete filename including directory to store the final dynamic spectrum of all files."),

        incoherent_sum_file=dict(default=lambda self: os.path.join(self.output_subdir, self.root_filename + "-incohrent_sum" + self.fileext),
                            doc="Complete filename including directory to store the incoherent sum of all antennas."),

        output_files=dict(default=lambda self: dict(all_dynamic_spectrum=[], dynamic_spectrum=[], incoherent_sum=[], average_spectrum=[]), output=True,
                          doc="Dict containing the outpufiles created in the task. Keywords are 'dynamic_spectrum', 'incohrent_sum', 'average_spectrum', which in turn return listsof filenames"),

        qualitycheck=dict(default=True,
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

        mean_antenna=dict(default=lambda self: cr.hArray(float, [self.nantennas * self.nfiles], name="Mean per Antenna"),
                          doc="Mean value of time series per antenna.",
                          output=True, workarray=True),

        rms_antenna=dict(default=lambda self: cr.hArray(float, [self.nantennas * self.nfiles], name="RMS per Antenna"),
                         doc="RMS value of time series per antenna.",
                         output=True, workarray=True),

        npeaks_antenna=dict(default=lambda self: cr.hArray(float, [self.nantennas * self.nfiles], name="Number of Peaks per Antenna"),
                            doc="Number of peaks of time series per antenna.",
                            output=True, workarray=True),

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
                        doc="RMS of npeaks over all antennas.",
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

        maxpeak=dict(default=7,
                     doc="Maximum height of the maximum in each block (in sigma,i.e. relative to rms) before quality is failed."),

        spikeexcess=dict(default=20,
                         doc="Set maximum allowed ratio of detected over expected peaks per block to this level (1 is roughly what one expects from Gaussian noise)."),

        rmsfactor=dict(default=3,
                       doc="Factor by which the RMS is allowed to change within one chunk of time series data before it is flagged."),

        rmsrange_min=dict(default=0,
                          doc="Min value of the rms of the time series that are allowed before a block is flagged. Will be igrored if None."),

        rmsrange_max=dict(default=0,
                          doc="Max value of the rms of the time series that are allowed before a block is flagged. Will be igrored if None."),

        rmsrange=dict(default=lambda self: (self.rmsrange_min, self.rmsrange_max) if self.rmsrange_min and self.rmsrange_max else None,
                      doc="Tuple with absolute min/max values of the rms of the time series that are allowed before a block is flagged. Will be igrored if None.",
                      output=False),

        meanfactor=dict(default=3,
                        doc="Factor by which the mean is allowed to change within one chunk of time series data before it is flagged."),

        randomize_peaks=dict(default=True,
                             doc="Replace all peaks in time series data which are 'rmsfactor' above or below the mean with some random number in the same range."),

        peak_rmsfactor=dict(default=5,
                            doc="At how many sigmas above the mean will a peak be randomized."),

        start_frequency=dict(default=lambda self: self.freqs[0],
                             doc="Start frequency of spectrum",
                             unit="Hz"),

        end_frequency=dict(default=lambda self: self.freqs[-1],
                             doc="End frequency of spectrum",
                             unit="Hz"),

        delta_frequency=dict(default=lambda self: (self.end_frequency - self.start_frequency) / (self.speclen - 1.0),
                             doc="Separation of two subsequent channels in final spectrum"),

        delta_band=dict(default=lambda self: (self.end_frequency - self.start_frequency) / self.stride * 2,
                        doc="Frequency width of one section/band of spectrum",
                        unit="Hz"),

        full_blocklen=dict(default=lambda self: self.blocklen * self.stride,
                           doc="Full block length (blocklen*stride)",
                           unit="Samples"),

        fullsize=dict(default=lambda self: self.nblocks * self.full_blocklen,
                      doc="The full length of the raw time series data used for one spectral chunk (nblocks*blocklen*stride)."),

        nblocks=dict(default=lambda self: 2 ** int(math.floor(math.log(self.chunksize_estimated / self.full_blocklen, 2))),
                     doc="Number of blocks of lenght ``blocklen`` the time series data set is split in. The blocks are also used for quality checking."),

        nbands=dict(default=lambda self: (self.stride + 1) / 2,
                    doc="Number of bands in spectrum which are separately written to disk, if ``stride > 1``. This is one half of stride, since only the lower half of the spectrum is written to disk."),

        subspeclen=dict(default=lambda self: self.blocklen * self.nblocks,
                        doc="Size of one section/band of the final spectrum"),

        nsamples_data=dict(default=lambda self: float(self.fullsize) / 10 ** 6,
                           doc="Number of samples in raw antenna file",
                           unit="MSamples"),

        size_data=dict(default=lambda self: float(self.fullsize) / 1024 / 1024 * 16,
                       doc="Number of samples in raw antenna file",
                       unit="MBytes"),

        nantennas=dict(default=lambda self: len(self.antennas),
                       doc="The number of antennas to be calculated in one file."),

        nantennas_file=dict(default=lambda self: self.datafile["NOF_DIPOLE_DATASETS"],
                            doc="The actual number of antennas available for calculation in the file."),

        antennas_start=dict(default=0,
                            doc="Start with the *n*-th antenna in each file (see also ``nantennas_stride``). Can be used for selecting odd/even antennas."),

        antennas_end=dict(default=-1,
                          doc="Maximum antenna number (plus one, zero based ...) to use in each file."),

        antennas_stride=dict(default=1,
                             doc="Take only every *n*-th antenna from antennas list (see also ``antennas_start``). Use 2 to select odd/even."),

        antennas=dict(default=lambda self: cr.hArray(range(min(self.antennas_start, self.nantennas_file - 1), self.nantennas_file if self.antennas_end < 0 else min(self.antennas_end, self.nantennas_file), self.antennas_stride)),
                      doc="Array of index numbers for antennas to be processed from the current file."),

        antenna_list=dict(default={},
                          doc="Dictionary of antenna indices used as input from each filename.",
                          output=True),

        antennaIDs=dict(default=lambda self: cr.hArray(self.datafile["DIPOLE_NAMES"]),
                        doc="Antenna IDs from the current file."),

        antennas_used=dict(default=lambda self: set(),
                           doc="A set of antenna names that were actually included in the average spectrum, excluding the flagged ones.",
                           output=True),

        nantennas_total=dict(default=0,
                             doc="Total number of antennas that were processed (flagged or not) in this run.",
                             output=True),

        nchunks_max=dict(default=lambda self: float(self.datalength) / self.fullsize,
                         doc="Maximum number of spectral chunks to average"),

        nchunks=dict(default=lambda self: min(self.datalength / self.fullsize, self.maxnchunks) if self.maxnchunks > 0 else self.datalength / self.fullsize,
                     doc="Number of spectral chunks that are averaged"),

        nspectraadded=dict(default=lambda self: 0,
                           doc="Number of spectra added at the end.",
                           output=True),

        nspectraadded_per_antenna=dict(default=lambda self: cr.Vector(int, self.nantennas, fill=0),
                                       doc="Number of spectra added per antenna.",
                                       output=True),

        nspectraflagged=dict(default=lambda self: 0,
                             doc="Number of spectra flagged and not used.",
                             output=True),

        nspectraflagged_per_antenna=dict(default=lambda self: cr.Vector(int, self.nantennas, fill=0),
                                         doc="Number of spectra flagged and not used per antenna.",
                                         output=True),

        samplerate=dict(default=lambda self: self.datafile["SAMPLE_INTERVAL"][0],
                        doc="Sample length in raw data set.",
                        unit="s"),

        starttime_unix=dict(default=lambda self: self.datafile["TIME"][0], output=True,
                            doc="Unix time stamp of start time.",
                            unit="s"),

        starttime_local=dict(default=lambda self: time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(self.starttime_unix)), output=True,
                             doc="Start time of the observation in local time"),

        starttime_UTC=dict(default=lambda self: time.strftime("%Y-%m-%d %H:%M:%S", time.gmtime(self.starttime_unix)), output=True,
                           doc="Start time of the observation in UTC"),

        starttimes_unix=dict(default=[],
                             doc="Start times of the observations in unix time time per file",
                             unit="s",
                             output=True),

        starttimes_local=dict(default=[],
                              doc="Start times of the observations in local time per file",
                              output=True),

        starttimes_UTC=dict(default=[],
                            doc="Start times of the observations in UTC per file",
                            output=True),

        datalength=dict(default=lambda self: self.datafile["DATA_LENGTH"][0],
                         doc="The (maximum) length of one full data set of one antenna per file."),

        chunksize_estimated=dict(default=lambda self: int(min(1. / self.delta_nu / self.samplerate, self.datalength)),
                                 doc="The desired chunksize for a single FFT. Can be set directly, otherwise estimated from ``delta_nu``. The actual size will be rounded to give a power of two, hence, see ``chunksize_used`` for the actual chunksize used."),

        chunksize_used=dict(default=lambda self: self.blocklen * self.nblocks,
                            doc="The chunksize one would use for a single FFT."),

        blocklen_section=dict(default=lambda self: self.blocklen / self.stride,
                              doc="Blocklength of a section"),

        nsubblocks=dict(default=lambda self: self.stride * self.nblocks,
                        doc="Number of subblocks"),

        nblocks_section=dict(default=lambda self: self.nblocks / self.stride,
                             doc="Number of blocks per section"),

        header=dict(default=lambda self: self.datafile.getHeader(),
                    doc="Header of datafile",
                    export=False),

        freqs=dict(default=lambda self: self.datafile["FREQUENCY_DATA"],
                   doc="Frequencies",
                   export=False),

        # Now define all the work arrays used internally

        frequencies=dict(workarray=True,
                           doc="Frequency axis for final power spectrum.",
                           default=lambda self: cr.hArray(float, [self.power_size], name="Frequency", units=("M", "Hz"), header=self.header)),

        power_size = dict(default=lambda self: (self.subspeclen / 2 if self.stride == 1 else self.subspeclen) if self.doublefft else self.speclen,
                          doc="Size of the output spectrum in the vector ``power``"),

        speclen = dict(default=lambda self: self.chunksize_used / 2 + 1,
                       doc="The length of the spectrum for a single FFT"),

        power = dict(workarray=True,
                     doc="Resulting average power spectrum (or part thereof if ``stride > 1``)",
                     default=lambda self: cr.hArray(float, [self.power_size], name="Spectral Power", par=dict(logplot="y"), header=self.header, xvalues=self.frequencies) if self.addantennas else cr.hArray(float, [self.nantennas, self.power_size], name="Spectral Power", par=dict(logplot="y"), header=self.header, xvalues=self.frequencies)),

        power2 = dict(workarray=True,
                     doc="Copy of power with 2 dimensions also when antennas are being added.",
                     default=lambda self: cr.hArray(self.power.vec(), [1, self.power_size], name="Spectral Power", par=dict(logplot="y"), header=self.header, xvalues=self.frequencies) if self.addantennas else cr.hArray(self.power.vec(), [self.nantennas, self.power_size], name="Spectral Power", par=dict(logplot="y"), header=self.header, xvalues=self.frequencies)),

        tdata = dict(workarray=True,
                     doc="Array to read time series data in",
                     default=lambda self: cr.hArray(float, [self.nblocks, self.blocklen], name="tdata", header=self.header)),

        incoherent_sum = dict(workarray=True, doc="Incoherent sum of the power in all antennas (timeseries data).",
                              default=lambda self: cr.hArray(float, [self.nchunks, self.chunksize_used], name="Power(t)", header=self.header)),

        cdata = dict(workarray=True,
                     doc="Complex array holding the FFT of the time series data and other things",
                     default=lambda self: cr.hArray(complex, [self.nblocks, self.blocklen], name="cdata", header=self.header)),

        cdata2 = dict(workarray=True,
                      doc="Supplementary complex array holding the FFT of the time series for a single FFT",
                      default=lambda self: cr.hArray(complex, [self.speclen], name="cdata2", header=self.header)),

        fftdata = dict(workarray=True,
                      doc="Complex array holding the FFT of the time series for a single FFT (Same memory as cdata)",
                      default=lambda self: cr.hArray(complex, [self.speclen], name="FFT(tdata)", header=self.header)),

        cdataT = dict(workarray=True,
                      doc="Complex array holding the FFT of the time series data and other things transposed.",
                      default=lambda self: cr.hArray(complex, [self.blocklen, self.nblocks], name="cdataT", header=self.header)),

        # Note, that all the following arrays have the same memory als cdata and cdataT

        tmpspecT = dict(workarray=True,
                        doc="Wrapper array for ``cdataT``",
                        default=lambda self: cr.hArray(self.cdataT.vec(), [self.stride, self.nblocks_section, self.blocklen], header=self.header)),

        tmpspec = dict(workarray=True,
                       doc="Wrapper array for ``cdata``",
                       default=lambda self: cr.hArray(self.cdata.vec(), [self.nblocks_section, self.full_blocklen], header=self.header)),

        specT = dict(workarray=True,
                     doc="Wrapper array for ``cdataT``",
                     default=lambda self: cr.hArray(self.cdataT.vec(), [self.full_blocklen, self.nblocks_section], header=self.header)),

        specT2 = dict(workarray=True,
                      doc="Wrapper array for ``cdataT``",
                      default=lambda self: cr.hArray(self.cdataT.vec(), [self.stride, self.blocklen, self.nblocks_section], header=self.header)),

        spec = dict(workarray=True,
                    doc="Wrapper array for ``cdata``",
                    default=lambda self: cr.hArray(self.cdata.vec(), [self.blocklen, self.nblocks], header=self.header)),

        store_spectra = dict(default=True,
                             doc="Write calculated spectra to disk."),

        calc_dynspec = dict(default=False,
                            doc="Calculate a dynamic spectrum as well (if no doublefft)"),

        calc_all_dynspec = dict(default=lambda self: (self.calc_dynspec and self.nfiles > 2),
                                doc="Calculate a dynamic spectrum from the average spectra of all files as well (if no doublefft)"),

        calc_averagespectrum = dict(default=True,
                                    doc="Calculate the averagespectrum (not yet fully implemented ... to not do it)."),

        dynspec_time=dict(default=lambda self: cr.hArray(float, [self.nsects], fill=0.0, name="Time", units=("", "s")),
                          doc="Time axis for dynamic spectrum.",
                          workarray=True),

        dynspec_nspectra_added=dict(workarray=True,
                                    doc="Number of spectra added per time step.",
                                    default=lambda self: cr.hArray(float, [self.nsects], fill=0.0, name="Number of spectra added")),

        all_dynspec=dict(workarray=True,
                         doc="Dynamic spectrum from the average spectra of all files (if no doublefft and calc_dynspec=True).",
                         default=lambda self: cr.hArray(float, [self.nfiles, self.power_size], fill=0.0, name="Dynamic Spectrum of All Files", logplot="y", header=self.header, xvalues=self.frequencies, avspec=self.all_avspec, cleanspec=self.all_cleanspec)),

        dynspec=dict(workarray=True,
                     doc="Dynamic spectrum of all antennas (if no doublefft and calc_dynspec=True).",
                     default=lambda self: cr.hArray(float, [self.nsects, self.power_size], fill=0.0, name="Dynamic Spectrum", logplot="y", header=self.header, xvalues=self.frequencies, avspec=self.avspec, cleanspec=self.cleanspec)),

        cleanspec=dict(workarray=True,
                       doc="Cleaned dynamic spectrum.",
                       default=lambda self: cr.hArray(float, [self.nsects, self.power_size], fill=0.0, name="Clean Dynamic Spectrum", logplot="y", header=self.header, xvalues=self.frequencies, avspec=self.avspec)),

        all_cleanspec=dict(workarray=True,
                           doc="Cleaned dynamic spectrum.",
                           default=lambda self: cr.hArray(float, [self.nfiles, self.power_size], fill=0.0, name="Clean Dynamic Spectrum from All Files", logplot="y", header=self.header, xvalues=self.frequencies, avspec=self.avspec)),

        avspec=dict(workarray=True,
                    doc="Average spectrum over all times, used for plotting ...?",
                    default=lambda self: cr.hArray(float, [self.power_size], fill=0.0, name="Average Spectrum", par=dict(logplot="y"), header=self.header, xvalues=self.frequencies)),

        all_avspec=dict(workarray=True,
                        doc="Average spectrum over all times and all files, used for plotting ...?",
                        default=lambda self: cr.hArray(float, [self.power_size], fill=0.0, name="Average Spectrum", par=dict(logplot="y"), header=self.header, xvalues=self.frequencies)),

        t_int=dict(default=10 ** -3,
                   doc="Desired intergration time per spectrum for dynamic spectrum - will be rounded off to get integer number of spectral chunks",
                   unit="s"),

        t_int_used=dict(default=lambda self: self.sectduration,
                        doc="Actual time resolution of dynamic spectrum",
                        unit="s"),

        chunkduration=dict(default=lambda self: self.chunksize_used * self.samplerate,
                           doc="Length (duration) of one chunk of data.",
                           unit="s"),

        sectlen=dict(default=lambda self: self.chunksize_used * self.chunks_per_sect,
                     doc="Length of one section of data used to extract one chunk, i.e. on time step in dynamic spectrum.",
                     unit="Samples"),

        nsects=dict(default=lambda self: int(math.ceil(self.nchunks / float(self.chunks_per_sect))),
                      doc="Number of sections in the data file",
                      unit="Samples"),

        sectduration=dict(default=lambda self: self.sectlen * self.samplerate,
                          doc="Length in time units of one section of data used to extract one chunk, i.e. on time step in dynamic spectrum.",
                          unit="s"),

        end_time=dict(default=lambda self: self.sectduration * self.nchunks,
                      doc="End of time axis for dynamic spectrum.",
                      unit="s"),

        start_time=dict(default=0,
                        doc="Start of time axis for dynamic spectrum.",
                        unit="s"),

        chunks_per_sect=dict(default=lambda self: max(int(round(self.t_int / self.chunkduration)), 1),
                             doc="Number of chunks per section of data."),
        )


class AverageSpectrum(tasks.Task):
    """
    The function will calculate an average spectrum from a list of
    files and a series of antennas (all averaged into one
    spectrum).

    The task will do a basic quality check of each time series data
    set and only average good data.

    For very large FFTs, and to save memory, one can use a ``doublefft``
    which means that one big FFT is split into two steps of smaller
    FFts. This allows almost arbitrary resolution.

    The function will go through all files in the list and then loop,
    one-by-one, over all antennas in the files. Data will be read in
    in ``nchunks`` chunks (i.e. blocks) of size ``fullsizes``. For a
    single FFT these chunks will be subdivided into ``nblock`` blocks
    to do quality checks (i.e., to see if certain blocks deviate from
    others). For a double FFT with stride>1 only a fraction of these
    blocks are read in at once.

    The desired frequency resolution is provided with the parameter
    ``delta_nu``, but this will be rounded off to the nearest value
    using channel numbers of a power of 2. For single FFTs you can
    also set the parameter ``chunksize``.

    Note: the spectrum has N/2 channels and not N/2+1 channels as is
    usually the case. This means that the last channel is missing!!

    The resulting spectrum is stored in the array ``Task.power`` (only
    complete for ``stride=1``) and written to disk as an cr.hArray with
    parameters stored in the header dict (use
    ``getHeader('AverageSpectrum')`` to retrieve this.)

    Note for single FFTs one can decide whether to average all
    antennas into one spectrum (e.g., an incoherent 'station
    spectrum'), or whether to keep the antennas separate and get an
    average spectrum per antenna (``addantennas=False``).

    This spectrum can be read back and a baseline can be fitted with
    ``FitBaseline``.


    **Incoherent Beam**

    With the option ``calc_incoherent_sum`` one can add the square of
    all time series data of all antennas into one long time series
    data (containing the power) for single FFTs. The resulting array
    Task.incoherent_sum has the length of the entire data set (of one
    antenna, of course) and has the dimensions
    [nchunks,chunksize=blocksize]. It will be saved on disk in
    sp.par.incoherent_sum.

    **Dynamic Spectrum**

    With the option ``calc_dynspec`` turned on, the task will
    calculate a dynamic spectrum for each file within the length of
    the data set. It will add all antennas of a station and then
    divide the data sets into sections of length ``t_int`` within
    which all chunks of data are intergated. The section is the one
    time-step in the dynamic spectrum. The result is provided in
    ``Task.dynspec`` and ``Task.cleanspec`` (where the dynamic spectrum
    is divided by an average spectrum, stored in
    ``Task.avspec``). Avspec and cleanspec are also par-array of
    dynspec (i.e., ``Task.dynspsc.par.cleanspec``). The dynamic
    spectrum will also be stored in the output directory
    (``output_subdir=filename.dir``) as a separate file.

    If multiple files are provided, also a dynamic spectrum is made
    out of the average spectra of the files being worked on and an
    average spectrum of all files. (Note, that the files have to be
    sorted by timestamp in the ``Task.filenames``, which is produced
    by ``Task.filefilter``.

    **Quality Checking**

    To avoid the spectrum being influenced by spikes in the time
    series, those spikes can be replaced by random numbers, before the
    FFT is taken (see ``randomize_peaks``).

    The quality information (RMS, MEAN, flagged blocks per antennas)
    is stored in a data 'quality database' in text and python form and
    is also available as ``Task.quality``. (See also:
    ``Task.antennacharacteristics``, ``Task.mean``, ``Task.mean_rms``, ``Task.rms``,
    ``Task.rms_rms``, ``Task.npeaks``, ``Task.npeaks_rms``,
    ``Task.homogeneity_factor`` - the latter is printend and is a useful
    hint if something is wrong)

    One can specify the maximum number of blocks in a chunk that are
    allowed to fail a quality criterium (``maxblocksflagged``), before
    the entrie chunk is discarded. The way quality checking (see
    module :func:qualitycheck) works, is that each chunk is divided
    into blocks. If a few of these blocks have a mean or rms that
    deviates too much from the others, they will be flagged. Also, a
    noise level will be determined where one expects roughly one peak
    per block above that noise for Gaussian noise. If the actual
    number of peaks is ``spikeexcess`` above that number, flag the
    block (and perhaps the entire chunk)

    Flagged blocks can be easily inspeced using the task method
    ``Task.qplot`` (``qplot`` for quality plot).

    If you see an outputline like this::

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


    **Sections, Chunks, Blocks**

    The terminology is as follows: the smallest unit is a block. If
    stride>1 only every nth block will be read in to do a double fft.

    ``stride*blocks`` will be one chunk of data. The size of a chunk
    determines the resolution of an FFT.
    """
    WorkSpace = WorkSpace

    def init(self):
        """
        Initialize the task
        """

        # Start initialization
        # Nothing to do
        self.current_file_number = 0
        return

    def run(self):
        """Run the program.
        """

        # Skip calculation if file already exists and is asked for
        if self.load_if_file_exists and os.path.exists(self.spectrum_file):
            print "#AverageSpectrum: File", self.spectrum_file, "exists. Skipping calculation Loading Task.power from file!"
            self.power = cr.hArrayRead(self.spectrum_file)
            return

        del self.output_files
        del self.starttimes_unix
        del self.starttimes_local
        del self.starttimes_UTC

        self.quality = []
        self.antennas_used = set()
        self.antennacharacteristics = {}
        self.spectrum_file_bin = os.path.join(self.spectrum_file, "data.bin")
        self.dostride = (self.stride > 1)
        if not self.addantennas and (self.stride > 1 or self.doublefft):
            print "ERROR: averagespectrum - Can't use'addantennas=True with self.stride>1 or doublefft=True"
            return
#        self.power.getHeader()["FREQUENCY_INTERVAL"]=self.delta_frequency
#        self.updateHeader(self.power,["nofAntennas","nspectraadded","nspectraadded_per_antenna","filenames","antennas_used","antennas","antenna_list"],fftLength="speclen",blocksize="fullsize",filename="spectrum_file")
        if not self.doublefft:
            self.frequencies.fillrange(self.start_frequency / 10 ** 6, self.delta_frequency / 10 ** 6)
        if self.stride > 1 or self.doublefft:
            if self.calc_incoherent_sum:
                self.calc_incoherent_sum = False

        if self.calc_incoherent_sum:
            self.ntimeseries_data_added_per_chunk.fill(0)
            self.incoherent_sum.fill(0)
            self.power.par.incoherent_sum = self.incoherent_sum

        if self.calc_dynspec:
            if self.stride > 1 or self.doublefft:
                print "ERROR: averagespectrum - Can't calculate dynspec with self.stride>1 or doublefft=True"
                return
            self.all_dynspec.fill(0)
            self.all_cleanspec.fill(0)
            self.all_avspec.fill(0)

        self.t0 = time.clock()  # ; print "Reading in data and doing a double FFT."

        clearfile = True
        dataok = True
        initialround = True

        fftplan = cr.FFTWPlanManyDftR2c(self.blocklen * self.nblocks, 1, 1, 1, 1, 1, cr.fftw_flags.ESTIMATE)

        if self.doplot:
            self.plot_finish.plotfiles = []

        npass = self.nchunks * self.stride

        self.nantennas_total_last_file = 0
        self.nantennas_total = 0

        for fnumber in range(self.file_start_number, self.nfiles):
            self.current_file_number = fnumber
            self.ws.update(workarrays=False, nonexport=False)  # since the current_file_number was changed, make an update to get the correct file, etc.

            self.starttimes_unix.append(self.starttime_unix)
            self.starttimes_local.append(self.starttime_local)
            self.starttimes_UTC.append(self.starttime_UTC)

            self.nspectraflagged = 0
            self.nspectraadded = 0
            self.nspectraadded_per_antenna.fill(0)
            self.nspectraflagged_per_antenna.fill(0)

            if self.calc_dynspec:
                self.dynspec.fill(0)
                self.cleanspec.fill(0)
                self.dynspec_time.fillrange(0, self.t_int_used)
                self.dynspec_nspectra_added.fill(0)

            self.count = 0
            self.nofAntennas = 0

            if self.doplot:
                self.plot_finish.filename = os.path.join(
                    self.output_subdir, self.root_filename + self.__taskname__ +
                    ("-" + self.plot_name + "-" if self.plot_name else ""))

            print "# Using File", str(self.current_file_number) + ":", self.filename
            if self.stride == 1:
                self.datafile["BLOCKSIZE"] = self.chunksize_used  # Setting initial block size
            else:
                self.datafile["BLOCKSIZE"] = self.blocklen  # Setting initial block size

            # Creating output directory, if needed
            if not os.path.exists(self.output_subdir):
                print "# AverageSpectrum: creating new output directory", self.output_subdir
                os.makedirs(self.output_subdir)

            # self.antenna_list[self.filename]=self.antennas
            antenna_output_index = -1
            for antenna in self.antennas:
                if self.addantennas:
                    antenna_output_index = 0  # always add to the same spectrum
                else:
                    antenna_output_index += 1  # keep output antennas separate
                rms = 0
                mean = 0
                npeaks = 0
                self.datafile["SELECTED_DIPOLES"] = [int(antenna)]
                antennaID = self.antennaIDs[antenna]
                print "# Start antenna =", antenna, "(ID=", str(antennaID) + ") -", npass, "passes:"

                # Loop over all chunks (determining spectal resolution)
                for nchunk in range(self.nchunks):
                    if self.dostride:
                        ofiles = []
                        ofiles2 = []
                        ofiles3 = []

                    # Loop over strides within chunk, if spectrum is too large
                    for offset in range(self.stride):

                        # Read data in: either one block, or several strided blocks

                        try:
                            if self.stride == 1:
                                self.tdata.read(self.datafile, "TIMESERIES_DATA", nchunk)
                            else:
                                blocks = range(offset + nchunk * self.nsubblocks, (nchunk + 1) * self.nsubblocks, self.stride)
                                self.tdata[...].read(self.datafile, "TIMESERIES_DATA", blocks)
                        except RuntimeError:
                            print "#Error reading chunk", nchunk
                            self.tdata.fill(0)

                        # Do a quality check of the data
                        if self.qualitycheck:
                            self.count = len(self.quality)
                            self.quality.append(
                                qualitycheck.CRQualityCheckAntenna(
                                    self.tdata,
                                    datafile=self.datafile,
                                    normalize=False,
                                    blockoffset=offset + nchunk * self.nsubblocks if self.doublefft else 0,
                                    spikeexcess=self.spikeexcess,
                                    spikyness=100000,
                                    maxpeak=self.maxpeak,
                                    rmsfactor=self.rmsfactor,
                                    rmsrange=self.rmsrange,
                                    meanfactor=self.meanfactor,
                                    count=self.count,
                                    chunk=nchunk
                                    )
                                )
                            if not self.quality_db_filename == "":
                                outfilepath = os.path.join(self.output_dir, self.quality_db_filename + ".txt")
                                qualitycheck.CRDatabaseWrite(outfilepath, self.quality[self.count])

                            mean += self.quality[self.count]["mean"]
                            rms += self.quality[self.count]["rms"]
                            npeaks += self.quality[self.count]["npeaks"]
                            dataok = (self.quality[self.count]["nblocksflagged"] <= self.maxblocksflagged)

                        if not dataok:
                            print " # Data flagged!"
                            break

                        # Randomize peaks in the time series data, if desired
                        if  self.qualitycheck and self.randomize_peaks and self.quality[self.count]["npeaks"] > 0:
                            lower_limit = self.quality[self.count]["mean"] - self.peak_rmsfactor * self.quality[self.count]["rms"]
                            upper_limit = self.quality[self.count]["mean"] + self.peak_rmsfactor * self.quality[self.count]["rms"]
                            self.tdata.randomizepeaks(lower_limit, upper_limit)

                        # Do first step of double fft, if required
                        if self.doublefft:
                            self.cdata.copy(self.tdata)
                            self.cdataT.doublefft1(self.cdata, self.fullsize, self.nblocks, self.blocklen, offset)

                            # Double FFT that does not fit in memory needs to write out scratch files to disk
                            if self.dostride:
                                ofile = self.tmpfilename + str(offset) + "a" + self.fileext
                                ofiles += [ofile]
                                self.cdata.writefilebinary(ofile)  # output of doublefft1 is in cdata ...

                    if dataok:
                        self.nspectraadded += 1
                        self.nspectraadded_per_antenna[antenna_output_index] += 1
                        if not antennaID in self.antennas_used:
                            self.antennas_used.add(antennaID)
                            self.nofAntennas += 1

                        # do second step of double fft, if required
                        if self.doublefft:
                            # Now sort the different blocks together (which requires a transpose over passes/strides)
                            for offset in range(self.stride):
                                if self.dostride:
                                    self.tmpspecT[...].readfilebinary(cr.Vector(ofiles), cr.Vector(int, self.stride, fill=offset) * (self.nblocks_section * self.blocklen))
                                    # This transpose is to make sure the blocks are properly interleaved
                                    cr.hTranspose(self.tmpspec, self.tmpspecT, self.stride, self.nblocks_section)
                                self.specT.doublefft2(self.tmpspec, self.nblocks_section, self.full_blocklen)
                                if self.dostride:
                                    ofile = self.tmpfilename + str(offset) + "b" + self.fileext
                                    self.specT.writefilebinary(ofile)
                                    ofiles2 += [ofile]
                                if (self.nspectraadded == 1):  # first chunk
                                    self.power.fill(0.0)
                                else:  # 2nd or higher chunk, so read data in and add new spectrum to it
                                    if self.dostride:
                                        self.power.readfilebinary(self.spectrum_file_bin, self.subspeclen * offset)
                                    self.power *= (self.nspectraadded - 1.0) / (self.nspectraadded)
                                if self.dostride:
                                    self.specT2[...].readfilebinary(cr.Vector(ofiles2), cr.Vector(int, self.stride, fill=offset) * (self.blocklen * self.nblocks_section))
                                    cr.hTranspose(self.spec, self.specT2, self.stride, self.blocklen)  # Make sure the blocks are properly interleaved
                                    if self.nspectraadded > 1:
                                        self.spec /= float(self.nspectraadded)
                                    self.power.spectralpower(self.spec)
                                else:  # no striding, data is allready fully in memory
                                    if self.nspectraadded > 1:
                                        self.specT /= float(self.nspectraadded)
                                    self.power.spectralpower(self.specT)
                                self.frequencies.fillrange((self.start_frequency + offset * self.delta_band) / 10 ** 6, self.delta_frequency / 10 ** 6)
                                self.updateHeader(self.power, ["nofAntennas", "nspectraadded", "nspectraflagged", "antennas_used", "quality"])
                                if self.store_spectra:
                                    self.power.write(self.spectrum_file, nblocks=self.nbands, block=offset, clearfile=clearfile)
                                clearfile = False
                                if self.doplot > 1 and offset == self.nbands / 2 and self.nspectraadded % self.plotskip == 0:
                                    self.power2[self.plot_antenna, max(self.power_size / 2 - self.plotlen, 0):min(self.power_size / 2 + self.plotlen, self.power_size)].plot()
                                    cr.plt.draw()

                        else:  # single FFT
                            #############################################################################
                            # Here begins the normal calculation of an average spectrum from single FFTs
                            ############################################################################
                            cr.hFFTWExecutePlan(self.fftdata, self.tdata, fftplan)

                            if self.calc_averagespectrum:
#                                if self.nspectraadded_per_antenna[antenna_output_index]>1:
#                                    self.fftdata/=float(self.nspectraadded_per_antenna[antenna_output_index])
#                                self.power2[antenna_output_index] *= (self.nspectraadded_per_antenna[antenna_output_index]-1.0)/(self.nspectraadded_per_antenna[antenna_output_index])
                                # AC: change spectralpower to spectralpower2. Take proper power (squaring), later sqrt
                                self.power2[antenna_output_index].spectralpower2(self.fftdata)
                                if self.doplot > 1 and antenna_output_index == 0 and self.nspectraadded_per_antenna[antenna_output_index] % self.plotskip == 0:
                                    self.power2[antenna_output_index, max(self.power_size / 2 - self.plotlen, 0):min(self.power_size / 2 + self.plotlen, self.power_size)].plot(
                                        title="nspec={0:5d}, mean={1:8.3f}, rms={2:8.3f}".format(
                                            self.nspectraadded_per_antenna[antenna_output_index],
                                            self.power2[antenna_output_index, max(self.power_size / 2 - self.plotlen, 0):min(self.power_size / 2 + self.plotlen, self.power_size)].mean().val(),
                                            self.power2[antenna_output_index, max(self.power_size / 2 - self.plotlen, 0):min(self.power_size / 2 + self.plotlen, self.power_size)].stddev().val()))
                                    cr.plt.draw()

                            if self.calc_dynspec:
                                self.current_time_step = nchunk / self.chunks_per_sect
                                self.dynspec[self.current_time_step].spectralpower(self.fftdata)
                                self.dynspec_nspectra_added[self.current_time_step] += 1

                            if self.calc_incoherent_sum:
                                self.incoherent_sum[nchunk].squareadd(self.tdata)
                                self.ntimeseries_data_added_per_chunk[nchunk] += 1

                    else:  # data not ok
                        self.nspectraflagged += 1
                        self.nspectraflagged_per_antenna[antenna_output_index] += 1

                    # end loop over chunks

                print "# End   antenna =", antenna, " Time =", time.clock() - self.t0, "s  nspectraadded =", self.nspectraadded, "nspectraflagged =", self.nspectraflagged

                # Now prepare writing the output for the different spectra

                if self.qualitycheck:
                    mean /= self.nchunks
                    rms /= self.nchunks
                    self.mean_antenna[self.nantennas_total] = mean
                    self.rms_antenna[self.nantennas_total] = rms
                    self.npeaks_antenna[self.nantennas_total] = npeaks
                    self.antennacharacteristics[antennaID] = {"mean": mean, "rms": rms, "npeaks": npeaks, "quality": self.quality[-self.nchunks:]}
                    print "#          mean =", mean, "rms =", rms, "npeaks =", npeaks
                    if not self.quality_db_filename == "":
                        f = open(self.quality_db_filename + ".py", "a")
                        f.write('antennacharacteristics["' + str(antennaID) + '"]=' + str(self.antennacharacteristics[antennaID]) + "\n")
                        f.close()

                self.nantennas_total += 1
                # End loop over antennas

            # Now prepare writing the output for the different spectra
            self.frequencies.fillrange((self.start_frequency) / 10 ** 6, self.delta_frequency / 10 ** 6)

            if self.qualitycheck and self.nantennas_total > 1:
                self.mean = cr.asvec(self.mean_antenna[self.nantennas_total_last_file:self.nantennas_total]).mean()
                self.mean_rms = cr.asvec(self.mean_antenna[self.nantennas_total_last_file:self.nantennas_total]).stddev(self.mean)
                self.rms = cr.asvec(self.rms_antenna[self.nantennas_total_last_file:self.nantennas_total]).mean()
                self.rms_rms = cr.asvec(self.rms_antenna[self.nantennas_total_last_file:self.nantennas_total]).stddev(self.rms)
                self.npeaks = cr.asvec(self.npeaks_antenna[self.nantennas_total_last_file:self.nantennas_total]).mean()
                self.npeaks_rms = cr.asvec(self.npeaks_antenna[self.nantennas_total_last_file:self.nantennas_total]).stddev(self.npeaks)
                self.homogeneity_factor = -self.npeaks_rms / 2. / self.npeaks if self.npeaks > 0 else 0.5
                self.homogeneity_factor -= self.rms_rms / 2. / self.rms if self.rms > 0 else 0.5
                self.homogeneity_factor += 1
                print "Mean values for all antennas in file: Task.mean =", self.mean, "+/-", self.mean_rms, "(Task.mean_rms)"
                print "RMS values for all antennas in file: Task.rms =", self.rms, "+/-", self.rms_rms, "(Task.rms_rms)"
                print "NPeaks values for all antennas in file: Task.npeaks =", self.npeaks, "+/-", self.npeaks_rms, "(Task.npeaks_rms)"
                print "Quality factor =", self.homogeneity_factor * 100, "%"
                self.updateHeader(self.power, ["mean", "mean_rms", "rms", "rms_rms", "npeaks", "npeaks_rms", "homogeneity_factor", "rms_antenna", "npeaks_antenna", "antennacharacteristics"],
                                  fftLength="speclen", blocksize="fullsize")

            if self.calc_averagespectrum:
                self.nspectraadded_per_antenna.max(1)
                self.power2[...] /= self.nspectraadded_per_antenna.vec()
                # AC: take square root to get RMS spectrum
                self.power2[...].sqrt()  # dimensions?
                self.updateHeader(self.power, ["nofAntennas", "nspectraadded", "nspectraadded_per_antenna", "filenames", "antennas_used", "quality"],
                                  fftLength="speclen", blocksize="fullsize")
                if self.store_spectra:
                    self.power.write(self.spectrum_file)  # same as power2, just nicer dimensions
                self.output_files["average_spectrum"].append(self.spectrum_file)
                if self.doplot:
                    title = ("Spectrum of Antenna #" + str(self.plot_antenna) if not self.addantennas else "Average Spectrum of Antennas") + "\n" + self.root_filename
                    self.power2[self.plot_antenna, 1:].plot(title=title)
                    self.plot_finish(name="-average_spectrum", same_row=False)
                    if self.plot_zoom_slice:
                        self.power2[self.plot_antenna, self.plot_zoom_slice].plot(title=title)
                        self.plot_finish(name="-average_spectrum_slice", same_row=True)

            if self.calc_incoherent_sum:
                # Normalize the incoherent time series power
                self.ntimeseries_data_added_per_chunk.max(1)
                self.incoherent_sum[...] /= self.ntimeseries_data_added_per_chunk.vec()
                if self.store_spectra:
                    self.incoherent_sum.write(self.incoherent_sum_file)
                self.output_files["incoherent_sum"].append(self.incoherent_sum_file)

            if self.calc_dynspec:
                self.updateHeader(self.dynspec, ["nofAntennas", "nspectraadded", "nspectraadded_per_antenna", "filenames", "antennas_used", "quality"],
                                  fftLength="speclen", blocksize="fullsize")
                self.dynspec_nspectra_added.max(1)
                self.dynspec[...] /= self.dynspec_nspectra_added.vec()  # normalize spectra
                self.avspec.fill(0.0)
                self.dynspec[...].addto(self.avspec)
                self.avspec /= self.nsects
                self.cleanspec.copy(self.dynspec)
                try:
                    self.cleanspec /= self.avspec
                except ValueError:
                    print "Error: avspec contains zeros"
                if self.store_spectra:
                    self.dynspec.write(self.dynspec_file)
                self.output_files["dynamic_spectrum"].append(self.dynspec_file)

                if self.doplot:
                    title = "Average Spectrum of all Antennas" + "\n" + self.root_filename
                    self.avspec[1:].plot(title=title)
                    self.plot_finish(name="-average_spectrum_all_antennas", same_row=True)
                    self.dynplot()
                    self.plot_finish(name="-dynamic_spectrum", same_row=True)
                    if self.plot_zoom_slice:
                        self.avspec[self.plot_zoom_slice].plot(title=title)
                        self.plot_finish(name="-average_spectrum_all_antennas_zoom", same_row=True)
                        self.dynplot(zoom_slice=self.plot_zoom_slice, zoom_x=self.plot_zoom_x)
                        self.plot_finish(name="-dynamic_spectrum", same_row=True)

                if self.calc_all_dynspec:
                    self.all_avspec += self.avspec
                    self.all_dynspec[fnumber].copy(self.avspec)

            self.nantennas_total_last_file = self.nantennas_total

            print "# End File", str(self.current_file_number) + ":", self.filename
        if self.qualitycheck and self.nantennas_total > 1:
            self.mean = cr.asvec(self.mean_antenna[:self.nantennas_total]).mean()
            self.mean_rms = cr.asvec(self.mean_antenna[:self.nantennas_total]).stddev(self.mean)
            self.rms = cr.asvec(self.rms_antenna[:self.nantennas_total]).mean()
            self.rms_rms = cr.asvec(self.rms_antenna[:self.nantennas_total]).stddev(self.rms)
            self.npeaks = cr.asvec(self.npeaks_antenna[:self.nantennas_total]).mean()
            self.npeaks_rms = cr.asvec(self.npeaks_antenna[:self.nantennas_total]).stddev(self.npeaks)
            self.homogeneity_factor = -self.npeaks_rms / 2. / self.npeaks if self.npeaks > 0 else 0.5
            self.homogeneity_factor -= self.rms_rms / 2. / self.rms if self.rms > 0 else 0.5
            self.homogeneity_factor += 1
            print "Mean values for all antennas: Task.mean =", self.mean, "+/-", self.mean_rms, "(Task.mean_rms)"
            print "RMS values for all antennas: Task.rms =", self.rms, "+/-", self.rms_rms, "(Task.rms_rms)"
            print "NPeaks values for all antennas: Task.npeaks =", self.npeaks, "+/-", self.npeaks_rms, "(Task.npeaks_rms)"
            print "Quality factor =", self.homogeneity_factor * 100, "%"

        if self.calc_all_dynspec:
            self.updateHeader(self.all_dynspec, ["nofAntennas", "nspectraadded", "nspectraadded_per_antenna", "filenames", "antennas_used", "quality", "nfiles"], fftLength="speclen", blocksize="fullsize")
            self.all_avspec /= self.nfiles
            self.all_cleanspec.copy(self.all_dynspec)
            try:
                self.all_cleanspec /= self.all_avspec
            except ValueError:
                print "Error: all_avspec contains zeros!"
            if self.store_spectra:
                self.all_dynspec.write(self.all_dynspec_file)
            self.output_files["all_dynamic_spectrum"].append(self.all_dynspec_file)

            if self.doplot:
                title = "Average Spectrum of all Antennas"
                self.dynplot(dynspec=self.all_dynspec, ylabel="Dataset #",
                             extent=(self.frequencies[0], self.frequencies[-1], 0, self.nfiles))
                self.plot_finish(name="-dynamic_spectrum_all_files", same_row=False)
                self.all_avspec[1:].plot(title=title)
                self.plot_finish(name="-average_spectrum_all_files", same_row=True)

                if self.plot_zoom_slice:
                    self.dynplot(dynspec=self.all_dynspec, ylabel="Dataset #",
                                 extent=(self.plot_zoom_x[0], self.plot_zoom_x[1], 0, self.nfiles),
                                 zoom_slice=self.plot_zoom_slice)
                    self.plot_finish(name="-dynamic_spectrum_all_files_zoom", same_row=True)
                    self.all_avspec[self.plot_zoom_slice].plot(title=title)
                    self.plot_finish(name="-average_spectrum_all_files_zoom", same_row=True)

        print "Finished - total time used:", time.clock() - self.t0, "s."
        print "To inspect flagged blocks, used 'Task.qplot(Nchunk)', where Nchunk is the first number in e.g. '#Flagged: chunk= ...'"
        print "These output files were created, use the folowing lines to read them back in:"
        for k, v in self.output_files.items():
            for f in v:
                print "f =", k, "= hArrayRead('" + f + "')"

    def qplot(self, entry=0, flaggedblock=0, block=-1, all=True):
        """
        If you see an output line like this::

          # Start antenna = 0 (ID= 2000000) - 375 passes:
          # Flagged: #0 chunk= 0 , Block    14: mean= 121.68, rel. rms= 970.1, npeaks=    0, spikyness=  -1.00, spikeexcess=  0.00   ['rms', 'mean']
          # Flagged: #0 chunk= 0 , Block    15: mean= 139.70, rel. rms=1049.7, npeaks=    0, spikyness=  -1.00, spikeexcess=  0.00   ['rms', 'mean']
          # Flagged: #2 chunk= 2 , Block    15: mean= 139.70, rel. rms=1049.7, npeaks=    0, spikyness=  -1.00, spikeexcess=  0.00   ['rms', 'mean']
          # Data flagged!
          # End   antenna = 0  Time = 14.825337 s  nspectraadded = 374 nspectraflagged = 1

        this will tell you that Antenna 2000000 was worked on (the 1st
        antenna in the data file) and the chunk 0 (blocks 14 and 15
        within that chunk) and chunk 2 contained some suspicious data
        (with and rms and mean which fluctuated too much). If you want
        to inspect this, you can call::

          >>> Task.qplot(0)

        This will plot the chunk and highlight the first flagged block
        (#14) in that chunk::

          >>> Task.qplot(0,1)

        would highlight the second flagged block (#15)

        If the chunks are too long to be entirely plotted, use::

          >>> Task.qplot(186,all=False).

        """
        quality_entry = self.quality[entry]
        filename = quality_entry["filename"]
        datafile = cr.open(filename)
        datafile["SELECTED_DIPOLES"] = [quality_entry["antenna"]]
        if block < 0 and flaggedblock < len(quality_entry["flaggedblocks"]):
            block = quality_entry["flaggedblocks"][flaggedblock]
            s = "flaggedblock # " + str(flaggedblock) + "/" + str(len(quality_entry["flaggedblocks"]) - 1)
        else:
            s = ""
        print "Filename:", filename, "block =", block, "blocksize =", quality_entry["blocksize"], s
        if all:
            datafile["BLOCKSIZE"] = quality_entry["size"]
            datafile["BLOCK"] = quality_entry["offset"] * quality_entry["blocksize"] / quality_entry["size"] if self.doublefft else quality_entry["chunk"]
            y0 = datafile["TIMESERIES_DATA"]
            y0.par.xvalues = datafile["TIME_DATA"]
            y0.par.xvalues.setUnit("mu", "")
            y0.plot()
        datafile["BLOCKSIZE"] = quality_entry["blocksize"]
        datafile["BLOCK"] = block
        y = datafile["TIMESERIES_DATA"]
        y.par.xvalues = datafile["TIME_DATA"]
        y.par.xvalues.setUnit("mu", "")
        y.plot(clf=not all)

    def dynplot(self, dynspec=None, plot_cleanspec=None, dmin=None, dmax=None, cmin=None, cmax=None, ylabel=None, xlabel=None, extent=None, zoom_slice=None, zoom_x=None):
        """
        Plot the dynamic spectrum. Provide the dynamic spectrum
        computed by the Task AverageSpectrum as input.

        ================ ==== ==============================================================================
        *plot_cleanspec* None If False, don't plot the cleaned spectrum (provided as dynspec.par.cleanspec).
        *dmin*                Minimum z-value (intensity) in dynamic spectrum to plot.
        *dmax*                Maximum z-value (intensity) in dynamic spectrum to plot.
        *cmin*                Minimum z-value (intensity) in clean dynamic spectrum to plot.
        *cmax*                Maximum z-value (intensity) in clean dynamic spectrum to plot.
        *y/xlabel*            Label of y and x-axis.
        *extent*              (x0,x1,y0,y1) = define extent/range of x and y axis (just for labelling)
        *zoom_slice*          slice(n0,n1) - Zoom into this slice of the data
        *zoom_x*              (nu0,nu1) - zoom_slice slice corresponds to these frequencies
        ================ ==== ==============================================================================

        Example::

          >>> tload "AverageSpectrum"; go
          >>> dsp=hArrayRead("Saturn.h5.dynspec.pcr")
          >>> Task.dynplot(dsp,cmin=2.2*10**-5,cmax=0.002,dmin=6.8,dmax=10)

        """
        if not dynspec:
            dynspec = self.dynspec

        if hasattr(dynspec, "par") and hasattr(dynspec.par, "cleanspec") and not plot_cleanspec == False:
            cleanspec = dynspec.par.cleanspec
        else:
            cleanspec = None

        hdr = dynspec.getHeader("AverageSpectrum")

        if not ylabel:
            ylabel = "+/- Time [ms]"

        if not xlabel:
            xlabel = "Frequency [MHz]"

        if not extent:
            if zoom_slice:
                extent = zoom_x + (hdr["start_time"] * 1000, hdr["end_time"] * 1000)
            else:
                extent = (hdr["start_frequency"] / 10 ** 6, hdr["end_frequency"] / 10 ** 6, hdr["start_time"] * 1000, hdr["end_time"] * 1000)

        dim = dynspec.shape()

        if zoom_slice:
            dim = (dim[-2], zoom_slice.stop - zoom_slice.start)
            scratch_array = cr.hArray(float, dim)

        npdynspec = np.zeros(dim)
        npcleanspec = np.zeros(dim)

        if zoom_slice:
            cr.hCopy(scratch_array[...], cleanspec[..., zoom_slice])
            cr.hCopy(npcleanspec, scratch_array)
            cr.hCopy(scratch_array[...], dynspec[..., zoom_slice])
            cr.hCopy(npdynspec, scratch_array)
        else:
            cr.hCopy(npcleanspec, cleanspec)
            cr.hCopy(npdynspec, dynspec)

        np.log(npdynspec, npdynspec)

        if cleanspec:
            cr.plt.subplot(1, 2, 1)

        cr.plt.title("Dynamic Spectrum")
        cr.plt.imshow(npdynspec, aspect='auto', cmap=cr.plt.cm.hot, origin='lower', vmin=dmin, vmax=dmax, extent=extent)
        cr.plt.xlabel(xlabel)
        cr.plt.ylabel(ylabel)

        try:
            print "dynspec: min=", math.log(dynspec.min().val()), "max=", math.log(dynspec.max().val()), "rms=", math.log(dynspec.stddev().val()), "(all in log)"
        except ValueError:
            print "dynspec: min=", dynspec.min().val(), "max=", dynspec.max().val(), "rms=", dynspec.stddev().val(), "(Cannot take log!)"

        if cleanspec:
            cr.plt.subplot(1, 2, 2)
            cr.plt.title("Cleaned Dynamic Spectrum")
            cr.plt.imshow(npcleanspec, aspect='auto', cmap=cr.plt.cm.hot, origin='lower', vmin=cmin, vmax=cmax, extent=extent)
            cr.plt.xlabel(xlabel)
            cr.plt.ylabel(ylabel)

            print "cleanspec: min=", cleanspec.min().val(), "max=", cleanspec.max().val(), "rms=", cleanspec.stddev().val()

        cr.plt.draw()

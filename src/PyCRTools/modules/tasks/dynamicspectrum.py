"""
Dynamic spectrum documentation
==============================
"""


"""
# ************************************************************************
# Replacements for new tbb.py
# ************************************************************************
(query-replace "nofAntennas" "NOF_DIPOLE_DATASETS" nil (point-at-eol) (point-max))
(query-replace "AntennaIDs" "DIPOLE_NAMES" nil (point-at-eol) (point-max))
(query-replace "sampleInterval" "SAMPLE_INTERVAL" nil (point-at-eol) (point-max))
 datafile["SAMPLE_INTERVAL"] -> datafile["SAMPLE_INTERVAL"][0]
(query-replace "datafile.hdr" "datafile.getHeader()" nil (point-at-eol) (point-max))
(query-replace "fftLength" "FFTSIZE" nil (point-at-eol) (point-max))
(query-replace "blocksize" "BLOCKSIZE" nil (point-at-eol) (point-max))
(query-replace "block" "BLOCK" 1 (point-at-eol) (point-max))
(query-replace "[\"Fx\"]" "[\"TIMESERIES_DATA\"]" nil (point-at-eol) (point-max))
(query-replace "[\"Time\"]" "[\"TIME_DATA\"]" nil (point-at-eol) (point-max))
(query-replace "[\"selectedAntennasID\"]" "[\"SELECTED_DIPOLES\"]" nil (point-at-eol) (point-max))
(query-replace "[\"selectedAntennas\"]" "[\"SELECTED_DIPOLES\"]" nil (point-at-eol) (point-max))
(query-replace "[\"Frequency\"]" "[\"FREQUENCY_DATA\"]" nil (point-at-eol) (point-max))
(query-replace "datafile.filesize" "datafile[\"DATA_LENGTH\"][0]" nil (point-at-eol) (point-max))


"""
# import pdb; pdb.set_trace()

from pycrtools import *
import pycrtools.tasks as tasks
import pycrtools.qualitycheck as qualitycheck
import time
# from pycrtools import IO

# Defining the workspace parameters


def dynamicspectrum_getfile(ws):
    """
    To produce an error message in case the file does not exist
    """
    if ws.file_start_number < len(ws.filenames):
        f = open(ws.filenames[ws.file_start_number])
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


#    files = [f for f in files if test.search(f)]

class DynamicSpectrum(tasks.Task):
    """
    The function will calculate a dynamic spectrum from a list of
    files and a series of antennas (all averaged into one
    spectrum).

    The task will do a basic quality check of each time series data
    set and only average good data.

    The desired frequency resolution is provided with the parameter
    delta_nu, but this will be rounded off to the nearest value using
    channel numbers of a power of 2. The time resolution will also be
    rounded to give an integer number of blocks which Nyquist sample
    the rounded frequency resolution.

    The resulting spectrum is stored in the array ``Task.dynspec`` and
    written to disk as an hArray with parameters stored in the header
    dict (use ``getHeader('DynamicSpectrum')`` to retrieve this.)

    This spectrum can be read back and viewed with ``Task.dynspec``.

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

    Flagged blocks can be easily inspeced using the task method
    Task.qplot (qplot for quality plot).

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
    (#514) in that chunk::

      >>> Task.qplot(186,1)

    would highlight the second flagged block (which does not exist here).

    If the chunks are too long to be entirely plotted, use::

      >>> Task.qplot(186,all=False).
    """
    parameters = dict(

        filefilter=dict(default="$LOFARSOFT/data/lofar/RS307C-readfullsecondtbb1.h5",
                        doc="Unix style filter (i.e., with ``*``, ~, ``$VARIABLE``, etc.), to describe all the files to be processed."),

        file_start_number=dict(default=0,
                               doc="Integer number pointing to the first file in the ``filenames`` list with which to start. Can be changed to restart a calculation."),

        datafile=dict(default=dynamicspectrum_getfile,
                      export=False,
                      doc="Data file object pointing to raw data."),

        doplot=dict(default=False,
                    doc="Plot current spectrum while processing."),

        plotlen=dict(default=2 ** 12,
                     doc="How many channels ``+/-`` the center value to plot during the calculation (to allow progress checking)."),

        plotskip=dict(default=1,
                      doc="Plot only every ``plotskip``-th spectrum, skip the rest (should not be smaller than 1)."),

        plot_center=dict(default=0.5,
                         doc="Center plot at this relative distance from start of vector (0=left end, 1=right end)."),

        plot_start=dict(default=lambda self: max(int(self.speclen * self.plot_center) - self.plotlen, 0),
                        doc="Start plotting from this sample number."),

        plot_end=dict(default=lambda self: min(int(self.speclen * self.plot_center) + self.plotlen, self.speclen),
                      doc="End plotting before this sample number."),

        plot_finish=dict(default=lambda self: plotfinish(doplot=self.doplot, plotpause=False),
                         doc="Function to be called after each plot to determine whether to pause or not (see ::func::plotfinish)"),

        plot_name=dict(default="",
                       doc="Extra name to be added to plot filename."),


        delta_nu=dict(default=10 ** 4,
                      doc="Desired frequency resolution - will be rounded off to get powers of 2x ``blocklen``",
                      unit="Hz"),

        delta_t=dict(default=10 ** -3,
                     doc="Desired time resolution - will be rounded off to get integer number of spectral chunks",
                     unit="s"),

        maxnantennas=dict(default=1,
                          doc="Maximum number of antennas per file to sum over (also used to allocate some vector sizes)."),

        maxnchunks=dict(default=128,
                        doc="Maximum number of spectral chunks to include in dynamic spectrum."),

        maxblocksflagged=dict(default=2,
                              doc="Maximum number of blocks that are allowed to be flagged before the entire spectrum of the chunk is discarded."),

        stride=dict(default=1,
                    doc="If ``stride > 1`` skip (``stride - 1``) blocks."),

        tmpfileext=dict(default=".pcr",
                        doc="Extension of ``filename`` for temporary data files (e.g., used if ``stride > 1``.)",
                        export=False),

        tmpfilename=dict(default="tmp",
                         doc="Root filename for temporary data files.",
                         export=False),

        filenames=dict(default=lambda self: listFiles(self.filefilter),
                       doc="List of filenames of data file to read raw data from."),

        output_dir=dict(default="",
                        doc="Directory where output file is to be written to."),

        output_filename=dict(default=lambda self: (os.path.split(self.filenames[0])[1] if len(self.filenames) > 0 else "unknown") + ".dynspec" + self.tmpfileext,
                             doc="Filename (without directory, see ``output_dir``) to store the final spectrum."),

        spectrum_file=dict(default=lambda self: os.path.join(os.path.expandvars(os.path.expanduser(self.output_dir)), self.output_filename),
                           doc="Complete filename including directory to store the final spectrum."),

        qualitycheck=dict(default=True,
                          doc="Perform basic qualitychecking of raw data and flagging of bad data sets."),

        quality_db_filename=dict(default="qualitydatabase",
                                 doc="Root filename of log file containing the derived antenna quality values (uses '.py' and '.txt' extension)."),

        quality=dict(default=[],
                     doc="A list containing quality check information about every large chunk of data that was read in. Use ``Task.qplot(Entry#, flaggedblock=nn)`` to plot blocks in question.",
                     export=False,
                     output=True),

        antennacharacteristics=dict(default={},
                                    doc="A dict with antenna IDs as key, containing quality information about every antenna.",
                                    export=False,
                                    output=True),

        mean_antenna=dict(default=lambda self: hArray(float, [self.maxnantennas], name="Mean per Antenna"),
                          doc="Mean value of time series per antenna.",
                          output=True),

        rms_antenna=dict(default=lambda self: hArray(float, [self.maxnantennas], name="RMS per Antenna"),
                         doc="RMS value of time series per antenna.",
                         output=True),

        npeaks_antenna=dict(default=lambda self: hArray(float, [self.maxnantennas], name="Number of Peaks per Antenna"),
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
                 doc="Mean of rms time series values of all antennas.",
                 output=True),

        rms_rms=dict(default=0,
                     doc="RMS of rms of mean time series values of all antennas.",
                     output=True),

        homogeneity_factor=dict(default=0,
                                doc="``=1-(rms_rms/rms+ npeaks_rms/npeaks)/2`` - this describes the homogeneity of the data processed. A ``homogeneity_factor=1`` means that all antenna data were identical, a low factor should make one wonder if something went wrong.",
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

        #------------------------------------------------------------------------
        # Derived parameters

        blocklen=dict(default=lambda self: min(2 ** int(round(log(1. / self.delta_nu / self.samplerate, 2))), self.filesize / self.stride),
                      doc="The size of a block used for the FFT, limited by filesize.",
                      unit="Sample"),

        block_duration=dict(default=lambda self: self.samplerate * self.blocklen,
                            doc="The length of a block in time units.",
                            unit="s"),

        speclen=dict(default=lambda self: self.blocklen / 2 + 1,
                     doc="Length of one spectrum.",
                     unit="Channels"),

        samplerate=dict(default=lambda self: self.datafile["SAMPLE_INTERVAL"][0],
                        doc="Length in time of one sample in raw data set.",
                        unit="s"),

        filesize=dict(default=lambda self: self.datafile["DATA_LENGTH"][0],
                      doc="Length of file.",
                      unit="Samples"),

        fullsize=dict(default=lambda self: self.nblocks * self.blocklen * self.nchunks,
                      doc="The full length of the raw time series data used for the dynamic spectrum.",
                      unit="Samples"),

        delta_nu_used=dict(default=lambda self: 1 / (self.samplerate * self.blocklen),
                           doc="Actual frequency resolution of dynamic spectrum",
                           unit="Hz"),

        delta_t_used=dict(default=lambda self: self.samplerate * self.blocklen * self.nblocks,
                          doc="Actual frequency resolution of dynamic spectrum",
                          unit="s"),

        max_nblocks=dict(default=lambda self: int(floor(self.filesize / self.stride / self.blocklen)),
                         doc="Maximum number of blocks in file."),

        nblocks=dict(default=lambda self: int(min(max(round(self.delta_t / self.block_duration), 1), self.max_nblocks)),
                     doc="Number of blocks of length blocklen integrated per spectral chunk. The blocks are also used for quality checking."),

        chunklen=dict(default=lambda self: self.nblocks * self.blocklen,
                      doc="Length of one chunk of data used to calculate one time step in dynamic spectrum.",
                      unit="Samples"),

        sectlen=dict(default=lambda self: self.chunklen * self.stride,
                     doc="Length of one section of data used to extract one chunk, i.e. on time step in dynamic spectrum.",
                     unit="Samples"),

        sectduration=dict(default=lambda self: self.sectlen * self.samplerate,
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

        nchunks=dict(default=lambda self: min(int(floor(self.filesize / self.sectlen)), self.maxnchunks),
                     doc="Maximum number of spectral chunks to average"),

        start_frequency=dict(default=lambda self: self.freqs[0],
                             doc="Start frequency of spectrum",
                             unit="Hz"),

        end_frequency=dict(default=lambda self: self.freqs[-1],
                           doc="End frequency of spectrum",
                           unit="Hz"),

        delta_frequency=dict(default=lambda self: (self.end_frequency - self.start_frequency) / (self.speclen - 1.0),
                             doc="Separation of two subsequent channels in final spectrum"),

        #------------------------------------------------------------------------

        nantennas=dict(default=lambda self: len(self.antennas),
                       doc="The actual number of antennas available for calculation in the file (``< maxnantennas``)."),

        nantennas_start=dict(default=0,
                             doc="Start with the *n*-th antenna in each file (see also ``nantennas_stride``). Can be used for selecting odd/even antennas."),

        antenna_list=dict(default={},
                          doc="List of antenna indices used as input from each filename.",
                          output=True),

        nantennas_stride=dict(default=1,
                              doc="Take only every *n*-th antenna from antennas list (see also ``nantennas_start``). Use 2 to select odd/even."),

        nspectraadded=dict(default=lambda self: hArray(int, [self.nchunks], fill=0, name="Spectra added"),
                           doc="Number of spectra added per chunk.",
                           output=True),

        nspectraflagged=dict(default=lambda self: hArray(int, [self.nchunks], fill=0, name="Spectra flagged"),
                             doc="Number of spectra flagged per chunk.",
                             output=True),

        antennas=dict(default=lambda self: hArray(range(min(self.datafile["NOF_DIPOLE_DATASETS"], self.maxnantennas))),
                      doc="Antennas from which to select initially for the current file."),

        antennas_used=dict(default=lambda self: set(),
                           doc="A set of antenna names that were actually included in the average spectrum, excluding the flagged ones.",
                           output=True),

        antennaIDs=dict(default=lambda self: ashArray(hArray(self.datafile["DIPOLE_NAMES"])[self.antennas]),
                        doc="Antenna IDs to be selected from for current file."),

        nantennas_total=dict(default=0,
                             doc="Total number of antennas that were processed (flagged or not) in this run.",
                             output=True),

        header=dict(default=lambda self: self.datafile.getHeader(),
                    doc="Header of datafile.",
                    export=False),

        freqs=dict(default=lambda self: self.datafile["FREQUENCY_DATA"],
                   export=False),

        # Now define all the work arrays used internally
        data=dict(workarray=True,
                  doc="Main input array of raw data.",
                  default=lambda self: hArray(float, [self.nblocks, self.blocklen], name="data", header=self.header)),

        fftdata=dict(workarray=True,
                     doc="Main input array of raw data.",
                     default=lambda self: hArray(complex, [self.nblocks, self.speclen], name="fftdata", header=self.header)),

        frequencies=dict(workarray=True,
                         doc="Frequency axis for final power spectrum.",
                         default=lambda self: hArray(float, [self.speclen], name="Frequency", units=("M", "Hz"), header=self.header)),

        dynspec = dict(workarray=True,
                       doc="Resulting dynamic spectrum.",
                       default=lambda self: hArray(float, [self.nchunks, self.speclen], fill=0.0, name="Dynamic Spectrum", par=dict(logplot="y"), header=self.header, xvalues=self.frequencies)),

        cleanspec = dict(workarray=True,
                         doc="Resulting cleaned dynamic spectrum.",
                         default=lambda self: hArray(float, [self.nchunks, self.speclen], fill=0.0, name="Clean Dynamic Spectrum", par=dict(logplot="y"), header=self.header, xvalues=self.frequencies)),

        avspec = dict(workarray=True,
                      doc="Average spectrum over all times.",
                      default=lambda self: hArray(float, [self.speclen], fill=0.0, name="Average Spectrum", par=dict(logplot="y"), header=self.header, xvalues=self.frequencies)),

        npcleanspec = dict(workarray=True,
                           doc="Resulting clean dynamic spectrum in a numpy array for plotting.",
                           default=lambda self: np.zeros([self.nchunks, self.speclen]))
        )

    def run(self):
        """Run the program.
        """
        self.quality = []
        self.antennas_used = set()
        self.antennacharacteristics = {}
        self.spectrum_file_bin = os.path.join(self.spectrum_file, "data.bin")
        self.dostride = (self.stride > 1)
        self.nspectraflagged.fill(0)
        self.nspectraadded.fill(0)
        self.count = 0
        self.nofAntennas = 0
        self.nantennas_total = 0
        self.dynspec.par.avspec = self.avspec
        self.dynspec.par.cleanspec = self.cleanspec
        self.updateHeader(self.dynspec, ["nofAntennas", "nspectraadded", "filenames", "antennas_used", "nchunks"], delta_t="delta_t_used", delta_nu="delta_nu_used", fftLength="speclen", blocksize="blocklen", filename="spectrum_file")
        self.frequencies.fillrange((self.start_frequency) / 10 ** 6, self.delta_frequency / 10 ** 6)
        dataok = True

        self.t0 = time.clock()  # ; print "Reading in data and doing a double FFT."

        fftplan = FFTWPlanManyDftR2c(self.blocklen, 1, 1, 1, 1, 1, fftw_flags.ESTIMATE)

        if self.doplot:
            plt.ioff()

        original_file_start_number = self.file_start_number

        for fname in self.filenames[self.file_start_number:]:
            print "# Start File", str(self.file_start_number) + ":", fname
            self.ws.update(workarrays=False)  # since the file_start_number was changed, make an update to get the correct file
            self.datafile["BLOCKSIZE"] = self.blocklen  # Setting initial block size
            self.antenna_list[fname] = range(self.nantennas_start, self.nantennas, self.nantennas_stride)
            for iantenna in self.antenna_list[fname]:
                antenna = self.antennas[iantenna]
                rms = 0
                mean = 0
                npeaks = 0
                self.datafile["SELECTED_DIPOLES"] = [antenna]
                antennaID = self.antennaIDs[iantenna]
                print "# Start antenna =", antenna, "(ID=", str(antennaID) + "):"
                for nchunk in range(self.nchunks):
                    blocks = range(nchunk * self.blocks_per_sect, (nchunk + 1) * self.blocks_per_sect, self.stride)
                    self.data[...].read(self.datafile, "TIMESERIES_DATA", blocks)
                    if self.qualitycheck:
                        self.count = len(self.quality)
                        self.quality.append(qualitycheck.CRQualityCheckAntenna(self.data, datafile=self.datafile, normalize=False, spikeexcess=self.spikeexcess, spikyness=100000, rmsfactor=self.rmsfactor, meanfactor=self.meanfactor, chunk=nchunk, count=self.count, blockoffset=nchunk * self.blocks_per_sect))
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
                            self.nofAntennas += 1
                        if self.qualitycheck and self.randomize_peaks and self.quality[self.count]["npeaks"] > 0:
                            lower_limit = self.quality[self.count]["mean"] - self.peak_rmsfactor * self.quality[self.count]["rms"]
                            upper_limit = self.quality[self.count]["mean"] + self.peak_rmsfactor * self.quality[self.count]["rms"]
                            self.data.randomizepeaks(lower_limit, upper_limit)
                        hFFTWExecutePlan(self.fftdata[...], self.data[...], fftplan)
                        # self.fftdata[...].fftw(self.data[...])
                        if self.nspectraadded[nchunk] > 1:
                            self.fftdata /= float(self.nspectraadded[nchunk])
                        self.dynspec[nchunk:nchunk + 1] *= (self.nspectraadded[nchunk] - 1.0) / (self.nspectraadded[nchunk])
                        self.dynspec[nchunk:nchunk + 1, ...].spectralpower(self.fftdata[...])
                    # print "#  Time:",time.clock()-self.t0,"s for processing this chunk. Number of spectra added =",self.nspectraadded

                        if self.doplot > 2 and self.nspectraadded[nchunk] % self.plotskip == 0:
                            self.dynspec[nchunk, self.plot_start:self.plot_end].plot()
                            print "RMS of plotted spectrum=", self.dynspec[nchunk, self.plot_start:self.plot_end].stddev()
                            plt.draw()
                     # End for nchunk
                if self.doplot > 1:
                    self.avspec.fill(0.0)
                    self.dynspec[...].addto(self.avspec)
                    self.cleanspec.copy(self.dynspec)
                    self.cleanspec /= self.avspec
                    self.dynplot(self.dynspec)
                    plt.draw()

                print "# End   antenna =", antenna, " Time =", time.clock() - self.t0, "s  nspectraadded =", self.nspectraadded.sum(), "nspectraflagged =", self.nspectraflagged.sum()
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
            print "# End File", str(self.file_start_number) + ":", fname
            self.avspec.fill(0.0)
            self.dynspec[...].addto(self.avspec)
            self.cleanspec.copy(self.dynspec)
            self.cleanspec /= self.avspec
            self.updateHeader(self.dynspec, ["nofAntennas", "nspectraadded", "filenames", "antennas_used"])
            self.dynspec.write(self.spectrum_file)
            self.file_start_number += 1
        self.file_start_number = original_file_start_number  # reset to original value, so that the parameter file is correctly written.
        if self.qualitycheck:
            self.mean = asvec(self.mean_antenna[:self.nantennas_total]).mean()
            self.mean_rms = asvec(self.mean_antenna[:self.nantennas_total]).stddev(self.mean)
            self.rms = asvec(self.rms_antenna[:self.nantennas_total]).mean()
            self.rms_rms = asvec(self.rms_antenna[:self.nantennas_total]).stddev(self.rms)
            self.npeaks = asvec(self.npeaks_antenna[:self.nantennas_total]).mean()
            self.npeaks_rms = asvec(self.npeaks_antenna[:self.nantennas_total]).stddev(self.npeaks)
            self.homogeneity_factor = 1 - (self.npeaks_rms / self.npeaks + (self.rms_rms / self.rms if self.rms > 0 else 0)) / 2. if self.npeaks > 0 else 1 - (self.rms_rms / self.rms if self.rms > 0 else 0)
            print "Mean values for all antennas: Task.mean =", self.mean, "+/-", self.mean_rms, "(Task.mean_rms)"
            print "RMS values for all antennas: Task.rms =", self.rms, "+/-", self.rms_rms, "(Task.rms_rms)"
            print "NPeaks values for all antennas: Task.npeaks =", self.npeaks, "+/-", self.npeaks_rms, "(Task.npeaks_rms)"
            print "Quality factor =", self.homogeneity_factor * 100, "%"
        print "Finished - total time used:", time.clock() - self.t0, "s."
        print "To inspect flagged blocks, used 'Task.qplot(Nchunk)', where Nchunk is the first number in e.g. '184 - Mean=  3.98, RMS=...'"
        print "To read back the spectrum type: dsp=hArrayRead('" + self.spectrum_file + "')"
        print "To plot the spectrum, use 'Task.dynplot(dsp)'."
        if self.doplot:
            self.dynplot(self.dynspec)
            self.plot_finish(name=self.__taskname__ + self.plot_name)

    def dynplot(self, dynspec, plot_cleanspec=None, dmin=None, dmax=None, cmin=None, cmax=None):
        """
        Plot the dynamic spectrum. Provide the dynamic spectrum
        computed by the Task DynamicSpectrum as input.

        ================ ==== ==============================================================================
        *plot_cleanspec* None If False, don't plot the cleaned spectrum (provided as dynspec.par.cleanspec).
        *dmin*                Minimum z-value (intensity) in dynamic spectrum to plot.
        *dmax*                Maximum z-value (intensity) in dynamic spectrum to plot.
        *cmin*                Minimum z-value (intensity) in clean dynamic spectrum to plot.
        *cmax*                Maximum z-value (intensity) in clean dynamic spectrum to plot.
        ================ ==== ==============================================================================

        Example::

          >>> tload "DynamicSpectrum"
          >>> dsp=hArrayRead("Saturn.h5.dynspec.pcr")
          >>> Task.dynplot(dsp,cmin=2.2*10**-5,cmax=0.002,dmin=6.8,dmax=10)
        """
        if hasattr(dynspec, "par") and hasattr(dynspec.par, "cleanspec") and not plot_cleanspec == False:
            cleanspec = dynspec.par.cleanspec
            npcleanspec = np.zeros(cleanspec.shape())
        else:
            cleanspec = None
        hdr = dynspec.getHeader("DynamicSpectrum")
        npdynspec = np.zeros(dynspec.shape())
        hCopy(npdynspec, dynspec)
        np.log(npdynspec, npdynspec)
        if cleanspec:
            plt.subplot(1, 2, 1)
        plt.title("CDynamic Spectrum")
        plt.imshow(npdynspec, aspect='auto', cmap=plt.cm.hot, origin='lower', vmin=dmin, vmax=dmax,
                   extent=(hdr["start_frequency"] / 10 ** 6, hdr["end_frequency"] / 10 ** 6, hdr["start_time"] * 1000, hdr["end_time"] * 1000))
        print "dynspec: min=", log(dynspec.min().val()), "max=", log(dynspec.max().val()), "rms=", log(dynspec.stddev().val())
        plt.xlabel("Frequency [MHz]")
        plt.ylabel("+/- Time [ms]")
        if cleanspec:
            hCopy(npcleanspec, cleanspec)
            plt.subplot(1, 2, 2)
            plt.title("Cleaned Dynamic Spectrum")
            plt.imshow(npcleanspec, aspect='auto', cmap=plt.cm.hot, origin='lower', vmin=cmin, vmax=cmax,
                    extent=(hdr["start_frequency"] / 10 ** 6, hdr["end_frequency"] / 10 ** 6, hdr["start_time"] * 1000, hdr["end_time"] * 1000))
            print "cleanspec: min=", cleanspec.min().val(), "max=", cleanspec.max().val(), "rms=", cleanspec.stddev().val()
            plt.xlabel("Frequency [MHz]")
            plt.ylabel("+/- Time [ms]")
        plt.draw()

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
        datafile = open(filename)
        datafile["SELECTED_DIPOLES"] = [quality_entry["antenna"]]
        if block < 0 and flaggedblock < len(quality_entry["flaggedblocks"]):
            block = quality_entry["flaggedblocks"][flaggedblock]
            s = "flaggedblock # " + str(flaggedblock) + "/" + str(len(quality_entry["flaggedblocks"]) - 1)
        else:
            s = ""
        print "Filename:", filename, "block =", block, "blocksize =", quality_entry["blocksize"], s
        if all:
            datafile["BLOCKSIZE"] = quality_entry["size"]
            datafile["BLOCK"] = quality_entry["chunk"]
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

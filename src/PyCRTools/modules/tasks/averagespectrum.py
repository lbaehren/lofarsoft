"""
Task: Average spectrum
======================

Usage::

  >>> sp=cr.hArrayRead('tmpspec.dat')
  >>> make_frequencies(sp)
  >>> t=task()
  >>> r=t(sp)

  >>> current_task=False
  >>> import tasks

  >>> import tasks.averagespectrum as avspec
  >>> av=avspec.averagespectrum(antennas_end=2)

  >>> import tasks.averagespectrum
  >>> tt=avspec.ttest(y=2)

  >>> import tasks.averagespectrum as avspec
  >>> ws=avspec.WorkSpace(antennas_end=2)
  >>> ws.stride=2
  >>> del ws.stride
  >>> ws

  >>> ws=tasks.WorkSpace(**args)


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
#import pdb; pdb.set_trace()

#from pycrtools import *
import pycrtools as cr
from pycrtools.tasks import shortcuts as sc
from pycrtools import tasks
from pycrtools import qualitycheck
import time
#from pycrtools import IO

#Defining the workspace parameters

def make_frequencies(spectrum,offset=-1,frequencies=None,setxvalues=True):
    """Calculates the frequencies for the calculated spectrum (outdated)
    """
    hdr=spectrum.par.hdr
    if offset<0 and hdr.has_key("nbands"):
        mult=hdr["nbands"]
    else:
        mult=1
    if offset<0: offset=0
    if hdr.has_key("subspeclen"):
        l=hdr["subspeclen"]
        if hdr["stride"]==1: l/=2
    else:
        l=len(spectrum)
    if frequencies==None:
        frequencies=cr.hArray(float,[l*mult],name="Frequency",units=("M","Hz"),header=hdr)
    frequencies.fillrange((hdr["start_frequency"]+offset*hdr["delta_band"])/10**6,hdr["delta_frequency"]/10**6)
    if setxvalues:
        spectrum.par.xvalues=frequencies
    return frequencies

#def averagespectrum_calcfrequencies(self):
#    """Calculates the frequencies for the calculated spectrum.
#    """


def averagespectrum_getfile(ws):
    """
    To produce an error message in case the file does not exist
    """
    if ws.file_start_number < len(ws.filenames):
        f=open(ws.filenames[ws.file_start_number])
        if ws.lofarmode:
            print "Setting ANTENNA_SET=",ws.lofarmode,"in the data file!"
            f["ANTENNA_SET"]=ws.lofarmode
        return f
    else:
        print "ERROR: File "+ws.filefilter+" not found!"
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

class WorkSpace(tasks.WorkSpace(taskname="AverageSpectrum")):
    """
    """
    parameters = dict(

        filefilter = sc.p_("$LOFARSOFT/data/lofar/RS307C-readfullsecondtbb1.h5",
                           "Unix style filter (i.e., with ``*``, ~, ``$VARIABLE``, etc.), to describe all the files to be processed."),

        file_start_number = dict(default=0,
                                 doc="Integer number pointing to the first file in the ``filenames`` list with which to start. Can be changed to restart a calculation."),

        load_if_file_exists = dict(default=False,
                                   doc="If average spectrum file (``spectrumfile``) already exists, skip calculation and load existing file."),

        datafile = dict(default=averagespectrum_getfile,
                        export=False,
                        doc="Data file object pointing to raw data."),

        lofarmode = dict(default="LBA_OUTER",
                         doc="Which ANTENNA_SET/LOFAR mode was used (HBA_DUAL/LBA_OUTER/LBA_INNER,etc.). If not None or False it will set the ANTENNA_SET parameter in the datafile to this value."),

        addantennas = dict(default=True,
                           doc="If True, add all antennas into one average spectrum, otherwise keep them separate in memory per file."),

        doplot = dict(default=False,
                      doc="Plot current spectrum while processing."),

        plotlen = dict(default=2**12,
                       doc="How many channels ``+/-`` the center value to plot during the calculation (to allow progress checking)."),

        plotskip = dict(default=1,
                        doc="Plot only every 'plotskip'-th spectrum, skip the rest (should not be smaller than 1)."),

        stride_n = sc.p_(0,
                         "if >0 then divide the FFT processing in ``n=2**stride_n blocks``. This is slower but uses less memory."),

        doublefft = dict(default=False,
                         doc="if True split the FFT into two, thereby saving memory."),

        calc_incoherent_sum = dict(default=False,
                                   doc="Calculate the incoherent sum of all antennas (doublefft=False). See incoherent_sum for result."),

        ntimeseries_data_added_per_chunk = dict(default=lambda ws:cr.hArray(int,[ws.nchunks]),
                                                doc="Number of chunks added in each bin for incoherent sum"),

        delta_nu = dict(default=1000,
                        doc="Frequency resolution",
                        unit="Hz"),

        blocklen = dict(default=lambda ws:2**int(round(log(ws.blocksize,2))/2),
                        doc="The size of a block being read in if stride>1, otherwise ``self.blocklen*self.nblocks`` is the blocksize."),

        maxnchunks = dict(default=-1,
                          doc="Maximum number of chunks of raw data to integrate spectrum over (-1 = all)."),

        maxblocksflagged = dict(default=0,
                                doc="Maximum number of blocks that are allowed to be flagged before the entire spectrum of the chunk is discarded. If =0 then flag a chunk if any block is deviant."),

        stride = dict(default=lambda ws: 2**ws.stride_n if ws.doublefft else 1,
                      doc="If ``stride>1`` divide the FFT processing in ``n=stride`` blocks."),

        tmpfileext = dict(default=".pcr",
                          doc="Extension of filename for temporary data files (e.g., used if ``stride > 1``.)"),

        tmpfilename = dict(default="tmp",
                           doc="Root filename for temporary data files."),

        filenames = dict(default=lambda ws:listFiles(ws.filefilter),
                         doc="List of filenames of data file to read raw data from."),

        output_dir = dict(default="",
                          doc="Directory where output file is to be written to."),

        output_filename = dict(default=lambda self:(os.path.split(self.filenames[0])[1] if len(self.filenames)>0 else "unknown")+".spec"+self.tmpfileext,
                               doc="Filename (without directory, see output_dir) to store the final spectrum."),

        spectrum_file = dict(default=lambda self:os.path.join(os.path.expandvars(os.path.expanduser(self.output_dir)),self.output_filename),
                             doc="Complete filename including directory to store the final spectrum."),

        qualitycheck = dict(default=True,
                            doc="Perform basic qualitychecking of raw data and flagging of bad data sets."),

        quality_db_filename = dict(default="qualitydatabase",
                                   doc="Root filename of log file containing the derived antenna quality values (uses '.py' and '.txt' extension)."),

        quality = dict(default=[],
                       doc="A list containing quality check information about every large chunk of data that was read in. Use ``Task.qplot(Entry#,flaggedblock=nn)`` to plot blocks in question.",
                       export=False,
                       output=True),

        antennacharacteristics = dict(default={},
                                      doc="A dict with antenna IDs as key, containing quality information about every antenna.",
                                      export=False,
                                      output=True),

        mean_antenna = dict(default=lambda self: cr.hArray(float,[self.nantennas], name="Mean per Antenna"),
                            doc="Mean value of time series per antenna.",
                            output=True),

        rms_antenna = dict(default= lambda self: cr.hArray(float,[self.nantennas], name="RMS per Antenna"),
                           doc="RMS value of time series per antenna.",
                           output=True),

        npeaks_antenna = dict(default= lambda self: cr.hArray(float,[self.nantennas], name="Number of Peaks per Antenna"),
                              doc="Number of peaks of time series per antenna.",
                              output=True),

        mean = dict(default=0,
                    doc="Mean of mean time series values of all antennas.",
                    output=True),

        mean_rms = dict(default=0,
                        doc="RMS of mean of mean time series values of all antennas.",
                        output=True),

        npeaks = dict(default=0,
                      doc="Mean of number of peaks all antennas.",
                      output=True),

        npeaks_rms = dict(default=0,
                          doc="RMS of npeaks over all antennas.",
                          output=True),

        rms = dict(default=0,
                   doc="Mean of RMS time series values of all antennas.",
                   output=True),

        rms_rms = dict(default=0,
                       doc="RMS of rms of mean time series values of all antennas.",
                       output=True),

        homogeneity_factor = dict(default=0,
                                  doc="``=1-(rms_rms/rms+ npeaks_rms/npeaks)/2`` - this describes the homogeneity of the data processed. A ``homogeneity_factor = 1`` means that all antenna data were identical, a low factor should make one wonder if something went wrong.",
                                  output=True),

        maxpeak = dict(default=7,
                       doc="Maximum height of the maximum in each block (in sigma,i.e. relative to rms) before quality is failed."),

        spikeexcess = dict(default=20,
                           doc="Set maximum allowed ratio of detected over expected peaks per block to this level (1 is roughly what one expects from Gaussian noise)."),

        rmsfactor = dict(default=3,
                         doc="Factor by which the RMS is allowed to change within one chunk of time series data before it is flagged."),

        rmsrange = dict(default=None,
                        doc="Tuple with absolute min/max values of the rms of the time series that are allowed before a block is flagged. Will be igrored if None."),

        meanfactor = dict(default=3,
                          doc="Factor by which the mean is allowed to change within one chunk of time series data before it is flagged."),

        randomize_peaks = dict(default=True,
                           doc="Replace all peaks in time series data which are 'rmsfactor' above or below the mean with some random number in the same range."),

        peak_rmsfactor = dict(default=5,
                              doc="At how many sigmas above the mean will a peak be randomized."),

        start_frequency = dict(default=lambda ws:ws.freqs[0],
                               doc="Start frequency of spectrum",
                               unit="Hz"),

        end_frequency = dict(default=lambda ws:ws.freqs[-1],
                             doc="End frequency of spectrum",
                             unit="Hz"),

        delta_frequency = sc.p_(lambda ws:(ws.end_frequency-ws.start_frequency)/(ws.speclen-1.0),
                                "Separation of two subsequent channels in final spectrum"),

        delta_band = dict(default=lambda ws:(ws.end_frequency-ws.start_frequency)/ws.stride*2,
                          doc="Frequency width of one section/band of spectrum",
                          unit="Hz"),

        full_blocklen = dict(default=lambda ws:ws.blocklen*ws.stride,
                             doc="Full block length",
                             unit="Samples"),

        fullsize = sc.p_(lambda ws:ws.nblocks*ws.full_blocklen,
                         "The full length of the raw time series data used for one spectral chunk."),

        nblocks = dict(default=lambda ws:2**int(round(log(ws.blocksize/ws.full_blocklen,2))),
                       doc="Number of blocks of lenght ``blocklen`` the time series data set is split in. The blocks are also used for quality checking."),

        nbands = dict(default=lambda ws:(ws.stride+1)/2,
                      doc="Number of bands in spectrum which are separately written to disk, if ``stride > 1``. This is one half of stride, since only the lower half of the spectrum is written to disk."),

        subspeclen = dict(default=lambda ws:ws.blocklen*ws.nblocks,
                          doc="Size of one section/band of the final spectrum"),

        nsamples_data = dict(default=lambda ws:float(ws.fullsize)/10**6,
                             doc="Number of samples in raw antenna file",
                             unit="MSamples"),

        size_data = dict(default=lambda ws:float(ws.fullsize)/1024/1024*16,
                         doc="Number of samples in raw antenna file",
                         unit="MBytes"),

        nantennas = dict(default=lambda ws:len(ws.antennas),
                         doc="The number of antennas to be calculated in one file."),

        nantennas_file = dict(default=lambda ws:ws.datafile["NOF_DIPOLE_DATASETS"],
                              doc="The actual number of antennas available for calculation in the file."),

        antennas_start = dict(default=0,
                              doc="Start with the *n*-th antenna in each file (see also ``nantennas_stride``). Can be used for selecting odd/even antennas."),

        antennas_end = dict(default=-1,
                            doc="Maximum antenna number (plus one, zero based ...) to use in each file."),

        antennas_stride = dict(default=1,
                               doc="Take only every *n*-th antenna from antennas list (see also ``antennas_start``). Use 2 to select odd/even."),

        antennas = sc.p_(lambda ws:cr.hArray(range(min(ws.antennas_start,ws.nantennas_file-1),ws.nantennas_file if ws.antennas_end<0 else min(ws.antennas_end,ws.nantennas_file),ws.antennas_stride)),
                         "Array of index numbers for antennas to be processed from the current file."),

        antenna_list = dict(default={},
                            doc="List of antenna indices used as input from each filename.",
                            output=True),

        antennaIDs = sc.p_(lambda ws:cr.hArray(ws.datafile["DIPOLE_NAMES"]),
                           "Antenna IDs from the current file."),

        antennas_used = sc.p_(lambda ws:set(),
                              "A set of antenna names that were actually included in the average spectrum, excluding the flagged ones.",
                              output=True),

        nantennas_total = sc.p_(0,
                                "Total number of antennas that were processed (flagged or not) in this run.",
                                output=True),

        nchunks_max = dict(default=lambda ws:float(ws.datafile["DATA_LENGTH"][0])/ws.fullsize,
                           doc="Maximum number of spectral chunks to average"),

        nchunks = dict(default=lambda ws:min(ws.datafile["DATA_LENGTH"][0]/ws.fullsize,ws.maxnchunks) if ws.maxnchunks>0 else ws.datafile["DATA_LENGTH"][0]/ws.fullsize,
                       doc="Number of spectral chunks that are averaged"),

        nspectraadded = sc.p_(lambda ws:0,
                              "Number of spectra added at the end.",
                              output=True),

        nspectraadded_per_antenna = sc.p_(lambda ws:Vector(int,ws.nantennas,fill=0),
                                          "Number of spectra added per antenna.",
                                          output=True),

        nspectraflagged = sc.p_(lambda ws:0,
                                "Number of spectra flagged and not used.",
                                output=True),

        nspectraflagged_per_antenna = sc.p_(lambda ws:Vector(int,ws.nantennas,fill=0),
                                         "Number of spectra flagged and not used per antenna.",
                                         output=True),

        delta_t = sc.p_(lambda ws:ws.datafile["SAMPLE_INTERVAL"][0],
                        "Sample length in raw data set.",
                        "s"),

        blocksize = sc.p_(lambda ws:int(min(1./ws.delta_nu/ws.delta_t,ws.datafile["DATA_LENGTH"][0])),
                          "The desired blocksize for a single FFT. Can be set directly, otherwise estimated from delta_nu. The actual size will be rounded to give a power of two, hence, see ``blocksize_used` for the actual blocksize used."),

        blocksize_used = sc.p_(lambda ws:ws.blocklen*ws.nblocks,
                               "The blocksize one would use for a single FFT."),

        blocklen_section = sc.p_(lambda ws:ws.blocklen/ws.stride),

        nsubblocks = sc.p_(lambda ws:ws.stride*ws.nblocks),

        nblocks_section = sc.p_(lambda ws:ws.nblocks/ws.stride),

        speclen = sc.p_(lambda ws:ws.fullsize/2+1),

        header = sc.p_(lambda ws:ws.datafile.getHeader(),
                       "Header of datafile",
                       export=False),

        freqs = sc.p_(lambda ws:ws.datafile["FREQUENCY_DATA"],
                      export=False),


#Now define all the work arrays used internally

        frequencies = dict(workarray=True,
                           doc="Frequency axis for final power spectrum.",
                           default=lambda ws:cr.hArray(float,[ws.subspeclen/2 if ws.stride==1 else ws.subspeclen],name="Frequency",units=("M","Hz"),header=ws.header)),

        power_size = dict(default=lambda ws:ws.subspeclen/2 if ws.stride==1 else ws.subspeclen,
                          doc="Size of the output spectrum in the vector power"),

        power = dict(workarray=True,
                     doc="Resulting average power spectrum (or part thereof if ``stride > 1``)",
                     default=lambda ws:cr.hArray(float,[ws.power_size],name="Spectral Power",par=dict(logplot="y"),header=ws.header,xvalues=ws.frequencies) if ws.addantennas else cr.hArray(float,[ws.nantennas,ws.power_size],name="Spectral Power",par=dict(logplot="y"),header=ws.header,xvalues=ws.frequencies)),

#                cr.hArray(float,[ws.subspeclen],name="Spectral Power",xvalues=ws.frequencies,par=[("logplot","y")],header=ws.header)),
#Need to cerate frequencies, taking current subspec (stride) into account

        fdata = dict(workarray=True,
                     doc="main input and work array",
                     default=lambda ws:cr.hArray(float,[ws.nblocks,ws.blocklen],name="fdata",header=ws.header)),

        incoherent_sum = dict(doc="Incoherent sum of the power in all antennas (timeseries data).",
                              default=lambda ws:cr.hArray(float,[ws.nchunks,ws.blocksize],name="Power(t)",header=ws.header)),

        cdata = dict(workarray=True,
                     doc="main input and work array",
                     default=lambda ws:cr.hArray(complex,[ws.nblocks,ws.blocklen],name="cdata",header=ws.header)),

        cdata2 = dict(workarray=True,
                      doc="main input and work array",
                      default=lambda ws:cr.hArray(complex,[ws.nblocks*ws.blocklen/2+1],name="cdata2",header=ws.header)),

        cdataT = dict(workarray=True,
                      doc="Secondary input and work array",
                      default=lambda ws:cr.hArray(complex,[ws.blocklen,ws.nblocks],name="cdataT",header=ws.header)),

#Note, that all the following arrays have the same memory als cdata and cdataT

        tmpspecT = dict(workarray=True,
                        doc="Wrapper array for ``cdataT``",
                        default=lambda ws:cr.hArray(ws.cdataT.vec(),[ws.stride,ws.nblocks_section,ws.blocklen],header=ws.header)),

        tmpspec = dict(workarray=True,
                       doc="Wrapper array for ``cdata``",
                       default=lambda ws:cr.hArray(ws.cdata.vec(),[ws.nblocks_section,ws.full_blocklen],header=ws.header)),

        specT = dict(workarray=True,
                     doc="Wrapper array for ``cdataT``",
                     default=lambda ws:cr.hArray(ws.cdataT.vec(),[ws.full_blocklen,ws.nblocks_section],header=ws.header)),

        specT2 = dict(workarray=True,
                      doc="Wrapper array for ``cdataT``",
                      default=lambda ws:cr.hArray(ws.cdataT.vec(),[ws.stride,ws.blocklen,ws.nblocks_section],header=ws.header)),

        spec = dict(workarray=True,
                    doc="Wrapper array for ``cdata``",
                    default=lambda ws:cr.hArray(ws.cdata.vec(),[ws.blocklen,ws.nblocks],header=ws.header))
        )


#    files = [f for f in files if test.search(f)]

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
    also set the parameter ``blocksize``.

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

    Incoherent Beam
    ================

    With the option ``calc_incoherent_sum`` one can add the square of
    all time series data of all antennas into one long time series
    data (containing the power) for single FFTs. The resulting array
    Task.incoherent_sum has the length of the entire data set (of one
    antenna, of course) and has the dimensions
    [nchunks,chunksize=blocksize]. It will be saved on disk in
    sp.par.incoherent_sum.

    Quality Checking
    ================

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

    One can specify the maimum number of blocks in a chunk that are
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

    """
    WorkSpace = WorkSpace

    def init(self):
        """
        Initialize the task
        """

        # Start initialization
        #Nothing to do
        return


    def run(self):
        """Run the program.
        """
        #crfile=IO.open(filenames)

        #print "Time:",time.clock()-self.ws.t0,"s for set-up."

        #Skip calculation if file already exists and is asked for
        if self.load_if_file_exists and os.path.exists(self.spectrum_file):
            print "#AverageSpectrum: File",self.spectrum_file,"exists. Skipping calculation Loading Task.power from file!"
            self.power=cr.hArrayRead(self.spectrum_file)
            return

        self.quality=[]
        self.antennas_used=set()
        self.antennacharacteristics={}
        self.spectrum_file_bin=os.path.join(self.spectrum_file,"data.bin")
        self.dostride=(self.stride>1)
        self.nspectraflagged=0
        self.nspectraadded=0
        self.nspectraadded_per_antenna.fill(0)
        self.nspectraflagged_per_antenna.fill(0)
        if not self.addantennas and (self.stride>1 or self.doublefft):
            print "ERROR: averagespectrum - Can't use'addantennas=True with self.stride>1 or doublefft=True"
            return
        self.count=0
        self.nofAntennas=0
        self.nantennas_total=0
#        self.power.getHeader()["FREQUENCY_INTERVAL"]=self.delta_frequency
        self.updateHeader(self.power,["nofAntennas","nspectraadded","nspectraadded_per_antenna","filenames","antennas_used","antennas","antenna_list"],fftLength="speclen",blocksize="fullsize",filename="spectrum_file")
        if not self.doublefft:
            self.frequencies.fillrange(self.start_frequency/10**6,self.delta_frequency/10**6)
        if self.stride>1 or self.doublefft:
            if self.calc_incoherent_sum: self.calc_incoherent_sum=False
        if self.calc_incoherent_sum:
            self.ntimeseries_data_added_per_chunk.fill(0)
            self.incoherent_sum.fill(0)
            self.power.par.incoherent_sum=self.incoherent_sum

        self.t0=time.clock() #; print "Reading in data and doing a double FFT."

        clearfile=True
        dataok=True
        initialround=True

        fftplan = FFTWPlanManyDftR2c(self.blocklen*self.nblocks, 1, 1, 1, 1, 1, fftw_flags.ESTIMATE)

        if self.doplot:
            plt.ioff()

        npass = self.nchunks*self.stride
        original_file_start_number=self.file_start_number

        for fname in self.filenames[self.file_start_number:]:
            print "# Start File",str(self.file_start_number)+":",fname
            self.ws.update(workarrays=False) # since the file_start_number was changed, make an update to get the correct file
            if self.stride==1:
                self.datafile["BLOCKSIZE"]=self.blocklen*self.nblocks #Setting initial block size
            else:
                self.datafile["BLOCKSIZE"]=self.blocklen #Setting initial block size
            #self.antenna_list[fname]=self.antennas
            antenna_processed=-1
            for antenna in self.antennas:
                antenna_processed+=1
                rms=0; mean=0; npeaks=0
                self.datafile["SELECTED_DIPOLES"]=[int(antenna)]
                antennaID=self.antennaIDs[antenna]
                print "# Start antenna =",antenna,"(ID=",str(antennaID)+") -",npass,"passes:"
                for nchunk in range(self.nchunks):
                    #if self.nchunks>1: sys.stdout.write("*")
                    #print "#  Chunk ",nchunk,"/",self.nchunks-1,". Reading in data and doing a double FFT."
                    ofiles=[]; ofiles2=[]; ofiles3=[];
                    for offset in range(self.stride):
                        #if self.stride>1: sys.stdout.write(".")
                        #print "#    Pass ",offset,"/",self.stride-1,"Starting block=",offset+nchunk*self.nsubblocks
                        if self.stride==1:
                            self.fdata.read(self.datafile,"TIMESERIES_DATA",nchunk)
                        else:
                            blocks=range(offset+nchunk*self.nsubblocks,(nchunk+1)*self.nsubblocks,self.stride)
                            self.fdata[...].read(self.datafile,"TIMESERIES_DATA",blocks)
                        if self.qualitycheck:
                            self.count=len(self.quality)
                            self.quality.append(
                                qualitycheck.CRQualityCheckAntenna(
                                    self.fdata,
                                    datafile=self.datafile,
                                    normalize=False,
                                    blockoffset=offset+nchunk*self.nsubblocks if self.doublefft else 0,
                                    observatorymode=self.lofarmode,
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
                            if not self.quality_db_filename=="":
                                qualitycheck.CRDatabaseWrite(self.quality_db_filename+".txt",self.quality[self.count])
                            mean+=self.quality[self.count]["mean"]
                            rms+=self.quality[self.count]["rms"]
                            npeaks+=self.quality[self.count]["npeaks"]
                            dataok=(self.quality[self.count]["nblocksflagged"]<=self.maxblocksflagged)
                        if not dataok:
                            print " # Data flagged!"
                            break
                        #            print "Time:",time.clock()-self.t0,"s for reading."
                        if  self.qualitycheck and self.randomize_peaks and self.quality[self.count]["npeaks"]>0:
                            lower_limit=self.quality[self.count]["mean"]-self.peak_rmsfactor*self.quality[self.count]["rms"]
                            upper_limit=self.quality[self.count]["mean"]+self.peak_rmsfactor*self.quality[self.count]["rms"]
                            self.fdata.randomizepeaks(lower_limit,upper_limit)
                        self.cdata.copy(self.fdata)
                        if self.doublefft:
                            self.cdataT.doublefft1(self.cdata,self.fullsize,self.nblocks,self.blocklen,offset)
                        else:
                            #self.cdataT.fftw(self.cdata)
                            hFFTWExecutePlan(self.cdata2, self.fdata, fftplan)
                        #            print "Time:",time.clock()-self.t0,"s for 1st FFT."
                        if self.dostride and self.doublefft:
                            ofile=self.tmpfilename+str(offset)+"a"+self.tmpfileext
                            ofiles+=[ofile]
                            self.cdata.writefilebinary(ofile)  # output of doublefft1 is in cdata ...
                    #Now sort the different blocks together (which requires a transpose over passes/strides)
                    #print "Time:",time.clock()-self.t0,"s for 1st FFT now doing 2nd FFT."
                    if dataok:
                        self.nspectraadded+=1
                        self.nspectraadded_per_antenna[antenna_processed]+=1
                        if not antennaID in self.antennas_used:
                            self.antennas_used.add(antennaID)
                            self.nofAntennas+=1
                        if self.doublefft:  # do second step of double fft, if required
                            for offset in range(self.stride):
                                if self.dostride:
                                    #print "#    Offset",offset
                                    self.tmpspecT[...].readfilebinary(Vector(ofiles),Vector(int,self.stride,fill=offset)*(self.nblocks_section*self.blocklen))
                                    #This transpose it to make sure the blocks are properly interleaved
                                    cr.hTranspose(self.tmpspec,self.tmpspecT,self.stride,self.nblocks_section)
                                self.specT.doublefft2(self.tmpspec,self.nblocks_section,self.full_blocklen)
                                if self.dostride:
                                    ofile=self.tmpfilename+str(offset)+"b"+self.tmpfileext
                                    self.specT.writefilebinary(ofile)
                                    ofiles2+=[ofile]
                            #print "Time:",time.clock()-self.t0,"s for 2nd FFT now doing final transpose. Now finalizing (adding/rearranging) spectrum."                            for offset in range(self.nbands):
                                if (self.nspectraadded==1): # first chunk
                                    self.power.fill(0.0)
                                else: #2nd or higher chunk, so read data in and add new spectrum to it
                                    if self.dostride:
                                        self.power.readfilebinary(self.spectrum_file_bin,self.subspeclen*offset)
                                    self.power *= (self.nspectraadded-1.0)/(self.nspectraadded)
                                if self.dostride:
                                    #print "#    Offset",offset
                                    self.specT2[...].readfilebinary(Vector(ofiles2),Vector(int,self.stride,fill=offset)*(self.blocklen*self.nblocks_section))
                                    cr.hTranspose(self.spec,self.specT2,self.stride,self.blocklen) # Make sure the blocks are properly interleaved
                                    if self.nspectraadded>1:
                                        self.spec/=float(self.nspectraadded)
                                    self.power.spectralpower(self.spec)
                                else: # no striding, data is allready fully in memory
                                    if self.nspectraadded>1:
                                        self.specT/=float(self.nspectraadded)
                                    self.power.spectralpower(self.specT)
                                self.frequencies.fillrange((self.start_frequency+offset*self.delta_band)/10**6,self.delta_frequency/10**6)
                                self.updateHeader(self.power,["nofAntennas","nspectraadded","nspectraflagged","antennas_used","quality"])
                                self.power.write(self.spectrum_file,nblocks=self.nbands,block=offset,clearfile=clearfile)
                                clearfile=False
                                if self.doplot and offset==self.nbands/2 and self.nspectraadded%self.plotskip==0:
                                    self.power[max(len(self.power)/2-self.plotlen,0):min(len(self.power)/2+self.plotlen,len(self.power))].plot()
                                    #print "mean=",self.power[max(len(self.power)/2-self.plotlen,0):min(len(self.power)/2+self.plotlen,len(self.power))].mean()
                                    plt.draw()
                        else: # doublefft - i.e. here do just a single FFT
                            if self.addantennas:
                                if self.nspectraadded>1:
                                    self.cdata2/=float(self.nspectraadded)
                                self.power *= (self.nspectraadded-1.0)/(self.nspectraadded)
                                self.power.spectralpower(self.cdata2)
                                #print "#  Time:",time.clock()-self.t0,"s for processing this chunk. Number of spectra added =",self.nspectraadded
                                if self.doplot and self.nspectraadded%self.plotskip==0:
                                    self.power[max(self.power_size/2-self.plotlen,0):min(self.power_size/2+self.plotlen,self.power_size)].plot()
                                    print "mean=",self.power[max(self.power_size/2-self.plotlen,0):min(self.power_size/2+self.plotlen,self.power_size)].mean()
                                    plt.draw()
                            else: #keep antennas separately
                                if self.nspectraadded_per_antenna[antenna_processed]>1:
                                    self.cdata2/=float(self.nspectraadded_per_antenna[antenna_processed])
                                self.power[antenna_processed] *= (self.nspectraadded_per_antenna[antenna_processed]-1.0)/(self.nspectraadded_per_antenna[antenna_processed])
                                self.power[antenna_processed].spectralpower(self.cdata2)
                                if self.doplot and antenna_processed==0 and self.nspectraadded_per_antenna[antenna_processed]%self.plotskip==0:
                                    self.power[antenna_processed,max(self.power_size/2-self.plotlen,0):min(self.power_size/2+self.plotlen,self.power_size)].plot(
                                        title="nspec={0:5d}, mean={1:8.3f}, rms={2:8.3f}".format(
                                            self.nspectraadded_per_antenna[antenna_processed],
                                            self.power[antenna_processed,max(self.power_size/2-self.plotlen,0):min(self.power_size/2+self.plotlen,self.power_size)].mean().val(),
                                            self.power[antenna_processed,max(self.power_size/2-self.plotlen,0):min(self.power_size/2+self.plotlen,self.power_size)].stddev().val()))
                                    plt.draw()
                            if self.calc_incoherent_sum:
                                self.incoherent_sum[nchunk].squareadd(self.fdata)
                                self.ntimeseries_data_added_per_chunk[nchunk]+=1
                    else: #data not ok
                        self.nspectraflagged+=1
                        self.nspectraflagged_per_antenna[antenna_processed]+=1
                        #print "#  Time:",time.clock()-self.t0,"s for reading and ignoring this chunk.  Number of spectra flagged =",self.nspectraflagged
                print "# End   antenna =",antenna," Time =",time.clock()-self.t0,"s  nspectraadded =",self.nspectraadded,"nspectraflagged =",self.nspectraflagged
                self.frequencies.fillrange((self.start_frequency)/10**6,self.delta_frequency/10**6)
                self.updateHeader(self.power,["nofAntennas","nspectraadded","nspectraadded_per_antenna","filenames","antennas_used","quality"],fftLength="speclen",blocksize="fullsize")
                self.power.write(self.spectrum_file)
                if self.qualitycheck:
                    mean/=self.nchunks
                    rms/=self.nchunks
                    self.mean_antenna[self.nantennas_total]=mean
                    self.rms_antenna[self.nantennas_total]=rms
                    self.npeaks_antenna[self.nantennas_total]=npeaks
                    self.antennacharacteristics[antennaID]={"mean":mean,"rms":rms,"npeaks":npeaks,"quality":self.quality[-self.nchunks:]}
                    print "#          mean =",mean,"rms =",rms,"npeaks =",npeaks
                    f=open(self.quality_db_filename+".py","a")
                    f.write('antennacharacteristics["'+str(antennaID)+'"]='+str(self.antennacharacteristics[antennaID])+"\n")
                    f.close()
                self.nantennas_total+=1
            print "# End File",str(self.file_start_number)+":",fname
            self.file_start_number+=1
        self.file_start_number=original_file_start_number # reset to original value, so that the parameter file is correctly written.
        #Normalize the incoherent time series power
        if self.calc_incoherent_sum:
            self.incoherent_sum[...] /= self.ntimeseries_data_added_per_chunk.vec()
        if self.qualitycheck:
            self.mean=asvec(self.mean_antenna[:self.nantennas_total]).mean()
            self.mean_rms=asvec(self.mean_antenna[:self.nantennas_total]).stddev(self.mean)
            self.rms=asvec(self.rms_antenna[:self.nantennas_total]).mean()
            self.rms_rms=asvec(self.rms_antenna[:self.nantennas_total]).stddev(self.rms)
            self.npeaks=asvec(self.npeaks_antenna[:self.nantennas_total]).mean()
            self.npeaks_rms=asvec(self.npeaks_antenna[:self.nantennas_total]).stddev(self.npeaks)
            self.homogeneity_factor=1-(self.npeaks_rms/self.npeaks + self.rms_rms/self.rms)/2. if self.npeaks>0 else 1-(self.rms_rms/self.rms)
            print "Mean values for all antennas: Task.mean =",self.mean,"+/-",self.mean_rms,"(Task.mean_rms)"
            print "RMS values for all antennas: Task.rms =",self.rms,"+/-",self.rms_rms,"(Task.rms_rms)"
            print "NPeaks values for all antennas: Task.npeaks =",self.npeaks,"+/-",self.npeaks_rms,"(Task.npeaks_rms)"
            print "Quality factor =",self.homogeneity_factor * 100,"%"
        print "Finished - total time used:",time.clock()-self.t0,"s."
        print "To inspect flagged blocks, used 'Task.qplot(Nchunk)', where Nchunk is the first number in e.g. '#Flagged: chunk= ...'"
        print "To read back the spectrum type: sp=cr.hArrayRead('"+self.spectrum_file+"')"
        if self.doplot:
            plt.ion()


    def qplot(self,entry=0,flaggedblock=0,block=-1,all=True):
        """
        If you see an output line like this::

          # Start antenna = 0 (ID= 2000000) - 375 passes:
          #Flagged: #0 chunk= 0 , Block    14: mean= 121.68, rel. rms= 970.1, npeaks=    0, spikyness=  -1.00, spikeexcess=  0.00   ['rms', 'mean']
          #Flagged: #0 chunk= 0 , Block    15: mean= 139.70, rel. rms=1049.7, npeaks=    0, spikyness=  -1.00, spikeexcess=  0.00   ['rms', 'mean']
          #Flagged: #2 chunk= 2 , Block    15: mean= 139.70, rel. rms=1049.7, npeaks=    0, spikyness=  -1.00, spikeexcess=  0.00   ['rms', 'mean']
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
        quality_entry=self.quality[entry]
        filename=quality_entry["filename"]
        datafile=open(filename)
        datafile["SELECTED_DIPOLES"]=[quality_entry["antenna"]]
        if block<0 and flaggedblock<len(quality_entry["flaggedblocks"]):
            block=quality_entry["flaggedblocks"][flaggedblock]
            s="flaggedblock # "+str(flaggedblock)+"/"+str(len(quality_entry["flaggedblocks"])-1)
        else:
            s=""
        print "Filename:",filename,"block =",block,"blocksize =",quality_entry["blocksize"],s
        if all:
            datafile["BLOCKSIZE"]=quality_entry["size"]
            datafile["BLOCK"]=quality_entry["offset"]*quality_entry["blocksize"]/quality_entry["size"] if self.doublefft else quality_entry["chunk"]
            y0=datafile["TIMESERIES_DATA"]
            y0.par.xvalues=datafile["TIME_DATA"]
            y0.par.xvalues.setUnit("mu","")
            y0.plot()
        datafile["BLOCKSIZE"]=quality_entry["blocksize"]
        datafile["BLOCK"]=block
        y=datafile["TIMESERIES_DATA"]
        y.par.xvalues=datafile["TIME_DATA"]
        y.par.xvalues.setUnit("mu","")
        y.plot(clf=not all)




# class test1(tasks.Task):
#     """
#     Documentation of task - parameters will be added automatically
#     """
#     parameters = {x = sc.p_(None,"x-value - a positional parameter",positional=True),y = sc.p_(2,"y-value - a normal keyword parameter"),"xy":p_(lambda ws:ws.y*ws.x,"Example of a derived parameter.")}
#     def init(self):
#         print "Calling optional initialization routine - Nothing to do here."
#     def run(self):
#         print "Calling Run Function."
#         print "self.x=",self.x,"self.y=",self.y,"self.xz=",self.xy

# class test2(tasks.Task):
#     """
#     Documentation of task - parameters will be added automatically
#     """
#     def call(self,x,y=2,xy=lambda ws:ws.x*ws.y):
#         pass
#     def init(self):
#         print "Calling optional initialization routine - Nothing to do here."
#     def run(self):
#         print "Calling Run Function."
#         print "self.x=",self.x,"self.y=",self.y,"self.xz=",self.xy


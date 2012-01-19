"""
Calculate complex beams towards multiple directions. Also calculates
the average spectrum in each beam and for an incoherent beam.

Example::

  file=crfile(LOFARSOFT+"/data/lopes/example.event")
  tpar antenna_positions=dict(zip(file["antennaIDs"],file.getCalData("Position")))
  tpar pointings=[dict(az=178.9*deg,el=28*deg),dict(az=0*deg,el=90*deg,r=1)]
  tpar cal_delays=dict(zip(file["antennaIDs"],file.getCalData("Delay")))
  tpar phase_center=[-84.5346,36.1096,0]
  tpar FarField=True
  tpar NyquistZone=2
  tpar randomize_peaks=False

  #file=crfile(LOFARSOFT+"/data/lopes/2004.01.12.00:28:11.577.event")
  #file["SelectedAntennasID"]=[0]
  #fx0=file["TIMESERIES_DATA"]


  # ------------------------------------------------------------------------
  tload "BeamFormer"
  file=crfile(LOFARSOFT+"/data/lopes/2004.01.12.00:28:11.577.event")
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



#import pdb; pdb.set_trace()

import pycrtools as cr
#from pycrtools import *
from pycrtools.tasks import shortcuts as sc
from pycrtools import tasks
from pycrtools import qualitycheck
import time
import pytmf
import math

#from pycrtools import IO
"""
"""

#Defining the workspace parameters

deg=math.pi/180.
pi2=math.pi/2.

def makeGrid(AZ,EL,Distance,offset=5*deg):
    return [dict(az=AZ-offset, el=EL+offset,r=Distance),dict(az=AZ, el=EL+offset,r=Distance),dict(az=AZ+offset, el=EL+offset,r=Distance),
            dict(az=AZ-offset, el=EL,r=Distance),dict(az=AZ, el=EL,r=Distance),dict(az=AZ+offset, el=EL,r=Distance),
            dict(az=AZ-offset, el=EL-offset,r=Distance),dict(az=AZ, el=EL-offset,r=Distance),dict(az=AZ+offset, el=EL-offset,r=Distance)]


def getfile(ws):
    """
    To produce an error message in case the file does not exist
    """
    if ws.file_start_number < len(ws.filenames):
        f=cr.open(ws.filenames[ws.file_start_number])
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

class BeamFormer(tasks.Task):
    """
    The function will calculate multiple beams for a list of files and
    a series of antennas (all integrated into one compex spectrum per
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
    """

    parameters = dict(
#Beamformer parameters:
#------------------------------------------------------------------------
        pointings = dict(default=[dict(az=178.9*deg,el=28*deg),dict(az=0*deg,el=90*deg,r=1)],
                         doc="List of coordinate dicts (``{'az':az1,'el':elevation value}``) containing pointing directions for each beam on the sky."),

        cal_delays = dict(default={},
                          doc="A dict containing 'cable' delays for each antenna in seconds as values. Key is the antenna ID. Delays will be added to geometrical delays.",
                          unit="s"),

        phase_center = dict(default=[0,0,0],
                            doc="List or vector containing the *x*, *y*, *z* positions of the phase center of the array.",
                            unit="m"),

        FarField = dict(default=True,
                        doc="Form a beam towards the far field, i.e. no distance."),

        NyquistZone = dict(default=1,
                           doc="In which Nyquist zone was the data taken (e.g. ``NyquistZone = 2`` if data is from 100-200 MHz for 200 MHz sampling rate)."),

#------------------------------------------------------------------------
#Some standard parameters
#------------------------------------------------------------------------
        filefilter = sc.p_("$LOFARSOFT/data/lofar/RS307C-readfullsecondtbb1.h5",
                        "Unix style filter (i.e., with ``*``, ~, ``$VARIABLE``, etc.), to describe all the files to be processed."),

        file_start_number = dict(default=0,
                                 doc="Integer number pointing to the first file in the ``filenames`` list with which to start. Can be changed to restart a calculation."),

        datafile = dict(default=getfile,
                        export=False,
                        doc="Data file object pointing to raw data."),

        doplot = dict(default=False,
                      doc="Plot current spectrum while processing."),

        newfigure = sc.p_(True,"Create a new figure for plotting for each new instance of the task."),

        figure = sc.p_(None,"The matplotlib figure containing the plot",output=True),

        plotspec = dict(default=True,
                        doc="If **True** plot the beamformed average spectrum at the end, otherwise the time series."),

        plotlen = dict(default=2**12,
                       doc="How many channels ``+/-`` the center value to plot during the calculation (to allow progress checking)."),

        plotskip = dict(default=1,
                        doc="Plot only every ``plotskip``-th spectrum, skip the rest (should not be smaller than 1)."),

        plot_center = dict(default=0.5,
                           doc="Center plot at this relative distance from start of vector (0=left end, 1=right end)."),

        plot_pause = dict(default=True,
                          doc="Pause after every plot?"),

        plot_start = dict(default=lambda self: max(int(self.speclen*self.plot_center)-self.plotlen,0),
                          doc="Start plotting from this sample number."),

        plot_end = dict(default=lambda self: min(int(self.speclen*self.plot_center)+self.plotlen,self.speclen),
                        doc="End plotting before this sample number."),

        delta_nu = dict(default=1,
                        doc="Desired frequency resolution - will be rounded off to get powers of 2x ``blocklen``. Alternatively set blocklen directly.",
                        unit="Hz"),

        maxnantennas = dict(default=96,
                            doc="Maximum number of antennas per file to sum over (also used to allocate some vector sizes)."),

        maxnchunks = dict(default=2**15,
                          doc="Maximum number of spectral chunks to calculate."),

        maxchunklen = dict(default=10**25,
                           doc="Maximum length of one chunk of data that is kept in memory.",
                           unit="Samples"),

        maxblocksflagged = dict(default=2,
                                doc="Maximum number of blocks that are allowed to be flagged before the entire spectrum of the chunk is discarded."),

        stride = dict(default=1,
                      doc="If ``stride > 1`` skip (``stride - 1``) blocks."),

        tmpfileext = dict(default=".pcr",
                          doc="Extension of filename for temporary data files (e.g., used if ``stride > 1``.)",
                          export=False),

        tmpfilename = dict(default="tmp",
                           doc="Root filename for temporary data files.",
                           export=False),

        filenames = dict(default=lambda self:listFiles(self.filefilter),
                         doc="List of filenames of data file to read raw data from."),

        output_dir = dict(default="",
                          doc="Directory where output file is to be written to."),

        output_filename = dict(default=lambda self:(os.path.split(self.filenames[0])[1] if len(self.filenames)>0 else "unknown")+".beams"+self.tmpfileext,
                               doc="Filename (without directory, see ``output_dir``) to store the final spectrum."),

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

        mean_antenna = dict(default=lambda self: cr.hArray(float,[self.maxnantennas], name="Mean per Antenna"),
                            doc="Mean value of time series per antenna.",
                            output=True),

        rms_antenna = dict(default= lambda self: cr.hArray(float,[self.maxnantennas], name="RMS per Antenna"),
                           doc="RMS value of time series per antenna.",
                           output=True),

        npeaks_antenna = dict(default= lambda self: cr.hArray(float,[self.maxnantennas], name="Number of Peaks per Antenna"),
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
                          doc="RMS of ``npeaks`` over all antennas.",
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

        spikeexcess = dict(default=20,
                           doc="Set maximum allowed ratio of detected over expected peaks per block to this level (1 is roughly what one expects from Gaussian noise)."),

        rmsfactor = dict(default=2,
                         doc="Factor by which the RMS is allowed to change within one chunk of time series data before it is flagged."),

        meanfactor = dict(default=3,
                          doc="Factor by which the mean is allowed to change within one chunk of time series data before it is flagged."),

        randomize_peaks = dict(default=True,
                               doc="Replace all peaks in time series data which are ``rmsfactor`` above or below the mean with some random number in the same range."),

        peak_rmsfactor = dict(default=5,
                              doc="At how many sigmas above the mean will a peak be randomized."),

        nantennas = dict(default=lambda self:len(self.antennas),
                         doc="The actual number of antennas available for calculation in the file (``< maxnantennas``)."),

        nantennas_start = dict(default=0,
                               doc="Start with the *n*-th antenna in each file (see also ``nantennas_stride``). Can be used for selecting odd/even antennas."),

        antenna_list = dict(default={},
                            doc="List of antenna indices used as input from each filename.",
                            output=True),

        nantennas_stride = dict(default=1,
                                doc="Take only every *n*-th antenna from antennas list (see also ``nantennas_start``). Use 2 to select odd/even."),

        nspectraadded = sc.p_(lambda self:cr.hArray(int,[self.nchunks],fill=0,name="Spectra added"),
                           "Number of spectra added per chunk.",
                           output=True),

        nspectraflagged = sc.p_(lambda self:cr.hArray(int,[self.nchunks],fill=0,name="Spectra flagged"),
                             "Number of spectra flagged per chunk.",
                             output=True),

        antennas = sc.p_(lambda self:cr.hArray(range(min(self.datafile["NOF_DIPOLE_DATASETS"],self.maxnantennas))),
                      "Antennas from which to select initially for the current file."),

        antennas_used = sc.p_(lambda self:set(),
                           "A set of antenna names that were actually included in the average spectrum, excluding the flagged ones.",
                           output=True),

        antennaIDs = sc.p_(lambda self:ashArray(cr.hArray(self.datafile["DIPOLE_NAMES"])[self.antennas]),
                        "Antenna IDs to be selected from for current file."),

        nantennas_total = sc.p_(0,
                             "Total number of antennas that were processed (flagged or not) in this run.",
                             output=True),

        header = sc.p_(lambda self:self.datafile.getHeader(),
                    "Header of datafile",
                    export=False),

        antenna_positions = dict(default=lambda self:dict(map(lambda x: (x[0],x[1].array()),zip(self.datafile["DIPOLE_NAMES"],self.datafile["ANTENNA_POSITIONS"]))),
                                 doc="A dict containing *x*, *y*, *z*-Antenna positions for each antenna in seconds as values. Key is the antenna ID.",
                                 unit="m"),

#------------------------------------------------------------------------
# Derived parameters

        nbeams = dict(default=lambda self:len(self.pointings),
                      doc="Number of beams to calculate."),

        phase_center_array = dict(default=lambda self:cr.hArray(list(self.phase_center), name="Phase Center", units=("","m")),
                                  doc="List or vector containing the *x*, *y*, *z* positions of the phase center of the array.",
                                  unit="m"),

        blocklen = dict(default=lambda self:min(2**int(round(math.log(1./self.delta_nu/self.sample_interval,2))),min(self.maxchunklen,self.filesize/self.stride)),
                        doc="The size of a block used for the FFT, limited by filesize.",
                        unit="Sample"),

#        blocklen = dict(default=lambda self:min(2*int(round(1./self.delta_nu/self.sample_interval/2)),self.filesize/self.stride),doc="The size of a block used for the FFT, limited by filesize.",unit="Sample"),

        block_duration = dict(default=lambda self:self.sample_interval*self.blocklen,
                              doc="The length of a block in time units.",
                              unit="s"),

        speclen = sc.p_(lambda self:self.blocklen/2+1,
                     "Length of one spectrum.",
                     "Channels"),

        sample_interval = sc.p_(lambda self:self.datafile["SAMPLE_INTERVAL"][0],
                             "Length in time of one sample in raw data set.",
                             "s"),

        filesize = sc.p_(lambda self:self.datafile["DATA_LENGTH"][0],
                      "Length of file for one antenna.",
                      "Samples"),

        fullsize = sc.p_(lambda self:self.nblocks*self.blocklen*self.nchunks,
                      "The full length of the raw time series data used for the dynamic spectrum.",
                      "Samples"),

        delta_nu_used = dict(default=lambda self:1/(self.sample_interval*self.blocklen),
                             doc="Actual frequency resolution of dynamic spectrum",
                             unit="Hz"),

        max_nblocks = dict(default=lambda self:int(floor(self.filesize/self.stride/self.blocklen)),
                           doc="Maximum number of blocks in file."),

        chunklen = dict(default=lambda self:min(self.filesize/self.stride,self.maxchunklen),
                        doc="Length of one chunk of data treated in memory.",
                        unit="Samples"),

        nblocks = dict(default=lambda self:int(min(max(round(self.chunklen/self.blocklen),1),self.max_nblocks)),
                       doc="Number of blocks of length blocklen integrated per spectral chunk. The blocks are also used for quality checking."),

        sectlen = dict(default=lambda self:self.blocklen*self.nblocks*self.stride,
                       doc="Length of one section of data used to extract one chunk of data treated in memory.",
                       unit="Samples"),

        sectduration = dict(default=lambda self:self.sectlen*self.sample_interval,
                            doc="Length in time units of one section of data used to extract one chunk, i.e. on time step in dynamic spectrum.",
                            unit="s"),

        end_time = dict(default=lambda self:self.sectduration*self.nchunks,
                        doc="End of time axis.",
                        unit="s"),

        start_time = dict(default=lambda self:0,
                          doc="Start of time axis.",
                          unit="s"),

        blocks_per_sect = dict(default=lambda self:self.nblocks*self.stride,
                               doc="Number of blockse per section of data."),

        nchunks = dict(default=lambda self:min(int(floor(self.filesize/self.sectlen)),self.maxnchunks),
                       doc="Maximum number of spectral chunks to average."),

        start_frequency = dict(default=lambda self:self.datafile["FREQUENCY_RANGE"][0][0],
                               doc="Start frequency of spectrum.",
                               unit="Hz"),

        end_frequency = dict(default=lambda self:self.datafile["FREQUENCY_RANGE"][0][1],
                             doc="End frequency of spectrum.",
                             unit="Hz"),

        delta_frequency = sc.p_(lambda self:(self.end_frequency-self.start_frequency)/(self.speclen-1.0),
                             "Separation of two subsequent channels in final spectrum."),

#------------------------------------------------------------------------

#Now define all the work arrays used internally
        data = dict(workarray=True,
                    doc="Main input array of raw data.",
                    default=lambda self:cr.hArray(float,[self.nblocks,self.blocklen],name="data",header=self.header)),

        fftdata = dict(workarray=True,
                       doc="Main input array of raw data.",
                       default=lambda self:cr.hArray(complex,[self.nblocks,self.speclen],name="fftdata",header=self.header)),

        avspec = dict(workarray=True,
                      doc="Average spectrum in each beam.",
                      default=lambda self:cr.hArray(float,[self.nbeams,self.speclen],name="Average Spectrum",header=self.header,par=dict(logplot="y"),xvalues=self.frequencies)),

        avspec_incoherent = dict(workarray=True,
                                 doc="The average spectrum of all blocks in an incoherent beam (i.e. squaring before adding).",default=lambda self:
                                 cr.hArray(float,[self.speclen],name="Incoherent Average Spectrum",header=self.header,par=dict(logplot="y"),xvalues=self.frequencies)),

        tbeam_incoherent = dict(workarray=True,
                                doc="Contains the power as a function of time of an incorehent beam of all antennas (simply the sqaure of the ADC values added).",
                                default=lambda self: cr.hArray(float,[self.blocklen*self.nblocks],name="Incoherent Time Beam",header=self.header)),

        beams = dict(workarray=True,
                     doc="Output array containing the FFTed data for each beam.",
                     default=lambda self:cr.hArray(complex,[self.nblocks,self.nbeams,self.speclen],name="beamed FFT",header=self.header,par=dict(logplot="y"),xvalues=self.frequencies)),

        pointingsXYZ = dict(default=lambda self:cr.hArray(float,[self.nbeams,3],fill=[item for sublist in [convert(coords,"CARTESIAN") for coords in self.pointings] for item in sublist],name="Beam Direction XYZ"),
                            doc="Array of shape ``[nbeams,3]`` with *x*, *y*, *z* positions for each beam on the sky."),

        phases = dict(workarray=True,
                      doc="Complex phases for each beam and each freqeuncy channel used to calculate complex weights for beamforming.",
                      default=lambda self:cr.hArray(float,[self.nbeams,self.speclen],name="Phases",header=self.header)),

        weights = dict(workarray=True,
                       doc="Complex weights for each beam and each freqeuncy channel used to calculate beams.",
                       default=lambda self:cr.hArray(complex,[self.nbeams,self.speclen],name="Weights",header=self.header)),

        delays = dict(workarray=True,
                      doc="Contains the geometric delays for the current antenna for each beam.",
                      default=lambda self:cr.hArray(float,[self.nbeams],name="Delays",header=self.header)),

        antpos = dict(workarray=True,
                      doc="Cartesian coordinates of the current antenna relative to the phase center of the array.",
                      default=lambda self:cr.hArray(float,[3],name="Delays",header=self.header,units=("","m")),
                      unit="m"),

        frequencies = dict(workarray=True,
                           doc="Frequency axis for the final power spectrum.",
                           default=lambda self:cr.hArray(float,[self.speclen],name="Frequency",units=("","Hz"),header=self.header))
)

    def run(self):
        """Run the program.
        """
        self.quality=[]
        self.antennas_used=set()
        self.antennacharacteristics={}
        self.spectrum_file_bin=os.path.join(self.spectrum_file,"data.bin")
        self.dostride=(self.stride>1)
        self.nspectraflagged.fill(0)
        self.nspectraadded.fill(0)
        self.count=0
        self.NOF_DIPOLE_DATASETS=0
        self.nantennas_total=0
        self.beams.setHeader(FREQUENCY_INTERVAL=self.delta_frequency)
        self.beams.par.avspec=self.avspec
        self.beams.par.avspec_incoherent=self.avspec_incoherent
        self.beams.par.tbeam_incoherent=self.tbeam_incoherent

        self.updateHeader(self.beams,["NOF_DIPOLE_DATASETS","nspectraadded","filenames","antennas_used","nchunks"],delta_nu="delta_nu_used",FFTSIZE="speclen",BLOCKSIZE="blocklen",filename="spectrum_file")
        self.frequencies.fillrange((self.start_frequency),self.delta_frequency)

        dataok=True
        clearfile=True

        self.t0=time.clock() #; print "Reading in data and doing a double FFT."

        fftplan = cr.FFTWPlanManyDftR2c(self.blocklen, 1, 1, 1, 1, 1, cr.fftw_flags.ESTIMATE)

        if self.doplot:
            cr.plt.ioff()
            if self.newfigure and not self.figure:
                self.figure=cr.plt.figure()

        original_file_start_number=self.file_start_number
        self.beams.fill(0)
        self.avspec.fill(0)
        self.avspec_incoherent.fill(0)
        self.tbeam_incoherent.fill(0)
        for fname in self.filenames[self.file_start_number:]:
            print "# Start File",str(self.file_start_number)+":",fname
            self.ws.update(workarrays=False) # since the file_start_number was changed, make an update to get the correct file
            self.datafile["BLOCKSIZE"]=self.blocklen #Setting initial block size
            self.antenna_list[fname]=range(self.nantennas_start,self.nantennas, self.nantennas_stride)
            for iantenna in self.antenna_list[fname]:
                antenna=self.antennas[iantenna]
                rms=0; mean=0; npeaks=0
                antennaID=self.antennaIDs[iantenna]
                self.datafile["SELECTED_DIPOLES"]=[antennaID]
                print "# Start antenna =",antenna,"(ID=",str(antennaID)+"):"

                self.antpos=cr.hArray(copy=self.antenna_positions[antennaID]); #print "Antenna position =",self.antpos
                self.antpos -= self.phase_center_array; #print "Relative antenna position =",self.antpos

                #Calculate the geometrical delays needed for beamforming
                cr.hGeometricDelays(self.delays,self.antpos,self.pointingsXYZ, self.FarField)

                #Add the cable/calibration delay
                cal_delay=self.cal_delays.setdefault(antennaID,0.0); #print "Delay =",cal_delay
                self.delays+=cal_delay; #print "Total delay =",self.delays

                self.phases.delaytophase(self.frequencies,self.delays)
                self.weights.phasetocomplex(self.phases)
                #cr.hGeometricWeights(self.weights,self.frequencies,self.antpos,self.pointings_cartesian,self.FarField)

                for nchunk in range(self.nchunks):
                    blocks=range(nchunk*self.blocks_per_sect,(nchunk+1)*self.blocks_per_sect,self.stride)
                    self.data[...].read(self.datafile,"TIMESERIES_DATA",blocks)
                    if self.qualitycheck:
                        self.count=len(self.quality)
                        self.quality.append(qualitycheck.CRQualityCheckAntenna(self.data,datafile=self.datafile,normalize=False,spikeexcess=self.spikeexcess,spikyness=100000,rmsfactor=self.rmsfactor,meanfactor=self.meanfactor,chunk=self.count,blockoffset=nchunk*self.blocks_per_sect))
                        if not self.quality_db_filename=="":
                            qualitycheck.CRDatabaseWrite(self.quality_db_filename+".txt",self.quality[self.count])
                            mean+=self.quality[self.count]["mean"]
                            rms+=self.quality[self.count]["rms"]
                            npeaks+=self.quality[self.count]["npeaks"]
                            dataok=(self.quality[self.count]["nblocksflagged"]<=self.maxblocksflagged)
                    if not dataok:
                        print " # Data flagged!"
                        self.nspectraflagged[nchunk]+=1
                    else:
                        self.nspectraadded[nchunk]+=1
                        if not antennaID in self.antennas_used:
                            self.antennas_used.add(antennaID)
                            self.NOF_DIPOLE_DATASETS+=1
                        if self.qualitycheck and self.randomize_peaks and self.quality[self.count]["npeaks"]>0:
                            lower_limit=self.quality[self.count]["mean"]-self.peak_rmsfactor*self.quality[self.count]["rms"]
                            upper_limit=self.quality[self.count]["mean"]+self.peak_rmsfactor*self.quality[self.count]["rms"]
                            self.data.randomizepeaks(lower_limit,upper_limit)
                        cr.hFFTWExecutePlan(self.fftdata, self.data[...], fftplan)
                        #self.fftdata[...].fftw(self.data[...])
                        self.fftdata[...].nyquistswap(self.NyquistZone)
                        self.avspec_incoherent.spectralpower2(self.fftdata[...])
                        self.tbeam_incoherent.squareadd(self.data)
                        if self.nspectraadded[nchunk]>1:
                            self.fftdata/=float(self.nspectraadded[nchunk])
                        if self.NOF_DIPOLE_DATASETS==1:
                            self.beams.fill(0)
                        elif self.nchunks>1:
                            self.beams.readfilebinary(self.spectrum_file_bin,nchunk*self.speclen*self.nbeams*self.nblocks)
                        self.beams *= (self.nspectraadded[nchunk]-1.0)/(self.nspectraadded[nchunk])
                        self.beams[...].muladd(self.weights,self.fftdata[...])
                        self.avspec.spectralpower2(self.beams[...])
                        self.beams.write(self.spectrum_file,nblocks=self.nchunks,block=nchunk,clearfile=clearfile)
                        clearfile=False
                        #print "#  Time:",time.clock()-self.t0,"s for processing this chunk. Number of spectra added =",self.nspectraadded
                        if self.doplot>2 and self.nspectraadded[nchunk]%self.plotskip==0:
                            self.avspec[...,self.plot_start:self.plot_end].plot()
                            print "RMS of plotted spectra=",self.avspec[...,self.plot_start:self.plot_end].stddev()
                            self.plotpause()
                     #End for nchunk
                if self.doplot>1:
                    self.avspec[...].plot();self.plotpause()
                print "# End   antenna =",antenna," Time =",time.clock()-self.t0,"s  nspectraadded =",self.nspectraadded.sum(),"nspectraflagged =",self.nspectraflagged.sum()
                if self.qualitycheck:
                    mean/=self.nchunks
                    rms/=self.nchunks
                    self.mean_antenna[self.nantennas_total]=mean
                    self.rms_antenna[self.nantennas_total]=rms
                    self.npeaks_antenna[self.nantennas_total]=npeaks
                    if not self.quality_db_filename=="":
                        self.antennacharacteristics[antennaID]={"mean":mean,"rms":rms,"npeaks":npeaks,"quality":self.quality[-self.nchunks:]}
                        print "#          mean =",mean,"rms =",rms,"npeaks =",npeaks
                        f=open(self.quality_db_filename+".py","a")
                        f.write('antennacharacteristics["'+str(antennaID)+'"]='+str(self.antennacharacteristics[antennaID])+"\n")
                        f.close()
                self.nantennas_total+=1
            print "# End File",str(self.file_start_number)+":",fname
            self.updateHeader(self.beams,["NOF_DIPOLE_DATASETS","nspectraadded","filenames","antennas_used"])
            self.file_start_number+=1
        self.avspec /= self.nspectraadded
        self.avspec_incoherent /= self.nspectraadded
        self.tbeam_incoherent /= self.nspectraadded
        self.file_start_number=original_file_start_number # reset to original value, so that the parameter file is correctly written.
        if self.qualitycheck:
            self.mean=cr.asvec(self.mean_antenna[:self.nantennas_total]).mean()
            self.mean_rms=cr.asvec(self.mean_antenna[:self.nantennas_total]).stddev(self.mean)
            self.rms=cr.asvec(self.rms_antenna[:self.nantennas_total]).mean()
            self.rms_rms=cr.asvec(self.rms_antenna[:self.nantennas_total]).stddev(self.rms)
            self.npeaks=cr.asvec(self.npeaks_antenna[:self.nantennas_total]).mean()
            self.npeaks_rms=cr.asvec(self.npeaks_antenna[:self.nantennas_total]).stddev(self.npeaks)
            self.homogeneity_factor=1-(self.npeaks_rms/self.npeaks + self.rms_rms/self.rms)/2. if self.npeaks>0 else 1-(self.rms_rms/self.rms)
            print "Mean values for all antennas: Task.mean =",self.mean,"+/-",self.mean_rms,"(Task.mean_rms)"
            print "RMS values for all antennas: Task.rms =",self.rms,"+/-",self.rms_rms,"(Task.rms_rms)"
            print "NPeaks values for all antennas: Task.npeaks =",self.npeaks,"+/-",self.npeaks_rms,"(Task.npeaks_rms)"
            print "Quality factor =",self.homogeneity_factor * 100,"%"
        print "Finished - total time used:",time.clock()-self.t0,"s."
        print "To inspect flagged blocks, used 'Task.qplot(Nchunk)', where Nchunk is the first number in e.g. '184 - Mean=  3.98, RMS=...'"
        print "To read back the beam formed data type: bm=cr.hArrayRead('"+self.spectrum_file+"')"
        print "To calculate or plot the invFFTed times series of one block, use 'Task.tcalc(bm)' or 'Task.tplot(bm)'."
        if self.doplot:
            self.tplot(plotspec=self.plotspec)
            self.plotpause()
            cr.plt.ion()


    def tplot(self,beams=None,block=0,NyquistZone=1,doabs=True,smooth=0,mosaic=True,plotspec=False,xlim=None,ylim=None,recalc=False):
        """
        Take the result of the BeamForm task, i.e. an array of
        dimensions ``[self.nblocks,self.nbeams,self.speclen]``, do a
        NyquistSwap, if necessary, and then calculate an inverse
        FFT. If the time series (``self.tbeams``) has already been
        caclulated it will only be recalculated if explicitly asked
        for with ``recalc=True``.

        ============= ===== ===================================================================
        *beams*       None  Input array. Take self.beams from task, if None.
        *recalc*      False If true force a recalculation of the time series beam if it exists.
        *block*       0     Which  block to plot, or Ellipsis ('...') for all.
        *NyquistZone* 1     NyquistZone=2,4,... means flip the data left to right before FFT.
        *doabs*       True  Take the absolute of the timeseries before plotting.
        *smooth*      0     If > 0 smooth data with a Gaussian kernel of that size.
        *plotspec*    False If True plot the average spectrum instead.
        *xlim*              Tuple with minimum and maximum limits for the *x*-axis.
        *ylim*              Tuple with minimum and maximum limits for the *y*-axis.
        ============= ===== ===================================================================

        The final time series (all blocks) is stored in ``Task.tbeams``.
        """
        if beams==None:
            beams=self.beams
        hdr=beams.getHeader()
        if hdr.has_key("BeamFormer"):
            if NyquistZone==None:
                NyquistZone=hdr["BeamFormer"]["NyquistZone"]
        if not plotspec and (not hasattr(self,"tbeams") or recalc):
            self.tcalc(beams=beams,NyquistZone=NyquistZone,doabs=doabs,smooth=smooth)

        if ylim==None and not plotspec:
            ylim=(self.tbeams.min().val(),self.tbeams.max().val())
        if mosaic:
            npanels=beams.shape()[-2]
            width=int(ceil(sqrt(npanels)))
            height=int(ceil(npanels/float(width)))
            cr.plt.clf()
            for n in range(npanels):
                cr.plt.subplot(width,height,n+1)
                if plotspec:
                    beams.par.avspec[n].plot(clf=False,title=str(n)+".",xlim=xlim,ylim=ylim)
                else:
                    self.tbeams[block,n].plot(clf=False,title=str(n)+".",xlim=xlim,ylim=ylim)
        else:
            if plotspec:
                beams.par.avspec[...].plot(clf=True,xlim=xlim,ylim=ylim)
            else:
                self.tbeams[block,...].plot(clf=True,xlim=xlim,ylim=ylim)


    def tcalc(self,beams=None,block=0,NyquistZone=1,doabs=False,smooth=0):
        """
        Calculate the time series after beamforming, i.e. take the
        result of the BeamForm task, i.e. an array of dimensions
        [self.nblocks,self.nbeams,self.speclen], do a NyquistSwap, if
        necessary, and then an inverse FFT.

        *beams* = None - Input array. Take self.beams from task, if None.
        *NyquistZone* = 1 - NyquistZone=2,4,... means flip the data left to right before FFT.
        *doabs* = False - Take the absolute of the timeseries, if True.
        *smooth* = 0 - If > 0 smooth datat with a Gaussian kernel of that size.

        The final time series (all blocks) is stored in Task.tbeams and returned.
        """
        if beams==None:
            beams=self.beams
        hdr=beams.getHeader()
        if hdr.has_key("BeamFormer"):
            if NyquistZone==None:
                NyquistZone=hdr["BeamFormer"]["NyquistZone"]
        dim=beams.getDim();blocklen=(dim[-1]-1)*2
        self.beamscopy=cr.hArray(dimensions=[dim[-3]*dim[-2],dim[-1]],copy=beams)
        self.beamscopy[...].nyquistswap(self.NyquistZone)
        self.tbeams2=cr.hArray(float,[dim[-3]*dim[-2],blocklen],name="TIMESERIES_DATA")
        self.tbeams=cr.hArray(self.tbeams2.vec(),[dim[-3],dim[-2],blocklen],name="TIMESERIES_DATA")
        self.tbeams2[...].invfftw(self.beamscopy[...])
        self.tbeams /= blocklen
        if doabs:
            self.tbeams.abs()
        if smooth>0:
            self.tbeams[...].runningaverage(smooth,cr.hWEIGHTS.GAUSSIAN)
        if hdr.has_key("SAMPLE_INTERVAL"):
            dt=beams.getHeader("SAMPLE_INTERVAL")[0]
            self.tbeams.par.xvalues=cr.hArray(float,[blocklen],name="Time",units=("","s"))
            self.tbeams.par.xvalues.fillrange(-(blocklen/2)*dt,dt)
            self.tbeams.par.xvalues.setUnit("mu","")
        return self.tbeams


    def dynplot(self,dynspec,plot_cleanspec=None,dmin=None,dmax=None,cmin=None,cmax=None):
        """
        Plot the dynamic spectrum. Provide the dynamic spectrum
        computed by the Task DynamicSpectrum as input.

        ================ ==== ===================================================================
        *plot_cleanspec* None If False, don't plot the cleaned spectrum
                              (provided as dynspec.par.cleanspec).
        *dmin*                Minimum z-value (intensity) in dynamic spectrum to plot
        *dmax*                Maximum z-value (intensity) in dynamic spectrum to plot
        *cmin*                Minimum z-value (intensity) in clean dynamic spectrum to plot
        *cmax*                Maximum z-value (intensity) in clean dynamic spectrum to plot
        ================ ==== ===================================================================

         Example::

           >>> tload "DynamicSpectrum"
           >>> dsp=cr.hArrayRead("Saturn.h5.dynspec.pcr")
           >>> Task.dynplot(dsp,cmin=2.2*10**-5,cmax=0.002,dmin=6.8,dmax=10)

          """
        if hasattr(dynspec,"par") and hasattr(dynspec.par,"cleanspec") and not plot_cleanspec==False:
            cleanspec=dynspec.par.cleanspec
            npcleanspec=np.zeros(cleanspec.getDim())
        else:
            cleanspec=None
        hdr=dynspec.getHeader("DynamicSpectrum")
        npdynspec=np.zeros(dynspec.getDim())
        cr.hCopy(npdynspec,dynspec)
        np.log(npdynspec,npdynspec)
        if cleanspec:
            cr.plt.subplot(1,2,1)
        cr.plt.imshow(npdynspec,aspect='auto',cmap=cr.plt.cm.hot,origin='lower',vmin=dmin,vmax=dmax,
                   extent=(hdr["start_frequency"]/10**6,hdr["end_frequency"]/10**6,hdr["start_time"]*1000,hdr["end_time"]*1000));
        print "dynspec: min=",math.log(dynspec.min().val()),"max=",math.log(dynspec.max().val()),"rms=",math.log(dynspec.stddev().val())
        cr.plt.xlabel("Frequency [MHz]")
        cr.plt.ylabel("+/- Time [ms]")
        if cleanspec:
            cr.hCopy(npcleanspec,cleanspec)
            cr.plt.subplot(1,2,2)
            cr.plt.imshow(npcleanspec,aspect='auto',cmap=cr.plt.cm.hot,origin='lower',vmin=cmin,vmax=cmax,
                    extent=(hdr["start_frequency"]/10**6,hdr["end_frequency"]/10**6,hdr["start_time"]*1000,hdr["end_time"]*1000));
            print "cleanspec: min=",cleanspec.min().val(),"max=",cleanspec.max().val(),"rms=",cleanspec.stddev().val()
            cr.plt.xlabel("Frequency [MHz]")
            cr.plt.ylabel("+/- Time [ms]")


    def qplot(self,entry=0,flaggedblock=0,block=-1,all=True):
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
        quality_entry=self.quality[entry]
        filename=quality_entry["filename"]
        datafile=crfile(filename)
        iantenna=datafile["Antennas"].find(quality_entry["antenna"])
        datafile["selectedAntennas"]=[iantenna]
        if block<0 and flaggedblock<len(quality_entry["flaggedblocks"]):
            block=quality_entry["flaggedblocks"][flaggedblock]
            s="flaggedblock # "+str(flaggedblock)+"/"+str(len(quality_entry["flaggedblocks"])-1)
        else:
            s=""
        print "Filename:",filename,"block =",block,"BLOCKSIZE =",quality_entry["BLOCKSIZE"],s
        if all:
            datafile["BLOCKSIZE"]=quality_entry["size"]
            datafile["BLOCK"]=quality_entry["offset"]*quality_entry["BLOCKSIZE"]/quality_entry["size"]
            y0=datafile["TIMESERIES_DATA"]
            y0.par.xvalues=datafile["Time"]
            y0.par.xvalues.setUnit("mu","")
            y0.plot()
        datafile["BLOCKSIZE"]=quality_entry["BLOCKSIZE"]
        datafile["BLOCK"]=block
        y=datafile["TIMESERIES_DATA"]
        y.par.xvalues=datafile["Time"]
        y.par.xvalues.setUnit("mu","")
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
           filesize = sc.p_(lambda self:self.datafile["DATA_LENGTH"][0],
                            "Length of file for one antenna.",
                            "Samples"),

           self.beams.setHeader("FREQUENCY_INTERVAL"=self.delta_frequency)
           self.datafile["SELECTED_DIPOLES"]=[antennaID]

   getfile:

   ************************************************************************
   Example:
   file=crfile(LOFARSOFT+"/data/lopes/example.event")
   tpar antenna_positions=dict(zip(file["antennaIDs"],file.getCalData("Position")))
   tpar pointings=[dict(az=178.9*deg,el=28*deg),dict(az=0*deg,el=90*deg,r=1)]
   tpar cal_delays=dict(zip(file["antennaIDs"],file.getCalData("Delay")))
   tpar phase_center=[-84.5346,36.1096,0]
   tpar FarField=True
   tpar NyquistZone=2
   tpar randomize_peaks=False

   #file=crfile(LOFARSOFT+"/data/lopes/2004.01.12.00:28:11.577.event")
   #file["SelectedAntennasID"]=[0]
   #fx0=file["TIMESERIES_DATA"]


   ------------------------------------------------------------------------
   tload "BeamFormer"
   file=crfile(LOFARSOFT+"/data/lopes/2004.01.12.00:28:11.577.event")
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

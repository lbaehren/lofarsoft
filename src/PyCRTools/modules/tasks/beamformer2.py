"""
Calculate complex beams towards multiple directions. Also calculates
the average spectrum in each beam and for an incoherent beam.

.. moduleauthor:: Name of the module author <email address of the module author>

**Example**::

  self=Task
  self.beams[...,0].nyquistswap(self.NyquistZone)
  fxb=cr.hArray(float,[2,self.blocklen],name="TIMESERIES_DATA"); fxb[...].saveinvfftw(self.beams[...,0],1);  fxb.abs()
  fxb[...].plot(clf=True); cr.plt.show()

"""

import pycrtools as cr
from pycrtools import tasks
from scipy.optimize import fmin
import time
import pytmf
import math
import os

cr.tasks.__raiseTaskDeprecationWarning(__name__)

# Defining the workspace parameters

deg = math.pi / 180.
pi2 = math.pi / 2.


def getfile(ws):
    """
    To produce an error message in case the file does not exist
    """
    if not ws.filenames:
        return None
    if ws.file_start_number < len(ws.filenames):
        f = cr.open(ws.filenames[ws.file_start_number])
        return f
    else:
        print "ERROR: File number " + ws.file_start_number + " too large!"
        return None

"""
*default*  contains a default value or a function that will be assigned
when the parameter is accessed the first time and no value has been
explicitly set. The function has the form "lambda ws: functionbody", where ws is the worspace itself, so that one can access other
parameters. E.g.: default:lambda ws: ws.par1+1 so that the default
value is one larger than he value in par1 in the workspace.

*doc* a documentation string describing the parameter

*unit* the unit of the value (for informational purposes only)

*export* (True) if False do not export the parameter with
 ws.parameters() or print the parameter

*workarray* (False) If True then this is a workarray which contains
 large amount of memory and is listed separately and not written to
 file.
"""


#    files = [f for f in files if test.search(f)]

class BeamFormer2(tasks.Task):
    """

    The function will calculate multiple beams for a list of files and
    a series of antennas (all integrated into one compex spectrum per
    beam). Also incoherent beams can be calculated and the sifted time
    series of each antenna can be retrieved.

    This beamformer will read all selected antennas at once into
    memory and hence is optimized for short datasets with many antennas
    (e.g. cosmic ray event ).

    It can also be called without a datafile and just hArrays as
    input. In this case provide the following information:

    ``Task(data=timeseries_data, antennas=file["SELECTED_DIPOLES"], antpos=file["ANTENNA_POSITIONS"], sample_interval=file["SAMPLE_INTERVAL"][0], pointings=[dict(az=143.4092*deg,el= 81.7932*deg, r=600.3)])``

    If the FFT of the time series data is already available, it can be
    provided with the parameter ``fft_data`` (in addition to the time
    series that - at this moment - is still required). You then have
    to also set ``dofft=False`` to avoid the FFT being recalculated.

    The desired frequency resolution is provided with the parameter
    delta_nu, but by default this will be rounded off to the nearest
    value using channel numbers of a power of 2. This will then set
    the block size for reading in the data.

    **Results**
    The resulting beam is stored in the array Task.beam and written to
    disk as an hArray with parameters stored in the header dict (use
    ``getHeader('BeamFormer2')`` to retrieve this.)

    The incoherent and beamed average spectra are stored in
    Task.avspec_incoherent and Task.avspec respectively. They are also
    available as attributes to Task.bf.par (also when stored to disk).

    The incoherent beam is found in ``Task.tbeam_incoherent``.

    The beam can be FFTed back to time using ``Task.tcalc`` and viewed
    with ``Task.tplot``.

    """
    parameters = dict(
# Beamformer parameters:
        #------------------------------------------------------------------------
        pointings=dict(default=[dict(az=178.9 * deg, el=28 * deg), dict(az=0 * deg, el=90 * deg, r=1)],
                       doc="List of coordinate dicts (``{'az':az1,'el':elevation value}``) containing pointing directions for each beam on the sky."),

        phase_center=dict(default=[0, 0, 0],
                          doc="List or vector containing the X,Y,Z positions of the phase center of the array.",
                          unit="m"),

        FarField=dict(default=True,
                      doc="Form a beam towards the far field, i.e. no distance."),

        NyquistZone=dict(default=1,
                         doc="In which Nyquist zone was the data taken (e.g. NyquistZone=2 if data is from 100-200 MHz for 200 MHz sampling rate)."),

        #------------------------------------------------------------------------
        # Some standard parameters
        #------------------------------------------------------------------------

        filefilter=dict(default="$LOFARSOFT/data/lofar/RS307C-readfullsecondtbb1.h5",
                        doc="Unix style filter (i.e., with ``*,~, $VARIABLE``, etc.), to describe all the files to be processed."),

        file_start_number=dict(default=0,
                               doc="Integer number pointing to the first file in the 'filenames' list with which to start. Can be changed to restart a calculation."),

        start_block=dict(default=0,
                         doc="Block number to start with. Will be used to to read the first block from file (if provided) and to calculate time."),

        datafile=dict(default=getfile,
                      export=False,
                      doc="Data file object pointing to raw data."),

        calc_timeseries=dict(default=False,
                             doc="If True also do the inverse FFT of all antennas and return the shifted time series for each antenna in data_shifted."),

        calc_tbeams=dict(default=True,
                         doc="Calculate the inverse FFT of all beams and return the shifted and beamformed time series for each beam in Task.tbeams"),

        doabs=dict(default=True,
                   doc="Take the absolute of the tbeam."),

        dosquare=dict(default=False,
                      doc="Take the square (power) of the tbeam."),

        dofft=dict(default=True,
                   doc="If False do not take the fft of the timeseries data. In this case it is assumed that the user has provided the array ``fft_data``, which already contains the fft",),

        smooth_width=dict(default=0,
                          doc="Do a Gaussian smoothing of the beamformed time-series data in Task.tbeam with this width.",
                          unit="Samples"),

        newfigure=dict(default=True,
                       doc="Create a new figure for plotting for each new instance of the task."),

        figure=dict(default=None,
                    doc="The matplotlib figure containing the plot",
                    output=True),

        doplot=dict(default=False,
                    doc="Plot current spectrum while processing."),

        plotspec=dict(default=True,
                      doc="If True plot the beamformed average spectrum at the end, otherwise the time series."),

        plotlen=dict(default=2 ** 12,
                     doc="How many channels +/- the center value to plot during the calculation (to allow progress checking)."),

        plotskip=dict(default=1,
                      doc="Plot only every 'plotskip'-th spectrum, skip the rest (should not be smaller than 1)."),

        plot_antennas=dict(default=lambda self: range(self.nantennas),
                           doc="A list of antenna indices (from 0 to nantennas, i.e. relative to the selected antennas and not necessarily the same indices as in the file!) for which to plot the time series (if doplot>1)."),

        plot_center=dict(default=0.5,
                         doc="Center plot at this relative distance from start of vector (0=left end, 1=right end)."),

        plot_start=dict(default=lambda self: max(int(self.blocklen * self.plot_center) - self.plotlen, 0),
                        doc="Start plotting from this sample number."),

        plot_end=dict(default=lambda self: min(int(self.blocklen * self.plot_center) + self.plotlen, self.blocklen),
                      doc="End plotting before this sample number."),

        plot_finish=dict(default=lambda self: cr.plotfinish(doplot=self.doplot),
                         doc="Function to be called after each plot to determine whether to pause or not (see :func:`plotfinish`)"),

        delta_nu=dict(default=1,
                      doc="Desired frequency resolution - will be rounded off to get powers of 2 blocklen. Alternatively set blocklen directly.",
                      unit="Hz"),

        maxnantennas=dict(default=96,
                          doc="Maximum number of antennas per file to sum over (also used to allocate some vector sizes)."),

        maxblocksflagged=dict(default=2,
                              doc="Maximum number of blocks that are allowed to be flagged before the entire spectrum of the chunk is discarded."),

        stride=dict(default=1,
                    doc="If stride>1 skip (stride-1) blocks."),

        tmpfileext=dict(default=".pcr",
                        doc="Extension of filename for temporary data files (e.g., used if stride>1.)",
                        export=False),

        tmpfilename=dict(default="tmp",
                         doc="Root filename for temporary data files.",
                         export=False),

        filenames=dict(default=lambda self: cr.listFiles(self.filefilter),
                       doc="List of filenames of data file to read raw data from."),

        output_dir=dict(default="",
                        doc="Directory where output file is to be written to."),

        output_filename=dict(default=lambda self: (os.path.split(self.filenames[0])[1] if len(self.filenames) > 0 else "beamformer2") + ".beams" + self.tmpfileext,
                             doc="Filename (without directory, see output_dir) to store the final spectrum."),

        spectrum_file=dict(default=lambda self: os.path.join(os.path.expandvars(os.path.expanduser(self.output_dir)), self.output_filename),
                           doc="Complete filename including directory to store the final spectrum."),

        store_spectrum=dict(default=True,
                            doc="Store the final spectrum."),

        nantennas=dict(default=lambda self: len(self.antennas),
                       doc="The actual number of antennas available for calculation in the file (<maxnantennas)."),

        nantennas_start=dict(default=0,
                             doc="Start with the nth antenna in each file (see also natennas_stride). Can be used for selecting odd/even antennas."),

        antenna_list=dict(default={},
                          doc="List of antenna indices used as input from each filename.",
                          output=True),

        nantennas_stride=dict(default=1,
                              doc="Take only every nth antenna from antennas list (see also natennas_start). Use 2 to select odd/even."),

        nspectraadded=dict(default=lambda self: cr.hArray(int, [self.nblocks], fill=0, name="Spectra added"),
                           doc="Number of spectra added per block.",
                           output=True),

        nspectraflagged=dict(default=lambda self: cr.hArray(int, [self.nblocks], fill=0, name="Spectra flagged"),
                             doc="Number of spectra flagged per block.",
                             output=True),

        nofAntennas=dict(default=lambda self: self.datafile["NOF_DIPOLE_DATASETS"] if self.datafile else self.data.shape()[-2],
                         output=True),

        antennas=dict(default=lambda self: range(max(self.nantennas_start, 0), min(self.nofAntennas, self.maxnantennas), self.nantennas_stride),
                      doc="Antennas from which to select initially for the current file."),

        antennas_used=dict(default=lambda self: set(),
                           doc="A set of antenna names that were actually included in the average spectrum, excluding the flagged ones.",
                           output=True),

        header=dict(default=lambda self: self.datafile.getHeader() if self.datafile else {},
                    doc="Header of datafile",
                    export=False),

        verbose=dict(default=True,
                     doc="Print progress information."),

        #----------------------------------------------------------------------t--
        # Derived parameters

        nbeams=dict(default=lambda self: len(self.pointings),
                    doc="Number of beams to calculate."),

        mainbeam=dict(default=lambda self: self.nbeams / 2,
                      doc="The main beam which is used to plot things. Default= nbeams/2."),

        phase_center_array=dict(default=lambda self: cr.hArray(list(self.phase_center), name="Phase Center", units=("", "m")),
                                doc="List or vector containing the X,Y,Z positions of the phase center of the array.",
                                unit="m"),

        block_duration = dict(default=lambda self: self.sample_interval * self.blocklen,
                              doc="The length of a block in time units.",
                              unit="s"),

        speclen = dict(default=lambda self: self.blocklen / 2 + 1,
                       doc="Length of one spectrum.",
                       unit="Channels"),

        sample_interval = dict(default=lambda self: self.datafile["SAMPLE_INTERVAL"][0] if datafile else 1,
                               doc="Length in time of one sample in raw data set.",
                               unit="s"),

        filesize = dict(default=lambda self: self.datafile["DATA_LENGTH"][0] if self.datafile else self.data.shape()[-1],
                        doc="Length of file for one antenna.",
                        unit="Samples"),

        delta_nu_used = dict(default=lambda self: 1 / (self.sample_interval * self.blocklen),
                             doc="Actual frequency resolution of dynamic spectrum",
                             unit="Hz"),

        max_nblocks = dict(default=lambda self: int(math.floor(self.filesize / self.stride / self.blocklen)),
                           doc="Maximum number of blocks in file."),

        nblocks = dict(default=lambda self: int(min(max(round((self.filesize - self.start_block * self.blocklen * self.stride) / self.stride / self.blocklen), 1), self.max_nblocks)),
                       doc="Number of blocks to read in."),

        blockduration = dict(default=lambda self: self.blocklen * self.sample_interval,
                             doc="Length in time units of one block of data, i.e. on time step in dynamic spectrum.",
                             unit="s"),

        end_time = dict(default=lambda self: self.blockduration * self.nblocks * self.stride,
                        doc="End of time axis.",
                        unit="s"),

        start_time = dict(default=lambda self: self.start_block * self.blockduration,
                          doc="Start of time axis.",
                          unit="s"),

        start_frequency = dict(default=lambda self: self.datafile["FREQUENCY_RANGE"][0][0] if self.datafile else ((self.NyquistZone - 1) / self.sample_interval / 2.0),
                               doc="Start frequency of spectrum",
                               unit="Hz"),

        end_frequency = dict(default=lambda self: self.datafile["FREQUENCY_RANGE"][0][1] if self.datafile else (self.NyquistZone / self.sample_interval / 2.0),
                             doc="End frequency of spectrum",
                             unit="Hz"),

        delta_frequency = dict(default=lambda self: (self.end_frequency - self.start_frequency) / (self.speclen - 1.0),
                               doc="Separation of two subsequent channels in final spectrum"),

        blocklen = dict(default=lambda self: min(2 ** int(round(math.log(1. / self.delta_nu / self.sample_interval, 2))), 2 ** int(round(math.log(self.filesize / self.stride, 2)))) if self.datafile else self.data.shape()[-1],
                        doc="The size of a block used for the FFT, limited by filesize.",
                        unit="Sample"),

        cable_delays = dict(default=lambda self: cr.hArray(float, [self.nantennas], name="Cable Delays", units=("", "s")),
                            doc="An ``hArray`` containing 'cable' delays for each selected antenna in seconds as values. These delays will be added to the geometrical delays.",
                            unit="s"),

        #------------------------------------------------------------------------

        # Now define all the work arrays used internally
        data=dict(workarray=True,
                  doc="Main input array of raw data - can be set directly instead of providing a data file.",
                  default=lambda self: cr.hArray(float, [self.nantennas if self.datafile else 1, self.blocklen if self.datafile else 2 ** 8], name="E-Field", xvalues=self.times, header=self.header)),

        data_shifted=dict(workarray=True,
                          doc="Array for plotting the shifted e-field of main beam",
                          default=lambda self: cr.hArray(float, [self.nantennas, self.blocklen], name="shifted E-field", xvalues=self.times, header=self.header)),

        fftdata=dict(workarray=True,
                     doc="main input array of raw data",
                     default=lambda self: cr.hArray(complex, [self.nantennas, self.speclen], name="fftdata", header=self.header)),

        avspec=dict(workarray=True,
                    doc="Average spectrum in each beam over multiple blocks.",
                    default=lambda self: cr.hArray(float, [self.nbeams, self.speclen], name="Average Spectrum", header=self.header, par=dict(logplot="y"), xvalues=self.frequencies)),

        avspec_incoherent=dict(workarray=True,
                               doc="The average spectrum of all blocks in an incoherent beam (i.e. squaring before adding).",
                               default=lambda self: cr.hArray(float, [self.speclen], name="Incoherent Average Spectrum", header=self.header, par=dict(logplot="y"), xvalues=self.frequencies)),

        tbeam_incoherent=dict(workarray=True,
                              doc="Contains the power as a function of time of an incoherent beam (in the direction of the pointing) of all antennas (simply the square of the ADC values of the siffted time series data added).",
                              default=lambda self: cr.hArray(float, [self.nbeams, self.nblocks, self.blocklen], name="Incoherent Time Beam", header=self.header)),

        beams=dict(workarray=True,
                   doc="Output array containing the FFTed data for each beam.",
                   default=lambda self: cr.hArray(complex, [self.nbeams, self.speclen], name="beamed FFT", header=self.header, par=dict(logplot="y"), xvalues=self.frequencies)),

        pointingsXYZ=dict(default=lambda self: cr.hArray(float, [self.nbeams, 3], fill=[item for sublist in [cr.convert(coords, "CARTESIAN") for coords in self.pointings] for item in sublist], name="Beam Direction XYZ"),
                          doc="Array of shape [nbeams,3] with x,y,z positions for each beam on the sky."),

        phases=dict(workarray=True,
                    doc="Complex phases for each beam and each freqeuncy channel used to calculate complex weights for beamforming.",
                    default=lambda self: cr.hArray(float, [self.nbeams, self.nantennas, self.speclen], name="Phases", header=self.header)),

        weights=dict(workarray=True,
                     doc="Complex weights for each beam and each freqeuncy channel used to calculate beams.",
                     default=lambda self: cr.hArray(complex, [self.nbeams, self.nantennas, self.speclen], name="Weights", header=self.header)),

        delays=dict(workarray=True,
                    doc="Contains the geometric delays for the current antenna for each beam",
                    default=lambda self: cr.hArray(float, [self.nantennas, self.nbeams], name="Delays", header=self.header)),

        antpos=dict(workarray=True,
                    doc="Cartesian coordinates of the current antenna relative to the phase center of the array.",
                    default=lambda self: cr.hArray(float, [self.nantennas, 3], name="Delays", header=self.header, units=("", "m")),
                    unit="m"),

        frequencies=dict(workarray=True,
                         doc="Frequency axis for final power spectrum.",
                         default=lambda self: cr.hArray(float, [self.speclen], name="Frequency", units=("", "Hz"), header=self.header)),

        times=dict(workarray=True,
                   doc="Time axis for data.",
                   default=lambda self: cr.hArray(float, [self.blocklen], name="Time", units=("", "s"), header=self.header)),

        blocksize=dict(default=lambda self: self.data.shape()[-1],
                       doc="Length of the data for each antenna"),

        fftplan=dict(default=lambda self: cr.FFTWPlanManyDftR2c(self.blocksize, 1, 1, 1, 1, 1, cr.fftw_flags.ESTIMATE),
                     doc="Memory and plan for FFT",
                     output=False,
                     workarray=True),

        invfftplan=dict(default=lambda self: cr.FFTWPlanManyDftC2r(self.blocksize, 1, 1, 1, 1, 1, cr.fftw_flags.ESTIMATE),
                        doc="Memory and plan for inverse FFT",
                        output=False,
                        workarray=True)
        )

    def run(self):
        """Run the program.
        """
        self.spectrum_file_bin = os.path.join(self.spectrum_file, "data.bin")
        self.dostride = (self.stride > 1)
        self.nspectraadded.fill(0)
        self.beams.par.avspec = self.avspec
        self.beams.par.avspec_incoherent = self.avspec_incoherent
        self.beams.par.tbeam_incoherent = self.tbeam_incoherent

        self.updateHeader(self.beams, ["nspectraadded", "filenames", "antennas_used"], delta_nu="delta_nu_used", FFTSIZE="speclen", BLOCKSIZE="blocklen", filename="spectrum_file")
        self.frequencies.fillrange((self.start_frequency), self.delta_frequency)

        clearfile = True

        # Note: when re-running task, need to redo pointingsXYZ as it is calculated only once from self.pointings.
        self.pointingsXYZ = cr.hArray(float, [self.nbeams, 3], fill=[item for sublist in [cr.convert(coords, "CARTESIAN") for coords in self.pointings] for item in sublist], name="Beam Direction XYZ")

        self.t0 = time.clock()  # ; print "Reading in data and doing a double FFT."

        # fftplan = cr.FFTWPlanManyDftR2c(self.data.shape()[-1], 1, 1, 1, 1, 1, cr.fftw_flags.ESTIMATE)

        if self.doplot:
            wasinteractive = cr.plt.isinteractive()
            cr.plt.ioff()
            if self.newfigure and not self.figure:
                self.figure = cr.plt.figure()

        original_file_start_number = self.file_start_number
        if self.avspec:
            self.avspec.fill(0)
        if self.avspec_incoherent:
            self.avspec_incoherent.fill(0)
        if self.tbeam_incoherent:
            self.tbeam_incoherent.fill(0)
        self.file_count = 0
        for fname in self.filenames[self.file_start_number:] if self.datafile else ["nofile"]:
            self.file_count += 1
            if self.verbose:
                print "# Start File", str(self.file_start_number) + ":", fname
            if self.file_count > 1:
                self.ws.update(workarrays=False)  # since the file_start_number was changed, make an update to get the correct file
            if self.datafile:
                self.datafile["BLOCKSIZE"] = self.blocklen  # Setting initial block size
                self.antennaIDs = list(cr.ashArray(cr.hArray(self.datafile["DIPOLE_NAMES"])[self.antennas]))
                self.datafile["SELECTED_DIPOLES"] = self.antennaIDs
                self.antpos = self.datafile["ANTENNA_POSITIONS"]
                self.antpos -= self.phase_center_array
                # print "Relative antenna position =",self.antpos
            for block in range(self.start_block, self.start_block + self.nblocks, self.stride):
                if self.datafile:
                    self.datafile["BLOCK"] = block
                if self.verbose:
                    print "# Start block =", block

                # Calculate the geometrical delays needed for beamforming
                cr.hGeometricDelays(self.delays, self.antpos, self.pointingsXYZ, self.FarField)

                # Add the cable/calibration delay
                self.delays.add(cr.ashArray(self.cable_delays))

                self.phases.delaytophase(self.frequencies, self.delays)
                self.weights.phasetocomplex(self.phases)
                # cr.hGeometricWeights(self.weights,self.frequencies,self.antpos,self.pointings_cartesian,self.FarField)

                if self.datafile:
                    self.data.read(self.datafile, "TIMESERIES_DATA", block)
                self.times.fillrange(self.start_time, self.sample_interval)
                self.times.setUnit("mu", "")
                self.nspectraadded[block] += self.nantennas
                if self.dofft:
                    cr.hFFTWExecutePlan(self.fftdata[...], self.data[...], self.fftplan)
                    self.fftdata[...].nyquistswap(self.NyquistZone)
                if self.avspec_incoherent:
                    self.avspec_incoherent.squareadd(self.fftdata[...])
                if self.nspectraadded[block] > self.nantennas:
                    self.fftdata /= float(self.nspectraadded[block])
                if self.file_count > 1:
                    if self.spectrum_file:
                        self.beams.readfilebinary(self.spectrum_file_bin, block * self.speclen * self.nbeams * self.nantennas)
                else:
                    self.beams.fill(0)
                self.beams *= (self.nspectraadded[block] - self.nantennas) / (self.nspectraadded[block])
                self.beams[...].muladdsum(self.weights[...], self.fftdata)  # [nbeam,speclen] - [nbeam,nant,speclen] - [nant,speclen]
                if self.avspec:
                    self.avspec.squareadd(self.beams)
                if self.spectrum_file and self.store_spectrum:
                    self.beams.write(self.spectrum_file, nblocks=self.nblocks, block=block, clearfile=clearfile)
                    clearfile = False
                if self.calc_timeseries:
                    self.fftdata.mul(self.weights[self.mainbeam])
                    self.fftdata[...].nyquistswap(self.NyquistZone)
                    cr.hFFTWExecutePlan(self.data_shifted[...], self.fftdata[...], self.invfftplan)
                    self.data_shifted /= self.blocklen
                    if  self.tbeam_incoherent:
                        self.tbeam_incoherent[block].squareadd(self.data_shifted[...])
                if self.doplot > 1 and self.nspectraadded[block] % self.plotskip == 0:
                    if (self.plotspec or not self.calc_timeseries) and self.avspec:
                        self.avspec[self.mainbeam].plot()
                    elif self.calc_timeseries:
                        self.data_shifted[self.plot_antennas, ..., self.plot_start:self.plot_end].plot(title="Shifted Time Series in Direction of Main Beam")
                    self.plot_finish(name=self.__taskname__)
                if self.verbose:
                    print "# End  block =", block, " Time =", time.clock() - self.t0, "s  nspectraadded =", self.nspectraadded.sum(), "nspectraflagged =", self.nspectraflagged.sum()
            if self.verbose:
                print "# End File", str(self.file_start_number) + ":", fname
            self.updateHeader(self.beams, ["nspectraadded", "filenames", "antennas_used"])
            self.file_start_number += 1
        if self.avspec:
            self.avspec /= self.nspectraadded.sum().val()
        if self.avspec_incoherent:
            self.avspec_incoherent /= self.nspectraadded.sum().val()
        if self.tbeam_incoherent:
            self.tbeam_incoherent[...] /= self.nspectraadded[...].val()
        if self.calc_tbeams:
            self.tcalc(NyquistZone=self.NyquistZone, doabs=self.doabs, dosquare=self.dosquare, smooth=self.smooth_width)
        self.file_start_number = original_file_start_number  # reset to original value, so that the parameter file is correctly written.
        if self.verbose:
            print "Finished - total time used:", time.clock() - self.t0, "s."
            print "To read back the beam formed data type: bm=cr.hArrayRead('" + self.spectrum_file + "')"
            print "To calculate or plot the invFFTed times series of one block, use 'Task.tcalc(bm)' or 'Task.tplot(bm)'."
        if self.doplot:
            if wasinteractive:
                cr.plt.ion()

    def tplot(self, beams=None, block=0, NyquistZone=1, doabs=True, dosquare=False, smooth=0, mosaic=True, plotspec=False, xlim=None, ylim=None, recalc=False):
        """
        Take the result of the BeamForm task, i.e. an array of
        dimensions [self.nblocks,self.nbeams,self.speclen], do a
        NyquistSwap, if necessary, and then calculate an inverse
        FFT. If the time series (self.tbeams) has already been
        caclulated it will only be recalculated if explicitly asked
        for with recalc=True.

        **Properties**

        ==============  ==========  ==========================================================================
        Parameter       Default         Description
        ==============  ==========  ==========================================================================
        *beams*         ``None``    Input array. Take self.beams from task, if None.
        *recalc*        ``False``   If true force a recalculation of the time series beam if it exists
        *block*         0           Which  block to plot, or Ellipsis ('...') for all
        *NyquistZone*   1           NyquistZone=2,4,... means flip the data left to right before FFT.
        *doabs*         ``True``    Take the absolute of the timeseries before plotting.
        *smooth*        0           If > 0 smooth data with a Gaussian kernel of that size.
        *plotspec*      ``False``   If True plot the average spectrum instead.
        *xlim*          ``None``    Tuple with minimum and maximum limits for the *x*-axis.
        *ylim*          ``None``    Tuple with minimum and maximum limits for the *y*-axis.
        ==============  ==========  ==========================================================================

        The final time series (all blocks) is stored in Task.tbeams.

        """
        if beams == None:
            beams = self.beams
        hdr = beams.getHeader()
        if "BeamFormer" in hdr:
            if NyquistZone == None:
                NyquistZone = hdr["BeamFormer"]["NyquistZone"]
        if not plotspec and (not hasattr(self, "tbeams") or recalc):
            self.tcalc(beams=beams, NyquistZone=NyquistZone, doabs=doabs, dosquare=dosquare, smooth=smooth)
        if ylim == None and not plotspec:
            ylim = (self.tbeams.min().val(), self.tbeams.max().val())
        if mosaic:
            npanels = self.beams.shape()[-2]
            width = int(math.ceil(math.sqrt(npanels)))
            height = int(math.ceil(npanels / float(width)))
            cr.plt.clf()
            cr.plt.subplots_adjust(hspace=0.4, wspace=0.4)
            for n in range(npanels):
                cr.plt.subplot(width, height, n + 1)
                if plotspec:
                    beams.par.avspec[n].plot(clf=False, xlabel="", title=str(n) + ".", xlim=xlim, ylim=ylim)
                else:
                    self.tbeams[n].plot(clf=False, xlabel="", title=str(n) + ".", xlim=xlim, ylim=ylim)
        else:
            if plotspec:
                beams.par.avspec[...].plot(clf=True, xlim=xlim, ylim=ylim, title="Beamformed spectrum around main direction")
            else:
                self.tbeams[...].plot(clf=True, xlim=xlim, ylim=ylim, title="Beamformed time series around main direction")

    def tcalc(self, beams=None, block=0, NyquistZone=1, doabs=False, dosquare=False, smooth=0):
        """
        Calculate the time series after beamforming, i.e. take the
        result of the BeamForm task, i.e. an array of dimensions
        [self.nblocks,self.nbeams,self.speclen], do a NyquistSwap, if
        necessary, and then an inverse FFT.

        ==============  ============  ==========================================================================
        Parameter       Default       Description
        ==============  ============  ==========================================================================
        *beams*         ``None``      Input array. Take self.beams from task, if None.
        *NyquistZone*   1             NyquistZone=2,4,... means flip the data left to right before FFT.
        *doabs*         ``False``     Take the absolute of the timeseries, if True.
        *smooth*        0             If > 0 smooth datat with a Gaussian kernel of that size.
        ==============  ============  ==========================================================================

        The final time series (all blocks) is stored in Task.tbeams and returned.

        """

        if beams == None:
            beams = self.beams
        hdr = beams.getHeader()
        if "BeamFormer2" in hdr:
            if NyquistZone == None:
                NyquistZone = hdr["BeamFormer"]["NyquistZone"]
        dim = beams.shape()
        blocklen = (dim[-1] - 1) * 2
        self.beamscopy = cr.hArray(dimensions=[dim[-2], dim[-1]], copy=beams)
        self.beamscopy[...].nyquistswap(self.NyquistZone)
        if not hasattr(self, "tbeams2size") or not self.tbeams2size == blocklen:
            self.tbeams2size = blocklen
            self.tbeams2plan = cr.FFTWPlanManyDftC2r(blocklen, 1, 1, 1, 1, 1, cr.fftw_flags.ESTIMATE)
        self.tbeams2 = cr.hArray(float, [dim[-2], blocklen], name="TIMESERIES_DATA")
        self.tbeams = cr.hArray(self.tbeams2.vec(), [dim[-2], blocklen], name="TIMESERIES_DATA")
        cr.hFFTWExecutePlan(self.tbeams2[...], self.beamscopy[...], self.tbeams2plan)
        # self.tbeams2[...].invfftw(self.beamscopy[...])
        self.tbeams /= blocklen
        if doabs:
            self.tbeams.abs()
        if dosquare:
            self.tbeams.square()
        if smooth > 0:
            self.tbeams[...].runningaverage(smooth, cr.hWEIGHTS.GAUSSIAN)
        if "SAMPLE_INTERVAL" in hdr:
            dt = beams.getHeader("SAMPLE_INTERVAL")[0]
            self.tbeams.par.xvalues = cr.hArray(float, [blocklen], name="Time", units=("", "s"))
            self.tbeams.par.xvalues.fillrange(-(blocklen / 2) * dt, dt)
            self.tbeams.par.xvalues.setUnit("mu", "")
        return self.tbeams

    def smoothedPulsePowerMaximizer(self, direction, verbose=False):
        """
        Maximizer function to be called with scipy.fmin.
        Parameter 'direction' is a tuple (az, el) in radians.
        (but somehow, the fmin function passes an np.array([az, el]) instead...)

        It calls self.run() again, with a new 'self.pointings' towards given direction.
        """
        az = direction[0]
        el = direction[1]
        position = (az, el, 10000)  # fake distance 10000 as used in the pipeline...

        self.pointings = cr.rf.makeAZELRDictGrid(*(position), nx=1, ny=1)

        self.run()
        # answer, squared and smoothed, is in self.tbeams
        # this means the maximum in power, over a window size 'self.smooth_width', is in
        # the maximum of the tbeams array
        value = - self.tbeams[0].max()[0]  # note the minus sign, as we are maximizing power using a minimizer method
        if verbose:
            print 'Evaluated az = %f, el = %f: power = %f' % (az / deg, el / deg, - value)

        return value

    def optimizeDirection(self, start_direction=None, smooth=7, verbose=False):
        """
        Use scipy's fmin function (downhill-simplex minimizer) to obtain a maximum pulse power.
        """

        # retaining 3 variables in order to create the beam in the same way as an earlier beam was done.
        temp = (self.dosquare, self.doabs, self.smooth_width)

        self.doplot = False
        self.store_spectrum = False
        self.calc_tbeams = True
        self.dosquare = True
        self.doabs = False
        self.smooth_width = smooth

        if not start_direction:
            midpoint = (len(self.pointings) - 1) / 2  # index of middle point of pointings
            start_position = (self.pointings[midpoint]["az"], self.pointings[midpoint]["el"])  # no R yet
        else:
            start_position = start_direction

        optimum = fmin(self.smoothedPulsePowerMaximizer, start_position, (verbose,), xtol=1e-1, ftol=1e-1, full_output=1)
        optimum_direction = optimum[0]
        # create timeseries beam self.tbeams in the same way as the variables specified
        (self.dosquare, self.doabs, self.smooth_width) = temp
        self.smoothedPulsePowerMaximizer(optimum_direction, verbose=False)  # calculates the beam

        return optimum_direction  # is a tuple just like the parameters being optimized over, i.e. (az, el)

    def dynplot(self, dynspec, plot_cleanspec=None, dmin=None, dmax=None, cmin=None, cmax=None):
        """
        Plot the dynamic spectrum. Provide the dynamic spectrum
        computed by the Task DynamicSpectrum as input.

        =================  ========  ============================================================
        Parameter          Default   Description
        =================  ========  ============================================================
        *plot_cleanspec*   ``None``  If False, don't plot the cleaned spectrum (provided as dynspec.par.cleanspec).
        *dmin*             ``None``  Minimum z-value (intensity) in dynamic spectrum to plot
        *dmax*             ``None``  Maximum z-value (intensity) in dynamic spectrum to plot
        *cmin*             ``None``  Minimum z-value (intensity) in clean dynamic spectrum to plot
        *cmax*             ``None``  Maximum z-value (intensity) in clean dynamic spectrum to plot
        =================  ========  ============================================================

        Example::

          tload "DynamicSpectrum"
          dsp=cr.hArrayRead("Saturn.h5.dynspec.pcr")
          Task.dynplot(dsp,cmin=2.2*10**-5,cmax=0.002,dmin=6.8,dmax=10)

        """
        if hasattr(dynspec, "par") and hasattr(dynspec.par, "cleanspec") and not plot_cleanspec == False:
            cleanspec = dynspec.par.cleanspec
            npcleanspec = np.zeros(cleanspec.shape())
        else:
            cleanspec = None
        hdr = dynspec.getHeader("DynamicSpectrum")
        npdynspec = np.zeros(dynspec.shape())
        cr.hCopy(npdynspec, dynspec)
        np.log(npdynspec, npdynspec)
        if cleanspec:
            cr.plt.subplot(1, 2, 1)
        cr.plt.imshow(npdynspec, aspect='auto', cmap=cr.plt.cm.hot, origin='lower', vmin=dmin, vmax=dmax,
                   extent=(hdr["start_frequency"] / 10 ** 6, hdr["end_frequency"] / 10 ** 6, hdr["start_time"] * 1000, hdr["end_time"] * 1000))
        print "dynspec: min=", math.log(dynspec.min().val()), "max=", math.log(dynspec.max().val()), "rms=", math.log(dynspec.stddev().val())
        cr.plt.xlabel("Frequency [MHz]")
        cr.plt.ylabel("+/- Time [ms]")
        if cleanspec:
            cr.hCopy(npcleanspec, cleanspec)
            cr.plt.subplot(1, 2, 2)
            cr.plt.imshow(npcleanspec, aspect='auto', cmap=cr.plt.cm.hot, origin='lower', vmin=cmin, vmax=cmax,
                    extent=(hdr["start_frequency"] / 10 ** 6, hdr["end_frequency"] / 10 ** 6, hdr["start_time"] * 1000, hdr["end_time"] * 1000))
            print "cleanspec: min=", cleanspec.min().val(), "max=", cleanspec.max().val(), "rms=", cleanspec.stddev().val()
            cr.plt.xlabel("Frequency [MHz]")
            cr.plt.ylabel("+/- Time [ms]")
        cr.plt.draw()
        cr.plt.show()

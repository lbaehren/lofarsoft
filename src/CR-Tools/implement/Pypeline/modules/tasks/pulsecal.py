"""
A number of tools useful in calibrating radio data

"""

import pycrtools as cr
import pycrtools.tasks as tasks
import pytmf
from pycrtools.tasks.shortcuts import *
from math import *

deg=pi/180.
pi2=pi/2.


class LocatePulseTrain(tasks.Task):
    """
    **Usage**

    ``LocatePulseTrain()(timeseries_data[blocklen],nsigma=7,maxgap=7,minpulselen=7) -> Task.start, Task.end, Task.time_series_cut``

    **Description**

    Finds the pulse train with the highest peak in time series data,
    determine its location, and cut out the time series data around
    the pulse. If the input array has multiple dimensions (multiple
    antennnas are present), then sum the antennas first and search the
    pulse in the sum of all antennas.

    The task calculates noise and threshold level above which to find
    pulse trains. A pulse train can have gaps of some `N=maxgaps`
    samples which are below the threshold and yet are considered part
    of the pulse train (i.e., this is the maximum separation of
    individual pulses to be considered part of the pulse train.).

    Returns start and end index of the strongest pulse in
    ``Task.start`` and ``Task.end``. The cut-out time series is
    returned in ``Task.timeseries_data_cut``.

    See :func:`hFindSequenceGreaterThan` for a description of the other
    parameters.

    **See also:**

    :func:`hFindSequenceGreaterThan`

    **Example**

    ::

        file=open("/Users/falcke/LOFAR/usg/data/lofar/oneshot_level4_CS017_19okt_no-9.h5") #
        file["BLOCKSIZE"]=2**int(round(log(file["DATA_LENGTH"][0],2)))
        file["SELECTED_DIPOLES"]=["017000001", "017000002", "017000005", "017000007", "017001009", "017001010", "017001012", "017001015", "017002017", "017002019", "017002020", "017002023", "017003025", "017003026", "017003029", "017003031", "017004033", "017004035", "017004037", "017004039", "017005041", "017005043", "017005045", "017005047", "017006049", "017006051", "017006053", "017006055", "017007057", "017007059", "017007061", "017007063", "017008065", "017008066", "017008069", "017008071", "017009073", "017009075", "017009077", "017009079", "017010081", "017010083", "017010085", "017010087", "017011089", "017011091", "017011093", "017011095"]
        pulse=trun('LocatePulseTrain',file["TIMESERIES_DATA"])
        #(pulse.start,pulse.end) -> (65806L, 65934L)
        pulse.timeseries_data_sum.plot(highlight=(pulse.start,pulse.end),nhighlight=1)
        pulse.timeseries_data_cut[0].plot()
    """
    parameters=dict(
        nblocks=p_(16,"The time series data is subdivided in ``nblock`` blocks where the one with the lowest RMS is used to determine the threshold level for finding peaks."),
        nsigma=p_(7,"Pulse has to be ``nsigma`` above the noise."),
        maxgap=p_(7,"Maximum gap length allowed in a pulse where data is not above the threshold"),
        minpulselen=p_(7,"Minimum width of pulse."),
        maxlen=p_(1024,"Maximum length allowed for the pulse train to be returned."),
        minlen=p_(32,"Minimum length allowed for cutting the time time series data."),
        prepulselen=p_(16,"Safety margin to add before the start of the pulse for cutting and ``start``."),
        docut=p_(True,"Cut the time series around the pulse and return it in ``timeseries_data_cut``."),
        cut_to_power_of_two=p_(True,"Cut the time series to a length which is a power of two?"),
        maxnpeaks=p_(256,"Naximum number of train peaks to look for in data"),
        start=p_(0,"Start index of the strongest pulse train",output=True),
        end=p_(32,"End index of the strongest pulse train",output=True),
        npeaks=p_(0,"Number of peaks found in data",output=True),
        maxima=p_(None,"Values of the maxima of the pulse trains",output=True),
        indexlist=p_(lambda self:cr.hArray(int,[self.maxnpeaks,2]),"Original locations of the peaks found in the data",output=True),
        cutlen=p_(None,"Length of the cut-out data.",output=True),
        timeseries_data_sum=p_(None,"Incoherent (squared) sum of all antennas."),
        timeseries_data_cut=p_(lambda self:cr.hArray(float,self.timeseries_data.shape()[:-1]+[self.cutlen]),"Contains the time series data cut out around the pulse.",output=True)
        )

    def call(self,timeseries_data):
        pass

    def run(self):
        #Sum all antennas, if more than one antenna is present
        if len(self.timeseries_data.shape())==1:
            self.timeseries_data_sum=self.timeseries_data
        else:
            self.timeseries_data_sum=cr.rf.TimeBeamIncoherent(self.timeseries_data)

        self.minrms=cr.Vector(float,1)
        self.minmean=cr.Vector(float,1)
        self.blen=cr.Vector(int,1,fill=max(self.timeseries_data_sum.getSize()/self.nblocks,8))
        self.timeseries_data_sum.minstddevblock(self.blen,self.minrms,self.minmean)
        self.npeaks=self.indexlist.findsequencegreaterthan(self.timeseries_data_sum,self.minmean[0]+self.nsigma*self.minrms[0],self.maxgap,self.minpulselen).first()
        if self.npeaks <= 0:
            return
        self.maxima=cr.hArray(float,[self.npeaks])
        self.maxima.maxinsequences(self.timeseries_data_sum,self.indexlist,self.npeaks)
        self.maxseq=self.maxima.maxpos().first()

        self.start=self.indexlist[self.maxseq,0]
        self.end=self.indexlist[self.maxseq,1]
        self.cutlen=int(2**ceil(log(min(max(self.end-self.start+self.prepulselen,self.minlen),self.maxlen),2))) if self.cut_to_power_of_two else min(max(self.end-self.start+self.prepulselen,self.minlen),self.maxlen)

        self.start-=self.prepulselen; self.end=self.start+self.cutlen

        #Cut the data around the main pulse
        if self.docut:
            if len(self.timeseries_data.shape())>1:
                self.timeseries_data_cut[...].copy(self.timeseries_data[...,self.start:self.end])
            else:
                self.timeseries_data_cut.copy(self.timeseries_data[self.start:self.end])


class CrossCorrelateAntennas(tasks.Task):
    """
    **Usage:**

    ``Task = trun("CrossCorrelateAntennas",timeseries_data[n_dataset,size], refant=0, reference_data=None, reference_fft_data=None, fft_data=None, fft_reference_data=None, crosscorr_data=None) -> Task.crosscorr_data (i.e., strength of cross-correlation as a function of time)``

    To provide only fft data use:

    ``crosscorr_data = tload("CrossCorrelateAntennas",False)(None,fft_data=bf.fftdata).crosscorr_data``

    **Description:**

    Calculates the cross-correlation of a number of time series data
    sets with respective to a reference data set.

    The data set is a matrix of N equal-length timeseries. If not
    reference data set is provided, a reference antenna from the input
    time series matrix is specified (by default = 0, i.e. all data is
    correlated with the first data set, then the first cross
    correlation output will actually be an autocorrelation).

    Make sure N is even. This is not tested for uneven N.

    NB: This has also not yet been tested for NyquistZone 2!

    If ``timeseries_data == None``, then one needs to provide
    ``fft_data`` instead and the fft will not be (re-)calculated. Note
    that the function uses fftw convention!

    **See also:**

    :class:`CrossCorrelateAntennas`,
    :class:`LocatePulseTrain`,
    :class:`FitMaxima`

    **Example:**

    ::

        file=open("/Users/falcke/LOFAR/usg/data/lofar/oneshot_level4_CS017_19okt_no-9.h5")
        file["BLOCKSIZE"]=2**int(round(log(file["DATA_LENGTH"][0],2)))
        file["SELECTED_DIPOLES"]=[f for f in file["DIPOLE_NAMES"] if int(f)%2==1] # select uneven antenna IDs
        timeseries_data=file["TIMESERIES_DATA"]
        (start,end)=trun("LocatePulseTrain",rf.TimeBeamIncoherent(timeseries_data),nsigma=7,maxgap=3)
        start-=16
        end=start+int(2**ceil(log(end-start,2)));
        timeseries_data_cut=hArray(float,[timeseries_data.shape()[-2],end-start])
        timeseries_data_cut[...].copy(timeseries_data[...,start:end])
        timeseries_data_cut[...]-=timeseries_data_cut[...].mean()
        timeseries_data_cut[...]/=timeseries_data_cut[...].stddev(0)

        crosscorr_data=tasks.pulsecal.CrossCorrelateAntennas()(timeseries_data_cut,signvec2=hArray(int,[2],fill=[1,1])).crosscorr_data

        crosscorr_data.abs()
        crosscorr_data[...].runningaverage(15,hWEIGHTS.GAUSSIAN)
        crosscorr_data[0:4,...].plot()
    """
    parameters=dict(
        refant=p_(0,doc="Which data set in ``data`` to use as reference data set if no `reference_data`` provided."),
        oversamplefactor=p_(1,"Oversample the final cross-correlation by so many sub-steps. If equal to one then no oversampling."),
        dim=p_(lambda self:self.timeseries_data.shape(),doc="Dimension of input array.",output=True),
        dimfft=p_(lambda self:self.fft_data.shape(),doc="Dimension of fft array.",output=True),
        reference_data=p_(None,doc="Reference data set to cross-correlate with"),
        fft_data=p_(lambda self:cr.hArray(complex,[self.dim[-2],self.dim[-1]/2+1]),doc="FFT of the input timeseries data",workarray=True),
        fft_reference_data=p_(lambda self:cr.hArray(complex,[self.dimfft[-1]]),
                              doc="FFT of the reference data, dimensions [N data sets, (data length)/2+1]. If no extra reference antenna array is provided, cross correlate with reference antenna in data set",workarray=True),
        crosscorr_data_orig=p_(lambda self:cr.hArray(float,[self.dimfft[-2],(self.dimfft[-1]-1)*2]),
                                                     "Scratch cross correlation vector in original size to hold intermediate results",workarray=True),
        crosscorr_data=p_(lambda self:cr.hArray(float,[self.dimfft[-2],(self.dimfft[-1]-1)*2*self.oversamplefactor]),
                          doc="Output array of dimensions [N data sets, data length * oversamplefactor] containing the cross correlation.",output=True)
        )

    def call(self,timeseries_data):
        pass

    def run(self):
        if self.timeseries_data:
            self.fft_data[...].fftw(self.timeseries_data[...])
        if not self.reference_data:
            self.fft_reference_data.copy(self.fft_data[self.refant])
        else:
            self.fft_reference_data.fftw(self.reference_data)
        self.fft_data[...].crosscorrelatecomplex(self.fft_reference_data,True)

        self.fft_data /= self.crosscorr_data.shape()[-1]

        if self.oversamplefactor>1:
            self.shift=1.0/self.oversamplefactor
            for i in range(self.oversamplefactor):
                self.crosscorr_data_orig[...].invfftw(self.fft_data[...])
                self.crosscorr_data[...].redistribute(self.crosscorr_data_orig[...],i,self.oversamplefactor) # distribute with gaps in between and shift by a fraction
                self.fft_data[...].shiftfft(self.fft_data[...],self.shift) # apply a sub-sample shift to the FFt data
        else:
            self.crosscorr_data[...].invfftw(self.fft_data[...])

class FitMaxima(tasks.Task):
    """
    Find the maxima in multiple datsets and fit a basis spline
    polynomial to the data around their respective maximum.  Then
    determine where the maximum in the interpolated data will be. This
    allows one to determine the maximum with subsample precision.

    **Usage:**

    ``Task = FitMaxima()(data[n_datasets,n_samples], peak_width=20, nsubsamples=16, ncoeffs=5, splineorder=3, doplot=False, plotend=4, plotstart=0, sampleinterval=None, refant=None) -> Task.maxx, Task.maxy, Task.lag``

    The main result will be in ``Task.maxx`` which returns the
    (fractional) sample number of the maximum and ``Task.maxy`` the
    y-value of the maximum. ``Task.lag`` returns the relative offset of
    the peaks relative to the reference dataset (``refant``) in user
    units, specified by ``sampleinterval``.

    **See also:**

    :class:`CrossCorrelateAntennas`,
    :class:`LocatePulseTrain`

    **Example:**

    ::

        file=open("/Users/falcke/LOFAR/usg/data/lofar/oneshot_level4_CS017_19okt_no-9.h5")
        file["BLOCKSIZE"]=2**int(round(log(file["DATA_LENGTH"][0],2)))
        file["SELECTED_DIPOLES"]=[f for f in file["DIPOLE_NAMES"] if int(f)%2==1] # select uneven antenna IDs
        timeseries_data=file["TIMESERIES_DATA"]
        (start,end)=trun("LocatePulseTrain",rf.TimeBeamIncoherent(timeseries_data),nsigma=7,maxgap=3)
        start-=16
        end=start+int(2**ceil(log(end-start,2)));
        timeseries_data_cut=hArray(float,[timeseries_data.shape()[-2],end-start])
        timeseries_data_cut[...].copy(timeseries_data[...,start:end])
        timeseries_data_cut[...]-=timeseries_data_cut[...].mean()
        timeseries_data_cut[...]/=timeseries_data_cut[...].stddev(0)
        crosscorr_data=tasks.pulsecal.CrossCorrelateAntennas()(timeseries_data_cut).crosscorr_data
        crosscorr_data.abs()
        crosscorr_data[...].runningaverage(15,hWEIGHTS.GAUSSIAN)

        mx=trun("FitMaxima",crosscorr_data,doplot=True,refant=0,sampleinterval=5)

        print "Lags =",list(mx.lags),"nanoseconds."

    """
    parameters=dict(
        peak_width=p_(20,doc="Width of the data over which to fit the peak."),
        nsubsamples=p_(16,doc="Divide each original sample into that many finer pixels. Determines maximum resolution of fit."),
        ncoeffs=p_(lambda self:self.peak_width/2,doc="Number of breakpoints to fit spline polynomial around the peak."),
        splineorder=p_(3,doc="Order of spline to fit. 3=cubic spline."),
        refant=p_(None,"Reference antenna who's maximum determines the zero point of the x-axis (e.g., to get relative delays)."),
        sampleinterval=p_(None,"Separation of two subsequent samples on the x-axis in desired units - used to return lags in proper units."),
        doplot=p_(False,"Plot results."),
        plotend=p_(4,doc="Stop plotting at this data set."),
        plotstart=p_(0, doc="Start plotting at this data set."),
        legend=p_(None,doc="List containing labels for each antenna data set for plotting the label"),
        dim2=p_(lambda self:self.data.shape()[-2]),
        coeffs=p_(lambda self:cr.hArray(float,[self.dim2,self.ncoeffs],name="Spline Coefficients"),doc="Coefficients of spline fit.",output=True),
        fits_xdata=p_(lambda self:cr.hArray(dimensions=[self.dim2,(self.peak_width-1)*(self.nsubsamples)+1],properties=self.data,name="x"),workarray=True),
        fits_ydata=p_(lambda self:cr.hArray(dimensions=[self.dim2,(self.peak_width-1)*(self.nsubsamples)+1],properties=self.data,xvalues=self.fits_xdata,name="Fit"),workarray=True),
        covariance=p_(lambda self:cr.hArray(float,[self.dim2,self.ncoeffs,self.ncoeffs]),workarray=True,name="covariance"),
        xpowers=p_(lambda self:cr.hArray(float,[self.dim2,self.peak_width,self.ncoeffs]),workarray=True,name="Spline xpowers"),
        xdata=p_(lambda self:cr.hArray(float,[self.dim2,self.peak_width]),doc="x-axis value for the fit"),
        xvalues=p_(lambda self:cr.hArray(float,[self.data.shape()[-1]],fill=range(self.data.shape()[-1])),doc="Samplenumber for the time series data"),
        maxy=p_(None,"Y-values of fitted maxima",output=True),
        maxx=p_(None,"X-values of fitted maxima",output=True,unit="Samplenumber"),
        lags=p_(None,"X-values of fitted maxima",output=True,unit="User Units")
        )

    def call(self,data):
        pass

    def run(self):
        if self.peak_width < self.splineorder*2+2:
            raise ValueError("peak_width must be >= 2*splineorder+2!")
        self.maximum=cr.asvec(self.data[...].maxpos())
        self.start=cr.Vector(int,copy=self.maximum)
        self.start-=self.peak_width/2
        self.start.max(0)
        self.end=cr.Vector(int,copy=self.start)
        self.end+=self.peak_width
        self.startf=cr.Vector(float,copy=self.start)
        self.endf=cr.Vector(float,copy=self.end)
        self.fits_xdata[...].fillrange(self.startf,cr.asvec(1.0/self.nsubsamples))
        self.xdata[...].fillrange(self.start,cr.asvec(1))
        chisquare=self.coeffs[...].bsplinefit(self.covariance[...],self.xpowers[...],self.xdata[...],self.data[...,self.start:self.end],self.startf,self.endf,cr.asvec(self.splineorder+1))
        self.fits_ydata[...].bsplinecalc(self.fits_xdata[...],self.coeffs[...],self.startf,self.endf,cr.asvec(self.splineorder+1))
        self.maxy=self.fits_ydata[...].max()
        self.maxpos=self.fits_ydata[...].maxpos()
        self.maxx=self.fits_xdata[...].elem(self.maxpos)
        if self.doplot:
            self.data[self.plotstart:self.plotend,...].plot(xvalues=self.xvalues,legend=self.legend[self.plotstart:self.plotend] if self.legend else None)
            self.fits_ydata[self.plotstart:self.plotend,...].plot(clf=False)
            cr.plt.plot(self.maxx[self.plotstart:self.plotend],self.maxy[self.plotstart:self.plotend],marker="o",linestyle='None')
        self.lags=cr.Vector(copy=self.maxx)
        if self.sampleinterval:
            self.lags*=self.sampleinterval
        if not self.refant==None:
            self.lags-=self.lags[self.refant]


class DirectionFitTriangles(tasks.Task):
    """
    Find the direction of a source provided one has the measured delay
    of arrival and the positions of the antenna.

    Description:

    This task uses the function
    :func:`hDirectionTriangulationsCartesian` to calculate a list of
    arrival directions in Cartesian coordinates of a pulse/source from
    all possible triangles of (groups of three) antennas and the
    measured arrival times at these antennas.

    For the direction finding it is assumed that the signal arrives as
    a plane wave. If one triangle of antennas does not give a valid
    solution a solution at the horizion is returned with the complex
    part of the solution (the "closure error") returned in ``Task.error``.

    To cope with near-field sources for each triangle the coordinates
    of its center are also calculated. This allows one to later look
    for points where the dircetions from all triangles intercept.

    The inverse of the average lengths of the triangle baselines are
    returned in ``weights`` giving some indication how accurate the
    measurement in principle can be.

    From three antennas we get in general two solutions for the
    direction, unless the arrival times are out of bounds, i.e. larger
    than the light-time between two antennas.  Usually, when the three
    antennas are in a more or less horizontal plane, one of the
    solutions will appear to come from below the horizon (el < 0) and
    can be discarded. With `sign_of_solution` one can pick either the
    positive of the negative elevation.

    If :math:`N` triangles are provided then the *Directions*,
    *Origins*, and *weights* vectors have
    :math:`\\frac{N(N-1)(N-2)}{6}` results (times number of components
    of each result, i.e. times three for *origins* and times one for
    the rest.)


    The actually measured lag is assumed to consist of two parts: the
    geometric delay due to light travel times plus the cable delay in
    each antenna.

    ``timelag = geometric delay + cable delay``

    **Usage:**


    The main result will be in ``Task.meandirection`` which contains
    the direction vector to the source in Cartesian coordinates. See
    also ``Task.meandirection_azel`` and
    ``Task.meandirection_azel_deg``.

    **See also:**

    :class:`CrossCorrelateAntennas`,
    :class:`LocatePulseTrain`,
    :class:`FitMaxima`

    **Example:**

    ::

        file=open("$LOFARSOFT/data/lofar/oneshot_level4_CS017_19okt_no-9.h5")
        file["ANTENNA_SET"]="LBA_OUTER"
        file["BLOCKSIZE"]=2**17

        file["SELECTED_DIPOLES"]=["017000001","017000002","017000005","017000007","017001009","017001010","017001012","017001015","017002017","017002019","017002020","017002023","017003025","017003026","017003029","017003031","017004033","017004035","017004037","017004039","017005041","017005043","017005045","017005047","017006049","017006051","017006053","017006055","017007057","017007059","017007061","017007063","017008065","017008066","017008069","017008071","017009073","017009075","017009077","017009079","017010081","017010083","017010085","017010087","017011089","017011091","017011093","017011095"]

        timeseries_data=file["TIMESERIES_DATA"]
        positions=file["ANTENNA_POSITIONS"]

        #First determine where the pulse is in a simple incoherent sum of all time series data

        pulse=trun("LocatePulseTrain",timeseries_data,nsigma=7,maxgap=3)

        #Normalize the data which was cut around the main pulse
        pulse.timeseries_data_cut[...]-=pulse.timeseries_data_cut[...].mean()
        pulse.timeseries_data_cut[...]/=pulse.timeseries_data_cut[...].stddev(0)

        #Cross correlate all pulses with each other
        crosscorr=trun('CrossCorrelateAntennas',pulse.timeseries_data_cut,oversamplefactor=5)

        #And determine the relative offsets between them
        mx=trun('FitMaxima',crosscorr.crosscorr_data,doplot=True,refant=0,plotstart=4,plotend=5,sampleinterval=10**-9,peak_width=6,splineorder=2)

        #Now fit the direction and iterate over cable delays to get a stable solution
        direction=trun("DirectionFitTriangles",positions=positions,timelags=hArray(mx.lags),maxiter=10,verbose=True,doplot=True)

        print "========================================================================"
        print "Fit Arthur Az/El   ->  143.409 deg 81.7932 deg"
        print "Triangle Fit Az/EL -> ", direction.meandirection_azel_deg,"deg"

        # Triangle Fit Az/EL ->  (144.1118392216996, 81.84042919170588) deg for odd antennas
        # Triangle Fit Az/EL ->  (145.17844721833896, 81.973693266380721) deg for even antennas

    """
    parameters=dict(
        positions={doc:"hArray with Cartesian coordinates of the antenna positions",unit:"m"},
        timelags={doc:"hArray with Cartesian coordinates of the antenna positions",unit:"s"},
        expected_timelags=p_(lambda self:cr.hArray(float,[self.NAnt],name="Expected Time Lags"),"Exact time lags expected for each antenna for a given source position",unit="s"),
        geometric_timelags=p_(lambda self:cr.hArray(float,[self.NAnt],name="Geometric Time Lags"),"Time lags minus cable delay = pure geometric delay if no error",unit="s"),
        delays=p_(lambda self:cr.hArray(float,[self.NAnt],name="Delays"),"Instrumental delays needed to calibrate the array. Will be added to timelags and will be updated during iteration",unit="s"),
        delta_delays=p_(lambda self:cr.hArray(float,[self.NAnt],name="Delta Delays"),"Additional instrumental delays needed to calibrate array will be added to timelags and will be updated during iteration",unit="s"),
        delays_history=p_(lambda self:cr.hArray(float,[self.NAnt,self.maxiter],name="Delays"),"Instrumental delays for each iteration (for plotting)",unit="s"),
        maxiter=p_(1,"if >1 iterate (maximally tat many times) position and delays until solution converges."),
        delay_error=p_(1e-12,"Target for the RMS of the delta delays where iteration can stop.",unit="s"),
        rmsfactor=p_(2.,"How many sigma (times RMS) above the average can a delay deviate from the mean before it is considered bad."),
        unitscalefactor=p_(1e-9,"Scale factor to apply for printing and plotting."),
        unitname=p_("ns","Unit corresponding to scale factor."),
        doplot=p_(False,"Plot results."),
        plotant_start=p_(0,"First antenna to plot."),
        plotant_end=p_(lambda self:self.NAnt,"Last antenna to plot plus one."),
        verbose=p_(False,"Print progress information."),
        refant=p_(0,"Reference antenna for which geometric delay is zero."),
        solution=p_(1,"Can be +/-1, determine whether to take the upper or the lower ('into the ground') solution."),
        NAnt=p_(lambda self: self.timelags.shape()[-1],"Number of antennas and time lags. If set explicitly, take only the first NAnt antennas from ``Task.positions`` and ``Task.timelags``."),
        NTriangles=p_(lambda self:self.NAnt*(self.NAnt-1)*(self.NAnt-2)/6,"Number of Triangles = NAnt*(NAnt-1)*(NAnt-2)/6."),
        directions=p_(lambda self:cr.hArray(float,[self.NTriangles,3],name="Directions"),"Cartesian direction vector for each triangle"),
        centers=p_(lambda self:cr.hArray(float,[self.NTriangles,3],name="Centers of Triangles"),"Cartesian coordinates of center for each triangle.",unit="m"),
        errors=p_(lambda self:cr.hArray(float,[self.NTriangles],name="Closure Errors"),"Closure errors for each triangle (nor error if = 0)."),
        triangles_size=p_(lambda self:cr.hArray(float,[self.NTriangles],name="Triangle Size"),"Average size for each triangle.",unit="m"),
        index=p_(lambda self:cr.hArray(int,[self.NTriangles],name="Index"),"Index array of good triangles.",workarray=True),
        error_tolerance=p_(1e-10,"Level above which a closure error is considered to be non-zero (take -1 to ignore closure errors)."),
        ngood=p_(0,"Number of good triangles (i.e., without closure errors)",output=True),
        ngooddelays=p_(0,"Number of good delays (i.e., with only minor deviation)",output=True),
        delayindex=p_(lambda self:cr.hArray(int,[self.NAnt],name="Delay index"),"Index array of good delays.",workarray=True),
        meandirection=p_(lambda self:cr.hArray(float,[3]),"Cartesian coordinates of mean direction from all good triangles",output=True),
        meancenter=p_(lambda self:cr.hArray(float,[3]),"Cartesian coordinates of mean cetral position of all good triangles",output=True),
        goodones=p_(lambda self:cr.hArray(float,[self.NTriangles,3],name="Scratch array"),"Scratch array to hold good directions.",unit="m"),
        meandirection_spherical=p_(lambda self:pytmf.cartesian2spherical(self.meandirection[0],self.meandirection[1],self.meandirection[2]),"Mean direction in spherical coorindates."),
        meandirection_azel=p_(lambda self:(pi-(self.meandirection_spherical[2]+pi2),pi2-(self.meandirection_spherical[1])),"Mean direction as Azimuth, Elevation tuple."),
        meandirection_azel_deg=p_(lambda self:(180-(self.meandirection_spherical[2]+pi2)/deg,90-(self.meandirection_spherical[1])/deg),"Mean direction as Azimuth, Elevation tuple in degrees.")
        )

    def call(self):
        pass

    def run(self):

        self.farfield=True
        self.delta_delays_max=0
        self.delta_delays_min=1e99
        self.enditer=False
        if self.verbose:
            allantennas=set(range(self.NAnt))

        for it in range(self.maxiter):
            #Calculate directions from all triangles
            self.geometric_timelags.sub(self.timelags,self.delays)
            cr.hDirectionTriangulationsCartesian(self.directions,self.errors,self.centers,self.triangles_size,self.positions[:self.NAnt],self.geometric_timelags[:self.NAnt],+1)

            if self.doplot:  #store delays for plotting
                self.delays_history.redistribute(self.delays,it,self.maxiter)

            #Find antennas with zero closure errors.
            self.ngood=self.index.findlessthan(self.errors,self.error_tolerance).val()

            #Get mean position of good antennas
            self.goodones.copyvec(self.centers,self.index,self.ngood,3)
            self.meancenter.mean(self.goodones[:self.ngood])

            #Get mean direction from good antennas
            self.goodones.copyvec(self.directions,self.index,self.ngood,3)
            self.meandirection.mean(self.goodones[:self.ngood])           # mean direction of all good antennas
            self.meandirection /= self.meandirection.vectorlength().val() #normalize direction vector

            cr.hGeometricDelays(self.expected_timelags,self.positions,self.meandirection, self.farfield)
            self.expected_timelags -= self.expected_timelags[self.refant]
            self.delta_delays.sub(self.expected_timelags,self.geometric_timelags)

            self.delta_delays_mean=self.delta_delays.vec().mean()
            self.delta_delays_rms=self.delta_delays.vec().stddev(self.delta_delays_mean)
            self.delta_delays_max=max(self.delta_delays.vec().max(),self.delta_delays_max)
            self.delta_delays_min=min(self.delta_delays.vec().min(),self.delta_delays_min)

            rfac=self.rmsfactor*(1.0-float(it)/self.maxiter)
            self.ngooddelays=self.delayindex.findlessthanabs(self.delta_delays,self.delta_delays_mean+self.delta_delays_rms*rfac).val()
            if self.ngooddelays>0:
                self.delta_delays.set(self.delayindex[:self.ngooddelays],0.0)
                self.delays-=self.delta_delays
            else:
                self.enditer=True

            if self.verbose:
                print "------------------------------------------------------------------------"
                badantennas=allantennas.difference(set(self.delayindex[:self.ngooddelays])) if self.ngooddelays>0 else allantennas
                if self.maxiter>1:
                    print "Iteration #",it,", rms factor =",rfac
                self.ws.updateParameter("meandirection_spherical",forced=True)
                self.ws.updateParameter("meandirection_azel_deg",forced=True)
                print "Triangle Fit Az/EL -> ", self.meandirection_azel_deg,"deg"
                print "Triangle Fit Az/EL -> ", self.meandirection
                print "Mean delta delays ["+self.unitname+"] =",self.delta_delays_mean/self.unitscalefactor,"+/-",self.delta_delays_rms/self.unitscalefactor," (number bad antennas =",self.NAnt-self.ngooddelays,")"
                print "Bad Antennas = ",badantennas

            if self.delta_delays_rms<self.delay_error or self.enditer: #Solution has converged
                break

        #End of Iteration
        self.niter=it
        if self.verbose:
            print "------------------------------------------------------------------------"
            print "Number of iterations used: Task.niter =",self.niter
            print " "

        if self.doplot:
            self.delays_history /= self.unitscalefactor
            self.offset=cr.Vector(float,self.NAnt)
            self.offset.fillrange(0,(self.delta_delays_max-self.delta_delays_min)/self.unitscalefactor)
            self.delays_history[...] += self.offset
            self.delays_history[self.plotant_start:self.plotant_end,...,:it+1].plot(xlabel="Iteration #",ylabel="Delay (+offset)")
            self.delays_history[...] -= self.offset

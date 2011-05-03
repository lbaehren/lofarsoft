"""
A number of tools useful in calibrating radio data

"""

import pycrtools as cr
import pycrtools.tasks as tasks
from pycrtools.tasks.shortcuts import *
from math import *

deg=pi/180.
pi2=pi/2.

    
class LocatePulseTrain(tasks.Task):
    """
    **Usage**

    ``LocatePulseTrain()(timeseries_data[blocklen],nsigma=7,maxgap=7,minlength=7) -> (start,end)``

    **Description**
    Finds the pulse train with the highest peak in time series
    data. Calculates noise and threshold level above which to find
    pulse trains. A pulse train can have gaps of some n samples which
    are below the threshold and yet are considered part of the pulse
    train.

    Returns a tuple with start and end index of the strongest pulse in
    the time series data. See hFindSequenceGreaterThan for a
    description of the other parameters.
    

    **See also:**
    
    :func:`hFindSequenceGreaterThan`
    
    **Example**
    ::
        file=open("/Users/falcke/LOFAR/usg/data/lofar/oneshot_level4_CS017_19okt_no-9.h5") #
        file["BLOCKSIZE"]=2**int(round(log(file["DATA_LENGTH"][0],2)))
        file["SELECTED_DIPOLES"]=[f for f in file["DIPOLE_NAMES"] if int(f)%2==1] # select uneven antenna IDs
        timeseries_data=rf.TimeBeamIncoherent(file["TIMESERIES_DATA"])
        (start,end)=trun("LocatePulseTrain",timeseries_data)
        timeseries_data.plot(highlight=(start,end),nhighlight=1)
    """
    parameters=dict(
        nblocks=p_(16,"The time series data is subdivided in ``nblock`` blocks where the one with the lowest RMS is used to determine the threshold level for finding peaks."),
        nsigma=p_(7,"pulse has to be ``nsigma`` above the noise"),
        maxgap=p_(7,"maximum gap length allowed in a pulse where data is not above the threshold"),
        minlength=p_(7,"minimum length of pulse returned"),
        
        )
        
    def call(self,timeseries_data):
        pass
    
    def run(self):
        minrms=cr.Vector(float,1)
        minmean=cr.Vector(float,1)
        blen=cr.Vector(int,1,fill=max(self.timeseries_data.getSize()/self.nblocks,8))
        self.timeseries_data.minstddevblock(blen,minrms,minmean)
        indexlist=cr.hArray(int,[blen[0],2])
        npeaks=indexlist.findsequencegreaterthan(self.timeseries_data,minmean[0]+self.nsigma*minrms[0],self.maxgap,self.minlength).first()
        maxima=cr.hArray(float,[npeaks])
        maxima.maxinsequences(self.timeseries_data,indexlist,npeaks)
        maxseq=maxima.maxpos().first()
        return tuple(indexlist[maxseq].vec())

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
        dim=p_(lambda self:self.timeseries_data.shape(),doc="Dimension of input array."),
        dimfft=p_(lambda self:self.fft_data.shape(),doc="Dimension of fft array."),
        reference_data=p_(None,doc="Reference data set to cross-correlate with"),
        fft_data=p_(lambda self:cr.hArray(complex,[self.dim[-2],self.dim[-1]/2+1]),doc="FFT of the input timeseries data",workarray=True),
        fft_reference_data=p_(lambda self:cr.hArray(complex,[self.dimfft[-1]]), 
                              doc="FFT of the reference data, dimensions [N data sets, (data length)/2+1]. If no extra reference antenna array is provided, cross correlate with reference antenna in data set"),
        crosscorr_data=p_(lambda self:cr.hArray(float,[self.dimfft[-2],(self.dimfft[-1]-1)*2]),
                          doc="Output array of dimensions [N data sets, data length] containing the cross correlation.")
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
        self.crosscorr_data[...].invfftw(self.fft_data[...])  
        self.crosscorr_data /= self.crosscorr_data.shape()[-1]


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
        sampleinterval=p_(None,"Separation of two subsequent samples on the x-axis."),
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

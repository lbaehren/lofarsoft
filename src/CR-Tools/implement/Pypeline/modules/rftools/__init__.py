"""
A number of tools useful in calibrating radio data

execfile("/Users/falcke/LOFAR/usg/src/CR-Tools/implement/Pypeline/modules/rftools/__init__.py")
"""

from math import *
import pycrtools as cr

deg=pi/180.
pi2=pi/2.

def makeAZELRDictGrid(AZ,EL,Distance,nx=3,ny=3,offset=5*deg):
    """
    Make a list containing direction dicts that represent a grid of
    ``nx * ny`` pointings around a central value specified by
    azimuth, elevation (in radians), and distance (in meter).
    """
    return [dict(az=AZ+(x-nx/2)*offset, el=EL+(y-ny/2)*offset,r=Distance) for y in range(ny)  for x in range(nx)]

def TimeBeamIncoherent(timeseries_data,incoherent_sum=None):
    """
    **Usage:**

    ``TimeBeamIncoherent(timeseries_data[nAntennas,blocklen],incoherent_sum=None) -> incoherent_sum.squareadd(timeseries_data)''

    returns ``incoherent_sum[blocklen]``

    **Description:**
    
    Calculate the incoherent sum of the timeseries data of multiple
    antennas. Will square the E-Field and add all antennas into one
    time series, thus containing the 'incoherent power'.
    
    **Example:**
    ::
    file=cr.open("/Users/falcke/LOFAR/usg/data/lofar/oneshot_level4_CS017_19okt_no-9.h5") #
    file["BLOCKSIZE"]=2**int(round(log(file["DATA_LENGTH"][0],2)))
    file["SELECTED_DIPOLES"]=[f for f in file["DIPOLE_NAMES"] if int(f)%2==1] # select uneven antenna IDs
    timeseries_data=TimeBeamIncoherent(file["TIMESERIES_DATA"])
    timeseries_data.plot()
    """
    dims=timeseries_data.shape()
    if not incoherent_sum:
        incoherent_sum=cr.hArray(float,[dims[-1]])
    incoherent_sum.squareadd(timeseries_data)
    return incoherent_sum
    
def LocatePulseTrain(timeseries_data,nblocks=16,nsigma=7,maxgap=7,minlength=7):
    """
    Finds the pulse train with the highest peak in time series
    data. Calculates noise and threshold level above which to find
    pulse trains. A pulse train can have gaps of some n samples which
    are below the threshold and yet are considered part of the pulse
    train.

    Returns a tuple with start and end index of the strongest pulse in
    the time series data. See hFindSequenceGreaterThan for a
    description of the other parameters.
    
    **Usage**

    LocatePulseTrain(timeseries_data[blocklen],nsigma=7,maxgap=7,minlength=7) -> (start,end)

    **Parameters**
    
    *nblocks* - The time series data is subdivided in ``nblock``
     blocks where the one with the lowest RMS is used to determine the
     threshold level for finding peaks.

    *nsigma* - pulse has to be ``nsigma`` above the noise 

    *maxgap* - maximum gap length allowed in a pulse where data is not
     above the threshold

    *minlength* - minimum length of pulse returned
    
    **See also:**
    
    hFindSequenceGreaterThan
    
    **Example:**
    :: 
    file=open("/Users/falcke/LOFAR/usg/data/lofar/oneshot_level4_CS017_19okt_no-9.h5") #
    file["BLOCKSIZE"]=2**int(round(log(file["DATA_LENGTH"][0],2)))
    file["SELECTED_DIPOLES"]=[f for f in file["DIPOLE_NAMES"] if int(f)%2==1] # select uneven antenna IDs
    timeseries_data=rf.TimeBeamIncoherent(file["TIMESERIES_DATA"])
    (start,end)=rf.LocatePulseTrain(timeseries_data)
    timeseries_data.plot(highlight=(start,end),nhighlight=1)

    """
    minrms=cr.Vector(float,1)
    minmean=cr.Vector(float,1)
    blen=cr.Vector(int,1,fill=max(timeseries_data.getSize()/nblocks,8))
    timeseries_data.minstddevblock(blen,minrms,minmean)
    indexlist=cr.hArray(int,[blen[0],2])
    npeaks=indexlist.findsequencegreaterthan(timeseries_data,minmean[0]+nsigma*minrms[0],maxgap,minlength).val()
    maxima=cr.hArray(float,[npeaks])
    maxima.maxinsequences(timeseries_data,indexlist,npeaks)
    maxseq=maxima.maxpos().val()
    return tuple(indexlist[maxseq].vec())

def CrossCorrelateAntennas(timeseries_data,refant=0,reference_data=None,reference_fft_data=None,fft_data=None,fft_reference_data=None,crosscorr_data=None):
    """    
    **Usage**

    CrossCorrelateAntennas(timeseries_data[n_dataset,size],refant=0,reference_data=None,reference_fft_data=None,fft_data=None,fft_reference_data=None,crosscorr_data=None) -> strength of crosscorrelation as a function of time

    **Description**

    Calculates the cross-correlation of a number of time series data
    sets with respective to a reference data set.

    The data set is a matrix of N equal-length timeseries. If not
    reference data set is provided, a reference antenna from the input
    time series matrix is specified (by default = 0, i.e. all data is
    correlated with the first data set, then the first cross
    correlation output will actually be an autocorrelation)

    **Parameters**
    *timeseries_data* - array of [n_dataset,size],
    refant=0,reference_data=None,reference_fft_data=None,fft_data=None,fft_reference_data=None,crosscorr_data=None
    
    **Example:**
    ::
    file=open("/Users/falcke/LOFAR/usg/data/lofar/oneshot_level4_CS017_19okt_no-9.h5") 
    file["BLOCKSIZE"]=2**int(round(log(file["DATA_LENGTH"][0],2)))
    file["SELECTED_DIPOLES"]=[f for f in file["DIPOLE_NAMES"] if int(f)%2==1] # select uneven antenna IDs
    timeseries_data=file["TIMESERIES_DATA"]
    (start,end)=rf.LocatePulseTrain(rf.TimeBeamIncoherent(timeseries_data),nsigma=7,maxgap=3)
    start-=16
    end=start+int(2**ceil(log(end-start,2)));  
    timeseries_data_cut=hArray(float,[timeseries_data.shape()[-2],end-start])
    timeseries_data_cut[...].copy(timeseries_data[...,start:end])
    timeseries_data_cut[...]-=timeseries_data_cut[...].mean()
    timeseries_data_cut[...]/=timeseries_data_cut[...].stddev(0)
    crosscorr_data=rf.CrossCorrelateAntennas(timeseries_data_cut)
    crosscorr_data.abs()
    crosscorr_data[...].runningaverage(15,hWEIGHTS.GAUSSIAN)
    crosscorr_data[0:4,...].plot()
    """
    if fft_data==None:
        dim=timeseries_data.shape()
        fft_data=cr.hArray(complex,[dim[-2],dim[-1]/2+1])
    if fft_reference_data==None:
        dim=timeseries_data.shape()
        fft_reference_data=cr.hArray(complex,[dim[-1]/2+1])
    if crosscorr_data==None:
        crosscorr_data=cr.hArray(properties=timeseries_data)

    fft_data[...].fftcasa(timeseries_data[...],1)

    #if no extra reference antenna array is provided, cross correlate with reference antenna in data set
    if not reference_data:  
        fft_reference_data.copy(fft_data[refant])
    else:
        fft_reference_data.fftcasa(reference_data,1)

    fft_data2=cr.hArray(copy=fft_data)
    fft_data2[...].crosscorrelatecomplex(fft_reference_data)
    crosscorr_data[...].invfftcasa(fft_data2[...],1)  #invfftw does not work here, please explain to me why ...
    return crosscorr_data

def FitMaxima(data,width=20,nsubsamples=16,ncoeffs=5,splineorder=3,doplot=False,plotend=4,plotstart=0):
    """

    Find the maxima in multiple datsets and fit a polynomial to the data around their respective maximum.
    Then determine where the maximum in the interpolated data will be. This allows one
    to determine the maximum with subsample precision.
    
    **Usage:**

    FitMaxima(data[n_datasets,n_samples],width=20,nsubsamples=16,ncoeffs=5,splineorder=3,doplot=False,plotend=4,plotstart=0)
    
    **Example:**

    data=crosscorr_data[...]
    """
    dim=data.shape()[-2:]
    dim[-1]=ncoeffs
    coeffs=hArray(float,dim)
    maximum=asvec(data[...].maxpos())
    start=Vector(int,copy=maximum)
    end=Vector(int,copy=maximum)
    start-=width/2
    end+=width/2
    startf=Vector(float,copy=start)
    endf=Vector(float,copy=end)
    dim[-1]=(width-1)*(nsubsamples)+1
    fits_xdata=hArray(dimensions=dim,properties=data)
    fits_xdata[...].fillrange(startf,asvec(1.0/nsubsamples))
    fits_ydata=hArray(dimensions=dim,properties=data,xvalues=fits_xdata)
    covariance=hArray(float,[dim[-2],ncoeffs,ncoeffs])
    xpowers=hArray(float,[dim[-2],width,ncoeffs])
    xdata=hArray(float,[dim[-2],width])
    xdata[...].fillrange(start,asvec(1))
    chisquare=coeffs[...].bsplinefit(covariance[...],xpowers[...],xdata[...],data[...,start:end],startf,endf,asvec(splineorder+1))
    fits_ydata[...].bsplinecalc(fits_xdata[...],coeffs[...],startf,endf,asvec(splineorder+1))
    maxy=fits_ydata[...].max()
    maxpos=fits_ydata[...].maxpos()
    maxx=fits_xdata[...].elem(maxpos)
    if doplot:
        data[plotstart:plotend,...].plot()
        fits_ydata[plotstart:plotend,...].plot(clf=False)
        plt.plot(maxx,maxy,marker="o",linestyle='None',clf=False)
    return (maxx,maxy)

"""
A number of tools useful in calibrating radio data

execfile("/Users/falcke/LOFAR/usg/src/CR-Tools/implement/Pypeline/modules/rftools/__init__.py")
"""
from pycrtools import *
from math import *

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
    Usage:

    TimeBeamIncoherent(timeseries_data[nAntennas,blocklen],incoherent_sum=None) -> incoherent_sum[blocklen]

    Description:
    
    Calculate the incoherent sum of the timeseries data of multiple
    antennas. Will square the E-Field and add all antennas into one
    time series, thus containing the 'incoherent power'.

    
    Example:
file=open("/Users/falcke/LOFAR/usg/data/lofar/oneshot_level4_CS017_19okt_no-9.h5") #
file["BLOCKSIZE"]=2**int(round(log(file["DATA_LENGTH"][0],2)))
file["SELECTED_DIPOLES"]=[f for f in file["DIPOLE_NAMES"] if int(f)%2==1] # select uneven antenna IDs
timeseries_data=TimeBeamIncoherent(file["TIMESERIES_DATA"])
timeseries_data.plot()
    """
    dims=timeseries_data.shape()
    if not incoherent_sum:
        incoherent_sum=hArray(float,[dims[-1]])
    incoherent_sum.squareadd(timeseries_data)
    return incoherent_sum
    
def LocatePulseTrain(timeseries_data,nblocks=16,nsigma=7,maxgap=7,minlength=7):
    """
    Finds the pulse train with the highest peak in time series
    data. Calculates noise and threshold level above which to find
    pulse trains. A pulse train can have gaps of some n samples which
    are below the threshold and yet are considered part of the pulse
    train.
    
    Usage:

    LocatePulseTrain(timeseries_data[blocklen],nsigma=7,maxgap=7,minlength=7) -> (start,end)

    Returns a tuple with start and end index of the strongest pulse in
    the time series data. See hFindSequenceGreaterThan for a
    description of the other parameters.

    *nblocks* = 16 - The time series data is subdivided in ``nblock``
     blocks where the one with the lowest RMS is used to determine the
     threshold level for finding peaks.
    
    Example:

file=open("/Users/falcke/LOFAR/usg/data/lofar/oneshot_level4_CS017_19okt_no-9.h5") #
file["BLOCKSIZE"]=2**int(round(log(file["DATA_LENGTH"][0],2)))
file["SELECTED_DIPOLES"]=[f for f in file["DIPOLE_NAMES"] if int(f)%2==1] # select uneven antenna IDs
timeseries_data=TimeBeamIncoherent(file["TIMESERIES_DATA"])
(start,end)=LocatePulseTrain(timeseries_data)
timeseries_data.plot(highlight=(start,end),nhighlight=1)

    """
    minrms=Vector(float,1)
    minmean=Vector(float,1)
    blen=Vector(int,1,fill=max(timeseries_data.getSize()/nblocks,8))
    timeseries_data.minstddevblock(blen,minrms,minmean)
    indexlist=hArray(int,[blen[0],2])
    npeaks=indexlist.findsequencegreaterthan(timeseries_data,minmean[0]+nsigma*minrms[0],maxgap,minlength).val()
    maxima=hArray(float,[npeaks])
    maxima.maxinsequences(timeseries_data,indexlist,npeaks)
    maxseq=maxima.maxpos().val()
    return tuple(indexlist[maxseq].vec())

def CrossCorrelateAntennas(timeseries_data,refant=0,reference_data=None,reference_fft_data=None,fft_data=None,fft_reference_data=None,crosscorr_data=None):
    """
    Example:
file=open("/Users/falcke/LOFAR/usg/data/lofar/oneshot_level4_CS017_19okt_no-9.h5") #
file["BLOCKSIZE"]=2**int(round(log(file["DATA_LENGTH"][0],2)))
file["SELECTED_DIPOLES"]=[f for f in file["DIPOLE_NAMES"] if int(f)%2==1] # select uneven antenna IDs
timeseries_data=file["TIMESERIES_DATA"]
(start,end)=LocatePulseTrain(TimeBeamIncoherent(timeseries_data))
extension=32
length=int(2**ceil(log(end-start+extension,2))); center=(end+start)/2
start=center-length/2; end=start+length
timeseries_data_cut=hArray(float,[timeseries_data.shape()[-2],end-start])
timeseries_data_cut[...].copy(timeseries_data[...,start:end])

crosscorr_data=CrossCorrelateAntennas(timeseries_data)
    """
    if fft_data==None:
        dim=timeseries_data.shape()
        fft_data=hArray(complex,[dim[-2],dim[-1]/2+1])
    if fft_reference_data==None:
        dim=reference_data.shape()
        fft_reference_data=hArray(complex,[dim[-1]/2+1])
    if crosscorr_data==None:
        crosscorr_data=hArray(properties=timeseries_data)

    fft_data[...].fftw(timeseries_data[...])

    #if no extra reference antenna array is provided, cross correlate with reference antenna in data set
    if not reference_data:  
        fft_reference_data.copy(fft_data[refant])
    else:
        fft_reference_data.fftw(reference_data)
    
    fft_data[...].crosscorrelatecomplex(fft_reference_data)
    crosscorr_data[...].invfft(fft_data[...])
    return crosscorr_data
    
    

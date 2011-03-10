#

from pycrtools import *
import time

#import pdb
#pdb.set_trace()

def CRQualityCheckAntenna(dataarray,qualitycriteria=None,normalize=False,nsigma=-1,rmsfactor=2,meanfactor=3,spikyness=20,spikeexcess=20,refblock=None,blockoffset=0,antennaID=None,date=None,datafile=None,observatory=None,observatorymode=None,verbose=True,filename="",count=""):
    """
    Usage:
    CRQualityCheckArray(dataarray,qualitycriteria=None,antennaID=0,nsigma=-1,rmsfactor=2,meanfactor=7,spikyness=7,spikeexcess=7,refblock=None,verbose=True) -> list of antennas failing the qualitycriteria limits

    Do a basic quality check of raw time series data, looking for rms,
    mean and spikes and return a list of antennas which have failed
    the quality check and their statistical properties.

    The data array is expected to be split into blocks (purely the
    dimension of the data array). The algorithm will then caclulate
    the minium RMS and mean of all the blocks and use that minimum as
    a reference value. If blocks deviate too much from these values,
    they are being flagged and returned.

    Example:
    >> datafile=crfile(filename); nblocks=10
    >> dataarray=hArray(float,[nblocks,datafile["blocksize"]])
    >> datarray[...].read(datafile,"Fx",range(nblocks))
    >> qualitycriteria={"mean":(-15,15),"rms":(5,15),"spikyness":(-7,7),"spikeexcess":(-1,7)}
    >> flaglist=CRQualityCheck(dataarray,qualitycriteria=qualitycriteria,antennaID=0,nsigma=-1,rmsfactor=2,meanfactor=7,spikyness=20,spikeexcess=20,refblock=None,verbose=True)

    Parameters:

    *dataarray* - an array containing the data with the dimensions
    [nblocks,blocksize]. 

    *qualitycriteria* - a Python dict with keywords of parameters and
    tuples with limits thereof (lower, upper). Keywords currently
    implemented are mean, rms, spikyness (i.e. spikyness).
    Example: qualitycriteria={"mean":(-15,15),"rms":(5,15),"spikyness":(-7,7)}

    *normalize* - If true subtract the mean from the data and divide by the rms.
    
    *antennaID* - the ID of the current antenna (for output only)

    *blockoffset* - offset of the first block from the beginning of the
    file (for output only)

    *date* - Date of observation (for output only) - (GMT-)seconds since 1.1.1970 (standard UNIX)

    *datafile* - to obtain date, time, antennaID from file directly (output only)

    *nsigma* - determines for the peak counting algorithm the
    threshold for peak detection in standard deviations. If nsigma is
    negative set the sigma level such that -nsigma peaks are expected.

    *rmsfactor* - if no quality criteria present, set limits for rms at this
    factor times the rms in the reference block (or divided by this factor)

    *meanfactor* - if no quality criteria present, set limits for mean
    at mean[refblock]-rms[refblock]/sqrt(blocksize)*meanfactor

    *spikyness* - if no quality criteria present, set maximum
    spikyness level to this values and minimum value to
    -spikyness. The spiykness is the detected number of peaks above
    n*sigma minus the expected number divided by the square root
    (i.e. error) of expected peak. For Gaussian noise one expects this
    to fluctuate +/- 1 around zero. The value dervide from the data
    can be negative if too few peaks are detected.

    *spikeexcess* - if no quality criteria present, set maximum allowed
    ratio of detected over expected peaks per block to this level (1
    is roughly what one expects from Gaussian noise). In contrast to
    'spikyness' this value cannot be negative.

    *refblock* = None - if >=0 use this block as reference for mean/rms to
    calculate limits. For -1 pick one in the first quarter of
    blocks, otherwise take minimum RMS/MEAN in the data array.

    *observatory* - for output and archiving only

    *observatorymode* - for output and archiving only

    *filename* - for output and archiving only

    *count* - an identifier to print in front of flag information (output only)

    *verbose* - sets whether or not to print additional information
    """
    dim=dataarray.getDim()
    (nblocks,blocksize)=dim
    qualityflaglist=[]
    nblocksflagged=0
    flaggedblocklist=[]
    noncompliancelist=[]
    if not datafile==None:
        if antennaID==None: antennaID=datafile["AntennaIDs"][datafile["selectedAntennas"][0]]
        if date==None: date=datafile["Date"]
        if observatory==None: observatory=datafile["Observatory"]
        if observatorymode==None: observatorymode="unknown"
	if filename=="": filename=datafile.filename
#Calculate probabilities to find peaks
    if nsigma<0:
	nsigma=sqrt(-2.0*log(sqrt(2.*pi)/blocksize))  # set nsigma such that the expected number of peaks is one
    probability=funcGaussian(nsigma,1,0) # what is the probability of a 5 sigma peak
    npeaksexpected=probability*blocksize # what is the probability to see such a peak for the given blocksize
    npeaksexpected_full=npeaksexpected*nblocks # what is the probability to see such a peak for the given blocksize
    npeakserror=sqrt(npeaksexpected) # what is the statistical error on that expectation
#Start checking and calculate mean, rms, and number of peaks
    datamean = dataarray[...].mean()
    datarms = dataarray[...].stddev(datamean)
#    mean=datamean[datamean.minpos()]; rms=datarms[datarms.minpos()];
#    mean=datamean.mean(); rms=datarms.mean();
    mean=datamean.median(); rms=datarms.median();
    datamean -= mean;  datarms /= rms
    if normalize:
        dataarray -= mean
        dataarray /= rms
        lower_limit=Vector(float,nblocks,fill=-nsigma)
        upper_limit=Vector(float,nblocks,fill=nsigma)
    else:
        lower_limit=datamean-datarms*nsigma;
        upper_limit=datamean+datarms*nsigma;
    datanpeaks = dataarray[...].countoutside(lower_limit,upper_limit)
    npeaks=datanpeaks.sum(); peaksexcess=npeaks/npeaksexpected_full
    if verbose:
#        print "Blocksize=",blocksize,", nsigma=",nsigma, ", number of peaks expected per block=",npeaksexpected,"+/-",npeakserror
        print "{8} - Mean={0:6.2f}, RMS={1:6.2f}, Npeaks={2:5d}, Nexpected={3:6.2f} (Npeaks/Nexpected={4:6.2f}), nsigma={7:6.2f}, limits=({5:6.2f}, {6:6.2f})".format(mean,rms,npeaks,npeaksexpected_full,peaksexcess,lower_limit.mean(),upper_limit.mean(),nsigma,count)
    dataNonGaussianity = Vector(float,nblocks)
    dataSpikeExcess = Vector(float,nblocks)
    dataNonGaussianity.sub(datanpeaks,npeaksexpected)
    dataSpikeExcess.div(datanpeaks,npeaksexpected)
    dataNonGaussianity /= npeakserror
    dataproperties=zip(range(blockoffset,nblocks+blockoffset),datamean,datarms,datanpeaks,dataNonGaussianity,dataSpikeExcess)
    if qualitycriteria==None:
        if refblock==-1:
            refblock=nblocks/4 # Take something around the first quarter of the data
        if not refblock==None:
            mean=datamean[refblock]; rms=datarms[refblock];
        if normalize:
            qualitycriteria={"mean":(-rms*meanfactor/sqrt(blocksize),rms*meanfactor/sqrt(blocksize)),"rms":(1/rmsfactor,rmsfactor),"spikyness":(-spikyness,spikyness),"spikeexcess":(-1,spikeexcess)}
        else:
            qualitycriteria={"mean":(mean-rms/sqrt(blocksize)*meanfactor,mean+rms/sqrt(blocksize)*meanfactor),"rms":(rms/rmsfactor,rms*rmsfactor),"spikyness":(-spikyness,spikyness),"spikeexcess":(-spikeexcess,spikeexcess)}
#        if verbose:
#            print "Quality criteria =",qualitycriteria
    for prop in iter(dataproperties):
        noncompliancelist=CheckParameterConformance(prop,{"mean":1,"rms":2,"spikyness":4,"spikeexcess":5},qualitycriteria)
        if noncompliancelist:
            nblocksflagged+=1
            flaggedblocklist.append(prop[0])
            qualityflaglist.append({"block":prop[0],"mean":prop[1],"rms":prop[2],"npeaks":prop[3],"spikyness":prop[4],"spikeexcess":prop[5],"flags":noncompliancelist})
            if verbose:
                print "- Block {0:5d}: mean={1: 6.2f}, rel. rms={2:6.1f}, npeaks={3:5d}, spikyness={4: 7.2f}, spikeexcess={5: 6.2f}".format(*prop)," ",noncompliancelist
    return {"filename":filename,"type":"QualityFx","observatory":observatory,"mode":observatorymode,"antenna":antennaID,"date":'"'+time.strftime("%Y-%m-%d %H:%M:%S %z",time.gmtime(date))+'"',"idate":date,"offset":blockoffset,"size":blocksize*nblocks,"blocksize":blocksize,"nblocks":nblocks,"mean":mean,"rms":rms,"npeaks":npeaks,"npeaksexpected":npeaksexpected_full,"peaksexcess":peaksexcess,"nblocksflagged":nblocksflagged,"flaggedblocks":flaggedblocklist,"flags":qualityflaglist}


def CRDatabaseWrite(filename, quality):
    """Write a quality entry calculated by CRQualityCheckAntenna to a database (currently a simple textfile)

    """
    qkeys=quality.keys()
    orderedkeys=["type","observatory","mode","antenna","idate","date","size","blocksize","offset","nblocks","mean","rms","npeaks","npeaksexpected","peaksexcess","nblocksflagged","flaggedblocks","flags"]
    keys=orderedkeys+list(set(qkeys).difference(orderedkeys))
    f=open(filename,"a")
    f.write("#"+time.strftime("%Y-%m-%d %H:%M:%S %z")+"\n")
    for k in keys:
        if k in qkeys:
            f.write(k+"= "+str(quality[k])+"  ")
    f.write("\n")
    f.close()
        

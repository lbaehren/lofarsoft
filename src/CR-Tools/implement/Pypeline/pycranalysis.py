#------------------------------------------------------------------------
# Pypeline Extension, Functions and Algorithms
#------------------------------------------------------------------------

import time as time

def hCRAverageSpectrum(spectrum,datafile,blocks=None,fx=None,fft=None, verbose=False):
    """
    Usage: CRAverageSpectrum(spectrum,datafile,blocks=None,fx=None,fft=None)

    Reads several blocks in a CR data file, does an FFT and then
    averages the powers to calculuate an average spectrum. The
    parameters are:

    spectrum - a float array of dimensions [nofAntennas,fftLength],
    containing the average spectrum.
    
    datafile - a datareader object where the block size has been set
    appropriately.

    blocks - a list of blocknumbers to read
    (e.g. range(number_of_blocks)). Default is to read all blocks.

    fx - a work Array of dimensions
    [datafile.nofAntennas,datafile.blocksize] which is used to read in
    the raw antenna data. Will be created if not provided.

    fft - a work Array of dimensions
    [datafile.nofAntennas,datafile.fftLength] which is used to
    calculate the FFT from the raw antenna data. Will be created if
    not provided.

    verbose - Provide progress messages

    """
    if fx==None: fx=datafile["emptyFx"]
    if fft==None: fft=datafile["emptyFFT"]
    if blocks==None: blocks=range(datafile.filesize/datafile.blocksize)
    if verbose:
        count=0; 
        maxcount=len(blocks)
        lastprogress=-1
        print "Calculating",maxcount,"blocks of size",datafile.blocksize
        t0=time.clock()
    for block in blocks:
        fx.read(datafile,"Fx")
        fft[...].fftw(fx[...])
        spectrum[...].spectralpower(fft[...])
        if verbose:
            count +=1
            progress=count*10/maxcount
            if not lastprogress == progress:
                t=time.clock()-t0
                print progress*10,"% -",t,"s (Remaining:",t/count*maxcount-t,"s) - Calculated block #",block
                lastprogress=progress


def CheckParameterConformance(data,keys,limits):
    """
    Usage:

    qualitycriteria={"mean":(-15,15),"rms":(5,15),"nonGaussianity":(-3,3)}

    CheckParameterConformance([Antenna,mean,rms,npeaks,nonGaussianity],{"mean":1,"rms":2,"nonGaussianity":4},qualitycriteria)  ->  ["rms",...]

    Parameters:

    data -  is a list of quality values (i.e. numbers) to check

    keys - a dictionary of fieldnames to be checked and indices
           telling, where in data the field can be found

    limits - a dictionary of fieldnames and limits (lowerlimit,
             upperlimit)

    Checks whether a list of numbers is within a range of limits. The
    limits are provided as a dictionary of fieldnames and tuples, of
    the form FIELDNAME:(LOWERLIMT,UPPERLIMIT). A list of fieldnames is
    returned where the data does not fall within the specified range.

    """
    result=[]
    for k in keys:
        if (data[keys[k]]<limits[k][0]) | (data[keys[k]]>limits[k][1]): result.append(k)
    return result


def CRQualityCheck(limits,datafile=None,dataarray=None,maxblocksize=65536,nsigma=5,verbose=True):
    """
    Usage:

    CRQualityCheck(qualitycriteria,datafile,dataarray=None,maxblocksize=65536,nsigma=5,verbose=True) -> list of antennas failing the limits

    qualitycriteria={"mean":(-15,15),"rms":(5,15),"nonGaussianity":(-3,3)}

    Will step through all antennas of a file assess the data quality
    and return a list with antennas which have failed the quality
    check and their statistical properties.

    Parameters:


    qualitycriteria - a Python dict with keywords of parameters and limits thereof (lower, upper)

    datafile - Data Reader file object, if none, use values in dataarray and don't read in again

    array - an optional data storage array to read in the data

    maxblocksize - The algorithms takes by default the first and last
    quarter of a file but not more samples than given in this
    paramter.

    nsigma - determines for the peak counting algorithm the threshold
    for peak detection in standard deviations

    verbose - sets whether or not to print additional information
    """
#Initialize some parameters
    if not datafile==None:
        nAntennas=datafile.get("nofSelectedAntennas")
        selected_antennas=datafile.get("selectedAntennas")
        filesize=datafile.get("filesize")
        blocksize=min(filesize/4,maxblocksize)
        datafile.set("blocksize",blocksize)
        nBlocks=filesize/blocksize;
        blocklist=range(nBlocks/4)+range(3*nBlocks/4,nBlocks)
        if dataarray==None: dataarray=hArray(float,[nAntennas,blocksize])
    else:
        nAntennas=dataarray.getDim()[0]
        blocksize=dataarray.getDim()[1]
        selected_antennas=range(nAntennas)
        blocklist=[0]
#Create the some scratch vectors
    qualityflaglist=[]
#Calculate probabilities to find certain peaks
    probability=funcGaussian(nsigma,1,0) # what is the probability of a 5 sigma peak
    npeaksexpected=probability*blocksize # what is the probability to see such a peak with the blocksize
    npeakserror=sqrt(npeaksexpected) # what is the statisitcal error on that expectation
#Start checking
    if verbose:
        if not datafile==None: print "Quality checking of file ",datafile.filename
        print "Considering",nAntennas," antennas and the Blocks:",blocklist
        print "Blocksize=",blocksize,", nsigma=",nsigma, ", number of peaks expected per block=",npeaksexpected
    for Block in blocklist:
        if verbose:
            print "\nBlock = ", Block
            print "-----------------------------------------------------------------------------------------"
        if not datafile==None: datafile.set("block",Block).read("Voltage",dataarray.vec())
        datamean = dataarray[...].mean()
        datarms = dataarray[...].stddev(datamean)
        datanpeaks = dataarray[...].countgreaterthanabs(datarms*nsigma)
        dataNonGaussianity = Vector(float,nAntennas)
        dataNonGaussianity.sub(datanpeaks,npeaksexpected)
        dataNonGaussianity /= npeakserror
        dataproperties=zip(selected_antennas,datamean,datarms,datanpeaks,dataNonGaussianity)
        noncompliancelist=[]
        for prop in iter(dataproperties):
            noncompliancelist=CheckParameterConformance(prop,{"mean":1,"rms":2,"nonGaussianity":4},limits)
            if noncompliancelist: qualityflaglist.append([prop[0],Block,prop[1:],noncompliancelist])
            if verbose:
                print "Antenna {0:3d}: mean={1: 6.2f}, rms={2:6.1f}, npeaks={3:5d}, nonGaussianity={4: 7.2f}".format(*prop)," ",noncompliancelist
    return qualityflaglist


#qualitycriteria={"mean":(-15,15),"rms":(5,15),"nonGaussianity":(-3,3)}
#CRQualityCheck(datafile,qualitycriteria,maxblocksize=65536,nsigma=5,verbose=True)


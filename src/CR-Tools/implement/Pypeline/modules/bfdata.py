import struct
import numpy as np
import os

def sb2str(sb):
    if sb<10:
       return '00'+str(sb)
    elif sb<100:
       return '0'+str(sb)
    else:
       return str(sb)

def get_stokes_data(file, block, channels, samples, nrstations=1, type="StokesI"):
    """Get a lofar datablock from stokes (I or IQUV) raw data format. 
    Returns a array of data(stations,channels,samples,nrstokes).
    
    *file* opened data file
    *block* which block to read, negative numbers mean the next block
    *channels* nr of channels per subband
    *samples* nr of samples per subband (This is the samples/block divided by integration length)
    *nr stations* Nr of stations in flysEye mode.
    *type* StokesI, I, StokesIQUV, IQUV
    """
    
    # 
    if type is "StokesI" or type is "I":
        nrStokes=1
    elif type is "StokesIQUV" or type is "IQUV":
        nrStokes=4
    else:
        print "Unsupported datatype. Aborting...."
        assert False
 
    # Calculate how large the datablock is.
    n=nrstations*channels*(samples|2)*nrStokes
    
    # Format string for sequence number, padding and data (Big endian)
    fmt='>I508x'+str(n)+'f'
    
    # Size represented by format string in bytes
    sz=struct.calcsize(fmt)
    
    # read data from file
    if block < 0:
        x=file.read(sz)
        # unpack struct into intermediate data
        t=struct.unpack(fmt,x)
    else:
        file.seek(sz*block)
        x=file.read(sz)
        # unpack struct into intermediate data
        t=struct.unpack(fmt,x)
        if t[0] != block:
            file.seek(0)
            x=file.read(sz)
            t=struct.unpack(fmt,x)
            startblock=t[0]
            file.seek((block-startblock)*sz)
            x=file.read(sz)
            t=struct.unpack(fmt,x)
            if t[0] != block:
                print "Discontinuous data is not supported yet in this mode. Use blocknr = -1 and step through the data"
                assert False
                 
    # unpack struct into intermediate data
    #t=struct.unpack(fmt,x)
    
    
    # return sequence number as uint, and data as nparray
    return np.asarray(t[1:]).reshape(nrstations,channels,samples|2,nrStokes)[:,:,0:samples,:]

def get_rawvoltage_data(file, block, channels, samples, nrstations=1, nrpol=2):
    """Get a lofar datablock from raw voltage complex data. 
    Returns a array of data(stations,channels,samples,nrpol,(real,imag)).
    
    *file* opened data file
    *block* which block to read, negative numbers mean the next block
    *channels* nr of channels per subband
    *samples* nr of samples per subband (This is the samples/block divided by integration length)
    *nr stations* Nr of stations in flysEye mode.
    *nrpol* Nr of polarisations
    """
    
    # 
    
    # Calculate how large the datablock is.
    n=nrstations*channels*(samples|2)*2*nrpol
  
    # Format string for sequence number, padding and data (Big endian)
    fmt='>I508x'+str(n)+'f'
    
    # Size represented by format string in bytes
    sz=struct.calcsize(fmt)
    
    # read data from file
    if block < 0:
        x=file.read(sz)
        # unpack struct into intermediate data
        t=struct.unpack(fmt,x)
    else:
        file.seek(sz*block)
        x=file.read(sz)
        # unpack struct into intermediate data
        t=struct.unpack(fmt,x)
        if t[0] != block:
            file.seek(0)
            x=file.read(sz)
            t=struct.unpack(fmt,x)
            startblock=t[0]
            file.seek((block-startblock)*sz)
            x=file.read(sz)
            t=struct.unpack(fmt,x)
            if t[0] != block:
                print "Discontinuous data is not supported yet in this mode. Useblocknr = -1 and step through the data"
                assert False
                 
    # unpack struct into intermediate data
    #t=struct.unpack(fmt,x)
    
    
    # return sequence number as uint, and data as nparray
    return np.asarray(t[1:]).reshape(nrstations,channels,samples|2,nrpol,2)[:,:,0:samples|2,:,:]




def check_data_parameters(file, channels, samples, nrstations=1, type="StokesI"):
    """Checks if the first two blocks have a continuous sequence number.
        
    *file* opened data file
    *channels* nr of channels per subband
    *samples* nr of samples per subband
    *type* StokesI, I, StokesIQUV, IQUV or RawVoltage
    """
    
    # 
    if type is "StokesI" or type is "I":
        nrStokes=1
    elif type is "StokesIQUV" or type is "IQUV" or type is "RawVoltage":
        nrStokes=4
    
    # Calculate how large the datablock is.
    n=nrstations*channels*(samples|2)*nrStokes
    
    # Format string for sequence number, padding and data (Big endian)
    fmt='>I508x'+str(n)+'f'
    
    # Size represented by format string in bytes
    sz=struct.calcsize(fmt)
    
    file.seek(0)
    
    for i in range(5):
        x=file.read(sz)
        t0=struct.unpack(fmt,x)
    
        x=file.read(sz)
        t1=struct.unpack(fmt,x)
        if t1[0]-t0[0] == 1:
            break
    
    
    file.seek(0)
    
     
    return t1[0]-t0[0] == 1

def get_data_size(channels, samples, nrstations=1, type="StokesI"):
    """Get the size in bytes of a lofar data block. 
    
    *channels* nr of channels per subband
    *samples* nr of samples per subband
    *type* StokesI or StokesIQUV
    """
    
    # 
    if type is "StokesI" or type is "I":
        nrStokes=1
    elif type is "StokesIQUV" or type is "IQUV" or type is "RawVoltage":
        nrStokes=4
    
    # Calculate how large the datablock is.
    n=nrstations*channels*(samples|2)*nrStokes
    
    # Format string for sequence number, padding and data (Big endian)
    fmt='>I508x'+str(n)+'f'
    
    # Size represented by format string in bytes
    sz=struct.calcsize(fmt)
    
    return sz

def get_sequence_number(file, block, channels, samples, nrstations=1, type="StokesI"):
    """Get a lofar datablock from stokesI raw data format. 
    Returns a tuple of sequence number and data(channels,samples).
    Note that the internal data format is data(channels,samples|2).
    
    *file* opened data file
    *block* which block to read, negative numbers means just the first block
    *channels* nr of channels per subband
    *samples* nr of samples per subband
    *type* StokesI,I, StokesIQUV, IQUV or RawVoltage
    """
    
    # Calculate the size of one datablock
    seeksize=get_data_size(channels,samples,nrstations,type)
    
    if block >= 0: 
        # Go to the start of one datablock
        file.seek(block*seeksize)
    
    # Format string for sequence number, padding and data (Big endian)
    fmt='>I'
    
    # Size represented by format string in bytes
    sz=struct.calcsize(fmt)
    
    # read data from file
    x=file.read(sz)
    
    # unpack struct into intermediate data
    t=struct.unpack(fmt,x)
   
    if block >= 0: 
        # Go back to the start of one datablock
        file.seek(block*seeksize)
    
    # return sequence number as uint, and data as nparray
    return t[0] 



def get_block(file, channels=64, samples=3056):
    """Get a lofar datablock from stokesI raw data format. 
    Returns a tuple of sequence number and data(channels,samples).
    Note that the internal data format is data(channels,samples|2).
    
    *file* opened data file
    *channels* nr of channels per subband
    *samples* nr of samples per subband
    """
    
    # Calculate how large the datablock is.
    n=channels*(samples|2)
    
    # Format string for sequence number, padding and data (Big endian)
    fmt='>I508x'+str(n)+'f'

    # Size represented by format string in bytes
    sz=struct.calcsize(fmt)

    # read data from file
    x=file.read(sz)

    # unpack struct into intermediate data
    t=struct.unpack(fmt,x)

    # return sequence number as uint, and data as nparray
    return t[0],np.asarray(t[1:]).reshape(channels,samples|2)



def get_block_oldstyle(file, channels=64, samples=3056):
    """Get a lofar datablock from stokesI old raw dataformat. 
    Returns a tuple of sequence number and data(channels,samples).
    Note that the internal data format is data(channels,samples|2)."""
    # Calculate how large the datablock is.
    n=channels*(samples|2)
    
    # Format string for sequence number (Big endian)
    fmt='>I'

    # Size represented by format string in bytes
    sz=struct.calcsize(fmt)

    # read data from file
    x=file.read(sz)

    # unpack struct into sequence number
    t0=struct.unpack(fmt,x)[0]

    # Format string of data block (Little endian)
    fmt='<'+str(n)+'f'

    # Size represented by data block in bytes
    sz=struct.calcsize(fmt)

    # read data from file
    x=file.read(sz)

    # unpack struct into data container
    t1=struct.unpack(fmt,x)

    # return sequence number, data
    return t0,np.asarray(t1).reshape(channels,samples|2)

    
def get_block_rawvoltagedata(file, stations, channels=64, samples=3056, npol=2):
    """Get a lofar datablock from stokesI raw data format. 
    Returns a tuple of sequence number and data(channels,samples).
    Note that the internal data format is data(channels,samples|2)."""
    
    # Calculate how large the datablock is.
    n=stations*channels*(samples|2)*npol*2
    
    # Format string for sequence number, padding and data (Big endian)
    fmt='>I508x'+str(n)+'f'

    # Size represented by format string in bytes
    sz=struct.calcsize(fmt)

    # read data from file
    x=file.read(sz)
    

    # unpack struct into intermediate data
    t=struct.unpack(fmt,x)

    # return sequence number as uint, and data as nparray
    return t[0],np.asarray(t[1:]).reshape(stations,channels,samples|2,npol,2)

    
def get_block_rawvoltagedata_old(file, stations, channels=64, samples=3056, npol=2):
    """Get a lofar datablock from stokesI raw data format. 
    Returns a tuple of sequence number and data(channels,samples).
    Note that the internal data format is data(channels,samples|2)."""
    
    # Calculate how large the datablock is.
    n=stations*channels*(samples|2)*npol*2
    
    # Format string for sequence number (Big endian)
    fmt='>I'

    # Size represented by format string in bytes
    sz=struct.calcsize(fmt)

    # read data from file
    x=file.read(sz)

    # unpack struct into sequence number
    t0=struct.unpack(fmt,x)[0]

    # Format string of data block (Little endian)
    fmt='<'+str(n)+'f'

    # Size represented by data block in bytes
    sz=struct.calcsize(fmt)

    # read data from file
    x=file.read(sz)

    # unpack struct into data container
    t1=struct.unpack(fmt,x)

    # return sequence number, data
    return t0,np.asarray(t1).reshape(stations,channels,samples|2,npol,2)

def read_header_raw_station_data(file):
    """Get the header of a raw station data file.
    Returns a tuple of (bitsPerSample,nrPolarizations,nrSubbands,
    nrSamplesPerSubband,station,sampleRate,subbandFrequencies,beamDirections,
    subbandToBeamMapping)"""

    file.seek(0)
    maxNrSubbands=62
    # Check endiannes. This is done by a magic number at the beginning of the file
    endiannes='>'
    fmt=endiannes+'I'
    encoded_data=file.read(struct.calcsize(fmt))
    magic_nr=struct.unpack(fmt,encoded_data)[0]
    
    # Check if this is the right endiannes
    if magic_nr != 0x3F8304EC:
        # If not change the endiannes.
        endiannes='<'
        fmt=endiannes+'I'
        magic_nr=struct.unpack(fmt,encoded_data)
        
    # Check if the magic nr is now correct. If not, quit the operation.
    assert magic_nr == 0x3F8304EC


    # Now we can do the same reading and unpacking for the other data formats

    # The number of bits per sample
    fmt=endiannes+'B'
    bitsPerSample = struct.unpack(fmt,file.read(struct.calcsize(fmt)))[0]

    # The number of polarizations
    fmt=endiannes+'B'
    nrPolarizations= struct.unpack(fmt,file.read(struct.calcsize(fmt)))[0]

    # The number of subbands
    fmt=endiannes+'H'
    nrSubbands= struct.unpack(fmt,file.read(struct.calcsize(fmt)))[0]

    # 155648 (160MHz) or 196608 (200MHz)
    fmt=endiannes+'I'
    nrSamplesPerSubband = struct.unpack(fmt,file.read(struct.calcsize(fmt)))[0]

    # Name of the station
    fmt=endiannes+'20s'
    station = struct.unpack(fmt,file.read(struct.calcsize(fmt)))[0].strip('\x00')

    # The sample rate: 156260.0 or 195312.5 .. double (number of samples per second for each subband
    fmt=endiannes+'d'
    sampleRate = struct.unpack(fmt,file.read(struct.calcsize(fmt)))[0]

    # The frequencies within a subband
    fmt=endiannes+str(maxNrSubbands)+'d'
    subbandFrequencies = struct.unpack(fmt,file.read(struct.calcsize(fmt)))

    # The beam pointing directions
    fmt=endiannes+'16d'
    beamDirections = struct.unpack(fmt,file.read(struct.calcsize(fmt)))

    # mapping from subbands to beams
    fmt=endiannes+str(maxNrSubbands)+'h'
    subbandToBeamMapping = struct.unpack(fmt,file.read(struct.calcsize(fmt)))
    
    # Padding
    fmt=endiannes+'I'
    padding = struct.unpack(fmt,file.read(struct.calcsize(fmt)))

    # Return the parameters
    return (bitsPerSample,nrPolarizations,nrSubbands,nrSamplesPerSubband,station,sampleRate,subbandFrequencies[0:nrSubbands],beamDirections,subbandToBeamMapping[0:nrSubbands])


def get_blockheader_raw_station_data(file,nrSamplesPerSubband,nrSubbands):
    """Get the header of a raw station data block
    returns (coarseDelayApplied,fineDelayRemainingAtBegin, fineDelayRemainingAfterEnd, time, flags)"""
    
    

    # Check endiannes. This is done by a magic number at the beginning of the file
    endiannes='>'
    fmt=endiannes+'I'
    encoded_data=file.read(struct.calcsize(fmt))
    magic_nr=struct.unpack(fmt,encoded_data)[0]
    
    # Check if this is the right endiannes
    if magic_nr != 0x2913D852:
        # If not change the endiannes.
        endiannes='<'
        fmt=endiannes+'I'
        magic_nr=struct.unpack(fmt,encoded_data)
        
    # Check if the magic nr is now correct. If not, quit the operation.
    assert magic_nr == 0x2913D852
    
    
    # 
    fmt=endiannes+'8i'
    coarseDelayApplied= struct.unpack(fmt,file.read(struct.calcsize(fmt)))
    
    # for alignment
    fmt=endiannes+'4B'
    padding = struct.unpack(fmt,file.read(struct.calcsize(fmt)))
    
    #
    fmt=endiannes+'8d'
    fineDelayRemainingAtBegin = struct.unpack(fmt,file.read(struct.calcsize(fmt)))
    
    #
    fmt=endiannes+'8d'
    fineDelayRemainingAfterEnd = struct.unpack(fmt,file.read(struct.calcsize(fmt)))
    
    #
    fmt=endiannes+'8q'
    time= struct.unpack(fmt,file.read(struct.calcsize(fmt)))
    
    # This has the structure: ( nrFlagsRanges, (range begin,range end)[16] )  [8]  
    fmt=endiannes+'264I'
    flags = struct.unpack(fmt,file.read(struct.calcsize(fmt)))
   

    
    # Read data
    if endiannes is '>':
        endiannes='<'
    else:
        endiannes='>'
    fmt=endiannes+str(nrSamplesPerSubband*nrSubbands*4)+'h' 
    data= struct.unpack(fmt,file.read(struct.calcsize(fmt)))
    data=np.array(data)
    data.resize(nrSubbands,nrSamplesPerSubband,2,2)

    # Return the values
    return (coarseDelayApplied,fineDelayRemainingAtBegin, fineDelayRemainingAfterEnd, time, flags,data)



def get_parameters(obsid, useFilename=False):
    """Get the most important observation parameters. Returns a dictionary with these parameters.
    *obsid*        Observation id, f.e. L2010_08834 or D2009_16234. Only works on lofar cluster.
    *useFilename*  If set to true, obsid is filename of the parset.
    """
    if useFilename:
        parsetfilename=obsid
    else:
        # What is the observation number? This determines the filename
        obsnr=obsid.split('_')[1]
        
        # Name of the parset file
        parsetfilename='/globalhome/lofarsystem/log/'+obsid+'/RTCP-'+obsnr+'.parset'
        if not os.path.isfile(parsetfilename):
            parsetfilename='/globalhome/lofarsystem/log/'+obsid+'/RTCP.parset.0'   

 
    # Open the file
    parsetfile=open(parsetfilename,'r')
    parsetfile.seek(0)
    # Read in the parameters from the parset file
    allparameters={}
    for line in parsetfile:
        str2 = line.split(None,2)
        if len(str2)>=3:
            allparameters[str2[0]] = str2[2].strip('\n')
        else:
            allparameters[str2[0]] = "UNDEFINED"

    # Link the important parameters
    parameters={}
    parameters["channels"]=int(allparameters["Observation.channelsPerSubband"])
    parameters["samples"]=int(allparameters["OLAP.CNProc.integrationSteps"])
    parameters["namemask"]=allparameters["Observation.MSNameMask"]
    parameters["stationnames"]=allparameters["OLAP.storageStationNames"]
    parameters["storagenodes"]=allparameters["Observation.VirtualInstrument.storageNodeList"].strip('[]').split(',')
    parameters["storagenodes"].sort()
    SBspernode=allparameters["OLAP.storageNodeList"].strip('[]').split(',')
    for i in range(len(SBspernode)):
        SBspernode[i]=int(SBspernode[i].split('*')[0])
    parameters["SBspernode"]=SBspernode
    parameters["timeintegration"]=int(allparameters["OLAP.Stokes.integrationSteps"])
    parameters["subbands"]=allparameters["Observation.subbandList"]
    #parameters["subbandsperMS"]=allparameters["OLAP.StorageProc.subbandsPerMS"]
    #parameters["antennaset"]=allparameters["Observation.antennaSet"]
    parameters["filterselection"]=allparameters["Observation.bandFilter"]
    parameters["clockfrequency"]=allparameters["Observation.sampleClock"]
    obsid2=allparameters["Observation.ObsID"]
    while len(obsid2) < 5:
        obsid2 = '0' + obsid2
    parameters["obsid"]=obsid2
    parameters["starttime"]=allparameters["Observation.startTime"]
    parameters["stoptime"]=allparameters["Observation.stopTime"]
    parameters["coherentstokes"]=allparameters["OLAP.outputCoherentStokes"]=='true'
    parameters["incoherentstokes"]=allparameters["OLAP.outputIncoherentStokes"]=='true'
    parameters["beamformeddata"]=allparameters["OLAP.outputBeamFormedData"]=='true'
    parameters["correlateddata"]=allparameters["OLAP.outputCorrelatedData"]=='true'
    parameters["filtereddata"]=allparameters["OLAP.outputFilteredData"]=='true'
    parameters["flyseyes"]=allparameters["OLAP.PencilInfo.flysEye"]=='true' 
    parameters["stokestype"]=allparameters["OLAP.Stokes.which"]
    if "Observation.Beam[0].target" in allparameters.keys():
        parameters["target"]=allparameters["Observation.Beam[0].target"]
    # Get file names
    year=parameters['starttime'].split('-')[0].replace('\'','')

    subcluster={}
    subcluster["lse001"]="sub1"
    subcluster["lse002"]="sub1"
    subcluster["lse003"]="sub1"
    subcluster["lse004"]="sub2"
    subcluster["lse005"]="sub2"
    subcluster["lse006"]="sub2"
    subcluster["lse007"]="sub3"
    subcluster["lse008"]="sub3"
    subcluster["lse009"]="sub3"
    subcluster["lse010"]="sub4"
    subcluster["lse011"]="sub4"
    subcluster["lse012"]="sub4"
    subcluster["lse013"]="sub5"
    subcluster["lse014"]="sub5"
    subcluster["lse015"]="sub5"
    subcluster["lse016"]="sub6"
    subcluster["lse017"]="sub6"
    subcluster["lse018"]="sub6"
    subcluster["lse019"]="sub7"
    subcluster["lse020"]="sub7"
    subcluster["lse021"]="sub7"
    subcluster["lse022"]="sub8"
    subcluster["lse023"]="sub8"
    subcluster["lse024"]="sub8"
   
    names=[]
    datatype=[] 
    if parameters["coherentstokes"]:
        sb=0
        for i in range(len(parameters["storagenodes"])):
            node=parameters["storagenodes"][i]
            nrpernode=parameters["SBspernode"][i]
            mask=parameters["namemask"]
            for j in range(int(nrpernode)):
                name='/net/'+subcluster[node]+'/'+node
                name=name+mask.replace('${YEAR}',year).replace('${MSNUMBER}',parameters['obsid']).replace('${SUBBAND}',str(sb))
                name=name+'.stokes'
                names.append(name)
                sb+=1
        
    if parameters["incoherentstokes"]:
        sb=0
        for i in range(len(parameters["storagenodes"])):
            node=parameters["storagenodes"][i]
            nrpernode=parameters["SBspernode"][i]
            mask=parameters["namemask"]
            for j in range(int(nrpernode)):
                name='/net/'+subcluster[node]+'/'+node
                name=name+mask.replace('${YEAR}',year).replace('${MSNUMBER}',parameters['obsid']).replace('${SUBBAND}',str(sb))
                name=name+'.incoherentstokes'
                names.append(name)
                sb+=1

    if parameters["beamformeddata"]:
        sb=0
        for i in range(len(parameters["storagenodes"])):
            node=parameters["storagenodes"][i]
            nrpernode=parameters["SBspernode"][i]
            mask=parameters["namemask"]
            for j in range(int(nrpernode)):
                name='/net/'+subcluster[node]+'/'+node
                name=name+mask.replace('${YEAR}',year).replace('${MSNUMBER}',parameters['obsid']).replace('${SUBBAND}',str(sb))
                name=name+'.beams'
                names.append(name)
                sb+=1


    if parameters["correlateddata"]:
        sb=0
        for i in range(len(parameters["storagenodes"])):
            node=parameters["storagenodes"][i]
            nrpernode=parameters["SBspernode"][i]
            mask=parameters["namemask"]
            for j in range(int(nrpernode)):
                name='/net/'+subcluster[node]+'/'+node
                name=name+mask.replace('${YEAR}',year).replace('${MSNUMBER}',parameters['obsid']).replace('${SUBBAND}',str(sb))
                name=name+'.MS'
                names.append(name)
                sb+=1


    if parameters["filtereddata"]:
        sb=0
        for i in range(len(parameters["storagenodes"])):
            node=parameters["storagenodes"][i]
            nrpernode=parameters["SBspernode"][i]
            mask=parameters["namemask"]
            for j in range(int(nrpernode)):
                name='/net/'+subcluster[node]+'/'+node
                name=name+mask.replace('${YEAR}',year).replace('${MSNUMBER}',parameters['obsid']).replace('${SUBBAND}',str(sb))
                name=name+'.unknown'
                names.append(name)
                sb+=1

    parameters["files"]=names

    return parameters


def get_parameters_new(obsid, useFilename=False):
    """Get the most important observation parameters. Returns a dictionary with these parameters.
    *obsid*        Observation id, f.e. L2010_08834 or D2009_16234. Only works on lofar cluster.
    *useFilename*  If set to true, obsid is filename of the parset.
    """
    if useFilename:
        parsetfilename=obsid
    else:
        # What is the observation number? This determines the filename
        if '_' in obsid:
           obsnr=obsid.split('_')[1]
        else:
           obsnr=obsid.strip('L')
        
        # Name of the parset file
        parsetfilename='/globalhome/lofarsystem/production/lofar/bgfen/log/L'+obsnr+'/L'+obsnr+'.parset'
        if not os.path.isfile(parsetfilename):
            parsetfilename='/globalhome/lofarsystem/log/'+obsid+'/RTCP.parset.0'   

 
    # Open the file
    parsetfile=open(parsetfilename,'r')
    parsetfile.seek(0)
    # Read in the parameters from the parset file
    allparameters={}
    for line in parsetfile:
        str2 = line.split(None,2)
        if len(str2)>=3:
            allparameters[str2[0]] = str2[2].strip('\n')
        else:
            allparameters[str2[0]] = "UNDEFINED"

    # Link the important parameters
    parameters={}
    parameters["channels"]=int(allparameters["Observation.channelsPerSubband"])
    parameters["samples"]=int(allparameters["OLAP.CNProc.integrationSteps"])
    parameters["namemask"]=allparameters["Observation.MSNameMask"]
    parameters["stationnames"]=allparameters["OLAP.storageStationNames"]
    parameters["storagenodes"]=allparameters["Observation.VirtualInstrument.storageNodeList"].strip('[]').split(',')
    parameters["storagenodes"].sort()
    SBspernode=allparameters["OLAP.storageNodeList"].strip('[]').split(',')
    for i in range(len(SBspernode)):
        SBspernode[i]=int(SBspernode[i].split('*')[0])
    parameters["SBspernode"]=SBspernode
    parameters["timeintegration"]=int(allparameters["OLAP.Stokes.integrationSteps"])
    parameters["subbands"]=allparameters["Observation.subbandList"]
    #parameters["subbandsperMS"]=allparameters["OLAP.StorageProc.subbandsPerMS"]
    #parameters["antennaset"]=allparameters["Observation.antennaSet"]
    parameters["filterselection"]=allparameters["Observation.bandFilter"]
    parameters["clockfrequency"]=allparameters["Observation.sampleClock"]
    obsid2=allparameters["Observation.ObsID"]
    while len(obsid2) < 5:
        obsid2 = '0' + obsid2
    parameters["obsid"]=obsid2
    parameters["starttime"]=allparameters["Observation.startTime"]
    parameters["stoptime"]=allparameters["Observation.stopTime"]
    parameters["coherentstokes"]=allparameters["OLAP.outputCoherentStokes"]=='true'
    parameters["incoherentstokes"]=allparameters["OLAP.outputIncoherentStokes"]=='true'
    parameters["beamformeddata"]=allparameters["OLAP.outputBeamFormedData"]=='true'
    parameters["correlateddata"]=allparameters["OLAP.outputCorrelatedData"]=='true'
    parameters["filtereddata"]=allparameters["OLAP.outputFilteredData"]=='true'
    parameters["flyseyes"]=allparameters["OLAP.PencilInfo.flysEye"]=='true' 
    parameters["stokestype"]=allparameters["OLAP.Stokes.which"]
    if "Observation.Beam[0].target" in allparameters.keys():
        parameters["target"]=allparameters["Observation.Beam[0].target"]
    # Get file names
    year=parameters['starttime'].split('-')[0].replace('\'','')

    subcluster={}
    subcluster["lse001"]="sub1"
    subcluster["lse002"]="sub1"
    subcluster["lse003"]="sub1"
    subcluster["lse004"]="sub2"
    subcluster["lse005"]="sub2"
    subcluster["lse006"]="sub2"
    subcluster["lse007"]="sub3"
    subcluster["lse008"]="sub3"
    subcluster["lse009"]="sub3"
    subcluster["lse010"]="sub4"
    subcluster["lse011"]="sub4"
    subcluster["lse012"]="sub4"
    subcluster["lse013"]="sub5"
    subcluster["lse014"]="sub5"
    subcluster["lse015"]="sub5"
    subcluster["lse016"]="sub6"
    subcluster["lse017"]="sub6"
    subcluster["lse018"]="sub6"
    subcluster["lse019"]="sub7"
    subcluster["lse020"]="sub7"
    subcluster["lse021"]="sub7"
    subcluster["lse022"]="sub8"
    subcluster["lse023"]="sub8"
    subcluster["lse024"]="sub8"
   
    names=[]
    datatype=[] 
    if parameters["coherentstokes"]:
        sb=0
        for i in range(min(len(parameters["storagenodes"]),len(parameters["SBspernode"]))):
            node=parameters["storagenodes"][i]
            nrpernode=parameters["SBspernode"][i]
            mask=parameters["namemask"]
            for j in range(int(nrpernode)):
                name='/net/'+subcluster[node]+'/'+node
                name=name+mask.replace('${YEAR}',year).replace('${MSNUMBER}',parameters['obsid']).replace('${SUBBAND}',sb2str(sb))
                name=name+'.stokes'
                names.append(name)
                sb+=1
        
    if parameters["incoherentstokes"]:
        sb=0
        for i in range(len(parameters["storagenodes"])):
            node=parameters["storagenodes"][i]
            nrpernode=parameters["SBspernode"][i]
            mask=parameters["namemask"].strip('.MS')
            for j in range(int(nrpernode)):
                name='/net/'+subcluster[node]+'/'+node
                name=name+mask.replace('${YEAR}_${MSNUMBER}',year+'_'+parameters['obsid']).replace('/SB${SUBBAND}','/L'+parameters['obsid']+'_SB'+sb2str(sb))
                name=name+'_bf.incoherentstokes'
                names.append(name)
                sb+=1

    if parameters["beamformeddata"]:
        sb=0
        for i in range(min(len(parameters["storagenodes"]),len(parameters["SBspernode"]))):
            node=parameters["storagenodes"][i]
            nrpernode=parameters["SBspernode"][i]
            mask=parameters["namemask"]
            for j in range(int(nrpernode)):
                name='/net/'+subcluster[node]+'/'+node
                name=name+mask.replace('${YEAR}',year).replace('${MSNUMBER}',parameters['obsid']).replace('${SUBBAND}',sb2str(sb))
                name=name+'.beams'
                names.append(name)
                sb+=1


    if parameters["correlateddata"]:
        sb=0
        for i in range(len(parameters["storagenodes"])):
            node=parameters["storagenodes"][i]
            nrpernode=parameters["SBspernode"][i]
            mask=parameters["namemask"]
            for j in range(int(nrpernode)):
                name='/net/'+subcluster[node]+'/'+node
                name=name+mask.replace('${YEAR}',year).replace('${MSNUMBER}',parameters['obsid']).replace('${SUBBAND}',sb2str(sb))
                name=name+'.MS'
                names.append(name)
                sb+=1


    if parameters["filtereddata"]:
        sb=0
        for i in range(len(parameters["storagenodes"])):
            node=parameters["storagenodes"][i]
            nrpernode=parameters["SBspernode"][i]
            mask=parameters["namemask"]
            for j in range(int(nrpernode)):
                name='/net/'+subcluster[node]+'/'+node
                name=name+mask.replace('${YEAR}',year).replace('${MSNUMBER}',parameters['obsid']).replace('${SUBBAND}',sb2str(sb))
                name=name+'.unknown'
                names.append(name)
                sb+=1

    parameters["files"]=names

    return parameters


def get_all_parameters_new(obsid, useFilename=False):
    """Get the most important observation parameters. Returns a dictionary with these parameters.
    *obsid*        Observation id, f.e. L2010_08834 or D2009_16234. Only works on lofar cluster.
    *useFilename*  If set to true, obsid is filename of the parset.
    """
    if useFilename:
        parsetfilename=obsid
    else:
        # What is the observation number? This determines the filename
        obsnr=obsid.split('_')[1]
        
        # Name of the parset file
        parsetfilename='/globalhome/lofarsystem/production/lofar/bgfen/log/L'+obsnr+'/L'+obsnr+'.parset'
        if not os.path.isfile(parsetfilename):
            parsetfilename='/globalhome/lofarsystem/log/'+obsid+'/RTCP.parset.0'   

 
    # Open the file
    parsetfile=open(parsetfilename,'r')
    parsetfile.seek(0)
    # Read in the parameters from the parset file
    allparameters={}
    for line in parsetfile:
        str2 = line.split(None,2)
        if len(str2)>=3:
            allparameters[str2[0]] = str2[2].strip('\n')
        else:
            allparameters[str2[0]] = "UNDEFINED"
    return allparameters

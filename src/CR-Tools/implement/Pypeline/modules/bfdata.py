import struct
import numpy as np

def get_block(file, channels=64, samples=3056):
    """Get a lofar datablock from stokesI raw data format. 
    Returns a tuple of sequence number and data(channels,samples).
    Note that the internal data format is data(channels,samples|2)."""
    
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
    fmt=endiannes+str(nrSamplesPerSubband*nrSubbands*4)+'h' 
    data= struct.unpack(fmt,file.read(struct.calcsize(fmt)))
    data=np.array(data)
    data.resize(nrSubbands,nrSamplesPerSubband,2,2)

    # Return the values
    return (coarseDelayApplied,fineDelayRemainingAtBegin, fineDelayRemainingAfterEnd, time, flags,data)


    #  sizeof(blockheader)
    #  +
    #  ( header.nrSamplesPerSubband * header.nrSubbands * sizeof(Sample) );
    #nrOfBlocks = (file_byte_size - sizeof(header)) / oneSecondBlockSize;
    #std::cout << "Number of one second blocks in file: " << nrOfBlocks << std::endl;

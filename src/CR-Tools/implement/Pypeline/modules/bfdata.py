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

from pycrtools import *
import os
from math import *
LOFARSOFT=os.environ["LOFARSOFT"]
#filename=LOFARSOFT+"/data/lopes/example.event"
filename=LOFARSOFT+"/data/lofar/rw_20080701_162002_0109.h5"
file=crfile(filename)
#obsdate   =file.get("Date"); print "obsdate=",obsdate
#filesize  =file.get("Filesize"); print "filesize=",filesize
nAntennas =file.get("nofAntennas"); print "nAntennas=",nAntennas
blocksize =file.get("blocksize"); print "blocksize=",blocksize
#antennas  =file.get("antennas"); print "antennas=",antennas
#antennaIDs=file.get("AntennaIDs"); print "antennaIDs=",antennaIDs
#selectedAntennas=file.get("selectedAntennas"); print "selectedAntennas=",selectedAntennas
fftlength =file.get("fftLength"); print "fftlength=",fftlength
sampleFrequency =file.get("sampleFrequency"); print "sampleFrequency=",sampleFrequency
blocksize=1024
file.set("Blocksize",blocksize)
timedata=FloatVec()
timedata.resize(blocksize)
#file.read("Time",timedata)

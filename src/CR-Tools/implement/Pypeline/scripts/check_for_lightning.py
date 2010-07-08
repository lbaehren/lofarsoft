#! /usr/bin/env python 

from pycrtools import *
import os
import numpy as np
import matplotlib.pyplot as plt
import pyfits
import sys

# (start) configuration
# Data information
# Lightning data can be obtained from (it is 3 GB per file)
# make sure you have a LOFAR_DATA_DIR variable of the directory you want to store the data
# mkdir $LOFAR_DATA_DIR
# scp <username>@helios /mnt/lofar/lightning/lightning* $LOFAR_DATA_DIR/lightning/
# This program is used to identify lightning in a single dipole to determine if all data should be dumped.
# Use as ipython -p pycr -i check_for_lightning.py <filename>

tstart=time.time()
trackHistory=False
# Get environment variables
LOFARSOFT = os.environ["LOFARSOFT"].rstrip('/')+'/'
LOFAR_DATA_DIR = os.environ['LOFAR_DATA_DIR'].rstrip('/')+'/'
data_directory = LOFAR_DATA_DIR+"/lightning/" 
stationname="CS302"
#filenames=["lightning_16_48.h5"]
filenames=["lightning_17_23.h5"]
nrfiles=len(filenames)
antennaset="LBA_INNER"
blocksize=2**16
nblocks=4074
startblock=1 
# (end) configuration

# (start) Imaging

# Initialisation
# read in the files
print "time: ",time.time()
if len(sys.argv) == 6:
    file=crfile(sys.argv[5])
else:
    file=crfile(data_directory+filenames[0])

file.set("SelectedAntennas",[0])

file["blocksize"]=blocksize

time_axis=hArray(float,nblocks)
time_axis.fillrange(startblock*blocksize/1024,blocksize/1024)
time_axis.setUnit("","pages")
file_efield=file["emptyVoltage"]
file_average=hArray(float,nblocks,xvalues=time_axis)


t0=time.clock()
for block in range(nblocks):
    file["block"]=block+startblock
    file_average[block]=file_efield.read(file,"Voltage").stddev()
    #file_average[block]=file_efield.read(file,"Voltage").sumabs()

print "t=",time.clock()-t0,"s -","Average calculated"

file_average.plot()
print "File: ",file.filename
print "Stddev per block of ",blocksize
print "Minimum: ",file_average.min().val()
print "Maximum: ",file_average.max().val()
print "Average: ",file_average.mean().val()


print "t=",time.time()-tstart,"s -","Total run time"
print "time: ",time.time()


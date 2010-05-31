#! /usr/bin/env python 

from pycrtools import *
import os
import numpy as np
import matplotlib.pyplot as plt
import pyfits

# (start) configuration
# Data information
# Data should show the Crab Nebula and a giant pulse in limited unknown bandwidth
# The position of the Crab Nebula should be Az,El 206.229,57.0392

# Get environment variables
LOFARSOFT = os.environ["LOFARSOFT"].rstrip('/')+'/'
LOFAR_DATA_DIR = os.environ['LOFAR_DATA_DIR'].rstrip('/')+'/'
data_directory = LOFAR_DATA_DIR+"Crab/pulse3/"
stationname="CS302"
filenames=["CS302C-B0T6:01:48.h5","CS302C-B1T6:01:48.h5","CS302C-B2T6:01:48.h5","CS302C-B3T6:01:48.h5","CS302C-B4T6:01:48.h5","CS302C-B5T6:01:48.h5"]
TBBboards=[0,1,2,3,4,5]
antennaset="HBA"
antennafilename=LOFARSOFT+"data/calibration/AntennaArrays/"+stationname+"-AntennaArrays.conf"
blocksize=2**16
nblocks=10
caltablefilename=LOFARSOFT+"src/CR-Tools/data/calibrationCS302.dat"

# (end) configuration


# (start) get METAdata

# Get antenna positions
# Task: Read antenna positions from a config file for the specified station and antennaset
# Returns a 96*3 hArray antenna_positions
# To select them for each board use: (antenna_positions[TBBboards[TBBboard]*16:TBBboards[TBBboard]*16+nofSelectedAntennas-1]
# where the current TBBboard and nofSelectedAntennas (=file["nofSelectedAntennas"]) still has to be specified
# In the future the antenna positions should be selected from the current datafile

antfile = open(antennafilename,'r') 
antfile.seek(0)
str = ''
while antennaset not in str:
    str = antfile.readline()

str = antfile.readline()
str = antfile.readline()
nrantennas = str.split()[0]
nrdir = str.split()[4]
ant = []
for i in range(0,int(nrantennas)):
    str=antfile.readline()
    ant.extend([float(str.split()[0]),float(str.split()[1]),float(str.split()[2])])  #X polarization
    ant.extend([float(str.split()[0]),float(str.split()[1]),float(str.split()[2])])  #Y polarization


antenna_positions=hArray(ant,[int(nrantennas),int(nrdir)])

# Get gain calibration coefficients

# These where obtained by correlation on the source for a dataset from an earlier time
# De gegeven getallen zijn p0 en p1, waarbij fase =  p0+p1*freq. Om p1 om te zetten in delays moet je het nog delen door (2*pi)
# Gebruikswijze in oude code
# double phase=p0[rcu_ids[i](antenna)/2]+frequencies(ifreq)*p1[rcu_ids[i](antenna)/2];
# matCal(ifreq,antenna)=DComplex(cos(-calibrationfactor*phase),sin(-calibrationfactor*phase));
# dr[i]->setFFT2calFFT(matCal);

caltablefile=open(caltablefilename,'r')
caltablefile.seek(0)
str=''
caltable2 = []
for i in range(48):
    str=caltablefile.readline()
    caltable2.extend([str.split()[0],str.split()[1]])

caltable=hArray(caltable2,[48,2])

# (end) get METAdata


# (start) Imaging

# Initialisation
# read in the files

files=[crfile(data_directory+filenames[0])]
for i in range(1,6):
    files.append(crfile(data_directory+filenames[i]))

for i in range(0,6):
    files[i]["blocksize"]=blocksize

# At the moment we don't know how much data there is for each file.
e_field=hArray(float,dimensions=[96,blocksize])
e_fieldtemp=hArray(float,dimensions=[16,blocksize])

for i in range(0,6):
     e_fieldtemp.read(files[i],"Fx").none()
     e_field[i*16:i*16+16].copy(e_fieldtemp)


# RFI mitigation and baseline calculation

# Beamforming section

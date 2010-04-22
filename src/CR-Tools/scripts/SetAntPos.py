#!/sw/bin/env python2.6
#
#  SetAntPos.py
#  
#
#  Created by Sander ter Veen on 4/19/10.
#  Copyright (c) 2010 Radboud University Nijmegen. All rights reserved.
#
import h5py
import sys
datafilename='/mnt/lofar2/terveen/data/2010-04-15-coinc-test-4_copy.h5'
parsetfilename = '/mnt/lofar2/terveen/data/TBBdefault.parset.0'
antennafilename='/Users/STV/usg/data/calibration/AntennaArrays/RS205-AntennaArrays.conf'
interactive=False
#print "Number of commandline arguments: ", len(sys.argv)
#print sys.argv
if len(sys.argv) > 1:
 if "help" in sys.argv[1]:
  print "This file sets metadata to TBB files. Usage:"
  print "python ",sys.argv[0], "<Set non-found arguments manually True/False> <datafile> <parsetfile> <antennafile>"
  sys.exit()
 else:
  interactive=bool('rue' in sys.argv[1])
 print "User interaction: ", interactive
 if len(sys.argv) > 2:
  datafilename=sys.argv[2]
  print "Datafilename: ", datafilename
 if len(sys.argv) > 3:
  parsetfilename=sys.argv[3]
  print "Parsetfile: ", parsetfilename
 if len(sys.argv) > 4:
  antennafilename=sys.argv[4]
  print "antennafile: ", antennafilename
#=============================================================================
#get observation parameters
parsetfile = open(parsetfilename,'r')
parsetfile.seek(0)
parameters={}
for line in parsetfile:
 str = line.split(None,2)
 if len(str)>=3:
  parameters[str[0]] = str[2]
 else:
  parameters[str[0]] = "UNDEFINED"
#=============================================================================
#set link between ATTRIBUTE name and observation parameter name
common_attr_to_par = {}  #attributes to parameter, conversion between HDF5 and parset files. 
#common_attr_to_par["PROJECT_ID"]=
common_attr_to_par["PROJECT_TITLE"]="Observation.Campaign.title"
common_attr_to_par["PROJECT_PI"]="Observation.Campaign.PI"
common_attr_to_par["PROJECT_CO_I"]="Observation.Campaign.CO_I"
common_attr_to_par["PROJECT_CONTACT"]="Observation.Campaign.contact"
common_attr_to_par["OBSERVER"]="Observation.ObserverName"
common_attr_to_par["OBSERVATION_ID"]="Observation.ObsID"
common_attr_to_par["OBSERVATION_DATE_START"]="Observation.startTime"
common_attr_to_par["OBSERVATION_DATE_END"]="Observation.stopTime"
common_attr_to_par["ANTENNA_SET"]="Observation.antennaSet"
common_attr_to_par["FILTER_SELECTION"]="Observation.bandFilter"
common_attr_to_par["CLOCK_FREQUENCY"]="Observation.sampleClock"
# common_attr_to_par["CLOCK_FREQUENCY_UNIT"]=   #defaults to MHz
#common_attr_to_par["TARGET"]=
#common_attr_to_par["SYSTEM_VERSION"]=
#common_attr_to_par["PIPELINE_NAME"]=
#common_attr_to_par["PIPELINE_VERSION"]=
#common_attr_to_par["NOF_STATIONS"]=
common_attr_to_par["STATIONS_LIST"]="Observation.VirtualInstrument.stationList"
#common_attr_to_par["NOTES"]=
#=============================================================================
#Make shorter names
c=common_attr_to_par;
p=parameters;
#=============================================================================
#open hdf5 file and set common attributes
f = h5py.File(datafilename,'a')
print "Attributes that could not be set:"
for i in f.attrs.keys():
 if i not in c.keys():
  print i, " ", f.attrs.__getitem__(i)
  #print 'Do you want to manually alter this value (y/n): '
  if interactive:
   yesno= raw_input('Do you want to manually alter this value (y/n): ')
   if 'y' in yesno:
    #print 'What value do you want to give this attribute?'
    value = raw_input('What value do you want to give this attribute?')
    f.attrs.__setitem__(i,value)

print "Attributes found:"
for i in f.attrs.keys():
 if i in c.keys():
  print i, "\t", p[c[i]]
  if interactive:
   if "UNDEFINED" in p[c[i]]:
    yesno= raw_input('Do you want to manually alter this value (y/n): ')
    if 'y' in yesno:
     #print 'What value do you want to give this attribute?'
     value = raw_input('What value do you want to give this attribute?')
     f.attrs.__setitem__(i,value)  
   else:
    f.attrs.__setitem__(i, p[c[i]])
  else:
   f.attrs.__setitem__(i, p[c[i]])
#=============================================================================
#Calculate nyquist zone
filter=f.attrs.__getitem__("FILTER_SELECTION")
nyquist_zone=1
if "LBA" in filter:
 nyquist_zone=1
elif "110_190" in filter:
 nyquist_zone=2
elif "170_230" in filter:
 nyquist_zone=3
elif "210_250" in filter:
 nyquist_zone=3
print "nyquist zone is: ", nyquist_zone
#=============================================================================
#set Antenna positions and nyquistzone
print "setting antenna positions."
antennaset=f.attrs.__getitem__('ANTENNA_SET')
if "HBA_ONE" in antennaset:
 antennaset="HBA_0"
elif "HBA_TWO" in antennaset:
 antennaset="HBA_1"
elif "HBA_BOTH" in antennaset:
 antennaset="HBA"
if "UNDEFINED" in antennaset:
 print "antennaset undefined, no position data will be added"
 #antennaset=f.attrs.__setitem__('ANTENNA_SET','LBA_OUTER')
else:
 for s in f.keys():
  grp=h5py.Group(f,s,False)
  file = open(antennafilename,'r') 
  file.seek(0)
  str = ''
  while antennaset not in str:
   str = file.readline()
  
  str = file.readline()
  grp.attrs.__setitem__('STATION_POSITION_VALUE',[float(str.split()[2]),float(str.split()[3]),float(str.split()[4])])
  grp.attrs.__setitem__('STATION_POSITION_UNIT',['deg','deg','m'])
  grp.attrs.__setitem__('STATION_POSITION_FRAME','WGS84')
  str = file.readline()
  nrantennas = str.split()[0]
  nrdir = str.split()[4]
  ant = []
  for i in range(0,int(nrantennas)):
   ant.append(file.readline().split()[0:3])
  for t in grp.keys():
   grp.get(t).attrs.__setitem__('ANTENNA_POSITION_VALUE',[float(ant[(int(t)%1000)/2][0]),float(ant[(int(t)%1000)/2][1]),float(ant[(int(t)%1000)/2][2])])
   #print [float(ant[(int(t)%1000)/2][0]),float(ant[(int(t)%1000)/2][1]),float(ant[(int(t)%1000)/2][2])]
   grp.get(t).attrs.__setitem__('ANTENNA_POSITION_FRAME','local')
   grp.get(t).attrs.__setitem__('ANTENNA_POSITION_UNIT',['m','m','m'])
   grp.get(t).attrs.__setitem__('ANTENNA_ORIENTATION_VALUE',[0.0,0.0,1.0])
   grp.get(t).attrs.__setitem__('ANTENNA_ORIENTATION_FRAME','local')
   grp.get(t).attrs.__setitem__('ANTENNA_ORIENTATION_UNIT',['m','m','m'])
   grp.get(t).attrs.__setitem__('NYQUIST_ZONE',nyquist_zone)
   

#f.close()
#file.close()

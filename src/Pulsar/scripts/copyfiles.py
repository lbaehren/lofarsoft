FirstUse=True  # Put this to false to actually use the script
useFilename=False #for the parsetfilename
parsetfilename="notused" #isn't used if the above value is valse
obsid="L2011_25252" 
destinationdir="/data/scratch/alexov/L2011_25252/" # where to put the data
overwrite=False # Overwriting existing files?
copyincoherentstokes=True # Copy incoherent stokes data?
copycoherentstokes=False  # Copy the coherent stokes data?

if FirstUse:
    print "Change the parameters in the start of this file to copy the relevant data"
    exit()

import os
import subprocess

if useFilename:
    parsetfilename=obsid
else:
    # What is the observation number? This determines the filename
    if '_' in obsid:
        obsnr=obsid.split('_')[1]
    else:
        obsnr=obsid.strip('L')

    # Name of the parset file
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

names=[]
if copycoherentstokes:
    if "Observation.Dataproducts.Output_CoherentStokes.filenames" in allparameters.keys():
        filenames=allparameters["Observation.Dataproducts.Output_CoherentStokes.filenames"].strip('[]').split(',')
        filedirs=allparameters["Observation.Dataproducts.Output_CoherentStokes.locations"].strip('[]').split(',')
        fileindex=0
        for dir in filedirs:
            if '*' in dir:
                (nfiles,location)=dir.split('*')
            else:
                nfiles=1
                location=dir
            nfiles=int(nfiles)
            for num in range(nfiles):
                if fileindex < len(filenames):
                    names.append(location+filenames[fileindex])
                    fileindex+=1
                else:
                    print "Error, could not find a new file position",fileindex,"in array",filenames


if copyincoherentstokes:
    if "Observation.Dataproducts.Output_IncoherentStokes.filenames" in allparameters.keys():
        filenames=allparameters["Observation.Dataproducts.Output_IncoherentStokes.filenames"].strip('[]').split(',')
        filedirs=allparameters["Observation.Dataproducts.Output_IncoherentStokes.locations"].strip('[]').split(',')
        fileindex=0
        for dir in filedirs:
            if '*' in dir:
                (nfiles,location)=dir.split('*')
            else:
                nfiles=1
                location=dir
            nfiles=int(nfiles)
            for num in range(nfiles):
                names.append(location+filenames[fileindex])
                fileindex+=1

for name in names:
    shortname=name.split('/')[-1]
    if not overwrite and shortname in os.listdir(destinationdir):
        print "File",destinationdir+shortname,"already exists. Not overwriting file"
    else:
        subprocess.Popen(["scp",name,destinationdir])
    
subprocess.Popen(["chgrp","-R","pulsar",destinationdir])
subprocess.Popen(["chmod","-R","777",destinationdir])

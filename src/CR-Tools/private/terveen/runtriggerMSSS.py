#! /usr/bin/env python

#import bfdata as bfd
from pycrtools import bfdata as bfd
import sys
import time
import os
import numpy as np


#Set up parameters
# Which fraction of the rest of the data to analyze
fraction=1.0
# Use the specified nr blocks to analyze? If not is will use the fraction of the total
use_absolute_nrblocks=True
nrblocks=60
pulsehomedir="/home/veen/analysis/"
SBsPerBeam=240

#
DM="2 0 26.83"
#DM="32 0 10 10.5 11 11.5 12 12.5 13 13.5 14 14.5 15 15.5 16 16.5 17 17.5 18 18.5 19 19.5 20 20.5 21 21.5 22 22.5 23 23.5 24 24.5 25"
#DM="10 0 1 2 3 4 5 6 7 8 9" 
#10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29"
# 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59"

UseVerbose=False
UsePad=True
calcCoinNr=True
calcnrCSB=False
coincidenceNumber=8
nrCSB=15
triggerlength=1
coincidenceTime=30
triggerlevel=10
badchannels=np.load("/home/veen/logs/badchannels.npy")

# Check if an observation ID is provided
if len(sys.argv)>=2:
    print sys.argv[1]
else:
    print "usage",sys.argv[0],"ObsID"
    print "example:",sys.argv[0],"L21117"
    quit()

# Get the observation ID
if sys.argv[1]=="runtriggerMSSS.py":
    ObsID=sys.argv[2]
    beamnr=int(sys.argv[3])
else:
    ObsID=sys.argv[1]
    beamnr=int(sys.argv[2])

# Library containing all parameters
ObsPar = {}
# Get the observation parameters
parsetdir='/globalhome/lofarsystem/production/lofar/bgfen/log/'
parsetsubdir=ObsID+'/'
#parsetdir='/Users/STV/Astro/Analysis/FRAT/'
#parsetsubdir=''
p=bfd.get_parameters_new(parsetdir+parsetsubdir+'/'+ObsID+'.parset',True)

#if not p["incoherentstokes"]:
#    print "No incoherent stokes data to trigger on"
#    quit()

#print "Parameters from ObsID"
#for key in p.keys():
#    if key != "files":
#	print key,p[key]

#print "End of parameter list\n\n"
# Calcultate starttime and endtime in seconds since EPOCH
year=int(p["starttime"][1:5])
month=int(p["starttime"][6:8])
day=int(p["starttime"][9:11])
hour=int(p["starttime"][12:14])
minute=int(p["starttime"][15:17])
second=int(p["starttime"][18:20])

endyear=int(p["stoptime"][1:5])
endmonth=int(p["stoptime"][6:8])
endday=int(p["stoptime"][9:11])
endhour=int(p["stoptime"][12:14])
endminute=int(p["stoptime"][15:17])
endsecond=int(p["stoptime"][18:20])

starttime=time.mktime((year,month,day,hour,minute,second,0,0,0))
stoptime=time.mktime((endyear,endmonth,endday,endhour,endminute,endsecond,0,0,0))
currenttime=time.time()

# Calculate the duration of one block of data
blockduration=1/(float(p["clockfrequency"])*1e6)*1024*float(p["samples"])*float(p["channels"])

print starttime, stoptime, stoptime-starttime, blockduration 

# Calculate where to start
totalnumberbl=(stoptime-starttime)/blockduration
if currenttime > stoptime:
   print "Observation finished, processing all data offline"
   startblock=0
elif currenttime > starttime:
   print "Observation already started, starting at a later time"
   startblock=int((currenttime-starttime)/blockduration+30)
   totalnumberbl-=startblock
else:
   startblock=0    

print "start block=",startblock

if not use_absolute_nrblocks:
   nrblocks=int(fraction*totalnumberbl)
print "number of blocks=",nrblocks

ObsPar["stb"]=startblock
ObsPar["n"]=nrblocks


obs_nr=int(p["obsid"])

pulsedir=pulsehomedir+"obs"+str(obs_nr)+"/"
if not os.path.isdir(pulsedir):
    os.mkdir(pulsedir)

if 'runnr.dat' in os.listdir(pulsedir):
    runnrfile=open(pulsedir+'/runnr.dat','rw+')
    runnrfile.seek(0)
    runnr=int(runnrfile.read().strip('\n'))+1
    runnrfile.close()
    os.remove(runnrfile.name)
    runnrfile=open(pulsedir+'runnr.dat','w+')
else:
    runnrfile=open(pulsedir+'runnr.dat','w+')
    runnr=1

runnrfile.seek(0)
runnrfile.write(str(runnr))
runnrfile.close()

pulsedir+="run"+str(runnr)+"/"
if not os.path.isdir(pulsedir):
    os.mkdir(pulsedir)

ObsPar["pulsedir"]=pulsedir

ObsPar["indirs"]=""
ObsPar["log"]=pulsedir+"TransientTrigger.log"
#ObsPar["ext"]=""
ObsPar["ch"]=p["channels"]
ObsPar["sa"]=p["samples"]/p["timeintegration"]
if UseVerbose: ObsPar["verbose"]=""
if UsePad: ObsPar["pad"]=""
ObsPar["tInt"]=p["timeintegration"]
ObsPar["DM"]=DM
ObsPar["startSB"]=p["subbands"][0]#str(int(p["subbands"].split('..')[0].strip('['))-SBsPerBeam*beamnr)
ObsPar["Cnr"]=""
ObsPar["tl"]=triggerlevel
ObsPar["nrCSB"]=""
#ObsPar["FSB"]=""
#ObsPar["LSB"]=""
ObsPar["Ctime"]=""
ObsPar["il"]=p["timeintegration"]

print ObsPar

from socket import gethostname; 
hostname=gethostname()
fileselection=""
usedfiles=[]
#hostname='lse004'

if 'lse' in hostname:
    print "Program is on a storage node",hostname
    fileselection=hostname
    useLSE=True
    useLOCAL=False
elif 'lce' in hostname:
    print "Program is on a processing node",hostname
    subcluster=int(hostname[3:6])/9+1
    fileselection="sub"+str(subcluster)
    useLSE=False
    useLOCAL=False
elif 'sander' in hostname or 'locus' in hostname:
    print "Program is on own laptop",hostname
    useLOCAL=True
    useLSE=False
else:
    print "Can't run trigger on this node"
    quit()

print useLOCAL
if useLOCAL:
    import glob
    usedfiles=glob.glob("/data/L"+p["obsid"]+"/*.raw")
else:
    for file in p["files"]:
        if fileselection in file and "incoherent" in file:
            if useLSE:
                 usedfiles.append('/data'+file.split('data')[1])
            else:
                 usedfiles.append(file)




    #usedfiles=usedfiles[35*beamnr:35*(beamnr+1)]
    #if len(usedfiles) is 0:
    #    print "Quiting program, no files available to run trigger on"
    #    #quit()

if False:
    print usedfiles
    #raw_input("Press ENTER to exit")
    FSB=usedfiles[0].split('SB')[1][0:3].lstrip('0')
    if FSB is '':
       FSB='0'
    LSB=usedfiles[-1].split('SB')[1][0:3].lstrip('0')
    if LSB is '':
       LSB='0'
    ObsPar["ext"]=usedfiles[0].split('SB')[1][3:]
    if useLSE or useLOCAL:
        ObsPar['indirs']="1 "+usedfiles[0].split('SB')[0]+" "+FSB 
    else:
        nrdirs=0
        fileindex=0
        for k in range(1,4):
             lsenode=str((subcluster-1)*3+k)
             while len(lsenode) < 3:
                lsenode='0'+lsenode
             print "reading files for node",lsenode
             if fileindex < len(usedfiles):
               if "lse"+lsenode in usedfiles[fileindex]:
                 nrdirs+=1
                 SBnr=usedfiles[fileindex].split('SB')[1][0:3].lstrip('0')
                 if SBnr is '':
                    SBnr='0'
                 ObsPar['indirs']+=" "+usedfiles[fileindex].split('SB')[0]+" "+SBnr
                 while "lse"+lsenode in usedfiles[fileindex]:
                     fileindex+=1
                     if fileindex >= len(usedfiles):
                         break
        ObsPar['indirs']=str(nrdirs)+ObsPar['indirs']


if True:
    #ObsPar["ext"]=usedfiles[0].split('SB')[1][3:]
    if len(usedfiles)==1:
        ObsPar['i']=usedfiles[0]
    else:
        print "Please implement the use of more file in one directory"
    print ObsPar



#    ObsPar["FSB"]=FSB
#    ObsPar["LSB"]=LSB
#    nrSB=int(LSB)-int(FSB)
#    if calcCoinNr or coincidenceNumber*nrCSB > nrSB:
#        coincidenceNumber=nrSB/nrCSB 
#    elif calcnrCSB:
#        nrCSB=nrSB/coincidenceNumber
    if coincidenceNumber == 0:
        coincidenceNumber=1
        nrCSB=1

    ObsPar["nrCSB"]=str(nrCSB)
    ObsPar["Cnr"]=coincidenceNumber
    ObsPar["il"]=triggerlength
    ObsPar["Ctime"]=coincidenceTime
    if 'LBA' in p['filterselection']:
        ObsPar["LBA"]=""

    frequencies=p['frequencies']
    ObsPar["freq"]=str(len(frequencies))
    for freq in frequencies:
        ObsPar["freq"]+=" "+str(freq)
    command="/home/veen/apps/FRATStrigger"

    ObsPar["badch"]=str(len(badchannels))
    for ch in badchannels:
        ObsPar["badch"]+=" "+str(ch)
    #command="/home/veen/TransientTriggerV9"
    args=[]
    args.append(command)
    command+=" -i "+str(ObsPar["i"])
    args.append('-i')

    args.extend(ObsPar["indirs"].split(' '))
    for key in ObsPar.keys():
        if key != "indirs":
            command+=" -"+key+" "+str(ObsPar[key])
            args.append('-'+key)
            if ObsPar[key]!="":
                args.append(str(ObsPar[key]))

    print command
    set=open(ObsPar["pulsedir"]+'/settings.log','w')
    set.write(command)
    set.close()

    beamf=open(ObsPar["pulsedir"]+'/beam'+str(beamnr)+'_'+obsid,'w')
    beamf.write(command)
    beamf.close()


    import subprocess
    #print args
    #os.popen(command)
    #subprocess.Popen(command)


    #~/TransientTriggerV9 -indirs $filedirs -n $number -stb $startblock -log $pulsepa
    #th/TransientTrigger.log -tlog /home/veen/trigger.log -ext "_bf.incoherentstokes"
    # -ch $channels -sa $samples -verbose -pad -tInt $intSteps -DM $DM -startSB $star
    #tsubband -Cnr $coincidencenumber -tl $triggerlevel -nrCSB $combinedSubbands -FSB
    # $FSB -LSB $LSB -Ctime $coincidencetime -il $integrationlength -pulsedir $pulsep
    #ath # > $pulsepath/output.log


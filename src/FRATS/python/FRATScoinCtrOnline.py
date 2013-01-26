#! /usr/bin/env python
# UDP server example
import socket
import struct
import time
import struct
import numpy as np
try:
    import bfdata as bf
except ImportError:
    from pycrtools import bfdata as bf
import signal,sys
import os
import glob
from FRATSanalysis import obsParameters,derivedParameters,msgToPlotcommand
from optparse import OptionParser
parser=OptionParser()
parser.add_option("--startDM",type="float")
parser.add_option("--stepDM",type="float")
parser.add_option("--stopDM",type="float")
parser.add_option("--samples","--sa",type="int",default=512)
parser.add_option("-n","--nrblocks",type="int",default=970)
parser.add_option("-f","--file",type="string")
parser.add_option("-w","--window",type="int",default=50)
parser.add_option("--coinNr",type="int",default=3)
parser.add_option("--minTriggerDM",type="float",default=0.0)
parser.add_option("--minDM",type="float",default=5.0)
parser.add_option("--DMdelay",type="float",default=65000)
parser.add_option("--nstreams",type="int",default=8)
parser.add_option("-l","--logfile",type="string")
parser.add_option("-t","--threshold",type="float",default=5.5)
parser.add_option("-m","--maxlength",type="int",default=1024)
parser.add_option("--nosend",action="store_true",dest="nosend",default="False")
parser.add_option("--resampleT",type="int",default=1)
parser.add_option("--beam",type="string",default="B012")

(options,args)=parser.parse_args()
print options
print options.startDM
print options.stopDM
print options.stepDM
print """ PARAMETERS """
if options.startDM==None or options.stepDM==None or options.stopDM==None:
    raise ValueError("please specify --startDM, --stepDM and --stopDM")
else:
    print "Running on DMs",options.startDM,"-",options.stopDM," with step ",options.stepDM

if not options.file:
    raise ValueError("please specify where to store the triggermessages")

minimumStreams=options.coinNr
maxlength=options.maxlength
streamsRequired=options.coinNr
minDMrequired=options.minDM
n=options.nrblocks
sa=options.samples
nstreams=options.nstreams
DMdelay=int(options.DMdelay)+options.window
halfwindow=options.window
nDMs=np.round((options.stopDM-options.startDM)/options.stepDM)+1
minDM=options.startDM
DMstep=options.stepDM
threshold=options.threshold
outfilename=options.file #'/home/veen/FRATS/TriggerMsgRecent.log'
resampleT=options.resampleT
mydir=os.path.split(outfilename)[0]
beam=options.beam




nosend=options.nosend
if options.logfile:
    logfile=open(options.logfile,'w')
    logfile.writelines(["new observation started at "+str(time.time())+" "+str(time.strftime("%D %T"))+"\n", \
    "settings: "+" n="+str(n)+ " sa="+str(sa)+" DMdelay="+str(DMdelay)+" w="+str(halfwindow)+" thr="+str(threshold)+"\n"])
    logging=True
else:
    logging=False

print "Nr of blocks",n
print "samples per block",sa
print "Nr of streams",nstreams
print "Coincidence window",halfwindow
print "Writing to file",options.file


def signal_handler(signal,frame):
    print "Terminating program by signal",signal
    fout.close()
    if logging:
        logfile.writelines(["observation stopped at "+str(time.time())+" "+str(time.strftime("%D %T"))+"\n"])
    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)
signal.signal(signal.SIGTERM, signal_handler)


port1=0x7BA0 # receiving port
port2=31660 # sending port

sendadress="10.135.252.101" #lhn001
#sendadress="localhost"
#fmt='3L4i2f2i4f2i'
fmt='<3q4i2f2i4f4i'
fmtlen=struct.calcsize(fmt)
fmtsend='>cciic'
sourceID='\x65' # identifies FRATS
magic1='\x99'
magic2='\xA0'

#senddata=str(sec)+"_"i+str(nsec)

client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
if 'server_socket' not in dir():
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    server_socket.bind(("",port1))

trmsg=dict()




def get_pointing(obsID,beam):
    from os import environ
    if 'parsetdir' in environ.keys():
        parsetdir=os.environ['parsetdir']
    else:
        parsetdir='/Users/STV/Astro/Analysis/FRAT/parsets/'
    if type(obsID)==type(1):
        p=bf.get_parameters_new('parsets/L'+str(obsID)+'.parset',True)
    else:
        p=bf.get_parameters_new('parsets/'+obsID+'.parset',True)
    RA=p['beam'][beam]['RA']
    DEC=p['beam'][beam]['DEC']
    return (RA,DEC)

def DMtoIndex(DM,minDM,DMstep):
    return int(round((DM-minDM)/DMstep,0))

def IndexToDM(index,minDM,DMstep):
    return minDM+index*DMstep


checkdurations=[]
coincidences=[]
#filename='/Users/STV/Astro/Analysis/FRAT/analysis/L65367/SAP002_11-16/TriggerMsg.log'
#filename='/Users/STV/Astro/Analysis/FRAT/analysis/L65367/SAP002_0-12.46/timeseries_0_972/TriggerMsg.log'

#outfilename='/Users/STV/Astro/Analysis/FRAT/analysis/L65367/SAP002_0-12.46/timeseries_0_972/TriggerMsgOut.log'
#ft=open(filename)
fout=open(outfilename,'w')
#data=ft.read()
print "listening at port ",port1
flush=10000

dtype = [('time', 'int64'),('utc_second','int64'),('utc_nanosecond','int64'),('length','int32'),('sample','int32'),('block','int32'),('subband','int32'),('sum','float32'),('max','float32'),('obsID','int32'),('beam','int32'),('DM','float32'),('SBaverage','float32'),('SBstddev','float32'),('Threshold','float32'),('nrFlaggedChannels','int32'),('nrFlaggedSamples','int32')]#,('msgindex','int)]

#trmsg2=np.zeros(BUFFERSIZE,dtype=dtype)
#index=np.arange(BUFFERSIZE)

#nDMs=3
#minDM=0
#DMstep=6.22
trmatrix=np.zeros((nDMs,6,n*sa/halfwindow+1+DMdelay/halfwindow,nstreams),'int')
len2index={1:0,2:1,4:2,8:3,16:4,32:5}
triggers=[]

#maxtrmsg=20000
#startmsg=0#200000
bNumpyMethod=False
#trmsg2=np.frombuffer(data[0:maxtrmsg*fmtlen],dtype)
#totaltriggers=min(len(data)/fmtlen,maxtrmsg)
t0=time.time()
i=0
#    while i<min(len(data)/fmtlen,maxtrmsg):
while True:
        recvdata, address = server_socket.recvfrom(fmtlen)
#        recvdata=data[(i+startmsg)*fmtlen:(i+startmsg+1)*fmtlen]
        msg=struct.unpack(fmt,recvdata)+(i,)
        DM=msg[11]
        if DM>=minDMrequired and msg[3]<maxlength and msg[8]>threshold:
            DMindex=DMtoIndex(DM,minDM,DMstep)
            time2=msg[0]
            timeindex=time2/halfwindow
            subband=msg[6]
            length=len2index[msg[3]]
            trmatrix[DMindex,length,timeindex,subband]=1
            t_offset=-1
            mysum=np.sum(trmatrix[DMindex,length,timeindex]|trmatrix[DMindex,length,timeindex+t_offset])
            if mysum < streamsRequired:
            #print DMindex,timeindex,-1
                t_offset=+1
                mysum=np.sum(trmatrix[DMindex,length,timeindex]|trmatrix[DMindex,length,timeindex+t_offset])

            #print DMindex,timeindex,+1
            if mysum >= streamsRequired:
                triggers.append((DMindex,timeindex,mysum,t_offset))
                print IndexToDM(DMindex,minDM,DMstep),msg[11],msg[0],msg[1],msg[2],mysum,msg[8],length
                par=obsParameters(mydir)
                if 'resampleT' not in par.keys():
                    par['resampleT']=1
                files=glob.glob(mydir+"/*beam*")
                if len(files)==1:
                    programname=os.path.split(files[0])[1]
                    if "LOTAAS" in programname:
                        programname="frats_beam_LOTAAS_verification.sh"
                else:
                    programname="Run "

                derivedParameters(par,msg[11])
                totalDelay=round(max(par['delaysPerDM'])*DM/par['sa'])
                startblock=int(msg[0]/par['sa']*int(par['resampleT'])-totalDelay-5)
                nblocks=int(totalDelay+10)
                if DM%1<1e-5:
                    DMstr=str(round(DM+0.001,3)) #meed to add a bit extra else other program has problems
                elif DM%0.1<1e-5:
                    DMstr=str(round(DM,1))
                elif DM%0.01<1e-5:
                    DMstr=str(round(DM,2))
                elif DM%0.001<1e-5:
                    DMstr=str(round(DM,3))
                else:
                    DMstr=str(DM)

                print programname,"L"+str(msg[9]),'SAP00'+str(msg[10]),0,DMstr,DMstr,startblock,nblocks,beam
                print "fa.plotTimeseries(*fa.getTimeseriesPar(\""+mydir+"\",DMvalue="+DMstr+"),integrationlength="+str(msg[3])+",centraltime="+str(msg[0])+",window="+str(par['sa'])+",plotThreshold=True)"
                print msgToPlotcommand(msg,mydir,"run")
                if logging:
                    logfile.writelines(['Trigger found at'+str(time.time())+' , not sending='+str(nosend)+', DM='+str(msg[11])+' time='+str(msg[0])+' / '+str(msg[1]) \
                +'s'+str(msg[2])+'ns + coinStreams='+str(mysum)+' strength='+str(msg[8])+' l='+str(msg[3])+' in streams '+ \
                str(np.arange(nstreams)[trmatrix[DMindex,length,timeindex]|trmatrix[DMindex,length,timeindex+t_offset]])+'\n'])
                    logfile.writelines([str(msg)+'\n'])
                    logfile.writelines([programname+" L"+str(msg[9])+" SAP00"+str(msg[10])+" 0 "+DMstr+" "+DMstr+" "+str(startblock)+" "+str(nblocks)+" "+str(beam)+"\n"])
                    logfile.writelines(["fa.plotTimeseries(*fa.getTimeseriesPar(\""+mydir+"\",DMvalue="+DMstr+"),integrationlength="+str(msg[3])+",centraltime="+str(msg[0])+",window="+str(par['sa'])+",plotThreshold=True)"+"\n"])
                    logfile.writelines([msgToPlotcommand(msg,mydir,"run")])
                    logfile.flush()
                if True and msg[10]!=1:#not nosend:
                   print "sending time only to ",sendadress,port2
                   sec=msg[1]
                   nsec=msg[2]
                   senddata=struct.pack(fmtsend,magic1,magic2,sec,nsec,sourceID)
                   client_socket.sendto(senddata, (sendadress,port2))


        #trmatrix[DMtoIndex(msg[11],minDM,DMstep),int(msg[0]/halfwindow),msg[6]]=1
        i+=1
        if i>1:
            fout.write(recvdata)

t1=time.time()
triggers=list(set(triggers))





    #('\x99','\xA0',sec,nsec,DM,referenceFrequency,sourceID)
 #   print "sending time only to ",sendadress,port2
 #   senddata=struct.pack(fmtsend,magic1,magic2,sec,nsec,sourceID)
 #   client_socket.sendto(senddata, (sendadress,port2))





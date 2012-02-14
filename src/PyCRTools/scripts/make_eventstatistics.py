import pycrtools as cr
from pycrtools import lora
import os
import sys
import pycrtools.tasks as tasks
from time import gmtime
from datetime import *
from sidereal import *
import numpy as np
from skyplot import *



path = '/Volumes/MyBook/LOFAR_DATA/Results'
#path = '/Users/annanelles/Uni/LOFAR/data/GOOD'
dirList=os.listdir(path)
#dirList = ['VHECR_LORA-20111214T211001.912Z']

rmslist = []
timelist = []
azimuthgood = []
zenithgood = []
azimuthbad = []
zenithbad = []
for fname in dirList:
    if "VHECR" in fname:
        Ev = str(fname)
        try:

            statistics=cr.trun("eventstatistics",topdir=path,events=Ev)
            #print statistics
            rms = statistics[0]
            utc = statistics[1]
            direction = statistics[2]
            status = statistics[3]
            if direction[0] >= 0:
                if direction[1] >= 0:
                    if status == True:
                        print "OK"
                        zenithgood.append(90-direction[1])
                        azimuthgood.append(90-direction[0])
                    if status == False:
                        print "Bad"
                        zenithbad.append(90-direction[1])
                        azimuthbad.append(90-direction[0])                        
                       
            timestr = gmtime(utc)
            daysec = timestr[3]*3600+timestr[4]*60+timestr[5]
            rmslist.append(rms)
    #            timelist.append(daysec)
    
            stamp = datetime.datetime.utcfromtimestamp(utc)	
            GMSTstamp = SiderealTime.fromDatetime(stamp)
            LSTstamp = SiderealTime.lst(GMSTstamp,6.86313*pi/180.) 
            siderealsec = int(LSTstamp[0])*3600+int(LSTstamp[1])*60+float(LSTstamp[2]) 
            timelist.append(siderealsec)    
        except KeyboardInterrupt:
            raise
        except:
            continue     

cr.plt.figure()
cr.plt.plot(timelist,rmslist,linestyle="None",marker='o') 

from skyplot import *
coord = np.array(([zenithgood, azimuthgood]))  
coord = coord.transpose()
#print coord
cr.plt.figure()
skyplot(coord, 2,titleStr="Events seen",plotName="skyplot-good.ps") 


print "Bad Angles:", zenithbad, azimuthbad
coordbad = np.array(([zenithbad, azimuthbad]))  
coordbad = coordbad.transpose()
cr.plt.figure()
skyplot(coordbad,2,titleStr="Events missed",plotName="skyplot-bad.ps")    

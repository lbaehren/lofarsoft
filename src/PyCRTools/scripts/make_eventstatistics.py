import pytmf
import pycrtools as cr
from pycrtools import lora
import os
import sys
import pycrtools.tasks as tasks
from time import gmtime
import datetime
import numpy as np

# Longitude of LOFAR
L = pytmf.deg2rad(6.869837540)

path = '/Volumes/Data/VHECR/LORAtriggered/results'
dirList=os.listdir(path)

rmslist = []
timelist = []
azimuthgood = []
zenithgood = []
azimuthbad = []
zenithbad = []
for fname in dirList:
    if "VHECR" in fname:
        Ev = str(fname)
#        try:

        statistics=cr.trun("eventstatistics",topdir=path,events=Ev)
        print statistics
        rms = statistics[4][0]
        utc = statistics[1]
        #direction = statistics[2]
        status = statistics[3]
        #if direction[0] >= 0:
         #   if direction[1] >= 0:
          #      if status == True:
           #         print "OK"
            #        zenithgood.append(90-direction[1])
             #       azimuthgood.append(90-direction[0])
              #  if status == False:
               #     print "Bad"
                #    zenithbad.append(90-direction[1])
                 #   azimuthbad.append(90-direction[0])                        
                   
        timestr = gmtime(utc)
        daysec = timestr[3]*3600+timestr[4]*60+timestr[5]
        rmslist.append(rms)

        t = datetime.datetime.utcfromtimestamp(utc)	

        # Calculate JD(UT1)
        ut = pytmf.gregorian2jd(t.year, t.month, float(t.day) + ((float(t.hour) + float(t.minute) / 60. + float(t.second) / 3600.) / 24.))

        # Calculate JD(TT)
        dtt = pytmf.delta_tt_utc(pytmf.date2jd(t.year, t.month, float(t.day) + ((float(t.hour) + float(t.minute) / 60. + float(t.second) / 3600.) / 24.)))
        tt = pytmf.gregorian2jd(t.year, t.month, float(t.day) + ((float(t.hour) + float(t.minute) / 60. + (float(t.second) + dtt / 3600.)) / 24.));

        # Calculate Local Apparant Sidereal Time
        theta_L = pytmf.rad2circle(pytmf.last(ut, tt, L));

        timelist.append(theta_L)    
#        except KeyboardInterrupt:
#            raise
#        except Exception as e:
#            print e
#            continue     

np.savetxt("timelist", np.asarray(timelist))
np.savetxt("rmslist", np.asarray(rmslist))


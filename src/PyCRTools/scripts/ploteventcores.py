#! /usr/bin/env python
#
# A script to evaluate the results that come out of the cr_event pipeline and plot their core positions / directions.
# Arthur Corstanje, 2011 

import os
import numpy as np
import matplotlib.pyplot as plt
import pycrtools as cr

#topdir = '/Users/acorstanje/triggering/CR/results_john/'
topdir = '/data/VHECR/LORAtriggered/results/'
#event = 'VHECR_LORA-20110612T231913.199Z'
#event = 'VHECR_LORA-20110714T174749.986Z' # for use with Plotfootprint
#event = 'VHECR_LORA-20110716T094509.665Z'

filelist=cr.listFiles(topdir+'*') # hack to process only one event
eventlist=[]
for file in filelist: # can this be done shorter / in listFiles?
    if os.path.isdir(file):
        eventlist.extend([file])
        
# need to use only event directories, not files

print eventlist
print '**********'

# taking separate arrays (index = event in eventlist) instead of dicts with keys per event... better idea?
loracores = [0] * len(eventlist)
loradirections = [0] * len(eventlist) 
Ngood = [0] * len(eventlist) # number of 'good' (stations+)polarizations, i.e. delayquality < 1.0. 
lofardirections = [0] * len(eventlist) # average over all 'good' results?

k = -1
for eventdir in eventlist:
    print "Processing event in directory:",eventdir
    if os.path.isdir(os.path.join(eventdir,"pol0")) or os.path.isdir(os.path.join(eventdir, "pol1")):
        datadirs=cr.listFiles(os.path.join(os.path.join(eventdir,"pol?"),"*"))
    else:
        print 'no data dirs continue'
        continue
#    print eventdir
#    print datadirs
#    print '---'   
    k += 1 # index in eventlist
    for datadir in datadirs:
        if not os.path.isfile(os.path.join(datadir,"results.py")):
            continue # doesn't matter if 'datadir' isn't a directory...
        resfile = os.path.join(datadir,"results.py")
#        resfiletest = open(resfile)
#        if "nan" in resfiletest.read():
#            print "WARNING nan found. skipping file", resfile
#            resfiletest.close()
#            continue
#        resfiletest.close()
        res = {}
        execfile(resfile, res)
        res = res["results"]
        if res["delay_quality_error"] > 1.0:
            # if happens for all pol+stations, Ngood[k] will be 0 and the other entries will also be 0.
            print 'delay quality error bad for file', resfile
            continue
        print 'Good file', resfile
        Ngood[k] += 1
        if loracores[k] == 0:
            thisCore = res["pulse_core_lora"] # tuple, assume same for all results
            thisDirection = res["pulse_direction_lora"] # idem
            loracores[k] = thisCore
            loradirections[k] = thisDirection
        thisLofarDirection = res["pulse_direction"]
        if lofardirections[k] == 0:
            lofardirections[k] = thisLofarDirection
        else:
            lofardirections[k] = (lofardirections[k][0] + thisLofarDirection[0], lofardirections[k][1] + thisLofarDirection[1])
            # better way to add two tuples?
            
    if Ngood[k] > 0:
        lofardirections[k] = (lofardirections[k][0] / Ngood[k], lofardirections[k][1] / Ngood[k])
        # averaging directions... az and el separately. Only works if all directions are close
        # to each other, otherwise gives wrong results.
# end for event in eventlist        

# put plotting routine here...
goodevents = [x for x in range(len(eventlist)) if Ngood[x] > 0]
print '--- Events found with good results:'
for k in goodevents:
    print 'Event: ',
    print eventlist[k]
    print '# good results: ',
    print Ngood[k]
    print 'LORA core: ',
    print loracores[k]
    print 'LORA direction: ',
    print loradirections[k]
    print 'LOFAR direction: ',
    print lofardirections[k]
    print '-----'
    
    
    
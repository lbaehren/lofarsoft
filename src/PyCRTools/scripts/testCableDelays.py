#! /usr/bin/env python
#
# A test script to evaluate the results that come out of the cr_event pipeline.
# Arthur Corstanje, 2011 

import os
import numpy as np
import matplotlib.pyplot as plt
import pycrtools as cr
import pickle

topdir = '/Users/acorstanje/triggering/CR/results_john/'
#event = 'VHECR_LORA-20110612T231913.199Z'
event = 'VHECR_LORA-20110714T174749.986Z' # for use with Plotfootprint
#event = 'VHECR_LORA-20110716T094509.665Z'

filelist=cr.listFiles(topdir+'*') # hack to process only one event
eventlist=[]
for file in filelist: # can this be done shorter / in listFiles?
    if os.path.isdir(file):
        eventlist.extend([file])
        
# need to use only event directories, not files

print eventlist
print '**********'

antid = []
cabledelays = []
timestamps = []
cabledelays_database = dict()

for eventdir in eventlist:
    
    print "Processing event in directory:",eventdir
    if os.path.isdir(os.path.join(eventdir,"pol0")) or os.path.isdir(os.path.join(eventdir, "pol1")):
        print 'have data adirs'
        datadirs=cr.listFiles(os.path.join(os.path.join(eventdir,"pol?"),"*"))
    else:
        print 'no data dirs continue'
        continue
    print eventdir
    print datadirs
    print '---'
    for datadir in datadirs:
        if not os.path.isfile(os.path.join(datadir,"results.py")):
            continue # doesn't matter if 'datadir' isn't a directory...
        resfile = os.path.join(datadir,"results.py")
        res = {}
        execfile(resfile, res)
        res = res["results"]
        pol = res["polarization"]
        antid.extend([str(int(v)) for v in res["antennas"].values()])
        # check: same ordering for ant-id's and cable delays??? Relying on that.
        cabledelays.extend(res["antennas_residual_cable_delays"])  # make that 'total' cable delays, and have also 'residual' delays      
        # use antenna id as key in the database
        timelist = [res["TIME"]] * len(res["antennas"])
        timestamps.extend(timelist)
        
count = dict()
for i in range(len(antid)):
    if not antid[i] in cabledelays_database.keys():
        cabledelays_database[antid[i]] = dict(cabledelay = 0.0, spread = 0.0, delaylist = [], timelist = [])
#    cabledelays_database[antid[i]] += cabledelays[i]
#    count[antid[i]] += 1
    cabledelays_database[antid[i]]["delaylist"].extend([cabledelays[i]])
    cabledelays_database[antid[i]]["timelist"].extend([timestamps[i]])
#    count[antid[i]] = 1

for key in cabledelays_database:
    theseDelays = np.array(cabledelays_database[key]["delaylist"])
    avg = theseDelays.mean()
    spread = theseDelays.std()
    
    cabledelays_database[key]["cabledelay"] = avg
    cabledelays_database[key]["spread"] = spread

outfile = open(os.path.join(topdir, 'Cabledelays.pic'), 'wb')
pickle.dump(cabledelays_database, outfile)
outfile.close()
print 'pickled done'

# real fix: plot footprint of cable delays, correct for outliers... (logscale?)

# get list of ant ids from plotfootprint task; needed to ensure correct ordering

filefilter = os.path.join(topdir, event)
print filefilter
polarization = 0 # should do both
fptask = cr.trerun("plotfootprint", "0", colormap = 'jet', filefilter = filefilter, pol=polarization) 
print 'FP TAKS DONE'
y = []
yspread = []
for antid in fptask.antid:
    thisCableDelay = 1.0e9 * cabledelays_database[str(antid)]["cabledelay"]
    if thisCableDelay > 50:
        thisCableDelay = 0.0 # kick outliers
    y.extend([thisCableDelay])
    yspread.extend([cabledelays_database[str(antid)]["spread"]])

cdelays = cr.hArray(y)
cdelayspread = np.array(yspread)

# get original cable delays from metadata
cabledelays_full=cr.metadata.get("CableDelays", fptask.antid, res["ANTENNA_SET"], True)  # Obtain cabledelays
#cabledelays_full-=cabledelays_full[0] # Correct w.r.t. referecence antenna
#        cabledelays=cabledelays_full % sample_interval #Only sub-sample correction has not been appliedcabledelays=cabledelays_full % 5e-9  # Only sub-sample correction has not been applied
#        cabledelays=hArray(cabledelays)

fptask = cr.trerun("plotfootprint", "1", colormap = 'jet', filefilter = filefilter, arrivaltime = cdelays, power=cabledelays_full, size = 150,  pol=polarization) 

# now plot cable delays (quick fix for 3 events)

plt.figure()

x = np.zeros((len(cabledelays_database), 3))
for i in range(3):
    x[:, i] = np.arange(len(cabledelays_database))

y = np.zeros((len(cabledelays_database), 3))
ydiff = np.zeros((len(cabledelays_database), 3))
yerr = np.zeros(len(cabledelays_database))
n=0
for key in cabledelays_database:
    thisAnt = cabledelays_database[key]
#    y[n] = thisAnt["cabledelay"]
    for k in range(len(thisAnt["delaylist"])):
        thisAvg = 1.0e9 * thisAnt["cabledelay"]
        thisDelay = 1.0e9 * thisAnt["delaylist"][k]
        thisDiff = thisDelay - thisAvg
        y[n, k] = (30.0 * k + thisDelay) if thisDelay < 30.0 else (-10.0 + k)# remove outliers
        ydiff[n, k] = (0.0 * k + thisDiff) #if thisDiff < 30.0 else (30.0 + k) 
    yerr[n] = thisAnt["spread"]
    n+=1
    
plt.scatter(x[:,0], ydiff[:,0], c='b')
plt.scatter(x[:,1], ydiff[:,1], c='g')
plt.scatter(x[:,2], ydiff[:,2], c='r')
plt.ylabel('Delay offset from average [ns]')
plt.xlabel('Antenna number (concatenated)')
plt.title('Cable delays minus average for 3 selected events')

plt.figure()

plt.scatter(x[:,0], y[:,0], c='b')
plt.scatter(x[:,1], y[:,1], c='g')
plt.scatter(x[:,2], y[:,2], c='r')
plt.ylabel('Cable delays [ns] + 30x event number')
plt.xlabel('Antenna number (concatenated)')
plt.title('Cable delays for 3 selected events\n(outliers above 30.0 ns put at y<0)')

plt.figure()
plt.scatter(x[:, 0], yerr)
    
    
    
    
    
    
    
    
    
    
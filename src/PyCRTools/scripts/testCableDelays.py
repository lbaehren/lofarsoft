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

filelist=cr.listFiles(topdir+'*')
eventlist=[]
for event in filelist: # can this be done shorter / in listFiles?
    print event
    if os.path.isdir(event) == True:
        eventlist.extend([event])
        
# need to use only event directories, not files

print eventlist
print '**********'

antid = []
cabledelays = []
timestamps = []
cabledelays_database = dict()

for eventdir in eventlist:
    
    print "Processing event in directory:",eventdir
    datadirs=cr.listFiles(os.path.join(os.path.join(eventdir,"pol?"),"*"))
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
        cabledelays.extend(res["antennas_final_cable_delays"])  # make that 'total' cable delays, and have also 'residual' delays      
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

# now plot cable delays (quick fix for 3 events)
x = np.zeros((len(cabledelays_database), 3))
for i in range(3):
    x[:, i] = np.arange(len(cabledelays_database))

y = np.zeros((len(cabledelays_database), 3))
yerr = np.zeros(len(cabledelays_database))
n=0
for key in cabledelays_database:
    thisAnt = cabledelays_database[key]
#    y[n] = thisAnt["cabledelay"]
    for k in range(len(thisAnt["delaylist"])):
        y[n, k] = thisAnt["delaylist"][k]
    yerr[n] = thisAnt["spread"]
    n+=1
    
plt.scatter(x[:,0], y[:,0], c='b')
plt.scatter(x[:,1], y[:,1], c='g')
plt.scatter(x[:,2], y[:,2], c='r')

    
    
    
    
    
    
    
    
    
    
    
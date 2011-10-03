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

cabledelays_database = dict()

for eventdir in eventlist:
    
    print "Processing event in directory:",eventdir
    par={}
#    antid={0:[],1:[]}
#    signal={0:[],1:[]}
#    positions={0:[],1:[]}
#    positions2={0:[],1:[]}
#    ndipoles={0:0,1:0}
#    timelags={0:[],1:[]}
#    cabledelays = {0:[], 1:[]}
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
        cabledelays.extend(res["antennas_final_cable_delays"])  # make that 'total' cable delays, and have also 'residual' delays      
        # use antenna id as key in the database
        
count = dict()
for i in range(len(antid)):
    if antid[i] in cabledelays_database.keys():
        cabledelays_database[antid[i]] += cabledelays[i]
        count[antid[i]] += 1
    else:
        cabledelays_database[antid[i]] = cabledelays[i]
        count[antid[i]] = 1

for key in cabledelays_database.keys():
    cabledelays_database[key] /= count[key]

outfile = open(os.path.join(topdir, 'Cabledelays.pic'), 'wb')
pickle.dump(cabledelays_database, outfile)
outfile.close()

#! /usr/bin/env python
#
# A test script to evaluate the results that come out of the cr_event pipeline.
# Arthur Corstanje, 2011 

import os
import numpy as np
import matplotlib.pyplot as plt
import pycrtools as cr

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

for eventdir in eventlist:
    
    print "Processing event in directory:",eventdir
    par={}
    antid={0:[],1:[]}
    signal={0:[],1:[]}
    positions={0:[],1:[]}
    positions2={0:[],1:[]}
    ndipoles={0:0,1:0}
    timelags={0:[],1:[]}

    datadirs=cr.listFiles(os.path.join(os.path.join(eventdir,"pol?"),"*"))
    print eventdir
    print datadirs
    print '---'


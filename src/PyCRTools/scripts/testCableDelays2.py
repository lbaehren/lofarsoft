#! /usr/bin/env python
#
# A test script to evaluate the results that come out of the cr_event pipeline.
# Arthur Corstanje, 2011 

import os
import numpy as np
import matplotlib.pyplot as plt
import pycrtools as cr
import pickle

topdir = '/Users/acorstanje/triggering/CR/results_CS003/'
#event = 'VHECR_LORA-20110612T231913.199Z'
#event = 'fullLOFAR--5' # for use with Plotfootprint
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
residualdelays = []
positions = []
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
        try:
            execfile(resfile, res)
        except NameError:
            print 'Name error! Skipping file %s ' % datadir
            continue
        res = res["results"]
        # check result status
#        if res["status"] != 'OK':
        if res["delay_quality_error"] >= 1 or res["delay_outliers"] > 5:
            print 'File status not OK: %s %s' % (datadir, res["delay_quality_error"])
            continue
        # read in antenna positions from file
        positions.extend(res["antenna_positions_array_XYZ_m"]) # in flat list
        pol = res["polarization"]
        if pol == 5:
            continue
        antid.extend([str(int(v)) for v in res["antennas"].values()])
            
        # check: same ordering for ant-id's and cable delays??? Relying on that.
        cabledelays.extend(res["antennas_final_cable_delays"])  # make that 'total' cable delays, and have also 'residual' delays      
        residualdelays.extend(res["antennas_residual_cable_delays"])
        # use antenna id as key in the database
        timelist = [res["TIME"]] * len(res["antennas"])
        timestamps.extend(timelist)
        
count = dict()
antennapositions = dict()
for i in range(len(antid)):
    if not antid[i] in cabledelays_database.keys():
        cabledelays_database[antid[i]] = dict(cabledelay = 0.0, residualdelay = 0.0, spread = 0.0, delaylist = [], residuallist = [], timelist = [])
        antennapositions[antid[i]] = positions[i * 3: (i+1)*3]
#    cabledelays_database[antid[i]] += cabledelays[i]
#    count[antid[i]] += 1
    cabledelays_database[antid[i]]["delaylist"].extend([cabledelays[i]])
    cabledelays_database[antid[i]]["residuallist"].extend([residualdelays[i]])
    cabledelays_database[antid[i]]["timelist"].extend([timestamps[i]])
#    count[antid[i]] = 1

# get averages of cable delays. Zero out if spread too high?
for key in cabledelays_database:
    theseDelays = cabledelays_database[key]["delaylist"]
    theseDelays = np.array(theseDelays)
    theseResiduals = np.array(cabledelays_database[key]["residuallist"])
#    theseDelays[np.where(abs(theseDelays) > 20.0)] = float('nan')
    avg = theseDelays.mean()
    residualavg = theseResiduals.mean()
    spread = theseDelays.std()

    if spread > 5.0e-9:
        avg = 0.0 # remove inconsistent fit values (improve?)
        residualavg = 0.0 # to show in plot...
    cabledelays_database[key]["cabledelay"] = avg
    cabledelays_database[key]["residualdelay"] = residualavg
    cabledelays_database[key]["spread"] = spread

outfile = open(os.path.join(topdir, 'Cabledelays.pic'), 'wb')
pickle.dump(cabledelays_database, outfile)
outfile.close()
print 'pickled done'

# real fix: plot footprint of cable delays, correct for outliers... (logscale?)

# get list of ant ids from plotfootprint task; needed to ensure correct ordering

#filefilter = os.path.join(topdir, event)
#print filefilter
polarization = 0 # should do both

#trerun("PlotAntennaLayout","Delays",positions = ,colors=final_residual_delays[antennas_with_strong_pulses],sizes=100,names=good_antennas_IDs[antennas_with_strong_pulses],title="Delay errors in station",plotlegend=True)

#fptask = cr.trerun("plotfootprint", "0", colormap = 'jet', filefilter = filefilter, pol=polarization) 
print 'FP TAKS DONE'
y = []
yspread = []
positions = []
names = []
for id in antennapositions.keys(): # get unique ids
    thisCableDelay = 1.0e9 * cabledelays_database[str(id)]["residualdelay"]
    thisSpread = 1.0e9 * cabledelays_database[str(id)]["spread"]
    positions.extend(antennapositions[id])
    names.append(str(int(id) % 100)) # NB. Append needed rather than extend, as we want to put a string into the list...
    if abs(thisSpread) > 5.0:
        thisCableDelay = 0.0 # kick outliers
    y.extend([thisCableDelay])
    yspread.extend([thisSpread])

cdelays = cr.hArray(y)
cdelayspread = np.array(yspread)
nofantennas = len(positions) / 3
cr.trerun("PlotAntennaLayout","Delays",positions = cr.hArray(dimensions=[nofantennas, 3], fill=positions), colors= cr.hArray(y),sizes=100,names = cr.hArray(names),title="Cable delays",plotlegend=True)


# get original cable delays from metadata
#cabledelays_full=cr.metadata.get("CableDelays", fptask.antid, res["ANTENNA_SET"], True)  # Obtain cabledelays
#cabledelays_full-=cabledelays_full[0] # Correct w.r.t. referecence antenna
#        cabledelays=cabledelays_full % sample_interval #Only sub-sample correction has not been appliedcabledelays=cabledelays_full % 5e-9  # Only sub-sample correction has not been applied
#        cabledelays=hArray(cabledelays)

#fptask = cr.trerun("plotfootprint", "1", colormap = 'jet', filefilter = filefilter, arrivaltime = cdelays, power=cabledelays_full, size = 150,  pol=polarization) 

# now plot cable delays with spread

plt.figure()

#x = np.zeros(len(cabledelays_database))
#for i in range(3):
x = np.arange(len(cabledelays_database))

y = np.zeros(len(cabledelays_database))

x = []
y_total = []
y_residual = []
n = 0
plotparameter = "residualdelay"
for key in cabledelays_database:
    thisAnt = cabledelays_database[key]
#    y[n] = thisAnt["cabledelay"]
    thisSpread = 1.0e9 * thisAnt["spread"]
    if thisSpread < 5.0:
        for k in range(len(thisAnt["residuallist"])): # residual and total lists are the same size...
            x.extend([n])
#            thisAvg = 1.0e9 * thisAnt["residualdelay"]
            thisResidualDelay = 1.0e9 * thisAnt["residuallist"][k]
            thisTotalDelay = 1.0e9 * thisAnt["delaylist"][k]
#            thisDiff = thisDelay - thisAvg
#            y.extend([thisDelay])
            y_residual.extend([thisResidualDelay])
            y_total.extend([thisTotalDelay])
    #        y[n, k] = (30.0 * k + thisDelay) if thisDelay < 30.0 else (-10.0 + k)# remove outliers
    #        ydiff[n, k] = (0.0 * k + thisDiff) #if thisDiff < 30.0 else (30.0 + k) 
    #    yerr[n] = thisAnt["spread"]
    n+=1

plt.scatter(x, y_residual)
plt.ylabel('Residual cable delay [ns]')
plt.xlabel('Antenna number (concatenated)')
plt.title('Residual cable delays after last pipeline iteration')
    
plt.figure()
plt.scatter(x, y_total)
plt.ylabel('Total cable delay [ns]')
plt.xlabel('Antenna number (concatenated)')
plt.title('Total cable delays after last pipeline iteration')

#plt.scatter(x[:,0], ydiff[:,0], c='b')
#plt.scatter(x[:,1], ydiff[:,1], c='g')
#plt.scatter(x[:,2], ydiff[:,2], c='r')
#plt.ylabel('Delay offset from average [ns]')
#plt.xlabel('Antenna number (concatenated)')
#plt.title('Cable delays minus average for 3 selected events')

#plt.figure()

#plt.scatter(x[:,0], y[:,0], c='b')
#plt.scatter(x[:,1], y[:,1], c='g')
#plt.scatter(x[:,2], y[:,2], c='r')
#plt.ylabel('Cable delays [ns] + 30x event number')
#plt.xlabel('Antenna number (concatenated)')
#plt.title('Cable delays for 3 selected events\n(outliers above 30.0 ns put at y<0)')

#plt.figure()
#plt.scatter(x[:, 0], yerr)
    
    
    
    
    
    
    
    
    
    
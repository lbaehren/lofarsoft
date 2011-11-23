"""
Task: Get cable delays by gathering / averaging from a given pipeline 'results' directory. 
======================

Created by Arthur Corstanje, Nov. 2011
"""

import pycrtools as cr
import pycrtools.tasks as tasks
import os
from pycrtools.tasks.shortcuts import *
from pycrtools import metadata as md
import pickle
import numpy as np
import matplotlib.pyplot as plt

def gatherresults(topdir, maxspread):
    """
    Gather fitted cable delay results from all 'results.py' files in the subdirectories of 'topdir'.
    """
    if not topdir:
        return None
    
    results = {}
    
    filelist=cr.listFiles(os.path.join(topdir,'*'))
     
    eventlist=[]
    for file in filelist: # can this be done shorter / in listFiles?
        if os.path.isdir(file):
            eventlist.extend([file])
            
    antid = []
    cabledelays = []
    residualdelays = []
    positions = []
    timestamps = []
    cabledelays_database = dict()
    for eventdir in eventlist:        
        print "Processing event in directory:",eventdir
        if os.path.isdir(os.path.join(eventdir,"pol0")) or os.path.isdir(os.path.join(eventdir, "pol1")):
            datadirs=cr.listFiles(os.path.join(os.path.join(eventdir,"pol?"),"*"))
        else:
            print 'no data dirs, continue'
            continue
        for datadir in datadirs:
            if not os.path.isfile(os.path.join(datadir,"results.py")):
                continue # doesn't matter if 'datadir' isn't a directory...
            resfile = os.path.join(datadir,"results.py")
            res = {}
            try:
                execfile(resfile, res)
            except NameError:
                print 'Warning: NAN found in results.py (Name error)! Skipping dir %s ' % datadir
                continue
            res = res["results"]
            # check result status
    #        if res["status"] != 'OK':
            noutliers = res["delay_outliers"] if "delay_outliers" in res.keys() else 0 
            if res["delay_quality_error"] >= 1 or noutliers > 5: # have status criterion in pipeline...
                print 'File status not OK, dir = %s, delay quality = %s, # outliers = %s' % (datadir, res["delay_quality_error"], noutliers)
                continue
            # read in antenna positions from file
            positions.extend(res["antenna_positions_array_XYZ_m"]) # in flat list
            antid.extend([str(int(v)) for v in res["antennas"].values()])
            # check: same ordering for ant-id's and cable delaysin 'res'??? Relying on that.
            cabledelays.extend(res["antennas_final_cable_delays"])  
            residualdelays.extend(res["antennas_residual_cable_delays"])
            timelist = [res["TIME"]] * len(res["antennas"])
            timestamps.extend(timelist)

    count = dict()
    antennapositions = dict() # will contain positions for each (unique) antenna id
    for i in range(len(antid)):
        # fill cabledelays_database
        if not antid[i] in cabledelays_database.keys():
            cabledelays_database[antid[i]] = dict(cabledelay = 0.0, residualdelay = 0.0, spread = 0.0, delaylist = [], residuallist = [], timelist = [])
            antennapositions[antid[i]] = positions[i * 3: (i+1)*3]
        cabledelays_database[antid[i]]["delaylist"].extend([cabledelays[i]])
        cabledelays_database[antid[i]]["residuallist"].extend([residualdelays[i]])
        cabledelays_database[antid[i]]["timelist"].extend([timestamps[i]])

    # get averages of cable delays. Zero out if spread too high.
    for key in cabledelays_database:
        theseDelays = cabledelays_database[key]["delaylist"]
        theseDelays = np.array(theseDelays)
        theseResiduals = np.array(cabledelays_database[key]["residuallist"])
    #    theseDelays[np.where(abs(theseDelays) > 20.0)] = float('nan')
        avg = theseDelays.mean()
        residualavg = theseResiduals.mean()
        spread = theseDelays.std()

        if spread > maxspread * 1.0e-9: # parameter!
            avg = 0.0 # remove inconsistent fit values (improve?)
            residualavg = 0.0 
        cabledelays_database[key]["cabledelay"] = avg
        cabledelays_database[key]["residualdelay"] = residualavg
        cabledelays_database[key]["spread"] = spread

    names = []   
    positions = [] # redo once for every id
    for id in antennapositions.keys(): # get unique ids
        names.append(str(int(id) % 100)) # NB. Append needed rather than extend, as we want to put a string into the list...
        positions.extend(antennapositions[id]) # extend list items into positions list
        
    nofantennas = len(antennapositions.keys())
    results["positions"] = cr.hArray(dimensions=[nofantennas, 3], fill=positions)
    results["names"] = cr.hArray(Type=str, dimensions=[nofantennas], fill=names)
    results["cabledelays_database"] = cabledelays_database
    results["antid"] = cr.hArray(Type=str, dimensions=[nofantennas], fill=antennapositions.keys())
    
    return results

def obtainvalue(par,key):
    """This function returns the value from a parameter dict or a default value if the key does not exist"""
    defaultvalues=dict(
        title=False,
        loracore=None,
        positions=None,
        antid=None,
        loradirection=None,
        names=False
    )

    if not par:
        if key in defaultvalues.keys():
            return defaultvalues[key]
        else: 
            print "please add ",key,"to defaults in cabledelays.py / obtainvalue "
            return None
    else:
        if key in par.keys():
            return par[key]
        else:
            if key in defaultvalues.keys():
                return defaultvalues[key]
            else: 
                print "please add ",key,"to defaults in cabledelays.py / obtainvalue "
                return None

class cabledelays(tasks.Task):
    """
    **Description:**

    Get cable delays.

    **Usage:**

    **See also:**
    :class:`plotfootprint`

    **Example:**

    ::
        filefilter="/Volumes/Data/sandertv/VHECR/LORAtriggered/results/VHECR_LORA-20110716T094509.665Z/"
        crfootprint=cr.trun("plotfootprint",filefilter=filefilter,pol=polarization)
   """
    parameters=dict(
        topdir={default:None, doc:"Top-level results directory from which to gather cable delays"},
#        filefilter={default:None,doc:"Obtains results from subdirectories of these files (from results.py)"},
        doPlot = {default:False, doc:"Produce output plots"},
        write_database = {default: True, doc: "Produce the Cabledelays.pic output file. Replaces an old one if it is there"},
        maxspread = {default: 5.0, doc: "Maximum spread (stddev) [ns] in fitted cable delays to accept as valid. Invalid delays are set to zero, as the value is considered not reliable."},
#        pol={default:0,doc:"0 or 1 for even or odd polarization"},
##        excludelist={default:None,doc:"List with stations not to take into account when making the footprint"},
        results=p_(lambda self:gatherresults(self.topdir, self.maxspread),doc="Results dict containing cabledelays_database, antenna positions and names"),
        cabledelays_database=p_(lambda self:obtainvalue(self.results, "cabledelays_database"), doc="Cable delays database dict as gathered from all good results.py files in the given file filter"),
        positions=p_(lambda self:obtainvalue(self.results,"positions"),doc="hArray of dimension [NAnt,3] with Cartesian coordinates of the antenna positions (x0,y0,z0,...)"),
        antid = p_(lambda self:obtainvalue(self.results,"antid"), doc="hArray containing strings of antenna ids"),
        names=p_(lambda self:obtainvalue(self.results,"names"),doc="hArray of dimension [NAnt] with the names or IDs of the antennas"),
#        plotnames={default:False,doc:"plot names of dipoles"},
#        title=p_(lambda self:obtainvalue(self.results,"title"),doc="Title for the plot (e.g., event or filename)"),
#        newfigure=p_(True,"Create a new figure for plotting for each new instance of the task."),
        plot_finish={default: lambda self:cr.plotfinish(doplot=True,plotpause=False),doc:"Function to be called after each plot to determine whether to pause or not (see ::func::plotfinish)"},
        plot_name={default:"cabledelays",doc:"Extra name to be added to plot filename."},
#        plotlegend={default:False,doc:"Plot a legend"},
#        positionsT=p_(lambda self:cr.hArray_transpose(self.positions),"hArray with transposed Cartesian coordinates of the antenna positions (x0,x1,...,y0,y1...,z0,z1,....)",unit="m",workarray=True),
        nofantennas=p_(lambda self: self.positions.shape()[-2],"Number of antennas.",output=True),
#        figure={default:None,doc:"No startplot"},
        filetype={default:"png",doc:"extension/type of output file"},
        save_images = {default:False,doc:"Enable if images should be saved to disk in default folder"},
        generate_html = {default:False,doc:"Default output to altair webserver"}
        
        )
                
    def call(self):
        pass

    def run(self):
       
        if self.write_database:
            outfile = open(os.path.join(self.topdir, 'Cabledelays.pic'), 'wb')
            pickle.dump(self.cabledelays_database, outfile)
            outfile.close()
            print 'Cabledelays.pic written'
        if not self.doPlot:
            return

        y = []
        yspread = []
        positions = []
        for id in self.antid: # get unique ids
            thisCableDelay = 1.0e9 * self.cabledelays_database[str(id)]["residualdelay"]
            thisSpread = 1.0e9 * self.cabledelays_database[str(id)]["spread"]
            if abs(thisSpread) > self.maxspread:
                thisCableDelay = 0.0 # kick outliers
            y.extend([thisCableDelay])
            yspread.extend([thisSpread])

        cdelays = cr.hArray(y)
        cdelayspread = np.array(yspread)
        
        cr.trerun("PlotAntennaLayout","Delays",positions = self.positions, colors= cr.hArray(y),sizes=100,names = self.names,title="Cable delays",plotlegend=True)

        # get original cable delays from metadata
        # NB! Is this a correct measure of cable delays???
        #cabledelays_full=cr.metadata.get("CableDelays", fptask.antid, res["ANTENNA_SET"], True)  # Obtain cabledelays
        #cabledelays_full-=cabledelays_full[0] # Correct w.r.t. referecence antenna
        #        cabledelays=cabledelays_full % sample_interval #Only sub-sample correction has not been appliedcabledelays=cabledelays_full % 5e-9  # Only sub-sample correction has not been applied
        #        cabledelays=hArray(cabledelays)


        # now plot cable delays with spread
        # make one figure per LOFAR station, with RCU number on the x-axis.
        
        antennasPerStationID = dict()
        for id in self.antid.val(): # loop over list rather than hArray
            thisStationID = int(id) / 1000000
            if not thisStationID in antennasPerStationID.keys():
                antennasPerStationID[thisStationID] = []
            antennasPerStationID[thisStationID].append(id)
        
        for station in antennasPerStationID:
            theseAntennas = antennasPerStationID[station]
            theseAntennas.sort()   # will sort by RCU number 
            thisStationName = md.idToStationName(station)
            
            plt.figure()
            x = []
            x_avg = []
            y_avg_residual = []
            y_avg_total = []
            y_total = []
            y_residual = []
            n = 0
            plotparameter = "residualdelay"
            for id in theseAntennas:
                thisAnt = self.cabledelays_database[id]
                thisAvg = 1.0e9 * thisAnt["residualdelay"]
                thisTotal = 1.0e9 * thisAnt["cabledelay"]
                thisSpread = 1.0e9 * thisAnt["spread"]
                x_avg.extend([n])
                y_avg_residual.extend([thisAvg])
                y_avg_total.extend([thisTotal])
                if thisSpread < self.maxspread:
                    for k in range(len(thisAnt["residuallist"])): # residual and total lists are the same size...
                        x.extend([n])
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

            plt.scatter(x, y_residual, label='Residual cable delays')
            plt.scatter(x_avg, y_avg_residual, c='r', label='Average residual delay')
            plt.ylabel('Residual cable delay [ns]')
            plt.xlabel('Antenna number (concatenated)')
            plt.title('Residual cable delays per antenna after last pipeline iteration\nStation '+thisStationName)
            plt.legend(loc='best')

            plt.figure()
            plt.scatter(x, y_total, label='Total cable delays')
            plt.scatter(x_avg, y_avg_total, c = 'r', label='Average cable delay')
            plt.ylabel('Total cable delay [ns]')
            plt.xlabel('Antenna number (concatenated)')
            plt.title('Total cable delays per antenna after last pipeline iteration\nStation '+thisStationName)
            plt.legend(loc='center right')

            self.plot_finish(filename=self.plot_name + '-' + thisStationName,filetype=self.filetype)   

        # end for station in antennasPerStationID
        
        
#        if self.title:
#            cr.plt.title(self.title)
  
#        if self.generate_html:
#                status = "new"
#                check_file = open(self.topdir+'index.html', 'r')
#                for line in check_file:
#                    if "footprint_pol"+str(self.pol) in line:
#                        status = "filled"
#                check_file.close()
#                
#                if status == "new":
#                    html_file = open(self.topdir+'index.html','a')
#                    name = "pol"+str(self.pol)+"/pycrfig-0001-"+name_extension+".png"
#                    
#                    html_file.write("\n<a name=\"%s"%name)
#                    html_file.write("\" href=\"%s"%name)
#                    html_file.write("\">%s</a> <br>"%name)
#                    html_file.write("\n<a href=\"%s"%name)
#                    html_file.write("\"><img src=\"%s\" width=800></a><br>"%name)
#                    html_file.close()
            

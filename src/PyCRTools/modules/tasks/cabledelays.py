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
from datetime import datetime


def timeStringNow():
    now = datetime.now()
    return now.strftime("%Y-%m-%d_%H%M%S")


def gatherresults(topdir, maxspread, antennaSet):
    """
    Gather fitted cable delay results from all 'results.py' files in the subdirectories of 'topdir'.
    """
    if not topdir:
        return None

    results = {}

    filelist = cr.listFiles(os.path.join(topdir, '*'))

    eventlist = []  # get all events for this top-dir
    for file in filelist:  # can this be done shorter / in listFiles?
        if os.path.isdir(file):
            eventlist.extend([file])

    antid = []
    cabledelays = []
    residualdelays = []
    positions = []
    timestamps = []
    cabledelays_database = dict()
    outputstring = ""  # make output string summarizing outlier counts etc.
    for eventdir in eventlist:
        print "Processing event in directory:", eventdir
        if os.path.isdir(os.path.join(eventdir, "pol0")) or os.path.isdir(os.path.join(eventdir, "pol1")):
            datadirs = cr.listFiles(os.path.join(os.path.join(eventdir, "pol?"), "*"))
        else:
            print 'no data dirs, continue'
            continue
        for datadir in datadirs:
            if not os.path.isfile(os.path.join(datadir, "results.py")):
                continue  # doesn't matter if 'datadir' isn't a directory...
            resfile = os.path.join(datadir, "results.py")
            res = {}
            try:
                execfile(resfile, res)
            except NameError:
                print 'Warning: NAN found in results.py (Name error)! Skipping dir %s ' % datadir
                continue
            res = res["results"]
            if res["ANTENNA_SET"] != antennaSet:
                print 'Skipping file in dir %s, wrong antenna set %s' % (datadir, res["ANTENNA_SET"])
                continue
            print 'Antenna set: %s ' % res["ANTENNA_SET"]
            # check result status
    #        if res["status"] != 'OK':
            # A 'status' keyword would be nice... Number of delay outliers above (say) 10 ns is in 'delay_outliers' key.
            noutliers = res["delay_outliers"] if "delay_outliers" in res.keys() else 0
            print '# outliers = %d' % noutliers
            if res["delay_quality_error"] >= 1 or noutliers > 5:  # have status criterion in pipeline...
                print 'File status not OK, dir = %s, delay quality = %s, # outliers = %s' % (datadir, res["delay_quality_error"], noutliers)
                continue
            # read in antenna positions from file
            positions.extend(res["antenna_positions_array_XYZ_m"])  # in flat list
            nof_new_antids = len([str(int(v)) for v in res["antennas"]])
            nof_new_cabledelays = len(res["antennas_residual_cable_delays_planewave"])
            nof_new_triangledelays = len(res["antennas_residual_cable_delays"])

           # print '*** New ant-ids: %d' % nof_new_antids
           # print '*** New cable delays: %d' % nof_new_cabledelays
           # print '*** New triangle delays: %d' % nof_new_triangledelays
            if nof_new_cabledelays != nof_new_antids:
                print '*** ERROR: WRONG NUMBER OF ANTENNAS!'
            theseAntIDs = [str(int(v)) for v in res["antennas"].values()]
            antid.extend(theseAntIDs)  # res["antennas"] is a dict!
            # check: same ordering for ant-id's and cable delays in 'res'??? Relying on that.
            cabledelays.extend(res["antennas_residual_cable_delays_planewave"])  # antennas_final_cable_delays
            residualdelays.extend(res["antennas_residual_cable_delays_planewave"])
            # test for spread and outliers
            theseDelays = np.array(res["antennas_residual_cable_delays_planewave"])
            avg = theseDelays.mean()
            spread = theseDelays.std() * 1.0e9
            outlierIndices = np.where(abs(theseDelays) > 3 * spread * 1.0e-9)  # 3-sigma outliers output to screen
            outliercount = len(outlierIndices[0])
            if outliercount > 0:
                outlierList = np.array(theseAntIDs)[outlierIndices]
                outlierList = np.array([int(id) % 100 for id in outlierList])
                outlierValues = theseDelays[outlierIndices] * 1.0e9
                outputstring += format('File %s has %d outliers, in RCUs: %s, values = %s' % (datadir, outliercount, str(outlierList), str(outlierValues))) + '\n'
#            print theseDelays
#            print 'length of array = %d' % len(theseDelays)
#            print 'Spread = %3.3f ns' % spread
#            print '# outliers for this event = %d' % outliercount

            timelist = [res["TIME"]] * len(res["antennas"])
            timestamps.extend(timelist)

    print len(cabledelays)
    print len(antid)
    print outputstring
#    import pdb; pdb.set_trace()
    count = dict()
    antennapositions = dict()  # will contain positions for each (unique) antenna id
    for i in range(len(antid)):
        # fill cabledelays_database and antennapositions dict
        if not antid[i] in cabledelays_database.keys():
            cabledelays_database[antid[i]] = dict(cabledelay=0.0, residualdelay=0.0, spread=0.0, delaylist=[], residuallist=[], timelist=[])
            antennapositions[antid[i]] = positions[i * 3: (i + 1) * 3]
#        print "%d" % i
        if cabledelays[i] > -9998.0:  # check if the value is valid, i.e. was actually calculated for this antenna
            cabledelays_database[antid[i]]["delaylist"].extend([cabledelays[i]])  # cabledelays[i] is a number; same antid occurs more than once, so the delaylist will grow...
            cabledelays_database[antid[i]]["residuallist"].extend([residualdelays[i]])
            cabledelays_database[antid[i]]["timelist"].extend([timestamps[i]])

    # get averages of cable delays. Zero out if stddev spread too high. Better use zero than a (probably) wrong value (?)
    for key in cabledelays_database:
        theseDelays = cabledelays_database[key]["delaylist"]
        theseDelays = np.array(theseDelays)
        theseResiduals = np.array(cabledelays_database[key]["residuallist"])
    #    theseDelays[np.where(abs(theseDelays) > 20.0)] = float('nan')
        # remove outliers by kicking out everything above 10.0 ns abs.
#        goodindices = np.where(abs(theseDelays) < 10.0e-9)
#        theseDelays = theseDelays[goodindices]
#        goodindices = np.where(abs(theseResiduals) < 10.0e-9)
#        theseResiduals = theseResiduals[goodindices]

        avg = theseDelays.mean()
        residualavg = theseResiduals.mean()
        spread = theseDelays.std()

        if spread > maxspread * 1.0e-9:  # parameter!
            avg = float('nan')  # remove inconsistent fit values (improve?)
            residualavg = float('nan')
            spread = 0.0
        cabledelays_database[key]["cabledelay"] = avg
        cabledelays_database[key]["residualdelay"] = residualavg
        cabledelays_database[key]["spread"] = spread

    names = []
    positions = []  # redo once for every id
    for id in antennapositions.keys():  # get unique ids
        names.append(str(int(id) % 100))  # NB. Append needed rather than extend, as we want to put a string into the list...
        positions.extend(antennapositions[id])  # extend list items into positions list

    nofantennas = len(antennapositions.keys())
    results["positions"] = cr.hArray(dimensions=[nofantennas, 3], fill=positions)
    results["names"] = cr.hArray(Type=str, dimensions=[nofantennas], fill=names)
    results["cabledelays_database"] = cabledelays_database
    results["antid"] = cr.hArray(Type=str, dimensions=[nofantennas], fill=antennapositions.keys())

    return results


def obtainvalue(par, key):
    """This function returns the value from a parameter dict or a default value if the key does not exist"""
    defaultvalues = dict(
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
            print "please add ", key, "to defaults in cabledelays.py / obtainvalue "
            return None
    else:
        if key in par.keys():
            return par[key]
        else:
            if key in defaultvalues.keys():
                return defaultvalues[key]
            else:
                print "please add ", key, "to defaults in cabledelays.py / obtainvalue "
                return None


def getDelaysFromPhaseCalibrationTables(stationName, theseAntennas):

    RCUlist = []  # get the RCU list we have in 'theseAntennas'
    for id in theseAntennas:
        thisRCU = int(id) % 100
        RCUlist.extend([thisRCU])
    RCUlist = np.array(RCUlist)

    phasecal = md.getStationPhaseCalibration(stationName, "LBA_OUTER")  # NB! Antennaset as parameter...
    phases = np.angle(phasecal)
    delays = (phases[:, 1] - phases[:, 0]) * (1024 / (2 * np.pi)) * 5.0e-9  # delay in seconds, 5.0 ns = sample time

    cabledelays_full = cr.metadata.getCableDelays(stationName, "LBA_OUTER")  # Obtain cabledelays

    cabledelays = cabledelays_full % 5.0e-9  # Only sub-sample correction has not been appliedcabledelays=cabledelays_full % 5e-9  # Only sub-sample correction has not been applied
    cabledelays -= cabledelays[0]  # Correct w.r.t. referecence antenna

#    delays += cabledelays
    return (delays[RCUlist], cabledelays[RCUlist])   # take only 'theseAntennas' which are present in the cabledelays_database


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
    parameters = dict(
        topdir=dict(default=None,
                    doc="Top-level results directory from which to gather cable delays"),

        # filefilter=dict(default:None,
        #                 doc:"Obtains results from subdirectories of these files (from results.py)"),

        doPlot = dict(default=False,
                      doc="Produce output plots"),

        antennaSet = dict(default="LBA_OUTER",
                          doc="Antenna set to use. Note: we can use only one antenna set at a time!"),

        write_database = dict(default=True,
                              doc="Produce delay calibration output file. It is merged with the original delays as found in 'origdelayfile'."),

        origdelayfile = dict(default=lambda self: os.environ["LOFARSOFT"].rstrip('/') + '/data/lofar/dipole_calibration_delay/LOFAR_DIPOLE_CALIBRATION_DELAY_' + self.antennaSet,
                             doc="Location of the original (LOFAR) calibration delay file. Must match with the antennaSet parameter."),

        delayfile = dict(default=lambda self: os.environ["LOFARSOFT"].rstrip('/') + '/data/lofar/merged_dipole_calibration_delay_' + timeStringNow() + '/LOFAR_DIPOLE_CALIBRATION_DELAY_' + self.antennaSet, doc="Output file for the new (merged) delay calibration"),

        maxspread = dict(default=5.0,
                         doc="Maximum spread (stddev) [ns] in fitted cable delays to accept as valid. Invalid delays are set to zero, as the value is considered not reliable."),

        # pol = dict(default=0,
        #            doc="0 or 1 for even or odd polarization"),

        # excludelist = dict(default:None,
        #                    doc:"List with stations not to take into account when making the footprint"),

        results = dict(default=lambda self: gatherresults(self.topdir, self.maxspread, self.antennaSet),
                       doc="Results dict containing cabledelays_database, antenna positions and names"),

        cabledelays_database = dict(default=lambda self: obtainvalue(self.results, "cabledelays_database"),
                                    doc="Cable delays database dict as gathered from all good results.py files in the given file filter"),

        positions = dict(default=lambda self: obtainvalue(self.results, "positions"),
                         doc="hArray of dimension [NAnt,3] with Cartesian coordinates of the antenna positions (x0,y0,z0,...)"),

        antid = dict(default=lambda self: obtainvalue(self.results, "antid"),
                     doc="hArray containing strings of antenna ids"),

        names = dict(default=lambda self: obtainvalue(self.results, "names"),
                     doc="hArray of dimension [NAnt] with the names or IDs of the antennas"),

        # plotnames = dict(default:False,
        #                  doc:"plot names of dipoles"),

        # title = dict(default=lambda self:obtainvalue(self.results,"title"),
        #              doc="Title for the plot (e.g., event or filename)"),

        # newfigure = dict(default=True,
        #                  doc="Create a new figure for plotting for each new instance of the task."),

        plot_finish = dict(default=lambda self: cr.plotfinish(doplot=True, filename="cabledelays", plotpause=False),
                           doc="Function to be called after each plot to determine whether to pause or not (see ::func::plotfinish)"),

        plot_name = dict(default="cabledelays",
                         doc="Extra name to be added to plot filename."),

        # plotlegend = dict(default=False,
        #                   doc="Plot a legend"),

        # positionsT = dict(default=lambda self:cr.hArray_transpose(self.positions),
        #                   doc="hArray with transposed Cartesian coordinates of the antenna positions (x0,x1,...,y0,y1...,z0,z1,....)",unit="m",workarray=True),

        nofantennas = dict(default=lambda self: self.positions.shape()[-2],
                           doc="Number of antennas.",
                           output=True),

        # figure = dict(default=None,
        #               doc="No startplot"),

        filetype = dict(default="png",
                        doc="extension/type of output file"),

        save_images = dict(default=False,
                           doc="Enable if images should be saved to disk in default folder"),

        generate_html = dict(default=False,
                             doc="Default output to altair webserver")

        )

    def call(self):
        pass

    def writeDatabase(self, origfile, filename):
        # create output directory
        (outpath, outfilename) = os.path.split(filename)
        if not os.path.exists(outpath):
            os.mkdir(outpath)
        else:  # should not happen as the path name includes the time 'now' in seconds...
            print 'Warning: output path already exists: %s' % outpath
        outfile = open(os.path.join(self.topdir, filename), 'w')
        infile = open(origfile, 'r')
        for line in infile:
            thisID = line.split()[0]  # first element is the antenna ID
            thisOrigDelay = float(line.split()[1])
            newDelay = thisOrigDelay  # use original (LOFAR) delay if no correction data present

            if str(int(thisID)) in self.cabledelays_database:  # leading zeros stripped; change in cabledelays_database?
                thisCorrection = self.cabledelays_database[str(int(thisID))]["cabledelay"]
                if not np.isnan(thisCorrection):
                    newDelay += thisCorrection
                else:
                    print 'New value is NaN, using old value: %s %f' % (thisID, thisCorrection)
        #    else:
                # print 'Ant-id NOT updated: %s' % thisID
            s = thisID + ' ' + str(newDelay) + '\n'
            outfile.write(s)

        outfile.close()

    def run(self):

        if not self.doPlot and not self.write_database:
            print 'WARNING: cabledelays Task: nothing to do! Provide write_database or doPlot flag.'
            return

        if self.write_database:
            self.writeDatabase(self.origdelayfile, self.delayfile)
#            outfile = open(os.path.join(self.topdir, 'Cabledelays.pic'), 'wb')
#            pickle.dump(self.cabledelays_database, outfile)
#            outfile.close()
            print 'Calibration delay file written'

        if not self.doPlot:
            return

        y = []
        yspread = []
        positions = []
        for id in self.antid:  # get unique ids
            thisCableDelay = 1.0e9 * self.cabledelays_database[str(id)]["residualdelay"]
            thisSpread = 1.0e9 * self.cabledelays_database[str(id)]["spread"]
            if abs(thisSpread) > self.maxspread:
                thisCableDelay = 0.0  # kick outliers
            y.extend([thisCableDelay])
            yspread.extend([thisSpread])

        cdelays = cr.hArray(y)
        cdelayspread = np.array(yspread)

        cr.trerun("PlotAntennaLayout", "Delays", positions=self.positions, colors=cr.hArray(y), sizes=100, names=self.names, title="Cable delays", plotlegend=True)

        # now plot cable delays with spread
        # make one figure per LOFAR station, with RCU number on the x-axis.

        antennasPerStationID = dict()
        for id in self.antid.val():  # loop over list rather than hArray
            thisStationID = int(id) / 1000000
#            print thisStationID
#            print id
            if not thisStationID in antennasPerStationID.keys():
                antennasPerStationID[thisStationID] = []
            antennasPerStationID[thisStationID].append(id)

        for station in antennasPerStationID:
            theseAntennas = antennasPerStationID[station]  # antenna IDs
            theseAntennas.sort()   # will sort by RCU number
            thisStationName = md.idToStationName(station)

            (delayfromphasetables, delayfromcabledelay) = getDelaysFromPhaseCalibrationTables(thisStationName, theseAntennas)
            delayfromcabledelay *= 1.0e9
            delayfromphasetables *= 1.0e9
            plt.figure()
            x = []
            x_avg = []
            y_avg_residual = []
            y_err = []
            y_avg_total = []
            y_total = []
            y_residual = []
            n = 0
            plotparameter = "residualdelay"
           # if station == 7:
           #     import pdb; pdb.set_trace()
            for id in theseAntennas:
                thisAnt = self.cabledelays_database[id]
                thisAvg = 1.0e9 * thisAnt["residualdelay"]
                thisTotal = 1.0e9 * thisAnt["cabledelay"]
                thisSpread = 1.0e9 * thisAnt["spread"]
                thisN = len(thisAnt["delaylist"])
                thisRCUNumber = int(id) % 100
                x_avg.extend([thisRCUNumber])  # the actual RCU number, not the n'th value we process...
                y_avg_residual.extend([thisAvg])

                y_err.extend([thisSpread / np.sqrt(thisN)])  # / np.sqrt(thisN)

                y_avg_total.extend([thisTotal])
                if thisSpread < self.maxspread:
                    for k in range(len(thisAnt["residuallist"])):  # residual and total lists are the same size...
                        x.extend([thisRCUNumber])
                        thisResidualDelay = 1.0e9 * thisAnt["residuallist"][k]
                        thisTotalDelay = 1.0e9 * thisAnt["delaylist"][k]
            #            thisDiff = thisDelay - thisAvg
            #            y.extend([thisDelay])
                        y_residual.extend([thisResidualDelay])
                        y_total.extend([thisTotalDelay])
                #        y[n, k] = (30.0 * k + thisDelay) if thisDelay < 30.0 else (-10.0 + k)# remove outliers
                #        ydiff[n, k] = (0.0 * k + thisDiff) #if thisDiff < 30.0 else (30.0 + k)
                #    yerr[n] = thisAnt["spread"]
                n += 1

            plt.scatter(x, y_residual, label='Individual events')
            plt.scatter(x_avg, y_avg_residual, c='r', label='Average residual delay')
            plt.ylabel('Residual cable delay [ns]')
            plt.xlabel('Antenna number (RCU)')
            plt.title('Residual cable delays per antenna after last pipeline iteration\nStation ' + thisStationName)
            plt.legend(loc='best')

            self.plot_finish(name=thisStationName + "-residuals_vs_RCU")

            plt.figure()
            plt.errorbar(x_avg, y_avg_residual, yerr=y_err, fmt='ro')
            plt.ylabel('Residual cable delay [ns]')
            plt.xlabel('Antenna number (RCU)')
            plt.title('Residual cable delays per antenna\nStation ' + thisStationName)

            self.plot_finish(name=thisStationName + "-residuals_errorbars")

            plt.figure()
#            plt.scatter(x, y_total, label='Individual events')
            plt.scatter(x_avg, y_avg_total, c='r', label='Average cable delay')
            plt.scatter(x_avg, delayfromphasetables, c='g', label='Delay from LOFAR Caltables')
#                plt.scatter(x_avg, delayfromcabledelay, c = 'm', label = 'Delay from Cable delay table')

            plt.ylabel('Total cable delay [ns]')
            plt.xlabel('Antenna number (RCU)')
            plt.title('Total cable delays per antenna after last pipeline iteration\nStation ' + thisStationName)
            plt.legend(loc='center right')

#            self.plot_finish(name="residualvsRCU")

            plt.figure()
            plt.scatter(x_avg, y_avg_total - delayfromphasetables, c='r', label='Difference: fit vs Caltables')
            plt.ylabel('Total cable delay [ns]')
            plt.xlabel('Antenna number (RCU)')
            plt.title('Difference between fitted cable delays and LOFAR Caltables\nStation ' + thisStationName)
            plt.grid()

            plt.figure()  # plot antenna layout of difference: fitted delays - Caltable delays
            # get antenna positions for this station only, for the used antennas only
            thesePositions = md.get('RelativeAntennaPositions', theseAntennas, 'LBA_OUTER', return_as_hArray=True)

#            thisStationsPositions = md.getRelativeAntennaPositions(thisStationName, 'LBA_OUTER', return_as_hArray=True)
            # difference = cr.hArray(y_avg_total - delayfromphasetables)
            difference = cr.hArray(y_avg_residual)
            # hacked LBA_OUTER, remove...
            cr.trerun("PlotAntennaLayout", "Delays", positions=thesePositions, colors=difference, sizes=100,
                      names=self.names, title="Cable delays", plotlegend=True)
            plt.title('Difference between fitted delays and LOFAR Caltables\nStation ' + thisStationName)

            self.plot_finish(filename=self.plot_name + '-' + thisStationName + "-layout", filetype=self.filetype)

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

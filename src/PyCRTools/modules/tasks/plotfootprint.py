"""A tool to plot the footprint of a cosmic ray event with the superterp as background.

"""

import pycrtools as cr
import pycrtools.tasks as tasks
import pycrtools.rftools as rf

from pycrtools.tasks.shortcuts import *
from pycrtools import lora
from pycrtools import xmldict

import numpy as np
import os
import time


def gatherresults(filefilter, pol, excludelist, plotlora, goodonly, loradir):
    """This function returns a dictionary with selected results from file in the subdirectories (/pol?/*/) of the filedir that are needed for the plotfootprint task."""

    if not filefilter:
        return None

    eventdirs = cr.listFiles(filefilter)

    if eventdirs == []:
        print "No events found in given directory"
        assert False

    for eventdir in eventdirs:
        print "Processing event in directory:", eventdir
        par = {}
        antid = {0: [], 1: []}
        signal = {0: [], 1: []}
        positions = {0: [], 1: []}
        ndipoles = {0: 0, 1: 0}
        timelags = {0: [], 1: []}
        datadirs = cr.listFiles(os.path.join(os.path.join(eventdir, "pol?"), "*"))

        for datadir in datadirs:
            if not os.path.isfile(os.path.join(datadir, "results.xml")):
                continue
            if excludelist:
                if True in [st in datadir for st in excludelist]:
                    continue

            res = {}

            try:
                res = xmldict.load(os.path.join(datadir, "results.xml"))

            except Exception:
                continue
                print "File skipped, nan found!"

            try:
                status = res["status"]

            except Exception:
                status = "OK assumed"  # the status has not be propagated for old files

            pulseblock = res["BLOCK"]
            blocksize = res["BLOCKSIZE"]

            theseMaximaX = res["pulses_maxima_x"]  # time of maxima in power of the pulses, in (fractional) samples
            refant = res["pulses_refant"]
            fractionalSampleOffset = np.modf(theseMaximaX[refant])[0]  # fractional part of reference antenna time

            pulseoffset = pulseblock * blocksize + res["SAMPLE_NUMBER"] + res["pulse_start_sample"] + fractionalSampleOffset

            pulseoffset /= res["SAMPLE_FREQUENCY"]
            antset = res["ANTENNA_SET"]
            par["loracore"] = res["pulse_core_lora"]
            par["loradirection"] = res['pulse_direction_lora']

            if goodonly:
                if 'OK' in status:
                    antid[res["polarization"]].extend([int(v) for v in res["antennas"].values()])
                    positions[res["polarization"]].extend(res["antenna_positions_array_XYZ_m"])
                    signal[res["polarization"]].extend(res["pulses_maxima_y"])
                    ndipoles[res["polarization"]] += res["ndipoles"]
                    stationtimelags = [l + pulseoffset for l in res["pulses_timelags_ns"]]
                    timelags[res["polarization"]].extend(stationtimelags)
            else:
                antid[res["polarization"]].extend([int(v) for v in res["antennas"].values()])
                positions[res["polarization"]].extend(res["antenna_positions_array_XYZ_m"])
                signal[res["polarization"]].extend(res["pulses_maxima_y"])
                ndipoles[res["polarization"]] += res["ndipoles"]
                stationtimelags = [l + pulseoffset for l in res["pulses_timelags_ns"]]
                timelags[res["polarization"]].extend(stationtimelags)

    if "TIME" not in res.keys():
        print "Time not found"
        return None

    if plotlora:
        lorainfo = lora.loraInfo(res["TIME"], datadir=loradir, silent=True)
        for k in ["core", "direction", "energy"]:
            par["lora" + k] = lorainfo[k]

        par["loraarrivaltimes"] = cr.hArray(float, dimensions=len(lorainfo["10*nsec"]), fill=lorainfo["10*nsec"]) / 1e10
        par["lorapositions"] = []
        for k in ["posX", "posY", "posZ"]:
            par["lorapositions"].extend(lorainfo[k])
        par["lorapositions"] = cr.hArray(par["lorapositions"], dimensions=[3, len(lorainfo["posX"])])
        par["lorapower"] = cr.hArray(lorainfo["particle_density(/m2)"], dimensions=[len(lorainfo["posX"])])

    if antid[pol] == []:
        print "No good antennas found in this polarization"
        return None

    clockcorrection = cr.metadata.get("ClockCorrection", antid[pol], antset, return_as_hArray=True)
    par["positions"] = cr.hArray(float, [ndipoles[pol], 3], positions[pol], name="Antenna Positions", units="m")
    par["power"] = cr.hArray(signal[pol])
    par["arrivaltime"] = cr.hArray(timelags[pol]) + clockcorrection
    par["arrivaltime"] -= min(par["arrivaltime"])
    par["arrivaltime"] *= 1e9

    timesec = res["TIME"]
    timestr = time.strftime("%Y%m%dT%H%M%S", time.gmtime(timesec))
    timens = str(res["SAMPLE_INTERVAL"] * res["SAMPLE_NUMBER"] * 1000)[0:3]

    par["time"] = timestr
    par["title"] = "Footprint of CR event " + timestr + " (pol " + str(pol) + ")"
    par["names"] = [str(a)[:-6] + "," + str(a)[-3:] for a in antid[pol]]
    par["antid"] = cr.hArray(antid[pol])
    return par


def obtainvalue(par, key):
    """This function returns the value from a parameter dict or a default value if the key does not exist"""
    defaultvalues = dict(
        power=20,
        title=False,
        loracore=None,
        positions=None,
        loraarrivaltimes=None,
        loraenergy=None,
        lorapower=100,
        lorapositions=None,
        arrivaltime='b',
        antid=None,
        loradirection=None,
        names=False,
        event_id=None
    )

    if not par:
        if key in defaultvalues.keys():
            return defaultvalues[key]
        else:
            print "please add ", key, "to defaults in plotfootprint.py / obtainvalue "
            return None
    else:
        if key in par.keys():
            return par[key]
        else:
            if key in defaultvalues.keys():
                return defaultvalues[key]
            else:
                print "please add ", key, "to defaults in plotfootprint.py / obtainvalue "
                return None


class plotfootprint(tasks.Task):
    """
    **Description:**

    Plots the footprint of the air shower on the ground. It will automatically assign colors to the arrivaltimes and scale the size with the signal power. The footprint is only suitable for stations that have seen a signal. To see also other stations set goodonly = False in the options. But note that then neither colors nor size will make any sense. It can also draw the LORA detector either in static or also with time information (plotlora = True, loracolor="time"). You need to have LORA data available on your computer. Without the LORA data only the shower geometry can be shown.

    **Usage:**

    See example.

    **Example:**

    ::
        polarization=0
        filefilter="/data/VHECR/LORAtriggered/LORA/results/VHECR_LORA-20110716T094509.665Z/"
        loradir = "/data/VHECR/LORAtriggered/LORA/"
        crfootprint=cr.trun("plotfootprint",filefilter=filefilter,pol=polarization,loradir=loradir)
   """
    parameters = dict(
        filefilter={default: None, doc: "Obtains results from subdirectories of these files (from results.py)"},
        pol={default: 0, doc: "0 or 1 for even or odd polarization"},
        plotlora={default: True, doc: "Plot the LORA data when positions are present"},
        loradir={default: '/data/VHECR/LORAtriggered/LORA/', doc: "Directory where LORA data files are stored"},
        plotlorashower={default: True, doc: "Plot LORA shower data, when stored in LOFAR results"},
        excludelist={default: None, doc: "List with stations not to take into account when making the footprint"},
        goodonly={default: True, doc: "Enables drawing of only stations that have been reconstructed as good (status OK)"},
        results=p_(lambda self: gatherresults(self.filefilter, self.pol, self.excludelist, self.plotlora, self.goodonly, self.loradir), unit="m", workarray=True),
        positions=p_(lambda self: obtainvalue(self.results, "positions"), doc="hArray of dimension [NAnt,3] with Cartesian coordinates of the antenna positions (x0,y0,z0,...)"),
        size={default: 300, doc: "Size of largest point."},
        sizes_min={default: None, doc: "If set, then use this as the minimum scale for the sizes, when normalizing and plotting."},
        sizes_max={default: None, doc: "If set, then use this as the maximum scale for the sizes, when normalizing."},
        normalize_sizes={default: True, doc: "Normalize the sizes to run from 0-1."},
        normalize_colors={default: False, doc: "Normalize the colors to run from 0-1."},
        power=p_(lambda self: obtainvalue(self.results, "power"), doc="hArray of dimension [NAnt] with the values for the power of each lofar antenna. This gives the sizes of the plot"),
        arrivaltime=p_(lambda self: obtainvalue(self.results, "arrivaltime"), doc="hArray of dimension [NAnt] with the values of relative peak arrival time. This gives the colors of the plot"),
        antid=p_(lambda self: obtainvalue(self.results, "antid"), doc="hArray containing strings of antenna ids"),
        names=p_(lambda self: obtainvalue(self.results, "names"), doc="hArray of dimension [NAnt] with the names or IDs of the antennas"),
        plotnames={default: False, doc: "plot names of dipoles"},
        title=p_(lambda self: obtainvalue(self.results, "title"), doc="Title for the plot (e.g., event or filename)"),
        event_id=p_(lambda self: obtainvalue(self.results, "time"), doc="String formatted time of event"),
        newfigure=p_(True, "Create a new figure for plotting for each new instance of the task."),
        plot_finish={default: lambda self: cr.plotfinish(doplot=True, plotpause=False), doc: "Function to be called after each plot to determine whether to pause or not (see ::func::plotfinish)"},
        plot_name={default: "footprint", doc: "Extra name to be added to plot filename."},
        plotlegend={default: False, doc: "Plot a legend"},
        positionsT=p_(lambda self: cr.hArray_transpose(self.positions), "hArray with transposed Cartesian coordinates of the antenna positions (x0,x1,...,y0,y1...,z0,z1,....)", unit="m", workarray=True),
        NAnt=p_(lambda self: self.positions.shape()[-2], "Number of antennas.", output=True),
        figure={default: None, doc: "No startplot"},
        colormap={default: "autumn", doc: "colormap to use for LOFAR timing"},
        loracore=p_(lambda self: obtainvalue(self.results, "loracore"), doc="Shower core position in hArray(float(X,Y,0))", unit="m"),
        loradirection=p_(lambda self: obtainvalue(self.results, "loradirection"), doc="Shower direction hArray(float,(Azimuth,Elevation)). Azimuth defined from North Eastwards. Elevation defined from horizon up", unit="degrees"),
        lorapositions=p_(lambda self: obtainvalue(self.results, "lorapositions"), doc="3-dim hArray with transposed Cartesian coordinates of the antenna positions (x0,x1,...,y0,y1...,z0,z1,....)", unit="m"),
        lorapower=p_(lambda self: obtainvalue(self.results, "lorapower"), doc="hArray with power of the LORA events"),
        loraarrivaltimes=p_(lambda self: obtainvalue(self.results, "loraarrivaltimes"), doc="hArray with arrival time of LORA events"),
        lorashape={default: "p", doc: "Shape of LORA detectors. e.g. 's' for square, 'p' for pentagram, 'h' for hexagon"},
        lofarshape={default: "o", doc: "Shape of LOFAR antennas. e.g. 'o' for circle, '^' for triangle"},
        loracolor={default: "#730909", doc: "Color used for LORA plots. If set to 'time' uses the arrival time"},
        plotlayout={default: True, doc: "Plot the LOFAR layout of the stations as the background"},
        filetype={default: "png", doc: "extension/type of output file"},
        usecolorbar={default: True, doc: "print the colorbar?"},
        save_images={default: False, doc: "Enable if images should be saved to disk in default folder"},
        generate_html={default: False, doc: "Default output to altair webserver"},


        )

    def call(self):
        pass

    def run(self):

        # Checking for sizes of antenna signals

        if isinstance(self.power, (list)):
            self.power = cr.hArray(self.power)
        if isinstance(self.power, (int, long, float)):
            self.ssizes = self.power
        elif isinstance(self.power, tuple(cr.hRealContainerTypes)):
            self.ssizes = cr.hArray(copy=self.power)
            if "Int" in repr(type(self.ssizes)):
                self.ssizes = cr.hArray(float, dimensions=self.ssizes, fill=self.ssizes)
            if self.normalize_sizes:
                self.ssizes /= self.ssizes.max().val() if self.sizes_max == None else self.sizes_max
                self.ssizes *= self.size
                if self.sizes_min:
                    minindex = cr.hArray(int, dimensions=self.ssizes)
                    nrless = cr.hFindLessThan(minindex, self.ssizes, self.sizes_min).val()
                    cr.hCopy(self.ssizes, cr.hArray(float, dimensions=[nrless], fill=self.sizes_min), minindex, nrless)
        else:
            raise TypeError("PlotAntennaLayout: parameter 'sizes' needs to be a number or an hArray of numbers.")

        # Checking for colors of antenna signals times

        if isinstance(self.arrivaltime, (str, int, long, float)):
            self.scolors = self.arrivaltime
        elif isinstance(self.arrivaltime, tuple(cr.hAllContainerTypes)):
            self.scolors = cr.hArray(copy=self.arrivaltime)
            if self.normalize_colors:
                self.scolors -= self.scolors.min().val()
                self.scolors /= self.scolors.max().val()
        else:
            raise TypeError("PlotAntennaLayout: parameter 'colors' needs to be string or an hArray thereof.")

         # Plotting the LORA detector with all instances
        cr.plt.clf()

        if self.plotlora:
            if isinstance(self.lorapower, (list)):
                self.lorapower = cr.hArray(self.lorapower)
            if isinstance(self.lorapower, (int, long, float)):
                self.lsizes = self.lorapower
                if self.lsizes < self.sizes_min:
                    self.lsizes = self.sizes_min
            elif isinstance(self.lorapower, (list)):
                self.lorapower = cr.hArray(self.lorapower)
            elif isinstance(self.lorapower, tuple(cr.hRealContainerTypes)):
                self.lsizes = cr.hArray(copy=self.lorapower)
                if "Int" in repr(type(self.lsizes)):
                    self.lsizes = cr.hArray(float, dimensions=self.lsizes, fill=self.lsizes)
                if self.normalize_sizes:
                    self.lsizes /= self.lsizes.max().val() if self.sizes_max == None else self.sizes_max
                    self.lsizes *= self.size
                    if self.sizes_min:
                        minindex = cr.hArray(int, dimensions=self.lsizes)
                        nrless = cr.hFindLessThan(minindex, self.lsizes, self.sizes_min).val()
                        cr.hCopy(self.lsizes, cr.hArray(float, dimensions=[nrless], fill=self.sizes_min), minindex, nrless)
            else:
                raise TypeError("PlotAntennaLayout: parameter 'sizes' needs to be a number or an hArray of numbers.")

            if self.lorapositions:
                if self.loracolor is "time":
                    if self.loraarrivaltimes:
                        self.loracolor = self.loraarrivaltimes
                        for i in xrange(len(self.loracolor)):
                            if self.loracolor[i] == 0:
                                self.loracolor[i] += max(self.loracolor)

                        cr.plt.scatter(self.lorapositions[0].vec(), self.lorapositions[1].vec(), s=self.lsizes, c=self.loracolor, marker=self.lorashape, cmap="winter")
                else:
                    cr.plt.scatter(self.lorapositions[0].vec(), self.lorapositions[1].vec(), s=self.lsizes, c=self.loracolor, marker=self.lorashape)

            if self.loracore:

                if type(self.loracolor) is str:
                    cr.plt.scatter(self.loracore[0], self.loracore[1], marker='x', s=600, color=self.loracolor, linewidth=4)
                else:
                    cr.plt.scatter(self.loracore[0], self.loracore[1], marker='x', s=600, color="#151B8D", linewidth=4)

                if self.loradirection:
                    dcos = cr.cos(cr.radians(self.loradirection[0]))
                    dsin = cr.sin(cr.radians(self.loradirection[0]))
                    elev = self.loradirection[1]
                    if type(self.loracolor) is str:
                        cr.plt.arrow(self.loracore[0] + elev * dsin, self.loracore[1] + elev * dcos, -elev * dsin, -elev * dcos, lw=4, color=self.loracolor)
                    else:
                        cr.plt.arrow(self.loracore[0] + elev * dsin, self.loracore[1] + elev * dcos, -elev * dsin, -elev * dcos, lw=4, color="#151B8D")

        elif self.plotlorashower:
            if self.loracore:
                if type(self.loracolor) is str:
                    cr.plt.scatter(self.loracore[0], self.loracore[1], marker='x', s=600, color=self.loracolor, linewidth=4)
                else:
                    cr.plt.scatter(self.loracore[0], self.loracore[1], marker='x', s=600, color="#151B8D", linewidth=4)
                if self.loradirection:
                    dcos = cr.cos(cr.radians(self.loradirection[0]))
                    dsin = cr.sin(cr.radians(self.loradirection[0]))
                    elev = self.loradirection[1]

                    if type(self.loracolor) is str:
                        cr.plt.arrow(self.loracore[0] + elev * dsin, self.loracore[1] + elev * dcos, -elev * dsin, -elev * dcos, lw=4, color=self.loracolor)
                    else:
                        cr.plt.arrow(self.loracore[0] + elev * dsin, self.loracore[1] + elev * dcos, -elev * dsin, -elev * dcos, lw=4, color="#151B8D")

        # Getting the background picture of the superterp
        if self.plotlayout:
            if self.positions:
                from os import environ
                from os.path import isfile
                if "LOFARSOFT" in environ.keys():
                    bgimname = environ["LOFARSOFT"] + "/src/PyCRTools/extras/LORA_layout_background.png"
                    if isfile(bgimname):
                        bgim = cr.plt.imread(bgimname)
                        cr.plt.imshow(bgim, origin='upper', extent=[-375 / 2, 375 / 2, -375 / 2 - 6 * 120 / 227, 375 / 2 - 6 * 120 / 227], alpha=1.0)
                    else:
                        print "WARNING Cannot plot layout. Image file not found. Run an svn update?"
                else:
                    print "WARNING Cannot plot layout. Environment variable LOFARSOFT not found."

        # Plotting the actual signals

        if self.positions:
            cr.plt.scatter(self.positionsT[0].vec(), self.positionsT[1].vec(), s=self.ssizes, c=self.scolors, marker=self.lofarshape, cmap=self.colormap)

        # Further plotting options

            if self.title:
                cr.plt.title(self.title)
            if self.usecolorbar:
                self.cbar = cr.plt.colorbar()
                self.cbar.set_label("Time of arrival (ns)")

            if self.names and self.plotnames:
                for label, x, y in zip(self.names, self.positionsT[0].vec(), self.positionsT[1].vec()):
                    print self.names
                    cr.plt.annotate(str(label), xy=(x, y), xytext=(-3, 3), textcoords='offset points', ha='right', va='bottom')

            cr.plt.xlabel("LOFAR East [meters] ")
            cr.plt.ylabel("LOFAR North [meters] ")

            if self.plotlegend:
                cr.plt.colorbar()
                cr.plt.text(100, -220, "Size denotes signal power")

            if self.filefilter:
                name_extension = "footprint_" + self.event_id + "_pol_" + str(self.pol)
                self.plot_name = self.filefilter + "/" + name_extension
                if self.save_images:

                    cr.plt.savefig(self.plot_name)

            if self.generate_html:
                status = "new"
                check_file = open(self.filefilter + 'index.html', 'r')
                for line in check_file:
                    if "footprint_pol" + str(self.pol) in line:
                        status = "filled"
                check_file.close()

                if status == "new":
                    html_file = open(self.filefilter + 'index.html', 'a')
                    name = "pol" + str(self.pol) + "/pycrfig-0001-" + name_extension + ".png"

                    html_file.write("\n<a name=\"%s" % name)
                    html_file.write("\" href=\"%s" % name)
                    html_file.write("\">%s</a> <br>" % name)
                    html_file.write("\n<a href=\"%s" % name)
                    html_file.write("\"><img src=\"%s\" width=800></a><br>" % name)
                    html_file.close()

            if not self.save_images:
                cr.plt.show()

"""

Task: Lateral Signal Distribution (LDF)
=======================================

.. moduleauthor:: Anna Nelles <a.nelles@astro.ru.nl>

"""

import pycrtools as cr
import pycrtools.tasks as tasks
import pytmf
import time
import os
import sys
import numpy as np
from pycrtools import xmldict

cr.tasks.__raiseTaskDeprecationWarning(__name__)

def GetInformationFromFile(topdir, events, plot_parameter, goodonly):

    eventdirs = cr.listFiles([os.path.join(topdir, event) for event in events])

    for eventdir in eventdirs:

        print "Processing event in directory:", eventdir

        par = {}
        antid = {0: [], 1: []}
        signal = {0: [], 1: []}
        rms = {0: [], 1: []}
        positions = {0: [], 1: []}
        ndipoles = {0: 0, 1: 0}
        meanpos = {0: [], 1: []}
        meansignal = {0: [], 1: []}
        names0 = []
        names1 = []
        res = {}

        datadirs = cr.listFiles(os.path.join(os.path.join(eventdir, "pol?"), "*"))

        for datadir in datadirs:
            if not os.path.isfile(os.path.join(datadir, "results.xml")):
                continue
            # resfile=open(os.path.join(datadir,"results.py"))
            # print "Processing data results directory:",datadir

            try:
                # execfile(os.path.join(datadir,"results.py"),res)
                res = xmldict.load(os.path.join(datadir, "results.xml"))
                # res=res["results"]
            except Exception:
                continue
                print "File skipped, NaN found!"

            try:
                status = res["status"]
            except Exception:
                status = "OK assumed"  # the status has not be propagated for old files

            if goodonly:

                if "OK" in status:
                    antid[res["polarization"]].extend([int(v) for v in res["antennas"]])
                    positions[res["polarization"]].extend(res["antenna_positions_array_XYZ_m"])

                    # Comment: Still to check, which pulse definition most suitable for LDF ploting

                    signal[res["polarization"]].extend(res[plot_parameter])
                    rms[res["polarization"]].extend(res["timeseries_rms"])
                    ndipoles[res["polarization"]] += res["ndipoles"]

                    # Average Station data

                    MPos = cr.hArray(float, [3])
                    MSig = cr.hArray(float, [1])
                    postemp = cr.hArray(float, [res["ndipoles"], 3], res["antenna_positions_array_XYZ_m"])
                    sigtemp = cr.hArray(float, [res["ndipoles"], 1], res[plot_parameter])
                    MPos.mean(postemp)
                    MSig.mean(sigtemp)

                    stationname = res["antennas"][0][0:3]

                    meansignal[res["polarization"]].extend(MSig)
                    meanpos[res["polarization"]].extend(MPos)

                    if res["polarization"] == 0:
                        names0.append(stationname)
                    else:
                        names1.append(stationname)

            else:
                antid[res["polarization"]].extend([int(v) for v in res["antennas"]])
                positions[res["polarization"]].extend(res["antenna_positions_array_XYZ_m"])

            # Comment: Still to check, which pulse definition most suitable for LDF ploting

                signal[res["polarization"]].extend(res[plot_parameter])
                rms[res["polarization"]].extend(res["timeseries_rms"])
                ndipoles[res["polarization"]] += res["ndipoles"]

            # Average Station data

                MPos = cr.hArray(float, [3])
                MSig = cr.hArray(float, [1])
                postemp = cr.hArray(float, [res["ndipoles"], 3], res["antenna_positions_array_XYZ_m"])
                sigtemp = cr.hArray(float, [res["ndipoles"], 1], res[plot_parameter])
                MPos.mean(postemp)
                MSig.mean(sigtemp)

                stationname = res["antennas"][0][0:3]

                meansignal[res["polarization"]].extend(MSig)
                meanpos[res["polarization"]].extend(MPos)

                if res["polarization"] == 0:
                    names0.append(stationname)
                else:
                    names1.append(stationname)

        # print "rms:", rms
        if res == {}:
            print "No results file found"
            sys.exit(0)

        print "Number of dipoles found:", ndipoles

        timesec = res["TIME"]
        timestr = time.strftime("%Y%m%dT%H%M%S", time.gmtime(timesec))

        timens = str(res["SAMPLE_INTERVAL"] * res["SAMPLE_NUMBER"] * 1000)[0:3]
        time_stamp = timestr + "." + timens + "Z"

        par["event_id"] = time_stamp

        par["antenna_set"] = res["ANTENNA_SET"]

        # LORA parameters
        # check for LORA core only being propagated with two coordinates instead of three

        lorcore = cr.hArray(res["pulse_core_lora"])
        Dim = lorcore.shape()
        if 2 in Dim:
            par["loracore"] = cr.hArray([lorcore[0], lorcore[1], 0.0])
        else:
            if 3 in Dim:
                par["loracore"] = lorcore
            else:
                print "Warning, Coreposition of LORA seems to contain the wrong number of coordinates"

        par["loradirection"] = cr.hArray(res["pulse_direction_lora"])
        par["loraenergy"] = res["pulse_energy_lora"]

        # if use ITRF positions, change conversion here

#        pos0=cr.hArray(float,[ndipoles[0],3],positions[0],name="Antenna Positions",units="m")
#        par["positions0"]=cr.metadata.convertITRFToLocal(pos0)
#
#        pos1=cr.hArray(float,[ndipoles[1],3],positions[1],name="Antenna Positions",units="m")
#        par["positions1"]=cr.metadata.convertITRFToLocal(pos1)

        par["positions0"] = cr.hArray(float, [ndipoles[0], 3], positions[0], name="Antenna Positions", units="m")

        par["positions1"] = cr.hArray(float, [ndipoles[1], 3], positions[1], name="Antenna Positions", units="m")

        # check normalization of signal (to be confirmed with respect to signal calibration)

        par["signals1"] = cr.hArray(signal[1])
        par["signals0"] = cr.hArray(signal[0])

        par["signaluncertainties0"] = cr.hArray(rms[0])
        par["signaluncertainties1"] = cr.hArray(rms[1])

        par["meansignals0"] = cr.hArray(meansignal[0])
        par["meansignals1"] = cr.hArray(meansignal[1])

        par["meanpositions0"] = cr.hArray(float, [len(names0), 3], meanpos[0])
        par["meanpositions1"] = cr.hArray(float, [len(names1), 3], meanpos[1])

        par["stationnames0"] = names0
        par["stationnames1"] = names1

    return par


class ldf(tasks.Task):

    """
    This task will use the information given by the LOFAR stations (signal strength) together with information from LORA (shower core and direction) to create a plot showing the dependecy of the signal strength in the antennas on the distance to the shower axis as measured with LORA. In cosmic ray physics this is usually referred to as the lateral distribution or LDF. This only works on reconstructed data.

    It can take into account uncertainties on the signal strength (as soon as provided by the pipeline) as well as uncertainties on the core position and direction. If none of the uncertainties are provided the LDF will be drawn with default errors and an output warning will remind the user.

    Example::

    import pycrtools as cr

    ldf=cr.trun("ldf",topdir="/Users/annanelles/Uni/LOFAR/data/",events=["VHECR_LORA-20110612T235954.184Z"])

    cr.plt.show()

    """

    parameters = dict(
        topdir=dict(default="/data/VHECR/LORAtriggered/results", doc="provide topdir", unit=""),
        eventslist=dict(default=lambda self: self.events if isinstance(self.events, list) else [self.events], doc="list with event names to process (directories in topdir)", unit=""),
        events=dict(default=["VHECR_LORA-20110612T231913.199Z"], doc="Event directories in topdir - either as list or as single string", unit=""),
        results=dict(default=lambda self: GetInformationFromFile(self.topdir, self.eventslist, plot_parameter=self.plot_parameter, goodonly=self.goodonly), doc="Provide task with topdirectory", unit=""),
        positions1=dict(default=lambda self: self.results["positions1"], doc="hArray of dimension [NAnt,3] with Cartesian coordinates of the antenna positions in pol 1 (x0,y0,z0,...)", unit="m"),
        positions0=dict(default=lambda self: self.results["positions0"], doc="hArray of dimension [NAnt,3] with Cartesian coordinates of the antenna positions in pol 0 (x0,y0,z0,...)", unit="m"),
        signals1=dict(default=lambda self: self.results["signals1"], doc="hArray of dimension [NAnt,1] with signals in antennas, pol 1", unit="a.u."),
        signals0=dict(default=lambda self: self.results["signals0"], doc="hArray of dimension [NAnt,1] with signals in antennas, pol 0", unit="a.u."),
        signaluncertainties1=dict(default=lambda self: self.results["signaluncertainties1"], doc="hArray of dimension [NAnt,1], signaluncertainties in pol 1", unit="a.u."),
        signaluncertainties0=dict(default=lambda self: self.results["signaluncertainties0"], doc="hArray of dimension [NAnt,1], signaluncertainties in pol 0", unit="a.u."),
        loracore=dict(default=lambda self: self.results["loracore"], doc="hArray of core position [x,y,z] as provided by Lora ", unit="m"),
        loradirection=dict(default=lambda self: self.results["loradirection"], doc="hArray of shower direction [az, el] as provided by Lora. Az is eastwards from north and El is up from horizon.", unit="degrees"),
        loracoreuncertainties=dict(default=None, doc="hArray of uncertainties of core position [ex,ey,cov]", unit="m"),
        loradirectionuncertainties=dict(default=None, doc="hArray of uncertainties of direction [eAz,eEl,cov]", unit="degrees"),
        loraenergy=dict(default=lambda self: self.results["loraenergy"], unit="eV?"),
        event_id=dict(default=lambda self: self.results["event_id"], doc="EventId for LOFAR Event"),
        square=dict(default=False, doc="Square the data (to get power) before plotting."),
        plot_parameter=dict(default="pulses_maxima_y", doc="Which parameter of the results-dict to plot: 'pulses_strength' or 'pulses_maxima_y'"),
        logplot=dict(default=True, doc="Draw y-axis logarithmically"),
        plot_clf=dict(default=True, doc="Clean window before plotting?"),
        plot_xmin=dict(default=0, doc="Mininum value of x-axis. To use turn plot_auto_scale off."),
        plot_xmax=dict(default=300, doc="Maximum value of x-axis. To use turn plot_auto_scale off."),
        remove_outliers=dict(default=False, doc="enable plotting with useful axes"),
        plot_scale_auto=dict(default=True, doc="Will scales axes to maximum values, will override manual plotting boundaries and whatever plt thinks it is doing."),
        goodonly=dict(default=False, doc="Plotting LDF only with good stations"),
        antenna_set=dict(default=lambda self: self.results["antenna_set"], doc="determines labelling of polarizations"),
        color_pol0=dict(default='#B30424', doc="color pol 0"),
        color_pol1=dict(default='#68C8F7', doc="color pol 1"),
        marker_pol0=dict(default='s'),
        marker_pol1=dict(default='o'),
        meanpositions0=dict(default=lambda self: self.results["meanpositions0"], doc="hArray of dimension [NAnt,3] with Cartesian coordinates of the station positions in pol 0 (x0,y0,z0,...)", unit="m"),
        meanpositions1=dict(default=lambda self: self.results["meanpositions1"], doc="hArray of dimension [NAnt,3] with Cartesian coordinates of the station positions in pol 1 (x0,y0,z0,...)", unit="m"),
        meansignals0=dict(default=lambda self: self.results["meansignals0"], doc="hArray of dimension [NAnt,1] with signals in antennas, pol 0", unit="a.u."),
        meansignals1=dict(default=lambda self: self.results["meansignals1"], doc="hArray of dimension [NAnt,1] with signals in antennas, pol 1", unit="a.u."),
        stationnames0=dict(default=lambda self: self.results["stationnames0"], doc="Stations in run in pol 0."),
        stationnames1=dict(default=lambda self: self.results["stationnames1"], doc="Stations in run in pol 1."),
        draw_global=dict(default=False, doc="Draw position and average signal of a LOFAR station in LDF"),
        CalcHorneffer=dict(default=False, doc="Draw expected field strength from Horneffer parametrization"),
        Draw3D=dict(default=False, doc="Draw 2D LDF"),
        save_images=dict(default=False, doc="Enable if images should be saved to disk in default folder"),
        generate_html=dict(default=False, doc="Default output to altair webserver"),
        use_lofar_information=dict(default=False, doc="Use minimization to draw different LDF"),
        draw_bary_center=dict(default=False, doc="Use barycenter to draw different LDF"),
        use_lofar_with_pol=dict(default=2, doc="Calculate LOFAR core for the polarization 0 or 1 or 2=both"),
        parameters_for_fitting=dict(default=[50, 5], doc="Give minimization parameters [bins,size]"),
        show_azimuth_distribution=dict(default=False, doc="plot for azimuth dependence of signals")

        )

    ## Functions for shower geometry and uncertainty propagation
    def GetDistance(self, loracore, loradirection, positions):

        """
        Calculating the distance of a station to the shower core in the shower plane.
        """

        distances = cr.hArray(copy=positions)
        finaldistance = cr.hArray(copy=positions)

        theta = cr.radians(loradirection[1])           # spherical coordinates: 0 = horizon, 90 = vertical
        phi = cr.radians(450 - loradirection[0])        # recalculate to 0 = east counter-clockwise

        distances = positions - loracore
        axis = cr.hArray([cr.cos(theta) * cr.cos(phi), cr.cos(theta) * cr.sin(phi), cr.sin(theta)])  # shower axis

        finaldistance[...].crossproduct(distances[...], axis)
        dist = cr.hVectorLength(finaldistance[...])

        return dist

    def GetTotalDistanceUncertainty(self, loracore, loracoreuncertainties, positions, loradirection, loradirectionuncertainties, distances):

        """
        Propagating the uncertatinties of the core and the directions into the uncertainties of the distance to the shower axis.

        """

        # Assuming differences in z-coordinates = 0

        dist = distances.vec()

        pos1 = cr.hArray_transpose(positions)[0].vec()
        pos2 = cr.hArray_transpose(positions)[1].vec()

        core1 = float(loracore[0])
        core2 = float(loracore[1])

        phi = cr.radians(450 - loradirection[0])        # recalculate to 0 = east counter-clockwise
        theta = cr.radians(loradirection[1])           # spherical coordinates: 0 = horizon, 90 = vertical

        ecore1 = float(loracoreuncertainties[0])
        ecore2 = float(loracoreuncertainties[1])
        covcore = float(loracoreuncertainties[2])

        ephi = cr.radians(float(loradirectionuncertainties[0]))
        etheta = cr.radians(float(loradirectionuncertainties[1]))
        covangles = float(loradirectionuncertainties[2])

        # short cuts

        cost = cr.cos(theta)
        sint = cr.sin(theta)
        cos2t = cr.cos(theta) ** 2
        sin2t = cr.sin(theta) ** 2
        cosp = cr.cos(phi)
        sinp = cr.sin(phi)
        cos2p = cr.cos(phi) ** 2
        sin2p = cr.sin(phi) ** 2

        m1 = pos1 - core1
        m2 = pos2 - core2

        sqrfac = 0.5 / dist

        # partical derivatives

        diffc1 = -2. * (m1 * sin2t + m1 * cos2t * sin2p - m2 * cos2t * sinp * cosp) * sqrfac
        diffc2 = -2. * (m2 * sin2t + m2 * cos2t * cos2p - m1 * cos2t * sinp * cosp) * sqrfac
        diffp = 2. * (m1 * m1 * cos2t * sinp * cosp - m2 * m2 * cos2t * cosp * sinp - m1 * m2 * cos2t * (cos2p - sin2p)) * sqrfac

        difft = 2. * ((m2 * m2) * sint * cost + (m1 * m1) * sint * cost - (m1 * m1) * sin2p * cost * sint - (m2 * m2) * cos2p * cost * sint + m1 * m2 * sint * cost * sinp * cosp) * sqrfac

        # adding contributions

        err = diffc1 * diffc1 * ecore1 * ecore1
        err = err + diffc2 * diffc2 * ecore2 * ecore2
        err = err + diffp * diffp * ephi * ephi
        err = err + difft * difft * etheta * etheta
        err = err + 2 * difft * diffp * covangles
        err = err + 2 * diffc1 * diffc2 * covcore

        err.sqrt()

        return err

    def CalculateBaryCenter(self, positions, signals):

        """
        Calculating the bary center, i.e. the average coordinate weigthed with the signal strengths.

        """

        pos1 = cr.hArray_transpose(positions)[0].vec()
        pos1 = cr.hArray(pos1)
        pos2 = cr.hArray_transpose(positions)[1].vec()
        pos2 = cr.hArray(pos2)

        SumSig = signals.sum()

        x_val = pos1 * signals
        sum_x = x_val.sum()
        sum_x = sum_x / SumSig

        y_val = pos2 * signals
        sum_y = y_val.sum()
        sum_y = sum_y / SumSig

        core = [sum_x[0], sum_y[0]]

        return core

    def init(self):
        pass

    def call(self):
        pass

    def run(self):

        """
        Making all sorts of things with the LDF. Not fully developed to do the only possible method.

        """

        if self.loracoreuncertainties == None:
            self.loracoreuncertainties = cr.hArray([5., 5., 0.])
            print "Warning: Using default for core uncertainties!"

        if self.loradirectionuncertainties == None:
            self.loradirectionuncertainties = cr.hArray([1., 1., 0])
            print "Warning: Using default for direction uncertainties!"

        self.Distances0 = self.GetDistance(self.loracore, self.loradirection, self.positions0)
        self.Distances1 = self.GetDistance(self.loracore, self.loradirection, self.positions1)

        self.signals0.par.xvalues = cr.hArray(self.Distances0)
        self.signals1.par.xvalues = cr.hArray(self.Distances1)

        if self.draw_global:

            self.stationDistances0 = self.GetDistance(self.loracore, self.loradirection, self.meanpositions0)
            self.stationDistances1 = self.GetDistance(self.loracore, self.loradirection, self.meanpositions1)

            self.meansignals0.par.xvalues = cr.hArray(self.stationDistances0)
            self.meansignals1.par.xvalues = cr.hArray(self.stationDistances1)

        if self.square:
            self.signals0.square()
            self.signals1.square()

        self.DistUncertainties0 = self.GetTotalDistanceUncertainty(self.loracore, self.loracoreuncertainties, self.positions0, self.loradirection, self.loradirectionuncertainties, self.Distances0)
        self.DistUncertainties1 = self.GetTotalDistanceUncertainty(self.loracore, self.loracoreuncertainties, self.positions1, self.loradirection, self.loradirectionuncertainties, self.Distances1)

        if self.plot_clf:
            cr.plt.clf()

        if self.antenna_set == "LBA_OUTER":
            labelpol0 = "Polarization NW-SE"
            labelpol1 = "Polarization NE-SW"
        else:
            labelpol0 = "pol 0"
            labelpol1 = "pol 1"

        if self.signals0:

            if self.remove_outliers:
                for i in xrange(len(self.signals0)):
                    if self.signals0[i] > 100000:
                        self.signals0[i] = 10
                    if self.signals0[i] < 0.001:
                        self.signals0[i] = 1

            if self.plot_scale_auto:
                y_max0 = self.signals0.max()[0] * 1.5
                y_min0 = self.signals0.min()[0] * 0.5

            if self.signaluncertainties0:
                if self.logplot:
                    # exception for too large errorbars in logplot (reaching negative numbers)
                    sig_lower0 = cr.hArray(copy=self.signaluncertainties0)
                    sig_lower0.fill(0.001)
                    cr.hMaximum(sig_lower0, self.signals0 - self.signaluncertainties0)
                    sig_uncer0 = self.signals0 - sig_lower0

                    cr.plt.errorbar(self.Distances0.vec(), self.signals0.vec(), yerr=[sig_uncer0.vec(), self.signaluncertainties0.vec()], xerr=self.DistUncertainties0.vec(), color=self.color_pol0, marker=self.marker_pol0, linestyle="None", label=labelpol0)
                    cr.plt.yscale("log")

                else:
                    cr.plt.errorbar(self.Distances0.vec(), self.signals0.vec(), self.signaluncertainties0.vec(), self.DistUncertainties0.vec(), color=self.color_pol0, marker=self.marker_pol0, linestyle="None", label="pol 0")

            else:
                self.signals0.plot(color='m', linestyle="None", marker=self.marker_pol0, label=labelpol0, clf=False)

        if self.signals1:

            if self.remove_outliers:
                for i in xrange(len(self.signals1)):
                    if self.signals1[i] > 100000:
                        self.signals1[i] = 10
                    if self.signals1[i] < 0.001:
                        self.signals1[i] = 1

            if self.plot_scale_auto:
                y_min = self.signals1.min()[0] * 0.5
                y_max = self.signals1.max()[0] * 1.5
                x_max = self.Distances1.max()

                if y_max < y_max0:
                    y_max = y_max0
                if y_min > y_min0:
                    y_min = y_min0
                if x_max < self.plot_xmax:
                    x_max = self.plot_xmax

            if self.signaluncertainties1:
                if self.logplot:
                    # exception for too large errorbars in logplot (reaching negative numbers)
                    sig_lower1 = cr.hArray(copy=self.signaluncertainties1)
                    sig_lower1.fill(0.00001)
                    cr.hMaximum(sig_lower1, self.signals1 - self.signaluncertainties1)
                    sig_uncer1 = self.signals1 - sig_lower1

                    cr.plt.errorbar(self.Distances1.vec(), self.signals1.vec(), yerr=[sig_uncer1.vec(), self.signaluncertainties1.vec()], xerr=self.DistUncertainties1.vec(), color=self.color_pol1, marker=self.marker_pol1, linestyle="None", label=labelpol1)
                    cr.plt.yscale("log")

                else:
                    cr.plt.errorbar(self.Distances1.vec(), self.signals1.vec(), self.signaluncertainties1.vec(), self.DistUncertainties1.vec(), color='b', marker="s", linestyle="None", label="pol 1")
            else:
                self.signals1.plot(color=self.color_pol1, linestyle="None", marker=self.marker_pol1, label=labelpol1, clf=False)

        cr.plt.legend(loc='upper right', shadow=False, numpoints=1)
        cr.plt.xlabel("Distance to Shower Axis [m]")
        if self.plot_parameter == "pulses_maxima_y":
            cr.plt.ylabel("Pulse Power [a.u.]")
        else:
            cr.plt.ylabel("Power [a.u.]")

        if self.plot_scale_auto:
            cr.plt.xlim(0, x_max)
            cr.plt.ylim(y_min, y_max)
        else:
            cr.plt.xlim(self.plot_xmin, self.plot_xmax)
            cr.plt.ylim(0.01, 10000)

        if self.save_images:
            directory = str(self.topdir) + "/VHECR_LORA-" + str(self.event_id) + "/"
            pic_name = str(self.event_id) + "_LDF.png"
            pic_ldf = directory + pic_name
            cr.plt.savefig(pic_ldf)

            if self.generate_html:
                status = "new"
                check_file = open(directory + 'index.html', 'r')
                for line in check_file:
                    if "LDF" in line:
                        status = "filled"
                check_file.close()

                if status == "new":
                    html_file = open(directory + 'index.html', 'a')

                    html_file.write("\n<a name=\"%s" % pic_name)
                    html_file.write("\" href=\"%s" % pic_name)
                    html_file.write("\">%s</a> <br>" % pic_name)
                    html_file.write("\n<a href=\"%s" % pic_name)
                    html_file.write("\"><img src=\"%s\" width=800></a><br>" % pic_name)
                    html_file.close()

        if self.draw_global:

            self.meansignals1.plot(color=self.color_pol1, marker='h', markersize=10, linestyle="None", clf=False)
            self.meansignals0.plot(color=self.color_pol0, marker='h', linestyle="None", markersize=10, clf=False)
            cr.plt.axis(xmin=self.plot_xmin, xmax=self.plot_xmax)

            for i in xrange(len(self.stationnames0)):
                station = "Station " + str(self.stationnames0[i])
                ycoord = self.meansignals0[i]
                xcoord = self.stationDistances0[i]
                cr.plt.annotate(str(station), xy=(xcoord, ycoord), xytext=(-30, 15), xycoords='data', textcoords='offset points', size='x-large', color=self.color_pol0)

            for i in xrange(len(self.stationnames1)):
                station = "Station " + str(self.stationnames1[i])
                ycoord = self.meansignals1[i]
                xcoord = self.stationDistances1[i]

                cr.plt.annotate(str(station), xy=(xcoord, ycoord), xytext=(-30, 20), xycoords='data', textcoords='offset points', size='x-large', color=self.color_pol1)
            cr.plt.xlabel("Distance to Shower Axis (LORA) [m]")
            if self.plot_parameter == "pulses_maxima_y":
                cr.plt.ylabel("Pulse Power [a.u.]")
            else:
                cr.plt.ylabel("Power [a.u.]")

        if self.event_id:
            cr.plt.title("LOFAR_" + str(self.event_id))

        if self.CalcHorneffer:
            print "Horneffer Formula: Not yet implemented"

        if self.Draw3D:

            from mpl_toolkits.mplot3d import Axes3D
            from matplotlib import cm

            # Polarization 0

            fig0 = cr.plt.figure()
            ax0 = Axes3D(fig0)
            # ax0 = fig0.gca(projection='3d')

            pos0 = cr.hArray(copy=self.positions0)
            posX0 = cr.hArray_transpose(pos0)[0].vec()
            posY0 = cr.hArray_transpose(pos0)[1].vec()

            sig0 = cr.hArray(copy=self.signals0)

            core = cr.hArray(copy=self.loracore)

            posX0 = posX0 - core[0]
            posY0 = posY0 - core[1]

            alpha0 = posY0 / posX0

            alpha0.atan()

            sinalpha0 = cr.hArray(copy=alpha0)
            sinalpha0.sin()
            cosalpha0 = cr.hArray(copy=alpha0)
            cosalpha0.cos()

            Dist0 = cr.hArray(self.Distances0)

            cosalpha0.mul(Dist0)
            sinalpha0.mul(Dist0)

            ax0.scatter(cosalpha0, sinalpha0, sig0, color=self.color_pol0, marker=self.marker_pol0)
            # ax0.contour(cosalpha0, sinalpha0, sig0)

            ax0.set_xlabel('East')
            ax0.set_ylabel('North')
            ax0.w_zaxis.set_scale("log")
            ax0.set_zlabel('Power [a.u.]')

            # Polarization 1

            fig1 = cr.plt.figure()
            ax1 = Axes3D(fig1)

            pos1 = cr.hArray(copy=self.positions1)
            posX1 = cr.hArray_transpose(pos1)[0].vec()
            posY1 = cr.hArray_transpose(pos1)[1].vec()

            sig1 = cr.hArray(copy=self.signals1)

            core = cr.hArray(copy=self.loracore)

            posX1 = posX1 - core[0]
            posY1 = posY1 - core[1]

            alpha1 = posY1 / posX1

            alpha1.atan()

            sinalpha1 = cr.hArray(copy=alpha1)
            sinalpha1.sin()
            cosalpha1 = cr.hArray(copy=alpha1)
            cosalpha1.cos()

            Dist1 = cr.hArray(self.Distances1)

            cosalpha1.mul(Dist1)
            sinalpha1.mul(Dist1)

            ax1.scatter(cosalpha1, sinalpha1, sig1, color=self.color_pol1, marker=self.marker_pol1)

            ax1.set_xlabel('East')
            ax1.set_ylabel('North')
            ax1.w_zaxis.set_scale("log")
            ax1.set_zlabel('Power [a.u.]')

            # Possibly implement errorbars
#
#                #plot errorbars
#                for i in np.arange(0, len(fx)):
#                    ax.plot([fx[i]+xerror[i], fx[i]-xerror[i]], [fy[i], fy[i]], [fz[i], fz[i]], marker="_")
#                    ax.plot([fx[i], fx[i]], [fy[i]+yerror[i], fy[i]-yerror[i]], [fz[i], fz[i]], marker="_")
#                    ax.plot([fx[i], fx[i]], [fy[i], fy[i]], [fz[i]+zerror[i], fz[i]-zerror[i]], marker="_")
        if self.use_lofar_information:

             # Barycenter

            self.CorePol0 = self.CalculateBaryCenter(self.positions0, self.signals0)
            self.CorePol0.append(0)
            self.CorePol0 = cr.hArray(self.CorePol0)
            self.CorePol1 = self.CalculateBaryCenter(self.positions1, self.signals1)
            self.CorePol1.append(0)
            self.CorePol1 = cr.hArray(self.CorePol1)

             ## pol 0

#            figlofarpol0 = cr.plt.figure()
#
#            self.DistNewpol01 = self.GetDistance(self.CorePol0,self.loradirection,self.positions1)
#            self.DistNewpol00 = self.GetDistance(self.CorePol0,self.loradirection,self.positions0)
#
#            cr.plt.plot(self.DistNewpol01.vec(),self.signals1.vec(),color=self.color_pol1,linestyle="None",marker=self.marker_pol1,label=labelpol1)
#
#            cr.plt.plot(self.DistNewpol00.vec(),self.signals0.vec(),color=self.color_pol0,linestyle="None",marker=self.marker_pol0,label=labelpol0)
#            cr.plt.yscale("log")
#            cr.plt.xlabel("Distance to Shower Axis from pol0 [m]")
#            if self.plot_parameter == "pulses_maxima_y":
#                cr.plt.ylabel("Pulse Power [a.u.]")
#            else:
#                cr.plt.ylabel("Power [a.u.]")
#            cr.plt.legend(loc='upper right', shadow=False, numpoints=1)
#            cr.plt.title("BaryCenter_POL0_"+str(self.event_id))
#
#            if self.plot_scale_auto:
#                cr.plt.axis(xmin=0,xmax=x_max,ymin=y_min,ymax=y_max)
#            else:
#                cr.plt.axis(xmin=self.plot_xmin,xmax=self.plot_xmax)

            ## pol1

#            figlofarpol1 = cr.plt.figure()
#
#            self.DistNewpol11 = self.GetDistance(self.CorePol1,self.loradirection,self.positions1)
#            self.DistNewpol10 = self.GetDistance(self.CorePol1,self.loradirection,self.positions0)
#
#            cr.plt.plot(self.DistNewpol11.vec(),self.signals1.vec(),color=self.color_pol1,linestyle="None",marker=self.marker_pol1,label=labelpol1)
#
#            cr.plt.plot(self.DistNewpol10.vec(),self.signals0.vec(),color=self.color_pol0,linestyle="None",marker=self.marker_pol0,label=labelpol0)
#            cr.plt.yscale("log")
#            cr.plt.xlabel("Distance to Shower Axis from pol1 [m]")
#            if self.plot_parameter == "pulses_maxima_y":
#                cr.plt.ylabel("Pulse Power [a.u.]")
#            else:
#                cr.plt.ylabel("Power [a.u.]")
#            cr.plt.legend(loc='upper right', shadow=False, numpoints=1)
#            cr.plt.title("BaryCenter_POL1_"+str(self.event_id))
#
#            if self.plot_scale_auto:
#                cr.plt.axis(xmin=0,xmax=x_max,ymin=y_min,ymax=y_max)
#            else:
#                cr.plt.axis(xmin=self.plot_xmin,xmax=self.plot_xmax)

            # Minimization

            StartDirection = self.loradirection

            size = self.parameters_for_fitting[1]
            bins = self.parameters_for_fitting[0]
            count = 0

            xgrid = np.zeros(bins * (bins))
            ygrid = np.zeros(bins * (bins))

            vgrid = np.zeros(bins * (bins))

            width = bins / 2. * size

            # use both polarizations

            if self.use_lofar_with_pol == 2:

                StartSignals = self.signals1
                NextSignals = self.signals0

                StartCore = cr.hArray(copy=self.loracore)

                StartCore[0] -= (bins / 2 * size)
                StartCore[1] -= (bins / 2 * size)

                MinCore = cr.hArray(copy=StartCore)
                starttestindex = float("inf")

                uncertainty1 = self.signaluncertainties1.mean()
                uncertainty0 = self.signaluncertainties0.mean()

                uncertainty = 3 * (uncertainty1 + uncertainty0 / 2.)

                for stepX in xrange(bins):

                    VarCore = cr.hArray((0., 0., 0.))
                    VarCore[0] = StartCore[0] + stepX * size

                    for stepY in xrange(bins):

                        VarCore[1] = StartCore[1] + stepY * size

                        if self.use_lofar_with_pol == 1:
                            StartDistances = self.GetDistance(VarCore, StartDirection, self.positions1).vec()
                            NextDistances = self.GetDistance(VarCore, StartDirection, self.positions0).vec()
                        else:
                            StartDistances = self.GetDistance(VarCore, StartDirection, self.positions0).vec()
                            NextDistances = self.GetDistance(VarCore, StartDirection, self.positions1).vec()

                        Length = int(StartSignals.shape()[0])
                        Value = np.zeros((Length, 2))

                        Length2 = int(NextSignals.shape()[0])
                        Value2 = np.zeros((Length2, 2))

                        Value[:, 0] = StartDistances
                        Value[:, 1] = StartSignals.vec()

                        Value2[:, 0] = NextDistances
                        Value2[:, 1] = NextSignals.vec()

                        order = Value[:, 0].argsort()
                        order2 = Value2[:, 0].argsort()

                        ValueSorted = np.take(Value, order, 0)

                        ValueSorted2 = np.take(Value2, order2, 0)

                        testvalue1 = 0
                        for i in xrange(Length - 1):
                            up = ValueSorted[i, 1]
                            down = ValueSorted[i + 1, 1]
                            if up > uncertainty and down > uncertainty:
                                testvalue1 += abs(up - down)

                        testvalue2 = 0
                        for i in xrange(Length2 - 1):
                            up = ValueSorted2[i, 1]
                            down = ValueSorted2[i + 1, 1]
                            if up > uncertainty and down > uncertainty:
                                testvalue2 += abs(up - down)

                        testvalue = testvalue2 + testvalue1

                        if testvalue < starttestindex:
                            MinCore = cr.hArray(copy=VarCore)
                            starttestindex = testvalue

                        xgrid[count] = stepX * size + StartCore[0]
                        ygrid[count] = stepY * size + StartCore[1]
                        vgrid[count] = testvalue
                        count += 1

            else:
                if self.use_lofar_with_pol == 1:
                    StartSignals = self.signals1
                else:
                    StartSignals = self.signals0

                StartCore = cr.hArray(copy=self.loracore)

                StartCore[0] -= (bins / 2 * size)
                StartCore[1] -= (bins / 2 * size)

                MinCore = cr.hArray(copy=StartCore)
                starttestindex = float("inf")

                if self.use_lofar_with_pol == 1:
                    uncertainty = self.signaluncertainties1.mean()
                else:
                    uncertainty = self.signaluncertainties1.mean()

                uncertainty *= 3

                for stepX in xrange(bins):

                    VarCore = cr.hArray((0., 0., 0.))
                    VarCore[0] = StartCore[0] + stepX * size

                    for stepY in xrange(bins):

                        VarCore[1] = StartCore[1] + stepY * size

                        if self.use_lofar_with_pol == 1:
                            StartDistances = self.GetDistance(VarCore, StartDirection, self.positions1).vec()
    #                        NextDistances  = self.GetDistance(VarCore,StartDirection,self.positions0).vec()
                        else:
                            StartDistances = self.GetDistance(VarCore, StartDirection, self.positions0).vec()
    #                        NextDistances  = self.GetDistance(VarCore,StartDirection,self.positions1).vec()

                        Length = int(StartSignals.shape()[0])
                        Value = np.zeros((Length, 2))

    #                    Length2 = int(NextSignals.shape()[0])
    #                    Value2 = np.zeros((Length2, 2))

                        Value[:, 0] = StartDistances
                        Value[:, 1] = StartSignals.vec()

    #                    Value2[:,0] = NextDistances
    #                    Value2[:,1] = NextSignals.vec()

                        order = Value[:, 0].argsort()
    #                    order2 = Value2[:,0].argsort()
    #
                        ValueSorted = np.take(Value, order, 0)

    #                    ValueSorted2 = np.take(Value2,order2,0)

                        testvalue1 = 0
                        for i in xrange(Length - 1):
                            up = ValueSorted[i, 1]
                            down = ValueSorted[i + 1, 1]
                            if up > uncertainty and down > uncertainty:
                                testvalue1 += abs(up - down)

    #                    testvalue2 = 0
    #                    for i in xrange(Length2-1):
    #                        up = ValueSorted2[i,1]
    #                        down = ValueSorted2[i+1,1]
    #                        if up  > uncertainty and  down > uncertainty:
    #                            testvalue2 += abs(up-down)
                        testvalue = testvalue1  # +  testvalue1

                        if testvalue < starttestindex:
                            MinCore = cr.hArray(copy=VarCore)
                            starttestindex = testvalue

                        xgrid[count] = stepX * size + StartCore[0]
                        ygrid[count] = stepY * size + StartCore[1]
                        vgrid[count] = testvalue
                        count += 1

            H, xedges, yedges = np.histogram2d(ygrid, xgrid, bins=(bins, bins), range=np.array([[StartCore[1], StartCore[1] + bins * size], [StartCore[0], StartCore[0] + bins * size]]), normed=False, weights=vgrid)

            extent = [yedges[0], yedges[-1], xedges[0], xedges[-1]]

            histo = cr.plt.figure()

            cr.plt.imshow(H, extent=extent, origin="lower", interpolation='nearest', cmap='jet')
            cr.plt.ylabel("LOFAR Y [m]")
            cr.plt.xlabel("LOFAR X [m]")
            cr.plt.colorbar()
            cr.plt.scatter(self.loracore[0], self.loracore[1], s=80, c="black", marker='d')
            cr.plt.scatter(MinCore[0], MinCore[1], s=80, c="black", marker='8')
            cr.plt.scatter(self.CorePol0[0], self.CorePol0[1], s=80, c="black", marker='>')
            cr.plt.scatter(self.CorePol1[0], self.CorePol1[1], s=80, c="black", marker='<')
            cr.plt.title("Minimization " + str(self.event_id) + " for pol " + str(self.use_lofar_with_pol))

            FittedLDF = cr.plt.figure()

            self.DistNewpol01 = self.GetDistance(MinCore, self.loradirection, self.positions1)
            self.DistNewpol00 = self.GetDistance(MinCore, self.loradirection, self.positions0)

            cr.plt.plot(self.DistNewpol01.vec(), self.signals1.vec(), color=self.color_pol1, linestyle="None", marker=self.marker_pol1, label=labelpol1)

            cr.plt.plot(self.DistNewpol00.vec(), self.signals0.vec(), color=self.color_pol0, linestyle="None", marker=self.marker_pol0, label=labelpol0)
            cr.plt.yscale("log")
            cr.plt.xlabel("Distance to Shower Axis from pol0 [m]")
            if self.plot_parameter == "pulses_maxima_y":
                cr.plt.ylabel("Pulse Power [a.u.]")
            else:
                cr.plt.ylabel("Power [a.u.]")
            cr.plt.legend(loc='upper right', shadow=False, numpoints=1)
            cr.plt.title("Core_from_FIT_" + str(self.event_id) + " for pol " + str(self.use_lofar_with_pol))

            if self.plot_scale_auto:
                cr.plt.axis(xmin=0, xmax=x_max, ymin=y_min, ymax=y_max)
            else:
                cr.plt.axis(xmin=self.plot_xmin, xmax=self.plot_xmax)

        if self.show_azimuth_distribution:

            self.DistPol0 = self.GetDistance(self.loracore, self.loradirection, self.positions0)

            self.XPos = cr.hArray_transpose(self.positions0)[0].vec()
            self.YPos = cr.hArray_transpose(self.positions0)[1].vec()

            self.DistPlane = cr.hArray(copy=self.XPos).vec()

            self.DistPlane = self.XPos * self.XPos + self.YPos * self.YPos
            self.DistPlane.sqrt()

            # print "d", self.DistPlane
            # print "x", self.XPos

            self.angle = cr.hArray(copy=self.XPos).vec()

            self.angle = self.XPos / self.DistPlane
            # print "x/d", self.angle
            self.angle.asin()
            self.angle = self.angle * 180 / (2 * cr.pi)
            # print "asin degree", self.angle
            DistPol0 = cr.hArray(self.DistPol0).toNumpy()
            Angle = cr.hArray(self.angle).toNumpy()
            SignalPol0 = cr.hArray(self.signals0).toNumpy()

            # print DistPol0
            sc = (150 < DistPol0) & (DistPol0 < 250)
            # print sc
            cr.plt.clf()
            cr.plt.plot(Angle[sc], SignalPol0[sc], linestyle="None", marker=self.marker_pol0)

            # 5<DistPol>4

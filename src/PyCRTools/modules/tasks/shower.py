"""
Module documentation
====================

"""

from pycrtools.tasks import Task
from pycrtools.grid import CoordinateGrid
import pycrtools as cr
import numpy as np


class Shower(Task):
    """
    Making all sorts of plots to understand the shower.

    Usage:

    LDF = cr.trun("Shower",positions=positions, signals=signals, signals_uncertainties=signals_uncertainties, core=core,direction=direction,core_uncertainties=core_uncertainties,direction_uncertainties=direction_uncertainties, ldf_enable=True)

    Task can be run without giving uncertainties. The resulting plot will then also not contain any uncertainties.

    FOOTPRINT = cr.trun("Shower", positions=positions, signals=signals, timelags = timelags, direction=direction, core = core, footprint_enable=True)

    """

    parameters = dict(
        positions=dict(default=None,
            doc="Antennas positions given in [Nx3], [NAntennas x (X,Y,Z)], X,Y,Y kartesian"),
        signals=dict(default=None,
            doc="Signals in the antennas given in [Nx3], [NAntennas x (pol_x,pol_y,pol_z)]"),
        signals_uncertainties=dict(default=None,
            doc="Uncertainties on signal in antennas in [Nx3]"),
        core=dict(default=None,
            doc="Core position of the shower given in [X,Y,Z], kartesian"),
        core_uncertainties=dict(default=None,
            doc="Uncertainties of core position of shower in [eX,eY,eZ], kartesian"),
        direction=dict(default=None,
            doc="Direction of the shower [AZ, EL], AZ is in degrees eastwards from north and EL is in degrees up from horizon."),
        direction_uncertainties=dict(default=None,
            doc="Uncertainties on the direction of the shower in [eAZ,eEL,Cov]"),
        timelags=dict(default=None,
            doc="Timelags of signals given in seconds [NAntennas x (X,Y,Z)] or [NAntennas]"),
        polarization_angle=dict(default=None,
            doc="Polarization angle per station [NAntannas x angle] in radians"),
        eventid=dict(default=None,
            doc="Give Event ID to be specified in plot title "),

        save_plots=dict(default=False,
            doc="Saving plost instead of showing them."),
        plot_prefix=dict(default="",
            doc="Prefix for stored plots"),
        plot_type=dict(default="png",
            doc="Plot type (e.g. png, jpeg, pdf)"),
        plotlist=dict(default=[],
            doc="List of output plots.", output=True),

        ldf_enable=dict(default=False,
            doc="Draw Lateral Distribution Function, signal vs. distance from shower axis"),
        ldf_logplot=dict(default=True,
            doc="Draw LDF with log axis"),
        ldf_integrated_signal=dict(default=False,
            doc="Change labels to match integrated pulse power"),
        ldf_remove_outliers=dict(default=True,
            doc="Do not allow values > 1000000 or < 0.001 for signal strength"),
        ldf_total_signal=dict(default=False,
            doc="draw only one signalstrength"),
        ldf_time_color = dict(default = False,
            doc="Use timelags as colors in LDF"),
        ldf_scale=dict(default=True, doc="Scale LDF to region around core only"),
        ldf_color_x=dict(default='#B30424', doc="color signals x"),
        ldf_color_y=dict(default='#68C8F7', doc="color signals y"),
        ldf_color_z=dict(default='#FF8C19', doc="color signals z"),
        ldf_marker_x=dict(default='s', doc="marker signals x"),
        ldf_marker_y=dict(default='o', doc="marker siganls y"),
        ldf_marker_z=dict(default='v', doc="marker siganls z"),


        footprint_enable=dict(default=False,
            doc="Draw footprint of the shower"),
        footprint_colormap=dict(default="autumn", doc="colormap to use for LOFAR timing"),
        footprint_marker_lofar=dict(default='o', doc="Marker for LOFAR stations in footprint"),
        footprint_use_background=dict(default=True, doc="Use LOFAR map as background for footprint"),
        footprint_point_scaling=dict(default=70, doc="Scaling factor for point size in LOFAR plot"),
        footprint_use_title=dict(default=True, doc="Draw title indicating polarizations and event id (if given)"),

        footprint_shower_enable=dict(default=True, doc='Draw shower geometry in footprint'),
        footprint_shower_color=dict(default="#151B8D", doc='Color in which the shower geometry is drawn'),
        footprint_scale=dict(default=True, doc="Scale footprint to display superterp only."),
        footprint_scale_box=dict(default=200, doc="Box around coordinate center which will be plotted"),
        footprint_scale_to_max=dict(default="False", doc="Scale circle size of the footprint to subtract minimum occuring value"),

        footprint_lora_enable=dict(default=True,
            doc="Draw Information from LORA"),
        lora_positions=dict(default=None,
            doc="LORA detector positions, given in [Nx3], [NDetectors x (X,Y,Z)], X,Y,Y kartesian"),
        lora_signals=dict(default=None,
            doc="Signals in LORA, given in [NDetectors]"),
        lora_timelags=dict(default=None,
            doc="Timelags of signals in LORA detectors, given in nanoseconds [NDetectors] "),
        footprint_color_lora=dict(default='#730909', doc="Color used for LORA plots. If set to 'time' uses the arrival time"),
        footprint_marker_lora=dict(default='p', doc="Marker for LORA stations in footprint"),
        footprint_lora_point_scaling=dict(default=200, doc="Scaling factor for point size in LORA shower"),

        skyplot_of_directions_enable=dict(default=False,
            doc="Plotting all directions of stations on sky"),
        all_directions=dict(default=None,
            doc="List of directions, [NStations x [az,el]]"),
        all_stations=dict(default=None,
            doc="List of stations corresponding to all_directions"),
        lora_direction=dict(default=None,
            doc='direction as reconstructed by LORA in [az,el]'),

        footprint_polarization_enable=dict(default=False,
            doc="Draw footprint with polarization arrows"),

        azimuth_in_distance_bins_enable=dict(default=False,
            doc="Making plots for different distance bins"),
        slicing=dict(default=None,
            doc="Give boundaries for azimuth bins in [a,b,c,d]"),


    )

    def __GetDistance(self, core, direction, positions):

        """
        Calculating the distance of a station to the shower core in the shower plane.
        """

        positions = cr.hArray(positions)
        core = cr.hArray(core)

        distances = cr.hArray(copy=positions)
        finaldistance = cr.hArray(copy=positions)

        theta = cr.radians(direction[1])           # spherical coordinates: 0 = horizon, 90 = vertical
        phi = cr.radians(450 - direction[0])        # recalculate to 0 = east counter-clockwise

        distances = positions - core
        axis = cr.hArray([cr.cos(theta) * cr.cos(phi), cr.cos(theta) * cr.sin(phi), cr.sin(theta)])  # shower axis

        finaldistance[...].crossproduct(distances[...], axis)
        dist = cr.hVectorLength(finaldistance[...])

        return dist

    def __GetTotalDistanceUncertainty(self, core, coreuncertainties, positions, direction, directionuncertainties, distances):

        """
        Propagating the uncertatinties of the core and the directions into the uncertainties of the distance to the shower axis.

        """
        # Assuming differences in z-coordinates = 0

        dist = distances.vec()
        positions = cr.hArray(positions)

        pos1 = cr.hArray_transpose(positions)[0].vec()
        pos2 = cr.hArray_transpose(positions)[1].vec()

        core1 = float(core[0])
        core2 = float(core[1])

        phi = cr.radians(450 - direction[0])        # recalculate to 0 = east counter-clockwise
        theta = cr.radians(direction[1])           # spherical coordinates: 0 = horizon, 90 = vertical

        ecore1 = float(coreuncertainties[0])
        ecore2 = float(coreuncertainties[1])
        covcore = float(coreuncertainties[2])

        ephi = cr.radians(float(directionuncertainties[0]))
        etheta = cr.radians(float(directionuncertainties[1]))
        covangles = float(directionuncertainties[2])

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

    def run(self):
        """Run the task.
        """


        # ---------------------  LDF  ------------------------------ #

        if self.ldf_enable:

            Dist = self.__GetDistance(self.core, self.direction, self.positions)

            if not self.ldf_total_signal:
               if self.ldf_remove_outliers:
                   for i in xrange(len(self.signals[:, 0])):
                       if self.signals[i, 0] > 100000:
                           self.signals[i, 0] = 10
                       if self.signals[i, 0] < 0.001:
                           self.signals[i, 0] = 1
                           if self.signals[i, 1] > 100000:
                               self.signals[i, 1] = 10
                           if self.signals[i, 1] < 0.001:
                               self.signals[i, 1] = 1
                           if self.signals[i, 2] > 100000:
                               self.signals[i, 2] = 10
                           if self.signals[i, 2] < 0.001:
                               self.signals[i, 2] = 1
            
            if self.ldf_total_signal:
                if self.signals.ndim > 1:
                    self.total_signals = self.signals[:,0]**2 + self.signals[:,1]**2 + self.signals[:,2]**2
                    self.total_uncertainties = self.signals_uncertainties[:,0]**2 + self.signals_uncertainties[:,1]**2 + self.signals_uncertainties[:,2]**2
                else:
                    self.total_signals = self.signals    
                    self.total_uncertainties = self.signals_uncertainties

            if self.core_uncertainties is not None and self.direction_uncertainties is not None and self.signals_uncertainties is not None:

                Dist_uncert = self.__GetTotalDistanceUncertainty(self.core, self.core_uncertainties, self.positions, self.direction, self.direction_uncertainties, Dist)

                if self.ldf_logplot:
                    sig_lower = np.copy(self.signals_uncertainties)   # Avoid negative errorbars
                    sig_lower.fill(0.01)
                    sig_lower = np.maximum(sig_lower, self.signals - self.signals_uncertainties)
                    sig_uncer = self.signals - sig_lower
                else:
                    sig_lower = np.copy(self.signals_uncertainties)
                
                
                
                cr.plt.figure()
                if self.ldf_total_signal:
                    cr.plt.errorbar(Dist, self.total_signals, yerr=self.total_uncertainties, xerr=Dist_uncert, c='k', marker=self.ldf_marker_x, linestyle="None", label="total")
                else:
                    cr.plt.errorbar(Dist, self.signals[:, 0], yerr=[sig_uncer[:, 0], self.signals_uncertainties[:, 0]], xerr=Dist_uncert, c=self.ldf_color_x, marker=self.ldf_marker_x, linestyle="None", label="x")
                    cr.plt.errorbar(Dist, self.signals[:, 1], yerr=[sig_uncer[:, 1], self.signals_uncertainties[:, 0]], xerr=Dist_uncert, c=self.ldf_color_y, marker=self.ldf_marker_y, linestyle="None", label="y")
                    cr.plt.errorbar(Dist, self.signals[:, 2], yerr=[sig_uncer[:, 2], self.signals_uncertainties[:, 0]], xerr=Dist_uncert, c=self.ldf_color_z, marker=self.ldf_marker_z, linestyle="None", label="z")

                cr.plt.xlabel("Distance to Shower Axis [m]")
                if self.ldf_integrated_signal:
                    cr.plt.ylabel("Integrated signal [a.u.]")
                else:
                    cr.plt.ylabel("Peak signal [a.u.]")

                if self.ldf_logplot:
                    cr.plt.yscale("log")
                cr.plt.legend(loc='upper right', shadow=False, numpoints=1)

            else:
                print "Not all uncertainties given, do not plot uncertainties"
                cr.plt.figure()
                if self.ldf_total_signal:
                    cr.plt.scatter(Dist, self.total_signals, c=self.ldf_color_x, marker=self.ldf_marker_x, label="total")
                else:
                    cr.plt.scatter(Dist, self.signals[:, 0], c=self.ldf_color_x, marker=self.ldf_marker_x, label="x")
                    cr.plt.scatter(Dist, self.signals[:, 1], c=self.ldf_color_y, marker=self.ldf_marker_y, label="y")
                    cr.plt.scatter(Dist, self.signals[:, 2], c=self.ldf_color_z, marker=self.ldf_marker_z, label="z")

                cr.plt.xlabel("Distance to Shower Axis [m]")
                if self.ldf_integrated_signal:
                    cr.plt.ylabel("Integrated signal [a.u.]")
                else:
                    cr.plt.ylabel("Peak signal [a.u.]")

                if self.ldf_logplot:
                    cr.plt.yscale("log")
                cr.plt.legend(loc='upper right', shadow=False, scatterpoints=1)

            if self.ldf_time_color:
                  if self.timelags is not None:
                        cr.plt.figure()
                        if self.timelags.ndim == 1:
                            ldf_colors = self.timelags - self.timelags.min()
                        else:
                            ldf_colors = self.timelags[:, 0] - self.timelags[:, 0].min()

                        if self.ldf_total_signal:
                            cr.plt.scatter(Dist, self.signals, c=ldf_colors, marker=self.ldf_marker_x, label="x",cmap=self.footprint_colormap)
                        else:
                            cr.plt.scatter(Dist, self.signals[:, 0], c=ldf_colors, marker=self.ldf_marker_x, label="x",cmap=self.footprint_colormap)
                            cr.plt.scatter(Dist, self.signals[:, 1], c=ldf_colors, marker=self.ldf_marker_y, label="y",cmap=self.footprint_colormap)
                            cr.plt.scatter(Dist, self.signals[:, 2], c=ldf_colors, marker=self.ldf_marker_z, label="z",cmap=self.footprint_colormap)
                        cr.plt.xlabel("Distance to Shower Axis [m]")
                        cr.plt.ylabel("Signal [a.u.]")
                  else:
                    print "Give timelags, if you want LDF in timing color"

            if self.ldf_scale:
                cr.plt.xlim(0, 500)

            if self.save_plots:
                if self.ldf_integrated_signal:
                    plotname = self.plot_prefix + "shower_integrated_ldf.{0}".format(self.plot_type)
                elif self.ldf_total_signal:
                    plotname = self.plot_prefix + "shower_total_ldf.{0}".format(self.plot_type)  
                elif self.ldf_time_color: 
                    plotname = self.plot_prefix + "shower_color_ldf.{0}".format(self.plot_type)  
                else:
                    plotname = self.plot_prefix + "shower_ldf.{0}".format(self.plot_type)
                cr.plt.savefig(plotname)
                self.plotlist.append(plotname)
            else:
                cr.plt.show()

       #---------------------- FOOTPRINT --------------------#

        if self.footprint_enable:

            # LORA
            LORA_regular = False
            LORA_time = False
            Data_available = True

            if self.lora_signals is not None and self.lora_positions is not None and self.footprint_lora_enable:

                self.lsizes = np.copy(self.lora_signals)
                self.lsizes = np.log10(self.lsizes)
                self.lsizes *= self.footprint_lora_point_scaling

                if self.lora_timelags is not None and self.footprint_color_lora == 'time':
                    LORA_time = True

                    self.loracolor = np.copy(self.lora_timelags)
                    for i in xrange(len(self.loracolor)):
                        if self.loracolor[i] == 0:
                            self.loracolor[i] += max(self.loracolor)
                else:
                    LORA_regular = True

            # Background
            if self.footprint_use_background:
                    from os import environ
                    from os.path import isfile
                    if "LOFARSOFT" in environ.keys():
                        bgimname = environ["LOFARSOFT"] + "/src/PyCRTools/extras/LORA_layout_background.{0}".format(self.plot_type)
                        if isfile(bgimname):
                            bgim = cr.plt.imread(bgimname)
                        else:
                            print "WARNING Cannot plot layout"
                    else:
                        print "WARNING Cannot plot layout. Environment variable LOFARSOFT not found."

            # Shower
            if self.footprint_shower_enable and self.direction is not None and self.core is not None:

                    dcos = cr.cos(cr.radians(self.direction[0]))*2
                    dsin = cr.sin(cr.radians(self.direction[0]))*2
                    elev = 90 - self.direction[1]

            if self.positions is not None and self.signals is not None:
              if self.signals.shape[0] > 0 and self.timelags.shape[0] > 0:

                if self.footprint_scale:
                    scaling_check = (self.positions[:, 0] > -self.footprint_scale_box) & (self.positions[:, 1] < self.footprint_scale_box) & (self.positions[:, 1] > -self.footprint_scale_box) & (self.positions[:, 0] < self.footprint_scale_box)
                    scaling_check = scaling_check.transpose()

                    self.signals = self.signals[scaling_check]
                    self.positions = self.positions[scaling_check]
                    self.timelags = self.timelags[scaling_check]
                    if self.signals.shape[0] == 0:
                        print "When scaling to superterp no stations left. Draw without scaling to see station."
                        Data_available = False

                if self.timelags is None:
                   self.scolors = "blue"
                   print "WARNING, footprint does not represent the time, only the signal strength"

                # ------------- POL 0
                if Data_available:
                    if len(self.signals.shape) == 1:
                        self.sizes0 = np.copy(self.signals)
                    else:
                        self.sizes0 = np.copy(self.signals[:, 0])
                    self.sizes0 = np.log(self.sizes0)
                    if self.footprint_scale_to_max == "True":
                        if self.sizes0.min() > 0:
                            if self.sizes0.max() != self.sizes0.min():
                                self.sizes0 = self.sizes0 - self.sizes0.min()

                    self.sizes0 *= self.footprint_point_scaling

                    if self.timelags is not None:
                        if self.timelags.ndim == 1:
                            self.scolors = self.timelags - self.timelags.min()
                        else:
                            self.scolors = self.timelags[:, 0] - self.timelags[:, 0].min()

                        self.scolors *= 1e9

                cr.plt.figure()

                # Adding LORA
                if LORA_time:
                    cr.plt.scatter(self.lora_positions[:, 0], self.lora_positions[:, 1], s=self.lsizes, c=self.loracolor, marker=self.footprint_marker_lora, cmap="winter")
                if LORA_regular:
                    cr.plt.scatter(self.lora_positions[:, 0], self.lora_positions[:, 1], s=self.lsizes, c=self.footprint_color_lora, marker=self.footprint_marker_lora)

                if bgim is not None and self.footprint_use_background:
                    cr.plt.imshow(bgim, origin='upper', extent=[-375 / 2, 375 / 2, -375 / 2 - 6 * 120 / 227, 375 / 2 - 6 * 120 / 227], alpha=1.0)

                if Data_available:
                    cr.plt.scatter(self.positions[:, 0], self.positions[:, 1], s=self.sizes0, c=self.scolors, marker=self.footprint_marker_lofar, cmap=self.footprint_colormap)
                    if self.timelags is not None:
                        self.cbar = cr.plt.colorbar()
                        self.cbar.set_label("Time of arrival (ns)")
                else:
                    cr.plt.scatter(0,0)
                    cr.plt.annotate('No superterp stations',[-100,100],fontsize='x-large')
                cr.plt.xlabel("LOFAR East [meters] ")
                cr.plt.ylabel("LOFAR North [meters] ")

                if self.footprint_use_title:
                    if self.eventid:
                        self.title = str(self.eventid) + " in pol X"
                    else:
                        if len(self.signals.shape) == 1:
                            self.title = 'total signal'
                        else:
                            self.title = 'pol X'
                    cr.plt.title(self.title)

                # Plotting the shower
                if self.footprint_shower_enable:
                    cr.plt.arrow(self.core[0] + elev * dsin, self.core[1] + elev * dcos, -elev * dsin, -elev * dcos, lw=4, color=self.footprint_shower_color)
                    cr.plt.scatter(self.core[0], self.core[1], marker='x', s=400, color=self.footprint_shower_color, linewidth=3)

                if self.save_plots:
                    plotname = self.plot_prefix + "shower_footprint_polX.{0}".format(self.plot_type)
                    cr.plt.savefig(plotname)
                    self.plotlist.append(plotname)

                # ----------- POL 1
                if Data_available:
                    if len(self.signals.shape) > 1:
                      if self.signals.shape[1] > 1:
                        self.sizes1 = np.copy(self.signals[:, 1])
                        self.sizes1 = np.log(self.sizes1)
                        if self.footprint_scale_to_max == "True":
                            if self.sizes1.min() > 0:
                                if self.sizes1.max() != self.sizes1.min():
                                    self.sizes1 = self.sizes1 - self.sizes1.min()
                        self.sizes1 *= self.footprint_point_scaling

                        cr.plt.figure()

                        # Adding LORA
                        if LORA_time:
                            cr.plt.scatter(self.lora_positions[:, 0], self.lora_positions[:, 1], s=self.lsizes, c=self.loracolor, marker=self.footprint_marker_lora, cmap="winter")
                        if LORA_regular:
                            cr.plt.scatter(self.lora_positions[:, 0], self.lora_positions[:, 1], s=self.lsizes, c=self.footprint_color_lora, marker=self.footprint_marker_lora)

                        # Background
                        if bgim is not None and self.footprint_use_background:
                            cr.plt.imshow(bgim, origin='upper', extent=[-375 / 2, 375 / 2, -375 / 2 - 6 * 120 / 227, 375 / 2 - 6 * 120 / 227], alpha=1.0)

                        # Signals
                        if self.timelags is not None:
                            if self.timelags.ndim == 1:
                                self.scolors = self.timelags - self.timelags.min()
                            else:
                                self.scolors = self.timelags[:, 1] - self.timelags[:, 1].min()
                            self.scolors *= 1e9
                        cr.plt.scatter(self.positions[:, 0], self.positions[:, 1], s=self.sizes1, c=self.scolors, marker=self.footprint_marker_lofar, cmap=self.footprint_colormap)
                        cr.plt.xlabel("LOFAR East [meters] ")
                        cr.plt.ylabel("LOFAR North [meters] ")
                        if self.timelags is not None:
                            self.cbar = cr.plt.colorbar()
                            self.cbar.set_label("Time of arrival (ns)")
                        if self.footprint_use_title:
                            if self.eventid:
                                self.title = str(self.eventid) + " in pol Y"
                            else:
                                self.title = 'pol Y'
                            cr.plt.title(self.title)

                        # Plotting the shower
                        if self.footprint_shower_enable:
                            cr.plt.arrow(self.core[0] + elev * dsin, self.core[1] + elev * dcos, -elev * dsin, -elev * dcos, lw=4, color=self.footprint_shower_color)
                            cr.plt.scatter(self.core[0], self.core[1], marker='x', s=400, color=self.footprint_shower_color, linewidth=3)

                        if self.save_plots:
                            plotname = self.plot_prefix + "shower_footprint_polY.{0}".format(self.plot_type)
                            cr.plt.savefig(plotname)
                            self.plotlist.append(plotname)

                    # -------- POL 2
                    if len(self.signals.shape) > 1:
                      if self.signals.shape[1] == 3:

                        self.sizes2 = np.copy(self.signals[:, 2])
                        self.sizes2 = np.log(self.sizes2)
                        if self.footprint_scale_to_max == "True":
                            if self.sizes2.min() > 0:
                                if self.sizes2.max() != self.sizes2.min():
                                    self.sizes2 = self.sizes2 - self.sizes2.min()
                        self.sizes2 *= self.footprint_point_scaling

                        cr.plt.figure()

                        # Adding LORA
                        if LORA_time:
                            cr.plt.scatter(self.lora_positions[:, 0], self.lora_positions[:, 1], s=self.lsizes, c=self.loracolor, marker=self.footprint_marker_lora, cmap="winter")
                        if LORA_regular:
                            cr.plt.scatter(self.lora_positions[:, 0], self.lora_positions[:, 1], s=self.lsizes, c=self.footprint_color_lora, marker=self.footprint_marker_lora)

                        if bgim is not None and self.footprint_use_background:
                            cr.plt.imshow(bgim, origin='upper', extent=[-375 / 2, 375 / 2, -375 / 2 - 6 * 120 / 227, 375 / 2 - 6 * 120 / 227], alpha=1.0)
                        if self.timelags is not None:
                            if self.timelags.ndim == 1:
                                self.scolors = self.timelags - self.timelags.min()
                            else:
                                self.scolors = self.timelags[:, 2] - self.timelags[:, 2].min()
                            self.scolors *= 1e9
                        cr.plt.scatter(self.positions[:, 0], self.positions[:, 1], s=self.sizes2, c=self.scolors, marker=self.footprint_marker_lofar, cmap=self.footprint_colormap)
                        cr.plt.xlabel("LOFAR East [meters] ")
                        cr.plt.ylabel("LOFAR North [meters] ")

                        if self.timelags is not None:
                            self.cbar = cr.plt.colorbar()
                            self.cbar.set_label("Time of arrival (ns)")
                        if self.footprint_use_title:
                            if self.eventid:
                                self.title = str(self.eventid) + " in pol Z"
                            else:
                                self.title = 'pol Z'
                            cr.plt.title(self.title)

                        # Plotting the shower
                        if self.footprint_shower_enable:
                            cr.plt.arrow(self.core[0] + elev * dsin, self.core[1] + elev * dcos, -elev * dsin, -elev * dcos, lw=4, color=self.footprint_shower_color)
                            cr.plt.scatter(self.core[0], self.core[1], marker='x', s=400, color=self.footprint_shower_color, linewidth=3)

                        if self.save_plots:
                            plotname = self.plot_prefix + "shower_footprint_polZ.{0}".format(self.plot_type)
                            cr.plt.savefig(plotname)
                            self.plotlist.append(plotname)

                if not self.save_plots:
                    pass
                    # cr.plt.show()

            else:
                print "WARNING: Give at least positions and signals to plot a footprint"



        # --------------------- Skyplot of all directions ----------------------------#

        if self.skyplot_of_directions_enable:

            from matplotlib import cm

            if self.all_directions is not None and self.all_stations is not None:

                az = 90 - self.all_directions[:,0]
                zen = 90 - self.all_directions[:,1]

                direction_colors = np.arange(float(len(self.all_stations)))/(len(self.all_stations)-1)

                cr.plt.figure()
                ax = cr.plt.subplot(111)

                for i in xrange(len(self.all_stations)):
                    cr.plt.polar(np.radians(az[i]),zen[i],marker="s",markersize=7, linestyle ="None",color=cm.gist_rainbow(direction_colors[i]),label=self.all_stations[i])

                if self.lora_direction:
                    cr.plt.polar(np.radians(90 - self.lora_direction[0]),90 -self.lora_direction[1], marker='o', markersize=5, linestyle="None", color='k', label='LORA')


                cr.plt.legend(bbox_to_anchor=(1., .0, 1., .0),loc=3,borderaxespad=0., shadow=False, numpoints=1)

                cr.plt.rgrids([10,30,60,80],labels=None, angle=202)
                ax.set_frame_on(False)
                ax.set_xticks([])
                ax.set_yticks([])

                if self.save_plots:
                    plotname = self.plot_prefix + "direction_on_sky.{0}".format(self.plot_type)
                    cr.plt.savefig(plotname)
                    self.plotlist.append(plotname)
                else:
                    cr.plt.show()



        # --------------------- Polarization Footprint  ------------------------------ #
        if self.footprint_polarization_enable:

            if self.positions is not None and self.polarization_angle is not None:
               cr.plt.figure()
               cr.plt.scatter(self.positions[:, 0], self.positions[:, 1])
               
               print "angles", self.polarization_angle

               x_pol = np.cos(self.polarization_angle) * 20
               y_pol = np.sin(self.polarization_angle) * 20

               for i in xrange(self.positions.shape[0]):
                     cr.plt.arrow(self.positions[i, 0] - x_pol[i] / 2., self.positions[i, 1] - y_pol[i] / 2., x_pol[i], y_pol[i], color='red', lw=2, head_length=1, head_width=1)
                     cr.plt.ylabel("LOFAR North [meters]")
                     cr.plt.xlabel("LOFAR East [meters] ")

               if self.core is not None and self.direction is not None:
                    elev = 90 - self.direction[1]
                    dcos = cr.cos(cr.radians(self.direction[0]))
                    dsin = cr.sin(cr.radians(self.direction[0]))
                    cr.plt.arrow(self.core[0] + elev * dsin, self.core[1] + elev * dcos, -elev * dsin, -elev * dcos, lw=4, color=self.footprint_shower_color)
                    cr.plt.scatter(self.core[0], self.core[1], marker='x', s=400, color=self.footprint_shower_color, linewidth=3)

            else:
                print "Give positions and polarization angles to draw footprint in polarization"

            if self.save_plots:
                plotname = self.plot_prefix + "polarization_footprint.{0}".format(self.plot_type)
                cr.plt.savefig(plotname)
                self.plotlist.append(plotname)
            else:
                cr.plt.show()

        # ---------------------  Azimuth binned distance  ------------------------------ #

        if self.azimuth_in_distance_bins_enable:

                Dist = self.__GetDistance(self.core, self.direction, self.positions)
                Dist = np.array(Dist)
                DistPlane = self.positions[:, 0] * self.positions[:, 0] + self.positions[:, 1] * self.positions[:, 1]

                DistPlane = np.sqrt(DistPlane)

                Angle = self.positions[:, 0] / DistPlane
                Angle = np.arcsin(Angle)
                Angle = np.rad2deg(Angle)

                # Slicing
                if self.slicing is None:
                    self.slicing = [100, 200, 300, 400]

                asc = (Dist < self.slicing[0])
                bsc = (Dist > self.slicing[0]) & (Dist < self.slicing[1])
                csc = (Dist > self.slicing[1]) & (Dist < self.slicing[2])
                dsc = (Dist > self.slicing[2]) & (Dist < self.slicing[3])
                esc = (Dist > self.slicing[3])

                cr.plt.figure()

                cr.plt.plot(Angle[asc], self.signals[:, 0][asc], linestyle="None", marker='o', color="green", label="d < 100m")
                cr.plt.plot(Angle[bsc], self.signals[:, 0][bsc], linestyle="None", marker='s', color="red", label="100m < d < 200m")
                cr.plt.plot(Angle[csc], self.signals[:, 0][csc], linestyle="None", marker='<', color="blue", label="200m < d < 300m")
                cr.plt.plot(Angle[dsc], self.signals[:, 0][dsc], linestyle="None", marker='>', color="magenta", label="300m < d < 400m")
                cr.plt.plot(Angle[esc], self.signals[:, 0][esc], linestyle="None", marker='v', color="black", label="d > 400m")
                cr.plt.ylabel("Signal [a.u.]")
                cr.plt.xlabel("Angle to shower core")
                cr.plt.legend(loc='upper right', shadow=False, numpoints=1)

                cr.plt.figure()

                cr.plt.plot(Angle[asc], self.signals[:, 1][asc], linestyle="None", marker='o', color="green", label="d < 100m")
                cr.plt.plot(Angle[bsc], self.signals[:, 1][bsc], linestyle="None", marker='s', color="red", label="100m < d < 200m")
                cr.plt.plot(Angle[csc], self.signals[:, 1][csc], linestyle="None", marker='<', color="blue", label="200m < d < 300m")
                cr.plt.plot(Angle[dsc], self.signals[:, 1][dsc], linestyle="None", marker='>', color="magenta", label="300m < d < 400m")
                cr.plt.plot(Angle[esc], self.signals[:, 1][esc], linestyle="None", marker='v', color="black", label="d > 400m")
                cr.plt.ylabel("Signal [a.u.]")
                cr.plt.xlabel("Angle to shower core")
                cr.plt.legend(loc='upper right', shadow=False, numpoints=1)

                if self.save_plots:
                    plotname = self.plot_prefix + "shower_azimuthal_signal.{0}".format(self.plot_type)
                    cr.plt.savefig(plotname)
                    self.plotlist.append(plotname)
                else:
                    cr.plt.show()

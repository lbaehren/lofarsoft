"""
Module documentation
====================

"""

from pycrtools.tasks import Task
from pycrtools.grid import CoordinateGrid
import numpy as np
import pycrtools as cr
import matplotlib.pyplot as plt
from scipy.stats import shapiro

class Noise(Task):
    """Noise characterization.

    """

    parameters = dict(
        timeseries_data = dict( default = None,
            doc = "Timeseries data." ),
        nantennas = dict( default = lambda self : self.timeseries_data.shape()[0],
            doc = "Number of antennas." ),
        nbins = dict( default = 100,
            doc = "Number of bins for histogram." ),
        shapiro = dict ( default = [],
            doc = "Results of Shapiro-Wilk test for normality." ),
        save_plots = dict( default = False,
            doc = "Store plots" ),
        plot_prefix = dict( default = "",
            doc = "Prefix for plots" ),
        plotlist = dict( default = [],
            doc = "List of plots" ),
        plot_antennas = dict( default = lambda self : range(self.nantennas),
            doc = "Antennas to create plots for." ),
    )

    def run(self):
        """Run the task.
        """

        for i in self.nantennas:

            # Compute Shapiro-Wilk test for normality
            self.shapiro.append(shapiro(self.timeseries_data[i]))

        if self.save_plots:

            # Histograms
            for i in self.plot_antennas:
                plt.clf()

                s = self.timeseries_data.toNumpy()

                plt.hist(s[i], self.nbins, color='g')

                p = self.plot_prefix + "noise_histogram_antenna-{0:d}.png".format(i)

                plt.title("Noise histogram for antenna {0:d}".format(i))
                plt.savefig(p)

                self.plotlist.append(p)


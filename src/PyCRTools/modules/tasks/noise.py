"""
Module documentation
====================

"""

from pycrtools.tasks import Task
from pycrtools.grid import CoordinateGrid
import numpy as np
import pycrtools as cr
import matplotlib.pyplot as plt
from scipy.stats import normaltest


class Noise(Task):
    """Noise characterization.

    """

    parameters = dict(
        timeseries_data=dict(default=None,
            doc="Timeseries data, hArray [NAntennas, samples]"),
        nantennas=dict(default=lambda self: self.timeseries_data.shape()[0],
            doc="Number of antennas."),
        nbins=dict(default=100,
            doc="Number of bins for histogram."),
        histrange=dict(default=(-5, 5),
            doc="Range of histogram to be plotted."),
        p_value = dict(default=0.05,
            doc="p value for normality test, everything above this value is regarded as matching a normal distribution."),
        normaltest_results = dict(default=[],
            doc="Results of D'Agostino and Pearson's test for normality."),
        suspect_antennas = dict(default=[],
            doc="Antennas for which the noise is found not to be drawn from a normal distribution (e.g p_value to small)."),
        plot_prefix = dict(default="",
            doc="Prefix for plots"),
        plot_type = dict(default="png",
            doc="Plot type (e.g. png, jpeg, pdf)"),
        plotlist = dict(default=[],
            doc="List of plots"),
        plot_only_suspect_antennas = dict(default=True,
            doc="Plot only those antennas that are suspect."),
    )

    def run(self):
        """Run the task.
        """

        # Force recalculation of all task parameters
        self.update(True)

        # Convert data to numpy
        s = self.timeseries_data.toNumpy()

        # Compensate for updating bug in tasks
        self.nantennas = s.shape[0]

        # Now compute some basic noise statistics
        self.mean = np.mean(s, axis=0)
        self.std = np.std(s, axis=0)

        self.suspect_antennas = []
        for i in range(self.nantennas):

            # Compute D'Agostino and Pearson's test for normality
            norm = normaltest(s[i])

            self.normaltest_results.append(norm)

            if norm[1] < self.p_value:
                self.suspect_antennas.append(i)

        # Histograms
        if self.plot_only_suspect_antennas:
            antennas = self.suspect_antennas
        else:
            antennas = range(self.nantennas)

        for i in antennas:
            plt.clf()

            plt.hist(s[i], bins=self.nbins, range=self.histrange, color='g')

            p = self.plot_prefix + "noise_histogram_antenna-{0:d}.{1}".format(i, self.plot_type)

            plt.title(r"Noise histogram for antenna {0:d}, $\mu={1:f}, \sigma={2:f}$".format(i, self.mean[i], self.std[i]))
            plt.savefig(p)

            self.plotlist.append(p)

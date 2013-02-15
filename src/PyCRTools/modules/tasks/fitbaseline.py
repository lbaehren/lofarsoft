"""
Spectrum documentation
======================

Usage::

  >>> sp=cr.hArrayRead('/Users/falcke/data/Pulses/oneshot_level4_CS017_19okt_no-0.h5.spec.pcr')

"""

import pycrtools as cr
from pycrtools import tasks
from matplotlib import pyplot as plt
import matplotlib
import math
import time
import os

"""

  =========== ===== ========================================================
  *default*         contains a default value or a function that will be
                    assigned when the parameter is accessed the first time
                    and no value has been explicitly set. The function has
                    the form ``lambda ws: functionbody``, where ws is the
                    worspace itself, so that one can access other
                    parameters. E.g.: ``default:lambda ws: ws.par1+1`` so
                    that the default value is one larger than the value in
                    ``par1`` in the workspace.
  *doc*             A documentation string describing the parameter.
  *unit*            The unit of the value (for informational purposes only)
  *export*    True  If ``False`` do not export the parameter with
                    ``ws.parameters()`` or print the parameter
  *workarray* False If ``True`` then this is a workarray which contains
                    large amount of memory and is listed separately and
                    not written to file.
  =========== ===== ========================================================

"""


def fitbaseline_calc_freqs(self):
    """
    """
    val = cr.hArray(float, dimensions=[self.nbins], name="Frequency", units=("M", "Hz"))
    val.downsample(self.frequency[self.numin_i:self.numax_i])
    return val


def fitbaseline_calc_numin_i(self):
    """
    """
    if self.numin > 0:
        numin_i = self.frequency.findlowerbound(self.numin).val()
    else:
        numin_i = 2
    numin_i = max(numin_i - int(numin_i * self.extendfit), 2)
    return numin_i


def fitbaseline_calc_numax_i(self):
    """
    """
    if self.numax > 0:
        numax_i = self.frequency.findlowerbound(self.numax).val()
    else:
        numax_i = len(self.frequency)
    numax_i = min(numax_i + int((len(self.frequency) - numax_i) * self.extendfit), len(self.frequency) - 1)
    return numax_i


class FitBaseline(tasks.Task):
    """
    **Usage**::

      FitBaseline(input_spectrum, **keywords)

    **Description**:

    Function to fit a baseline using a polynomial function
    (``fittype='POLY'``) or a basis spline fit (default: ``fittype='BSPLINE'``)
    to a spectrum while trying to ignore positive spikes in the fit
    (e.g., those coming from RFI = Radio Frequency Interference).

    The input_spectrum is a float array with input spectrum either of
    dimension ``[nofAntennas,nofChannels]`` or just ``[nofChannels]`` for a
    single spectrum. Note that the frequency values for the array are
    expected to be provided as
    ``input_spectrum.par.xvalues=cr.hArray(float,[nofChannels],fill=...)``
    the same you would use to provide xvalues for plotting. You can
    also provide the frequencies explicitly in the keyword
    ``frequency``, otherwise a simple numbering is assumed
    ``frequency=[0,1,2,3...]``.

    *RFI Treatment*:

    The spectra are then downsampled and checked which bins have a
    large RMS (when divided by the mean in each bin). Those bins are
    affected by interference and are ignored for the fitting. The
    final fit is done to the clean bins only.

    *Quality check*:

    At the end the fitted baselines are compared to each other
    (divided by the average spectrum of all antennas) and a list of
    deviant antennas is returned in ``Task.badantennas``.

    **Results**:

    The main results are the fit coefficients ``Task.coeffs``.

    Bad antennas are listed in ``Task.badantennas``.

    Use ``baseline.polynomial(task.frequency,task.coeffs,task.powers)`` to caluclate the
    baseline from the coefficients for the polynomial fit, or::

      >>> task.xpowers[...,[0]:task.nselected_bins].linearfitpolynomialx(task.clean_bins_x[...,[0]:task.nselected_bins],task.powers[...])
      >>> task.clean_bins_y[...,[0]:task.nselected_bins].bspline(task.xpowers[...,[0]:(task.nselected_bins)],task.coeffs[...])

    Parameters can be provided as additional keywords, e.g.::

      baseline.polynomial(frequency,coeffs,powers, parameter1=value1, parameter2=value2)

    or in a WorkSpace, e.g.::

      baseline.polynomial(frequency,coeffs,powers, ws=WorkSpace),

    or in a mix of the two. Otherwise default values are used.
    """

    parameters = dict(

        filename=dict(default=lambda self: "tmpspec.pcr" if not self.spectrum.hasHeader("FILENAME") else self.spectrum.getHeader("FILENAME"),
                        doc="Filename to write output to"),

        save_output=dict(default=False,
                           doc="If **True** save the results in the header files of the input spectrum with file name given in ``filename``"),

        nbins=dict(doc="The number of bins in the downsampled spectrum used to fit the baseline.",
                     default=lambda self: max(self.nofChannels / 256, min(256, self.nofChannels / 8))),

        polyorder=dict(doc="Order of the polynomial to fit. (output only)",
                         default=lambda self: self.ncoeffs - 1,
                         output=True),

        rmsfactor=dict(doc="Factor above and below the RMS in each bin at which a bin is no longer considered due to too many spikes.",
                         default=3.0),

        logfit=dict(doc="Actually fit the polynomial to the log of the (downsampled) data. (Hence you need to .exp the baseline afterwards).",
                      default=True),

        fittype=dict(doc="""Determine which type of fit to do: ``fittype='POLY'`` - do a polynomial fit, else ('BSPLINE') do a basis spline fit (default).""",
                       default="BSPLINE"),

        nofAntennas=dict(doc="""Number of antennas in input spectrum.""",
                           default=lambda self: 1 if len(self.dim_spectrum) == 1 else self.dim_spectrum[0]),

        nofChannels=dict(doc="""Number of channels in input spectrum.""",
                           default=lambda self: self.dim_spectrum[0] if len(self.dim_spectrum) == 1 else self.dim_spectrum[1]),

        nofChannelsUsed=dict(doc="""Number of channels remaining after downsampling and ignoring edges.""",
                               default=lambda self: self.numax_i - self.numin_i),

        dim_spectrum=dict(doc="""Dimension of input spectrum (typically *n* antennas times *m* spectral points) or just one dimensional for one antenna.""",
                            default=lambda self: self.spectrum.shape()),

        ncoeffs=dict(doc="""Number of coefficients for the polynomial.""",
                       default=18),

        splineorder=dict(doc="""Order of the polynomial to fit for the BSpline, ``splineorder=3`` is a bicubic spline.""",
                           default=3),

        numin=dict(doc="""Minimum frequency of useable bandwidth. Negative if to be ignored.""",
                     default=-1),

        numax=dict(doc="""Maximum frequency of useable bandwidth. Negative if to be ignored.""",
                     default=-1),

        numin_val_i=dict(doc="""Minimum frequency of useable bandwidth corresponding to ``numin_i`` (output)""",
                           default=lambda self: self.frequency[self.numin_i]),

        numax_val_i=dict(doc="""Maximum frequency of useable bandwidth corresponding to ``numax_i`` (output)""",
                           default=lambda self: self.frequency[self.numax_i]),

        numin_i=dict(doc="""Channel number in spectrum of the minimum frequency of the useable bandwidth. Negative if to be ignored.""",
                       default=fitbaseline_calc_numin_i,
                       output=True),

        numax_i=dict(doc="""Channel number in spectrum of the maximum frequency of the useable bandwidth. Negative if to be ignored.""",
                       default=fitbaseline_calc_numax_i,
                       output=True),

        extendfit=dict(doc="""Extend the fit by this factor at both ends beyond ``numax`` and ``numin``. The factor is relative to the unused bandwidth. Use this to make sure there is a stable solution at least between ``numax``/``numin``, i.e. avoid wiggles at the endpoint.""",
                         default=0.1),

        freqs=dict(doc="""Array of frequency values of the downsampled spectrum. (work vector)""",
                     default=fitbaseline_calc_freqs,
                     workarray=False),

        small_spectrum=dict(doc="""Array of power values holding the downsampled spectrum. (work vector)""",
                              default=lambda self: cr.hArray(float, [self.nofAntennas, self.nbins], name="Binned Spectrum", units="a.u.", xvalues=self.freqs, par=("logplot", "y")),
                              workarray=True),

        small_average_spectrum = dict(doc="""Array of power values holding the average (fitted) downsampled spectrum. (work vector)""",
                              default=lambda self: cr.hArray(float, [self.nbins], name="Binned Spectrum", units="a.u.", xvalues=self.fitted_bins_x, par=("logplot", "y")),
                              workarray=True),

        rms=dict(doc="""Array of RMS values of the downsampled spectrum. (work vector)""",
                   default=lambda self: cr.hArray(properties=self.small_spectrum, name="RMS of Spectrum"),
                   workarray=True),

        minmean=dict(doc="Mean value of data in the part of downsampled spectrum with the smallest RMS (output only)",
                       default=lambda self: cr.Vector(float, self.nofAntennas, fill=0.0),
                       output=True),

        minrms=dict(doc="RMS value of data in the part of downsampled spectrum with the smallest RMS (output only)",
                      default=lambda self: cr.Vector(float, self.nofAntennas, fill=0.0),
                      output=True),

        minrms_blen=dict(doc="Block length within downsampled data to look for the cleanest part of the spectrum.",
                           default=lambda self: min(64, max(self.nbins / 16, 4))),

        chisquare=dict(doc="""Returns the :math:`\\chi^2` of the baseline fit. (output only)""",
                         default=0,
                         output=True),

        weights=dict(doc="""Array of weight values for the fit. (work vector)""",
                       default=lambda self: cr.hArray(properties=self.small_spectrum, name="Fit Weights"),
                       workarray=True),

        ratio=dict(doc="""Array holding the ratio between RMS and power of the downsampled spectrum. (work vector)""",
                     default=lambda self: cr.hArray(properties=self.small_spectrum, name="RMS/Amplitude", par=("logplot", False)),
                     workarray=True),

        covariance=dict(doc="""Array containing the covariance matrix of the fit. (output only)""",
                          default=lambda self: cr.hArray(float, [self.nofAntennas, self.ncoeffs, self.ncoeffs]),
                          output=True),

        bwipointer=dict(doc="""Pointer to the internal BSpline workspace as integer. Don't change! """,
                          default=0,
                          export=False,
                          output=True),

        clean_bins_x=dict(doc="""Array holding the frequencies of the clean bins. (work vector)""",
                            default=lambda self: cr.hArray(dimensions=[self.nofAntennas, self.nbins], properties=self.freqs, name="Frequency"),
                            workarray=True),

        clean_bins_y=dict(doc="""Array holding the powers of the clean bins. (work vector)""",
                            default=lambda self: self.small_spectrum.new(),
                            workarray=True),

        fitted_bins_y=dict(doc="""Array holding the fitted, binned spectrum - for plotting and control. (work vector)""",
                            default=lambda self: cr.hArray(properties=self.small_spectrum, xvalues=self.fitted_bins_x, name="Fitted and binned spectrum"),
                            workarray=True),
        flattened_bins_y=dict(doc="""Array holding the binned spectrum of each antenna divided by the mean spectrum of all antennas - for plotting and control. (work vector)""",
                            default=lambda self: cr.hArray(properties=self.small_spectrum, xvalues=self.fitted_bins_x, name="Fitted and binned spectrum"),
                            workarray=True),

        fitted_bins_x=dict(doc="""Array holding the frequency values for the fitted, binned spectrum - for plotting and control. (work vector)""",
                            default=lambda self: cr.hArray(properties=self.freqs),
                            workarray=True),

        qualitycontrol=dict(doc="If True then select bad antennas (How many sigma can the rms of the difference spectrum deviate from those of others antennas)",
                      default=True),

        nsigma=dict(doc="Used to select bad antennas (How many sigma can the rms of the difference spectrum deviate from those of others antennas)",
                      default=3),

        badantennas=dict(doc="List of bad antennas (where the rms of the difference spectrum deviated from those of others antennas)",
                      default=[], output=True),

        goodantennas=dict(doc="List of bad antennas (where the rms of the difference spectrum deviated from those of others antennas)",
                      default=[], output=True),

        nbadantennas=dict(doc="Number of bad antennas (where the rms of the difference spectrum deviated from those of others antennas)",
                      default=0, output=True),

        xpowers=dict(doc="Array holding the *x*-values and their powers for calculating the baseline fit.",
                       default=lambda self: cr.hArray(float, [self.nofAntennas, self.nbins, self.ncoeffs], name="Powers of Frequency"),
                       workarray=True),

        powers=dict(doc="Array of integers, containing the powers to fit in the polynomial. (work vector)",
                      default=lambda self: cr.hArray(int, [self.nofAntennas, self.ncoeffs], range(self.ncoeffs)),
                      workarray=True),

        nselected_bins=dict(doc="""Number of clean bins after RFI removal. (output only)""",
                              default=0,
                              output=True),

        selected_bins=dict(doc="""Array of indices pointing to clean bins, i.e. with low RFI. (work vector)""",
                             default=lambda self: cr.hArray(int, self.small_spectrum, name="Selected bins"),
                             workarray=True),

        coeffs=dict(doc="""Polynomial coefficients of the baseline fit with the dimension ``[nofAntennas,ncoeffs]`` (output vector)""",
                      default=lambda self: cr.hArray(float, [self.nofAntennas, self.ncoeffs]),
                      output=True),

        frequency=dict(doc="Frequency values in Hz for each spectral channel (dimension: ``[nchannels]``)",
                         unit="Hz",
                         workarray=True,
                         default=lambda self: cr.hArray(float, [self.nofChannels], name="Frequency").fillrange(0., 1.) if not hasattr(self.spectrum.par, "xvalues") else self.spectrum.par.xvalues),

        spectrum=dict(doc="""Array with input spectrum either of dimension ``[nofAntennas,nchannels]`` or just ``[nchannels]`` for a single spectrum. Note that the frequency values for the array are expected to be provided as ``spectrum.par.xvalues=cr.hArray(float,[nofChannels],fill=...)`` otherwise provide the frequencies explicitly in ``frequency``"""),

        # work_frequency = dict(doc="Wrapper to frequencies with dimension ``[1,nchannels]`` even for a single spectrum.",
        #                       default=lambda self:cr.hArray(cr.asvec(self.frequency),dimensions=[1,self.nofChannels],properties=self.frequency),
        #                       export=False),

        work_spectrum=dict(doc="Wrapper to input spectrum with dimension [nofAntennas,nchannels] even for a single spectrum.",
                             default=lambda self: cr.hArray(cr.asvec(self.spectrum), dimensions=[self.nofAntennas, self.nofChannels], properties=self.spectrum, xvalues=self.frequency),
                             export=False),

        meanrms=dict(doc="""Estimate the mean RMS in the spectrum per antenna. (output vector)""",
                       default=0,
                       output=True),

        iteration=dict(doc="If 0 or **False**, then this is the first iteration, of the fitting otherwise the *n*-th iteration (information only at this point).",
                         default=0),

        verbose=dict(doc="""Print progress information.""",
                       default=True),

        plotlen=dict(default=2 ** 17,
                       doc="How many channels ``+/-`` the center value to plot during the calculation (to allow progress checking)."),

        plot_filename=dict(default="",
                             doc="Base filename to store plot in."),

        plot_antenna=dict(default=0,
                            doc="Which antenna to plot?"),

        plot_center=dict(default=0.5,
                           doc="Center plot at this relative distance from start of vector (0=left end, 1=right end)."),

        plot_start=dict(default=lambda self: max(int(self.nofChannels * self.plot_center) - self.plotlen, 0),
                          doc="Start plotting from this sample number."),

        plot_end=dict(default=lambda self: min(int(self.nofChannels * self.plot_center) + self.plotlen, self.nofChannels),
                        doc="End plotting before this sample number."),

        plot_finish=dict(default=lambda self: plotfinish(doplot=self.doplot, plotpause=False),
                           doc="Function to be called after each plot to determine whether to pause or not (see ::func::plotfinish)"),

        plot_name=dict(default="",
                         doc="Extra name to be added to plot filename."),

        doplot=dict(doc="""Plot progress information. If ``value > 1``, plot more information (the number of leves varies from task to task).""",
                      default=False)
    )

    def call(self, spectrum):
        """
        """
        pass

    def run(self):
        """Run the program.
        """
        if not type(self.spectrum) in cr.hAllArrayTypes:
            print "ERROR: please provide an cr.hArray as input for the positional argument 'spectrum'!"
            return

        if self.doplot:
            wasinteractive = plt.isinteractive()
            plt.ioff()

        self.t0 = time.clock()  # ; print "Reading in data and doing a double FFT."
        # Downsample spectrum
        if self.nbins > self.nofChannelsUsed / 8:
            print "Requested number of downsampled bins (", self.nbins, ") too large for the number of frequency channels (", self.nofChannelsUsed, "):"
            self.nbins = max(self.nofChannelsUsed / 8, 2)
            print "Resetting nbins to ", self.nbins, "!!!"
        if self.verbose:
            print time.clock() - self.t0, "s: Starting FitBaseline - Downsampling spectrum to ", self.nbins, "bins."
        self.small_spectrum[...].downsamplespikydata(self.rms[...], self.work_spectrum[..., self.numin_i:self.numax_i], 1.0)
        # Plotting
        if self.doplot > 2:
            print "Plotting full spectrum of refence antenna and downsampled spectra (doplot>=3) - can take some time..."
            plt.clf()
            plt.subplots_adjust(hspace=0.4)
            plt.subplot(2, 1, 0)
            # mvda_a = self.work_spectrum
            # print "plot_antenna %d" %(self.plot_antenna)
            # print "plot start, end = %d, %d" %(self.plot_start, self.plot_end)
            # print mvda_a[self.plot_antenna,self.plot_start:self.plot_end].vec()
            # mvda_a[self.plot_antenna,self.plot_start:self.plot_end].plot()
            # import pdb; pdb.set_trace()
            self.work_spectrum[self.plot_antenna, self.plot_start:self.plot_end].plot(color="blue", clf=False, title="Downsampled & Original Spectrum (antennas #" + str(self.plot_antenna) + ")")
            self.small_spectrum[self.plot_antenna].plot(xvalues=self.freqs, clf=False, color="red")
            plt.subplot(2, 1, 1)
            self.small_spectrum[...].plot(xvalues=self.freqs, clf=False, title="Downsampled Spectra (all antennas)")
            self.plot_finish(name=self.__taskname__ + "-downsampled_spectrum" + self.plot_name)
        # Normalize the spectrum to unity
        # self.meanspec=self.small_spectrum[...].mean()
        # Calculate RMS/amplitude for each bin
        self.ratio[...].div(self.rms[...], self.small_spectrum[...])
        # Get the RMS of the part of the spectrum where it is lowest (i.e. which is least affected by RFI)
        self.minblk = self.ratio[...].minstddevblock(cr.Vector(int, self.nbins, fill=self.minrms_blen), self.minrms, self.minmean)
        # Set limits for which spikes to ignore
        self.limit2 = self.minmean + self.minrms * self.rmsfactor
        self.limit1 = self.minmean - self.minrms * self.rmsfactor
        if self.doplot > 1:
            self.ratio[...].plot(xvalues=self.freqs, title="RMS/Amplitude (per channel block)", logplot="y", clf=True)
            cr.plotconst(self.freqs, self.limit1.mean()).plot(clf=False, color="red", logplot="y", linewidth=2)
            cr.plotconst(self.freqs, self.limit2.mean()).plot(clf=False, color="red", logplot="y", linewidth=2, xlabel="Frequency [MHz]", ylabel="RMS/Mean")
            self.plot_finish("Plotted relative RMS of downsampled spectrum (doplot>=2)", name=self.__taskname__ + "-rms_div_mean" + self.plot_name)
        # Now select bins where the ratio between RMS and amplitude is within the limits
        self.nselected_bins = self.selected_bins[..., 1:].findbetween(self.ratio[..., 1:-1], self.limit1, self.limit2)
        self.selected_bins += 1
        # We started the search only at bin #1 in self.ratio, so the indices returned are off by one
        # Make sure the endpoints are part of it
        self.nselected_bins += 2  # add the two endpoints
        self.selected_bins[..., 0].fill(0)
        self.endpoints = cr.hArray(int, [self.nofAntennas, 1], fill=self.nselected_bins - 1)
        self.selected_bins[...].set(self.endpoints[...], len(self.freqs) - 1)
        # Now copy only those bins with average RMS, i.e. likely with little RFI and take the log
        self.clean_bins_x.fill(0.)
        self.clean_bins_x[...].copy(self.freqs, self.selected_bins[...], cr.asvec(self.nselected_bins))
        self.clean_bins_y.fill(0.)
        self.clean_bins_y[...].copy(self.small_spectrum[...], self.selected_bins[...], cr.asvec(self.nselected_bins))
        #    self.weights.copy(self.clean_bins_y)
        if self.logfit:
            self.clean_bins_y[..., [0]:self.nselected_bins].log()
        if self.verbose:
            print time.clock() - self.t0, "s: Fitting baseline."
        if self.fittype == "POLY":
            if self.verbose:
                print "Performing a polynomial fit with ", self.ncoeffs, "coefficients."
            # Create the nth powers of the x value, i.e. the frequency, for the fitting
            self.xpowers[..., [0]:self.nselected_bins].linearfitpolynomialx(self.clean_bins_x[..., [0]:self.nselected_bins], self.powers[...])
            # Fit an nth order polynomial to the data
            self.chisquare = self.coeffs[...].linearfit(self.covariance[...], self.xpowers[...], self.clean_bins_y[...], self.nselected_bins)  # self.weights[...],
        else:
            if self.verbose:
                print "Performing a basis spline fit with ", self.ncoeffs + self.splineorder + 1 - 2, "break points."
            # Perform a Basis Spline fit to the data
            self.chisquare = self.coeffs[...].bsplinefit(self.covariance[...], self.xpowers[..., [0]:self.nselected_bins], self.clean_bins_x[..., [0]:self.nselected_bins], self.clean_bins_y[..., [0]:self.nselected_bins], self.numin_val_i, self.numax_val_i, self.splineorder + 1)

        # Calculate an estimate of the average RMS of the clean spectrum after baseline division
        self.ratio[...].copy(self.ratio, self.selected_bins[...], self.nselected_bins)
        meanrms = self.ratio[..., [0]:self.nselected_bins].meaninverse()

        # Now some quality control: check if some antenna fits deviate from the average
        if self.qualitycontrol and self.nofAntennas > 2:
            self.goodantennas = range(self.nofAntennas)
            self.fitted_bins_y.fill(0.0)
            self.fitted_bins_x.fillrange(self.numin, float(self.numax - self.numin) / (self.nbins - 1))
            self.small_average_spectrum.fill(0.0)
            if self.fittype == "POLY":
                self.fitted_bins_y[...].polynomial(self.fitted_bins_x, self.coeffs[...], self.powers[...])
            else:
                self.fitted_bins_y[...].bsplinecalc(self.fitted_bins_x, self.coeffs[...], self.numin_val_i, self.numax_val_i, self.splineorder + 1)

            keep_iterating = True
            nbandantennas_old = 0
            iteration = 0
            while keep_iterating:
                iteration += 1
                self.fitted_bins_y[self.goodantennas, ...].addto(self.small_average_spectrum)
                self.flattened_bins_y.copy(self.fitted_bins_y)
                self.flattened_bins_y /= self.small_average_spectrum
                self.ant_mean = cr.hArray(self.flattened_bins_y[...].mean())
                self.ant_rms = cr.hArray(self.flattened_bins_y[...].stddev(self.ant_mean.vec()))
                self.ant_mean_mean = self.ant_mean[self.goodantennas].mean().val()
                self.ant_mean_rms = self.ant_rms[self.goodantennas].mean().val()
                self.ant_rms_rms = self.ant_rms[self.goodantennas].stddev(self.ant_mean_rms).val()
                self.ant_rms.Find('outside', self.ant_mean_rms - self.nsigma * self.ant_rms_rms, self.ant_mean_rms + self.nsigma * self.ant_rms_rms)
                self.badantennas = list(self.ant_rms.Find('outside', self.ant_mean_rms - self.nsigma * self.ant_rms_rms, self.ant_mean_rms + self.nsigma * self.ant_rms_rms))
                self.nbadantennas = len(self.badantennas)
                keep_iterating = not nbandantennas_old == self.nbadantennas
                nbandantennas_old = self.nbadantennas
                self.goodantennas = [i for i in range(self.nofAntennas) if not i in self.badantennas]
                if self.nbadantennas > 0:
                    print "#FITBASELINE - qualitycontrol iteration #", iteration, "-", self.nbadantennas, "bad antennas found:", self.badantennas
                else:
                    print "#FITBASELINE - qualitycontrol iteration #", iteration, "- all antennas are good."
            print "#FITBASELINE - end qualitycontrol."

            if self.doplot:
                plt.clf()
                plt.subplots_adjust(hspace=0.4)
                plt.subplot(2, 1, 2)
                self.clean_bins_y[self.plot_antenna, 0:self.nselected_bins[self.plot_antenna]].plot(xvalues=self.clean_bins_x[0, 0:self.nselected_bins[self.plot_antenna]], logplot=False, color="blue", clf=False, title="Fitted baseline to downsampled spectrum (antenna #" + str(self.plot_antenna) + ")")
                self.fitted_bins_y[self.plot_antenna].plot(clf=False, color="red", logplot=False)
                plt.subplot(2, 1, 1)
                self.clean_bins_y[..., [0]:self.nselected_bins].plot(xvalues=self.clean_bins_x[..., [0]:self.nselected_bins], logplot=False, clf=False, title="Fitted baseline to downsampled spectrum (all antennas)")
                self.fitted_bins_y[...].plot(clf=False, logplot=False)
                self.plot_finish("Plotted downsampled and cleaned spectrum with baseline fit.", name=self.__taskname__ + "-clean_spectrum" + self.plot_name)
                if wasinteractive:
                    plt.ion()
                self.flattened_bins_y[self.goodantennas, ...].plot(title="Log of Fitted spetcra divided by average of (log of) all spectra", logplot=False)
                if self.badantennas:
                    self.flattened_bins_y[self.badantennas, ...].plot(clf=False, logplot=False, linestyle="-.")
                plt.legend(self.goodantennas + self.badantennas, ncol=2, title="Antenna #", prop=matplotlib.font_manager.FontProperties(family="Helvetica", size=7))
                self.plot_finish("Plotted fitted spectra of each antennas divided by average spectra of all antennas.", name=self.__taskname__ + "-difference_spectrum" + self.plot_name)

        if self.verbose:
            print time.clock() - self.t0, "s: Done fitting baseline."

        self.spectrum.setHeader(FitBaseline=self.ws.getParameters())
        if self.save_output:
            self.spectrum.writeheader(self.filename)
            print "Written spectrum to file, to read it back: sp=cr.hArrayRead('" + self.filename + "')"


CalcBaselineParameters = dict([(p, FitBaseline.parameters[p]) for p in
                               ['selected_bins',
                                'filename',
                                'save_output',
                                'logfit',
                                'verbose',
                                'nofChannels',
                                'doplot',
                                'spectrum',
                                'dim_spectrum',
                                'powers',
                                'work_spectrum',
                                #     'work_frequency',
                                'frequency',
                                'numin_val_i',
                                'numax_val_i',
                                'nofAntennas',
                                'polyorder',
                                'fittype',
                                'plotlen',
                                'plot_antenna',
                                'plot_start',
                                'plot_end',
                                'plot_finish',
                                'plot_name',
                                'plot_filename',
                                'plot_center'
                                ]])

CalcBaselineParameters.update(dict(
    baseline=dict(doc="Array containing the calculated baseline with the same dimensions as ``spectrum`` - can be provided as spectrum.par.baseline (will be created if not).",
                    #       default:lambda self:self.spectrum.par.baseline if hasattr(self.spectrum,"par") and hasattr(self.spectrum.par,"baseline") else cr.hArray(float,dimensions=self.spectrum,name="Baseline"),
                    default=None),

    addHanning=dict(default=True,
                      doc="Add a Hanning filter above ``nu_max`` and below ``nu_min`` to suppress out-of-band emission smoothly."),

    logfit=dict(default=lambda self: True if self.FitParameters == None else self.FitParameters["logfit"]),

    fittype=dict(default=lambda self: 'BSPLINE' if self.FitParameters == None else self.FitParameters["fittype"]),

#    work_baseline = dict(doc="Wrapper to baseline with dimension ``[nofAntennas,nchannels]`` even for a single spectrum.",
#                         default=lambda self:cr.hArray(cr.asvec(self.baseline), dimensions=[self.nofAntennas, self.nofChannels], properties=self.baseline, #xvalues=self.frequency),
#                         export=False),

    FitParameters=dict(doc="Parameters of the baseline fitting routine.",
                         default=lambda self: self.spectrum.getHeader("FitBaseline") if self.spectrum.hasHeader("FitBaseline") else None),

    numin=dict(doc="""Minimum frequency of useable bandwidth. Negative if to be ignored.""",
                 default=lambda self: -1 if self.FitParameters == None else self.FitParameters["numin"]),

    numax=dict(doc="""Maximum frequency of useable bandwidth. Negative if to be ignored.""",
                 default=lambda self: -1 if self.FitParameters == None else self.FitParameters["numax"]),

    numin_val_i=dict(doc="""Minimum frequency of useable bandwidth corresponding to ``numin_i`` (output)""",
                       default=lambda self: self.frequency[self.numin_i] if self.FitParameters == None else self.FitParameters["numin_val_i"]),

    numax_val_i=dict(doc="""Maximum frequency of useable bandwidth corresponding to ``numax_i`` (output)""",
                       default=lambda self: self.frequency[self.numax_i] if self.FitParameters == None else self.FitParameters["numax_val_i"]),

    numin_i=dict(doc="""Channel number in spectrum of the minium frequency where to calculate baseline. Apply hanning taper below.""",
                   default=lambda self: max(self.frequency.findlowerbound(self.numin).val(), 0) if self.numin > 0 else 0,
                   output=True),

    numax_i=dict(doc="""Channel number in spectrum of the maxium frequency where to calculate baseline. Apply hanning taper above.""",
                   default=lambda self: min(self.frequency.findlowerbound(self.numax).val(), self.nofChannels) if self.numax > 0 else self.nofChannels,
                   output=True),

    height_ends=dict(doc="""The heights of the baseline at the left and right endpoints of the usable bandwidth where a hanning function is smoothly added.""",
                       default=lambda self: cr.hArray(float, [2, self.nofAntennas])),

    coeffs=dict(doc="Polynomial coefficients of the baseline fit with the dimension ``[nofAntennas, ncoeffs]`` or ``[ncoeffs]``",
                  # input=True,
                  default=lambda self: cr.hArray(float, [1, 1], name="Coefficients", fill=0) if self.FitParameters == None else self.FitParameters["coeffs"]),

    dim_coeffs=dict(doc="Dimension of the coefficients array (which should be ``[nofAntennas, ncoeff]`` or ``[ncoeff]`` for ``nofAntennas==1``)",
                      default=lambda self: self.coeffs.shape()),

    nofAntennasCoeffs=dict(doc="Number of antennas in coeffcient array.",
                             default=lambda self: 1 if len(self.dim_coeffs) == 1 else self.dim_coeffs[0]),

    ncoeffs=dict(doc="Number of coefficients for the polynomial.",
                   default=lambda self: self.dim_coeffs[1] if len(self.dim_coeffs) >= 2 else self.dim_coeffs[0]),

    splineorder=dict(doc="Order of the polynomial to fit for the BSpline, ``splineorder=3`` is a bicubic spline.",
                       default=lambda self: 3 if self.FitParameters == None else self.FitParameters["splineorder"]),

    work_coeffs=dict(doc="Array with coefficients in the form ``[nofAntennas, ncoeff]``",
                       default=lambda self: cr.hArray(cr.asvec(self.coeffs), dimensions=[self.nofAntennasCoeffs, self.ncoeffs])),

    invert=dict(doc="Invert the baseline so that it can later simply be multiplied to take out the gain variations.",
                  default=True),

    HanningUp=dict(doc="Let the Hanning filter at the ends go up rather than down (useful if one wants to divide by baseline and suppress out-of-band-noise)",
                     default=True),

    normalize=dict(doc="If **True**, normalize the baseline to have a total sum of unity.",
                     default=True),

    powerlaw=dict(doc="Multiply gain curve with a powerlaw depending on value. 'GalacticT': return a powerlaw according to Galactic radio emission in Kelvin; alpha (i.e. a number): multiply with powerlaw of form :math`:\\nu^\\alpha`.",
                    default=False)

    ))


class CalcBaseline(tasks.Task):
    """
    Usage::

      >>> CalcBaseline(spectrum,par1=...,par2=...)

    Calculate a smooth baseline from a set of coefficients that
    determine the baseline (e.g., as calculated by :class:FitBaseline) and an
    array of frequencies (which need not be equi-spaced).

    The frequencies can be provided with the .par.xvalues array of the
    input spectrum (if available). Otherwise, they have to be
    explicitly provided.

    The result will be returned in an array ``Task.baseline``. This
    baseline will also be attached to the .par dict of the input
    spectrum, i.e. ``spectrum.par.baseline``.

    The task can return the inverse of the baseline (if
    ``invert=True``), i.e. what one needs to multiply a measured
    spectrum with.

    Note, sometimes the fit can go wrong if ``logfit=True`` the
    interpolation has wiggles. In this case the exponent of a large
    number is calculated, yielding a numerical error. In this case
    switch set ``logfit=False`` (to see what is going on).


    **Gain Calibration (DRAFT!)**

    Knowing that the antenna is exposed to a noise with a certain
    power, one can use this to make a rough estimate of the absolute
    gain of the antenna. E.g., in case for LOFAR antennas the spectrum
    measured in one antenna should follow a :math:`\\nu^{-0.5}` power law due
    to emission from the Galaxy (Milky Way). Hence, one can multiply
    the baseline one has fitted to the measured spectrum with a power
    law, so that after dividing the gain curve one reproduces the
    expected behaviour (i.e., Galactic noise). Note: if one wants to
    use calcbaseline for gain calibration one obviously has to provide
    a power law that is the inverse of the desired function.

    Powerlaws can be specified with the ``Task.powerlaw`` parameter,
    which is either just the power law index, or a tuple with
    amplitude and index, or a keyword (e.g., 'Galactic', of
    'GalacticT').


    The noise temperature of an antenna is:

    ``Tnoise = Power [Watts] /Bandwidth [Hz] / kb (Boltzmann constant) = 7.24296*10^22 K``

    The sky tempertature is according to Falcke & Gorham (2004)
    roughly: ``T_sky = 32*( (freq/408e6)^-2.5 )`` (``T_sky`` in K, freq in
    Hz)


    The recevied power of an antenna is ``Prec = Aeff * S``, where ``S`` is
    the power density (in ``W/m^2``).

    The effective area is

    ``Aeff = gain* lambda^2/(4 pi)``

    The power density is ``S=E^2/R`` (i.e., ``Electric field^2 / free space
    impedance`` (376.7 Ohm))


    The System Equivalent Flux Density (SEFD) is

    Flux density for bandwidth limlied pulses can be rougly converted
    to Field strength through
    ``Snu= Enu epsilon_0 c Bandwidth`` (Enu in Volt/Meter/MHz in SI units
    ..., check this formula)

    to be continued ....

    In the following a short description from Andreas Horneffer (in
    German) on how to do a simple gain correction.

    ::

      Unter der Annahme, dass der ganze Himmel eine konstante, und bekannte
      Strahlungstemperatur hat, das gemessene Rauschen davon dominiert ist,
      und man das Gain (die Richtwirkung) der Antennen kennt.

      Die Strahlungstemperatur des Himmels ist nach Falcke & Gorham (2002) etwa:
      T_sky = 32*( (freq/408e6)^-2.5 );
      (T_sky in K, freq in Hz)

      Die gemessene "Rauschtemperatur" ist:
      T_noise = P_noise / dNu / kB
      (T_noise in K, P_noise in W, dNu (Bandbreite) in Hz, kB die Boltzmann-Konstante)

      Die Rauschleistung ist: P_noise = V^2/R (P_noise in W, V (gemessene
      Spannung) in V, R (angenommene ADC-Impedanz) in Ohm) Die ADC-Impedanz
      kuertzt sich nachher weg. Wenn Du die Rauschleistung als Funktion der
      Frequenz nach der FFT fuers Kalibrieren verwenden willst, aber die
      Kalibration fuer die Zeitreihe nutzen willst, musst du noch die
      Normierung der FFT korrigieren. (Haengt von der verwendeten
      FFT-Routine ab.)

      Die Verstaerkung der Elektronik ist dann: K = T_sky / T_noise (K ist
      einheitenloses Verhaeltnis der Leistungen)

      Die Feldstaerke an der Antenne ist dann:

      E = sqrt( 4*pi*freq^2*mu0 / G(d,freq) * K * V^2/R )

      (E in V/m, mu0 die magnetische Feldkonstante,
      G(d,freq) das Gain der Antennen in die Richtung d, bei Frequenz freq
      als einheitenloses Verhaeltnis der Empfindlichkeit der Antennen zu der
      Empfindlichkeit eine isotropen Strahlers)

      Als Gain der LOFAR Antennen kann man vermutlich einfach das Gain der LOPES
      Antennen verwenden. (Natuerlich fuer die Ausrichtung der Dipole rotiert.)


    """
    parameters = CalcBaselineParameters

    def call(self, spectrum):
        """
        """
        pass

    def run(self):
        """
        """
        if not type(self.spectrum) in cr.hAllArrayTypes:
            print "ERROR: please provide an cr.hArray as input for the positional argument 'spectrum'!"
            return

        self.t0 = time.clock()
        if not hasattr(self.spectrum, "par"):
            setattr(self.spectrum, "par", cr.hArray_par())

        if not type(self.baseline) in cr.hAllArrayTypes:
            if hasattr(self.spectrum.par, "baseline"):
                self.baseline = self.spectrum.par.baseline
            else:
                self.spectrum.par.baseline = cr.hArray(float, properties=self.spectrum, name="Baseline")
                self.baseline = self.spectrum.par.baseline
        else:
            self.spectrum.par.baseline = self.baseline

        self.work_baseline = cr.hArray(cr.asvec(self.baseline), dimensions=[self.nofAntennas, self.nofChannels], properties=self.baseline, xvalues=self.frequency)

        if self.fittype == "POLY":
#            self.work_baseline[...,[self.numin_i]:[self.numax_i]].polynomial(self.work_frequency[...,[self.numin_i]:[self.numax_i]],self.work_coeffs[...],self.powers[...])
            self.work_baseline[..., [self.numin_i]:[self.numax_i]].polynomial(self.frequency[self.numin_i:self.numax_i], self.work_coeffs[...], self.powers[...])
        else:
#            self.work_baseline[...,[self.numin_i]:[self.numax_i]].bsplinecalc(self.frequency[...,[self.numin_i]:[self.numax_i]],self.work_coeffs[...],self.numin_val_i,self.numax_val_i,self.splineorder+1)
            self.work_baseline[..., [self.numin_i]:[self.numax_i]].bsplinecalc(self.frequency[self.numin_i:self.numax_i], self.work_coeffs[...], self.numin_val_i, self.numax_val_i, self.splineorder + 1)

        if self.powerlaw:
            if isinstance(self.powerlaw, (int, long, float)):
                alpha = -self.powerlaw  # if self.invert else self.powerlaw
                a = 1. / self.frequency[self.numin_i] ** alpha
            elif self.powerlaw == "GalacticT":
                alpha = -2.5
                nu0 = 408e6 / {"T": 1e12, "G": 1e9, "M": 1e6, "k": 1e3, "": 1}[self.frequency.getUnitPrefix()]
                a = 32 * (self.frequency[self.numin_i] / nu0) ** -2.5  # Falcke & Gorham 2004
            else:
                raise TypeError("CalcBaseline - powerlaw parameter has wrong type")
            if self.invert:
                a = 1 / a
                alpha = -alpha
            if self.logfit:
                a = math.log10(a)
                alpha = alpha / math.log(10.)
                self.work_baseline[..., self.numin_i:self.numax_i].loglinearfunctionadd(self.frequency[self.numin_i:self.numax_i], a, alpha)
            else:
                self.work_baseline[..., self.numin_i:self.numax_i].powerlawmul(self.frequency[self.numin_i:self.numax_i], a, alpha)

        # Now add nice ends (Hanning Filters) to the frequency range to suppress the noise outside the usuable bandwidth
        # Left end
        if self.addHanning:
            self.height_ends[0, ...].copy(self.work_baseline[..., self.numin_i])
            self.height_ends[1, ...].copy(self.work_baseline[..., self.numax_i - 1])
            self.factor = cr.hArray(float, self.nofAntennas, fill=6.9)  # Factor 1000 in log
            if not self.logfit:
                self.factor.fill(self.height_ends[0])
                self.factor *= 1000.0
            self.work_baseline[..., 0:self.numin_i].gethanningfilterhalf(cr.Vector(self.factor), cr.asvec(self.height_ends[0]) - (0 if self.HanningUp else cr.asvec(self.factor)), cr.Vector(bool, self.nofAntennas, fill=self.HanningUp))
            # Right end
            if not self.logfit:
                self.factor.fill(self.height_ends[1])
                self.factor *= 1000.0
            self.work_baseline[..., self.numax_i:].gethanningfilterhalf(cr.Vector(self.factor), cr.asvec(self.height_ends[1]) - (0 if self.HanningUp else cr.asvec(self.factor)), cr.Vector(bool, self.nofAntennas, fill=not self.HanningUp))
        if self.logfit:
            self.work_baseline.min(40)  # avoid numerical trouble
            self.work_baseline.max(-40)  # avoid numerical trouble
            self.work_baseline.exp()
        if self.doplot:
            print "#Plotting the calculated baseline of the reference antenna."
            wasinteractive = plt.isinteractive()
            plt.ioff()
            plt.clf()
            if self.doplot > 1:
                plt.subplots_adjust(hspace=0.4)
                plt.subplot(2, 1, 0)
            self.work_spectrum[self.plot_antenna, self.plot_start:self.plot_end].plot(color="blue", clf=False, title="Baseline (antenna #" + str(self.plot_antenna) + ")")
            self.work_baseline[self.plot_antenna, self.plot_start:self.plot_end].plot(color="red", clf=False)
            if self.doplot > 1:
                print "#Plotting the calculated baselines of all antennas."
                plt.subplot(2, 1, 1)
                self.work_baseline[..., self.plot_start:self.plot_end].plot(title="Baseline (all antennas)", clf=False)
            self.plot_finish(name=self.__taskname__ + self.plot_name)
            if wasinteractive:
                plt.ion()
        if self.invert:
            self.work_baseline.inverse()  # -> 1/baseline
        if self.normalize:
            self.work_baseline[...] /= self.work_baseline[..., self.numin_i:self.numax_i].mean()
        if self.save_output:
            self.spectrum.writeheader(self.filename)
            print "Written spectrum to file, to read it back: sp=cr.hArrayRead('" + self.filename + "')"
        if self.verbose:
            print time.clock() - self.t0, "s: Done CalcBaseline."


ApplyBaselineParameters = dict(
    [(p, CalcBaseline.parameters[p]) for p in
     [
     'FitParameters',
     'spectrum',
     'frequency',
#     'work_frequency',
     'work_spectrum',
     'baseline',
#     'work_baseline',
     'filename',
     'save_output',
     'numax_i',
     'numin_i',
     'numax',
     'numin',
     'verbose',
     'dim_spectrum',
     'nofChannels',
     'nofAntennas',
     'doplot',
     'plotlen',
     'plot_start',
     'plot_end',
     'plot_filename',
     'plot_name',
     'plot_finish',
     'plot_center'
         ]
     ]
    )

ApplyBaselineParameters.update(dict(

    filename=dict(default=lambda self: "tmpspec.clean.pcr" if not self.spectrum.hasHeader("filename") else root_filename(self.spectrum.getHeader("filename")) + ".clean",
                    doc="Filename to write output to."),

    # FitParameters = dict(doc="Parameters of the baseline fitting routine.",
    #                      default= lambda self: self.spectrum.getHeader("FitBaseline") if self.spectrum.hasHeader("FitBaseline") else None),

    # numin = dict(doc="""Minimum frequency of useable bandwidth. Negative if to be ignored.""",
    #              default=lambda self: -1 if self.FitParameters==None else self.FitParameters["numin"]),

    # numax = dict(doc="""Maximum frequency of useable bandwidth. Negative if to be ignored.""",
    #              default=lambda self: -1 if self.FitParameters==None else self.FitParameters["numax"]),

    plotchannel=dict(doc="Which channel to plot.",
                       default=0),

    adaptive_peak_threshold=dict(doc="If **True** then calculate the threshold above which to cut peaks in ``nbins`` separate bins and thus let it vary over the spectrum",
                                   default=False),

    apply_baseline=dict(doc="If **True** then divide spectrum by baseline before removing peaks.",
                          default=True),

    mean=dict(doc="Median mean value of blocks in downsampled spectrum - used to replace flagged data with (output only)",
                default=1),

    rms=dict(doc="Median RMS value of blocks in downsampled spectrum - used to calculate threshold for cutting peaks (output only)",
               default=lambda self: cr.Vector(float, self.nofAntennas, fill=0.0),
               output=True),

    means=dict(default=lambda self: cr.hArray(float, [self.nofAntennas, self.nbins]),
                 doc="Mean value per block in input spectrum"),

    stddevs=dict(default=lambda self: cr.hArray(float, [self.nofAntennas, self.nbins]),
                   doc="Standard deviation per block in input spectrum"),

    nofChannelsUsed=dict(doc="""Number of channels remaining after downsampling and ignoring edges.""",
                           default=lambda self: self.numax_i - self.numin_i),

    rmsfactor=dict(doc="""Factor above the RMS in each channel at which a channel is considered poluted by RFI.""",
                     default=5.0),

    nbins=dict(doc="""The number of bins to be used to subdivide spectrum for selecting the best RMS.""",
                 default=lambda self: max(self.nofChannelsUsed / 256, min(256, self.nofChannelsUsed / 8))),

    blocklen=dict(doc="""The block length of one bin used to subdivide spectrum for selecting the best RMS.""",
                    default=lambda self: self.nofChannelsUsed / self.nbins),

    ndirty_channels=dict(doc="""Number of dirty channels to be removed as RFI. (output only)""",
                           default=0,
                           output=True),

    dirty_channels=dict(doc="""Array of indices pointing to dirty bins, i.e. with high RFI. (work vector)""",
                          default=lambda self: cr.hArray(int, dimensions=self.work_spectrum, name="Dirty bins"),
                          workarray=True)
    ))


class ApplyBaseline(tasks.Task):
    """
    Usage::

      >>> ApplyBaseline(spectrum)

    contains ``spectrum.par.baseline``

    Also does RFI excision.
    """
    parameters = ApplyBaselineParameters

    def call(self, spectrum):
        pass

    def run(self):
        self.t0 = time.clock()

        if not type(self.spectrum) in cr.hAllArrayTypes:
            print "ERROR: please provide an cr.hArray as input for the positional argument 'spectrum'!"
            return

        if not hasattr(self.work_spectrum, "par"):
            setattr(self.spectrum, "par", cr.hArray_par())

        if hasattr(self.spectrum.par, "baseline") and self.baseline == None:
            self.baseline = self.spectrum.par.baseline

        self.work_baseline = cr.hArray(cr.asvec(self.baseline), dimensions=[self.nofAntennas, self.nofChannels], properties=self.baseline, xvalues=self.frequency)
        if self.apply_baseline:
            self.work_spectrum *= self.work_baseline

        # Some ugly way to fool the python bindings, since right now they down return data in the argument variables
        # self.minblk=self.work_spectrum[...,self.numin_i:self.numax_i].minstddevblock(cr.Vector(int,[self.nofAntennas],fill=self.blocklen),self.minrms,self.minmean)

        if self.adaptive_peak_threshold:
            self.means[...].downsamplespikydata(self.stddevs[...], self.work_spectrum[...], 1.0)
            self.mean = self.work_spectrum[..., self.numin_i:self.numax_i].mean().val()
            self.limit = cr.hArray(properties=self.work_baseline, name="Threshold", fill=0.0)
            self.limit[...].upsample(self.stddevs[...])
            self.limit *= self.rmsfactor
            self.limit[...].upsample(self.means[...])
        else:
            self.means[...].downsamplespikydata(self.stddevs[...], self.work_spectrum[..., self.numin_i:self.numax_i], 1.0)
            self.rms = self.stddevs[...].sortmedian()
            self.mean = self.means[...].sortmedian()
            self.limit = self.mean + self.rms * self.rmsfactor
        if self.doplot:
            wasinteractive = plt.isinteractive()
            plt.ioff()
            self.work_spectrum[self.plotchannel, self.plot_start:self.plot_end].plot(title="Spectrum with baseline applied", color='red')
            if self.adaptive_peak_threshold:
                self.limit[self.plotchannel, self.plot_start:self.plot_end].plot(clf=False, color="green", logplot="y")
            else:
                cr.plotconst(self.frequency[self.plot_start:self.plot_end], self.limit[self.plotchannel]).plot(clf=False, color="green", logplot="y")
        # Now select bins where the ratio between RMS and amplitude is within the limits
        if self.adaptive_peak_threshold:
            self.ndirty_channels = self.dirty_channels[...].findgreaterthanvec(self.work_spectrum[...], self.limit[...])
        else:
            self.ndirty_channels = self.dirty_channels[...].findgreaterthan(self.work_spectrum[...], self.limit)
        # Now copy only those bins with average RMS, i.e. likely with little RFI and take the log
        self.work_spectrum[..., self.dirty_channels[..., [0]:cr.asvec(self.ndirty_channels)]] = self.mean
        if self.save_output:
            self.spectrum.setHeader(filename=self.filename)
            self.spectrum.write(self.filename)
            print "Written spectrum to file, to read it back: sp=cr.hArrayRead('" + self.filename + "')"
        if self.verbose:
            print time.clock() - self.t0, "s: Done ApplyBaseline."
        if self.doplot:
            self.work_spectrum[self.plotchannel, self.plot_start:self.plot_end].plot(color='blue', clf=False)
            self.plot_finish(name=self.__taskname__ + self.plot_name)
            if wasinteractive:
                plt.ion()

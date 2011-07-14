
"""
Spectrum documentation
======================

Usage::

  >>> sp=hArrayRead('/Users/falcke/data/Pulses/oneshot_level4_CS017_19okt_no-0.h5.spec.pcr')
"""

from pycrtools import *
from pycrtools.tasks.shortcuts import *
import pycrtools.tasks as tasks
import time
import os
#from pycrtools import IO

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
    val=hArray(float,dimensions=[self.nbins],name="Frequency",units=("M","Hz"))
    val.downsample(self.frequency[self.numin_i:self.numax_i])
    return val

def fitbaseline_calc_numin_i(self):
    """
    """
    if self.numin>0: numin_i=self.frequency.findlowerbound(self.numin).val()
    else: numin_i=2
    numin_i=max(numin_i-int(numin_i*self.extendfit),2)
    return numin_i

def fitbaseline_calc_numax_i(self):
    """
    """
    if self.numax>0: numax_i=self.frequency.findlowerbound(self.numax).val()
    else: numax_i=len(self.frequency)
    numax_i=min(numax_i+int((len(self.frequency)-numax_i)*self.extendfit),len(self.frequency)-1)
    return numax_i

class FitBaseline(tasks.Task):
    """
    Usage::

      >>> FitBaseline(input_spectrum, **keywords)

    Function to fit a baseline using a polynomial function
    (``fittype='POLY'``) or a basis spine fit (default: ``fittype='BSPLINE'``)
    to a spectrum while trying to ignore positive spikes in the fit
    (e.g., those coming from RFI = Radio Frequency Interference).

    The input_spectrum is a float array with input spectrum either of
    dimension ``[nofAntennas,nofChannels]`` or just ``[nofChannels]`` for a
    single spectrum. Note that the frequency values for the array are
    expected to be provided as
    ``input_spectrum.par.xvalues=hArray(float,[nofChannels],fill=...)``
    the same you would use to provide xvalues for plotting. You can
    also provide the frequencies explicitly in the keyword
    ``frequency``, otherwise a simple numbering is assumed
    ``frequency=[0,1,2,3...]``.

    Returns its own workspace.

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
    parameters = {

        "filename":{default:lambda self: "tmpspec.pcr" if not self.spectrum.hasHeader("FILENAME") else self.spectrum.getHeader("FILENAME"),
            doc: "Filename to write output to"},    

        "save_output":{default:False,
                       doc:"If **True** save the results in the header files of the input spectrum with file name given in ``filename``"},

        "nbins":{doc:"The number of bins in the downsampled spectrum used to fit the baseline.",
                 default: lambda self:max(self.nofChannels/256,min(256,self.nofChannels/8))},

        "polyorder":{doc:"Order of the polynomial to fit. (output only)",
                     default:lambda self:self.ncoeffs-1,
                     output:True},

        "rmsfactor":{doc:"Factor above and below the RMS in each bin at which a bin is no longer considered due to too many spikes.",
                     default:3.0},

        "logfit":{doc:"Actually fit the polynomial to the log of the (downsampled) data. (Hence you need to .exp the baseline afterwards).",
                  default:True},

        "fittype":{doc:"""Determine which type of fit to do: ``fittype='POLY'`` - do a polynomial fit, else ('BSPLINE') do a basis spline fit (default).""",
                   default:"BSPLINE"},

        "nofAntennas":{doc:"""Number of antennas in input spectrum.""",
                       default:lambda self:1 if len(self.dim_spectrum)==1 else self.dim_spectrum[0]},

        "nofChannels":{doc:"""Number of channels in input spectrum.""",
                       default:lambda self:self.dim_spectrum[0] if len(self.dim_spectrum)==1 else self.dim_spectrum[1]},

        "nofChannelsUsed":{doc:"""Number of channels remaining after downsampling and ignoring edges.""",
                           default:lambda self:self.numax_i-self.numin_i},

        "dim_spectrum":{doc:"""Dimension of input spectrum (typically *n* antennas times *m* spectral points) or just one dimensional for one antenna.""",
                        default:lambda self:self.spectrum.getDim()},

        "ncoeffs":{doc:"""Number of coefficients for the polynomial.""",
                   default:18},

        "splineorder":{doc:"""Order of the polynomial to fit for the BSpline, ``splineorder=3`` is a bicubic spline.""",
                       default:3},

        "numin":{doc:"""Minimum frequency of useable bandwidth. Negative if to be ignored.""",
                 default:-1},

        "numax":{doc:"""Maximum frequency of useable bandwidth. Negative if to be ignored.""",
                 default:-1},

        "numin_val_i":{doc:"""Minimum frequency of useable bandwidth corresponding to ``numin_i`` (output)""",
                       default:lambda self:self.frequency[self.numin_i]},

        "numax_val_i":{doc:"""Maximum frequency of useable bandwidth corresponding to ``numax_i`` (output)""",
                       default:lambda self:self.frequency[self.numax_i]},

        "numin_i":{doc:"""Channel number in spectrum of the minimum frequency of the useable bandwidth. Negative if to be ignored.""",
                   default:fitbaseline_calc_numin_i,
                   output:True},

        "numax_i":{doc:"""Channel number in spectrum of the maximum frequency of the useable bandwidth. Negative if to be ignored.""",
                   default:fitbaseline_calc_numax_i,
                   output:True},

        "extendfit":{doc:"""Extend the fit by this factor at both ends beyond ``numax`` and ``numin``. The factor is relative to the unused bandwidth. Use this to make sure there is a stable solution at least between ``numax``/``numin``, i.e. avoid wiggles at the endpoint.""",
                     default:0.1},

        "freqs":{doc:"""Array of frequency values of the downsampled spectrum. (work vector)""",
                 default:fitbaseline_calc_freqs,
                 workarray:False},

        "small_spectrum":{doc:"""Array of power values holding the downsampled spectrum. (work vector)""",
                          default:lambda self:hArray(float,[self.nofAntennas,self.nbins],name="Binned Spectrum",units="a.u.",xvalues=self.freqs,par=("logplot","y")),
                          workarray:True},

        "rms":{doc:"""Array of RMS values of the downsampled spectrum. (work vector)""",
               default:lambda self:hArray(properties=self.small_spectrum, name="RMS of Spectrum"),
               workarray:True},

        "minmean":{doc:"Mean value of data in the part of downsampled spectrum with the smallest RMS (output only)",
                   default:lambda self:Vector(float,[self.nofAntennas],fill=0.0),
                   output:True},

        "minrms":{doc:"RMS value of data in the part of downsampled spectrum with the smallest RMS (output only)",
                  default:lambda self:Vector(float,[self.nofAntennas],fill=0.0),
                  output:True},

        "minrms_blen":{doc:"Block length within downsampled data to look for the cleanest part of the spectrum.",
                       default:lambda self:min(64,max(self.nbins/16,4))},

        "chisquare":{doc:"""Returns the :math`\\chi^2` of the baseline fit. (output only)""",
                     default:0,
                     output:True},

        "weights":{doc:"""Array of weight values for the fit. (work vector)""",
                   default:lambda self:hArray(properties=self.small_spectrum, name="Fit Weights"),
                   workarray:True},

        "ratio":{doc:"""Array holding the ratio between RMS and power of the downsampled spectrum. (work vector)""",
                 default:lambda self:hArray(properties=self.small_spectrum,name="RMS/Amplitude",par=("logplot",False)),
                 workarray:True},

        "covariance":{doc:"""Array containing the covariance matrix of the fit. (output only)""",
                      default:lambda self:hArray(float,[self.nofAntennas,self.ncoeffs,self.ncoeffs]),
                      output:True},

        "bwipointer":{doc:"""Pointer to the internal BSpline workspace as integer. Don't change! """,
                      default:0,
                      export:False,
                      output:True},

        "clean_bins_x":{doc:"""Array holding the frequencies of the clean bins. (work vector)""",
                        default:lambda self:hArray(dimensions=[self.nofAntennas,self.nbins],properties=self.freqs,name="Frequency"),
                        workarray:True},

        "clean_bins_y":{doc:"""Array holding the powers of the clean bins. (work vector)""",
                        default:lambda self:self.small_spectrum,
                        workarray:True},

        "xpowers":{doc:"Array holding the *x*-values and their powers for calculating the baseline fit.",
                   default:lambda self:hArray(float,[self.nofAntennas,self.nbins,self.ncoeffs],name="Powers of Frequency"),
                   workarray:True},

        "powers":{doc:"Array of integers, containing the powers to fit in the polynomial. (work vector)",
                  default:lambda self:hArray(int,[self.nofAntennas,self.ncoeffs],range(self.ncoeffs)),
                  workarray:True},

        "nselected_bins":{doc:"""Number of clean bins after RFI removal. (output only)""",
                          default:0,
                          output:True},

        "selected_bins":{doc:"""Array of indices pointing to clean bins, i.e. with low RFI. (work vector)""",
                         default:lambda self:hArray(int,self.small_spectrum,name="Selected bins"),
                         workarray:True},

        "coeffs":{doc:"""Polynomial coefficients of the baseline fit with the dimension ``[nofAntennas,ncoeffs]`` (output vector)""",
                  default:lambda self:hArray(float,[self.nofAntennas,self.ncoeffs]),
                  output:True},

        "frequency":{doc:"Frequency values in Hz for each spectral channel (dimension: ``[nchannels]``)",
                     unit:"Hz",
                     workarray:True,
                     default:lambda self:hArray(float,[self.nofChannels],name="Frequency").fillrange(0.,1.) if not hasattr(self.spectrum.par,"xvalues") else self.spectrum.par.xvalues},

        "spectrum":{doc:"Array with input spectrum either of dimension ``[nofAntennas,nchannels]`` or just ``[nchannels]`` for a single spectrum. Note that the frequency values for the array are expected to be provided as ``spectrum.par.xvalues=hArray(float,[nofChannels],fill=...)`` otherwise provide the frequencies explicitly in ``frequency``"},


#        "work_frequency":{doc:"Wrapper to frequencies with dimension ``[1,nchannels]`` even for a single spectrum.",
#                      default:lambda self:hArray(asvec(self.frequency),dimensions=[1,self.nofChannels],properties=self.frequency),export:False},

        "work_spectrum":{doc:"Wrapper to input spectrum with dimension [nofAntennas,nchannels] even for a single spectrum.",
             default:lambda self:hArray(asvec(self.spectrum),dimensions=[self.nofAntennas,self.nofChannels],properties=self.spectrum,xvalues=self.frequency),export:False},
        
        "meanrms":{doc:"""Estimate the mean rms in the spectrum per antenna. (output vector)""",
           default:0,output:True},

        "iteration":{doc:"If zero or False, then this is the first iteration, of the fitting otherwise the nth iteration (information only at this point).",default:0},

        "meanrms":{doc:"""Estimate the mean RMS in the spectrum per antenna. (output vector)""",
                   default:0,
                   output:True},

        "iteration":{doc:"If 0 or **False**, then this is the first iteration, of the fitting otherwise the *n*-th iteration (information only at this point).",
                     default:0},

        "verbose":{doc:"""Print progress information.""",
                   default:True},

        "plotlen":{default:2**17,
                   doc:"How many channels ``+/-`` the center value to plot during the calculation (to allow progress checking)."},

        "plot_filename":{default:"",doc:"Base filename to store plot in."},

        "plot_antenna":{default:0,doc:"Which antenna to plot?"},

        "plot_center":{default:0.5,
                       doc:"Center plot at this relative distance from start of vector (0=left end, 1=right end)."},

        "plot_start":{default:lambda self: max(int(self.nofChannels*self.plot_center)-self.plotlen,0),
                      doc:"Start plotting from this sample number."},

        "plot_end":{default:lambda self: min(int(self.nofChannels*self.plot_center)+self.plotlen,self.nofChannels),
                    doc:"End plotting before this sample number."},

        "plot_finish":{default: lambda self:plotfinish(doplot=self.doplot),doc:"Function to be called after each plot to determine whether to pause or not (see ::func::plotfinish)"},

        "doplot":{doc:"""Plot progress information. If ``value > 1``, plot more information (the number of leves varies from task to task).""",
                  default:False}
    }

    def call(self,spectrum):
        """
        """
        pass

    def run(self):
        """Run the program.
        """
        if not type(self.spectrum) in hAllArrayTypes:
            print "ERROR: please provide an hArray as input for the positional argument 'spectrum'!"
            return

        if self.doplot:
            wasinteractive=plt.isinteractive()
            plt.ioff();
        
        self.t0=time.clock() #; print "Reading in data and doing a double FFT."
        #Donwsample spectrum
        if self.nbins>self.nofChannelsUsed/8:
            print "Requested number of downsampled bins (",self.nbins,") too large for the number of frequency channels (",self.nofChannelsUsed,"):"
            self.nbins=max(self.nofChannelsUsed/8,2)
            print "Resetting nbins to ",self.nbins,"!!!"
        if self.verbose:
            print time.clock()-self.t0,"s: Starting FitBaseline - Downsampling spectrum to ",self.nbins,"bins."
        self.small_spectrum[...].downsamplespikydata(self.rms[...],self.work_spectrum[...,self.numin_i:self.numax_i],1.0)
        #Plotting
        if self.doplot>2:
            print "Plotting full spectrum of refence antenna and downsampled spectra (doplot>=3) - can take some time..."
            plt.clf()
            plt.subplots_adjust(hspace=0.4)
            plt.subplot(2,1,0)
            self.work_spectrum[self.plot_antenna,self.plot_start:self.plot_end].plot(color="blue",clf=False,title="Downsampled & Original Spectrum (antennas #"+str(self.plot_antenna)+")")
            self.small_spectrum[self.plot_antenna].plot(xvalues=self.freqs,clf=False,color="red")
            plt.subplot(2,1,1)
            self.small_spectrum[...].plot(xvalues=self.freqs,clf=False,title="Downsampled Spectra (all antennas)")
            self.plot_finish(name=self.__taskname__+"-donwsampled_spectrum")
        #Normalize the spectrum to unity
        #self.meanspec=self.small_spectrum[...].mean()
        #Calculate RMS/amplitude for each bin
        self.ratio[...].div(self.rms[...],self.small_spectrum[...])
        #Get the RMS of the part of the spectrum where it is lowest (i.e. which is least affected by RFI)
        self.minblk=self.ratio[...].minstddevblock(Vector(int,[self.nbins],fill=self.minrms_blen),self.minrms,self.minmean)
        #Set limits for which spikes to ignore
        self.limit2=self.minmean+self.minrms*self.rmsfactor
        self.limit1=self.minmean-self.minrms*self.rmsfactor
        if self.doplot>1:
            self.ratio[...].plot(xvalues=self.freqs,title="RMS/Amplitude (per channel block)",logplot="y",clf=True)
            plotconst(self.freqs,(self.limit1).val()).plot(clf=False,color="red",logplot="y",linewidth=2)
            plotconst(self.freqs,(self.limit2).val()).plot(clf=False,color="red",logplot="y",linewidth=2,xlabel="Frequency [MHz]",ylabel="RMS/Mean")
            self.plot_finish("Plotted relative RMS of downsampled spectrum (doplot>=2)",name=self.__taskname__+"-rms_div_mean")
        #Now select bins where the ratio between RMS and amplitude is within the limits
        self.nselected_bins=self.selected_bins[...,1:].findbetween(self.ratio[...,1:-1],self.limit1,self.limit2)
        self.selected_bins+=1; # We started the search only at bin #1 in self.ratio, so the indices returned are off by one
        #Make sure the endpoints are part of it
        self.nselected_bins+=2 # add the two endpoints
        self.selected_bins[...,0].fill(0)
        self.endpoints=hArray(int,[self.nofAntennas,1],fill=self.nselected_bins-1)
        self.selected_bins[...].set(self.endpoints,len(self.freqs)-1)
        #Now copy only those bins with average RMS, i.e. likely with little RFI and take the log
        self.clean_bins_x[...].copy(self.freqs,self.selected_bins[...],asvec(self.nselected_bins))
        self.clean_bins_y[...].copy(self.small_spectrum[...],self.selected_bins[...],asvec(self.nselected_bins))
        #    self.weights.copy(self.clean_bins_y)
        if self.logfit:
            self.clean_bins_y[...,[0]:self.nselected_bins].log()
        if self.verbose:
            print time.clock()-self.t0,"s: Fitting baseline."
        if self.fittype=="POLY":
            if self.verbose:
                print "Performing a polynomial fit with ",self.ncoeffs,"coefficients."
            #Create the nth powers of the x value, i.e. the frequency, for the fitting
            self.xpowers[...,[0]:self.nselected_bins].linearfitpolynomialx(self.clean_bins_x[...,[0]:self.nselected_bins],self.powers[...])
            #Fit an nth order polynomial to the data
            self.chisquare=self.coeffs[...].linearfit(self.covariance[...],self.xpowers[...],self.clean_bins_y[...],self.nselected_bins)  #self.weights[...],
        else:
            if self.verbose:
                print "Performing a basis spline fit with ",self.ncoeffs+self.splineorder+1-2,"break points."
            #Perform a Basis Spline fit to the data
            self.chisquare=self.coeffs[...].bsplinefit(self.covariance[...],self.xpowers[...,[0]:self.nselected_bins],self.clean_bins_x[...,[0]:self.nselected_bins],self.clean_bins_y[...,[0]:self.nselected_bins],self.numin_val_i,self.numax_val_i,self.splineorder+1)

        #Calculate an estimate of the average RMS of the clean spectrum after baseline division
        self.ratio[...].copy(self.ratio,self.selected_bins[...],self.nselected_bins)
        meanrms=self.ratio[...,[0]:self.nselected_bins].meaninverse()
        if self.verbose:
            print time.clock()-self.t0,"s: Done fitting baseline."
        if self.doplot:
            plt.clf()
            plt.subplots_adjust(hspace=0.4)
            plt.subplot(2,1,0)
            self.clean_bins_y[self.plot_antenna,0:self.nselected_bins[self.plot_antenna]].plot(xvalues=self.clean_bins_x[0,0:self.nselected_bins[self.plot_antenna]],logplot=False,color="blue",clf=False,title="Fitted baseline to downsampled spectrum (antenna #"+str(self.plot_antenna)+")")
            plt.subplot(2,1,1)
            self.clean_bins_y[...,[0]:self.nselected_bins].plot(xvalues=self.clean_bins_x[...,[0]:self.nselected_bins],logplot=False,clf=False,title="Fitted baseline to downsampled spectrum (all antennas)")
            self.clean_bins_y.fill(0.0)
            if self.fittype=="POLY":
                self.clean_bins_y[...,[0]:self.nselected_bins].polynomial(self.clean_bins_x[...,[0]:self.nselected_bins],self.coeffs[...],self.powers[...])
            else:
                self.clean_bins_y[...,[0]:self.nselected_bins].bsplinecalc(self.clean_bins_x[...,[0]:(self.nselected_bins)],self.coeffs[...],self.numin_val_i,self.numax_val_i,self.splineorder+1)
            plt.subplot(2,1,0)
            self.clean_bins_y[self.plot_antenna,[0]:self.nselected_bins-1].plot(xvalues=self.clean_bins_x[...,[0]:self.nselected_bins-1],clf=False,color="red",logplot=False)
            plt.subplot(2,1,1)
            self.clean_bins_y[...,[0]:self.nselected_bins-1].plot(xvalues=self.clean_bins_x[...,[0]:self.nselected_bins-1],clf=False,logplot=False)
            self.plot_finish("Plotted downsampled and cleaned spectrum with baseline fit.",name=self.__taskname__+"-clean_spectrum");
            if wasinteractive: plt.ion()
        self.spectrum.setHeader(FitBaseline=self.ws.getParameters())
        if self.save_output:
            self.spectrum.writeheader(self.filename)
            print "Written spectrum to file, to read it back: sp=hArrayRead('"+self.filename+"')"

CalcBaselineParameters=  dict([(p,FitBaseline.parameters[p]) for p in
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
     'plot_filename',
     'plot_center'
     ]])

CalcBaselineParameters.update({
    "baseline":{doc:"Array containing the calculated baseline with the same dimensions as ``spectrum`` - can be provided as spectrum.par.baseline (will be created if not).",
                #       default:lambda self:self.spectrum.par.baseline if hasattr(self.spectrum,"par") and hasattr(self.spectrum.par,"baseline") else hArray(float,dimensions=self.spectrum,name="Baseline"),
                default:None
                },

    "addHanning":{default: True,
                  doc:"Add a Hanning filter above ``nu_max`` and below ``nu_min`` to suppress out-of-band emission smoothly."},

    "logfit":{default:lambda self:True if self.FitParameters==None else self.FitParameters["logfit"]},

    "fittype":{default:lambda self:'BSPLINE' if self.FitParameters==None else self.FitParameters["fittype"]},

    "work_baseline":{doc:"Wrapper to baseline with dimension ``[nofAntennas,nchannels]`` even for a single spectrum.",
             default:lambda self:hArray(asvec(self.baseline),dimensions=[self.nofAntennas,self.nofChannels],properties=self.baseline,xvalues=self.frequency),export:False},

    "FitParameters":{doc:"Parameters of the baseline fitting routine.",
                     default: lambda self: self.spectrum.getHeader("FitBaseline") if self.spectrum.hasHeader("FitBaseline") else None},

    "numin":{doc:"""Minimum frequency of useable bandwidth. Negative if to be ignored.""",
             default:lambda self: -1 if self.FitParameters==None else self.FitParameters["numin"]},

    "numax":{doc:"""Maximum frequency of useable bandwidth. Negative if to be ignored.""",
             default:lambda self: -1 if self.FitParameters==None else self.FitParameters["numax"]},

    "numin_val_i":{doc:"""Minimum frequency of useable bandwidth corresponding to ``numin_i`` (output)""",
                   default:lambda self:self.frequency[self.numin_i] if self.FitParameters==None else self.FitParameters["numin_val_i"]},

    "numax_val_i":{doc:"""Maximum frequency of useable bandwidth corresponding to ``numax_i`` (output)""",
                   default:lambda self:self.frequency[self.numax_i] if self.FitParameters==None else self.FitParameters["numax_val_i"]},

    "numin_i":{doc:"""Channel number in spectrum of the minium frequency where to calculate baseline. Apply hanning taper below.""",
               default: lambda self:max(self.frequency.findlowerbound(self.numin).val(),0) if self.numin>0 else 0,
               output:True},

    "numax_i":{doc:"""Channel number in spectrum of the maxium frequency where to calculate baseline. Apply hanning taper above.""",
               default: lambda self:min(self.frequency.findlowerbound(self.numax).val(),self.nofChannels) if self.numax>0 else self.nofChannels,
               output:True},

    "height_ends":{doc:"""The heights of the baseline at the left and right endpoints of the usable bandwidth where a hanning function is smoothly added.""",
                   default:lambda self:hArray(float,[2,self.nofAntennas])},

    "coeffs":{doc:"Polynomial coefficients of the baseline fit with the dimension ``[nofAntennas, ncoeffs]`` or ``[ncoeffs]``",
              #input:True,
              default:lambda self:hArray(float,[1,1],name="Coefficients",fill=0) if self.FitParameters==None else self.FitParameters["coeffs"]},

    "dim_coeffs":{doc:"Dimension of the coefficients array (which should be ``[nofAntennas, ncoeff]`` or ``[ncoeff]`` for ``nofAntennas==1``)",
                  default:lambda self:self.coeffs.getDim()},

    "nofAntennasCoeffs":{doc:"Number of antennas in coeffcient array.",
                         default:lambda self:1 if len(self.dim_coeffs)==1 else self.dim_coeffs[0]},

    "ncoeffs":{doc:"Number of coefficients for the polynomial.",
               default:lambda self:self.dim_coeffs[1] if len(self.dim_coeffs)>=2 else self.dim_coeffs[0]},

    "splineorder":{doc:"Order of the polynomial to fit for the BSpline, ``splineorder=3`` is a bicubic spline.",
                   default: lambda self:3 if self.FitParameters==None else self.FitParameters["splineorder"]},

    "work_coeffs":{doc:"Array with coefficients in the form ``[nofAntennas, ncoeff]``",
           default:lambda self:hArray(asvec(self.coeffs),dimensions=[self.nofAntennasCoeffs,self.ncoeffs])},

    "invert":{doc:"Invert the baseline so that it can later simply be multiplied to take out the gain variations.",
              default:True},

    "HanningUp":{doc:"Let the Hanning filter at the ends go up rather than down (useful if one wants to divide by baseline and suppress out-of-band-noise)",
                 default:True},

    "normalize":{doc:"If **True**, normalize the baseline to have a total sum of unity.",
                 default:True}

    })


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
    """
    parameters=CalcBaselineParameters

    def call(self,spectrum):
        pass

    def run(self):
        if not type(self.spectrum) in hAllArrayTypes:
            print "ERROR: please provide an hArray as input for the positional argument 'spectrum'!"
            return

        self.t0=time.clock()
        if not hasattr(self.spectrum,"par"):
            setattr(self.spectrum,"par",hArray_par())

        if not type(self.baseline) in hAllArrayTypes:
            if hasattr(self.spectrum.par,"baseline"):
                self.baseline=self.spectrum.par.baseline
            else:
                self.spectrum.par.baseline=hArray(float,properties=self.spectrum,name="Baseline")
                self.baseline=self.spectrum.par.baseline
        else:
            self.spectrum.par.baseline=self.baseline

        if self.fittype=="POLY":
#            self.work_baseline[...,[self.numin_i]:[self.numax_i]].polynomial(self.work_frequency[...,[self.numin_i]:[self.numax_i]],self.work_coeffs[...],self.powers[...])
            self.work_baseline[...,[self.numin_i]:[self.numax_i]].polynomial(self.frequency[self.numin_i:self.numax_i],self.work_coeffs[...],self.powers[...])
        else:
#            self.work_baseline[...,[self.numin_i]:[self.numax_i]].bsplinecalc(self.frequency[...,[self.numin_i]:[self.numax_i]],self.work_coeffs[...],self.numin_val_i,self.numax_val_i,self.splineorder+1)
            self.work_baseline[...,[self.numin_i]:[self.numax_i]].bsplinecalc(self.frequency[self.numin_i:self.numax_i],self.work_coeffs[...],self.numin_val_i,self.numax_val_i,self.splineorder+1)

        #Now add nice ends (Hanning Filters) to the frequency range to suppress the noise outside the usuable bandwidth
        #Left end
        if self.addHanning:
#            import pdb; pdb.set_trace()
            self.height_ends[0,...].copy(self.work_baseline[...,self.numin_i])
            self.height_ends[1,...].copy(self.work_baseline[...,self.numax_i-1])
            self.factor=hArray(float,self.nofAntennas,fill=6.9) # Factor 1000 in log
            if not self.logfit:
                self.factor.fill(self.height_ends[0])
                self.factor *= 1000.0
            self.work_baseline[...,0:self.numin_i].gethanningfilterhalf(Vector(self.factor),asvec(self.height_ends[0])-(0 if self.HanningUp else asvec(self.factor)),Vector(bool,self.nofAntennas,fill=self.HanningUp))
            #Right end
            if not self.logfit:
                self.factor.fill(self.height_ends[1])
                self.factor *= 1000.0
            self.work_baseline[...,self.numax_i:].gethanningfilterhalf(Vector(self.factor),asvec(self.height_ends[1])-(0 if self.HanningUp else asvec(self.factor)),Vector(bool,self.nofAntennas,fill=not self.HanningUp))
        if self.logfit:
            self.work_baseline.min(40)# avoid numerical trouble
            self.work_baseline.max(-40)# avoid numerical trouble
            self.work_baseline.exp()
        if self.doplot:
            print "#Plotting the calculated baseline of the reference antenna."
            wasinteractive=plt.isinteractive()
            plt.ioff()
            plt.clf();
            if self.doplot>1:
                plt.subplots_adjust(hspace=0.4)
                plt.subplot(2,1,0)
            self.work_spectrum[self.plot_antenna,self.plot_start:self.plot_end].plot(color="blue",clf=False,title="Baseline (antenna #"+str(self.plot_antenna)+")")
            self.work_baseline[self.plot_antenna,self.plot_start:self.plot_end].plot(color="red",clf=False)
            if self.doplot>1:
                print "#Plotting the calculated baselines of all antennas."
                plt.subplot(2,1,1)
                self.work_baseline[...,self.plot_start:self.plot_end].plot(title="Baseline (all antennas)",clf=False)
            self.plot_finish(name=self.__taskname__)
            if wasinteractive: plt.ion()
        if self.invert:
            self.work_baseline.inverse() # -> 1/baseline
        if self.normalize:
            self.work_baseline[...] /= self.work_baseline[...,self.numin_i:self.numax_i].mean()
        if self.save_output:
            self.spectrum.writeheader(self.filename)
            print "Written spectrum to file, to read it back: sp=hArrayRead('"+self.filename+"')"
        if self.verbose:
            print time.clock()-self.t0,"s: Done CalcBaseline."


ApplyBaselineParameters = dict(
    [(p,CalcBaseline.parameters[p]) for p in
     [
     'FitParameters',
     'spectrum',
     'frequency',
#     'work_frequency',
     'work_spectrum',
     'baseline',
     'work_baseline',
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
     'plot_finish',
     'plot_center'
         ]
     ]
    )

ApplyBaselineParameters.update({

    "filename":{default:lambda self: "tmpspec.clean.pcr" if not self.spectrum.hasHeader("filename") else root_filename(self.spectrum.getHeader("filename"))+".clean",
                doc: "Filename to write output to."},

    # "FitParameters":{doc:"Parameters of the baseline fitting routine.",
    #                  default: lambda self: self.spectrum.getHeader("FitBaseline") if self.spectrum.hasHeader("FitBaseline") else None},

    # "numin":{doc:"""Minimum frequency of useable bandwidth. Negative if to be ignored.""",
    #         default:lambda self: -1 if self.FitParameters==None else self.FitParameters["numin"]},

    # "numax":{doc:"""Maximum frequency of useable bandwidth. Negative if to be ignored.""",
    #          default:lambda self: -1 if self.FitParameters==None else self.FitParameters["numax"]},

    "plotchannel":{doc:"Which channel to plot.",
                   default:0},

    "adaptive_peak_threshold":{doc:"If **True** then calculate the threshold above which to cut peaks in ``nbins`` separate bins and thus let it vary over the spectrum",
                               default:False},

    "apply_baseline":{doc:"If **True** then divide spectrum by baseline before removing peaks.",
                      default: True},

    "mean":{doc:"Median mean value of blocks in downsampled spectrum - used to replace flagged data with (output only)",
            default:1},

    "rms":{doc:"Median RMS value of blocks in downsampled spectrum - used to calculate threshold for cutting peaks (output only)",
           default:lambda self:Vector(float,[self.nofAntennas],fill=0.0),
           output:True},

    "means":{default:lambda self:hArray(float,[self.nofAntennas,self.nbins]),
             doc:"Mean value per block in input spectrum"},

    "stddevs":{default:lambda self:hArray(float,[self.nofAntennas,self.nbins]),
               doc:"Standard deviation per block in input spectrum"},

    "nofChannelsUsed":{doc:"""Number of channels remaining after downsampling and ignoring edges.""",
                       default:lambda self:self.numax_i-self.numin_i},

    "rmsfactor":{doc:"""Factor above the RMS in each channel at which a channel is considered poluted by RFI.""",
                 default:5.0},

    "nbins":{doc:"""The number of bins to be used to subdivide spectrum for selecting the best RMS.""",
             default: lambda self:max(self.nofChannelsUsed/256,min(256,self.nofChannelsUsed/8))},

    "blocklen":{doc:"""The block length of one bin used to subdivide spectrum for selecting the best RMS.""",
                default: lambda self:self.nofChannelsUsed/self.nbins},

    "ndirty_channels":{doc:"""Number of dirty channels to be removed as RFI. (output only)""",
                       default:0,
                       output:True},

    "dirty_channels":{doc:"""Array of indices pointing to dirty bins, i.e. with high RFI. (work vector)""",
                      default:lambda self:hArray(int,dimensions=self.work_spectrum,name="Dirty bins"),
                      workarray:True}
    })


class ApplyBaseline(tasks.Task):
    """
    Usage::

      >>> ApplyBaseline(spectrum)

    contains ``spectrum.par.baseline``

    Also does RFI excision.
    """
    parameters=ApplyBaselineParameters

    def call(self,spectrum):
        pass

    def run(self):
        self.t0=time.clock()

        if not type(self.spectrum) in hAllArrayTypes:
            print "ERROR: please provide an hArray as input for the positional argument 'spectrum'!"
            return

        if not hasattr(self.work_spectrum,"par"):
            setattr(self.spectrum,"par",hArray_par())

        if hasattr(self.spectrum.par,"baseline") and self.baseline==None:
            self.baseline=self.spectrum.par.baseline

        if self.apply_baseline:
            self.work_spectrum *= self.work_baseline

        # Some ugly way to fool the python bindings, since right now they down return data in the argument variables
        #self.minblk=self.work_spectrum[...,self.numin_i:self.numax_i].minstddevblock(Vector(int,[self.nofAntennas],fill=self.blocklen),self.minrms,self.minmean)


        if self.adaptive_peak_threshold:
            self.means[...].downsamplespikydata(self.stddevs[...],self.work_spectrum[...],1.0);
            self.mean=self.work_spectrum[...,self.numin_i:self.numax_i].mean().val()
            self.limit=hArray(properties=self.work_baseline,name="Threshold",fill=0.0)
            self.limit[...].upsample(self.stddevs[...])
            self.limit *= self.rmsfactor
            self.limit[...].upsample(self.means[...])
        else:
            self.means[...].downsamplespikydata(self.stddevs[...],self.work_spectrum[...,self.numin_i:self.numax_i],1.0);
            self.rms=self.stddevs[...].sortmedian()
            self.mean=self.means[...].sortmedian()
            self.limit=self.mean+self.rms*self.rmsfactor
        if self.doplot:
            wasinteractive=plt.isinteractive()
            plt.ioff()
            self.work_spectrum[self.plotchannel,self.plot_start:self.plot_end].plot(title="Spectrum with baseline applied",color='red')
            if self.adaptive_peak_threshold:
                self.limit[self.plotchannel,self.plot_start:self.plot_end].plot(clf=False,color="green",logplot="y")
            else:
                plotconst(self.frequency[self.plot_start:self.plot_end],self.limit[self.plotchannel]).plot(clf=False,color="green",logplot="y")
        #Now select bins where the ratio between RMS and amplitude is within the limits
        if self.adaptive_peak_threshold:
            self.ndirty_channels=self.dirty_channels[...].findgreaterthanvec(self.work_spectrum[...],self.limit[...])
        else:
            self.ndirty_channels=self.dirty_channels[...].findgreaterthan(self.work_spectrum[...],self.limit)
        #Now copy only those bins with average RMS, i.e. likely with little RFI and take the log
        self.work_spectrum[...,self.dirty_channels[...,[0]:asvec(self.ndirty_channels)]]=self.mean
        if self.save_output:
            self.spectrum.setHeader(filename=self.filename)
            self.spectrum.write(self.filename)
            print "Written spectrum to file, to read it back: sp=hArrayRead('"+self.filename+"')"
        if self.verbose:
            print time.clock()-self.t0,"s: Done ApplyBaseline."
        if self.doplot:
            self.work_spectrum[self.plotchannel,self.plot_start:self.plot_end].plot(color='blue',clf=False)
            self.plot_finish(name=self.__taskname__)
            if wasinteractive: plt.ion()

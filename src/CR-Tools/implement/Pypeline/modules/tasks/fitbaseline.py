"""Spectrum documentation.
"""

from pycrtools import *
from pycrtools.tasks.shortcuts import *
import pycrtools.tasks as tasks
import time
#from pycrtools import IO

"""
*default*  contains a default value or a function that will be assigned
when the parameter is accessed the first time and no value has been
explicitly set. The function has the form "lambda ws: functionbody", where ws is the worspace itself, so that one can access other
parameters. E.g.: default:lambda ws: ws.par1+1 so that the default
value is one larger than he value in par1 in the workspace.

*doc* a documentation string describing the parameter

*unit* the unit of the value (for informational purposes only)

*export* (True) if False do not export the parameter with
 ws.parameters() or print the parameter

*workarray* (False) If True then this is a workarray which contains
 large amount of memory and is listed separately and not written to
 file.
"""

def fitbaseline_calc_freqs(self):
    val=hArray(float,dimensions=[1,self.nbins],name="Frequency",units=("M","Hz"))
    val.downsample(self.frequency[self.numin_i:self.numax_i])
    return val

def fitbaseline_calc_numin_i(self):
    if self.numin>0: numin_i=self.frequency.findlowerbound(self.numin).val()
    else: numin_i=1
    numin_i=max(numin_i-int(numin_i*self.extendfit),0)
    return numin_i

def fitbaseline_calc_numax_i(self):
    if self.numax>0: numax_i=self.frequency.findlowerbound(self.numax).val()
    else: numax_i=len(self.frequency)
    numax_i=min(numax_i+int((len(self.frequency)-numax_i)*self.extendfit),len(self.frequency))
    return numax_i

class FitBaseline(tasks.Task):
    """
    Usage:
    
    FitBaseline(input_spectrum, **keywords)

    Function to fit a baseline using a polynomial function
    (fittype='POLY') or a basis spine fit (fittype='BSPLINE', default)
    to a spectrum while trying to ignore positive spikes in the fit
    (e.g., those coming from RFI = Radio Frequency Interference).

    The input_spectrum is a float array with input spectrum either of
    dimension [nofAntennas,nofChannels] or just [nofChannels] for a
    single spectrum. Note that the frequency values for the array are
    expected to be provided as
    input_spectrum.par.xvalues=hArray(float,[nofChannels],fill=...) -
    the same you would use to provide xvalues for plotting. You can
    also provide the frequencies explicitly in the keyword
    'frequency', otherwise a simple numbering is assumed
    frequency=[0,1,2,3...].


    Returns a tuple with the first value being the array of baseline
    fit coefficients (also accessible through 'task.coeff') and the second value
    being a vector of the mean RMS of the spectra.
    
    Use baseline.polynomial(task.frequency,task.coeffs,task.powers) to caluclate the
    baseline from the coefficients for the polynomial fit, or

    task.xpowers[...,[0]:task.nselected_bins].linearfitpolynomialx(task.clean_bins_x[...,[0]:task.nselected_bins],task.powers[...])
    task.clean_bins_y[...,[0]:task.nselected_bins].bspline(task.xpowers[...,[0]:(task.nselected_bins)],task.coeffs[...])


    
    Parameters can be provided as additional keywords

    e.g., baseline.polynomial(frequency,coeffs,powers, parameter1=value1, parameter2=value2)

    or in a WorkSpace.

    e.g., baseline.polynomial(frequency,coeffs,powers, ws=WorkSpace),

    or in a mix of the two. Otherwise default values are used.

    """
    parameters = {
	"filefilter":{default:"",
		      doc:"Unix style filter (i.e., with *,~, $VARIABLE, etc.), to describe all the files to be processed."},
	"filenames":{default:lambda self:listfiles(self.filefilter),
		     doc:"List of filenames of data file to read raw data from."},
	"output_files":{default:lambda self:["fitbaseline-"+name for name in self.filenames],
			 doc:"Filenames to store the original spectrum with results of the fit."},
	"nbins":{doc:"""The number of bins in the downsampled spectrum used to fit the baseline.""",
		 default: lambda self:max(self.nofChannels/256,min(256,self.nofChannels/8))},
	"polyorder":{doc:"""Order of the plyonomial to fit.  (output only)""",
		     default:lambda self:self.ncoeffs-1,output:True},
	"rmsfactor":{doc:"""Factor above and below the RMS in each bin at which a bin is no longer considered due to too many spikes.""",
		     default:2.0},
	"logfit":{doc:"""Actually fit the polynomial to the log of the (downsampled) data. (Hence you need to .exp the baseline afterwards).""",
		  default:True},
	"fittype":{doc:"""Determine which type of fit to do: fittype="POLY" - do a polynomial fit, else ("BSPLINE") do a basis spline fit (default).""",
		   default:"BSPLINE"},
	"nofAntennas":{doc:"""Number of antennas in input spectrum.""",
		       default:lambda self:1 if len(self.dim_spectrum)==1 else self.dim_spectrum[0]},
	"nofChannels":{doc:"""Number of channels in input spectrum.""",
		       default:lambda self:self.dim_spectrum[0] if len(self.dim_spectrum)==1 else self.dim_spectrum[1]},
	"nofChannelsUsed":{doc:"""Number of channels remaining after downsampling and ignoring edges.""",
		       default:lambda self:self.numax_i-self.numin_i},
	"dim_spectrum":{doc:"""Dimension of input spectrum (typically n antennas times m spectral points) or just one dimensional for one antenna.""",
		       default:lambda self:self.spectrum.getDim()},
	"ncoeffs":{doc:"""Number of coefficients for the polynomial.""",
		   default:18},
	"numin":{doc:"""Minimum frequency of useable bandwidth. Negative if to be ignored.""",
		 default:-1},
	"numax":{doc:"""Maximum frequency of useable bandwidth. Negative if to be ignored.""",
		 default:-1},
	"numin_i":{doc:"""Channel number in spectrum of the minimum frequency of the useable bandwidth. Negative if to be ignored.""",
		   default:fitbaseline_calc_numin_i,output:True},
	"numax_i":{doc:"""Channel number in spectrum of the maximum frequency of the useable bandwidth. Negative if to be ignored.""",
		   default:fitbaseline_calc_numax_i,output:True},
	"extendfit":{doc:"""Extend the fit by this factor at both ends beyond numax and numin. The factor is relative to the unused bandwidth.""",
		     default:0.1},
	"freqs":{doc:"""Array of frequency values of the downsampled spectrum. (work vector)""",
		 default:fitbaseline_calc_freqs,workarray:False},
	"small_spectrum":{doc:"""Array of power values holding the downsampled spectrum. (work vector)""",
		    default:lambda self:hArray(float,[self.nofAntennas,self.nbins],name="Binned Spectrum",units="a.u.",xvalues=self.freqs,par=("logplot","y")),workarray:True},
	"rms":{doc:"""Array of RMS values of the downsampled spectrum. (work vector)""",
	       default:lambda self:hArray(properties=self.small_spectrum, name="RMS of Spectrum"),workarray:True},
	"minmean":{doc:"Mean value of data in the part of downsampled spectrum with the smallest RMS (output only)",
		   default:lambda self:Vector(float,[self.nofAntennas],fill=0.0),output:True},
	"minrms":{doc:"RMS value of data in the part of downsampled spectrum with the smallest RMS (output only)",
		   default:lambda self:Vector(float,[self.nofAntennas],fill=0.0),output:True},
	"minrms_blen":{doc:"Block length within downsampled data to look for the cleanest part of the spectrum.",
		   default:lambda self:min(64,max(self.nbins/16,4))},
	"chisquare":{doc:"""Returns the chisquare of the baseline fit. (output only)""",
		     default:0,output:True},
	"weights":{doc:"""Array of weight values for the fit. (work vector)""",
		   default:lambda self:hArray(properties=self.small_spectrum, name="Fit Weights"),workarray:True},
	"ratio":{doc:"""Array holding the ratio between RMS and power of the downsampled spectrum. (work vector)""",
		 default:lambda self:hArray(properties=self.small_spectrum,name="RMS/Amplitude",par=("logplot",False)),workarray:True},
	"covariance":{doc:"""Array containign the covariance matrix of the fit. (output only)""",
		      default:lambda self:hArray(float,[self.nofAntennas,self.ncoeffs,self.ncoeffs]),output:True},
	"bwipointer":{doc:"""Pointer to the internal BSpline workspace as integer. Don't change! """,
		      default:0,export:False,output:True},
	"clean_bins_x":{doc:"""Array holding the frequencies of the clean bins. (work vector)""",
			default:lambda self:hArray(dimensions=[self.nofAntennas,self.nbins],properties=self.freqs,name="Clean Frequencies"),workarray:True},
	"clean_bins_y":{doc:"""Array holding the powers of the clean bins. (work vector)""",
			default:lambda self:self.small_spectrum,workarray:True},
	"xpowers":{doc:"Array holding the x-values and their powers for calculating the baseline fit.",
		   default:lambda self:hArray(float,[self["nofAntennas"],self["nbins"],self["ncoeffs"]],name="Powers of Frequency"),
		   workarray:True},
	"powers":{doc:"Array of integers, containing the powers to fit in the polynomial. (work vector)",
		  default:lambda self:hArray(int,[self.nofAntennas,self.ncoeffs],range(self.ncoeffs)),workarray:True},
	"height_ends":{doc:"""The heights of the baseline at theleft and right endpoints of the usable bandwidth where a hanning function is smoothly added.""",
		       default:lambda self:hArray(float,[2,self.nofAntennas])},
	"nselected_bins":{doc:"""Number of clean bins after RFI removal. (output only)""",
			  default:0,output:True},
	"selected_bins":{doc:"""Array of indices pointing to clean bins, i.e. with low RFI. (work vector)""",
			 default:lambda self:hArray(int,self.small_spectrum,name="Selected bins"),workarray:True},
	"coeffs":{doc:"""Polynomial coefficients of the baseline fit with the dimension [nofAntennas,ncoeffs] (output vector)""",
		  default:lambda self:hArray(float,[self.nofAntennas,self.ncoeffs]),output:True},
	"frequency":{doc:"Frequency values in Hz for each spectral channel (dimension: [nchannels])",unit:"Hz",workarray:True,
		     default:lambda self:hArray(float,[self.nofChannels],name="Frequency").fillrange(0.,1.) if not hasattr(self.spectrum.par,"xvalues") else self.spectrum.par.xvalues},
	"spectrum":{doc:"Array with input spectrum either of dimension [nofAntennas,nchannels] or just [nchannels] for a single spectrum. Note that the frequency values for the array are expected to be provided as spectrum.par.xvalues=hArray(float,[nofChannels],fill=...) otherwise provide the frequencies explicitly in 'frequency'"},
	"work_spectrum":{doc:"Wrapper to input spectrum with dimension [nofAntennas,nchannels] even for a single spectrum.",
			 default:lambda self:hArray(self.spectrum,dimensions=[self.nofAntennas,self.nofChannels]),export:False},
	"meanrms":{doc:"""Estimate the mean rms in the spectrum per antenna. (output vector)""",
		   default:0,output:True},
	"verbose":{doc:"""Print progress information""",default:True},
	"doplot":{doc:"""Plot progress information. If value >1, plot more information.""",default:True}
	}

    def call(self,spectrum):
	pass
    
    def run(self):
        """Run the program.
        """
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
	    print "Plotting full spectrum and downsampled spectrum (doplot>=3) - can take some time..."
	    self.work_spectrum[0].plot(title="RFI Downsampling")
	    self.small_spectrum[...].plot(xvalues=self.freqs,clf=False)
	    raw_input("... press Enter to continue.")
    #Normalize the spectrum to unity
	self.meanspec=self.small_spectrum[...].mean()
	self.small_spectrum[...] /= self.meanspec
    #Calculate RMS/amplitude for each bin
	self.ratio[...].div(self.rms[...],self.small_spectrum[...])
    #Get the RMS of the part of the spectrum where it is lowest (i.e. which is least affected by RFI)
	self.minblk=self.ratio[...].minstddevblock(Vector(int,[self.nbins],fill=self.minrms_blen),self.minrms,self.minmean)	
    #Set limits for which spikes to ignore
	self.limit2=self.minmean+self.minrms*self.rmsfactor
	self.limit1=self.minmean-self.minrms*self.rmsfactor
	if self.doplot>1:
	    self.ratio[...].plot(xvalues=self.freqs,title="RMS/Amplitude",clf=True)
	    plotconst(self.freqs,(self.limit1).val()).plot(clf=False,color="green")
	    plotconst(self.freqs,(self.limit2).val()).plot(clf=False,color="green")
	    raw_input("Plotted relative RMS of downsampled spectrum (doplot>=2) - press Enter to continue...")
    #Now select bins where the ratio between RMS and amplitude is within the limits
	self.nselected_bins=self.selected_bins[...].findbetween(self.ratio[...],self.limit1,self.limit2)
    #Now copy only those bins with average RMS, i.e. likely with little RFI and take the log
	self.clean_bins_x[...].copy(self.freqs,self.selected_bins[...],self.nselected_bins)
	self.clean_bins_y[...].copy(self.small_spectrum[...],self.selected_bins[...],self.nselected_bins)
    #    self.weights.copy(self.clean_bins_y)
	if self.logfit:
	    self.clean_bins_y[...,[0]:self.nselected_bins].log()
	if self.verbose: print time.clock()-self.t0,"s: Fitting baseline."
	if self.fittype=="POLY":
	    if self.verbose:
		print "Performing a polynomial fit with ",self.ncoeffs,"coefficients."
	    #Create the nth powers of the x value, i.e. the frequency, for the fitting
	    self.xpowers[...,[0]:self.nselected_bins].linearfitpolynomialx(self.clean_bins_x[...,[0]:self.nselected_bins],self.powers[...])
	    #Fit an nth order polynomial to the data
	    self.chisquare=self.coeffs[...].linearfit(self.covariance[...],self.xpowers[...],self.clean_bins_y[...],self.nselected_bins)  #self.weights[...],
	else:
	    if self.verbose:
		print "Performing a basis spline fit with ",self.ncoeffs-2,"break points."
	    #Perform a Basis Spline fit to the data
	    self.chisquare=self.coeffs[...].bsplinefit(self.covariance[...],self.xpowers[...,[0]:self.nselected_bins],self.clean_bins_x[...,[0]:self.nselected_bins],self.clean_bins_y[...,[0]:self.nselected_bins])
		  
	#Calculate an estimate of the average RMS of the clean spectrum after baseline division
	self.ratio[...].copy(self.ratio,self.selected_bins[...],self.nselected_bins)
	meanrms=self.ratio[...,[0]:self.nselected_bins].meaninverse()
	if self.verbose: print time.clock()-self.t0,"s: Done fitting baseline."
	if self.doplot:
	    self.clean_bins_y[...,[0]:self.nselected_bins].plot(xvalues=self.clean_bins_x[...,[0]:self.nselected_bins],logplot=False)
	    self.clean_bins_y.fill(0.0)
	    if self.fittype=="POLY":
		self.clean_bins_y[...,[0]:self.nselected_bins].polynomial(self.clean_bins_x[...,[0]:self.nselected_bins],self.coeffs[...],self.powers[...])
	    else:
		self.clean_bins_y[...,[0]:self.nselected_bins].bspline(self.xpowers[...,[0]:(self.nselected_bins)],self.coeffs[...])
#		self.clean_bins_y[...,[0]:self.nselected_bins].bsplinecalc(self.clean_bins_x[...,[0]:(self.nselected_bins)],self.coeffs[...])
	    self.clean_bins_y[...,[0]:self.nselected_bins-1].plot(xvalues=self.clean_bins_x[...,[0]:self.nselected_bins-1],clf=False,logplot=False)
	    print "Plotted downsampled and cleaned spectrum with baseline fit."
	self.spectrum.setHeader(FitBaseline=self.ws.getParameters())
	if len(self.output_files)>0:
	    self.power.write(self.output_files[0])
	    print "Written spectrum to file, to read it back: sp=hArrayRead('"+self.output_files[0]+"')"
	return (self.coeffs,meanrms)

CalcBaselineParameters=    parameters = dict([(p,FitBaseline.parameters[p]) for p in 
	['selected_bins',
	'logfit',
	'numax',
	'spectrum',
	'numax_i',
	'verbose',
	'nofChannels',
	'doplot',
	'spectrum',
	'dim_spectrum',
	'nofAntennas',
	'numin',
	'height_ends',
	'polyorder',
	'extendfit',
	'frequency',
	'clean_bins_x',
	'fittype',
	'powers',
	'work_spectrum',
	'numin_i'
	 ]])

CalcBaselineParameters.update({
    "FitParameters":{doc:"Parameters of the baseline fitting routine.",
		     default: lambda self: self.spectrum.getHeader("FitBaseline")},
    "coeffs":{doc:"Polynomial coefficients of the baseline fit with the dimension [nofAntennas,ncoeffs] or [ncoeffs]",#input:True,
	      default:lambda self:hArray(float,[1,1],name="Coefficients",fill=0) if self.FitParameters==None else self.FitParameters["coeffs"]},
    "dim_coeffs":{doc:"Dimension of the coefficients array (which should be [nofAntennas,ncoeff] or [ncoeff] for nofAntennas==1)",
		  default:lambda self:self.coeffs.getDim()},
    "nofAntennasCoeffs":{doc:"Number of antennas in coeffcient array.",
			 default:lambda self:1 if len(self.dim_coeffs)==1 else self.dim_coeffs[0]},
    "ncoeffs":{doc:"Number of coefficients for the polynomial.",
	       default:lambda self:self.dim_coeffs[1] if len(self.dim_coeffs)>=2 else self.dim_coeffs[0]},
    "work_coeffs":{doc:"Array with coefficients in the form [nofAntennas,ncoeff]",
		   default:lambda self:hArray(self.coeffs,dimensions=[self.nofAntennasCoeffs,self.ncoeffs])},
    "powers":{doc:"Array of integers, containing the powers to fit in the polynomial. (work vector)",
	      default:lambda self:hArray(int,[self.nofAntennasCoeffs,self.ncoeffs],range(self.ncoeffs)),workarray:True}
    })


class CalcBaseline(tasks.Task):
    """
    CalcBaseline(spectrum):

    Calculate a smooth baseline from a set of coefficients that
    determine the baseline (e.g. as calculated by FitBaseline) and
    an array of frequencies (which need not be equi-spaced).
    test = range(
    """
    parameters=CalcBaselineParameters
    
    def call(self,spectrum):
	pass
    
    def run(self):
	self.t0=time.clock() 
	if self.fittype=="POLY":
	    self.work_spectrum.fill(0.0)
	    self.work_spectrum[...,self.numin_i:self.numax_i].polynomial(self.frequency[self.numin_i:self.numax_i],self.work_coeffs[...],self.powers[...])
	else:
	    self.work_spectrum[...,self.numin_i:self.numax_i].bsplinecalc(self.frequency[self.numin_i:self.numax_i],self.work_coeffs[...])
	#Now add nice ends (Hanning Filters) to the frequency range to suppress the noise outside the usuable bandwidth
	#Left end
	self.height_ends[0,...].copy(self.work_spectrum[...,self.numin_i])
	self.factor=hArray(float,self.nofAntennas,fill=6.9) # Factor 1000 in log
	if not self.logfit:
	    self.factor.copy(self.height_ends[0])
	    self.factor *= 1000.0
	self.work_spectrum[...,0:self.numin_i].gethanningfilterhalf(Vector(self.factor),Vector([self.height_ends[0]]),Vector(bool,self.nofAntennas,fill=True))
	#Right end
	self.height_ends[1,...].copy(self.work_spectrum[...,self.numax_i-2])
	if not self.logfit:
	    self.factor.copy(self.height_ends[1])
	    self.factor *= 1000.0
	self.work_spectrum[...,self.numax_i-1:].gethanningfilterhalf(Vector(self.factor),Vector([self.height_ends[1]]),Vector(bool,self.nofAntennas,fill=False))
	if self.logfit: self.work_spectrum.exp()
	if self.verbose:
	    print time.clock()-self.t0,"s: Done CalcBaseline."
	if self.doplot:
	    self.work_spectrum[...].plot(title="Baseline")
	    raw_input("Plotted baseline - press Enter to continue...")


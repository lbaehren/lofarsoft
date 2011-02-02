"""Provides a flexible data container class.
"""

#import pdb
#pdb.set_trace()

import time

from pycrtools.core import *

CRWorkSpaceList = []

class CRWorkSpace():
    """
    This class holds the arrays and vectors used by the various
    analysis tasks. Hence this is the basic workspace in the memory.
    """
    def __init__(self,parent=None,modulename=None,**keywords):
        self.modules=[]
        if modulename==None:
            self.modulename=self.name
        else:
            self.modulename=modulename
#Set the parent workspace if it exists and add itself to the parent's "modules" list.
        if not parent==None:
            self.parent=parent
            self.parent.modules.append(self.modulename)
            setattr(parent,self.modulename,self)
        l=len("default")+1
#Create a list of all the parameters with default values (and initialization functions)
        if not hasattr(self,"parameters"): self.parameters=[]
        self.parameters+=list(set(map(lambda s:s[l:],filter(lambda s:s.find("default_")==0,dir(self)))).difference(set(self.parameters)))
        for par in self.parameters:
            if hasattr(self,"default_"+par):
                if type(getattr(self,"default_"+par).__doc__)==str: #assign docstring, reusing the one from the initialization function
                    setattr(self,"__"+par+"__doc__",getattr(self,"default_"+par).__doc__)
        self.auxparameters=[]
        if not hasattr(self,"locals"): self.locals=set()
        self.setParameters(**keywords)
    def __setitem__(self,par,val):
        """Sets the value of a parameter and make it local."""
        if par not in self.parameters+self.auxparameters: self.auxparameters.append(par)
        if not self.isLocal(par): self.locals.add(par)
        setattr(self,par,val)
    def __getitem__(self,par):
        """
        Retrieve a parameter value from the workspace. If it does not
        exist locally, search the parent modules. If it is not
        available there execute the default function and assign the
        value locally. If no default value function exists, return
        None.
        """
        if self.isLocal(par):
            if hasattr(self,par): # return local copy
                return getattr(self,par)
            else:
                return self.setParameterDefault(par) # no local copy exists, return default paramter and make local
        elif hasattr(self,"parent"): #Is global, hence recursively retrieve it from the higher level (parent)
            val=self.parent[par]
            if not val==None:
                setattr(self,par,val) # Make a local copy, but don't mark as local
                return val
        #else: Does not exist localy or globaly, so create local default
        return self.setParameterDefault(par)
    def isLocal(self,par):
        return (par in self.locals)
    def markLocal(self,par):
        if not self.isLocal(par): self.locals.add(par) # Make parameter local
    def markGlobal(self,par):
        if self.isLocal():
            self.locals.discard(par)
            self["par"]
    def setParameterDefault(self,par):
        """Assign a parameter its default value and make it local."""
        if hasattr(self,"default_"+par):
            val=getattr(self,"default_"+par)()
            setattr(self,par,val)
            self.markLocal(par) # Make parameter local
        else: #no Defaults defined!
            val=None
        return val
    def setParameters(self,**keywords):
        """
        This method will set the parameters listed as arguments in the
        function call, i.e. modify the workspace attributes accordingly.
        """
        for k in keywords.keys(): self[k]=keywords[k]
        return self
    def __repr__(self,parentname=""):
        """
        Returns a readable summary of all parameters in the workspace.
        """
        s="#WorkSpace("+self.name+"):\n"
        myname=parentname+self.modulename
        for par in self.parameters+self.auxparameters:
            s+=myname+'["'+par+'"] = '+str(self[par])
            if self.isLocal(par): s+=" # (local)\n"
            else: s+=" # (parent)\n"
        for m in self.modules:
            s+=getattr(self,m).__repr__(parentname=myname+".")+"\n"
        return s
    def doc_parameters(self):
        s=""
        for par in self.parameters:
            val=self[par]
            s+="    "+par
            if ishArray(val): s+=" = "+val.__repr__(long=False)
            elif not val==None: s+=" = "+str(val)
            if hasattr(self,"__"+par+"__doc__"):
                s+=" - "+getattr(self,"__"+par+"__doc__")
            s+="\n"
        return s
    def help(self):
        print self.__doc__
        print "\nAvailable parameters:"
        print self.doc_parameters()
    def getModule(self,name):
        if self.name==name: return self
        for m in self.modules:
            val=getattr(self,m).getModule(name)
            if not val==None: return val
        return None

class CRAverageSpectrumWorkSpace(CRWorkSpace):
    """Workspace for hCRAverageSpectrum. See also CRMainWorkSpace and CRWorkSpace."""
    def __init__(self,parent=None,modulename=None,**keywords):
        self.parameters=["datafile","max_nblocks","nblocks"] # Create those parameters first and in this order
        CRWorkSpace.__init__(self,parent=parent,modulename=modulename,**keywords)
#    def default_datafile(self):
#        """DataReader object to read the data from."""
#        return crfile(DEFAULTDATAFILE)
    def default_nblocks(self):
        """Number of blocks to average, take all blocks by default."""
        return min(self["datafile"]["filesize"]/self["datafile"]["blocksize"],self["max_nblocks"])
    def default_max_nblocks(self):
        """Absolute maximum number of blocks to average, irrespective of filesize."""
        return 100000
    def default_blocks(self):
        """List of blocks to process."""
        return range(self["nblocks"])
    def default_fx(self):
        """Array to hold the x-values of the raw time series data. (work vector)"""
        return self["datafile"]["emptyFx"]
    def default_fft(self):
        """Array to hold the FFTed x-values (i.e. complex spectrum) of the raw time series data. (work vector)"""
        return self["datafile"]["emptyFFT"]

#Now add the new workspsace to the overall workspace list
CRWorkSpaceList.append(CRAverageSpectrumWorkSpace)
#And tell it its name ...
CRWorkSpaceList[-1].name="AverageSpectrum"

class CRFitBaselineWorkSpace(CRWorkSpace):
    """Workspace for hCRFitBaseline. See also CRMainWorkSpace and CRWorkSpace."""
    def __init__(self,parent=None,modulename=None,**keywords):
        self.parameters=["nbins","ncoeffs","polyorder","nofAntennas","freqs","spectrum"]
        self.locals=set(["nbins","polyorder","freqs","spectrum","rms","ratio","selected_bins"])
        CRWorkSpace.__init__(self,parent=parent,modulename=modulename,**keywords)
    def default_nbins(self):
        """The number of bins in the downsampled spectrum used to fit the baseline."""
        return 2**8
    def default_polyorder(self):
        """Order of the plyonomial to fit.  (output only)"""
        return self["ncoeffs"]-1
    def default_rmsfactor(self):
        """Factor above and below the RMS in each bin at which a bin is no longer considered."""
        return 2.0
    def default_fftLength(self):
        """Length of unbinned spectrum."""
        return 0
    def default_logfit(self):
        """Actually fit the polynomial to the log of the (downsampled) data. (Hence you need to .exp the baseline afterwards)."""
        return True
    def default_fittype(self):
        """Determine which type of fit to do: fittype="POLY" - do a polynomial fit, else ("BSPLINE") do a basis spline fit (default)."""
        return "BSPLINE"
    def default_nofAntennas(self):
        """Number of antennas held in memory."""
        return 1
    def default_ncoeffs(self):
        """Number of coefficients for the polynomial."""
        return 18
    def default_numin(self):
        """Minimum frequency of useable bandwidth. Negative if to be ignored."""
        return -1
    def default_numax(self):
        """Maximum frequency of useable bandwidth. Negative if to be ignored."""
        return -1
    def default_numin_i(self):
        """Channel number in spectrum of the minimum frequency of the useable bandwidth. Negative if to be ignored."""
        return -1
    def default_numax_i(self):
        """Channel number in spectrum of the maximum frequency of the useable bandwidth. Negative if to be ignored."""
        return -1
    def default_extendfit(self):
        """Extend the fit by this factor at both ends beyond numax and numin. The factor is relative to the unsued bandwidth."""
        return 0.1
    def default_freqs(self):
        """Array of frequency values of the downsampled spectrum. (work vector)"""
        return hArray(float,dimensions=[1,self["nbins"]],name="Frequency",units=("M","Hz"))
    def default_spectrum(self):
        """Array of power values holding the downsampled spectrum. (work vector)"""
        return hArray(float,[self["nofAntennas"],self["nbins"]],name="Binned Spectrum",units="a.u.",xvalues=self["freqs"],par=("logplot","y"))
    def default_rms(self):
        """Array of RMS values of the downsampled spectrum. (work vector)"""
        return hArray(properties=self["spectrum"], name="RMS of Spectrum")
    def default_chisquare(self):
        """Returns the chisquare of the baseline fit. (output only)"""
        return 0
    def default_weights(self):
        """Array of weight values for the fit. (work vector)"""
        return hArray(properties=self["spectrum"], name="Fit Weights")
    def default_ratio(self):
        """Array holding the ratio between RMS and power of the downsampled spectrum. (work vector)"""
        return hArray(properties=self["spectrum"],name="RMS/Amplitude",par=("logplot",False))
    def default_powers(self):
        """Array of integers, containing the powers to fit in the polynomial. (work vector)"""
        return hArray(int,[self["nofAntennas"],self["ncoeffs"]],range(self["ncoeffs"]))
    def default_xpowers(self):
        """Array holding the x-values and their powers for the fit. (work vector)"""
        return hArray(float,[self["nofAntennas"],self["nbins"],self["ncoeffs"]],name="Powers of Frequency")
    def default_covariance(self):
        """Array containign the covariance matrix of the fit. (outpur only)"""
        return hArray(float,[self["nofAntennas"],self["ncoeffs"],self["ncoeffs"]])
    def default_bwipointer(self):
        """Pointer to the internal BSpline workspace as integer. Don't change! """
        return 0
    def default_clean_bins_x(self):
        """Array holding the frequencies of the clean bins. (work vector)"""
        return hArray(dimensions=[self["nofAntennas"],self["nbins"]],properties=self["freqs"],name="Clean Frequencies")
    def default_clean_bins_y(self):
        """Array holding the powers of the clean bins. (work vector)"""
        return self["spectrum"] #hArray(properties=self["spectrum"],xvalues=self["clean"]_bins_x)
    def default_baseline_x(self):
        """Array holding the x-values and their powers for calculating the baseline fit."""
        return hArray(float,[self["fftLength"],self["ncoeffs"]],name="Powers of Frequency")
    def default_height_ends(self):
        """The heights of the baseline at theleft and right endpoints of the usable bandwidth where a hanning function is smoothly added."""
        return hArray(float,[2,self["nofAntennas"]])
    def default_nselected_bins(self):
        """Number of clean bins after RFI removal. (output only)"""
        return 0
    def default_selected_bins(self):
        """Array of indices pointing to clean bins, i.e. with low RFI. (work vector)"""
        return hArray(int,self["spectrum"],name="Selected bins")
    def default_coeffs(self):
        """Polynomial coeffieients of the baseline fit. (output vector)"""
        return hArray(float,[self["nofAntennas"],self["ncoeffs"]])
    def default_meanrms(self):
        """Estimate the mean rms in the spectrum per antenna. (output vector)"""
        return 0

#Now add the new workspsace to the overall workspace list
CRWorkSpaceList.append(CRFitBaselineWorkSpace)
#And tell it its name ...
CRWorkSpaceList[-1].name="FitBaseline"

class CRMainWorkSpace(CRWorkSpace):
    """
    ws=CRMainWorkSpace()

    WorkSpace for global parameters.

    This is a class to hold the variable, vectors, and arrays of all
    parameters used in the analysis.

    You can access the parameters using ws["parametername"] and set
    them using ws["parametername"]=val.

    Every known parameter has an associated function of the format
    '.global_parameter' to return a default value. The defaults will
    be set when calling the function ws.initParameters() or the first
    time when you access a parameter. A local copy will be made in an
    attribute of the class. So, you can access it also with

    ws.parametername

    Workspaces can be stacked hierarchically in a tree structure, by
    providing a parent workspace as an argument during creation, e.g.

    child_ws=CRFitBaselineWorkSpace(ws)

    Parameter can than be local or global. If a parameter is not yet
    set locally, it will be searched in the parent works space. If it
    is not found in either workspace, the default value will be
    calculated and assigned locally.

    You can assign your own value with ws["parname"]=value before
    initialization to avoid execution of the defaulting mechanism.

    E.g. to set the blocksize to 1024, simply set ws["blocksize"]=1024
    prior to calling initParameters.

    Note that the local copy of a value is only made once
    initParameters is called or the parameter explicitly with
    ws["..."]. So, if a global value changes, the change will not be
    immediately reflected in the ws.parname value - hence use that with
    care.

    ws.initParameters() - set all parameters (i.e. attributes) that do
    not exist yet, and assign a default value.

    ws.help() - get a list of all parameters and their values.

Available parameters:
    nbins = 256 - The number of bins in the downsampled spectrum used to fit the baseline.
    ncoeffs = 45 - Number of coefficients for the polynomial.
    polyorder = 44 - Order of the plyonomial to fit.  (output only)
    nofAntennas = 16 - Number of antennas held in memory.
    freqs = hArray(float) - Array of frequency values of the downsampled spectrum. (work vector)
    spectrum = hArray(float) - Array of power values holding the downsampled spectrum. (work vector)
    rms = hArray(float) - Array of RMS values of the downsampled spectrum. (work vector)
    rmsfactor = 2.0 - Factor above and below the RMS in each bin at which a bin is no longer considered.
    verbose = True - Print progress information during processing.
    selected_bins = hArray(int) - Array of indices pointing to clean bins, i.e. with low RFI. (work vector)
    numax_i = 27459 - Channel number in spectrum of the maximum frequency of the useable bandwidth. Negative if to be ignored.
    chisquare = Vec(int,16)=[1,1,1,2,1,...] - Returns the chisquare of the baseline fit. (output only)
    doplot = False - Make plots during processing to inspect data.
    numin = 12 - Minimum frequency of useable bandwidth. Negative if to be ignored.
    ratio = hArray(float) - Array holding the ratio between RMS and power of the downsampled spectrum. (work vector)
    xpowers = hArray(float) - Array holding the x-values and their powers for the fit. (work vector)
    bwipointer = 0 - Pointer to the internal BSpline workspace as integer. Don't change!
    nselected_bins = Vec(int,16)=[236,236,239,239,238,...] - Number of clean bins after RFI removal. (output only)
    clean_bins_y = hArray(float) - Array holding the powers of the clean bins. (work vector)
    clean_bins_x = hArray(float) - Array holding the frequencies of the clean bins. (work vector)
    baseline_x = hArray(float) - Array holding the x-values and their powers for calculating the baseline fit.
    numin_i = 3539 - Channel number in spectrum of the minimum frequency of the useable bandwidth. Negative if to be ignored.
    covariance = hArray(float) - Array containign the covariance matrix of the fit. (outpur only)
    numax = 82 - Maximum frequency of useable bandwidth. Negative if to be ignored.
    logfit = True - Actually fit the polynomial to the log of the (downsampled) data. (Hence you need to .exp the baseline afterwards).
    meanrms = Vec(float,16)=[3.48111317062,3.4784567903,3.47579235899,3.47705921127,3.47188685811,...] - Estimate the mean rms in the spectrum per antenna. (output vector)
    fftLength = 32769 - Length of unbinned spectrum.
    height_ends = hArray(float) - The heights of the baseline at theleft and right endpoints of the usable bandwidth where a hanning function is smoothly added.
    extendfit = 0.1 - Extend the fit by this factor at both ends beyond numax and numin. The factor is relative to the unsued bandwidth.
    t0 = 2.971779 - The cpu starting time of the processingin seconds, used for benchmarking.
    weights = hArray(float) - Array of weight values for the fit. (work vector)
    coeffs = hArray(float) - Polynomial coeffieients of the baseline fit. (output vector)
    fittype = BSPLINE - Determine which type of fit to do: fittype="POLY" - do a polynomial fit, else ("BSPLINE") do a basis spline fit (default).
    powers = hArray(int) - Array of integers, containing the powers to fit in the polynomial. (work vector)
    """
    def __init__(self,modulename=None,**keywords):
# Here list the parameters which have to be initialized in a
# particular order at the beginning (e.g., if the depend on each
# other). The attribute will be extended automatically.
        self.parameters=["filename","blocksize","datafile","nofAntennas","fftLength","frequency","numin","numax","numin_i","numax_i","ncoeffs"]
        CRWorkSpace.__init__(self,modulename=modulename,parent=None,**keywords)
    def default_filename(self):
        """Name of the data file to process."""
        return LOFARSOFT+"/data/lofar/rw_20080701_162002_0109.h5"
    def default_datafile(self):
        """Datafile object. Will be created from the filename and set to the right blocksize, if it does not exist yet."""
        return crfile(self["filename"]).set("blocksize",self["blocksize"])
    def default_blocksize(self):
        """Size (number of values) of each block to be read in."""
        return 2**16
    def default_nofAntennas(self):
        """Number of antennas in the datafile (output only)"""
        return self["datafile"]["nofAntennas"]
    def default_fftLength(self):
        """Size of the FFT or spectrum derived from the datareader object."""
        return self["datafile"]["fftLength"]
    def default_numin(self):
        """Minimum frequency of useable bandwidth. Negative if to be ignored."""
        return -1
    def default_numax(self):
        """Maximum frequency of useable bandwidth. Negative if to be ignored."""
        return -1
    def default_numin_i(self):
        """Channel number in spectrum of the minimum frequency of the useable bandwidth. (output only)"""
        if self["numin"]>0: return self["frequency"].findlowerbound(self["numin"]).val()
        else: return 1
    def default_numax_i(self):
        """Channel number in spectrum of the maximum frequency of the useable bandwidth. Negative if to be ignored."""
        if self["numax"]>0: return self["frequency"].findlowerbound(self["numax"]).val()
        else: return len(self["frequency"])
    def default_frequency(self):
        """Frequency values (x-axis) corresponding to FFT and spectrum"""
        return self["datafile"]["Frequency"].setUnit("M","")
    def default_spectrum(self):
        """Power as a function of frequency."""
        return hArray(float,[self["nofAntennas"],self["datafile"]["fftLength"]],fill=0,name="Spectrum",units="a.u.",xvalues=self["frequency"],par=[("logplot","y")])
    def default_cleanspec(self):
        """Copy of the spectrum with the gain curve and the spiky channels taken out."""
        return hArray(properties=self["spectrum"])
    def default_bad_channels(self):
        """Indexlist of bad channels containing RFI. (output only)"""
        return hArray(int,self["spectrum"],name="Bad Channels")
    def default_nbad_channels(self):
        """Number of bad channels (output only)"""
        return 0
    def default_rfi_nsigma(self):
        """Threshold for identifying a spike in Frequency as an RFI line to flag, in units of standard deviations of the noise."""
        return 5
    def default_fx(self):
        """Raw time series antenna data."""
        return self["datafile"]["emptyFx"]
    def default_fft(self):
        """FFT of the Raw time series antenna data."""
        return self["datafile"]["emptyFFT"]
    def default_ncoeffs(self):
        """Number of coefficients to describe the baseline."""
        return 18
    def default_coeffs(self):
        """Polynomial coeffieients of the baseline fit. (output vector)"""
        return hArray(float,[self["nofAntennas"],self["ncoeffs"]])
    def default_baseline(self):
        """Array with a baseline fit to the spectrum."""
        return hArray(properties=self["spectrum"],xvalues=self["frequency"],name="Baseline")
    def default_qualitycriteria(self):
        """a Python dict with keywords of parameters and
        tuples with limits thereof (lower, upper). Keywords currently
        implemented are mean, rms, spikyness (i.e. spikyness).
        Example: qualitycriteria={"mean":(-15,15),"rms":(5,15),"spikyness":(-7,7)}
        """
        return {"mean":(-15,15),"rms":(5,15),"spikyness":(-7,7)}
    def default_flaglist(self):
        """A list of bad antennas which failed the qualitycheck. (output only)"""
        return []

def CRWorkSpace_default_doplot(self):
    """Make plots during processing to inspect data."""
    return False
def CRWorkSpace_default_t0(self):
    """The cpu starting time of the processingin seconds, used for benchmarking."""
    return time.clock()
def CRWorkSpace_default_verbose(self):
    """Print progress information during processing."""
    return False


#Now add the new workspsace to the overall workspace list, the main workspace should be the first entry in the list
CRWorkSpaceList=[CRMainWorkSpace]+CRWorkSpaceList
#And tell it its name ...
CRWorkSpaceList[0].name="Main"

#Automatically add certain workspace methods
for ws in CRWorkSpaceList:
#Add creation methods to main WorkSpace
    setattr(CRMainWorkSpace,"make"+ws.name,ws)
#Add some of generic control methods and parameters to all workspaces
    for m in ["doplot","verbose","t0"]:
        setattr(ws,"default_"+m,eval("CRWorkSpace_default_"+m))


def CRsetWorkSpace(ws,modulename,**keywords):
    """
    Sets the workspace in a function if not defined already and
    initialize parameters. One can provide a global workspace and the
    functions will pick the module corresponding to modulename.

    ws - the workspace, if ws==None then create new one using function func

    modulename - name of the (sub)module to use, will call the
    initialization function (naming convention:
    "CR"+modulename+"WorkSpace") if workspace does not exist.

    keywords - local parameters to overwrite

    """
    func=eval("CR"+modulename+"WorkSpace")
    if ws==None: ws=func(**keywords)
    ws=ws.getModule(modulename)
    if ws==None: ws=func(**keywords)
    ws.setParameters(**keywords)
    return ws

def hCRCalcBaseline(baseline, frequency, numin_i,numax_i,coeffs,ws=None, **keywords):
    """
    hCRCalcBaseline(baseline, coeffs, frequency,ws=None, **keywords):

    Calculate a smooth baseline from a set of coefficients that
    determine the baseline (e.g. as calculated by hCRFitBaseline) and
    an array of frequencies (which need not be equi-spaced).

    """
    ws=CRsetWorkSpace(ws,"FitBaseline",**keywords)
    if ws["verbose"]:
        print time.clock()-ws["t0"],"s: Starting CalcBaseline."
    if ws["fittype"]=="POLY":
        """
        Calculate a smooth baseline for the full (large) spectrum from the polynomial coefficients
        """
        baseline.fill(0.0)
        baseline[...,numin_i:numax_i].polynomial(frequency[numin_i:numax_i],coeffs[...],ws["powers"][...])
    else:
        #Create the correct powers of x
        #        ws["baseline_x"][numin_i:numax_i].bsplinefitxvalues(frequency[numin_i:numax_i],ws["clean_bins_x"][...,0].val(),ws["clean_bins_x"][...,-1].val(),ws["ncoeffs"])
        ws["baseline_x"][numin_i:numax_i].bsplinefitxvalues(frequency[numin_i:numax_i],ws["clean_bins_x"][...,0].val(),ws["clean_bins_x"][...,ws["nselected_bins"]-1:ws["nselected_bins"]].val(),ws["ncoeffs"])
        baseline[...,numin_i:numax_i].bspline(ws["baseline_x"][numin_i:numax_i],ws["coeffs"][...])
    #Now add nice ends (Hanning Filters) to the frequency range to suppress the noise outside the usuable bandwidth
    #Left end
    import pdb; pdb.set_trace()
    ws["height_ends"][0,...].copy(baseline[...,numin_i])
    factor=hArray(float,ws["nofAntennas"],fill=6.9) # Factor 1000 in log
    if not ws["logfit"]:
        factor.copy(ws["height_ends"][0])
        factor *= 1000.0
    baseline[...,0:numin_i].gethanningfilterhalf(Vector(factor),Vector([ws["height_ends"][0]]),Vector(bool,ws["nofAntennas"],fill=True))
    #Right end
    ws["height_ends"][1,...].copy(baseline[...,numax_i-2])
    if not ws["logfit"]:
        factor.copy(ws["height_ends"][1])
        factor *= 1000.0
    baseline[...,numax_i-1:].gethanningfilterhalf(Vector(factor),Vector([ws["height_ends"][1]]),Vector(bool,ws["nofAntennas"],fill=False))
    if ws["logfit"]: baseline.exp()
    if ws["verbose"]:
        print time.clock()-ws["t0"],"s: Done CalcBaseline."
    if ws["doplot"]:
        baseline[...].plot(title="Baseline")
        raw_input("Plotted baseline - press Enter to continue...")

def hCRFitBaseline(coeffs, frequency, spectrum, ws=None, **keywords):
    """
    hCRFitBaseline(coeffs, frequency, spectrum, ws=None, **keywords)

    Function to fit a baseline using a polynomial function
    (fittype='POLY') or a basis spine fit to a spectrum while ignoring
    positive spikes in the fit (e.g., those coming from RFI = Radio Frequency
    Interference). The functions returns an array of coefficients of
    the polynomial or splines.

    Use baseline.polynomial(frequency,coeffs,powers) to caluclate the
    baseline from the coefficients.

    Parameters can be provided as additional keywords

    e.g., baseline.polynomial(frequency,coeffs,powers, parameter1=value1, parameter2=value2)

    or in a WorkSpace.

    e.g., baseline.polynomial(frequency,coeffs,powers, ws=WorkSpace),

    or in a mix of the two. Otherwise default values are used.

    A full list of parameters can be obtained with with:

    CRFitBaselineWorkSpace().help()
    """
#Defining the workspace, setting up the arrays, if not existing
    keywords["nofAntennas"]=spectrum.getDim()[0]
    ws=CRsetWorkSpace(ws,"FitBaseline",**keywords)
#
#Donwsample spectrum
    if ws["numin"]>0: ws["numin_i"]=frequency.findlowerbound(ws["numin"]).val()
    else: ws["numin_i"]=1
    if ws["numax"]>0: ws["numax_i"]=frequency.findlowerbound(ws["numax"]).val()
    else: ws["numax_i"]=len(frequency)
    ws["numax_i"]=min(ws["numax_i"]+int((len(frequency)-ws["numax_i"])*ws["extendfit"]),len(frequency))
    ws["numin_i"]=max(ws["numin_i"]-int(ws["numin_i"]*ws["extendfit"]),0)
    l=ws["numax_i"]-ws["numin_i"]
    if ws["nbins"]>l/4:
        print "Requested number of downsampled bins (",ws["nbins"],") too large for the number of frequency channels (",l,"):"
        ws["nbins"]=l/4
        print "Resetting nbins to ",ws["nbins"],"!!!"
    if ws["verbose"]:
        print time.clock()-ws["t0"],"s: Starting FitBaseline - Downsampling spectrum to ",ws["nbins"],"bins."
    ws["freqs"].downsample(frequency[ws["numin_i"]:ws["numax_i"]])
    ws["spectrum"][...].downsamplespikydata(ws["rms"][...],spectrum[...,ws["numin_i"]:ws["numax_i"]],1.0)
#Plotting
    if ws["doplot"]:
        spectrum[0].plot(title="RFI Downsampling")
        ws["spectrum"][...,0:l].plot(xvalues=ws["freqs"][0,0:l],clf=False)
        raw_input("Plotted downsampled spectrum - press Enter to continue...")
#Normalize the spectrum to unity
    ws["meanspec"]=ws["spectrum"][...].mean()
    ws["spectrum"][...] /= ws["meanspec"]
#Calculate RMS/amplitude for each bin
    ws["ratio"][...,0:l].div(ws["rms"][...,0:l],ws["spectrum"][...,0:l])
#    ws["ratio"][...,0:l].square()
    ws["mratio"]=ws["ratio"][...,0:l].meaninverse()
    if ws["doplot"]:
        ws["ratio"][...,0:l].plot(xvalues=ws["freqs"][0,0:l],title="RMS/Amplitude")
        raw_input("Plotted relative RMS of downsampled spectrum - press Enter to continue...")
#Now select bins where the ratio between RMS and amplitude is within a factor 2 of the mean value
    ws["nselected_bins"]=ws["selected_bins"][...].findbetween(ws["ratio"][...,0:l],ws.mratio/ws["rmsfactor"],ws.mratio*ws["rmsfactor"])
#Now copy only those bins with average RMS, i.e. likely with little RFI and take the log
    ws["clean_bins_x"][...].copy(ws["freqs"],ws["selected_bins"][...],ws["nselected_bins"])
    ws["clean_bins_y"][...].copy(ws["spectrum"][...],ws["selected_bins"][...],ws["nselected_bins"])
#    ws["weights"].copy(ws["clean_bins_y"])
    if ws["logfit"]:
        ws["clean_bins_y"][...,[0]:ws["nselected_bins"]].log()
#
    if ws["verbose"]: print time.clock()-ws["t0"],"s: Fitting baseline."
    if ws["fittype"]=="POLY":
        if ws["verbose"]:
            print "Performing a polynomial fit with ",ws["ncoeffs"],"coefficients."
        #Create the nth powers of the x value, i.e. the frequency, for the fitting
        ws["xpowers"][...,[0]:ws["nselected_bins"]].linearfitpolynomialx(ws["clean_bins_x"][...,[0]:ws["nselected_bins"]],ws["powers"][...])
        #Fit an nth order polynomial to the data
        ws["chisquare"]=coeffs[...].linearfit(ws["covariance"][...],ws["xpowers"][...],ws["clean_bins_y"][...],ws["nselected_bins"])  #ws["weights"][...],
    else:
        if ws["verbose"]:
            print "Performing a basis spline fit with ",ws["ncoeffs"]-2,"break points."
        #Perform a Basis Spline fit to the data
        ws["chisquare"]=coeffs[...].bsplinefit(ws["covariance"][...],ws["xpowers"][...,[0]:ws["nselected_bins"]],ws["clean_bins_x"][...,[0]:ws["nselected_bins"]],ws["clean_bins_y"][...,[0]:ws["nselected_bins"]])
    #Calculate an estimate of the average RMS of the clean spectrum after baseline division
    ws["ratio"][...].copy(ws["ratio"],ws["selected_bins"][...],ws["nselected_bins"])
    meanrms=ws["ratio"][...,[0]:ws["nselected_bins"]].meaninverse()
#    meanrms.sqrt()
    if ws["verbose"]: print time.clock()-ws["t0"],"s: Done fitting baseline."
    if ws["doplot"]:
        ws["clean_bins_y"][...,[0]:ws["nselected_bins"]].plot(xvalues=ws["clean_bins_x"][...,[0]:ws["nselected_bins"]],logplot=False)
        ws["clean_bins_y"].fill(0.0)
        if ws["fittype"]=="POLY":
            ws["clean_bins_y"][...,[0]:ws["nselected_bins"]].polynomial(ws["clean_bins_x"][...,[0]:ws["nselected_bins"]],coeffs[...],ws["powers"][...])
        else:
            ws["clean_bins_y"][...,[0]:ws["nselected_bins"]].bspline(ws["xpowers"][...,[0]:(ws["nselected_bins"])],coeffs[...])
        ws["clean_bins_y"][...,[0]:ws["nselected_bins"]-1].plot(xvalues=ws["clean_bins_x"][...,[0]:ws["nselected_bins"]-1],clf=False,logplot=False)
        raw_input("Plotted downsampled spectrum - press Enter to continue...")
    return meanrms


def hCRAverageSpectrum(spectrum,datafile,ws=None,**keywords): #blocks=None,fx=None,fft=None, verbose=False)
    """
    Usage: CRAverageSpectrum(spectrum,datafile,blocks=None,fx=None,fft=None)

    Reads several blocks in a CR data file, does an FFT and then
    averages the powers to calculuate an average spectrum. The
    parameters are:

    spectrum - a float array of dimensions [nofAntennas,fftLength],
    containing the average spectrum.

    datafile - a datareader object where the block size has been set
    appropriately.

    blocks - a list of blocknumbers to read
    (e.g. range(number_of_blocks)). Default is to read all blocks.

    fx - a work Array of dimensions
    [datafile.nofAntennas,datafile.blocksize] which is used to read in
    the raw antenna data. Will be created if not provided.

    fft - a work Array of dimensions
    [datafile.nofAntennas,datafile.fftLength] which is used to
    calculate the FFT from the raw antenna data. Will be created if
    not provided.

    verbose - Provide progress messages

    Available parameters in the Workspace (Example):

    datafile = crfile('/Users/falcke/LOFAR/usg/data/lofar/RS307C-readfullsecondtbb1.h5') - DataReader object to read the data from.
    max_nblocks = 3 - Absolute maximum number of blocks to average, irrespective of filesize.
    nblocks = 3 - Number of blocks to average, take all blocks by default.
    blocks = [0, 1, 2] - List of blocks to process.
    verbose = True - Print progress information during processing.
    fx = hArray(float) - Array to hold the x-values of the raw time series data. (work vector)
    fft = hArray(complex) - Array to hold the FFTed x-values (i.e. complex spectrum) of the raw time series data. (work vector)
    t0 = 2.971839 - The cpu starting time of the processing in seconds, used for benchmarking.
    doplot = True - Make plots during processing to inspect data.
    """
    ws=CRsetWorkSpace(ws,"AverageSpectrum",datafile=datafile,**keywords)
    if ws["verbose"]:
        maxcount=len(ws["blocks"])
        print time.clock()-ws["t0"],"s: Calculating",maxcount,"blocks of size",datafile["blocksize"]
        count=0;
        lastprogress=-1
        ws["t0"]=time.clock()
    for block in ws["blocks"]:
        ws["datafile"]["block"]=block
        # Hack: fix compatibility with new-style-IO module opened files. They don't have 'filename' attribute
        # as they can have multiple files underneath. In that case read directly and put result into ws["fx"].
        if 'filename' in dir(datafile):
            ws["fx"].read(datafile,"Fx")  # that's the old method
        else:
            ws["fx"] = datafile["Fx"] # case sensitivity... 
            
        ws["fft"][...].fftw(ws["fx"][...])
        spectrum[...].spectralpower(ws["fft"][...])
        if ws["verbose"]:
            count +=1
            progress=count*10/maxcount
            if not lastprogress == progress:
                t=time.clock()-ws["t0"]
                print progress*10,"% -",t,"s (Remaining:",t/count*maxcount-t,"s) - Calculated block #",block
                lastprogress=progress
    spectrum /= len(ws["blocks"])

def CheckParameterConformance(data,keys,limits):
    """
    Usage:

    qualitycriteria={"mean":(-15,15),"rms":(5,15),"spikyness":(-3,3)}

    CheckParameterConformance([Antenna,mean,rms,npeaks,spikyness],{"mean":1,"rms":2,"spikyness":4},qualitycriteria)  ->  ["rms",...]

    Parameters:

    data -  is a list of quality values (i.e. numbers) to check

    keys - a dictionary of fieldnames to be checked and indices
           telling, where in data the field can be found

    limits - a dictionary of fieldnames and limits (lowerlimit,
             upperlimit)

    Checks whether a list of numbers is within a range of limits. The
    limits are provided as a dictionary of fieldnames and tuples, of
    the form FIELDNAME:(LOWERLIMT,UPPERLIMIT). A list of fieldnames is
    returned where the data does not fall within the specified range.

    """
    result=[]
    for k in keys:
        if (data[keys[k]]<limits[k][0]) | (data[keys[k]]>limits[k][1]): result.append(k)
    return result


def CRQualityCheck(limits,datafile=None,blocklist=None,dataarray=None,nantennas=None,nbocks=None,maxblocksize=65536,blocksize=None,nsigma=5,verbose=True):
    """
    Usage:
    CRQualityCheck(limits,datafile=None,blocklist=None,dataarray=None,maxblocksize=65536,nsigma=5,verbose=True)

    Do a basic quality check of raw time series data, looking for rms,
    mean and spikes.

    If a datafile is provided it will step through all (selected)
    antennas of a file, assess the data quality (checking first and
    last quarter of the file), and return a list of antennas which
    have failed the quality check and their statistical properties.

    Instead of providing a datafile one can also provdide a data
    array, which will be processed in full.

    Example:

    >> datafile=crfile(filename)
    >> qualitycriteria={"mean":(-15,15),"rms":(5,15),"spikyness":(-7,7)}
    >> flaglist=CRQualityCheck(qualitycriteria,datafile,dataarray=None,maxblocksize=65536,nsigma=5,verbose=True) # -> list of antennas failing the limits


    Parameters:

    qualitycriteria - a Python dict with keywords of parameters and
    tuples with limits thereof (lower, upper). Keywords currently
    implemented are mean, rms, spikyness (i.e. spikyness).
    Example: qualitycriteria={"mean":(-15,15),"rms":(5,15),"spikyness":(-7,7)}

    datafile - Data Reader file object, if None, use values in
    dataarray and don't read data in again.

    datarray - an optional data storage array to read in the data if
    no datafile is specified, this array should contain the data to
    inspect.  In a pipeline where teh function is called multiple
    times, it is recommended to always provide this array, since it
    saves one the creation and destruction of the array.

    blocksize - The blocksize for reading in the data, will be
    determined automatically is not provided explicitly here.  If None
    use information from datafile or use first dimension of dataarray.

    natennas - How many antennas are in the dataarray or in the
    datafile. If None use information from datafile or use first
    dimension of dataarray. If equal to 0 then dataarray does not have
    that dimension.

    nblocks - How many antennas are in the dataarray or in the
    datafile.  If None use information from datafile or use first
    dimension of dataarray.

    maxblocksize - If the blocksize is determined automatically, this
    is the maximum blocksize to use.

    blocklist - The algorithms takes by default the first and last
    quarter of a file (and sets the blocksize accordingly). If you
    want to investigate all or other blocks, you need to provide the
    list explicitly here and also set the desired blocksize.

    nsigma - determines for the peak counting algorithm the threshold
    for peak detection in standard deviations

    verbose - sets whether or not to print additional information
    """
    #Initialize some parameters
    if not datafile==None:
        nAntennas=datafile.get("nofSelectedAntennas")
        selected_antennas=datafile.get("selectedAntennas")
        oldblocksize=datafile["blocksize"]
        filesize=datafile.get("filesize")
        if blocksize==None:
            blocksize=min(filesize/4,maxblocksize)
        if not oldblocksize == blocksize: datafile["blocksize"]=blocksize
        nBlocks=filesize/blocksize;
        if blocklist==None:
            blocklist=range(nBlocks/4)+range(3*nBlocks/4,nBlocks)
        if dataarray==None: dataarray=hArray(float,[nAntennas,blocksize])
        elif not len(dataarray)==(nAntennas*blocksize):
            print "CRQualityCheck: Data array provided does not match specified blocksize."
            return
    else:
        nAntennas=dataarray.getDim()[0]
        blocksize=dataarray.getDim()[1]
        selected_antennas=range(nAntennas)
        blocklist=[0]
#Create the some scratch vectors
    qualityflaglist=[]
#Calculate probabilities to find certain peaks
    probability=funcGaussian(nsigma,1,0) # what is the probability of a 5 sigma peak
    npeaksexpected=probability*blocksize # what is the probability to see such a peak with the blocksize
    npeakserror=sqrt(npeaksexpected) # what is the statistical error on that expectation
#Start checking
    if verbose:
        if not datafile==None: print "Quality checking of file ",datafile.filename
        print "Considering",nAntennas," antennas and the Blocks:",blocklist
        print "Blocksize=",blocksize,", nsigma=",nsigma, ", number of peaks expected per block=",npeaksexpected
    for Block in blocklist:
        if verbose:
            print "\nBlock = ", Block
            print "-----------------------------------------------------------------------------------------"
        if not datafile==None: datafile.set("block",Block).read("Voltage",dataarray.vec())
        datamean = dataarray[...].mean()
        datarms = dataarray[...].stddev(datamean)
        # Count # peaks while accounting for DC offset
        datanpeaks = dataarray[...].countgreaterthan(datamean + datarms*nsigma)
        datanpeaks += dataarray[...].countlessthan(datamean - datarms*nsigma)
        dataNonGaussianity = Vector(float,nAntennas)
        dataNonGaussianity.sub(datanpeaks,npeaksexpected)
        dataNonGaussianity /= npeakserror
        dataproperties=zip(selected_antennas,datamean,datarms,datanpeaks,dataNonGaussianity)
        noncompliancelist=[]
        for prop in iter(dataproperties):
            noncompliancelist=CheckParameterConformance(prop,{"mean":1,"rms":2,"spikyness":4},limits)
            if noncompliancelist:
                qualityflaglist.append([prop[0],Block,prop[1:],noncompliancelist])
                if not verbose:
                    print "Block= {0:5d}, Antenna {1:3d}: mean={2: 6.2f}, rms={3:6.1f}, npeaks={4:5d}, spikyness={5: 7.2f}".format(*((Block,)+prop))," ",noncompliancelist
            if verbose:
                print "Antenna {0:3d}: mean={1: 6.2f}, rms={2:6.1f}, npeaks={3:5d}, spikyness={4: 7.2f}".format(*prop)," ",noncompliancelist
    if not datafile==None: datafile["blocksize"]=blocksize
    return qualityflaglist

#qualitycriteria={"mean":(-15,15),"rms":(5,15),"spikyness":(-3,3)}
#CRQualityCheck(datafile,qualitycriteria,maxblocksize=65536,nsigma=5,verbose=True)

for v in hRealContainerTypes:
    for s in ["hCRAverageSpectrum","hCRFitBaseline","hCRCalcBaseline"]:
        setattr(v,s[1:].lower(),eval(s))


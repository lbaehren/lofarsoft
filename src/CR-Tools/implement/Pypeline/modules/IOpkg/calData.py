
################################################################################################
################################################################################################
#
#   	MODIFIED IOCal MODULE
#   	(author: C.W.James, c.james@astro.ru.nl)
#
################################################################################################
################################################################################################  


"""

Module Documentation


This module contains the 'calData' class only. It functions similarly
to the 'TBB Data' class, except is designed to perform Fourier
transforms, calibrations etc in a way which is opaque to the user.

The general useage is to init the class with a data file and some
othrr options, then run 'getData' to return the required type of
data.

See the doc string for the class itself for further information.

* NEEDS calibration to be implemented properly

* needs antenna selection to be implemented properly

* can NOT currently return frequency-selected data


"""
import pycrtools as cr
import numpy as np
import os
import metadata as md
import iocalibration as ioCal
import PPF as PPF
import interfaces
import IOfunctions as iof
import iocalibration as ioc


# Defines whether the crfile objects can return the selected RCUs or
# can only return all of them regardless of selection
CANNOT_SELECT=True

# Use this to force the calibration to be turned off
TURN_CAL_OFF=False

# standard verbose definition - though I don't check this as much as I ought!
VERBOSE=True

# controls whether cable lengths and clock delays have been already accounted for in delay buffers
SET_CAL_SHIFTS=True


class calData(interfaces.IObase):
    """ calibrated Data class
    
    ****This doc-string is currently out-of-date. It will be updated shortly.****
    
    This class returns calibrated data from TBB data dumps.
    
    The user specifies a set of files, and what sort of manipulation
    is required, under the general categories of calibration,
    filtering, checking, and Fourier transforming.
    
    Defined routines are:
    
    METHODS:
    
    	__init__(self, fileList=None, startBlock=None,  selection=None,\
filterType='Hanning', blockSize=1024, calMethod=True,\
fftMethod="fftcasa", checkMethod=None, genFFTArray=True)
    
    	check_OK(self)
    		This checks that all values have been initialised properly,
    	and that the data files are present.
    
    	initOwnArrays(self):
    		Creates output arrays, initialises filiters, and creates
    	arrays for time- and frequency-domain data.
    	
    	get_next_block(self, freqData=None):
    		gets the next block of data, manipulates it, and returns it.
    	
    	getFrequencies(self)
    		Returns a lst of frequencies corresponding to the fft outputs
    	    
    	reset(self, startBlock=None):
    		Resets the object assuming a startblock of startBlock, otherwise
	    everything is at its initial values
    
    	newAntennaSet(self, selection):
    		returns an antenna set object with the given selection
    
    
    	getAntennaPos(self):
	    returns the antenna positions from the crfile file
	
    	getReturnDim(self):
    		returns the dimenisons of the returned hArray objects - 
	    useful for creating such objects externally.
    
    IMPORTANT ATTRIBUTES:
    
    	fileNames (list of strings) [REQUIRED]
    
    	selection (List) [default: all]
    	    A subset of the antennas to use. These must be specified
	    by a tuple with each entry corresponding either to a
	    simple index (0 through to 95) or by their RCUids, e.g.
	    [142000001, ...] However, the two methods can NOT be mixed!
    
    	blockSize (Int) [default: 1024]
    	    Number of consecutive data elements to Fourier transform
    	    as a group. The number of frequency channels is thus
	    blockSize/2+1. This blocksize is the standard used in
	    LOFAR real-time data. Changing the blockSize causes
	    filterType to change to 'Hanning', since currently we
	    only have a method to generate Hanning filters of
	    arbitrary size.
    
    	filterType (string) [default: "Hanning"]
    	    A code for the type of filter to apply to the data. This
    	    gets applied before the fft step. Options are currently
	    only 'PPF', 'Hanning', and 'None', and PPF can only be
	    applied if the blockSize is 1024. If BlockSize != 1024,
	    the filter defaults to a Hanning filter.
    
    	fftMethod (string) [default: "fftcasa"]
    	    A string giving the hArray method for FFT-ing the data.
    	    The only two options are 'fftw' and 'fftcasa'. The general
    	    difference is that fftcasa ensures the frequencies have the
    	    correct ordering, but fftcasa is also generally slower.
    
    	calMethod (bool) [default: False]
    	    Just a bool with which to perform any inbuilt calibration
    	    routine. Currently, none is implemented, so this does nothing.
    	    But in the future this may change!
    
    	genFFTArray (bool) [defaut: True]
	    If true, the cal object generates its own timeData array to
    	    hold (and then return) FFT data. If false, it does not thus
    	    such an array must be passed to the getData routine every time.
    
    	antennaSet (string) [default: None]
    	    A temporary input telling the data reader which antenna set
    	    the data came from. In theory this should be written
    	    automatically into the data, but this is not yet implemented.
    	    Currently, the default parameter is effectively "LBA_Outer",
    	    since this is the value which gets passed to the cr data file
    	    object. I do not know the other options.
    
    	fftMethod (string) [default: CASA]
	    Defines which FFT routine to use. Defaults to 'CASA', other
	    options are 'FFTW' or None. If none, time-domain data only will
	    be returned.
	    
    	calMethod (bool) [default: True]
	    If True, retrieves a caldata object from the IO and applies this
	    to the data. If False, it does not.
	
	checkMethod (bool) [default: False]
	    If True, the data will be checked for impossibly large values. If
	    False, it will not be. Eventually, therte may be more checks
	    performed, but we hope that this option will never need be used!
    
    """
    
       
    def __init__(self, fileList, antennaSet, blockSize=1024,\
antennaSelection=None, startBlock=0, filterType='Hanning',\
calMethod=True, shiftYN=True, forceRCUorder=False,\
fftMethod='FFTW', checkMethod=None, ownArrays=True,\
genFFTArray=True, genTimeArray=True, frequencySelection=None,
fmin=None, fmax=None):

    	"""
    	A class designed to read in, manipulate, and return
	calibrated TBB data.
	    
	The only inputs required to get it to work are the data-
	file and an antenna set.
	    
	This init sets as many variables as possible without
	specifying the antennaSelection. If the antennaSelection
	IS specified, the class will completely initialise.
	
	The 'setAntennaSelection' must be used for a full
	initialisation.
    	    
    	This init returns nothing.
    
    	"""
    	
	if TURN_CAL_OFF:
	    if calMethod:
	    	print 'Sorry, calibration does not currently work'
	    	calMethod=False
	
    	# checks for file existence and opens files, returning cr file objects
    	self.crfiles=None
	self.nFiles=None
	self.shiftYN=shiftYN
	self.blockSize=blockSize
	self.fileList=fileList
	if startBlock < 0:
	    print 'bad startblock specification'
	    startBlock=0
	self.startBlock = startBlock
	self.currentBlock = startBlock
	
	# initialises files, including setting shifts, blockSize etc
	self.initFiles()
    	
    	# gets file data for th above and checks it is consistent.
    	(nyquistZone, sampleFrequency, frequencyRange)=self.getFileData()
	self.nyquistZone=nyquistZone
    	self.sampleFrequency=sampleFrequency
    	self.frequencyRange=frequencyRange
	
    	# converts specified fft method to something sensible
    	self.fftMethod=self.interpret_fftMethod(fftMethod)
    	
    	# converts specified filter type to something sensible
    	self.filterType=self.interpret_filterType(filterType)
    	
    	# checcking antennaset inputs
	if antennaSet == None:
    	    print 'we still need to get an antenna set automatically\
 - arbitrarily setting this to LBA Outer'
	    antennaSet="LBA_Outer"
    
    	# checking file-list inputs
	if fileList != None:
	    nFiles=len(fileList)
	else:
	    nFiles=0

# sets own values. Some are then altered later, but this is an exhaustive list
	self.ppf = None
#	if self.blockSize != 1024 and calMethod:
#	    print 'Cannot currently calibrate with this blocksize - turning calibration off'
#	    calMethod=False
    	self.calMethod = calMethod
	self.checkMethod = checkMethod
	self.antennaSet = antennaSet
	self.fileList=fileList
	self.readBlocks = 0
	self.timeData = None
	self.freqData = None
	self.calObject=None
	self.ready = 0
    	self.genFFTArray = genFFTArray
	self.genTimeArray = genTimeArray
	self.ownArrays=ownArrays
	self.initialised=False
	self.relativeAntennaPositions=None
	self.initialised=False
	self.forceRCUorder=forceRCUorder
	self.mapping=None
	self.opDim=None
	
	self.nSelectedFrequencies = None
	self.frequencyIndices = None
	self.selectFrequencies = False
	
	if self.fftMethod:
	    self.fftSize=self.blockSize/2+1
	else:
	    self.fftSize = None
	
	if (fmax or fmin) and (frequencySelection or not (fmax and fmin)):
	    print 'bad specification of frequency selection!'
	    print 'No frequencies selected'
	    self.setReturnedFrequencies()
	elif fmax and fmin:
	    self.setFrequencyRange(fmin, fmax)
	elif frequencySelection:
	    self.setFrequencySelection(frequencySelection)
	else:
	    self.selectFrequencies=False
	    self.setReturnedFrequencies()
	    
	    
	
	# checks and gets antenna selection from specified inputs
	# also sets self.RCU selection
    	if antennaSelection:
    	    self.setAntennaSelection(antennaSelection)
	else:
	    self.antennaSelection=None
	    self.nAntennas=None
	
    def getTimeDim(self):
    	"""
	returns the dimensonality of the time-domain data
	"""
	return [self.nAntennas, self.blockSize]
    
    def getAllFreqDim(self):
    	"""
	returns the dimensionality of the unselected frequency-
	domain data (e.g. all frequency channels)
	"""
	if self.fftMethod == 'NONE':
	    return None
	else:
    	    return [self.nAntennas, self.fftSize]
    
    def getOPDim(self):
    	"""
	returns the dimensionality of the output currently specified
	"""
	if not self.opDim:
	    print 'Selection not initialised! No o/p dimensions specified'
	    return None
	else:
	    return self.opDim

    def initialise(self):
    	"""
	initialises arrays, caldata objects etc
	This routine only gets called when an antenna selection is made
	"""
	if (self.initialised==True):
	    print 'Already initialised! Memory leaks may be generated'
	
	if not self.antennaSelection:
	    self.setAntennaSelection('All')
	
	if not self.selectFrequencies:
	    self.setAllFrequencies()
	if self.calMethod:
	    self.getCalibrator()
	else:
	    self.calArray=cr.hArray(complex,self.getOPDim(),)
	
	self.getAntennaPositions()
	
	## initialises the filter arrays
	if self.filterType == 'PPF':
	    self.ppf = PPF.PPF()
	    self.filter=None
	elif self.filterType == 'HANNING':
	    hfilter=cr.hArray(float,[self.blockSize])
	    hfilter.gethanningfilter()
	    self.filter=hfilter
	    self.ppf=None
	elif self.filterType == 'NONE':
	    self.filter=None
	    self.ppf=None
	## initialises the raw data array ##
	if self.genTimeArray:
	    self.timeData=cr.hArray(float, [self.nAntennas, self.blockSize])
	else:
	    # the time-data array must be provided each time
	    self.timeData=None
	
	## initialises the FFT array ##
	if self.fftMethod=='NONE':
	    # we are not returning fft data!
	    self.freqData=None
	    self.allFreqData=None
	    self.opDim=[self.nAntennas, self.blockSize]
	else:
	    self.fftSize = self.blockSize/2+1
	    if self.selectFrequencies:
	    	self.opDim = [self.nAntennas, self.nSelectedFrequencies]
	    else:
	    	self.opDim = [self.nAntennas, self.fftSize]
		
	    if (self.genFFTArray):
	    	if self.selectFrequencies:
		    self.allFreqData = cr.hArray(complex, [self.nAntennas, self.fftSize])
		    self.freqData = cr.hArray(complex, [self.nAntennas, self.nSelectedFrequencies])
		else:
		    self.allFreqData=None
		    self.freqData = cr.hArray(complex, [self.nAntennas, self.fftSize])
	    else:
	    	self.allFreqData = None
		self.freqData=None
	
	self.initialised=True
	return None

    def interpret_fftMethod(self, fftMethod):
    	"""
    	converts possible alternative inputs to real ones
    	"""
	if (fftMethod == 'casa') or (fftMethod == 'fftcasa') or (fftMethod == 1):
	    fftMethod = 'CASA'
	elif (fftMethod == 'fftw') or (fftMethod == 2) or (fftMethod == 'FFTW'):
	    fftMethod = 'FFTW'
	elif (fftMethod == None) or (fftMethod == 'None') or (fftMethod == 'none') \
or fftMethod == 'NONE':
	    fftMethod = 'NONE'
	else:
	    print 'Unspecified fft method - defaulting to CASA'
	    fftMethod = 'CASA'
	return fftMethod
	
    def interpret_filterType(self, filterType):
    	"""
    	converts possible filterType specifications into something sensible
    	"""
	if filterType == 'None' or filterType==None or filterType == 'none':
	    filterType = 'NONE'
	elif filterType == 'Hanning' or filterType == 'hanning':
	    filterType = 'HANNING'
	elif filterType == 'PPF' or filterType == 'ppf':
	    filterType = 'PPF'
	else:
	    print 'Unspecified filter type - defaulting to Hanning'
	    filterType = 'HANNING'
	return filterType
	
	if blockSize != 1024 and filterType == 'PPF':
	    print 'PPF unspecified for a blocksize of ', blockSize
	    filterType = 'HANNING'
	return filterType
    
    def initFiles(self):
    	"""
	Initialises the files by opening them, setting the blocksize,
	and apply shifts
	"""
	
	self.openFiles()
    	self.setBlockSize()
	if self.shiftYN:
	    self.applyShifts()
	self.setBlock(self.startBlock)
	
	if CANNOT_SELECT:
	    self.initStupidSelection()
    
    def setAllFrequencies(self):
    	"""
	Simply tells the routine to use all available frequencies
	"""
    	
	self.frequencyIndices=range(self.fftSize)
	self.selectFrequencies=False
	self.nSelectedFrequencies=self.fftSize
	self.setReturnedFrequencies()
	
	
    def setFrequencyRange(self, selectedfmin, selectedfmax, inclusive=True):
    	"""
	Sets the frequency selection used in subsequent calls to
        `getFFTData`.

        Arguments:
        *selectedfmin*: Minimum wanted frequency
	* selectedfmax*: maximum wanted frequency
	inclusive: True by default. Includes sub-bands partly covered
	by the range if true, excludes them if false.
	
	Return value:
	It returns the frequency index list, as would also be
	returned by getFrequencyIndicies(), corresponding to the
	indices of the freqData array which will be picked out.
	"""
	
	fmin=(self.nyquistZone-1)*self.sampleFrequency
	fmax=self.nyquistZone*self.sampleFrequency
	df = self.sampleFrequency/self.fftSize
	
	if self.fftMethod == 'FFTW':
	    temp=fmin
	    fmin=fmax
	    fmax=temp
	    df = -df
	
	i1=int((selectedfmin-fmin)/df)
	i2=int((selectedfmax-fmin)/df)
    	
	imin=min(i1,i2)
	imax=max(i1,i2)
	
# applies the inclusive/exclusive part, accounts for fs being out of range
	if inclusive:
	    if imin < 0:
	    	imin=0
	    if imax >= self.fftSize-1:
	    	imax = self.fftSize-1
	    else:
	    	imax += 1
	else:
	    if imin < 0:
	    	imin = 0
	    else:
	    	imin += 1
	    if imax >= self.fftSize-1:
	    	imax = self.fftSize-1
	
	if imax <= imin:
	    print 'No frequencies in this range!'
	    return None
	
	ownFreqs = range(imin,imax)
	
	self.nSelectedFrequencies = imax-imin
	self.frequencyIndices=ownFreqs
	self.selectFrequencies=True
	self.setReturnedFrequencies()
	self.initialised=False
	
    
    def setFrequencyIndices(self, indices):
    	"""
	Sets the returned frequencies to the desired indices
	"""
	npi=np.array(indices)
	correct=np.where(npi >= 0)
	modindices=indices[correct]
	correct=np.where(modindices < self.fftSize)
	modindices=modindices[correct]
	if len(modindices) < len(npi):
	    print 'Not all indices were available!'
	self.nSelectedFrequencies=len(modindices)
	self.frequencyIndices=modindices
	self.selectFrequencies=True
	self.setReturnedFrequencies()
	self.initialised=False
    
    def setFrequencySelection(self, frequencySelection, prune=True):
    	"""
	Sets the frequency selection used in subsequent calls to
        `getFFTData`.

        Arguments:
        *frequencies* FloatVector of frequencies in Hz
	
	Return value:
	It returns the frequency index list, as would also be
	returned by getFrequencyIndicies(), corresponding to the
	indices of the freqData array which will be picked out.
	"""
	if not frequencies:
	    print 'No selection specified!'
	    return None
	
	fmin=(self.nyquistZone-1)*self.sampleFrequency
	fmax=self.nyquistZone*self.sampleFrequency
	df = self.sampleFrequency/self.fftSize
	
	if self.fftMethod == 'FFTW':
	    temp=fmin
	    fmin=fmax
	    fmax=temp
	    df = -df
	
	ownFreqs=range(fmin, fmax, df)
	frequencyIndices=[]
	for f in frequencySelection:
	    findex = int((f-fmin)/df)
	    if not (findex in frequencyIndices) or not prune:
	     	if findex >= 0 and findex < fftSize:
	    	    frequencyIndices.append(findex)
		else:
		    print 'Frequency ', f, ' is out of range!'
	    else:
	    	if VERBOSE:
		    print 'Frequency ', f, ' is repeated - ignoring.'
	
	self.nSelectedFrequencies = len(frequencyIndices)
	if prune and self.nSelectedFrequencies == range(self.fftSize):
	    print 'All frequencies selected! This is being wasteful! (idiot...)'
	
	self.frequencyIndices=frequencyIndices
	self.selectFrequencies=True
	self.setReturnedFrequencies()
	self.initialised=False
    
    def getFrequencies(self):
    	"""
	Returns a hArray object of frequencies corresponding
	to the returned data
	"""
	return cr.hArray(self.frequencies)
    
    def setReturnedFrequencies(self):
    	"""
	generates a vector giving the centre frequencies of the
	returned fftData and sets 
	"""
	if self.fftMethod == 'None':
	    frequencies=None
	elif self.fftMethod == 'FFTW':
	    f1=self.nyquistZone*self.sampleFrequency/2.
	    f2=(self.nyquistZone-1)*self.sampleFrequency/2.
	    df=(f2-f1)/(self.fftSize-1.)
	    frequencies=iof.frange(f1,f2+df/2.,df)
	else:
	    f1=(self.nyquistZone-1)*self.sampleFrequency/2.
	    f2=self.nyquistZone*self.sampleFrequency/2.
	    df=(f2-f1)/(self.fftSize-1)
	    frequencies=iof.frange(f1,f2+df/2.,df)
	if self.selectFrequencies:
	    self.frequencies=[]
	    for index in self.frequencyIndices:
	    	self.frequencies.append(frequencies[index])
	else:
	    self.frequencies=frequencies
	#    self.frequencies=frequencies[self.frequencyIndices]
	
    def initStupidSelection(self):
    	"""
	Initialises methods and data for the current IO which can not return data for specific
	antennas only. Yes, it IS retarded.
	"""
	self.totalAntennas=[]
	
	for thefile in self.crfiles:
	    self.totalAntennas.append(thefile.get("nofAntennas"))
	
	self.maxUnselectedAnts=max(self.totalAntennas)
	
	self.unselectedData=cr.hArray(float,[self.maxUnselectedAnts, self.blockSize])
	
    
    def setBlock(self, block=None):
    	"""
	Sets the first block to be read, with '0' being 'start from the beginning'
	"""
	if not block:
	    block=self.startBlock
	for crfile in self.crfiles:
	    crfile.set("block",block)

    def setBlockSize(self, blockSize=None):
    	"""
	Sets the block size, using the internal value as default
	"""
	if not blockSize:
	    blockSize=self.blockSize
	for crfile in self.crfiles:
	    crfile["blocksize"]=blockSize

    def openFiles(self):
    	"""This function opens files using the cr.crfile(filename) method
    
    	"""
    	fileList=self.fileList
	listoffiles=[]
	files=[]
	##### APPLY A CHECK TO SEE IF THE FILES EXISTS FIRST!!!!! ########
	
    	for filename in fileList:
    	    if os.path.isfile(filename):
	    	files.append(cr.crfile(filename))
		listoffiles.append(filename)
	    else:
	    	print 'file ', filename, ' can not be found'
	
	self.nFiles=len(files)
	self.crfiles=files
	self.fileList=listoffiles
	
    def setAntennaSelection(self, SelAntIDs=None):
        """Sets the antenna selection used in subsequent calls to
        `getAntennaPositions`, `getFFTData`, `getTimeseriesData`.

        Arguments:
        *antennaIDs* Either Python list with index of the antenna as
                     known to self (integers (e.g. [1, 5, 6]))
                     Or list of IDs to specify a LOFAR dipole
                     (e.g. ['142000005', '3001008'])

        Return value:
        This method does not return anything.
	
        It raises a ValueError if antenna selection cannot be set
        to requested value (e.g. specified antenna not in file).
	
	We want the antenna selection to be a list of size nfiles,
	with each entry being the antennas selected from that file
	
	Note that this function is rather long. It has to be to make
	sure we are sufficiently careful!
        """
        
	if SelAntIDs=='ALL' or SelAntIDs=='all' or SelAntIDs == None\
or SelAntIDs == 'All':
	    SelAntIDs=='All'
	    self.forceRCUorder=False
	
	# bool to tell if any duplicate entries have been detected
    	duplicates=False
	
    #	generates a 1-D list of RCU numbers from all the files in order
	allRCU_1Ds=[]
	allRCU_2Ds=[]
	
	for i in range(self.nFiles):
	    theseIDs=self.crfiles[i].get("antennaIDs")
	    allRCU_2Ds.append(theseIDs)
	    for ID in theseIDs:
	    	if ID in allRCU_1Ds:
		    if not duplicates:
		    	print 'warning - duplicate RCU detected'
		    	duplicates=True
	    	allRCU_1Ds.append(ID)
    	selection=[]
	selection2D=[]
	RCUSelection=[]
	RCUSelection2D=[]
	
	if (SelAntIDs=='All'):
	    # uses all of them by default
	    selection=range(len(allRCU_1Ds))
            self.antennaSelection=selection
	    self.RCUSelection=allRCU_1Ds
	    SelAntIDs = selection
	    for crfile in self.crfiles:
	    	selection2D.append(range(crfile.get("nofAntennas")))
	    self.antennaSelection2D=selection2D
	    self.RCUSelection2D=allRCU_2Ds
    	elif SelAntIDs[0] > 1000:
	    # specified by RCU number
    	    for (num,ant) in enumerate(allRCU_1Ds):
    	    	if ant in SelAntIDs:
    	    	    selection.append(num)
		    RCUSelection.append(ant)
	    
	    for (i,IDs) in enumerate(allRCU_2Ds):
	    	selection2D.append([])
		RCUSelection2D.append([])
	    	for (j, ID) in enumerate(IDs):
		    if ID in SelAntIDs:
		    	selection2D[i].append(j)
			RCUSelection2D[i].append(ID)
	    self.antennaSelection2D=selection2D
	    self.RCUSelection2D=RCUSelection2D
	    self.antennaSelection=selection
	    self.RCUSelection=RCUSelection
	    
	else:
	    # specified by integer numbers in the file
	    nAvailable=len(allRCU_1Ds)
	    maxRequired=max(SelAntIDs)
	    if nAvailable < maxRequired:
	    	print 'Not enough antennas to satisfy request'
	    	for ID in SelAntIDs:
		    if ID < nAvailable:
		    	selection.append(ID)
		    else:
		    	print 'Antenna number ', ID, ' unavailable'
	    else:
	    	selection=SelAntIDs
	    
	    self.antennaSelection = selection
	    start=0
	    for (i,IDs) in enumerate(allRCU_2Ds):
	        selection2D.append([])
	        RCUSelection2D.append([])
	        these=range(start,start+len(IDs))
	        for sel in self.antennaSelection:
    	            if sel in these:
		    	selection2D[i].append(sel)
			RCUSelection2D[i].append(allRCU_2Ds[i][sel])
	    self.antennaSelection2D=selection2D
		
	    self.RCUSelection=[]
	    for ID in self.antennaSelection:
	    	self.RCUSelection.append(allRCU_1Ds[ID])
    	
	if duplicates:
	    temp_selection=[]
	    temp_selection2D=[]
	    temp_RCUSelection=[]
	    temp_RCUSelection2D=[]
	    for (num1,ID1) in enumerate(self.antennaSelection2D):
	    	temp_RCUSelection2D.append([])
		temp_selection2D.append([])
	    	for (num2, ID2) in enumerate(ID1):
		    if not (ID2 in temp_selection):
		    	temp_selection.append(ID2)
			temp_RCUSelection.append(self.RCUSelection2D[num1][num2])
			temp_RCUSelection2D[num1].append(self.RCUSelection2D[num1][num2])
			temp_selection2D[num1].append(ID2)
	    self.RCUSelection2D=temp_RCUSelection2D
	    self.RCUSelection=temp_RCUSelection
	    self.antennaSelection=temp_selection
	    self.antennaSelection2D=temp_selection2D
	
	if not len(self.antennaSelection) == len(SelAntIDs):
	    	print 'some RCU numbers could not be found'
	#	for ID in SelAntIDs:
	#	    if not (ID in self.antennaSelection or ID in self.RCUSelection):
	#	    	print 'Requested RCU ', ID, ' not present'
	
	self.nAntennas=len(self.antennaSelection)
	if self.forceRCUorder:
	    generate_mapping(SelAntIDs)
	else:
	    self.mapping=None
	
	
    def generate_mapping(self, orderedIDs):
    	"""
	generates a map that says that the xth requested antenna can be found
	in the yth crfile at position z, via an array of A[x]=[y,z]
	"""
	mapping=[]
	allids=[]
	for crfile in self.crfiles:
	    allids.append(crfile.get("antennaIDs"))
	if orderedIDs[0] > 1000:
	    ### RCU numbers
	    for rcu in orderedIDs:
	    	for (anum,ifile) in allids:
	    	    for (num,ant) in enumerate(ifile):
		    	if ant == rcu:
		    	    mapping.append[anum,num]
	else:
	    for ID in orderedIDs:
	    	start=0
	    	for (anum,ifile) in allids:
		    length=len(ifile)
	    	    for (num,ant) in enumerate(range(start,start+length)):
		    	if ant == ID:
		    	    mapping.append[anum,num]
		    start += length
	self.mapping=mapping
    
    def setAntennaSet(self, antennaSet = "LBA_Outer"):
    	""" sets the antenna set - currently, no method to do this
	automatically, which should be chaged. Stupid thing!
	Sets it to LBA_Outer automatically now
	"""
	self.antennaSet = antennaSet
	# resets the antenna positions for the new antenna set
	self.getAntennaPositions()
    
    def getFileData(self):
    	"""
	Retrives information from the cr file and sets it to internal variables
	It also checks that the information is identical for all open files
	The info used is nyquistzone, sample frequency, and frequency range.
	Antenna set currently can not be given.
	"""
    	
	nyquistZone = self.crfiles[0].nyquistZone
	sampleFrequency = self.crfiles[0].sampleFrequency
	frequencyRange = self.crfiles[0].frequencyRange
	
	if (self.nFiles > 1):
	    for i in range(self.nFiles-1):
	    	if (not nyquistZone == None) and (not self.crfiles[i+1].nyquistZone == nyquistZone):
		    print 'Nyquist zones differ between files'
		    nyquistZone = None
		if (not sampleFrequency == None) and (not self.crfiles[i+1].sampleFrequency == sampleFrequency):
		    print 'sampleFrequencies differ between files'
		    sampleFrequency=None
		if (not frequencyRange == None) and (not self.crfiles[i+1].frequencyRange == frequencyRange):
		    print 'frequencyRanges differ between files'
		    frequencyRange=None
	
	# gets the minimum number of blocks
	self.nBlocks=999999999
	for i in range(self.nFiles):
	    size=self.crfiles[i].filesize/self.crfiles[i].nofAntennas/self.blockSize
	    if size < self.nBlocks:
	    	self.nBlocks=size
	self.nyquistZone=nyquistZone
	self.sampleFrequency=sampleFrequency
	self.frequencyRange=frequencyRange
	
	return (nyquistZone, sampleFrequency, frequencyRange)
    
    def getAntennaPositions(self):
        """Returns antenna positions for selected antennas, or all
        antennas if no selection was applied.

        Arguments:
        None

        Return value:
        a two dimensional array containing the 3-D Cartesian positions of
        each antenna in meters in local coordinates from a predefined
        center.
        So that if `a` is the returned array `a[i]` is an array of
        length 3 with positions (x,y,z) of antenna i.
	
	Notes:
	This function is called automatically whenever the setAntennaSet is called, since
	the antenna positions depend on it
        """
	if self.RCUSelection == None or self.antennaSet == None:
	    print 'Not enough info - please specify antennaSet and RCUSelection'
	    return None
        if self.relativeAntennaPositions==None:
	    self.relativeAntennaPositions = md.get("RelativeAntennaPositions", self.RCUSelection, self.antennaSet, True)
	
    	return self.relativeAntennaPositions
    
    def getCalibratorOld(self):
        """Returns  a calibrator object from the calibration class. 
        This can be used to do a phase calibration on the fftdata.
        For this the antennaset should have been specified or given as
        a parameter.

        *antennaset* Antennaset e.g. "HBA", "LBA_OUTER". If provided this
        will also set this antennaset for the TBBdata object.
	
	Note that this likely does NOT yet work, and this method will
	never get called.
        """
    	if self.antennaSet == None or self.antennaSelection == None:
	    print 'can not get calibrator - no antenna set specified!'
	    return None
        else:      
            self.calObject=ioCal.AntennaCalibration(self.crfiles,self.antennaSet,self.antennaSelection)
    
    def applyShifts(self):
    	"""
	Sets the shift argument correctly over multiple files, and on
	all antennas. It shifts everything relative to the earliest
	start time.
    	"""
	crfiles=self.crfiles
	nFiles=self.nFiles
	print crfiles, 'look here!!!'
    	times=iof.get(crfiles,"TIME", False)
    	tmin=times.min()
    	tmax=times.max()
    	#time stamp cannot be handled by integer function
    	assert tmax -tmin < 22
    	
    	shifts=[]
    	for i in range(nFiles):
#    	    crfiles[i]["blocksize"]=self.blockSize
    	    shifts.append(crfiles[i]["SAMPLE_NUMBER"])
    	    shifts[i].muladd(crfiles[i]["TIME"]-tmin,\
cr.Vector(int,crfiles[i]["nofSelectedAntennas"], int(crfiles[i]["sampleFrequency"])))
    	
# search for maximum in shifts
    	shiftmax=0
    	for i in range(0,nFiles):
    	    localmax=shifts[i].max()
    	    if localmax>shiftmax:
    	        shiftmax=localmax
    	
    	for i in range(0,nFiles):
    	    shifts2=[]
    	    for j in range(0,len(shifts[i])):
    	        shifts2.append(shiftmax-shifts[i][j])
    	    crfiles[i]["ShiftVector"]=shifts2
    
    ###########
    def assignArrays(self, timeData=None, finalFreqData=None, tempFreqData=None):
    	"""
	This function checks if the specified arrays are of the correct
	type, and assigns internal arrays as appropriate
	"""
    	### handles time data correctly ###
	if timeData:
	    if (isinstance(timeData, cr.FloatArray) and timeData.getDim() == self.getTimeDim()):
	    	pass
	    else:
	    	print 'incorrect timeData specified',\
' - must be a cr.FloatArray with dimensions ', self.timeDim
    	    	return None
	else:
	    timeData=self.timeData
	    if not self.timeData:
	    	print 'No array for time-domain data specified! Please call with timeData set'
		return None
	
	### handles temp data correctly ###
	if self.fftMethod=='None':
	    if tempFreqData:
	    	print 'warning: No fft specified - the provided freq data will not be altered'
		tempFreqData=None
	else:
	    if tempFreqData:
	    	if not self.selectFrequencies:
	    	    print 'no freq selection: temp freq ignored'
	    	elif isinstance(tempFreqData, cr.ComplexArray) and\
tempFreqData.getDim() == [self.nAntennas, self.fftSize]:
    	    	    # array of the correct format
		    pass
	    	else:
	    	    print 'incorrect temp freq specification'
		    tempFreqData=None
	    elif self.selectFrequencies == True:
		tempFreqData=self.allFreqData
	    else:
	    	print 'it is false!'
		tempFreqData=self.freqData
	
	### handles final frequency data ###  
	if self.fftMethod == 'None':
	    if finalFreqData:
	    	print 'warning: No fft specified - the provided freq data will not be altered'
		finalFreqData=None
	elif finalFreqData:
	    if (isinstance(finalFreqData, cr.ComplexArray) and\
finalFreqData.getDim() == self.getOPDim()):
	        pass
	    else:
	    	print 'incorrect freqData specified',\
' - must be a cr.FloatArray with dimensions ', self.getOPDim()
    	    	finalFreqData=None
	else:
	    finalFreqData=self.freqData
	
	return (timeData, finalFreqData, tempFreqData)
		
    
    def getAntennaSelection(self, RCUs=False, return2D=True):
    	"""
	Returns the antenna selection in a standard python list.
	
	RCUs True returns the RCU number; otherwise only the indicies
	are returned.
	
	return2D=True returns a 2D list with the 1st dimension over
	each open file, and the second dimension over each antenna
	indice. False just gives a 1D list.
	"""
	
	if RCUs:
	    if return2D:
	    	toreturn=self.RCUSelection2D
	    else:
	    	toreturn=self.RCUSelection
	else:
	    if return2D:
	    	toreturn=self.antennaSelection2D
	    else:
	    	toreturn=self.antennaSelection
	return toreturn
    
    def getFFTData(self, timeData=None, finalFreqData=None, allFreqData=None):
	"""
	A routine to return the 'next' block of interest.
	
	It simple reads in the next block as defined in the
	initialisation, performs whatever checks, filtering, and
	other manipulation has been requested, and returns the
	outcome.
	
	The only possible input is 'freqData', which if	set, returns
	the data in the user-specified array.
	
	"""
	if not self.initialised:
	    self.initialise()
	if not self.antennaSelection:
	    print 'No antennas selected - please specify!'
	    return None
	if not self.initialised:
	    print 'Object not initialised - cannot return data'
	    return None
	
	
    	(timeData,finalFreqData, freqData) = self.assignArrays(timeData, finalFreqData, allFreqData)
# sets the crfile object antenna selection and reads in the data
    	
	self.getTimeData(timeData, self.currentBlock)

# updates the number of read blocks, and the next block to be read 
	self.currentBlock += 1
	self.readBlocks += 1

# filters the data if applicable
	if self.filterType == 'PPF':
	    self.ppf.add(timeData)
	elif self.filterType != 'NONE':
	    timeData[...].mul(self.filter)
# performs a check on data quality if applicable
	if self.checkMethod:
	    if timeData.max().val() >= 2048 or timeData.min().val() < -2048:
	    	print 'Impossible data values detected'
		return None
	
# performs the Fourier Transform if appllicable
	if self.fftMethod == 'CASA':
	    # note here that the ... will loop over antennas in the antenna selection
	    freqData[...].fftcasa(timeData[...],self.nyquistZone)
	elif self.fftMethod == 'FFTW':
	    freqData[...].fftw(timeData[...])
	else:
	    return timeData

# calibrates the data in the frequency domain using internal vales
	if self.calMethod:
#	    self.calObject.applyCalibration(freqData)
    	    freqData.mul(freqData, self.calTable)
    	
	# returns only some frequency data. Needless to say, this is very wasteful!
	if self.selectFrequencies:
	    for i in range(self.nAntennas):
	    	finalFreqData[i].copy(freqData[i,self.frequencyIndices])
	    #finalFreqData[...].copy(freqData[...,self.frequencyIndices])
	else:
	    finalFreqData=freqData
	    
    	return finalFreqData
    
    
    def getFrequencies(self):
    	"""
	Returns an standard Python list of the selected frequencies
	in each band in Hz
	"""
	
	return self.frequencies
	
    def getSelectedData(self, timeData, starti, crfile, selection):
    	"""
	Artificially applies data selection the LONG way, unless all data is required
	Eventually, this function will be removed (I hope!).
	"""
	nants=crfile.get("nofAntennas")
	if selection == range(nants):
	    timeData[starti:starti + nants].read(crfile,"Fx")
    	else:
	    if not self.unselectedData:
	    	self.unselectedData = cr.hArray(float,[self.maxUnselectedAnts, self.blockSize])
	
	    self.unselectedData[0:crfile.get("nofAntennas")].read(crfile, "Fx")
	
	    for j,which in enumerate(selection):
	    	timeData[starti+j].copy(self.unselectedData[which])
    
    
    def getTimeData(self, timeData=None, block=None):
    	"""
	Retrieves raw timeseries data for specified antennas.
	"""
	if not self.initialised:
	    self.initialise()
	
	if not block:
	    block=self.currentBlock
	
	if not timeData:
	    timeData=self.assignArrays()[0]
	
	self.setBlock(block)
	
	#gets the right time data array to read in to
	if not timeData:
	    if not self.timeData:
	    	print 'no time data array specified - please do so!'
		return None
	    else:
	    	timeData=self.timeData
	
	# this forces the output order to be in the same order as the requested inputs
	# the mapping reminds us which file and antenna to get what data from
	if self.forceRCUorder:
	    for j,themap in enumerate(mapping):
		    if CANNOT_SELECT:
		    	self.getSelectedData(timeData, j, crfile[themap[0]],[themap[1]])
		    else:
		    	self.crfiles[themap[0]].set("antennaSelection", [themap[1]])
			timeData[j].read(crfiles[themap[0]],"Fx")
	else:
    	    start=0
	    end=0
	    for i in range(self.nFiles):
	    	end += len(self.antennaSelection2D[i])
	    	if CANNOT_SELECT:
		    self.getSelectedData(timeData, start,self.crfiles[i], self.antennaSelection2D[i])
		else:
	    	    timeData[start:end].read(self.crfiles[i],"Fx")
	    	start = end
    
    def setCalibration(self):
    	"""
	defines the calibration matrix for the chosen frequencies and antennas
	"""
	self.calibrator=ioc.AntennaCalibration(self.crfiles, self.antennaSet)
	
    def getCalibrator(self):
    	"""
	*** internal function only ***
	- defines the calibration matrix for the chosen frequencies and antennas,
	- sets further offsets in the data
	
	"""
	
    #### Retrieves the Information from Metadata ####
    	
	# CableDelays
	# calculates the delay time in terms of whole numbers of samples
	# (CableTimeDelays) and fractions (CableFractionalDelays)
        CableDelays=md.get("CableDelays",self.RCUSelection,self.antennaSet,True)
        CableFractionalDelays=CableDelays.new()
        CableFractionalDelays.fmod(CableDelays,1./self.sampleFrequency)
        CableTimeDelays=CableDelays.new()
        CableTimeDelays.sub(CableDelays,CableFractionalDelays)
    	
	# Clock Offsets
        # gets the clock offsets, and converts them into whole and fractional
	# parts of the sampling time, as with the cable delays above
        ClockDelays=md.get("ClockCorrection",self.RCUSelection,self.antennaSet,True)
        ClockFractionalDelays=ClockDelays.new()
        ClockFractionalDelays.fmod(ClockDelays,1./self.sampleFrequency)
        ClockTimeDelays=ClockDelays.new()
        ClockTimeDelays.sub(ClockDelays,ClockFractionalDelays)
	
    #### Calculates and applies the total shift (coarse calibration) ####
    	# note that this can already be done in delay buffers, so in general
	# this method is turned off
	if SET_CAL_SHIFTS:
	    totalShift=ClockTimeDelays.new()
            totalShift.copy(ClockTimeDelays)
	    
	    nrAnts=iof.get(self.crfiles,"nofSelectedAntennas",True)
	    
            allinitialShift=cr.hArray(iof.get(self.crfiles,"shift",False))
            allinitialShift.add(totalShift)
            shifts=[]
            startAnt=0
            endAnt=0
            for num,nAnts in enumerate(nrAnts):
            	startAnt=endAnt
            	endAnt+=nAnts
            	shifts.append(allinitialShift[startAnt:endAnt].val())
            iof.set(self.crfiles,"shiftVector",shifts)
        
    #### calculates the fractional shift ####
    # retrieves an array of nSelectedAntennas x n SelectedFrequencies
    	
	# gets the fine phase calibration table for the standard frequencies
	rawCalTable=md.get("StationPhaseCalibration", self.RCUSelection, self.antennaSet, True)
	
	# gets the standard 512 frequencies of the normal data channels
	fullFreqs=range((self.nyquistZone-1)*self.sampleFrequency/2., (self.nyquistZone-1./512.)*self.sampleFrequency/2., self.sampleFrequency/(1024.))
    	
	# interpolates the raw cal table for the selected frequencies over all antennas
	self.calTable=iof.getInterpolatedCalTable(rawCalTable, fullFreqs, range(self.nAntennas), self.frequencies)
    	
	# calculates the total fractional delay excess in seconds
	totalFractionalDelays=ClockFractionalDelays.new()
	totalFractionalDelays.sub(ClockFractionalDelays, CableFractionalDelays)
	
	# checks if a fine correction existed, and if not, then uses the fractional delays to calculate a phase offset
	# CHECK TO SEE IF THE SIGN IF CORRECT HERE
	
	hfrequencies=cr.hArray(self.frequencies)
	for i in range(self.nAntennas):
	    if self.calTable[i,0]==1. and self.calTable[i,1]==1.:
	    	# then no solution was available, and we use the excess fractions
		self.calTable[i].copy(hfrequencies)
		# phase delay is 2 pi time*frequency
		self.calTable[i].mul(2.*cr.pi*totalFractionalDelays[i])
	
def getSpecWeights(cal):
    """
	
    Searches the data for 'bad' RFI channels and gets rid of them.
    Also generates a suggested matrix of weights for all channels, as
    would be used for generating an image.
    """
    if not cal.initialised:
    	cal.initialise()
    maxBlocks=1000
    if maxBlocks > cal.nBlocks:
        maxBlocks=cal.nBlocks
	print 'max blocks being reduced to ', maxBlocks
    
    # saves original info
    orig_block=cal.currentBlock
    orig_cal_method=cal.calMethod
    cal.calMethod=False
    # initialises array
    avgspec=cr.hArray(float,cal.getOPDim(),fill=0.)
    
    for i in range(maxBlocks):
        this_spec=cal.getFFTData()
        this_spec.square(this_spec)
        this_spec.abs(this_spec)
        avgspec.add(avgspec,this_spec)
    avgspec.div(float(maxBlocks))
    
    nFreqs=cal.nSelectedFrequencies
    nAntennas=cal.nAntennas
    
    npavg=avgspec.toNumpy()
    OK, vetoed,runsum, thresholds, npavg=get_veto_bands(npavg,nFreqs,nAntennas,(1.+5./maxBlocks**0.5), nRepeat=2)
#    print '\n\nthe various dimensions involved are: '
#    print 'runsum: ', runsum.shape
#    print 'thresholds: ', thresholds.shape
#    print 'vetoed: ', vetoed
    
    cal.currentBlock=orig_block
    cal.calMethod=orig_cal_method
    
    return (OK, vetoed,runsum, thresholds, npavg)



def get_veto_bands(npavg, nFreqs, nAntennas, relthresh, navg=None, nRepeat=1):
    """
    Calculates a running sum, and sets values over the threshold to
    zero
    
    ** npavg **
    Numpy average spectrum of two dimensions
    
    ** nFreqs**
    Size of second dimension
    """
    
    # calculates how many to average over
    if not navg:
    	navg=int(nFreqs**0.5)
    
    navgon2=navg/2
    
    
    runsum=np.copy(npavg)
    for i in range(nAntennas):
        runsum[i,0:navgon2]=sum(npavg[i,0:navg])
        for j in range(nFreqs-navg):
    	    runsum[i,navgon2+j]=runsum[i,navgon2+j-1]+npavg[i,navg+j]-npavg[i,j]
    	runsum[i,(nFreqs-navgon2):nFreqs]=sum(npavg[i,nFreqs-navg:nFreqs])
    runsum /= navg
    
    navgon2up=navg-navgon2
    finalveto=np.zeros(nFreqs)
    
    
    for i in range(nRepeat):
    # calculates approximately a 5-sigma threshold
    	vetoed=np.zeros(nFreqs)
    	veto=np.where(npavg > relthresh*runsum)[1]
    	vetoed[veto]=1
	finalveto[veto]=1
	thisveto=np.where(vetoed == 1)[0]
    	
    	# extracts these values from runsum
    	for index in thisveto:
	    mini=max(0,index-navgon2-1)
	    maxi=min(index+navgon2up-1,nFreqs-1)
	    for j in range(nAntennas):
	    	runsum[j,mini:maxi] -= npavg[j,index]/navg
    
    OK=np.where(finalveto==0)[0]
    finalveto=np.where(finalveto == 1)[0]
    thresholds=np.copy(runsum)
    thresholds *= relthresh
    return OK, finalveto,runsum, thresholds, npavg
    
    
    

# needs to be able to read [blah] blocks ahead in an internal buffer
# run RFI flagger per block or what?
# run it over the whole block???


## Executing a module should run doctests.
#  This examines the docstrings of a module and runs the examples
#  as a selftest for the module.
#  See http://docs.python.org/library/doctest.html for more information.
if __name__=='__main__':
    import doctest
    doctest.testmod()

  



### created by Clancy James: c.james@astro.ru.nl ###

"""Documentation for the module 'caldata'

This module contains the class CalibratedData, a wrapper class around
the IO module which allows automatic data calibration upon reading.


The 'to-do' list for this class is:

    -check that it works with multiple input files (currently the IO
    behaviour wrst multiple files is ill-defined). Rather, check how
    it works. Actually, get the IO module to not be bonkers.

    - let it check the size of the opened files and make sure it
    behaves sensibly when the file is ended.
    In general, the IO module shoudl have methods to ask it if
    it has bits of dat, rather than asking for it and it returning
    a crash.

    - perhaps allow it to operate with multiple blocks at once
    (or getting slicing working properly would help with this!)

    - give an option so that PPFs automatically skip through the
    first 15 blocks where no input is generated



"""

# import TBBData as TBD
import IO as TBD
import pycrtools as cr
import ppf
import os

reload(TBD)


class AntennaSet():
    """

    __init__(self, calData, selection=None)

    This class reads in uncallibrated data using the IO class and,
    using pre-specified settings, performs calibration and fft steps
    to return calibrated frequency-domain data.

    This 'class' is specifically inherited from a calData object,
    so that all calibration settings are the same as its parent.

    The purpose of this class is to return data from a particular
    antenna selection but pass this request through its parent, to
    avoid read-write conflicts.

    It has been specifically tailored for the imager and parallel
    processing.

    Initialisation methods require at minimum a calibratedData
    object; leaving the antenna selection undefined sets the antenna
    selection to 'all'.

    """
    def __init__(self, calData, selection, genFFTArray=True):
        """
        __init__(self, calData, selection):

        Initialises values based on calData.
        Ths initialisation happens in one step, since the specifications for
        arrays etc are all inhereted, and verification is done by its parent.

        The required inputs are a calData object, and an antenna selection.

        e.g. antenna selection must be a normal Python list, e.g. [1,2,3,4,5]

        The only optional input is genFFTArray (default: true), which if set
        to false, means no internal fft buffer will be generated, i.e. the user
        will have to pass the output fft buffer to the 'getData' method every
        time.

        """

        if selection == None:
            self.selection = range(0, 96, 1)
            self.nAntennas = 96
        elif selection == "polA":
            self.selection = range(0, 96, 2)
            self.nAntennas = 48
        elif selection == "polB":
            self.selection = range(1, 96, 2)
            self.nAntennas = 48
        else:
            self.selection = selection
            self.nAntennas = len(selection)

        self.genFFTArray = genFFTArray
        self.blockSize = calData.blockSize
        self.fftMethod = calData.fftMethod
        self.filterType = calData.filterType
        self.calMethod = calData.calMethod
        self.checkMethod = calData.checkMethod
        self.antennaSet = calData.antennaSet
        self.nFiles = calData.nFiles
        self.startBlock = calData.startBlock
        self.currentBlock = calData.currentBlock
        self.readBlocks = 0
        self.crfile = calData.crfile
        self.nyquistZone = calData.nyquistZone
        self.timeData = cr.hArray(float, [self.nAntennas, self.blockSize])
        if self.fftMethod == 'NONE':
            self.fftSize = self.blockSize  # time-domain data!
        else:
            self.fftSize = self.blockSize / 2 + 1
        if (self.genFFTArray):
            self.freqData = cr.hArray(complex, [self.nAntennas, self.fftSize])
        else:
            self.freqData = None
        self.frequencies = calData.frequencies

        if not self.check_OK():
            print 'Improper initialisation - somehow...'
            return 0

## initialises the filter arrays
        if self.filterType == 'PPF':
            self.ppf = ppf.PPF()
            self.filter = None
        elif self.filterType == 'HANNING':
            hfilter = cr.hArray(float, [self.blockSize])
            hfilter.gethanningfilter()
            self.filter = hfilter
            self.ppf = None
        elif self.filterType == 'NONE':
            self.filter = None
            self.ppf = None

        if self.calMethod:
            self.crfile.setSelection(self.selection)
            self.cal = crfile.getCalibrator()
        else:
            self.cal = None

    def getFrequencies(self):
        return self.frequencies

    def getReturnDim(self):
        return [self.nAntennas, self.fftSize]

    def getData(self, freqData=None):
        """
        A routine to return the 'next' block of interest.

        It simple reads in the next block as defined in the
        initialisation, performs whatever checks, filtering, and
        other manipulation has been requested, and returns the
        outcome.

        The only possible input is 'freqData', which if set, returns
        the data in the user-specified array.

        """
        # sets output frequency data array to the internal one if no input array is provided
        if freqData == None:
            if self.freqData == None:
                print 'No internal freqData array - please provide!'
                return None
            else:
                freqData = self.freqData

# sets the crfile object antenna selection and reads in the data
        self.crfile.setSelection(self.selection)  # HOW DOES THIS WORK!??!?!?!?!? ###
        self.crfile.readdata(self.timeData, self.currentBlock)

# updates the number of read blocks, and the next block to be read
        self.currentBlock += 1
        self.readBlocks += 1

# filters the data if applicable
        if self.filterType == 'PPF':
            self.ppf.add(self.timeData)
        elif self.filterType != 'NONE':
            self.timeData.mul(self.filter)

# performs a check on data quality if applicable
        if self.checkMethod:
            if timeData.max().val() >= 2048 or timeData.min().val() < -2048:
                print 'Impossible data values detected'

# performs the Fourier Transform if appllicable
        if self.fftMethod == 'CASA':
            # note here that the ... will loop over antennas in the antenna selection
            freqData[...].fftcasa(self.timeData[...], self.nyquistZone)
        elif self.fftMethod == 'FFTW':
            freqData[...].fftw(self.timeData[...])
        else:
            freqData = self.timeData

# calibrates the data in the frequency domain using internal vales
        if self.calMethod:
            self.cal.applyCalibration(fftData)

# returns the data
        return freqData

    def getAntennaPos(self):
        """
        Returns the antenna positions as given by the IO module.
        """
        self.crfile.setSelection(self.selection)
        return self.crfile["positions"]

    def check_OK(self):
        """
        check_OK(self):
            Checks to see if it is possible to initialise.
            Returns either True or False
            This requires the number of files, antennas, blocksize,
            and startblock to be greater than 0

        """
        OK = True
        if not (self.nFiles > 0 and self.nAntennas > 0 and
self.blockSize > 0 and self.startBlock >= 0):
            OK = False
        if self.fftMethod == None:
            if self.calMethod != 0:
                print 'calibration useless for time dat! Ignored'
                self.calMethod = 0

        return OK


##################################################################
##################################################################
##################################################################
##################################################################


class CalibratedData():

    """
    This class returns calibrated data from TBB data dumps.

    The user specifies a set of files, and what sort of manipulation
    is required, under the general categories of calibration,
    filtering, checking, and Fourier transforming.

    Defined routines are:

    __init__(self, fileList=None, startBlock=None,  selection=None,\
filterType='Hanning', blockSize=1024, calMethod=True, antennaSet=None,\
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
    """

    def __init__(self, fileList=None, blockSize=1024, selection=None, startBlock=None,
filterType='Hanning', calMethod=True, antennaSet=None,
fftMethod="fftcasa", checkMethod=None, ownArrays=True, genFFTArray=True):

        """
        __init__(self, fileList=None, startBlock=None,  selection=None,\
filterType='Hanning', blockSize=1024, calMethod=None, antennaSet=None,\
fftMethod="fftcasa", checkMethod=None)

        The only input required to get it to work is a data-file, e.g.
    GetFrequencyData("my_file"), which should be a string. This is
    the same as GetFrequencyData(DataFile="myfile")

        The returned item is the frequency-domain data. This is stored in
    one three-dimensional complex hArray object. op[i,j,k] returns
    the kth complex frequency values from the jth antenna in the ith
    data block.

    Inputs (and corresponding self variables) are:

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



        self.antennaSet = antennaSet
        self.nFiles = nFiles
        self.nAntennas = nAntennas
        self.fileList = fileList
        self.selection = selection
        self.startBlock = startBlock
        self.currentBlock = startBlock
        self.readBlocks = 0
        self.crfile = None
        self.nyquistZone = None
        self.timeData = None
        self.freqData = None
        self.cal = None
        self.ready = 0
        self.genFFTArray = genFFTArray
        self.fftSize=self.blockSize/2+1
        self.ownArrays=ownArrays

    """
##############################################################
##### We begin by making sure all inputs are sensible ########

### converts possible alternative inputs to real ones
        if (fftMethod == 'casa') or (fftMethod == 'fftcasa') or (fftMethod == 1):
            fftMethod = 'CASA'
        elif (fftMethod == 'fftw') or (fftMethod == 2):
            fftMethod = 'FFTW'
        elif (fftMethod == None) or (fftMethod == 'None')or (fftMethod == 'none'):
            fftMethod = 'NONE'
        else:
            fftMethod = 'CASA'

        # converts possible filterType specifications into something sensible
        if filterType == 'None' or filterType == None or filterType == 'none':
            filterType = 'NONE'
        elif filterType == 'Hanning' or filterType == 'hanning':
            filterType = 'HANNING'
        elif filterType == 'PPF' or filterType == 'ppf':
            filterType = 'PPF'
        else:
            print 'Unspecified filter type - defaulting to Hanning'
            filterType = 'HANNING'

        if blockSize != 1024 and filterType == 'PPF':
            print 'PPF unspecified for a blocksize of ', blockSize
            filterType = 'HANNING'

    # defines the antenna selection
        if selection == None:
            if genFFTArray:
                nAntennas = 96
                selection = range(0, 96, 1)
            else:
                nAntennas = 0
        elif selection == "polA":
            selection = range(0, 96, 2)
            nAntennas = 48
        elif selection == "polB":
            selection = range(1, 96, 2)
            nAntennas = 48
        else:
            nAntennas = len(selection)

    # checcking antennaset inputs
        if antennaSet == None:
            print 'we still need to get an antenna set automatically\
 - arbitrarily setting this to LBA Outer'
            antennaSet = "LBA_Outer"

    # checking file-list inputs
        if fileList != None:
            nFiles = len(fileList)
        else:
            nFiles = 0

        if startBlock == None:
            startBlock = 0
        elif startBlock < 0:
            print 'bad startblock specification'

# sets own values. Some are then altered later, but this is an exhaustive list
        self.ppf = None
        self.blockSize = blockSize
        self.fftMethod = fftMethod
        self.filterType = filterType
        self.calMethod = calMethod
        self.checkMethod = checkMethod
        self.antennaSet = antennaSet
        self.nFiles = nFiles
        self.nAntennas = nAntennas
        self.fileList = fileList
        self.selection = selection
        self.startBlock = startBlock
        self.currentBlock = startBlock
        self.readBlocks = 0
        self.crfile = None
        self.nyquistZone = None
        self.timeData = None
        self.freqData = None
        self.cal = None
        self.ready = 0
        self.genFFTArray = genFFTArray
        self.fftSize = self.blockSize / 2 + 1
        self.ownArrays = ownArrays

        if ownArrays == True:
            if self.check_OK():
                print 'initialising own arrays...'
                self.initOwnArrays()
            else:
                print 'Initialisation not complete - specify proper',\
' values, then init_IO()'
        else:
            print 'No antennas selected - will not return data'

        ####### initialises IO of the data files
        self.crfile = TBD.open(self.fileList, self.blockSize, self.selection)  # self.selection
        self.crfile.setAntennaset(self.antennaSet)
        self.nyquistZone = self.crfile["nyquistZone"]

        self.frequencies = cr.hArray(float, [self.fftSize])

        if fftMethod == 'CASA':
            self.frequencies.fillrange(1e8 + 1e8 / self.fftSize, 1e8 / self.fftSize)
        elif fftMethod == 'FFTW':
            self.frequencies.fillrange(2e8 - 1e8 / self.fftSize, -1e8 / self.fftSize)
        else:
            self.frequencies = None

         # Apply calibration shift for clockdelays
        if self.calMethod == True:
            print 'applying the calibration shift'
            self.cal = self.crfile.getCalibrator()
    #       self.crfile.applyCalibrationShift()

    def getFrequencies(self):
        return self.frequencies

    def check_OK(self):
        """
        check_OK(self):
            Checks to see if it is possible to initialise.
            Returns either True or False

        """
        OK = True
        if self.nFiles > 0 and self.nAntennas > 0 and\
self.blockSize > 0 and self.startBlock >= 0:
            for fname in self.fileList:
                if not (os.path.isfile(fname)):
                    OK = False
                    print 'File ', fname, ' can not be found'
        else:
            OK = False
            print 'please check that nfiles, nantennas, and blocksize are', ' positive integers, and startblock is >= 0'
        return OK

##### BEGIN INITIALISATION OF IO AND ARRAYS ######
    def initOwnArrays(self):
## assigns own values from inouts directly
        """

        init_IO(self):
            Initialises arrays, transforms, filters etc, based on
        previous user inputs. It also checks that the files actually
        exist. It also performs a check, and returns 'True' if OK,
        'False' otherwise.

        """

        if not self.check_OK():
            print 'Improper initialisation'
            return 0

## initialises the filter arrays
        if self.filterType == 'PPF':
            import PPF as PPF
            self.ppf = ppf.PPF()
        elif self.filterType == 'HANNING':
            hfilter = cr.hArray(float, [self.blockSize])
            hfilter.gethanningfilter()
            self.filter = hfilter
        elif self.filterType == 'NONE':
            self.filter = None

#       self.timeData=self.crfile["emptyFx"]
#       self.freqData=self.crfile["emptyFFT"]
        self.timeData = cr.hArray(float, [self.nAntennas, self.blockSize])
        if self.genFFTArray:
            self.freqData = cr.hArray(complex, [self.nAntennas, self.fftSize])
        else:
            self.freqData = None

        self.ready = 1

### routine to get next lot of data
    def getData(self, freqData=None):

        """
        A routine to return the 'next' block of interest.

        It simple reads in the next block as defined in the
        initialisation, performs whatever checks, filtering, and
        other manipulation has been requested, and returns the
        outcome.

        The only possible input is 'freqData', which if set, returns
        the data in the user-specified array.

        """
        if not self.ready:
            print 'not set up to return data!'
            return None
        self.crfile.setSelection(self.selection)
        if not self.ready:
            print 'Not ready to return data!'
            return None
        print 'self.currentBlock is ', self.currentBlock
        self.crfile.readdata(self.timeData, self.currentBlock)

        self.currentBlock += 1

        if self.filterType == 'PPF':
            self.ppf.add(self.timeData)
        elif self.filterType != 'NONE':
            self.timeData.mul(self.filter)

        if freqData == None:
            freqData = self.freqData

        if self.checkMethod:
            if timeData.max().val() >= 2048 or timeData.min().val() < -2048:
                print 'Impossible data values detected'

        if self.fftMethod == 'CASA':
            # note here that the ... will loop over antennas in the antenna selection
            print 'dimensionality is: ', freqData.getDim(), self.timeData.getDim()
            freqData[...].fftcasa(self.timeData[...], self.nyquistZone)
        elif self.fftMethod == 'FFTW':
            freqData[...].fftw(self.timeData[...], self.nyquistZone)

        if self.calMethod:
            self.cal.applyCalibration(fftData)

        self.readBlocks += 1

        return freqData

    def reset(self, startBlock=None):
        """
        resets the current block to the initial start value, and
        clears and resets the PPF if applicable
        """

        if startBlock == None:
            startBlock = self.startBlock

        self.readBlocks = 0
        self.startBlock = startBlock
        if filterType == 'PPF':
            self.ppf.buffer.fill(0.)
            self.ppf.startrow = 0
            self.ppf.total_rows_added = 0

    def getAntennaPos(self, selection=None):
        if selection == None:
            if self.selection == None:
                print 'No selection set'
                return None
            else:
                self.crfile.setSelection(self.selection)
                return self.crfile["RelativeAntennaPositions"]
        else:
            self.crfile.setSelection(selection)
            return self.crfile["RelativeAntennaPositions"]

    def newAntennaSet(self, selection):
        return AntennaSet(self, selection)

    def getAntennaPos(self):
        self.crfile.setSelection(self.selection)
        return self.crfile["RelativeAntennaPositions"]

    def getReturnDim(self):
        return [self.nAntennas, self.fftSize]

#           self.timedata=self.crfile["emptyfx"]
#           self.crfile.setAntennaSet(AntennaSet) ### shouldn't this be implicit in the file anyway???
#           if self.fftmethod != NONE:
#               self.freqdata=self.crfile.["emptyFFt"]
#           else:
#               self.freqdata=None


## Executing a module should run doctests.
#  This examines the docstrings of a module and runs the examples
#  as a selftest for the module.
#  See http://docs.python.org/library/doctest.html for more information.
if __name__ == '__main__':
    import doctest
    doctest.testmod()

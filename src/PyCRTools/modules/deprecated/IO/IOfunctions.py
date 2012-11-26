
import numpy as np
import pycrtools as cr

################################################################################################
################################################################################################
#
#      A SERIES OF GENERAL 'GET' FUNCTIONS
#       (author: S. terVeen, s.terveen@astro.ru.nl)
#
################################################################################################
################################################################################################


def open(filenames, blocksize=1024, selection=None):
    """Open TBB hdf5 files. Returns a TBBdata object.

    *filenames* List of names from the files to open
    *blocksize* NR samples per data block (import for FFT resolution
    *selection* Which antennas to select. 3 options:
                - list of Antenna IDs
                   (first 3 digits stationID
                    next 3 digits RSPid (=RCUid/8)
                    next 3 digits RCUid)
                - list of antenna numbers in file
    """
    return TBBdata(filenames, blocksize, selection)


def openfiles(filenames, blocksize=1024):
    """This function opens files and aligns them by setting the shift argument correctly.

    *filenames* list of the files to open

    """

    files = []
    for filename in filenames:
        files.append(cr.crfile(filename))

    times = get(files, "TIME", False)
    print 'these are the time from OLD: ', times
    tmin = times.min()
    tmax = times.max()
    # time stamp cannot be handled by integer function
    assert tmax - tmin < 22

    shifts = []
    for i in range(len(files)):
        files[i]["blocksize"] = blocksize
        shifts.append(files[i]["SAMPLE_NUMBER"])
        shifts[i].muladd(files[i]["TIME"] - tmin, cr.Vector(int, files[i]["nofSelectedAntennas"], int(files[i]["sampleFrequency"])))

# search for maximum in shifts
    shiftmax = 0
    for i in range(0, len(files)):
        localmax = shifts[i].max()
        if localmax > shiftmax:
            shiftmax = localmax

    for i in range(0, len(files)):
        shifts2 = []
        for j in range(0, len(shifts[i])):
            shifts2.append(shiftmax - shifts[i][j])
        files[i]["ShiftVector"] = shifts2

    return files


def get(files, keyword, return_as_list=True):
    """ Get data or metadata for the specified keyword. If not to be returned as list
    it tries to return is as 1 object.

    If the value is the same for all files, this will be returned.
    If the value is an IntVec, FloatVec or an hArray where the first dimensions is
    the same as the number of selected antennas, an equal type object will be returned
    with the value for all the antennas.
    If neither of these can be achieved it will return a list with values per file.

    *files* List of crfiles
    *keyword* Variable to return
    *return_as_list* Return a list with a value per file.
    """
    if return_as_list:
        ret = []
        for i in range(len(files)):
            ret.append(files[i][keyword])
    else:
        ret = files[0][keyword]
        if isinstance(ret, (cr.IntVec, cr.FloatVec)) and len(ret) == files[0]["nofSelectedAntennas"]:
            for i in range(1, len(files)):
                ret.extend(files[i][keyword])
        elif isinstance(ret, (cr.IntArray, cr.FloatArray, cr.ComplexArray)) and ret.getDim()[0] == files[0]["nofSelectedAntennas"]:
            NrAnts = get(files, "nofSelectedAntennas")
            TotNrAnts = sum(NrAnts)
            shape = ret.getDim()
            shape[0] = TotNrAnts
            ret.resize(reduce(lambda x, y: x * y, shape))
            ret.setDim(shape)
            StartAnt = 0
            EndAnt = 0
            for i in range(0, len(files)):
                EndAnt += NrAnts[i]
                ret[StartAnt:EndAnt] = files[i][keyword]
                StartAnt = EndAnt
        else:
            retall = get(files, keyword, True)
            if isinstance(retall[0], (cr.IntVec, cr.FloatVec)):
                for i in range(0, len(files) - 1):
                    if ((retall[i] - retall[i + 1]).stddev() > 0.0):
                        return retall
            else:
                for i in range(0, len(files) - 1):
                    if retall[i] != retall[i + 1]:
                        return retall

            if "nof" in keyword and "Channels" not in keyword:
                ret = sum(retall)
                if "nofBaselines" is keyword:
                    ret += len(retall) - 1
            else:
                ret = retall[0]

    return ret


def set(files, keyword, value):
    """ Set the keyword with the value for all the files. Currently two options are
    supported:
        - One value for all files, e.g. files.set("block",1024)
        - A list with values per file.

    Support for a hArray of Vector with values for all antennas should be added.

    """
    if isinstance(value, list):
        for num, file in enumerate(files):
            file.set(keyword, value[num])
    elif isinstance(value, int):
        for file in files:
            file.set(keyword, value)

    return True


def readFx(files, fxdata, block=-1):
    """read a block of raw timeseries data for all (selected) antennas.

    *fxdata* Array in which to return the data
    *block*  Block for which data should be read

    """
    if block == -1:
        block = get(files, "block", True)
        for num, bl in enumerate(block):
            block[num] = bl + 1

    set(files, "block", block)

    selAnts = get(files, "nofSelectedAntennas")
    antBeg = 0
    antEnd = 0
    for num, nrA in enumerate(selAnts):
        antBeg = antEnd
        antEnd += nrA
        # fxdata.setSlice([antBeg*dim[1],antEnd*dim[1]])
        fxdata[antBeg:antEnd].read(files[num], "Fx")

    # fxdata.setSlice(0,antEnd*dim[1])
    return True


def applySelection(selection, array, rArray=None):
    """ Select from the data only the applied selection

    *selection* Number of antennas for which to return the data
                (NOT antennaIDs, the can be converted by antIDsToSelection)
    *array*     Array on which to apply the selection
    *rArray*    Array to return the selected data in.


    """
    # Still needs a dimenstion check, so may need files after all
    if not rArray:
        returnArray = True
    else:
        returnArray = False
    if not selection:
        if returnArray:
            return array
        else:
            rArray = array
            return True
    if isinstance(array, (cr.FloatVec, cr.IntVec)):
        array = cr.hArray(array)
    if isinstance(array, (cr.FloatArray, cr.IntArray, cr.ComplexArray, cr.BoolArray)):
        dim = array.getDim()
        if len(dim) >= 2:
            dim[0] = len(selection)
            if not rArray:
                rArray = cr.hArray(copy=array, dimensions=dim, fill=0)
            rArray[range(dim[0]), ...].copy(array[selection, ...])
        else:
            dim = len(selection)
            if not rArray:
                rArray = cr.hArray(copy=array, dimensions=dim, fill=0)
            rArray[...].copy(array[selection][...])

        if returnArray:
            return rArray
        else:
            return True
    else:
        if returnArray:
            return array
        else:
            rArray = array
            return False


def antIDsToSelection(files, SelAntIDs):
    """Gives the numbers of the selected antennas, according to the antenna IDs

    *files*   A list of cr files
    *SelAntIDs* IDs of the antenna selection (1e6*stationID+1e3*RSPid+RCUid )
                where RSPid = RCUid / 8

    returns: List with antenna selection.


    """

    allantIDs = get(files, "antennaIDs", False)
    SelAntIDs.sort()
    selection = []
    for (num, ant) in enumerate(allantIDs.val()):
        if ant in SelAntIDs:
            selection.append(num)

    return selection


def frange(start, end=None, inc=None):
    "A range function, that does accept float increments..."

    if end == None:
        end = start + 0.0
        start = 0.0

    if inc == None:
        inc = 1.0

    L = []
    while 1:
        next = start + len(L) * inc
        if inc > 0 and next >= end:
            break
        elif inc < 0 and next <= end:
            break
        L.append(next)

    return L


def getSpecWeights(cal):
    """

    Searches the data for 'bad' RFI channels and gets rid of them.
    Also generates a suggested matrix of weights for all channels, as
    would be used for generating an image.
    """
    maxBlocks = 10
    if maxBlocks > cal.nBlocks:
        maxBlocks = cal.nBlocks

    # saves original info
    orig_block = cal.currentBlock
    orig_cal_method = cal.calMethod
    cal.calMethod = False

    ws = cr.CRMainWorkSpace(filename=cal.fileList, doplot=False, verbose=False, modulename="ws")

    ws["blocksize"] = cal.blockSize
    ws["max_nblocks"] = 3
    ws["ncoeffs"] = 12
    ws["numin"] = min(cal.frequencies) / 1e6  # MHz
    ws["numax"] = max(cal.frequencies) / 1e6  # MHz

    # determines how many blocks to average over
    avgspec = cr.hArray(float, cal.getOPDim(), fill=0.)

    makeAverageSpectrum(cal, maxBlocks, avgspec)

    # initialises arrays
    fitcoeffs = cr.hArray(float, [cal.nAntennas, ws["ncoeffs"]])
    meanrms = cr.hArray(float, [cal.nAntennas])

    # frequency values of returned frequencies
    frequency = cal.frequencies
    ws["frequency"] = frequency
    meanrms[...] = fitcoeffs[...].crfitbaseline(ws["frequency"], avgspec[...], ws)


def makeAverageSpectrum(cal, maxblocks, avgspec):
    """
    makes an average spectrum as returned by the crfile

    *cal*
    calData object

    *avgspec*
    hArray of correct size for whatever is returned by the crfile

    *maxblocks*
    integer the number of blocks to average over
    """
     # initialises array

    this_spec = cr.hArray(complex, avgspec.getDim())

    # makes an average spectrum
    for i in range(maxBlocks):
        this_spec = cal.getFFTData()
        this_spec.square(this_spec)
        this_spec.abs(this_spec)
        avgspec.add(avgspec, this_spec)

    avgspec.div(float(maxBlocks))


def getInterpolatedCalTable(phaseTable, fullFreqs, selectedAntennas, selectedFrequencies):
    """
    Interpolates the SattionCalibration table for the desired frequencies

    phaseTable must be a hArray of dimensions [all_antennas x
    all_frequencies], as returned by the metadata call
    md.get("StationPhaseCalibration"") for the selected RCUs.

    fullFreqs is also a hArray of values of those frequencies in the full call table

    selectedAntennas are the indices of the full antenna range (normal vector)

    selectedFrequencies are the floating values of the selected frequencies (normal vector)
    """

    # calculates the magnitude and phase from the full call table

    # initialises an array to hold the new cal table for the returned data
    nWantedFreqs = len(selectedFrequencies)
    nWantedAnts = len(selectedAntennas)
#    wantedCalTable=cr.hArray(complex,[nWantedAnts,nWantedFreqs])
    # uses numpy to perform a linear interpolation
    # gets dimensions of cal table
    nGivenFreqs = phaseTable.getDim()
    nGivenAnts = nGivenFreqs[0]
    nGivenFreqs = nGivenFreqs[1]
    pt = phaseTable.toNumpy()
    npwantedtable = np.zeros((nWantedAnts, nWantedFreqs))
    for i in range(nWantedAnts):
        wys = NPgetLinInterpolationCoefs(fullFreqs, pt[i], np.array(selectedFrequencies))
        npwantedtable[i] = wys
    wantedCalTable = cr.hArray(npwantedtable)
    return wantedCalTable


def NPgetLinInterpolationCoefs(xs, npys, npwxs):
    """
    Does a numpy interpolation - gets a vector of given xs and ys,
    and takes a vector of 'wanted xs' for which the corresponding
    y-values must be returned. The inerpolation is linear.
    """
    dx = xs[1] - xs[0]
    nvals = len(xs)
    i1s = np.array((npwxs - xs[0]) / dx, dtype=int)
    a = np.where(i1s < 0)
    i1s[a[:]] = 0
    a = np.where(i1s > nvals - 2)
    i1s[a[:]] = nvals - 2
    i2s = i1s + 1
    npxs = np.array(xs)
    coefs2 = (npxs[i1s[:]] - npwxs) / dx
    coefs1 = 1. - coefs2
    wys = coefs1 * npys[i1s[:]] + coefs2 * npys[i2s[:]]
    return wys


def HgetLinInterpolationCoefs(values, vvector):
    """
    returns a vector of coefficients of the form
    [[index_1, weight_1], [index_2, weight_2]]

    It assumes that the values in vvector are ordered linearly
    with equal spacing, though they can be either increasing or
    decreasing
    """

    nvals = len(vvector)
    dval = vvector[1] - vvector[0]
    # note that this will round down:

    if1 = cr.hArray(float, [values.getSize()])
    if1.copy(values)
    if1.sub(vvector[0])
    if1.div(dval)
    i1 = cr.hArray(int, [if1.getSize()])
    i2 = cr.hArray(int, [if1.getSize()])
    i1.copy(if1)
    i2.copy(i1)
    i2.add(1)

    indicies = cr.hArray(int, i1.getDim())
    cr.hFindLessThan(indicies, i1, 0)
    i1[indicies].fill(0)
    i2[indicies].fill(1)

    cr.hFindGreaterThan(indicies, i1, nvals - 2)
    i1[indicies].fill(nvals - 2)
    i2[indicies].fill(nvals - 1)

    coefs1 = cr.hArray(float, i1.getDim())
    coefs2 = cr.hArray(float, i2.getDim())

    coefs1.copy(values)
    hvvector = cr.hArray(vvector)

    hvvector[i2[...]]
    coefs1.sub(hvvector[i2])
    coefs1.div(dval)

    coefs2.fill(1.)
    coefs2.sub(coefs1)
    return i1, i2, coefs1, coefs2


def getLinInterpolationCoefs(value, vvector):
    """
    returns a vector of coefficients of the form
    [[index_1, weight_1], [index_2, weight_2]]

    It assumes that the values in vvector are ordered linearly
    with equal spacing, though they can be either increasing or
    decreasing
    """

    nvals = len(vvector)
    dval = vvector[1] - vvector[0]
    # note that this will round down:
    i1 = int((value - vvector[0]) / dval)

    if i1 < 0:
        i1 = 0
        i2 = 1
    elif i1 >= nvals - 1:
        i1 = nvals - 2
        i2 = nvals - 1
    else:
        i2 = i1 + 1

    # coef 1 and coef 2 better add up to 1!
    coef1 = (value - vvector[i2]) / dval
    coef2 = (value - vvector[i1]) / dval

    return [[i1, coef1], [i2, coef2]]


def getLinInterpolationCoefsUnequal(value, vvector):
    """
    returns a vector of coefficients of the form
    [[index_1, weight_1], [index_2, weight_2]]

    DOES NOT WORK
    """

    nvals = len(vvector)
    if vvector[1] > vvector[0]:
        di = 1
        starti = 0
    else:
        di = -1
        starti = nvals - 1

    if value < vvector[starti]:
        # value is out of range by being too small
        i1 = starti
        i2 = starti + di
    elif value > vector[starti + (nvals - 1) * di]:
        # value is out of range by being too large
        i2 = starti + (nvals - 1) * di
        i1 = starti + (nvals - 2) * di
    else:
        # value in range!
        for i in range(vvector):
            index = starti + i * di
            if vvector[index] > value:
                i2 = index
                i1 = index - di
                break


## Executing a module should run doctests.
#  This examines the docstrings of a module and runs the examples
#  as a selftest for the module.
#  See http://docs.python.org/library/doctest.html for more information.
if __name__ == '__main__':
    import doctest
    doctest.testmod()

"""
  Module datacheck.py. Part of the LOFAR Cosmic Ray pipeline.
  Opens a CR event data file and performs elementary checks to ensure stability further down the pipeline.
  Provides methods for checking of data quality, flagging of antennas.

.. moduleauthor:: Arthur Corstanje <A.Corstanje@astro.ru.nl>
"""

import os
import time
from pycrtools import CRQualityCheck, Vector, hArray
import pycrtools as cr

import matplotlib.pyplot as plt


def safeOpenFile(filename, antennaset):  # antennaset only here because it's not set in the file
    """ Opens a file and performs some safety / consistency checks

    Required arguments:

    =========    ====================
    Parameter    Description
    =========    ====================
    *filename*   Name of the file to open
    *antennaset* antenna set descriptor, e.g. LBA_OUTER
    =========    ====================

    Returns:

    dictionary with keys:
    *success*                   Boolean
    [if not success] *reason*   String
    [else] *file*               opened file object

    """
    result = dict(success=False, name=filename)
    # test system exists / filesize first
    if not os.path.exists(filename.strip()):
        print 'file not found'
        result.update(reason="File doesn't exist")
        return result
    bytesize = os.path.getsize(filename.strip())
    if bytesize < 15 * 1048576:
        print 'File size too small: %d' % bytesize
        result.update(reason=format("File size too small: %d") % bytesize)
        return result

    crfile = cr.open(filename.strip(), 2 * 65536)
#    print 'KEYS: '
#    keys = crfile.keys()
#    keys.sort()
#    print keys
#    print ' '
    crfile["ANTENNA_SET"] = antennaset
    times = crfile["TIME"]

    fileDate = times[0]
    readableDate = time.strftime("%a, %d %b %Y %H:%M:%S", time.gmtime(fileDate))
    result.update(date=readableDate)
    # print result

  #  print crfile["shift"]
    if max(times) - min(times) > 0:
        print 'Error: Timestamps vary across antennas'  # redundant
        result.update(reason=format("Timestamps don't match, spread = %d seconds") % (max(times) - min(times)))
        return result

    sampleFrequency = crfile["SAMPLE_FREQUENCY_VALUE"][0]
    if crfile["SAMPLE_FREQUENCY_UNIT"][0] == "MHz":
        sampleFrequency *= 1.0e6
    if sampleFrequency != 200e6:
        print 'Error: wrong sampling frequency: %d; needs to be 200e6' % sampleFrequency
        result.update(reason=format("wrong sampling frequency %d") % sampleFrequency)
        return result

    dataLength = crfile["DATA_LENGTH"][0]
    if dataLength < 2 * 65536:
        result.update(reason=format("Data length too small: %d") % dataLength)
        return result
    result.update(datalength=dataLength)
    nofAntennas = crfile["NOF_DIPOLE_DATASETS"]
    if nofAntennas < 64:  # arbitrary choice...
        result.update(reason=format("not enough antennas: %d") % nofAntennas)
        return result
    result.update(nofAntennas=nofAntennas)

    result.update(success=True, file=crfile)
    return result
#    return result.update(success=True, file=crfile) !!! This actually returns None (nonetype)...


def qualityCheck(crfile, cr_efield, doPlot=False):
    if doPlot:
        efield = cr_efield.toNumpy()
        import numpy as np
        import matplotlib
        matplotlib.rc('xtick', labelsize=18)
        matplotlib.rc('ytick', labelsize=18)
        x = np.arange(efield[0].size, dtype=float)
        x *= 5.0  # ms
        x -= 328600
        # plt.tick_params(fontsize=18)
        plt.plot(x, efield.T)
#        plt.plot(efield[0].T[64700:65500])
#        a = np.ones(800) * 30.0
#        plt.plot(a, color='r', linewidth = 6)
#        plt.text(600, 38, 'threshold', size='x-large')
#        plt.title('Timeseries for one channel')
        plt.title('Timeseries for all channels', fontsize=21)
        plt.xlabel('Time (ns)', fontsize=21)
        plt.ylabel('Voltage (ADC units)', fontsize=21)
        raw_input("--- Plotted raw timeseries data - press Enter to continue...")
    qualitycriteria = {"mean": (-1.5, 1.5), "rms": (3, 15), "spikyness": (-5, 5)}

#    datalength = crfile["Filesize"]
#    blocksize = 1024
#    blocklist = hArray(int, [0.5 * datalength / blocksize])
    flaglist = CRQualityCheck(qualitycriteria, crfile, dataarray=None, maxblocksize=2 * 65536, nsigma=5, verbose=True)  # NOTE: this changes the crfile's blocksize! Change it back afterwards.
    crfile["BLOCKSIZE"] = 2 * 65536  # parameter...
    # make warnings for DC offsets and spikyness; flag out for rms too high (i.e. junk data)
    flagged = []
    highDCOffsets = 0
    spikyChannels = 0
    # import pdb; pdb.set_trace()
    for entry in flaglist:
        if entry[3][0] == 'mean':
            highDCOffsets += 1
        elif entry[3][0] == 'spikyness':
            spikyChannels += 1
        elif entry[3][0] == 'rms':
            flagged.append(entry[0])  # entry[0] = antenna nr.

    # Now count the pulses; taken from the pycrtools-tutorial
    nsigma = 5  # parameter
    datamean = cr_efield[...].mean()
    thresholdHigh = cr_efield[...].stddev(datamean)
    thresholdHigh *= nsigma
    thresholdLow = thresholdHigh * (-1.0)
    thresholdLow += datamean
    thresholdHigh += datamean
    maxgap = Vector(int, len(datamean), fill=20)  # parameter...
    minlength = Vector(int, len(datamean), fill=1)  # idem

    nofAntennas = crfile["NOF_DIPOLE_DATASETS"]
    pulses = hArray(int, dimensions=[nofAntennas, 100, 2], name='Location of the pulses')
    npulses = pulses[...].findsequenceoutside(cr_efield[...], thresholdLow, thresholdHigh, maxgap, minlength)

    # now we can check for spurious pulse locations and/or just for the count
    avgPulseCount = npulses.sum() / float(nofAntennas)
    maxPulseCount = npulses.max()
    if doPlot and len(flagged) > 0:
        cr_efield = crfile["EMPTY_TIMESERIES_DATA"]
        crfile.getTimeseriesData(cr_efield, 0)  # crfile["Fx"] crashes on invalid block number ???? While it was set to a valid value...
        efield = cr_efield.toNumpy()
        toplot = efield[flagged]
        plt.plot(toplot.T)
        raw_input("--- Plotted antennas flagged as BAD - press Enter to continue...")

    result = dict(success=True, action='Data quality check', flagged=flagged,
                  warnings=format("Too high DC offsets: %d; too high spikyness: %d") % (highDCOffsets, spikyChannels), avgPulseCount=avgPulseCount, maxPulseCount=maxPulseCount)
    return result

# Execute doctests if module is executed as script
if __name__ == "__main__":
    import doctest
    doctest.testmod()

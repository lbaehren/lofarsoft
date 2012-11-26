"""
This module supplies different function to mach datafiles from different
stations, datafiles and trigger messages, etc. together using their
timestamps.

.. moduleauthor:: Andreas Horneffer <Andreas Horneffer <A.Horneffer@astro.ru.nl>
"""

import os
import pycrtools as cr
import numpy as np
import time
import metadata as md


def matccrLes(dirs, sortstring, coinctime, mincoinc):
    """
    Match the datafiles from different stations together. It will print out
    the groups of filenames of that were taken at (about) the same time.

    Required arguments:

    ============ ======================================================
    Parameter    Description
    ============ ======================================================
    *dirs*       List of directories (patterns) for the files from the
                 different stations
    *sortstring* Strin that - when executed on a shell- sorths the list
                 of files by time
    *coinctime*  Coincidence-time in seconds
    *mincoinc*   Minimum number of files to give one conicidence
    ============ ======================================================

    Example::

        >>> matccrLes(['/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/trigger-dumps-2010-07-07-cs003/*1', '/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/trigger-dumps-2010-07-07-cs005/*1', '/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/trigger-dumps-2010-07-07-cs006/*1'], 'sort -rn --field-separator="-" --key=18', 1.5e-6, 3)
        0 : number of files: 9
        1 : number of files: 12
        2 : number of files: 12

    (All the other stuff is not python output)
    """
    files = dict()
    times = dict()

    for dirIndex in range(len(dirs)):
        fd = os.popen('ls ' + dirs[dirIndex] + ' | ' + sortstring)
        files[dirIndex] = fd.readlines()
        print dirIndex, ": number of files:", len(files[dirIndex])
        fd.close()
        times[dirIndex] = list()
        for file in files[dirIndex]:
            dr = cr.crfile(file.strip())
            ddate = dr["date"] + dr["SAMPLE_NUMBER"][0] / 200.0e6
            times[dirIndex].append(ddate)
            del dr

    indices = np.zeros((len(dirs)), int)
    running = True

    while running:
        mintime = times[0][(indices[0])]
        for dirIndex in range(1, len(dirs)):
            if (times[dirIndex][(indices[dirIndex])] <= mintime):
                mintime = times[dirIndex][(indices[dirIndex])]
        nocoinc = 0
        coincdirs = list()
        for dirIndex in range(len(dirs)):
            if (times[dirIndex][(indices[dirIndex])] <= (mintime + coinctime)):
                nocoinc += 1
                coincdirs.append(dirIndex)
        if (nocoinc >= mincoinc):
            print "Found coincident event:"
            for dirIndex in coincdirs:
                print files[dirIndex][(indices[dirIndex])]
        for dirIndex in coincdirs:
            indices[dirIndex] += 1
            if indices[dirIndex] >= len(files[dirIndex]):
                running = False


def readtriggers(crfile, directory=''):
    """
    Read in a TBBDriver-dumpfile with the data from the trigger-messages.
    Returns a tuple of 1-d numpy arrays: (antennaIDs, dDates, dates, samplenumers)

    Required arguments:

    ============ ======================================================
    Parameter    Description
    ============ ======================================================
    *filename*   Name of the TBBDriver-dumpfile to be read in
    ============ ======================================================

    Example::

        >>> (antennaIDs, dDates, dates, samplenumers) = readtriggers('/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/2010-07-07-triggers/2010-07-07_TRIGGER-cs005.dat')
        readtriggers: File: /mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/2010-07-07-triggers/2010-07-07_TRIGGER-cs005.dat has: 539405 lines

    """
    timestamp = crfile["TIME"][0]  # we know from datacheck that all are the same
    stationID = int(crfile["CHANNEL_ID"][0]) / int(1e6)  # assume all from the same station!
    # print stationID
    stationName = md.idToStationName(stationID)
    # print stationName
    datestring = time.strftime("%Y-%m-%d", time.gmtime(timestamp))  # like "2011-02-15"
#    import pdb; pdb.set_trace()
    h5filename = crfile["FILENAME"]
    directory = os.path.dirname(h5filename) + '/'
    filename = directory + datestring + "_TRIGGER-" + stationName + ".dat"  # like "2011-02-15_TRIGGER-RS307.dat"

#    fd = os.popen('wc '+ filename)
#    str_line = fd.readline()
#    fd.close()
#    nevents = int(str_line.split()[0])
#    print "readtriggers: File:" , filename ,"has:", nevents, "lines"

    antennaIDs = np.zeros(1000, int)
    dDates = np.zeros(1000)
    dates = np.zeros(1000, int)
    samplenumers = np.zeros(1000, int)
    missed = np.zeros(1000, int)

    print timestamp
    if os.path.isfile(filename):
        fd = open(filename)
    else:
        print 'No trigger file to be read: %s' % filename
        return []

    i = 0
    line = fd.readline()
    while not str(timestamp) in line:  # read over it until hitting 'timestamp' (second)
        line = fd.readline()

    while str(timestamp) in line:  # now process until passing beyond 'timestamp'
#        print line
        antennaIDs[i] = int(line.split()[0])
        testdate = long(line.split()[2])
        # Have to check for invalid dates! These are 2^32 - 1, and somehow Python complains about not getting that into its 'int' type... [ 32 bits system ]
        if testdate < 2.2e9:
            dates[i] = int(testdate)
        else:
            dates[i] = 0  # not easy to skip over it... it'll fall out when matching dates.

        testSampleNum = long(line.split()[3])  # and sometimes the sample number goes invalid as well. Glitches...
        if testSampleNum < 200e6:
            samplenumers[i] = int(testSampleNum)
        else:
            samplenumers[i] = 0
            print 'WARNING: sample number invalid in trigger log!'
        thisMissedCount = long(line.split()[9])  # And also here, 32-bits unsigned debug values can enter
        if thisMissedCount < 65536:
            missed[i] = int(thisMissedCount)
        else:
            missed[i] = -1
        dDates[i] = float(dates[i]) + float(samplenumers[i]) / 200.0e6
        i += 1  # !
        line = fd.readline()

    fd.close()
    antennaIDs = np.resize(antennaIDs, i)
    dDates = np.resize(dDates, i)
    dates = np.resize(dates, i)
    samplenumers = np.resize(samplenumers, i)
    missed = np.resize(missed, i)

    return (antennaIDs, dDates, dates, samplenumers, missed)


def getTriggerIndicesFromTime(ddate, dDates, coinctime=1e-6):
    """
    Find the indices of the triggers which arrived within  "coinctime" of
    "ddate".

    Required arguments:

    ============ ======================================================
    Parameter    Description
    ============ ======================================================
    *ddate*      Timestamp around which to find the triggers
    *dDates*     Numpy array with the timestamps of the triggers
    *coinctime*  Maximum time-difference to search the triggers in
    ============ ======================================================

    """
    return np.where(np.logical_and(dDates > (ddate - coinctime), dDates < (ddate + coinctime)))[0]


def matchTriggerfileToTime(ddate, triggerfile, coinctime=1e-5):
    """
    Find the triggers from a triggerfile which arrived within  "coinctime" of
    "ddate". Returns a tuple of 1-d numpy arrays: (antennaIDs, dDates, difftimes, dates, samplenumers)

    Required arguments:

    ============= ======================================================
    Parameter     Description
    ============= ======================================================
    *ddate*       Timestamp around which to find the triggers
    *triggerfile* Either the name of the TBBDriver-dumpfile to be read
                  in or a tuple with the data from a triggerfile
                  (as read in by "readtriggers()")
    *coinctime*   Maximum time-difference to search the triggers in
    ============= ======================================================

    Example::

        matchTriggerfileToTime((1278508300.6189337+0.00033024),'/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/2010-07-07-triggers/2010-07-07_TRIGGER-cs005.dat')

        >>> (mIDs, mdDates, mTdiffs, mscratch, mscratch) = matchTriggerfileToTime((1278508300.6189337+0.00033024),'/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/2010-07-07-triggers/2010-07-07_TRIGGER-cs005.dat',coinctime=1e-5)
        readtriggers: File: /mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/2010-07-07-triggers/2010-07-07_TRIGGER-cs005.dat has: 539405 lines

    """
    if isinstance(triggerfile, str):
        raise ValueError("Need as input the 'triggers' as obtained from matching.readtriggers(...)")
        #(allIDs, allDDates, alldates, allSNs) = readtriggers(triggerfile)
    else:
        (allIDs, allDDates, alldates, allSNs, allMissed) = triggerfile
    indices = getTriggerIndicesFromTime(ddate, allDDates, coinctime)
    if (len(indices) > 0):
        selectedIDs = allIDs[indices]
        selectedDDates = allDDates[indices]
        selecteddates = alldates[indices]
        selectedSNs = allSNs[indices]
        selectedMissed = allMissed[indices]

        sortind = np.argsort(selectedIDs)
        sortIDs = selectedIDs[sortind]
        sortDDates = selectedDDates[sortind]
        sortdates = selecteddates[sortind]
        sortSNs = selectedSNs[sortind]
        sortMissed = selectedMissed[sortind]
        if (len(np.unique(sortIDs)) != len(sortIDs)):
            print "matchTriggerfileToTime: duplicated antenna IDs!"
        outIDs = sortIDs
        outDDates = sortDDates
        outdates = sortdates
        outSNs = sortSNs
        outMissed = sortMissed
    else:
        outIDs = np.zeros((0), int)
        outDDates = np.zeros((0))
        outdates = np.zeros((0), int)
        outSNs = np.zeros((0), int)
        outMissed = np.zeros((0), int)
    difftimes = (outdates - ddate) + outSNs / 200.0e6

    return (outIDs, outDDates, difftimes, outdates, outSNs, outMissed)

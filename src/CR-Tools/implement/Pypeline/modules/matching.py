"""
This module supplies different function to mach datafiles from different 
stations, datafiles and trigger messages, etc. together using their
timestamps.

.. moduleauthor:: Andreas Horneffer <Andreas Horneffer <A.Horneffer@astro.ru.nl>
"""

import os
import pycrtools as cr
import numpy as np

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
    files=dict()
    times=dict()
    
    for dirIndex in range(len(dirs)):
        fd = os.popen('ls '+ dirs[dirIndex]+' | ' + sortstring)
        files[dirIndex] = fd.readlines()
        print dirIndex, ": number of files:", len(files[dirIndex])
        fd.close()
        times[dirIndex] = list()
        for file in files[dirIndex]:
            dr = cr.crfile( file.strip() )
            ddate = dr["date"] + dr["SAMPLE_NUMBER"][0]/200.0e6
            times[dirIndex].append(ddate) 
            del dr

    indices = np.zeros( (len(dirs)), int)
    running = True

    while running:
      mintime = times[0][(indices[0])]
      for dirIndex in range(1,len(dirs)):
        if (times[dirIndex][(indices[dirIndex])] <= mintime):
          mintime = times[dirIndex][(indices[dirIndex])]
      nocoinc = 0;
      coincdirs = list()
      for dirIndex in range(len(dirs)):
        if (times[dirIndex][(indices[dirIndex])] <= (mintime+coinctime) ):
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

def readtriggers(filename):
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
    fd = os.popen('wc '+ filename)
    str_line = fd.readline()
    fd.close()
    nevents = int(str_line.split()[0])
    print "readtriggers: File:" , filename ,"has:", nevents, "lines"

    antennaIDs = np.zeros( (nevents), int)
    dDates = np.zeros( (nevents) )
    dates = np.zeros( (nevents), int)
    samplenumers = np.zeros( (nevents), int)
    fd = open(filename, 'r')
    for i in range(nevents):
      str_line = fd.readline()
      antennaIDs[i] = int(str_line.split()[0])
      testdate = long(str_line.split()[2])
      # Have to check for invalid dates! These are 2^32 - 1, and somehow Python complains about not getting that into its 'int' type...
      if testdate < 2.2e9:
          dates[i] = int(testdate)
      else:
          dates[i] = 0 # not easy to skip over it... it'll fall out when matching dates.

      samplenumers[i] = int(str_line.split()[3])
      dDates[i] = float(dates[i]) + float(samplenumers[i])/200.0e6
    fd.close()

    return (antennaIDs, dDates, dates, samplenumers)


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
    return np.where(np.logical_and(dDates>(ddate-coinctime),dDates<(ddate+coinctime)))[0]

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
    if isinstance(triggerfile,str):
        (allIDs, allDDates, alldates, allSNs) = readtriggers(triggerfile)
    else:
        (allIDs, allDDates, alldates, allSNs) = triggerfile
    indices = getTriggerIndicesFromTime(ddate, allDDates, coinctime)
    if (len(indices)>0):
      selectedIDs = allIDs[indices]
      selectedDDates = allDDates[indices]
      selecteddates = alldates[indices]
      selectedSNs = allSNs[indices]
      sortind = np.argsort(selectedIDs)
      sortIDs = selectedIDs[sortind]
      sortDDates = selectedDDates[sortind]
      sortdates = selecteddates[sortind]
      sortSNs = selectedSNs[sortind]
      if (len(np.unique(sortIDs)) != len(sortIDs)):
        print "matchTriggerfileToTime: dublicated antenna IDs!"
      outIDs = sortIDs
      outDDates = sortDDates
      outdates = sortdates
      outSNs = sortSNs
    else:
      outIDs = np.zeros((0), int)
      outDDates = np.zeros((0))
      outdates = np.zeros((0), int)
      outSNs = np.zeros((0), int)
    difftimes = (outdates-ddate)+outSNs/200.0e6

    return (outIDs, outDDates, difftimes, outdates, outSNs) 


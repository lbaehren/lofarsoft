#! /usr/bin/env python

import os

subnet       = '/net/sub5'
parts        = ['data1', 'data2', 'data3', 'data4']
#nodes        = ['lse015']
nodes        = ['lse013', 'lse014', 'lse015']
pulsArch     = 'PULSAR_TESTARCH'
pulsArchPath = os.path.join(subnet, nodes[2], parts[3])
logLocation  = '/globalhome/lofarsystem/log'
logFileName  = 'run.Storage.R00.log'


class DISTError(IOError):
    """Exception for data distribution trouble"""
    def __init__(self):return


class RSPS():
    """
    Initialize some parameters that will be passed or otherwise determined.
    using a nested list for partition listings. This list is ordered and separable.

    a returned RSPS object will be list of ordered subband lists, where

    RSPS[0] = ['/path/SB0.MS.incoherentstokes',
    '/path/SB1.MS.incoherentstokes',
    ...,
    '/path/SB61.MS.incoherentstokes'
    ]

    RSPS[1] = ['/path/SB62.MS.incoherentstokes',
    '/path/SB63.MS.incoherentstokes',
    ...
    '/path/SB123.MS.incoherentstokes'
    ]
    etc.

    If the caller switches readlog argument to False, the searchNodes() function assumes nominal
    data delivery of 248 subbands, 0-247.  These lists are meant to contain (usually)
    about 62 subband files, a number chosen for presto post-processing, which imposes
    an arbitrary limit of 1024 directory files.  16 channels/subband on 62 subbands
    delivers 992 files to the output directory, within presto limits.

    N.B. There are hazards within: this assumes bands low to high are deposited and ordered on the
    cluster, such that first subbands --> lse013, 2nd subbands --> lse014, 3rd subbands --> lse15.
    This has been the nominally data delivery pattern, but could easily change and this function
    should be made robust under changing delivery conditions.  It is not robust now.
    """

    def __init__(self,obsid,filefactor,readlog=False):
        self.obsid      = obsid
        self.readlog    = readlog
        self.filefactor = int(filefactor)


    def RSPLists(self):
        """
        Return a list of subband file lists rather than a list of strings.
        """

        if self.readlog:
            try:
                rspList = self.__readLog()
            except DISTError:
                print "INFO| Caught DISTError exception on RPS build."
                print "INFO| Switching to node-based data hunt."
                rspList = self.__searchNodes()
                if not filenum(rspList):
                    print "WARN| Dataset appears to be incomplete."
                    print "WARN| Data for",self.obsid,"likely remain scattered.\n"
            except IOError:
                print "log file for",self.obsid,"not found\n"
                rspList = self.__searchNodes()
    
        else:
            print "searching nodes ..."
            rspList = self.__searchNodes()
        return rspList


    def RSPStrings(self):
        """
        Return a list of strings rather than a list of subband file lists (see RSPLists).
        """
        rspStrings = []
        rspLists   = RSPLists()
        for sbList in rspLists:
            rspStrings.append(' '.join(sbList))
        return rspStrings


    ###------------------- "private" class methods -----------------###


    def __searchNodes(self):
        rspList = []
        infiles = []
        orderedSBFiles = []
        for i in nodes:
            for j in parts:
                searchPath = os.path.join(subnet, i, j, self.obsid)
                try:
                    msFiles     = os.listdir(searchPath)
                except OSError:
                    continue

                pulsarFiles = self.__stripMS(msFiles)
                for (index, pulsarfile,) in enumerate(pulsarFiles):
                    infiles.append(os.path.join(searchPath, pulsarfile))

        for i in range(len(infiles)):
            orderedSBFiles.append( infiles[i])

        (sbChunk, mod,) = divmod(len(orderedSBFiles), self.filefactor)
        sbStart = 0

        for i in range(self.filefactor):
            rspList.append(orderedSBFiles[sbStart:(sbStart + sbChunk)])
            sbStart += sbChunk

        if mod:
            for i in reversed(range(mod)):
                rspList[-1].append(orderedSBFiles[-(i + 1)])
        return rspList


    def __stripMS(self,list):
        pulsarFiles = []
        for file in list:
            if "coherentstokes" in file:
                pulsarFiles.append(file)
            else: continue
        return pulsarFiles

            



    def __readLog(self):
        """
        Read the SAS/MAC Storage log file (run.Storage.R00.log) for passed obsid.
        The log details write locations for all subband data for a given obsid.
        These logs live in /globalhome/lofarsystem/log/<obsid>
            
        Returns same list kind as searchNodes().
        """

        dataWrites   =[]
        sortedWrites =[]
        rspList      =[]

        logFilePath = os.path.join(logLocation,self.obsid,logFileName)
        fileOb      = open(logFilePath)
        storageLog  = fileOb.readlines();fileOb.close();del fileOb

        try:
            self.__checkLog(storageLog)
        except DISTError:
            raise DISTError

        for entry in storageLog:
            if "written to /" and "coherentstokes" in entry:
                dataWrites.append(entry.split("@")[-1])
        # dataWrite is a complete, unordered set of data writing records for <obsid>.
        for i in range(len(dataWrites)):
            splitline   = dataWrites[i].split() 
            splitline[1]= int(splitline[1])
            sortedWrites.append(splitline)
        sortedWrites.sort()
        for i in range(len(sortedWrites)):
            node = sortedWrites[i][0].split("|")[0]
            tail = sortedWrites[i][-1].lstrip("/")
            rspList.append(os.path.join(subnet,node,tail))
        newRSPList = self.__splitList(rspList)
        return newRSPList


    def __splitList(self,fileList):
        """ Split a single list by self.filefactor.  Return a list of <filefactor> lists.

        i.e. w/ filefactor =3, from

        [file1,file2,file3,file4,file5,file6,file7,file8,file9]

        to

        [[file1,file2,file3],[file4,file5,file6],[file7,file8,file9]]

        """
        newRSPList = []
        filePoint  = 0
        files_per_rsp, mod = divmod(len(fileList),self.filefactor)
        for i in range(self.filefactor):
            newRSPList.append(fileList[filePoint:(filePoint+files_per_rsp)])
            filePoint = filePoint + files_per_rsp
        if mod:
            modlist = range(mod)
            modlist.reverse()
            for i in modlist:
                newRSPList[-1].append(fileList[-1*(i+1)])
        return newRSPList


    def __checkLog(self,log):
        for line in log:
            if "written to" not in line: continue
            else:
                device = line.split("|")[1].split("@")[-1]
                if device not in nodes:
                    raise DISTError
        return


    def __filenum(self,rspList):
        rlen = 0
        sig  = True
        for i in range(len(rspList)):
            for j in range(len(rspList[i])):
                rlen += 1
        if rlen < 248:
            sig = False
        return sig


#__________________________________________ ____________________________________________
#
#            Function-based interface that mimicks the RSPLists class (above)
#__________________________________________ ____________________________________________


def RSPLists(obsid,filefactor,readlog=True):
    """
    Return a list of subband file lists rather than a list of strings.

    Initialize some parameters that will be passed or otherwise determined.
    using a nested list for partition listings. This list is ordered and separable.

    a returned RSPS object will be list of ordered subband lists, where

    RSPS[0] = ['/path/SB0.MS.incoherentstokes',
    '/path/SB1.MS.incoherentstokes',
    ...,
    '/path/SB61.MS.incoherentstokes'
    ]

    RSPS[1] = ['/path/SB62.MS.incoherentstokes',
    '/path/SB63.MS.incoherentstokes',
    ...
    '/path/SB123.MS.incoherentstokes'
    ]
    etc.

    If the caller switches readlog argument to False, the __searchNodes() function assumes nominal
    data delivery of 248 subbands, 0-247.  These lists are meant to contain (usually)
    about 62 subband files, a number chosen for presto post-processing, which imposes
    an arbitrary limit of 1024 directory files.  16 channels/subband on 62 subbands
    delivers 992 files to the output directory, within presto limits.

    N.B. There are hazards within: this assumes bands low to high are deposited and ordered on the
    cluster, such that first subbands --> lse013, 2nd subbands --> lse014, 3rd subbands --> lse15.
    This has been the nominally data delivery pattern, but could easily change and this function
    should be made robust under changing delivery conditions.  It is not robust now.
    """

    if readlog:
        try:
            rspList = readLog(obsid,filefactor)
        except DISTError:
            print "INFO| Caught DISTError exception on RPS build."
            print "INFO| Switching to node-based data hunt."
            rspList = searchNodes(obsid,filefactor)
            if not filenum(rspList):
                print "WARN| Dataset appears to be incomplete."
                print "WARN| Data for",obsid,"likely remain scattered.\n"
        except IOError:
            print "log file for",obsid,"not found\n"
            rspList = searchNodes(obsid,filefactor)
            
    else:
        rspList = searchNodes(obsid,filefactor)
    return rspList


def RSPStrings(obsid, filefactor):
    """
    Return a list of strings rather than a list of subband file lists (see RSPLists).
    """
    rspStrings = []
    rspLists   = RSPLists(obsid, filefactor)
    for sbList in rspLists:
        rspStrings.append(' '.join(sbList))
    return rspStrings


def searchNodes(obsid,filefactor):
    rspList = []
    infiles = []
    orderedSBFiles = []
    for i in nodes:
        for j in parts:
            searchPath = os.path.join(subnet, i, j, obsid)
            try:
                msFiles = os.listdir(searchPath)
            except OSError:
                continue
            for (index, msfile,) in enumerate(msFiles):
                newitem = os.path.join(searchPath, msfile)
                msFiles[index] = newitem
            infiles.append(msFiles)

    for i in range(len(infiles)):
        orderedSBFiles += infiles[i]
    (sbChunk, mod,) = divmod(len(orderedSBFiles), int(filefactor))
    sbStart = 0

    for i in range(filefactor):
        rspList.append(orderedSBFiles[sbStart:(sbStart + sbChunk)])
        sbStart += sbChunk

    if mod:
        for i in reversed(range(mod)):
            rspList[-1].append(orderedSBFiles[-(i + 1)])
    return rspList


def readLog(obsid,filefactor):
    """Read the SAS/MAC Storage log file (run.Storage.R00.log) for passed obsid.
    The log details write locations for all subband data for a given obsid.
    These logs live in /globalhome/lofarsystem/log/<obsid>

    Returns same list kind as searchNodes().
    """

    dataWrites   =[]
    sortedWrites =[]
    rspList      =[]

    logFilePath = os.path.join(logLocation,obsid,logFileName)
    fob = open(logFilePath)
    print "reading file...\n"
    storageLog = fob.readlines();fob.close();del fob

    try:
        __checkLog(storageLog)
    except DISTError:
        raise DISTError

    for entry in storageLog:
        if "written to /" in entry:
            dataWrites.append(entry.split("@")[-1])
    # dataWrite is an complete, unordered set of data writing records for <obsid>.
    for i in range(len(dataWrites)):
        splitline   = dataWrites[i].split() 
        splitline[1]= int(splitline[1])
        sortedWrites.append(splitline)
    sortedWrites.sort()
    for i in range(len(sortedWrites)):
        node = sortedWrites[i][0].split("|")[0]
        tail = sortedWrites[i][-1].lstrip("/")
        rspList.append(os.path.join(subnet,node,tail))
    newRSPList = __splitList(rspList,filefactor)
    return newRSPList


def __splitList(fileList,filefactor):
    """ Split a single list by filefactor.  Return a list of <filefactor> lists.

    i.e. w/ filefactor =3, from

    [file1,file2,file3,file4,file5,file6,file7,file8,file9]

    to

    [[file1,file2,file3],[file4,file5,file6],[file7,file8,file9]]

    """
    newRSPList = []
    filePoint  = 0
    files_per_rsp, mod = divmod(len(fileList),int(filefactor))
    for i in range(filefactor):
        newRSPList.append(fileList[filePoint:(filePoint+files_per_rsp)])
        filePoint = filePoint + files_per_rsp
    if mod:
        modlist = range(mod)
        modlist.reverse()
        for i in modlist:
            newRSPList[-1].append(fileList[-1*(i+1)])
    return newRSPList


def __checkLog(log):
    for line in log:
        if "written to" not in line: continue
        else:
            device = line.split("|")[1].split("@")[-1]
            if device not in nodes:
                raise DISTError
    return


def filenum(rspList):
    rlen = 0
    sig  = True
    for i in range(len(rspList)):
        for j in range(len(rspList[i])):
            rlen += 1
    if rlen < 248:
        sig = False
    return sig

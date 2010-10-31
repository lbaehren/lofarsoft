#! /usr/bin/env python

#                                                          LOFAR PULSAR PIPELINE
#
#                                             Pulsar.pipeline.support.fullRSP.py
#                                                          Ken Anderson, 2009-10
#                                                            k.r.anderson@uva.nl
# ------------------------------------------------------------------------------

import os
import pulpEnv


class DISTError(IOError):
    """Exception for data distribution trouble"""
    def __init__(self):return


class FullRSP():

    """
    Build an RSP directory containing all .subnnnn files produced by bf2presto
    during an RSP split operation (nominal).  This involves making the "RSPA",
    populating it as a linked in set of all files across all RSPS.

    It will be incumbent upon the prepfold recipes to add this full file directory
    to the prepfold job queue.
    """

    def __init__(self, obsid, pulsar, filefactor, arch, env):

        obsEnv          = pulpEnv.PulpEnv(obsid,pulsar,arch,env)
        
        self.obsid      = obsEnv.obsid
        self.pulsar     = obsEnv.pulsar
        self.filefactor = filefactor
        self.pArchive   = obsEnv.pArchive
        self.obsidPath  = os.path.join(self.pArchive,self.obsid)
        self.stokes     = obsEnv.stokes

    def fillAllSubFiles(self):

        """
        method returns upon successful build of OBSID's RSPA directory.
        PULSAR_ARCIVE/<OBSID>/RSPA will not exist until now.
        """

        # RSPA directory will be built parallel to other RSP dirs
        # i.e. under OBSID/[somekind of stokes]/RSPA
        
        rspAdir = os.path.join(self.obsidPath,self.stokes,"RSPA")

        if not os.path.isdir(rspAdir):
            os.mkdir(rspAdir)
            os.chdir(rspAdir)

        subfileIndex = 0

        for i in range(self.filefactor):
            rspN   = "RSP"+str(i)
            srcPath= os.path.join(self.obsidPath,self.stokes,rspN)
            subFiles = os.listdir(srcPath)
            for subFile in subFiles:
                if ".list" in subFile:
                    continue
                linkTarget = os.path.join("..",rspN,subFile)
                newRoot    = os.path.splitext(subFile)[0][:-1] + "A"
                linkName   = newRoot+".sub%04d" % subfileIndex
                #print linkName, linkTarget
                os.symlink(linkTarget, linkName)
                subfileIndex += 1
        return

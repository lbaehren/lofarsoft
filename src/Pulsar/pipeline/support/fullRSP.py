#
# $Id: fullRSP.py 6881 2011-01-28 14:01:26Z kenneth $

#                                                          LOFAR PULSAR PIPELINE
#
#                                             Pulsar.pipeline.support.fullRSP.py
#                                                          Ken Anderson, 2010-10
#                                                            k.r.anderson@uva.nl
# ------------------------------------------------------------------------------

import os
import pulpEnv


# Repository info ...
__svn_revision__ = ('$Rev: 6881 $').split()[1]
__svn_revdate__  = ('$Date: 2011-01-28 15:01:26 +0100 (Fri, 28 Jan 2011) $')[7:26]
__svn_author__   = ('$Author: kenneth $').split()[1]


class DISTError(IOError):
    """Exception for data distribution trouble"""
    def __init__(self):return


class FullRSP():

    """
    Build an RSP directory containing all .subnnnn files produced by bf2presto
    during an RSP split operation (nominal).  This involves populating "RSPA",
    as a linked in set of all files across all RSPS.

    It will be incumbent upon the prepfold recipes to add this full file directory
    to the prepfold job queue.
    """

    def __init__(self, obsid, pulsar, filefactor, arch, env):

        obsEnv          = pulpEnv.PulpEnv(obsid,pulsar,arch,env)
        
        self.obsid      = obsEnv.obsid
        self.pulsar     = obsEnv.pulsar
        self.filefactor = filefactor
        self.pArchive   = obsEnv.pArchive
        self.stokes     = obsEnv.stokes
        self.obsidPath  = obsEnv.obsidPath
        self.stokesPath = obsEnv.stokesPath
        self.rspAdir    = os.path.join(self.stokesPath,"RSPA")


    def fillAllSubFiles(self):

        """
        method returns upon successful build of OBSID's RSPA directory.
        PULSAR_ARCIVE/<OBSID>/RSPA should have been build by 
        buildRSPS.buildRspDirs() method call in the master.buildRSPAll
        recipe.
        """

        # RSPA directory will be built parallel to other RSP dirs
        # i.e. under OBSID/[somekind of stokes]/RSPA
        

        if not os.path.isdir(self.rspAdir):
            os.mkdir(self.rspAdir)
            os.chdir(self.rspAdir)
        else: 
            os.chdir(self.rspAdir)

        subfileIndex = 0

        for i in range(self.filefactor):
            rspN   = "RSP"+str(i)
            srcPath= os.path.join(self.stokesPath,rspN)
            subFiles = os.listdir(srcPath)
            for subFile in subFiles:
                if ".list" in subFile:
                    continue
                linkTarget = os.path.join("..",rspN,subFile)
                newRoot    = os.path.splitext(subFile)[0][:-1] + "A"
                linkName   = newRoot+".sub%04d" % subfileIndex
                os.symlink(linkTarget, linkName)
                subfileIndex += 1
        return

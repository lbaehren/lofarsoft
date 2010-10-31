#!/usr/bin/python

#                                                          LOFAR PULSAR PIPELINE
#
#                                           Pulsar.pipeline.support.buildRSPS.py
#                                                          Ken Anderson, 2009-10
#                                                            k.r.anderson@uva.nl
# ------------------------------------------------------------------------------

import os, shutil
import RSPlist
import pulpEnv

class buildRSPS():

    """
    Build <filefactor> RSP directories.

    """

    def __init__(self, obsid, pulsar, filefactor, arch, uEnv):

        obsEnv = pulpEnv.PulpEnv(obsid, pulsar, arch, uEnv)
        
        self.arch      = arch
        self.uEnv      = uEnv
        self.obsid     = obsEnv.obsid
        self.pulsar    = obsEnv.pulsar
        self.stokes    = obsEnv.stokes
        self.pArchive  = obsEnv.archPaths[arch]
        self.obsidPath = os.path.join(self.pArchive,self.obsid)
        self.stokesPath= os.path.join(self.obsidPath,self.stokes)
        self.filefactor= filefactor


    def makeLists(self):

        """ build, populate <filefactor> number of RSP directories,
        """

        self.rspDirs  = []

        rspGroups     = RSPlist.RSPS(self.obsid,
                                     self.pulsar,
                                     self.filefactor,
                                     self.arch,
                                     self.uEnv
                                     )

        rspGroupLists = rspGroups.RSPLists()

        if os.path.isdir(self.pArchive): 
            pass
        else: 
            os.mkdir(self.pArchive)

        if   os.path.isdir(self.obsidPath):
             shutil.rmtree(self.obsidPath)
             os.makedirs(self.stokesPath)
        else:os.makedirs(self.stokesPath)

        for i in range(self.filefactor):
            rspDir =  os.path.join(self.stokesPath,"RSP"+str(i))
            if os.path.isdir(rspDir):
                self.rspDirs.append(rspDir)
            else:
                os.mkdir(rspDir)
                self.rspDirs.append(rspDir)

        return

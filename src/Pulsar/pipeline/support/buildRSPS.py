#!/usr/bin/python

#           $Id$

#                                                          LOFAR PULSAR PIPELINE
#
#                                           Pulsar.pipeline.support.buildRSPS.py
#                                                          Ken Anderson, 2010-10
#                                                            k.r.anderson@uva.nl
# ------------------------------------------------------------------------------

import os, shutil

# Pulp libs
import RSPlist
import pulpEnv

# Repository info ...
__svn_revision__ = ('$Rev$').split()[1]
__svn_revdate__  = ('$Date$')[7:26]
__svn_author__   = ('$Author$').split()[1]


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
        self.pArchive  = obsEnv.pArchive
        self.obsidPath = obsEnv.obsidPath
        self.stokesPath= obsEnv.stokesPath
        self.filefactor= filefactor



    def buildRspDirs(self):

        """ build, populate <filefactor> number of RSP directories,
        """

        self.rspDirs = []

        if os.path.isdir(self.pArchive): 
            pass
        else: 
            os.mkdir(self.pArchive)

        if os.path.isdir(self.obsidPath):
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

        # write the RSP data lists for <obsid>

        rspLists = self.__writeRspLists()
 
        return rspLists



    def __writeRspLists(self):

        """
        Write RSP lists for each RSP, as well as a full list in <obsid>
        directory.
        """

        rspGroups   = RSPlist.RSPS(self.obsid,
                                   self.pulsar,
                                   self.filefactor,
                                   self.arch,
                                   self.uEnv
                                   )
        rspLists    = rspGroups.RSPLists()
        rspDirs     = self.__getRspDirs()

        for i in range(len(rspLists)):
            dir         = rspDirs[i]
            rspListName = os.path.join(self.stokesPath,dir,self.obsid + "_"+dir + ".list")
            fullRSPFile = os.path.join(self.obsidPath,self.obsid+"_Master_RSP.list")
            rspFile     = open(rspListName,"w")
            RSPFile     = open(fullRSPFile,"a")

            for line in rspLists[i]:
                rspFile.write(line+"\n")
                RSPFile.write(line+"\n")

            rspFile.close()
            RSPFile.close()

        if self.filefactor > 1:
            rspAPath     = os.path.join(self.stokesPath,"RSPA")
            if not os.path.isdir(rspAPath):
                os.mkdir(rspAPath)
            rspaListName = os.path.basename(fullRSPFile)
            rspaFullName = os.path.join(rspAPath,rspaListName)
            shutil.copyfile(fullRSPFile,rspaFullName)
        else: pass

        del rspFile, RSPFile
        return
            

        return rspGroupLists


    def __getRspDirs(self):
        return os.listdir(self.stokesPath)

#
# $Id$

#                                                          LOFAR PULSAR PIPELINE
#
#                                        Pulsar.pipeline.support.prepInfFiles.py
#                                                          Ken Anderson, 2010-10
#                                                            k.r.anderson@uva.nl
# ------------------------------------------------------------------------------

import os
from   shutil import copyfile

# PULP libs
import foldingData
import RSPlist
import pulpEnv

# Repository info ...
__svn_revision__ = ('$Rev$').split()[1]
__svn_revdate__  = ('$Date$')[7:26]
__svn_author__   = ('$Author$').split()[1]


class PrepInfFiles():

    """
    Build the presto ".inf" files for each RSP directory.

    """

    def __init__(self, obsid, pulsar, filefactor, arch, env):

        obsEnv = pulpEnv.PulpEnv(obsid,pulsar,arch,env)

        self.obsid     = obsEnv.obsid
        self.pulsar    = obsEnv.pulsar
        self.xPulsars  = obsEnv.xPulsars
        self.stokes    = obsEnv.stokes
        self.pArchive  = obsEnv.pArchive
        self.defaultInf= obsEnv.defaultInf
        self.obsidPath = obsEnv.obsidPath
        self.stokesPath= obsEnv.stokesPath
        self.filefactor= filefactor
        self.parsetPath= obsEnv.parsetPath
        self.parset    = obsEnv.parsetName

        rspListObj     = RSPlist.RSPS(self.obsid,
                                      self.pulsar,
                                      self.filefactor,
                                      arch,
                                      env)

        self.rspLists  = rspListObj.RSPLists()


    def buildListsAndLinks(self):
        
        #self.writeRSPLists()  # method pushed to buildPulsArch recipe.
        self.prepRSPS()


    def writeRSPLists(self):
        """
        Write RSP lists for each RSP, as well as a full list in <obsid>
        directory.
        """

        self.rspDirs = self.__getRspDirs()
        fullRSPList  = []

        for i in range(len(self.rspLists)):
            dir         = self.rspDirs[i]
            rspListName = os.path.join(self.stokesPath,dir,self.obsid + "_"+dir + ".list")
            fullRSPFile = os.path.join(self.obsidPath,self.obsid+"_Master_RSP.list")
            rspFile     = open(rspListName,"w")
            RSPFile     = open(fullRSPFile,"a")
            for line in self.rspLists[i]:
                rspFile.write(line+"\n")
                RSPFile.write(line+"\n")
            rspFile.close()
            RSPFile.close()

        if self.filefactor > 1:
            rspAPath     = os.path.join(self.stokesPath,"RSPA")
            rspaListName = os.path.basename(fullRSPFile)
            rspaFullName = os.path.join(rspAPath,rspaListName)
            copyfile(fullRSPFile,rspaFullName)
        else: pass

        del rspFile, RSPFile
        return


    def prepRSPS(self):

        """
        self.rspLists will be used by the FoldingData class to facilitate calculation
        of several frequency parameters, namely

        Central freq of low channel (MHz)
        Total bandwidth (MHz)
        Number of channels
        Channel bandwidth (MHz)

        The FoldingData class will handle these calculations for the .inf file write.

        """

        # _____________________________________________ #
        # get default parset file into obsid directory  #

        finalParFile = os.path.join(self.obsidPath,self.obsid+".parset")
        print "copy ",self.parset,"to obsid directory"
        copyfile(self.parset,os.path.join(self.obsidPath,finalParFile))

        # _____________________________________________ #
        #        .inf files to the RSP directories      #

        prepInf = os.path.expanduser(self.defaultInf)
        outfile = os.path.basename(prepInf)
        copyfile(prepInf,os.path.join(self.obsidPath,outfile))

        infData = foldingData.FoldingData(prepInf, self.pulsar, self.rspLists)
        infData.readInf()
        infData.calcInfPars(self.parset, self.obsid, self.stokesPath)
        
        if self.xPulsars:
            for xpulsar in self.xPulsars:
                infData = foldingData.FoldingData(prepInf, xpulsar, self.rspLists)
                infData.readInf()
                infData.calcInfPars(self.parset, self.obsid, self.stokesPath)

        return


    # ________________________ private methods ___________________#


    def __getRspDirs(self):
        return os.listdir(self.stokesPath)
        

    def __findParsetFile(self):
        """
        Hunt down the parset for a given obsid.
        Host site is

        /globalhome/lofarsystem/log/

        which is 

        Invocation of this method necessarily implies that "nominalparsetFile"
        did not exist, and this obsid has a "new form" parset file.

        """

        parsetFile   = None
        parsetLogDir = None

        searchPath = self.parsetPath
        for root, dir, files in os.walk(searchPath,topdown=False):
            for file in files:
                if file == "RTCP-"+self.obsid.split("_")[1]+".parset":
                    parsetLogDir = root
                    parsetFile   = file
                    break
                continue
            if parsetFile and parsetLogDir:
                break
            else: continue
        return (parsetLogDir, parsetFile)


    def __findParsetFile(self):
        """
        Hunt down the parset for a given obsid.
        Host site is

        /globalhome/lofarsystem/log/

        which is 

        Invocation of this method necessarily implies that "nominalparsetFile"
        did not exist, and this obsid has a "new form" parset file.

        """

        parsetFile   = None
        parsetLogDir = None

        searchPath = self.parsetPath
        for root, dir, files in os.walk(searchPath,topdown=False):
            for file in files:
                if file == "RTCP-"+self.obsid.split("_")[1]+".parset":
                    parsetLogDir = root
                    parsetFile   = file
                    break
                continue
            if parsetFile and parsetLogDir:
                break
            else: continue
        return (parsetLogDir, parsetFile)



    def __buildNewInfName(self, n):

        parname = self.pulsar + "_" + self.obsid + "_" + "RSP" + str(n) + ".sub.inf"

        return parname
        

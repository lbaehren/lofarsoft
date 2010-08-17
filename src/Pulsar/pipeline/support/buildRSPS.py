#!/usr/bin/python

import os
import shutil
import infodata
import RSPlist

subnet       = '/net/sub5'
parts        = ['data1', 'data2', 'data3', 'data4']
nodes        = ['lse013', 'lse014', 'lse015']
#nodes        = ['lse015']
pulsArch     = 'PULSAR_ARCHIVE'
pulsArchPath = os.path.join(subnet, nodes[2], parts[3])
stokes       = "incoherentstokes"
lofarSoft    = "/home/kanderson/LOFAR/lofarsoft"
defaultInf   = os.path.join(lofarSoft, "release/share/pulsar/data/lofar_default.inf")
defParsets   = "/globalhome/lofarsystem/log"
defParFile   = "RTCP.parset.0"


class buildRSPS():
    """
    Build <filefactor> RSP directories,populate with default.inf files,
    make the rsp .inf files, one in each directory.  This is required by 
    prepfold.

    """

    def __init__(self, obsid, pulsar, filefactor=8, stokes=stokes):
        self.obsid     = obsid
        self.pulsar    = pulsar
        self.stokes    = stokes
        self.pArchive  = os.path.join(pulsArchPath,pulsArch)
        self.obsidPath = os.path.join(self.pArchive,self.obsid)
        self.stokesPath= os.path.join(self.obsidPath,stokes)
        self.filefactor= filefactor
        self.parset    = self.testParset()


    def buildRSPS(self):

        """ build and populate <filefactor> number of RSP directories,
        one .inf file is each.
        """

        self.rspDirs  = []
        rspGroups     = RSPlist.RSPS(self.obsid,self.filefactor)
        rspGroupLists = rspGroups.RSPLists()
        print "got rsp lists"
        print rspGroupLists

        if os.path.isdir(self.pArchive): 
            pass
        else: 
            os.mkdir(self.pArchive)
        if os.path.isdir(self.obsidPath):
            pass
        else:
            os.makedirs(self.stokesPath)

        for i in range(self.filefactor):
            rspDir =  os.path.join(self.stokesPath,"RSP"+str(i))
            if os.path.isdir(rspDir):
                self.rspDirs.append(rspDir)
            else:
                os.mkdir(rspDir)
                self.rspDirs.append(rspDir)

        self.__prepRSPS(rspGroupLists)
        self.__writeRSPLists(rspGroupLists)

        return


    def testParset(self):
        """
        test for parset location type

        old location style

        /globalhome/lofarsystem/log/<obsid>/RTCP.parset.0
        
        new location style,

        /globalhome/lofarsystem/log/L<yyy-mm-dd>_<hhmmss>/RTCP-09874.parset

        where hhmmss is the time of file write on day, <yyyy-mm-dd>

        """

        nominalParsetFile = os.path.join(defParsets, self.obsid, defParFile)

        if os.path.isfile(nominalParsetFile):
            print "Found nominal parset file."
            parset = nominalParsetFile
        else:
            print "searching for alternate parset..."
            parsetTuple = self.__findParsetFile()
            parset = os.path.join(parsetTuple[0],parsetTuple[1])
        return parset



    def __findParsetFile(self):
        """
        Hunt down the parset for a given obsid.
        Host site is

        /globalhome/lofarsystem/log/

        which is 

        Ivocation of this method necessarily implies that "nominalparsetFile"
        did not exist, and this obsid has a "new form" parset file.

        """

        parsetFile   = None
        parsetLogDir = None

        searchPath = defParsets
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



    def __prepRSPS(self, rspGroupLists):

        """
        self.rspGroupLists will be used by the InfoData class to facilitate calculation
        of several frequency parameters, namely

        Central freq of low channel (MHz)
        Total bandwidth (MHz)
        Number of channels
        Channel bandwidth (MHz)

        The InfoData class will handle these calculations for the .inf file write.

        """

        # _____________________________________________ #
        # get default parset file into obsid directory  #

        finalParFile = os.path.join(self.obsidPath,self.obsid+".parset")
        print finalParFile
        print "copy ",self.parset,"to obsid directory"
        shutil.copyfile(self.parset,os.path.join(self.obsidPath,finalParFile))

        # _____________________________________________ #
        # rename .inf files into the RSP directories    #

        prepInf = os.path.expanduser(defaultInf)
        outfile = os.path.basename(prepInf)
        shutil.copyfile(prepInf,os.path.join(self.obsidPath,outfile))

        infData = infodata.InfoData(prepInf, self.pulsar, rspGroupLists)
        infData.readInf()
        infData.calcInfPars(self.parset, self.obsid, self.stokesPath)

        return

    def __writeRSPLists(self,rspGroupLists):
        """
        Write RSP lists for each RSP, as well as a full list in <obsid>
        directory.
        """

        fullRSPList   = []
        for i in range(len(rspGroupLists)):
            rspListName = os.path.join(self.rspDirs[i],self.obsid+"_RSP"+str(i)+".list")
            fullRSPFile = os.path.join(self.obsidPath,self.obsid+"_Full_RSP.list")
            rspFile     = open(rspListName,"w")
            RSPFile     = open(fullRSPFile,"a")
            for line in rspGroupLists[i]:
                rspFile.write(line+"\n")
                RSPFile.write(line+"\n")
            rspFile.close()
            RSPFile.close()
            del rspFile, RSPFile
        return


    def __buildNewInfName(self, n):
        parname = self.pulsar + "_" + self.obsid + "_" + "RSP" + str(n) + ".sub.inf"
        return parname
        

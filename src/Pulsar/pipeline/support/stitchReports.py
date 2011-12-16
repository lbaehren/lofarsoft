#
# $Id: stitchReports.py 7110 2011-02-17 10:16:37Z kenneth $

#                                                          LOFAR PULSAR PIPELINE
#
#                                       Pulsar.pipeline.support.stitchReports.py
#                                                          Ken Anderson, 2011-02
#                                                            k.r.anderson@uva.nl
# ------------------------------------------------------------------------------

import os, glob

# ---- Pulp environment ---- #
import pulpEnv

# Repository info ...
__svn_revision__ = ('$Rev: 7110 $').split()[1]
__svn_revdate__  = ('$Date: 2011-02-17 11:16:37 +0100 (Thu, 17 Feb 2011) $')[7:26]
__svn_author__   = ('$Author: kenneth $').split()[1]


class StitchReports():

    """
    Post production merge of the rfi report files, i.e. "*.rfirep,"
    produced by subdyn.py, which produces an RFI report for
    each RSP split.  These files are dumped into cwd(). This is an
    offset append algorithm, where the offset is defined as

       <N_subbands>
        ----------  x (chan/subband) x (N_RSPgroup) ,
       <filefactor>   

    where,

    chan/subband - channelization,
                   hex channelization is currently nominal

    N_RSPgroup - index of <obsid> RSP directory

    This class does not employ <filefactor> per se, but rather
    makes an empirical determination by reading the appropriate
    `stokesPath' directory for a list of "RSP" directories, properly
    eschewing the "RSPA" directory, which is irrelevent here.
    """

    def __init__(self, obsid, pulsar, arch, uEnv):
        
        obsEnv = pulpEnv.PulpEnv(obsid,pulsar,arch,uEnv)
        
        self.obsid      = obsEnv.obsid
        self.stokes     = obsEnv.stokes
        self.pArchive   = obsEnv.pArchive
        self.obsidPath  = obsEnv.obsidPath
        self.stokesPath = obsEnv.stokesPath
        self.rspDirects = glob.glob(self.stokesPath+"/RSP[0-9]")
        self.offsetDict = self.__rfiOffsets()
        self.newRfiFile = os.path.join(self.stokesPath,
                                       self.obsid
                                       + "_"
                                       + "subAll.rfirep"
                                       )



    def show(self):
        """ Display the relational RSP-offset structure.
        """
        keys = self.offsetDict.keys()
        keys.sort()
        print "\n\n\tRSP CHANNEL OFFSETS for observation\n\t",self.obsid,":\n"
        print "\tRSP Directory     Measured offset"
        print "\t-------------     ---------------"
        for key in keys:
            print "\t",os.path.split(key)[1], "\t\t\t", self.offsetDict[key]
        print "\n\n"
        return



    def mergeFiles(self):

        """
        Offset merge .rfirep files, one file per RSP[n] directory.
        Files must be merged in sequential RSP order.

        mergeFiles() writes the full rfi report to the instance .stokesPath
        directory.

        return void
        """

        filePatt     = "RSP?/*.rfirep"
        globPatt     = os.path.join(self.stokesPath,filePatt)
        reportCards  = glob.glob(globPatt)
        header,lines = self.buildRfiReport(reportCards)
        self.writeFullRfiReport(header,lines)
        return


    def writeFullRfiReport(self, header,lines):
        fob = open(self.newRfiFile,"w")
        fob.write(header)
        for line in  lines:
            fob.write(line)
        fob.close()
        del fob
        return



    def buildRfiReport(self,reportCards):
        """
        Concatenate all rfi report file lines into rfiLines.
        return header, rfiLines : <str>, <list> 
        """

        reportCards.sort()
        rfiLines    = []
        header      = "# Subband\tFreq(MHz)\n"  # precautionary

        for rfi in reportCards:
            lines = open(rfi).readlines()
            rNum  = self.__getRspId(rfi)
            rPath = os.path.join(self.stokesPath,"RSP"+str(rNum))
            for line in lines:
                if "#" in line:
                    header = line
                    continue
                else:
                    channel=line.split("\t")[0].strip()
                    newChannel = int(channel) + self.offsetDict[rPath]
                    rfiLines.append(self.__buildNewFileLine(newChannel, line))
        return header,rfiLines



    def __buildNewFileLine(self,newchan,line):
        """ Builds a new, offset adjusted record for flagged channels.
        """

        newlinetail = line.split('\t')[-1].strip()
        newlineFull = str(newchan)+"\t\t"+newlinetail+"\n"
        return newlineFull



    def __getRspId(self,rfi):

        """Extract the RSP index from the rfi report filename.
        If this is not robust, something is very wrong.
        """
        rspN = os.path.split(rfi)[-1].split("_")[-1].split('.')[0][-1]
        return rspN



    def __rfiOffsets(self):

        """ Builds an offset reference structure.  Since we are not making file
        or directory assumptions, each RSP directory is length determined.
        
        the structure is a dictionary like,

        offsets = {'/path/to/RSP0' : <offset0>,
                   '/path/to/RSP1' : <offset1>,
                   '/path/to/RSP2' : <offset2>,
                   ...
                   }
        """
        
        subPatt = "/*.sub[0-9][0-9][0-9][0-9]"
        rfiOffsets = {}
        for i in range(len((self.rspDirects))):
            rfiOffsets[self.rspDirects[i]]= len(glob.glob(self.rspDirects[i]+subPatt))*i
        return rfiOffsets

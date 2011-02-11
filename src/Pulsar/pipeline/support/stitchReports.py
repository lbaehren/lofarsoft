#
# $Id$

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
__svn_revision__ = ('$Rev: 6880 $').split()[1]
__svn_revdate__  = ('$Date: 2011-01-28 14:51:09 +0100 (Fri, 28 Jan 2011) $')[7:26]
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
        self.obsidPath  = os.path.join(self.pArchive,self.obsid)
        self.stokesPath = os.path.join(self.obsidPath,self.stokes)
        self.newRfiFile = os.join.path(self.stokesPath,
                                       self.obsid +"_" +"subAll.rfirep")


    def mergeFiles(self):

        """
        Offset merge .rfirep files, one file per RSP[n] directory.
        Files must be merged in sequential RSP order.
        """

        filePatt   = "RSP?/*.rfirep"
        globPatt   = os.path.join(self.stokesPath,filePatt)
        reportCards= glob.glob(globPatt).sort()
        self.buildRfiReport(reportCards)


    def buildRfiReport(self,reportCards):
        """
        Concatenate all rfi report file lines into rfiLines.
        """
        rfiLines = []
        for rfi in reportCards:
            lines = open(rfi).readlines()
            for line in lines:
                if "#" or "Subband" or "Freq" in line: continue
                channel=line.split("\t")[0].strip()
                newChannel = channel+str((self.__getSubFileOffset(rfi)))
                rfiLines.append(self.__buildNewFileLine(newChannel, line))
        return


    def __buildNewFileLine(self,newchan,line):
        """ Builds a new, offset adjusted record for flagged channels.
        """

        newlinetail = line.split('\t')[-1].strip()
        newlineFull = newchan+"\t\t"+newlinetail+"\n"
        return newlineFull
        
                                         

    def __getSubfileOffset(self,rfi):
        """ `rfi' is a file path string of a passed .rfirep file
        """

        subPatt = ".sub[0-9][0-9][0-9][0-9]"
        rspId   = self.__getRspId(rfi)
        nFiles  = len(glob.glob(rdir+subPatt))
        offset  = nFiles*rspId
        return offset


            
    def __getRspId(self,rfi):

        """Extract the RSP index from the rfi report filename.
        If this is not robust, something is very wrong.
        """
        rspN = float(os.path.split(rfi)[-1].split("_")[-1].split('.')[0][-1])
        return rspN

#
# $Id: rfiDirectories.py 6881 2011-01-28 14:01:26Z kenneth $

#                                                          LOFAR PULSAR PIPELINE
#
#                                      Pulsar.pipeline.support.rfiDirectories.py
#                                                          Ken Anderson, 2010-10
#                                                            k.r.anderson@uva.nl
# ------------------------------------------------------------------------------

import os
import pulpEnv

# Repository info ...
__svn_revision__ = ('$Rev: 6881 $').split()[1]
__svn_revdate__  = ('$Date: 2011-01-28 15:01:26 +0100 (Fri, 28 Jan 2011) $')[7:26]
__svn_author__   = ('$Author: kenneth $').split()[1]


class RfiDirectories():

    """
    Produce various post production, value-added plots as requested.
    This involves running some scripts located in the pulsar build,
    i.e. 

    ${LOFARSOFT}/release/share/pulsar/bin/subdyn.py
    
    """

    def __init__(self, obsid, pulsar, arch, uEnv):
        
        obsEnv = pulpEnv.PulpEnv(obsid,pulsar,arch,uEnv)
        
        self.obsid      = obsEnv.obsid
        self.stokes     = obsEnv.stokes
        self.pArchive   = obsEnv.pArchive
        self.obsidPath  = os.path.join(self.pArchive,self.obsid)
        self.stokesPath = os.path.join(self.obsidPath,self.stokes)


    def getRspDirs(self):

        """
        returns a list of RSP directores that the rfiplot recipe will use
        
        """

        return os.listdir(self.stokesPath)
                

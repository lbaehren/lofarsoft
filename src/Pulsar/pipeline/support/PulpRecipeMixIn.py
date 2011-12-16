#$Id: PulpRecipeMixIn.py 6972 2011-02-04 14:03:18Z kenneth $

#
#                            Beam-formed data conversion Node (bf2presto) recipe
#                                     Pulsar.pipeline.support.PulpRecipeMixIn.py
#                                                          Ken Anderson, 2010-10
#                                                            k.r.anderson@uva.nl
# ------------------------------------------------------------------------------

import os.path

# Repository info ...
__svn_revision__ = ('$Rev: 6972 $').split()[1]
__svn_revdate__  = ('$Date: 2011-02-04 15:03:18 +0100 (Fri, 04 Feb 2011) $')[7:26]
__svn_author__   = ('$Author: kenneth $').split()[1]



class PulpRecipeBase(object):
    """
    Pulp recipe class to provide the ubiquitous method
    __buildUserEnv
    """
    

    def __buildUserEnv(self):
        """
        User environment must be pushed to the compute nodes.
        
        Environment variables needed:
        $LOFARSOFT
        $TEMPO
        $PRESTO
        
        These are stringified, pushed through to pulpEnv via calls on it,
	and unpacked in the pulpEnv module.
        
        """
        
        userEnv  = "LOFARSOFT = "+os.environ["LOFARSOFT"]
        userEnv += ":TEMPO = "   +os.environ["TEMPO"]
        userEnv += ":PRESTO ="   +os.environ["PRESTO"]

        return userEnv

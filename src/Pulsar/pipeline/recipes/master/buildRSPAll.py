#
# $Id$

#                                                          LOFAR PULSAR PIPELINE
#
#                                       Construct  RSP "ALL"(buildRSPAll) recipe
#                                  Pulsar.pipeline.recipes.master.buildRSPAll.py
#                                                          Ken Anderson, 2009-10
#                                                            k.r.anderson@uva.nl
# ------------------------------------------------------------------------------

import sys, os
 
# Local helpers
import lofarpipe.support.lofaringredient as ingredient

from lofarpipe.support.ipython       import LOFARTask
from lofarpipe.support.lofarrecipe   import LOFARrecipe
from lofarpipe.support.clusterlogger import clusterlogger
from lofarpipe.support.clusterdesc   import ClusterDesc, get_compute_nodes
from lofarpipe.support.lofarnode     import run_node

# Repository info ...
__svn_revision__ = ('$Rev$').split()[1]
__svn_revdate__  = ('$Date$')[7:26]
__svn_author__   = ('$Author$').split()[1]


class buildRSPAll(LOFARrecipe):

    """
    Pipeline-based mechanism for building the ad hoc RSPA 
    directory.

    Parser processes all arguments passed by the framework
    through cli arguments and any arguments specified in 
    tasks configuration files.

    Command line arguments override defaults set in the task.cfg.

    This recipe will build the "all" RSP directory for the given obsid.
    This is only done when filefactor is not equal to 1, which will not 
    be nominal (4 or 8 RSP splits will be nominal).  To this end, some
    fiddling to build a directory for the full set of subband data and then 
    add one job to the prepfold job queue.

    """
    inputs = {
        'obsid' : ingredient.StringField(
            '--obsid',
            dest="obsid",
            help="Observation identifier"
        ),
        'pulsar' : ingredient.StringField(
            '--pulsar',
            dest="pulsar",
            help="Pulsar name"
        ),
        'filefactor' : ingredient.IntField(
            '--filefactor',
            dest="filefactor",
            help="factor by which obsid subbands will be RSP split."
        ),
        'arch' : ingredient.StringField(
            '--arch',
            dest="",
            help="Destination PULSAR ARCHIVE"
        )
    }

    outputs = {
        'data': ingredient.ListField()
    }


    def go(self):
        super(buildRSPAll, self).go()

        self.logger.info("...building RSPA (RSP All) for obsid " + self.inputs['obsid'])

        obsid      = self.inputs['obsid']
        pulsar     = self.inputs['pulsar']
        filefactor = self.inputs['filefactor']
        arch       = self.inputs['arch']
        uEnv       = self.__buildUserEnv()

        # clusterlogger context manager accepts networked logging from compute nodes.
        # Need only one engine, get_ids()[0], to do the job.

        tc, mec = self._get_cluster()
        targets = mec.get_ids()[0]
        self.logger.info("Importing fullRSP ...")
        mec.execute("import fullRSP", targets=[targets])
        self.logger.info("Instatiating fullRSP.FullRSP()")
        mec.execute("rspAll = fullRSP.FullRSP(\"%s\",\"%s\",%d,\"%s\",\"%s\")" \
                        % (obsid,pulsar,filefactor,arch,uEnv), targets=[targets])
        self.logger.info("Calling ... rspAll.fillAllSubFiles()")
        mec.execute("rspAll.fillAllSubFiles()",targets=[targets])
        self.logger.info("Obsid directory, RSPA, is populated.")

        return


    def __buildUserEnv(self):
        """
        User environment must be pushed to the compute nodes.

        Environment variables needed:
        $LOFARSOFT
        $TEMPO
        $PRESTO

        These are stringified, pushed through to pulpEnv via RSPS call to pulpEnv,
        and unpacked in the pulpEnv module.

        """
        
        userEnv  = "LOFARSOFT = "+os.environ["LOFARSOFT"]
        userEnv += ":TEMPO = "   +os.environ["TEMPO"]
        userEnv += ":PRESTO ="   +os.environ["PRESTO"]

        return userEnv

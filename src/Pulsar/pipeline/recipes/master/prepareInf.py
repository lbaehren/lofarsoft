#                                                          LOFAR PULSAR PIPELINE
#
#                                    Initialize prepfold run (prepareInf) recipe
#                                                          Ken Anderson, 2009-10
#                                                            k.r.anderson@uva.nl
# ------------------------------------------------------------------------------

from __future__ import with_statement

import sys, os, logging 
from subprocess import check_call, CalledProcessError
from contextlib import closing
from collections import defaultdict

# Local helpers
from lofarpipe.support.ipython import LOFARTask
from lofarpipe.support.lofarrecipe import LOFARrecipe
from lofarpipe.support.clusterlogger import clusterlogger
from lofarpipe.support.clusterdesc import ClusterDesc, get_compute_nodes
import lofarpipe.support.utilities as utilities
from lofarpipe.support.lofarnode import run_node
import lofarpipe.support.lofaringredient as ingredient


class prepareInf(LOFARrecipe):

    """
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
        super(prepareInf, self).go()

        self.logger.info("...Preparing .inf files for obsid " + self.inputs['obsid'])

        obsid      = self.inputs['obsid']
        pulsar     = self.inputs['pulsar']
        arch       = self.inputs['arch']
        filefactor = self.inputs['filefactor']
        uEnv       = self.__buildUserEnv()

        # clusterlogger context manager accepts networked logging from compute nodes.
        # Need only one engine, get_ids()[0], to do the job.

        tc, mec = self._get_cluster()
        targets = mec.get_ids()[0]
        self.logger.info("Importing prepInfFiles ...")
        mec.execute("import prepInfFiles", targets=[targets])
        self.logger.info("Instatiating PrepInfFiles")
        mec.execute("infFiles = prepInfFiles.PrepInfFiles(\"%s\",\"%s\",%d,\"%s\",\"%s\")" \
                        % (obsid,pulsar,filefactor,arch,uEnv), targets=[targets])
        self.logger.info("Calling ...buildListsAndLinks()")
        mec.execute("infFiles.buildListsAndLinks()",targets=[targets])
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

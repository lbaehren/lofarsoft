#                                                          LOFAR PULSAR PIPELINE
#
#                                   Sbband directory build (buildPulsArch) recipe
#                                                          Ken Anderson, 2009-10
#                                                            k.r.anderson@uva.nl
# ------------------------------------------------------------------------------

import sys, os

# Local framework helpers
import lofarpipe.support.lofaringredient as ingredient

from lofarpipe.support.lofarrecipe   import LOFARrecipe
from lofarpipe.support.ipython       import LOFARTask
from lofarpipe.support.clusterlogger import clusterlogger
from lofarpipe.support.lofarnode     import run_node

# PULP libs 
import bf2Pars, RSPlist



class buildPulsArch(LOFARrecipe):
    """
    Provides a convenient, pipeline-based mechanism of running bf2presto
    on a dataset.

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
        super(buildPulsArch, self).go()

        self.logger.info("Building Pulsar Archive for obsid" + self.inputs['obsid'])

        obsid      = self.inputs['obsid']
        pulsar     = self.inputs['pulsar']
        filefactor = self.inputs['filefactor']
        arch       = self.inputs['arch']
        userEnv    = self.__buildUserEnv() # push to compute node

        # clusterlogger context manager accepts networked logging from compute nodes.
        # Need only one engine (..._ids()[0]) for this job.

        tc, mec = self._get_cluster()
        targets = mec.get_ids()[0]

        self.logger.info("import buildRSPS")
        mec.execute("import buildRSPS", targets=[targets])

        self.logger.info("Instantiating buildRSPS.buildRSPS() constructor ...")

        mec.execute("rsps = buildRSPS.buildRSPS(\"%s\",\"%s\",%d,\"%s\",\"%s\")" \
                        % (obsid,pulsar,filefactor,arch,userEnv), targets=[targets])

        self.logger.info("rsps = buildRSPS.buildRSPS(\"%s\",\"%s\",%d,\"%s\",\"%s\")" \
                             % (obsid,pulsar,filefactor,arch,userEnv))

        self.logger.info("calling makeLists method ...")
        mec.execute("rsps.makeLists()",targets=[targets])

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

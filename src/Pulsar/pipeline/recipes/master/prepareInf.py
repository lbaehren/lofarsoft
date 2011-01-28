#           $Id$

#                                                          LOFAR PULSAR PIPELINE
#
#                                    Initialize prepfold run (prepareInf) recipe
#                                   Pulsar.pipeline.recipes.master.prepareInf.py
#                                                          Ken Anderson, 2010-10
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


class prepareInf(LOFARrecipe):

    """
    Pipeline-based mechanism for writing pulp job parameter files.

    Parser processes all arguments passed by the framework
    through cli arguments and any arguments specified in tasks
    configuration files.

    Command line arguments override defaults set in the task.cfg.

    This recipe will build and install PRESTO-required ".inf" files
    for observation data processing.  The so-called "dotinf" files
    are written, one for each RSP directory, including the "all"
    directory, RSPA.
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

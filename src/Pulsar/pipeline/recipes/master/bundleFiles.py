#           $Id$

#                                                          LOFAR PULSAR PIPELINE
#
#                          Gather, tar select data products (bundleFiles) recipe
#                                         pipeline.recipes.master.bundleFiles.py
#                                                          Ken Anderson, 2010-11
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


class bundleFiles(LOFARrecipe):

    """
    Pipeline-based mechanism for creagting a tar archive of
    output data products from this pipeline.

    Parser processes all arguments passed by the framework
    through cli arguments and any arguments specified in 
    tasks configuration files.

    Command line arguments override defaults set in the task.cfg.

    This recipe will create a gzipped tarball for the obsid
    pulsar output.

    i.e. a file named like, <pulsarName>_<obsid>_plots.tar.gz

    eg.,

     B1112+50_L2010_21325_plots.tar.gz

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
            dest="arch",
            help="Destination output Pulsar Archive, string like 'arch134'",
            )
        }

    outputs = {
        'data': ingredient.ListField()
        }


    def go(self):
        super(bundleFiles, self).go()

        self.logger.info("... Bundling data for obsid " + self.inputs['obsid'])

        obsid      = self.inputs['obsid']
        pulsar     = self.inputs['pulsar']
        arch       = self.inputs['arch']
        filefactor = self.inputs['filefactor']
        #executable = self.inputs['executable']
        logDir     = self.config.get('layout','log_directory')
        userEnv    = self.__buildUserEnv()           # push to compute node

        # clusterlogger context manager accepts networked logging from compute nodes.
        # Need only one engine, get_ids()[0], to do the job.

        tc, mec = self._get_cluster()
        targets = mec.get_ids()[0]

        self.logger.info("Building tar archive ..." )
        self.logger.info("Tar archive will appear at OBSID directory level.")
        self.logger.info("remote import on bundlePlots ...")

        mec.execute("import bundlePlots",targets=[targets])

        self.logger.info("remote instantiation on bundlePlots.BundlePlots ...")

        mec.execute("tarball = bundlePlots.BundlePlots(\"%s\",\"%s\",\"%s\",\"%s\",\"%s\")" \
                        % (obsid,pulsar,arch,logDir,userEnv),targets=[targets])

        self.logger.info("calling ballIt() on tarball instance ...")

        mec.execute("tarch = tarball.ballIt()",targets=[targets])
        tarFile = mec.pull("tarch",targets=[targets])

        self.logger.info(obsid+" Tar archive built:")
        self.logger.info(tarFile)
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
        userEnv  = ""
        userEnv  = "LOFARSOFT = "+os.environ["LOFARSOFT"]
        userEnv += ":TEMPO = "   +os.environ["TEMPO"]
        userEnv += ":PRESTO ="   +os.environ["PRESTO"]

        return userEnv

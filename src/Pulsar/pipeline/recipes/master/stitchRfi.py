#
# $Id$

#                                                          LOFAR PULSAR PIPELINE
#
#                                                     master (stitchRfi)) recipe
#                                    Pulsar.pipeline.recipes.master.stitchRfi.py
#                                                          Ken Anderson, 2010-10
#                                                            k.r.anderson@uva.nl
# ------------------------------------------------------------------------------

from __future__ import with_statement

import sys, os

# Local helpers
import lofarpipe.support.lofaringredient as ingredient

from lofarpipe.support.ipython       import LOFARTask
from lofarpipe.support.lofarrecipe   import LOFARrecipe
from lofarpipe.support.clusterlogger import clusterlogger
from lofarpipe.support.clusterdesc   import ClusterDesc, get_compute_nodes
from lofarpipe.support.lofarnode     import run_node

# Repository info ...
__svn_revision__ = ('$Rev: 6880 $').split()[1]
__svn_revdate__  = ('$Date: 2011-01-28 14:51:09 +0100 (Fri, 28 Jan 2011) $')[7:26]
__svn_author__   = ('$Author: kenneth $').split()[1]

class stitchRfi(LOFARrecipe):

    """
    Pipeline-based mechanism of running th Pulsar Group code,
    subdyn.py, on a dataset.  "subdyn.py" is part of the pulsar
    build, located in the usual,

    ${LOFARSOFT}/release/share/pulsar/bin

    Parser processes all arguments passed by the framework
    through cli arguments and any arguments specified in 
    tasks configuration files.

    Command line arguments override defaults set in the task.cfg.

    This recipe will run the subdyn.py pulsar script (others as needed),
    producing the rfi report files,

    i.e. 

    """
    inputs = {
        'executable' : ingredient.StringField(
            '--executable',
            dest="executable",
            help="subdyn.py script"
            ),
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
        super(stitchRfi, self).go()

        self.logger.info("...Stitching rfi reports for obsid " + self.inputs['obsid'])

        obsid      = self.inputs['obsid']
        pulsar     = self.inputs['pulsar']
        arch       = self.inputs['arch']
        filefactor = self.inputs['filefactor']
        executable = self.inputs['executable']
        userEnv    = self.__buildUserEnv() # push to compute node

        # clusterlogger context manager accepts networked logging from
        # compute nodes.

        tc, mec = self._get_cluster()
        targets = mec.get_ids()[0]          # trim the wrapping list
        mec.execute("import stitchReports",targets=[targets])
        mec.execute("stitching= stitchReports.StitchReports(\"%s\",\"%s\",\"%s\",\"%s\")" \
                        % (obsid,pulsar,arch,userEnv),targets=[targets])
        mec.execute("fullRfi = stitching.mergeFiles()",targets=[targets])
        fullRfiReport = mec.pull("fullRfi",targets=[targets])
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

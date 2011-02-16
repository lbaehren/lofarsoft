#  
# $Id: bf2presto.py 6881 2011-01-28 14:01:26Z kenneth $

#                                                          LOFAR PULSAR PIPELINE
#
#                                 Beam-formed data conversion (bf2presto) recipe
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
__svn_revision__ = ('$Rev: 6881 $').split()[1]
__svn_revdate__  = ('$Date: 2011-01-28 15:01:26 +0100 (Fri, 28 Jan 2011) $')[7:26]
__svn_author__   = ('$Author: kenneth $').split()[1]



class bf2presto(LOFARrecipe):

    """
    Pipeline-based mechanism of running bf2presto on a dataset.
    Parser processes all arguments passed by the framework
    through cli arguments and any arguments specified in 
    tasks configuration files.

    Command line arguments override defaults set in the task.cfg.

    """

    inputs = {
        'executable' : ingredient.ExecField(
            '--executable',
            dest="executable",
            help="bf2presto executable"
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
            help="factor by which obsid subbands will be RSP split.",
            default=8
            ),
        'arch' : ingredient.StringField(
            '--arch',
            dest="arch",
            help="Destination output Pulsar Archive, string like 'arch134'",
            ),
        'collapse': ingredient.BoolField(
            '--collapse',
            dest="collapse",
            help="collapse the data.",
            default=False
            ),
        'nsigmas' : ingredient.IntField(
            '--nsigmas',
            dest="nsigmas",
            help="Data further than nsigma from the mean will be clipped",
            default=7
            )
        }

    outputs = {
        'data': ingredient.ListField()
    }


    def go(self):
        super(bf2presto, self).go()

        self.logger.info("Starting bf2presto run")

        obsid      = self.inputs['obsid']
        pulsar     = self.inputs['pulsar']
        filefactor = self.inputs['filefactor']
        arch       = self.inputs['arch']
        userEnv    = self.__buildUserEnv() # push to compute node

        # Build lists of available RSPs for <obsid> with remote calls.

        tc, mec = self._get_cluster()
        targets = mec.get_ids()[0]
        self.logger.debug("about to call RSPlist ...")
        mec.execute("import RSPlist",targets=[targets])
        self.logger.debug("import RSPlist success!...")
        mec.execute("rsps = RSPlist.RSPS(\"%s\",\"%s\",\"%d\",\"%s\",\"%s\")" \
                        % (obsid,pulsar,filefactor,arch,userEnv),targets=[targets])
        mec.execute("rls = rsps.RSPLists()",targets=[targets])
        rspLists = mec.pull("rls",targets=[targets])
        
        mec.push_function(dict(run_bf2presto=run_node))

        # clusterlogger context manager accepts networked logging from compute nodes.

        with clusterlogger(self.logger) as (loghost, logport):
            self.logger.debug("Logging to %s:%d" % (loghost, logport))

            # build task for each rspGroup, scheduling a bf2presto job for each one.

            tasks = []
            rspCount = 0
            for rspGroup in rspLists[0]:
                self.logger.info("rspGroup in process ... RSP"+str(rspCount)+":")
                task = LOFARTask(
                "result = run_bf2presto(inputs, rspGroup, uEnv, rspN)",
                push=dict(
                        recipename =self.name,
                        nodepath   =os.path.dirname(self.__file__.replace('master', 'nodes')),
                        rspGroup   =rspGroup,
                        inputs     =dict(self.inputs),
                        uEnv       =userEnv,
                        rspN       =rspCount,
                        loghost    =loghost,
                        logport    =logport
                        )
                )
                self.logger.info("Scheduling processing of %s" % ("RSP"+str(rspCount)))
                self.logger.info("files to process in rspGroup:")
                self.logger.info("RSP"+str(rspCount)+" appending to task list")

                tasks.append((tc.run(task), rspCount))
                rspCount+=1
            # Wait for all jobs to finish
            self.logger.info("Waiting for all bf2presto tasks to complete")
            self.logger.info("calling barrier on task list.")
            tc.barrier([task for task, rspCount in tasks])

        failure = False
        for task, rspCount in tasks:
            res = tc.get_task_result(task)
            if res.failure:
                self.logger.warn("Task %s failed (processing %s)" % (task, rspCount))
                self.logger.warn(res)
                self.logger.warn(res.failure.getTraceback())
                failure = True
        return failure



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

        


if (__name__ == '__main__'):
    sys.exit(bf2presto().main())

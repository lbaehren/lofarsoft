#                                                          LOFAR PULSAR PIPELINE
#
#                                        Executed data folding (prepfold) recipe
#                                     Pulsar.pipeline.recipes.master.prepfold.py
#                                                          Ken Anderson, 2009-10
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


class prepfold(LOFARrecipe):

    """
    Provides a convenient, pipeline-based mechanism of running bf2presto
    on a dataset.

    """

    inputs = {
        'executable' : ingredient.ExecField(
            '--executable',
            dest="executable",
            help="prepfold executable",
            default="/home/kanderson/LOFAR/lofarsoft/release/share/pulsar/bin/prepfold"
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
        'nopdsearch' : ingredient.BoolField(
            '--nopdsearch',
            dest="nopdsearch",
            help="Show but do not search over p-dot.",
            default=True
            ),
        'noxwin' : ingredient.BoolField(
            '--noxwin',
            dest="noxwin",
            help="No plots on-screen, only postscript files",
            default=True
            ),
        'fine': ingredient.BoolField(
            '--fine',
            dest="fine",
            help="Fine gridding in the p/pdot plane (for well known p and pdot)",
            default=True
            ),
        'nperstokes' : ingredient.IntField(
            '--nperstokes',
            dest="nperstokes",
            help="Number of Samples per Stokes Integration. Pulp default 256",
            default=256
            )
        }

    outputs = {
        'data': ingredient.ListField()
    }


    def go(self):
        super(prepfold, self).go()

        filefactor = self.inputs['filefactor']
        obsid      = self.inputs['obsid']
        arch       = self.inputs['arch']
        pulsar     = self.inputs['pulsar']
        userEnv    = self.__buildUserEnv() # push to compute node

        self.logger.info("Starting prepfold run on "+obsid+" ...")

        # start ipengines ...

        tc, mec = self._get_cluster()
        targets = mec.get_ids()[0]

        mec.execute("import RSPlist",targets=[targets])
        mec.execute("rsps = RSPlist.RSPS(\"%s\",\"%s\",\"%d\",\"%s\",\"%s\")" \
                        % (obsid,pulsar,filefactor,arch, userEnv),targets=[targets])
        mec.execute("rls = rsps.RSPLists()",targets=[targets])

        rspLists = mec.pull("rls",targets=[targets])
        mec.push_function(dict(run_prepfold=run_node))

        # clusterlogger context manager accepts networked logging from compute nodes.

        with clusterlogger(self.logger) as (loghost, logport):
            self.logger.debug("Logging to %s:%d" % (loghost, logport))

            # build task for each rspGroup, scheduling a prepfold job for each one.

            tasks = []
            rspCount = 0
            for rspGroup in rspLists[0]:                
                self.logger.info("rspGroup in process ... RSP"+str(rspCount)+":")
                task = LOFARTask(
                "result = run_prepfold(inputs, infiles, obsid, pulsar, arch, userEnv, rspCount)",
                push=dict(
                        recipename = self.name,
                        nodepath   = os.path.dirname(self.__file__.replace('master', 'nodes')),
                        inputs     = dict(self.inputs),
                        infiles    = rspGroup,
                        obsid      = obsid,
                        arch       = arch,
                        pulsar     = pulsar,
                        userEnv    = userEnv,
                        rspCount   = rspCount,
                        loghost    = loghost,
                        logport    = logport
                        )
                )
                self.logger.info("Scheduling processing of %s" % ("RSP"+str(rspCount)))
                tasks.append((tc.run(task),rspGroup))
                rspCount += 1

            # ____________________________________ #
            #   RSPA job appending to task list ...#

            if filefactor > 1:
                rspGroup = self.__ravelRspLists(rspLists)

                self.logger.info("rspGroup in process ... RSPA:")

                task = LOFARTask(
                    "result = run_prepfold(inputs, infiles, obsid, pulsar, arch, userEnv, rspCount)",
                    push=dict(
                        recipename = self.name,
                        nodepath   = os.path.dirname(self.__file__.replace('master', 'nodes')),
                        inputs     = dict(self.inputs),
                        infiles    = rspGroup,
                        obsid      = obsid,
                        pulsar     = pulsar,
                        arch       = arch,
                        userEnv    = userEnv,
                        rspCount   = "A",
                        loghost    = loghost,
                        logport    = logport
                        )
                    )

                # ____________________________________ #
                #   RSPA job appending   ...            #

                self.logger.info("Scheduling processing of %s" % ("RSPA"))
                tasks.append((tc.run(task),rspGroup))
            else:
                pass
            # task client (tc) blocks until completion.

            self.logger.info("Attending prepfold task completion...")
            tc.barrier([task for task, rspGroup in tasks])

            failure = False
            for task, rspGroup in tasks:
                res = tc.get_task_result(task)
                if res.failure:
                    self.logger.warn("Task %s failed." % (task,))
                    self.logger.warn(res)
                    self.logger.warn(res.failure.getTraceback())
                    failure = True
            if failure:
                return 1
        return 0


    def __ravelRspLists(self,rspLists):

        """ receives a <filefactor> folded RSP list, returns a the
        the full flat list of subband data.
        """

        fullList = []
        for group in rspLists:
            for file in group:
                fullList.append(file)

        return fullList


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



if (__name__ == '__main__'):
    sys.exit(prepfold().main())

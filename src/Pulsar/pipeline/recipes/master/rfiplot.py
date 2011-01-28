#           $Id$

#                                                          LOFAR PULSAR PIPELINE
#
#                                              rfiplot master (subdyn.py) recipe
#                                      Pulsar.pipeline.recipes.master.rfiplot.py
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
__svn_revision__ = ('$Rev$').split()[1]
__svn_revdate__  = ('$Date$')[7:26]
__svn_author__   = ('$Author$').split()[1]

class rfiplot(LOFARrecipe):

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
        super(rfiplot, self).go()

        self.logger.info("... Making rfi report for obsid " + self.inputs['obsid'])

        obsid      = self.inputs['obsid']
        pulsar     = self.inputs['pulsar']
        arch       = self.inputs['arch']
        filefactor = self.inputs['filefactor']
        executable = self.inputs['executable']
        userEnv    = self.__buildUserEnv() # push to compute node

        # clusterlogger context manager accepts networked logging from compute nodes.
        # Need only one engine, get_ids()[0], to do the job.

        tc, mec = self._get_cluster()
        targets = mec.get_ids()[0]

        mec.execute("import rfiDirectories",targets=[targets])
        mec.execute("rdirs = rfiDirectories.RfiDirectories(\"%s\",\"%s\",\"%s\",\"%s\")" \
                        % (obsid,pulsar,arch,userEnv),targets=[targets])

        mec.execute("rls = rdirs.getRspDirs()",targets=[targets])

        rspDirs = mec.pull("rls",targets=[targets])
        mec.push_function(dict(run_rfiplot=run_node))

        # rspDirs will include the RSPA directory.  This is not to be included
        # as an rfiplot job.  pop() off the last item, which is 'RSPA'
        # The pulled result 'rspDirs' is a nested list:
        #
        # [['dir1','dir2',dir3']]
        #
        if filefactor > 1:
            rspDirs[0].pop()     # discard 'RSPA' == rspDirs[0][-1]

        # clusterlogger context manager accepts networked logging from compute nodes.
        
        with clusterlogger(self.logger) as (loghost, logport):
            self.logger.debug("Logging to %s:%d" % (loghost, logport))

            # build task for each RSP dir, scheduling a rfiplot job for each one.

            tasks = []

            for rDir in rspDirs[0]:
                self.logger.info("RSP Directory in process ... "+rDir+":")
                task = LOFARTask(
                "result = run_rfiplot(executable,obsid,pulsar,arch,userEnv,rDir)",
                push=dict(
                        recipename=self.name,
                        nodepath  =os.path.dirname(self.__file__.replace('master', 'nodes')),
                        executable=executable,
                        obsid   = obsid,
                        pulsar  = pulsar,
                        arch    = arch,
                        userEnv = userEnv,
                        rDir    = rDir,
                        loghost = loghost,
                        logport = logport
                        )
                )

                self.logger.info("Scheduling processing of %s" % (rDir))
                self.logger.info(rDir+" appending to task list")

                tasks.append((tc.run(task), rDir))
            # Wait for all jobs to finish
            self.logger.info("Waiting for all rfiplot tasks to complete")
            self.logger.info("calling barrier on task list.")
            tc.barrier([task for task, rDir in tasks])

        failure = False
        for task, rDir in tasks:
            res = tc.get_task_result(task)
            if res.failure:
                self.logger.warn("Task %s failed (processing %s)" % (task, rDir))
                self.logger.warn(res)
                self.logger.warn(res.failure.getTraceback())
                failure = True
        return failure

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

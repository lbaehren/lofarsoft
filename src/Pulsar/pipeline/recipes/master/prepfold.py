#!/usr/bin/env python
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


class prepfold(LOFARrecipe):
    def __init__(self):
        super(prepfold, self).__init__()
        self.optionparser.add_option(
            '--executable', 
            dest='executable',
            default='/home/kanderson/LOFAR/lofarsoft/release/share/pulsar/bin/bf2presto'
            )

        # pipeline job args

        self.optionparser.add_option(
            '--obsid',
            dest="obsid",
            help="LOFAR observation id."
            )

        self.optionparser.add_option(
            '--pulsar',
            dest="pulsar",
            help="LOFAR observation pulsar name"
            )

        self.optionparser.add_option(
            '--filefactor',
            dest="filefactor",
            help="Number of RSP groups"
            )

        # prepfold cmd args

        self.optionparser.add_option(
            '--noxwin',
            dest="noxwin",
            help="No plots on-screen, only postscript files"
            )    

        self.optionparser.add_option(
            '--fine',
            dest="fine",
            help="A finer gridding in the p/pdot plane (for well known p and pdot)"
            )

        self.optionparser.add_option(
            '--nperstokes',
             dest="nperstokes",
            help="Number of Samples per Stokes Integration. Pulp default 256"
            )

        self.optionparser.add_option(
            '--nopdseaarch',
            dest="nopdsearch",
            help="how but do not search over p-dot."
            )


    def go(self):

        filefactor = self.inputs['filefactor']
        obsid      = self.inputs['obsid']
        
        try:
            self.__checkIn()
        except RuntimeError,msg:
            self.logger.info("prepfold runtime checkin failed."+msg)
            raise RuntimeError,msg

        self.logger.info("Starting prepfold run")

        super(prepfold, self).go()

        # start ipengines ...

        tc, mec = self._get_cluster()
        targets = mec.get_ids()[0]

        mec.execute("import RSPlist",targets=[targets])
        mec.execute("rsps = RSPlist.RSPS(\"%s\",\"%d\")" % (obsid,filefactor),targets=[targets])
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
                "result = run_prepfold(inputs, infiles, rspCount)",
                push=dict(
                        recipename = self.name,
                        nodepath   = os.path.dirname(self.__file__.replace('master', 'nodes')),
                        inputs     = self.inputs,
                        infiles    = rspGroup,
                        rspCount   = rspCount,
                        loghost    = loghost,
                        logport    = logport
                        )
                )
                self.logger.info("Scheduling processing of %s" % ("RSP"+str(rspCount)))
                tasks.append((tc.run(task),rspGroup))
                rspCount += 1
                # Wait for all jobs to finish

            self.logger.info("Waiting for all prepfold tasks to complete")
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


    def __checkIn(self):
        # check for required inputs...
        if not self.inputs["obsid"]:
            self.logger.debug("RuntimeError: no obsid found.")
            self.logger.debug("RuntimeError: Reinstantiate with --obsid=")
            raise RuntimeError("No obsid value found.\nReinstantiate with --obsid=")
        if not self.inputs["pulsar"]:
            self.logger.debug("RuntimeError: no pulsar found.")
            self.logger.debug("RuntimeError: Reinstantiate with --pulsar=")
            raise RuntimeError("No pulsar name.\nReinstantiate with --pulsar=")
        return


if (__name__ == '__main__'):
    sys.exit(prepfold().main())

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

import bf2Pars, RSPlist


class bf2presto(LOFARrecipe):
    def __init__(self):
        super(bf2presto, self).__init__()
        self.optionparser.add_option(
            '--executable', 
            dest='executable',
            default='/home/kanderson/LOFAR/lofarsoft/release/share/pulsar/bin/bf2presto8'
            )

        self.optionparser.add_option(
            '--log',
            dest='log',
            help='Log file'
            )

        self.optionparser.add_option(
            '-w',
            '--working-directory',
            dest="working_directory",
            help="Working directory used on compute nodes"
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
            help="factor by which obsid subbands will be RSP split."
            )

        # hooks to bf2presto args

        self.optionparser.add_option(
            '--subbase',
            dest="subbase",
            help="-f Number of the Base Subband (Default = 200)",
            default="1"       # pulp default:1, bf2presto actual: 200
            )

        self.optionparser.add_option(
            '--aveblocks',
            dest="aveblocks",
            help="-A averaging blocks",
            default="100"     # pulp default:100, bf2presto actual: 600
            )
        
        self.optionparser.add_option(
            '--collapse',
            dest="collapse",
            help="collapse the data.",
            default="False"
            )

        self.optionparser.add_option(
            '--nsigmas',
            dest="nsigmas",
            help=""
            )



    def go(self):
        try:
            self.__checkIn()
        except RuntimeError,msg:
            self.logger.info("bf2presto runtime checkin failed."+msg)
            raise RuntimeError,msg

        self.logger.info("Starting bf2presto run")

        super(bf2presto, self).go()

        obsid = self.inputs['obsid']
        pulsar= self.inputs['pulsar']
        filefactor = self.inputs['filefactor']

        # Build lists of available RSPs for <obsid> with remote calls.

        tc, mec = self._get_cluster()
        targets = mec.get_ids()[0]
        mec.execute("import RSPlist",targets=[targets])
        mec.execute("rsps = RSPlist.RSPS(\"%s\",\"%d\")" % (obsid,filefactor),targets=[targets])
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
                "result = run_bf2presto(inputs, rspGroup, rspN)",
                push=dict(
                        recipename =self.name,
                        nodepath   =os.path.dirname(self.__file__.replace('master', 'nodes')),
                        rspGroup   =rspGroup,
                        inputs     =self.inputs,
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
        if not self.inputs["filefactor"]:
            self.logger.info("no filefactor passed, default to 4")
            self.inputs["filefactor"]= 4
        return


if (__name__ == '__main__'):
    sys.exit(bf2presto().main())

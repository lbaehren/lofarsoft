#! /usr/bin/env python

from __future__ import with_statement
import sys, os

# Local helpers
import lofarpipe.support.utilities as utilities
from lofarpipe.support.lofarrecipe import LOFARrecipe
from lofarpipe.support.ipython import LOFARTask
from lofarpipe.support.clusterlogger import clusterlogger
from lofarpipe.support.lofarnode import run_node

import bf2Pars, RSPlist


class buildPulsArch(LOFARrecipe):
    def __init__(self):
        super(buildPulsArch, self).__init__()
        self.optionparser.add_option(
            '--executable', 
            dest='executable',
            default='/home/hassall/bin/bf2presto-working'
            )

        self.optionparser.add_option(
            '-w',
            '--working-directory',
            dest="working_directory",
            help="Working directory used on compute nodes"
            )

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

    def go(self):
        try:
            self.__checkIn()
        except RuntimeError, msg:
            self.logger.info("failed on RunTimeError",msg)
            raise RuntimeError, msg

        super(buildPulsArch, self).go()
        self.logger.info("building Pulsar Archive for obsid" + self.inputs['obsid'])

        job_directory = self.config.get("layout", "job_directory")
        obsid      = self.inputs['obsid']
        pulsar     = self.inputs['pulsar']
        filefactor = self.inputs['filefactor']

        # clusterlogger context manager accepts networked logging from compute nodes.

        tc, mec = self._get_cluster()
        targets = mec.get_ids()[0]
        mec.execute("import buildRSPS", targets=[targets])
        self.logger.info("import buildRSPS")
        mec.execute("rsps = buildRSPS.buildRSPS(\"%s\",\"%s\",%d)" % (obsid,pulsar,filefactor), targets=[targets])
        self.logger.info("rsps = buildRSPS.buildRSPS(\"%s\",\"%s\",%d)" % (obsid,pulsar,filefactor)) 
        self.logger.info("rsps.buildRSPS()")
        mec.execute("rsps.buildRSPS()",targets=[targets])


    def __checkIn(self):
        # check for required inputs...
        if not self.inputs["obsid"]:
            self.logger.debug("RuntimeError: no obsid found.")
            self.logger.debug("RuntimeError: Reinstantiate with --obsid=")
            raise RuntimeError("No obsid value found.\nReinstantiate with --obsid=")
        if not self.inputs["pulsar"]:
            self.logger.debug("RuntimeError: no pulsar name supplied")
            self.logger.debug("RuntimeError: Reinstantiate with --target=")
            raise RuntimeError("No pulsar name.\nReinstantiate with --pulsar=")
        if not self.inputs["filefactor"]:
            self.logger.info("no filefactor passed, default to 4")
            self.inputs["filefactor"]= 4
        return

#                                                          LOFAR PULSAR PIPELINE
#
#                            Beam-formed data conversion Node (bf2presto) recipe
#                                                          Ken Anderson, 2009-10
#                                                            k.r.anderson@uva.nl
# ------------------------------------------------------------------------------

from __future__ import with_statement
from subprocess import Popen, CalledProcessError, PIPE, STDOUT

import os.path

# local helpers
from lofarpipe.support.lofarnode       import LOFARnode
from lofarpipe.support.utilities       import log_time
from lofarpipe.support.lofarexceptions import ExecutableMissing

# PULP libs
import bf2Pars
import pulpEnv


class bf2presto(LOFARnode):
    def run(self, inputs, infiles, uEnv, rspN):

        self.inputs = inputs
        self.infiles= infiles
        self.rspN   = rspN
        
        obsid  = self.inputs['obsid']
        pulsar = self.inputs['pulsar']
        arch   = self.inputs['arch']
        uEnv   = uEnv

        obsEnv = pulpEnv.PulpEnv(obsid,pulsar,arch,uEnv) # environment

        self.obsid  = obsEnv.obsid
        self.pulsar = obsEnv.pulsar
        self.stokes = obsEnv.stokes
        self.pArch  = obsEnv.archPaths[arch]

        with log_time(self.logger):
            self.logger.info("Building bf2presto command line...")
            try: 
                bf2_cmd = self.__buildcmd(obsEnv)
            except KeyError, err: 
                self.logger.debug("caught exception on __buildcmd()")
                self.logger.debug(Exception)
                self.logger.debug(err)
                raise KeyError, err
            self.logger.info('done buildcmd: RSP'+str(self.rspN))

            try:
                self.logger.info("Running bf2presto: RSP"+str(self.rspN))
                bfproc = Popen(bf2_cmd, stdout=PIPE, stderr=PIPE)
                (sout, serr,) = bfproc.communicate()
                result = 0
            except CalledProcessError,e:
                self.logger.exception('Call to bf2presto failed')
                raise CalledProcessError(bf2_proc.returncode, executable)
                result = 1
                # For CalledProcessError isn't properly propagated by IPython
                # Temporary workaround...
                self.logger.error(str(e))
                raise Exception
            except ExecutableMissing, e:
                self.logger.error("%s not found" % (e.args[0]))
                raise ExecutableMissing
            return result


    def __buildcmd(self, obsEnv):
        """
        build the full bf2presto command line
        """
        self.logger.debug("@ bf2presto.__buildcmd()")
        bf2_cmd   = [self.inputs['executable']]
        bf2_flags = bf2Pars.bf2Pars(obsEnv)

        try:
            bf2_flags.readBFpars()
        except Exception, err:
            self.logger.debug("Call to readParset() raised an exception")

        if bf2_flags.transpose2:
            self.logger.debug("Observation parset indicates 2nd transpose.")
            bf2_cmd.extend([('-t')])

        if self.inputs["collapse"] != False:
            self.logger.debug("Collapse switch, -C: on")
            bf2_cmd.extend([('-C')])

        if self.inputs["collapse"] == False:
            self.logger.debug("Collapse switch off.")
            self.logger.debug("No command extension.")

        if self.inputs["nsigmas"] != 7:
            self.logger.debug("Got nsigmas:"+ str(self.inputs['nsigmas']))
            self.logger.debug("Extending cmd ...")
            bf2_cmd.extend([('-r ' + str(self.inputs["nsigmas"]))])

        bfRootOut = self.__buildOutRoot()

        bf2_cmd.extend([
                ('-A 10'),
                ('-f 0'),
                ('-c ' + bf2_flags.channels),
                ('-n 16'),
                ('-N ' + str(bf2_flags.Nsamples)),
                ('-o ' + bfRootOut),
                ])

        self.logger.debug("bf2presto opts/switches compiled:")
        self.logger.debug(bf2_cmd)
        bf2_cmd.extend(self.infiles)

        return bf2_cmd


    def __buildOutRoot(self):
        outRoot = os.path.join(
            self.pArch,
            self.obsid,
            self.stokes,
            "RSP"+str(self.rspN),
            self.pulsar+"_"+self.obsid+"_"+"RSP"+str(self.rspN)
            )
        return outRoot
            
 

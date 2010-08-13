# Python standard library
from __future__ import with_statement

from contextlib import closing
from subprocess import Popen, CalledProcessError, PIPE, STDOUT
import os.path, tempfile, shutil

from lofarpipe.support.lofarnode import LOFARnode
from lofarpipe.support.utilities import log_time
from lofarpipe.support.lofarexceptions import ExecutableMissing

import bf2Pars

#arch   = '/net/sub5/lse013/data4/PULSAR_ARCHIVE'
arch   = '/net/sub5/lse015/data4/PULSAR_ARCHIVE'
stokes = 'incoherentstokes'

class bf2presto(LOFARnode):
    def run(self, inputs, infiles, rspN):

        self.inputs = inputs
        self.obsid  = self.inputs['obsid']
        self.pulsar = self.inputs['pulsar']
        self.infiles= infiles
        self.rspN   = rspN
        self.stokes = stokes
        self.arch   = arch
        self.filefactor = self.inputs['filefactor']

        with log_time(self.logger):
            self.logger.info("Building bf2presto command line...")
            try: 
                bf2_cmd = self.__buildcmd()
            except KeyError, err: 
                self.logger.debug("caught exception on __buildcmd()")
                self.logger.debug(Exception)
                self.logger.debug(err)
                raise KeyError, err
            self.logger.info('done buildcmd: RSP'+str(self.rspN))
            #self.logger.info(bf2_cmd)
            try:
                self.logger.info("Running bf2presto: RSP"+str(self.rspN))
                #self.logger.debug(('Executing: %s' % bf2_cmd))
                bfproc = Popen(bf2_cmd, stdout=PIPE, stderr=PIPE)
                (sout, serr,) = bfproc.communicate()
                #self.logger.info(('bf2presto stdout: ' + sout))
                #self.logger.info(('bf2presto stderr: ' + serr))
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


    def __buildcmd(self):
        """
        build the full bf2presto command line
        """
        self.logger.debug("@ bf2presto.__buildcmd()")
        bf2_cmd   = [self.inputs['executable']]
        bf2_flags = bf2Pars.bf2Pars(self.obsid,self.pulsar)

        try:
            bf2_flags.readParset()
        except Exception, err:
            self.logger.debug("problem on readParset")
            
        if self.inputs["subbase"]:
            bf2_cmd.extend([('-f ' + self.inputs["subbase"])])
            self.logger.debug("Got subbase value. Extending cmd ...")

        if self.inputs["aveblocks"]:
            bf2_cmd.extend([('-A ' + self.inputs["aveblocks"])])
            self.logger.debug("Got aveblocks value. Extending cmd ...")

        if self.inputs["collapse"] != "False":
            self.logger.debug("Collapse switch, -C: on")
            bf2_cmd.extend([('-C ' + self.inputs["collapse"])])

        if self.inputs["nsigmas"] != "False":
            bf2_cmd.extend([('-r ' + self.inputs["nsigmas"])])
            self.logger.debug("Got nsigmas:"+self.inputs['nsigmas'])
            self.logger.debug("Extending cmd ...")

        bfRootOut = self.__buildOutRoot()

        bf2_cmd.extend([
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
            self.arch,
            self.obsid,
            self.stokes,
            "RSP"+str(self.rspN),
            self.pulsar+"_"+self.obsid+"_"+"RSP"+str(self.rspN)
            )
        return outRoot
            
 

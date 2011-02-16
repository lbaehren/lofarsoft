#
# $Id$

#                                                          LOFAR PULSAR PIPELINE
#
#                                                rfiplot (subdyn.py) Node recipe
#                                       Pulsar.pipeline.recipes.nodes.rfiplot.py
#                                                          Ken Anderson, 2010-10
#                                                            k.r.anderson@uva.nl
# ------------------------------------------------------------------------------

from __future__ import with_statement
from subprocess import Popen, CalledProcessError, PIPE, STDOUT

import os, glob

from lofarpipe.support.lofarnode       import LOFARnode
from lofarpipe.support.utilities       import log_time
from lofarpipe.support.lofarexceptions import ExecutableMissing

# PULP libs
import bf2Pars
import pulpEnv

# Repository info ...
__svn_revision__ = ('$Rev$').split()[1]
__svn_revdate__  = ('$Date$')[7:26]
__svn_author__   = ('$Author$').split()[1]



class rfiplot(LOFARnode):

    """
        Execute the plotting script, subdyn.py.

        script-kiddie behaviour requires moving into the local data directory.

        Set arguments for subdyn.py, where usage is ...
        Usage:

          subdyn.py 
                --nbins     -n,<value> - n samples to average (default: 7630)
                --win       -w,<value> - window size(seconds) to show
                --offset    -l,<value> - offset from start (in seconds)
                --threshold -t <value> - sigma to clip RFI (default: 6)
                --excludeonly          - only exclude completely junk subbands
                --rfilimit <value>     - allowable RFI fraction per subband
                --statistics           - info on large positive/negative samples
                --saveonly             - only saves png file and exits
                --help      -h,        - print this message

        Typical script cmd look like

        subdyn.py --saveonly -n ${SAMPLES}*10 <.subnnnn filelist>

        """

    def run(self, executable, obsid, pulsar, arch, userEnv,rdir):

        obsEnv = pulpEnv.PulpEnv(obsid,pulsar,arch,userEnv) # environment
 
        self.obsid  = obsEnv.obsid
        self.pulsar = obsEnv.pulsar
        self.stokes = obsEnv.stokes
        self.arch   = obsEnv.pArchive
        self.rDir   = rdir
        
        self.obsidPath  = os.path.join(self.arch, self.obsid)
        self.stokesPath = os.path.join(self.obsidPath,self.stokes)
        self.executable = executable

        # Move into local RSP directory, rdir
        curdir = os.getcwd()
        endD   = os.path.join(self.stokesPath,self.rDir)
        os.chdir(endD)
        
        # Okay, node recipe is now in the RSP directory, rDir.

        with log_time(self.logger):

            self.logger.info("Building subdyn command line...")

            try: 
                subdyn_cmd = self.__buildcmd(obsEnv)
            except KeyError, err: 
                self.logger.debug("caught exception on __buildcmd()")
                self.logger.debug(Exception)
                self.logger.debug(err)
                raise KeyError, err

            self.logger.info('done buildcmd:'+self.rDir)
            self.logger.info(subdyn_cmd)

            try:
                self.logger.info("Running subdyn on: "+self.rDir)
                rfiproc   = Popen(subdyn_cmd, stdout=PIPE, stderr=PIPE)
                sout,serr = rfiproc.communicate()
                result    = 0
                self.logger.info(('subdyn stdout: ' + sout))
                self.logger.info(('subdyn stderr: ' + serr))
            except CalledProcessError,e:
                self.logger.exception('Call to rfiplot failed')
                raise CalledProcessError(rfiproc.returncode, executable)
                result = 1
                # For CalledProcessError isn't properly propagated by IPython
                # Temporary workaround...
                self.logger.error(str(e))
                raise Exception
            except ExecutableMissing, e:
                self.logger.error("%s not found" % (e.args[0]))
                raise ExecutableMissing

        # return whence we came ...

        os.chdir(curdir)

        return result


    def __buildcmd(self, env):

        """
        build the subdyn.py  command line
        """

        self.logger.debug("@ rfiPlot.__buildcmd()")
        subFiles = self.__subFileList()
        nArg     = self.__getN(env)

        subdyn_cmd = [self.executable]
        subdyn_cmd.extend(["--saveonly"])
        subdyn_cmd.extend(['-n'])
        subdyn_cmd.extend([str(nArg)])

        self.logger.debug("subdyn opts/switches compiled:")
        self.logger.debug(subdyn_cmd)
        subdyn_cmd.extend(subFiles)

        return subdyn_cmd


    def __getN(self, env):

        """ Return the value ${SAMPLES}*10"""

        pars = bf2Pars.bf2Pars(env)
        pars.readBFpars()
        nArg = pars.Nsamples*10
        return nArg


    def __subFileList(self):
        
        subFileNames = []
        fullFileNames = glob.glob("*.sub[0-9]???")
        for file in fullFileNames:
            subFileNames.append(os.path.basename(file))
        
        return subFileNames

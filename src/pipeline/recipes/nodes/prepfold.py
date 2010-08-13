# Python standard library
from __future__ import with_statement
from contextlib import closing
from subprocess import Popen, check_call, CalledProcessError, PIPE, STDOUT
import os, tempfile, shutil

from lofarpipe.support.lofarnode import LOFARnode
from lofarpipe.support.utilities import patch_parset, create_directory, log_time, read_initscript
from lofarpipe.support.lofarexceptions import ExecutableMissing

#arch   = '/net/sub5/lse013/data4/PULSAR_ARCHIVE'
arch   = '/net/sub5/lse015/data4/PULSAR_ARCHIVE'
stokes = 'incoherentstokes'

import sys

class prepfold(LOFARnode):
    def run(self, inputs, infiles,  rspN):
        self.inputs     = inputs
        self.infiles    = infiles
        self.rspN       = rspN
        self.obsid      = self.inputs['obsid']
        self.pulsar     = self.inputs['pulsar']
        self.stokes     = stokes
        self.pulsArch   = arch
        self.executable = self.inputs['executable']
        
        with log_time(self.logger):
            try:
                os.access(self.executable, os.X_OK)
            except:
                raise ExecutableMissing(self.executable)

            self.logger.debug("Moving to RSP" + str(self.rspN))
            self.logger.debug("As working directory for prepfold ...")

             #                                                #
             # prepfold forcing a move to the RSP directories #
             #                                                #

            curDir = os.getcwd()
            newDir = os.path.dirname(self.__buildOutRoot())
            os.chdir(newDir)
            self.logger.debug("In RSP"+str(self.rspN)+" directory NOW")
            self.logger.debug(os.getcwd())
            
            ###  prepfold_cmd = self.__buildListCmd()
            ###  self.logger.info('__buildListCmd() success!')

            prepfold_cmd = self.__buildLocalListCmd()

            self.logger.info('__buildLocalListCmd() success!')
            self.logger.debug("Preping execution of prepfold in "+newDir)

            prepfold_cwd = os.path.dirname(self.__buildOutRoot())

            self.logger.debug("cwd set to:")
            self.logger.debug(prepfold_cwd)

            try:
                self.logger.debug(('Executing: %s' % self.executable))
                self.logger.debug("dumping built command line ...\n\n")
                self.logger.debug(prepfold_cmd)

                #check_call(prepfold_cmd)

                env = {'TEMPO': '/home/kanderson/LOFAR/lofarsoft/release/share/pulsar/bin',
                       'PATH': "/home/kanderson/LOFAR/lofarsoft/release/share/pulsar/bin"
                       }

                preproc = Popen(prepfold_cmd, cwd=prepfold_cwd, stdout=PIPE, stderr=PIPE, env=env)
                sout,serr = preproc.communicate()

                self.logger.info(('prepfold stdout: ' + sout))
                self.logger.info(('prepfold stderr: ' + serr))

                result = 0

            except CalledProcessError,e:
                self.logger.exception('Call to prepfold failed')
                raise CalledProcessError(prepproc.returncode, self.executable)
                result = 1
                # CalledProcessError isn't properly propagated by IPython
                # Temporary workaround...
                self.logger.error(str(e))
                raise Exception
            except ExecutableMissing, e:
                self.logger.error("%s not found" % (e.args[0]))
                raise ExecutableMissing

            os.chdir(curDir)

            return result


    # --------------------- # "private" # ---------------------- #


    def __buildListCmd(self):

        """
        build the full prepfold command line as an argument list
        """

        self.logger.debug("@ .__buildListCmd()")
        self.logger.debug("building prepfold command ...")

        outRoot      = self.__buildOutRoot()
        prepfold_cmd = [self.executable]

        prepfold_cmd.extend([('-psr')])
        prepfold_cmd.extend([(self.pulsar)])
        self.logger.debug("prepfold command extended: -psr "+self.pulsar)

        if self.inputs['noxwin'] == 'True':
            prepfold_cmd.extend([('-noxwin')])
            self.logger.debug("prepfold command extended: -noxwin")

        if self.inputs['fine']== 'True':
            prepfold_cmd.extend([('-fine')])
            self.logger.debug("prepfold command extended: -fine")

        if self.inputs['nopdsearch']== 'True':
            prepfold_cmd.extend([('-nopdsearch')])
            self.logger.debug("prepfold command extended: -nopdsearch")

        prepfold_cmd.extend([('-n')])
        nperstokes = str(self.inputs["nperstokes"])
        prepfold_cmd.extend([(nperstokes)])
        self.logger.debug("prepfold command extended: -n "+ nperstokes)

        prepfold_cmd.extend([('-o')])
        prepfold_cmd.extend([(outRoot)])
        self.logger.debug("prepfold command extended: -o "+outRoot)
        
        rspSubFileList = self.__buildSubFileList(outRoot)

        #rspSubFileList = self.__buildLocalSubFileList()
        prepfold_cmd.extend(rspSubFileList)

        self.logger.debug("prepfold command extended: <files>")

        for file in rspSubFileList:
            self.logger.debug(file)

        return prepfold_cmd


        # ------------------- #


    def __buildLocalListCmd(self):

        """
        build the full prepfold command line as an argument list
        """

        self.logger.debug("@ .__buildLocalListCmd()")
        self.logger.debug("building prepfold command for local dir ...")
        self.logger.debug(os.getcwd())

        outRoot      = self.__buildLocalOutRoot()
        prepfold_cmd = [self.executable]

        prepfold_cmd.extend([('-psr')])
        prepfold_cmd.extend([(self.pulsar)])
        self.logger.debug("prepfold command extended: -psr "+self.pulsar)

        if self.inputs['noxwin'] == 'True':
            prepfold_cmd.extend([('-noxwin')])
            self.logger.debug("prepfold command extended: -noxwin")

        if self.inputs['fine']== 'True':
            prepfold_cmd.extend([('-fine')])
            self.logger.debug("prepfold command extended: -fine")

        if self.inputs['nopdsearch']== 'True':
            prepfold_cmd.extend([('-nopdsearch')])
            self.logger.debug("prepfold command extended: -nopdsearch")

        prepfold_cmd.extend([('-n')])
        nperstokes = str(self.inputs["nperstokes"])
        prepfold_cmd.extend([(nperstokes)])
        self.logger.debug("prepfold command extended: -n "+ nperstokes)

        prepfold_cmd.extend([('-o')])
        prepfold_cmd.extend([(outRoot)])
        self.logger.debug("prepfold command extended: -o "+outRoot)
        
        rspSubFileList = self.__buildLocalSubFileList()
        prepfold_cmd.extend(rspSubFileList)

        self.logger.debug("prepfold command extended: <files>")

        for file in rspSubFileList:
            self.logger.debug(file)

        return prepfold_cmd

        # ------------------- #

    def __buildOutRoot(self):

        """ build the output root for prepfold -o option"""

        n = str(self.rspN)
        outRootPath = os.path.join(self.pulsArch,
                                   self.obsid,
                                   self.stokes,
                                   "RSP"+str(n),
                                   self.pulsar+"_"+self.obsid+"_"+"RSP"+n
                                   )
        return outRootPath

        # ------------------- #

    def __buildLocalOutRoot(self):

        """
        build file list and out root locally for operation within an RSP
        directory.
        """
        n = str(self.rspN)
        outRootName = self.pulsar+"_"+self.obsid+"_"+"RSP"+n
        return outRootName

        # ------------------- #

    def __buildSubFileList(self, outRoot):

        """ 
        build a list of all .subnnnn files for input to
        prepfold, excluding .sub.inf, and other potential cruf.
        """

        prepPath = os.path.dirname(outRoot)
        self.logger.debug("RSP"+str(self.rspN)+" prepfold outfile path:")
        self.logger.debug(prepPath)

        allFiles = os.listdir(prepPath)
        rspFiles = []

        for file in allFiles:
            if ".sub.inf" in file:
                continue
            if ".list" in file:
                continue
            if ".tmp" in file:
                continue
            rspFiles.append(os.path.join(prepPath,file))

        return rspFiles

        # ------------------- #

    def __buildLocalSubFileList(self):

        """ local file list."""

        allFiles = os.listdir('.')
        rspFiles = []

        for file in allFiles:
            if ".sub.inf" in file:
                continue
            if ".lis" in file:
                continue
            if ".tmp" in file:
                continue
            rspFiles.append(file)

        return rspFiles

#                                                          LOFAR PULSAR PIPELINE
#
#                                            Data Folding Node (prepfold) recipe
#                                     Pulsar.pipeline.recipes.nodes.prepfold.py
#                                                          Ken Anderson, 2009-10
#                                                            k.r.anderson@uva.nl
# ------------------------------------------------------------------------------

from __future__ import with_statement
from subprocess import Popen, check_call, CalledProcessError, PIPE, STDOUT
from os.path    import splitext

import os, glob

#local helpers
from lofarpipe.support.lofarnode       import LOFARnode
from lofarpipe.support.utilities       import log_time
from lofarpipe.support.lofarexceptions import ExecutableMissing

# PULP libs
import pulpEnv



class prepfold(LOFARnode):
    def run(self, inputs, infiles, obsid, pulsar, arch, userEnv, rspN):

        obsEnv = pulpEnv.PulpEnv(obsid, pulsar, arch, userEnv) 

        self.inputs     = inputs
        self.infiles    = infiles
        self.rspN       = str(rspN)         # int or "A" for RSPA
        self.obsid      = obsEnv.obsid
        self.pulsar     = obsEnv.pulsar
        self.stokes     = obsEnv.stokes
        self.pulsArch   = obsEnv.pArchive
        self.executable = self.inputs['executable']
        
        with log_time(self.logger):
            try:
                os.access(self.executable, os.X_OK)
            except:
                raise ExecutableMissing(self.executable)

            self.logger.debug("Moving to RSP" + self.rspN)
            self.logger.debug("As working directory for prepfold ...")

             #                                                #
             # prepfold forcing a move to the RSP directories #
             #                                                #

            curDir = os.getcwd()
            newDir = os.path.dirname(self.__buildOutRoot())
            os.chdir(newDir)
            self.logger.debug("In RSP"+ self.rspN +" directory NOW")
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

                env = {'TEMPO': obsEnv.TEMPO,
                       'PATH' : obsEnv.TEMPO
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

            try:
                self.convertPlots()
            except Exception, err:
                self.logger.debug("convertPlots failed")
                self.logger.debug(Exception)
                self.logger.debug(err)

            os.chdir(curDir)

            return result



    def convertPlots(self):
        """
        Method called by this node recipe will work on the assigned
        RSP directory.  Method will convert prepfold-generated pfd.ps
        files to .pdf, .png and also produce the thumbnail png file.
        
        Globbing should find only one .ps file, hilariously
        named like,
        
        <pulsar>_<obsid>_RSP{n}_PSR_<pulsar>.pfd.ps
        
        eg., 
        
        B0329+54_L2010_06160_RSP0_PSR_B0329+54.pfd.ps
        
        Sadly, PIL, v1.6.1,  appears unable to handle postscript files.
        """

        psFiles = glob.glob("*.ps")
        convertPrefix = "/usr/bin/convert"
        
        for file in psFiles:
            convertCmd    = [convertPrefix]
            convertCmd.extend([file])
            basename      = splitext(convertCmd[-1])[0]
            convertCmd.extend([basename+".pdf"])
            self.logger.debug("Converting postscript ...")
            self.logger.debug(convertCmd)
            convertProc   = Popen(convertCmd)
            stdOut,stdErr = convertProc.communicate()
            del stdOut, stdErr, basename
            
            convertPngCmd = [convertPrefix]
            convertPngCmd.extend(["-rotate"])
            convertPngCmd.extend(["90"])
            convertPngCmd.extend([file])
            basename      = splitext(convertPngCmd[-1])[0]
            convertPngCmd.extend([basename+".png"])
            self.logger.debug("Converting postscript ...")
            self.logger.debug(convertPngCmd)
            convertProc   = Popen(convertPngCmd)
            stdOut,stdErr = convertProc.communicate()
            del stdOut, stdErr, basename
            
            thumbnailCmd  = [convertPrefix]
            thumbnailCmd.extend(["-rotate"])
            thumbnailCmd.extend(["90"])
            thumbnailCmd.extend(["-crop"])
            thumbnailCmd.extend(["200x140-0"])
            thumbnailCmd.extend([file])
            basename      = splitext(convertCmd[-1])[0]
            thumbnailCmd.extend([basename+".th.png"])
            self.logger.debug("Cutting thumbnail...")
            self.logger.debug(thumbnailCmd)
            thumbnailProc = Popen(thumbnailCmd)
            stdOut,stdErr = thumbnailProc.communicate()
            del stdOut, stdErr, basename

        return

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

        if self.inputs['noxwin'] == True:
            prepfold_cmd.extend([('-noxwin')])
            self.logger.debug("prepfold command extended: -noxwin")

        if self.inputs['fine']== True:
            prepfold_cmd.extend([('-fine')])
            self.logger.debug("prepfold command extended: -fine")

        if self.inputs['nopdsearch']== True:
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

        if self.inputs['noxwin'] == True:
            prepfold_cmd.extend([('-noxwin')])
            self.logger.debug("prepfold command extended: -noxwin")

        if self.inputs['fine']== True:
            prepfold_cmd.extend([('-fine')])
            self.logger.debug("prepfold command extended: -fine")

        if self.inputs['nopdsearch']== True:
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

        outRootPath = os.path.join(self.pulsArch,
                                   self.obsid,
                                   self.stokes,
                                   "RSP"+self.rspN,
                                   self.pulsar+"_"+self.obsid+"_"+"RSP"+self.rspN
                                   )
        return outRootPath

        # ------------------- #

    def __buildLocalOutRoot(self):

        """
        build file list and out root locally for operation within an RSP
        directory.
        """

        outRootName = self.pulsar+"_"+self.obsid+"_"+"RSP"+self.rspN
        return outRootName

        # ------------------- #

    def __buildSubFileList(self, outRoot):

        """ 
        build a list of all .subnnnn files for input to
        prepfold, excluding .sub.inf, and other potential cruf.
        """

        prepPath = os.path.dirname(outRoot)
        self.logger.debug("RSP"+ self.rspN +" prepfold outfile path:")
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

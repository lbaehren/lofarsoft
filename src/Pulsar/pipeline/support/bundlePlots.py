#
#           $Id$

#                                                          LOFAR PULSAR PIPELINE
#
#                                         Pulsar.pipeline.support.bundlePlots.py
#                                                          Ken Anderson, 2010-11
#                                                            k.r.anderson@uva.nl
# ------------------------------------------------------------------------------

# python funcs, mods
from shutil import copyfile
import os, glob
import tarfile

# Pulp libs
import pulpEnv



class BundlePlots():

    """
    Produce a tarball containing various output plots and various post
    production, value-added plots as requested.

    Constructor recieves the directory within which tarring will commence.

    Under a observation output directory usually named like,
    
    L<YYYY>_<OBSID>[_red]/incoherentstokes/   , or possibly,
    L<YYYY>_<OBSID>[_red]/stokes/

    Files searched and tarred:

             '*.inf'
             '*.ps'
             '*.pfd'
             '*.pdf'
             '*.png'
             '*.rfirep'

    """

    def __init__(self, obsid, pulsar, arch, logDir, uEnv):

        """
        This constructor iniitalizes the usual and various paths and
        parameters, as well as defines the matching patterns for the
        grabbable output, defined initially in the pulsar shell script.
        The kinds of file added to the tar archive has been continually
        in flux, and should be expected for sometime yet.
        """

        # get the obsid environment ...
        obsEnv = pulpEnv.PulpEnv(obsid,pulsar,arch,uEnv)

        self.arch      = arch
        self.uEnv      = uEnv
        self.logDir    = logDir
        self.obsid     = obsEnv.obsid
        self.pulsar    = obsEnv.pulsar
        self.stokes    = obsEnv.stokes
        self.pArchive  = obsEnv.pArchive
        self.obsidPath = obsEnv.obsidPath
        self.stokesPath= obsEnv.stokesPath  # where the globbing begins ....
        self.curdir    = os.getcwd()

        self.rspGlobPatts = ['*.inf',
                             '*.ps',
                             '*.pfd',
                             '*.pdf',
                             '*.png',
                             '*.rfirep'
                             ]


    def ballIt(self):

        """
        Builds the tarfile of indicated processed file types.
        """

        os.chdir(self.obsidPath)
        tarPath = os.getcwd()
        tarName = self.pulsar+"_"+self.obsid+"_plots.tar.gz"
        self.generateTarFile(tarName)

        # Copy the pipeline log file from the 'pipeline_runtime' directory
        # as specified in the user's configuration file, pipeline.cfg, to
        # the observation's obsid path.

        newlogname = self.__getPipelineLog()
        os.chdir(self.curdir)
        return tarName


    def __getPipelineLog(self):
        
        """Method gets the pipeline log file, under a user's defined
        pipeline_runtime directory, moves and renames it something useful,
        and returns that new logfile name..
        """

        logfile    = os.path.join(self.logDir,"pipeline.log")
        newlogname = self.pulsar+"_"+self.obsid+"_pulp.log"
        newlogfile = os.path.join(self.obsidPath,newlogname)
        copyfile(logfile,newlogfile)
        return newlogname


    def generateTarFile(self,tarName):

        """ generate the tar file."""

        tarObj  = tarfile.open(tarName,mode='w:gz')
        for patt in self.rspGlobPatts:
            plotFileList = glob.glob(os.path.join(self.stokes+("/RSP?/"+patt)))
            for pfile in plotFileList:
                tarObj.add(pfile)
        tarObj.close()
        return



    def __tarfiles(self,files):

        """ an unused generator ..."""

        for patt in self.rspGlobPatts:
            for testfile in files:
                ftype = os.path.splitext(testfile)[1]
                if ftype == patt:
                    yield testfile
        return




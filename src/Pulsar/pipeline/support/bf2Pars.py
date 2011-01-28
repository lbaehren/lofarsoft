#
# $Id$

#                                                          LOFAR PULSAR PIPELINE
#
#                                             Pulsar.pipeline.support.bf2Pars.py
#                                                          Ken Anderson, 2010-10
#                                                            k.r.anderson@uva.nl
# ------------------------------------------------------------------------------

import os

# Repository info ...
__svn_revision__ = ('$Rev$').split()[1]
__svn_revdate__  = ('$Date$')[7:26]
__svn_author__   = ('$Author$').split()[1]



class bf2Pars():

    """ Class presents bf2presto command line arguments and other parameters
    as instance variables.  Values are stringified for cli.
    
    NOTE: output files from bf2presto are named like,

    <target_name>_<obsid>_RSP[i].sub[0-9][nnn],
    
    though this is dependent upon the of number input subbands.
    
    Below, the output files in our current operational mode
    would be under a directory called 'incoherentstokes' which 
    would lead to a output_base equal to
    
    incoherentstokes/RSP[0123]/<targetName>_<obsid>_RSP[0123],

    eg, under ../incoherentstokes/RSP0, we see files named
        
        B0329+54_L2010_06160_RSP0.sub0000
        B0329+54_L2010_06160_RSP0.sub0002
        B0329+54_L2010_06160_RSP0.sub0003
        B0329+54_L2010_06160_RSP0.sub0004
        ...

    so, in this case, output_base = 'incoherentstokes/RSP0/B0329+54_L2010_06160_RSP0'
    bf2presto produces the name extensions during channelization.

    Constructor receives a pulp environment object (an instance of PulpEnv()).

    """

    def __init__(self, env ):
        """ env: an instance of PulpEnv()
        """
        self.obsid  = env.obsid
        self.pulsar = env.pulsar
        self.arch   = env.arch

        # cmd line args, switches, flags, ...
        self.sb_base  = '0'         # -f  base Subband number
        self.array    = ''          #     "HBA", "LBA", etc.
        self.channels = ''          # -c  n channels 
        self.down     = ''          # -n  n int steps per Stokes
        self.magicNum = ''          # 
        self.Nsamples = ''          # -N  n samples per block (def 768)
        self.RSPlist  = []          # bf2preseto file arguments 

        # parset, output file path

        self.stokes      = env.stokes       # 1 of 'incoherentstokes' or 'raw'
        self.arch_base   = env.pArchive     # archive @ lse018/data4
        self.parsetPath  = env.parsetPath
        self.parsetName  = env.parsetName
        self.transpose2  = env.transpose2

        self.readBFpars()


    def readBFpars(self):
        """

        Read a LOFAR observation parset file for the passed LOFAR obsid.

        This method has been adapted to read 'nominal', re: old, and 'new'
        parameter sets, the kind determined by testParset() and indicated
        by instance variable, self.parsetKind

        Parameters grabbed from  parsets:

           Observation.bandFilter        -- currently not used.
           Observation.channelsPerSubband
           OLAP.Stokes.integrationSteps
           OLAP.CNProc.integrationSteps
           OLAP.BeamsAreTransposed       -- 2nd transpose, 'True' or 'False'

        TBD:
        ===

        -- New parameters for new processing modes

        NR_BEAMS         'Observation.nrBeams'         # <int>, N incoherent beams
        INCOHERENTSTOKES 'OLAP.outputIncoherentStokes' # <bool>,true or false
        COHERENTSTOKES   'OLAP.outputCoherentStokes'   # <bool>,true or false
        STOKES_TYPE      'OLAP.outputIncoherentStokes' # <bool>,true or false
        FLYSEYE          'OLAP.PencilInfo.flysEye'     # <bool>,true or false
        NBEAMS           'OLAP.storageStationNames'    # <list>


        The parameter `OLAP.storageStationNames' will have a value, the form of
        which is written as a list like,

        [CS001HBA0,CS001HBA1,CS002HBA0,CS002HBA1,CS003HBA0, ...]
        
        This will pass in as string that will need to be listified.

        """

        pars = open(self.parsetName).readlines()
        print "PARSET: ",self.parsetName
        for line in pars:
            if ('OLAP.outputIncoherentStokes' in line):
                print "Found! OLAP.outputIncoherentStokes."
                incoherentstokes = self.__extractValue(line)
                if incoherentstokes == 'True':
                    self.stokes = 'incoherentstokes'
                    continue
            if ('OLAP.Stokes.integrationSteps' in line):
                print "Found! OLAP.Stokes.integrationSteps"
                self.down = self.__extractValue(line)
                continue
            if ('OLAP.CNProc.integrationSteps' in line):
                print "Found! OLAP."
                self.magicNum = self.__extractValue(line)
                continue

            # `Observation.*' parameters ...

            if ('Observation.bandFilter' in line):
                print "Found! OLAP.BeamsAreTransposed"
                self.array = line.split('=')[1].split('_')[0].strip()
                continue
            if ('Observation.channelsPerSubband' in line):
                print "Found! OLAP.BeamsAreTransposed"
                self.channels = self.__extractValue(line)
                continue

            # The 'BeamsAreTransposed' boolean parameter may or may not be in the 
            # parset.

            if ('OLAP.BeamsAreTransposed' in line):
                print "found! OLAP.BeamsAreTransposed"
                self.transpose2 = self.__extractValue(line)
                continue
            else:
                self.transpose2 = False
                continue

            if (self.array and self.channels and self.down and self.magicNum):
                print "Got all params.",self.array.self.channels,self.down,self.magicNum
                break

        print "magic Number:",self.magicNum, "\nDOWN:",self.down

        self.Nsamples = int(float(self.magicNum)/float(self.down))
        return


    def show(self):
        """
        Display command line and other parameters, either read or constructed.
        """
        
        print "\n\n\tDisplaying parameters for LOFAR OBSID",self.obsid,"\n\n"
        for key in self.__dict__.keys():
            print "Parameter: ",key, "\t= ",self.__dict__[key]
        print "---------------------"
        return


    def __extractValue(self, line):
        return line.split('=')[1].strip()
        

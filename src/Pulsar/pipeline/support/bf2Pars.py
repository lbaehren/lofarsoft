#! /usr/bin/env python

#                                                          LOFAR PULSAR PIPELINE
#
#                                             Pulsar.pipeline.support.bf2Pars.py
#                                                          Ken Anderson, 2009-10
#                                                            k.r.anderson@uva.nl
# ------------------------------------------------------------------------------

import os


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

    __module__ = __name__

    def __init__(self, env ):
        self.obsid  = env.obsid
        self.pulsar = env.pulsar
        self.arch   = env.arch

        # cmd line args, switches, flags, ...
        self.sb_base  = '1'         # -f  base Subband number
        self.array    = ''          #     "HBA", "LBA", etc.
        self.channels = ''          # -c  n channels 
        self.down     = ''          # -n  n samples per Stokes
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

        INCOHERENTSTOKES 'OLAP.outputIncoherentStokes'
        COHERENTSTOKES   'OLAP.outputCoherentStokes'
        2ND TRANSPOSE    'OLAP.BeamsAreTransposed'
        STOKES_TYPE      'OLAP.outputIncoherentStokes'
        FLYSEYE          'OLAP.PencilInfo.flysEye'
        NBEAMS           'OLAP.storageStationNames'

        """

        pars = open(self.parsetName).readlines()
        for line in pars:

#             if ('OLAP.BeamsAreTransposed' in line):
#                 self.transpose2 = line.split('=')[1].strip()
#                 continue
#             else: self.transpose2 = False

            if ('OLAP.outputIncoherentStokes' in line):
                incoherentstokes = line.split('=')[1].strip()
                if incoherentstokes == 'True':
                    self.stokes = 'incoherentstokes'

            if ('Observation.bandFilter' in line):
                self.array = line.split('=')[1].split('_')[0].strip()
                continue
            if ('Observation.channelsPerSubband' in line):
                self.channels = line.split('=')[1].strip()
                continue
            if ('OLAP.Stokes.integrationSteps' in line):
                self.down = line.split('=')[1].strip()
                continue
            if ('OLAP.CNProc.integrationSteps' in line):
                self.magicNum = line.split('=')[1].strip()
                continue
            if (self.array and self.channels and self.down and self.magicNum):
                break
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

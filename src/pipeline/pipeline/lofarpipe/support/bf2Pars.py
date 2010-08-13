#! /usr/bin/env python

from os.path import join


#arch       = '/net/sub5/lse013/data4/PULSAR_ARCHIVE/'
arch       = '/net/sub5/lse015/data4/PULSAR_ARCHIVE/'
parsetPath = '/globalhome/lofarsystem/log/'
parsetName = 'RTCP.parset.0'
stokes     = 'incoherentstokes'

class bf2Pars():
    """ Class presents bf2presto command line arguments and other parameters
    as instance variables.  Values are stringified for cli.
    
    NOTE: output files from bf2presto are named like,

    <target_name>_<obsid>_RSP[i].sub[nnn],
    
    though this is dependent upon the number input subbands.
    Presently, these output files are dispersed across four (4)
    directories, named RSP[0123].  Below, the output files
    in our current operational mode would be under a directory called
    'incoherentstokes' which would lead to a output_base equal to
    
    incoherentstokes/RSP[0123]/<targetName>_<obsid>_RSP[0123],

    eg, under ../incoherentstokes/RSP0, we see files named
        
        B0329+54_L2010_06160_RSP0.sub0000
        B0329+54_L2010_06160_RSP0.sub0002
        B0329+54_L2010_06160_RSP0.sub0003
        B0329+54_L2010_06160_RSP0.sub0004
        ...

    so, in this case, output_base = 'incoherentstokes/RSP0/B0329+54_L2010_06160_RSP0'
    bf2presto produces the name extensions during channelization.

    """
    __module__ = __name__

    def __init__(self, obsid, target):
        self.obsid    = obsid
        self.target   = target

        # cmd line args, switches, flags, ...
        self.sb_base  = '1'         # -f  base Subband number
        self.array    = ''          #     "HBA", "LBA", etc.
        self.channels = ''          # -c  n channels 
        self.down     = ''          # -n  n samples per Stokes
        self.magicNum = ''          # 
        self.Nsamples = ''          # -N  n samples per block (def 768)
        self.RSPlist  = []          # bf2preseto file arguments 

        # parset, output file path

        self.RSP         = 'RSP'              # Caller must an RSP number (1,2,3, ... etc)
        self.stokes      = stokes
        self.arch_base   = arch
        self.parsetPath  = parsetPath
        self.parsetName  = parsetName

        # output_base will be the full "head" of archive path, i.e. everything but
        # the file extension.

        # Caller must an RSP number (1,2,3, ... etc)
        output_path  = join(self.arch_base, self.obsid, self.stokes, self.RSP)
        self.headout = join(output_path, self.target + '_' + self.obsid + '_' + self.RSP)


    def readParset(self):
        """
        Read a LOFAR observation parset file for the passed LOFAR obsid.
        Parameters grabbed:

        Observation.bandFilter -- currently not used.
        Observation.channelsPerSubband
        Observation.ObservationControl.OnlineControl.OLAP.Stokes.integrationSteps
        OLAP.CNProc.integrationSteps
        """

        parsetFile = join(self.parsetPath, self.obsid, self.parsetName)
        pars = open(parsetFile).readlines()
        for line in pars:

            if ('Observation.bandFilter' in line):
                self.array = line.split('=')[1].split('_')[0].strip()

            if ('Observation.channelsPerSubband' in line):
                self.channels = line.split('=')[1].strip()

            if ('"OLAP.Stokes.integrationSteps' in line):
                self.down = line.split('=')[1].strip()
            if ("Observation.ObservationControl.OnlineControl.OLAP.Stokes.integrationSteps" in line):
                self.down = line.split('=')[1].strip()

            if ('OLAP.CNProc.integrationSteps' in line):
                self.magicNum = line.split('=')[1].strip()

            #if ('OLAP.PencilInfo.flysEye' in line):
            #    self.flyseye = line.split('=').strip()

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

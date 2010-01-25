#!/usr/bin/env python

import os,sys,string, math
from optparse import OptionParser


class pardata:
    def __init__(self, filenm):
        for line in open(filenm):
            if line.startswith("Observation.Beam[0].angle1"):
                self.rar = line.split("=")[-1].strip()
                continue
            if line.startswith("Observation.Beam[0].angle2"):
                self.decr = line.split("=")[-1].strip()
                continue
            if line.startswith("Observation.sampleClock"):
                self.clock = line.split("=")[-1].strip()
                continue
            if line.startswith("Observation.startTime"):
                self.startTime = line.split("=")[-1].strip()[1:-1]
                continue
            if line.startswith("Observation.stopTime"):
                self.stopTime = line.split("=")[-1].strip()[1:-1]
                continue
            if line.startswith("Observation.bandFilter"):
                self.bandFilter = line.split("=")[-1].strip()
                continue
            if line.startswith("Observation.subbandList"):
                self.subbandFirst = float(line.split("=")[-1].strip()[1:4])
                self.subbandLast = float(line.split(",")[-1].strip()[0:3])
                continue
            if line.startswith("Observation.channelsPerSubband"):
                self.channelsPerSubband = line.split("=")[-1].strip()
                continue
            if line.startswith("OLAP.Stokes.integrationSteps"):
                self.integrationSteps = float(line.split("=")[-1].strip())
                continue
            if line.startswith("OLAP.CNProc.integrationSteps"):
                self.blocksperStokes = line.split("=")[-1].strip()
                continue
            self.pulsarsrc = ""
            if line.startswith("Observation.pulsarsrc"):
                self.pulsarsrc = line.split("=")[-1].strip()
                continue



class infodata:
    def __init__(self, filenm):
        self.breaks = 0
        self.notes = "Notes: "
        for line in open(filenm):
            if line.startswith(" Data file name"):
                self.basenm = line.split("=")[-1].strip()
                continue
            if line.startswith(" Telescope"):
                self.telescope = line.split("=")[-1].strip()
                continue
            if line.startswith(" Instrument"):
                self.instrument = line.split("=")[-1].strip()
                continue
            if line.startswith(" Object being observed"):
                self.object = line.split("=")[-1].strip()
                continue
            if line.startswith(" J2000 Right Ascension"):
                self.RA = line.split("=")[-1].strip()
                continue
            if line.startswith(" J2000 Declination"):
                self.DEC = line.split("=")[-1].strip()
                continue
            if line.startswith(" Data observed by"):
                self.observer = line.split("=")[-1].strip()
                continue
            if line.startswith(" Epoch"):
                self.epoch = float(line.split("=")[-1].strip())
                continue
            if line.startswith(" Barycentered?"):
                self.bary = int(line.split("=")[-1].strip())
                continue
            if line.startswith(" Number of bins"):
                self.N = int(line.split("=")[-1].strip())
                continue
            if line.startswith(" Width of each time series bin"):
                self.dt = float(line.split("=")[-1].strip())
                continue
            if line.startswith(" Any breaks in the data?"):
                self.breaks = int(line.split("=")[-1].strip())
                if self.breaks:
                    self.onoff = []
                continue
            if line.startswith(" On/Off bin pair"):
                vals = line.split("=")[-1].strip().split(",")
                self.onoff.append((int(vals[0]), int(vals[1])))
                continue
            if line.startswith(" Type of observation"):
                self.waveband = line.split("=")[-1].strip()
                continue
            if line.startswith(" Beam diameter"):
                self.beam_diam = float(line.split("=")[-1].strip())
                continue
            if line.startswith(" Dispersion measure"):
                self.DM = float(line.split("=")[-1].strip())
                continue
            if line.startswith(" Central freq of low channel"):
                self.lofreq = float(line.split("=")[-1].strip())
                continue
            if line.startswith(" Total bandwidth"):
                self.BW = float(line.split("=")[-1].strip())
                continue
            if line.startswith(" Number of channels"):
                self.numchan = int(line.split("=")[-1].strip())
                continue
            if line.startswith(" Channel bandwidth"):
                self.chan_width = float(line.split("=")[-1].strip())
                continue
            if line.startswith(" Data analyzed by"):
                self.analyzer = line.split("=")[-1].strip()
                continue

    def printInf(self):
        print ""
        print " Data file name without suffix          =  ",self.basenm
        print " Telescope used                         =  ",self.telescope
        print " Instrument used                        =  ",self.instrument
        print " Object being observed                  =  ",self.object
        print " J2000 Right Ascension (hh:mm:ss.ssss)  =  ",self.RA
        print " J2000 Declination     (dd:mm:ss.ssss)  =  ",self.DEC
        print " Data observed by                       =  ",self.observer
        print " Epoch of observation (MJD)             =  ",self.epoch
        print " Barycentered?           (1=yes, 0=no)  =  ",self.bary
        print " Number of bins in the time series      =  ",self.N
        print " Width of each time series bin (sec)    =  ",self.dt
        print " Any breaks in the data? (1=yes, 0=no)  =  ",self.breaks
        print " Type of observation (EM band)          =  ",self.waveband
        print " Beam diameter (arcsec)                 =  ",self.beam_diam
        print " Dispersion measure (cm-3 pc)           =  ",self.DM
        print " Central freq of low channel (MHz)      =  ",self.lofreq
        print " Total bandwidth (MHz)                  =  ",self.BW
        print " Number of channels                     =  ",self.numchan
        print " Channel bandwidth (MHz)                =  ",self.chan_width
        print " Data analyzed by                       =  ",self.analyzer
        print " Any additional notes:                     "
        print " ",self.notes

    def writeInf(self,ofile):
        ofile.write("\n")
        ofile.write(" Data file name without suffix          =  %s\n"%self.basenm)
        ofile.write(" Telescope used                         =  %s\n"%self.telescope)
        ofile.write(" Instrument used                        =  %s\n"%self.instrument)
        ofile.write(" Object being observed                  =  %s\n"%self.object)
        ofile.write(" J2000 Right Ascension (hh:mm:ss.ssss)  =  %s\n"%self.RA)
        ofile.write(" J2000 Declination     (dd:mm:ss.ssss)  =  %s\n"%self.DEC)
        ofile.write(" Data observed by                       =  %s\n"%self.observer)
        ofile.write(" Epoch of observation (MJD)             =  %s\n"%self.epoch)
        ofile.write(" Barycentered?           (1=yes, 0=no)  =  %s\n"%self.bary)
        ofile.write(" Number of bins in the time series      =  %s\n"%self.N)
        ofile.write(" Width of each time series bin (sec)    =  %s\n"%self.dt)
        ofile.write(" Any breaks in the data? (1=yes, 0=no)  =  %s\n"%self.breaks)
        ofile.write(" Type of observation (EM band)          =  %s\n"%self.waveband)
        ofile.write(" Beam diameter (arcsec)                 =  %s\n"%self.beam_diam)
        ofile.write(" Dispersion measure (cm-3 pc)           =  %s\n"%self.DM)
        ofile.write(" Central freq of low channel (MHz)      =  %s\n"%self.lofreq)
        ofile.write(" Total bandwidth (MHz)                  =  %s\n"%self.BW)
        ofile.write(" Number of channels                     =  %s\n"%self.numchan)
        ofile.write(" Channel bandwidth (MHz)                =  %s\n"%self.chan_width)
        ofile.write(" Data analyzed by                       =  %s\n"%self.analyzer)
        ofile.write(" Any additional notes:                     \n")
        ofile.write(" %s\n"%self.notes)

parser = OptionParser()


parser.add_option("-S", "--source", action="store", type="string",
		  dest="source",   help="Add or change the name of the source")
parser.add_option("-U", "--user", action="store", type="string",
		  dest="user",   help="Add or change the name of the person analysing")
parser.add_option("-o", "--ouput",action="store",type="string",dest="output",
                  help="Give output file name for .inf file (without the .inf)")
parser.add_option("-l", "--low_subband",action="store",type="int",dest="low_subband",
                  help="The lowest subband number to use if don't want them all")
parser.add_option("-u", "--upper_subband",action="store",type="int",dest="upper_subband",
                  help="The lowest subband number to use if don't want them all")
parser.add_option("-n", "--nsubbands", action="store", type="int",
		  dest="nsubbands", help="The number of subbands to be included in the inf file")
parser.add_option("-r", "--repeat",action="store",type="int",dest="repeat",
                  help="If producing multiple inf files how many times to repeat\n the number of subbands requested with -n option")

(options, args) = parser.parse_args()

filename = args[len(args)-1]
id = infodata("default.inf")
par = pardata(filename)


# Checking on command line options
if ( options.repeat and not options.nsubbands ):
    print "If you use the repeat option you must specify the number of subbands"
    print "That there should be in each repeat cycle using the -n option."
    exit(-1)
    
if ( options.low_subband ):
    if options.upper_subband:
        if options.low_subband > options.upper_subband:
            print "The lowest subband %d is higher than the highest subband %d"%(options.low_subband,options.upper_subband)
            exit(-1)
        id.numchan = options.upper_subband - options.low_subband + 1
        if options.nsubbands:
            if options.nsubbands != id.numchan:
                print "Specified more/less subbands than given by upper and lower limits"
                exit(-1)


# Change the source name
if (par.pulsarsrc):
    id.object = options.source
if (options.source):
    id.object = options.source


# Change the name of the person analysing the data.
if (options.user):
    id.analyzer = options.user

#  Calculate the RA and Dec of the source
rad = float(par.rar) * 180.0 / math.pi
rah = int(24.0 * rad / 360.0)
ram = int((24.0 * rad / 360.0 - rah) * 60.0)
ras = 60.0*(((24.0 * rad / 360.0 - rah) * 60.0) - ram)
id.RA = "%02d:%02d:%f" % (rah,ram,ras)

decd = float(par.decr) * 180.0 / math.pi
decdi = int(decd)
decm  = int((decd - decdi) * 60.0)
decs  = (((decd - decdi) * 60.0) - decm)*60.0
id.DEC = "%02d:%02d:%f" % (decdi,decm,decs)

# Calculate the MJD of the observation.

y = float(par.startTime[0:4])
m = float(par.startTime[5:7])
d = float(par.startTime[8:10])

hour = float(par.startTime[11:13])
min = float(par.startTime[14:16])
sec = float(par.startTime[17:19])

sec_mid = hour * 3600.0 + min * 60.0 + sec

a = int(7*(y+int((m+9)/12))/4)
jd = (367.0 * y) - a - int(3*(int((y+(m-9)/7)/100)+1)/4) + int(275*m/9) + d + 1721028.5
id.epoch = (float(jd) - 2400000.5) + sec_mid / 86400


# Calculate frequencies, bandwidths.
band = par.bandFilter[0:3]
if ( band == "HBA"):
    lowerBandFreq = float(par.bandFilter[4:7])
else:
    lowerBandFreq = float(par.bandFilter[4:6])

lowerBandEdge = 100.0
nsubbands = 512

if options.low_subband:
    par.subbandFirst = options.low_subband

if options.upper_subband:
    par.subbandLast = options.upper_subband

if options.nsubbands:
    par.subbandLast = par.subbandFirst + options.nsubbands - 1
    id.numchan = options.nsubbands * int(par.channelsPerSubband)


if (lowerBandFreq < 150.0 and lowerBandFreq > 100.0 and par.clock == "200"):
    Nyquist = 2
    bw = 100.0
    id.chan_width = bw / nsubbands / int(par.channelsPerSubband)
    lowerBandEdge = 100.0
elif (lowerBandFreq < 40.0 and par.clock == "200"):
    Nyquist = 1
    bw = 100.0
    id.chan_width = bw / nsubbands / int(par.channelsPerSubband)
    lowerBandEdge = 0.0

subband_width = bw / nsubbands

# Add some things to the Notes area
samplesPerBlock = float(par.blocksperStokes)  / par.integrationSteps
id.notes = id.notes + "Blocks per Stokes: " + par.blocksperStokes
id.notes = id.notes + "   Samples per block:   " + str(int(samplesPerBlock))

if options.repeat:
    rloop = 0
    while rloop < options.repeat:
        id.lofreq = lowerBandEdge + (float(par.subbandFirst) -0.5) * subband_width
        id.BW = (lowerBandEdge  + (float(par.subbandLast)+ 0.5) * subband_width ) - id.lofreq
        id.dt = par.integrationSteps / (1000000 * id.chan_width)
# Print out the inf file.

        if options.output:
            ofilename = options.output + str(par.subbandFirst) + ".inf"
            ofile=open(ofilename,'w')
            id.writeInf(ofile)
        else:
            id.printInf()
        rloop += 1
        par.subbandFirst = par.subbandFirst + 1 + options.nsubbands
        par.subbandLast = par.subbandLast + 1 + options.nsubbands
else:    
    id.lofreq = lowerBandEdge + (float(par.subbandFirst) -0.5) * subband_width
    id.BW = (lowerBandEdge  + (float(par.subbandLast) +0.5 )* subband_width ) - id.lofreq
    id.dt = par.integrationSteps / (1000000 * id.chan_width)
    
# Print out the inf file.

    if options.output:
        ofile=open(options.output,'w')
        id.writeInf(ofile)
    else:
        id.printInf()

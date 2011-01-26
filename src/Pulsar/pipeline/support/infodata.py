#! /usr/bin/env python

#                                                          LOFAR PULSAR PIPELINE
#
#                                            Pulsar.pipeline.support.infodata.py
#                                                          Ken Anderson, 2009-10
#                                                            k.r.anderson@uva.nl
# ------------------------------------------------------------------------------

import os,string, math
import pardata

class InfoData:
    """ 
    rspBands is a list of lists of RSP groups, one list for each grouping
    based on splitting of a full observation subband set by <filefactor>.
    These lists will be used to determine various frequency parameters for
    a given RSP group.
    """

    def __init__(self, infFile, pulsar, rspLists):
        self.infFile  = os.path.expanduser(infFile)
        self.breaks   = 0
        self.notes    = "Notes: "
        self.pulsar   = pulsar
        self.rspLists = rspLists
        self.nrsp     = len(rspLists)


    def readInf(self):
        for line in open(self.infFile).readlines():
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
            else: continue
            return


    def writeInf(self,outInfFile):
        infFileLines = self.__buildInf()
        newfile = open(outInfFile,'w')
        for line in infFileLines:
            newfile.write(line)
        newfile.close()
        del newfile
        return


    def calcInfPars(self, parSet, obsid, stokesPath):
        """
        Method performs the parameter calculations as done by the scripty par2inf.py

        """

        # _______________________________________ #
        #  Read <obsid> RTCP.parset.0 parameters  #

        par = pardata.ParData(parSet)
        par.readParSet()

        # _______________________________________ #
        # Calculate the RA and Dec of the source  #

        rad = float(par.rar) * 180.0 / math.pi
        rah = int(24.0 * rad / 360.0)
        ram = int((24.0 * rad / 360.0 - rah) * 60.0)
        ras = 60.0*(((24.0 * rad / 360.0 - rah) * 60.0) - ram)
        self.RA = "%02d:%02d:%f" % (rah,ram,ras)

        decd = float(par.decr) * 180.0 / math.pi
        decdi = int(decd)
        decm  = int((decd - decdi) * 60.0)
        decs  = (((decd - decdi) * 60.0) - decm)*60.0
        self.DEC = "%02d:%02d:%f" % (decdi,decm,decs)

        # _______________________________________ #
        # Calculate the MJD of the observation.   #
        
        y = float(par.startTime[0:4])
        m = float(par.startTime[5:7])
        d = float(par.startTime[8:10])
        
        hour = float(par.startTime[11:13])
        min = float(par.startTime[14:16])
        sec = float(par.startTime[17:19])
        
        sec_mid = hour * 3600.0 + min * 60.0 + sec
        
        a = int(7*(y+int((m+9)/12))/4)
        jd = (367.0 * y) - a - int(3*(int((y+(m-9)/7)/100)+1)/4) + int(275*m/9) + d + 1721028.5
        self.epoch = (float(jd) - 2400000.5) + sec_mid / 86400

        # _______________________________________ #
        #    Calculate frequencies, bandwidths.   #

        bw            = 100.0                       # bandwidth setting (from par2inf)
        lowerBandEdge = 100.0
        nsubbands     = 512
        band          = par.bandFilter[0:3]
        subband_width = bw / nsubbands

        self.object = self.basenm = self.pulsar

        if ( band == "HBA"):
            lowerBandFreq = float(par.bandFilter[4:7])
        else:
            lowerBandFreq = float(par.bandFilter[4:6])

        par.subbandLast = par.subbandFirst + nsubbands - 1
        
        if (lowerBandFreq < 150.0 and lowerBandFreq > 100.0 and par.clock == "200"):
            bw = 100.0
            self.chan_width = bw / nsubbands / int(par.channelsPerSubband)
            lowerBandEdge = 100.0
        elif (lowerBandFreq < 40.0 and par.clock == "200"):
            bw = 100.0
            self.chan_width = bw / nsubbands / int(par.channelsPerSubband)
            lowerBandEdge = 0.0

        # _______________________________________ #
        # Add some things to the Notes area       #

        samplesPerBlock = float(par.blocksperStokes)  / par.integrationSteps
        self.notes  = self.notes + "Blocks per Stokes: " + par.blocksperStokes
        self.notes  = self.notes + "   Samples per block:   " + str(int(samplesPerBlock))

        # _______________________________________ #
        #      Write the .inf files               #

        for i in range(self.nrsp):
            nRSPFiles     = len(self.rspLists[i])
            self.numchan  = len(self.rspLists[i]) * int(par.channelsPerSubband)
            self.lofreq = lowerBandEdge  + (float(par.subbandFirst)- 0.5) * subband_width
            self.BW     = self.numchan * self.chan_width
            self.dt     = par.integrationSteps / (1000000 * self.chan_width)
            
            print "got logreq",self.lofreq
            print "got RSP bandwidth.",self.BW
            print "got dt",self.dt

            outFile = os.path.join(stokesPath,"RSP"+str(i),self.pulsar+"_"+obsid+"_"+"RSP"+str(i)+".sub.inf")

            self.__buildInf()
            self.writeInf(outFile)

            par.subbandFirst = par.subbandFirst + nRSPFiles
            par.subbandLast  = par.subbandLast  + nRSPFiles
            print "got new subbandFirst and last,", par.subbandFirst,par.subbandLast

        return


    def __getInfVal(self, line):
        return line.split("=")[-1].strip()


    def __buildInf(self):

        infFileLines = []
        infFileLines.append("\n")
        infFileLines.append(" Data file name without suffix          =  %s\n"%self.basenm)
        infFileLines.append(" Telescope used                         =  %s\n"%self.telescope)
        infFileLines.append(" Instrument used                        =  %s\n"%self.instrument)
        infFileLines.append(" Object being observed                  =  %s\n"%self.object)
        infFileLines.append(" J2000 Right Ascension (hh:mm:ss.ssss)  =  %s\n"%self.RA)
        infFileLines.append(" J2000 Declination     (dd:mm:ss.ssss)  =  %s\n"%self.DEC)
        infFileLines.append(" Data observed by                       =  %s\n"%self.observer)
        infFileLines.append(" Epoch of observation (MJD)             =  %s\n"%self.epoch)
        infFileLines.append(" Barycentered?           (1=yes, 0=no)  =  %s\n"%self.bary)
        infFileLines.append(" Number of bins in the time series      =  %s\n"%self.N)
        infFileLines.append(" Width of each time series bin (sec)    =  %s\n"%self.dt)
        infFileLines.append(" Any breaks in the data? (1=yes, 0=no)  =  %s\n"%self.breaks)
        infFileLines.append(" Type of observation (EM band)          =  %s\n"%self.waveband)
        infFileLines.append(" Beam diameter (arcsec)                 =  %s\n"%self.beam_diam)
        infFileLines.append(" Dispersion measure (cm-3 pc)           =  %s\n"%self.DM)
        infFileLines.append(" Central freq of low channel (MHz)      =  %s\n"%self.lofreq)
        infFileLines.append(" Total bandwidth (MHz)                  =  %s\n"%self.BW)
        infFileLines.append(" Number of channels                     =  %s\n"%self.numchan)
        infFileLines.append(" Channel bandwidth (MHz)                =  %s\n"%self.chan_width)
        infFileLines.append(" Data analyzed by                       =  %s\n"%self.analyzer)
        infFileLines.append(" Any additional notes:                     \n")
        infFileLines.append(" %s\n"%self.notes)
        return infFileLines

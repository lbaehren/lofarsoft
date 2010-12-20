#!/usr/bin/env python

#                                                          LOFAR PULSAR PIPELINE
#
#                                         Pulsar.pipeline.support.foldingData.py
#                                                          Ken Anderson, 2009-10
#                                                            k.r.anderson@uva.nl
# ------------------------------------------------------------------------------

import os,string,math
import fnmatch, glob
import pardata

class FoldingData():
    """ 
    rspBands is a list of lists of RSP groups, one list for each grouping
    based on splitting of a full observation subband set by <filefactor>.
    These lists will be used to determine various frequency parameters for
    a given RSP group.
    """

    def __init__(self, infFile, pulsar, rspLists):
        self.infFile    = os.path.expanduser(infFile)
        self.breaks     = 0
        self.notes      = "Notes: "
        self.pulsar     = pulsar
        self.rspLists   = rspLists
        self.nrsp       = len(rspLists)


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
        self.RA = "%02d:%02d:%07.4f" % (rah,ram,ras)

        decd = float(par.decr) * 180.0 / math.pi
        decdi = int(decd)
        decm  = int((decd - decdi) * 60.0)
        decs  = (((decd - decdi) * 60.0) - decm)*60.0
        self.DEC = "%02d:%02d:%07.4f" % (decdi,decm,decs)

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

        # ________________________________________________________________ #
        #    Calculate frequencies, bandwidths.                            #

        bw            = 100.0             # bandwidth setting (from par2inf)
        lowerBandEdge = 100.0
        nsubbands     = 512
        band          = par.bandFilter[0:3]
        subband_width = bw / nsubbands
        # ________________________________________________________________ #

        if ( band == "HBA"):
            lowerBandFreq = float(par.bandFilter[4:7])
        else:
            lowerBandFreq = float(par.bandFilter[4:6])

        par.subbandLast = par.subbandFirst + nsubbands - 1
        
        if (lowerBandFreq < 150.0 and lowerBandFreq > 100.0 and par.clock == "200"):
            self.chan_width = bw / nsubbands / int(par.channelsPerSubband)
            lowerBandEdge   = 100.0

        elif (lowerBandFreq < 40.0 and par.clock == "200"):
            self.chan_width = bw / nsubbands / int(par.channelsPerSubband)
            lowerBandEdge   = 0.0

        # _______________________________________ #
        # Add some things to the Notes area       #

        samplesPerBlock = float(par.blocksperStokes)  / par.integrationSteps
        self.notes  = self.notes + "Blocks per Stokes: " + par.blocksperStokes
        self.notes  = self.notes + "   Samples per block:   " + str(int(samplesPerBlock))

        # _______________________________________ #
        # final adjustments ...

        self.object   = self.pulsar
        self.analyzer = os.environ['USER']

        # _______________________________________ #
        # Write the .inf files                    #

        for i in range(self.nrsp):
            rspDir    = os.path.join(stokesPath,"RSP"+str(i))
            outFile   = os.path.join(rspDir,self.pulsar+"_"+obsid+"_"+"RSP"+str(i)+".sub.inf")
            nRSPFiles = len(self.rspLists[i])

            self.numchan= len(self.rspLists[i]) * int(par.channelsPerSubband)
            self.lofreq = lowerBandEdge  + (float(par.subbandFirst)- 0.5) * subband_width
            self.BW     = self.numchan * self.chan_width
            self.dt     = par.integrationSteps / (1000000 * self.chan_width)
            self.N      = self.__getNBinVal(rspDir)
            
            self.__buildInf()
            self.writeInf(outFile)

            par.subbandFirst = par.subbandFirst + nRSPFiles
            par.subbandLast  = par.subbandLast  + nRSPFiles



        # _______________________________________ #
        #        Handle the RSPA directory        #

        # reset initial par-inf values ...

        reset_lowerBandEdge,reset_subbandFirst = self.__resetPars(parSet)
        rspADir      = os.path.join(stokesPath,"RSPA")
        if os.path.isdir(rspADir):
            outAFile     = os.path.join(rspADir,self.pulsar+"_"+obsid+"_"+"RSPA.sub.inf")
            nRSPAFiles   = glob.glob(rspADir+"/*RSPA.sub????")

            self.lofreq  = reset_lowerBandEdge + (float(reset_subbandFirst)- 0.5) * subband_width
            self.numchan = len(nRSPAFiles)
            self.BW      = self.numchan * self.chan_width

            self.__buildInf()
            self.writeInf(outAFile)
        else: pass

        return


    def __resetPars(self, parset):

        """ Reset the par values for RSPA."""

        par = pardata.ParData(parset)
        par.readParSet()
        subbandFirst = par.subbandFirst

        band = par.bandFilter[0:3]
        lowerBandEdge = 100.0
        nsubbands     = 512

        if ( band == "HBA"):
            lowerBandFreq = float(par.bandFilter[4:7])
        else:
            lowerBandFreq = float(par.bandFilter[4:6])          

        if (lowerBandFreq < 150.0 and lowerBandFreq > 100.0 and par.clock == "200"):
            bw            = 100.0
            Nyquist       = 2
            lowerBandEdge = 100.0
            chan_width    = bw / nsubbands / int(par.channelsPerSubband)

        elif (lowerBandFreq < 40.0 and par.clock == "200"):
            bw            = 100.0
            Nyquist       = 1
            lowerBandEdge = 0.0
            chan_width    = bw / nsubbands / int(par.channelsPerSubband)

        return (lowerBandEdge,subbandFirst)


    def __getNBinVal(self,rspDir):
        
        # "Number of bins in the time series", featured here as self.N, is dtermined
        # by taking stat on a .subnnnn file and scraping the size in bytes, dividing
        # two to get the number of actual time bins in the interval.
        
        subfiles = os.listdir(rspDir)
        for subf in subfiles:
            if fnmatch.fnmatch(subf,"*RSP?.sub0000"):
                statThisFile=os.path.join(rspDir,subf)
                bigN = (os.stat(statThisFile)[6])/2
                break
        return bigN


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

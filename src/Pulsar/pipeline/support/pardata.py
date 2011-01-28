#
#           $Id$

#                                                          LOFAR PULSAR PIPELINE
#
#                                             Pulsar.pipeline.support.pardata.py
#                                                          Ken Anderson, 2010-10
#                                                            k.r.anderson@uva.nl
# ------------------------------------------------------------------------------

import string
from os.path import expanduser

# Repository info ...
__svn_revision__ = ('$Rev$').split()[1]
__svn_revdate__  = ('$Date$')[7:26]
__svn_author__   = ('$Author$').split()[1]


class ParData:
    def __init__(self, parSet):

        self.parSet  = expanduser(parSet)

    def readParSet(self):

        for line in open(self.parSet):
            if line.startswith("Observation.Beam[0].angle1"):
                self.rar = self.__getParVal(line)
                continue
            if line.startswith("Observation.Beam[0].angle2"):
                self.decr = self.__getParVal(line)
                continue
            if line.startswith("Observation.sampleClock"):
                self.clock = self.__getParVal(line)
                continue
            if line.startswith("Observation.startTime"):
                self.startTime = self.__getParVal(line)[1:-1]
                continue
            if line.startswith("Observation.stopTime"):
                self.stopTime = self.__getParVal(line)[1:-1]
                continue
            if line.startswith("Observation.bandFilter"):
                self.bandFilter = self.__getParVal(line)
                continue
            if line.startswith("Observation.Beam[0].subbandList") or \
                    line.startswith("Observation.subbandList"):
                # for CSV list or ".." ranges
                try:
                    self.subbandFirst = float(self.__getParVal(line).split("..")[0][1:])
                except: 
                    self.subbandFirst = float(self.__getParVal(line)[1:4])               
                try:
                    self.subbandLast = float(self.__getParVal(line).split("..")[1][:-1])
                except:
                    self.subbandLast = float(line[-5:-2])
                continue

            if line.startswith("Observation.channelsPerSubband"):
                self.channelsPerSubband = self.__getParVal(line)
                continue
            if line.startswith("OLAP.Stokes.integrationSteps"):
                self.integrationSteps = float(self.__getParVal(line))
                continue
            if line.startswith("OLAP.CNProc.integrationSteps"):
                self.blocksperStokes = self.__getParVal(line)
                continue
            self.pulsarsrc = ""
            if line.startswith("Observation.Beam[0].target"):
                self.pulsarsrc = self.__getParVal(line)
                continue
            else: continue
        return


    def __getParVal(self, line):
        return line.split("=")[-1].strip()

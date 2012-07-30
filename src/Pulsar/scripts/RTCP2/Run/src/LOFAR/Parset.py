#!/usr/bin/python
import sys,os

# allow ../util to be found, a bit of a hack
sys.path += [(os.path.dirname(__file__) or ".")+"/.."]

import datetime
import time
import socket
import util.Parset
import getpass
import os
from itertools import count
from Partitions import PartitionPsets
from Locations import Hosts,Locations
from Stations import Stations
from RingCoordinates import RingCoordinates
from util.dateutil import parse,format,parseDuration,timestamp
from logging import error,warn
import math
from sets import Set

# if True, reroute all traffic from first IO node to all but the last storage node,
# and the rest to the last storage node
PERFORMANCE_TEST = False

NRRSPBOARDS=4
NRBOARBEAMLETS=61

NRCVSTOKES=2 # 2: X and Y, 4: Xr, Xi, Yr, Yi

class Parset(util.Parset.Parset):
    def __init__(self):
        util.Parset.Parset.__init__(self)

	self.stations = []
	self.storagenodes = []
	self.partition = ""
	self.psets = []

        self.filename = ""

    def setFilename( self, filename ):
        self.filename = filename

    def getFilename( self ):
        return self.filename

    def save(self):
        self.writeFile( self.filename )

    def distillStations(self, key="Observation.VirtualInstrument.stationList"):
        """ Distill station names to use from the parset file and return them. """

        if key not in self:
          return "+".join(self.get('OLAP.storageStationNames',""))

        # translate station name + antenna set to CEP comprehensable names
        antennaset = self["Observation.antennaSet"]

        def applyAntennaSet( station, antennaset ):
          if antennaset == "":
            # useful for manually entered complete station names like CS302HBA1
            suffix = [""]
          elif antennaset in ["LBA_INNER","LBA_OUTER","LBA_X","LBA_Y","LBA_SPARSE_EVEN","LBA_SPARSE_ODD"]:
            suffix = ["LBA"]
          elif station.startswith("CS"):
            if antennaset == "HBA_ZERO":
              suffix = ["HBA0"]
            elif antennaset == "HBA_ONE":  
              suffix = ["HBA1"]
            elif antennaset == "HBA_JOINED":  
              suffix = ["HBA"]
            else: 
              assert antennaset == "HBA_DUAL", "Unknown antennaSet: %s" % (antennaset,)
              suffix = ["HBA0","HBA1"]
          else:  
            suffix = ["HBA"]

          return "+".join(["%s%s" % (station,s) for s in suffix])

        return "+".join( [applyAntennaSet(s, self["Observation.antennaSet"]) for s in self[key]] )

    def distillPartition(self, key="OLAP.CNProc.partition"):
        """ Distill partition to use from the parset file and return it. """

        if key not in self:
          return ""

        return self[key]

    def distillStorageNodes(self, key="OLAP.Storage.hosts"):
        """ Distill storage nodes to use from the parset file and return it. """

        if key in self:
          return self.getStringVector(key)
  
        outputnames = ["Filtered","Correlated","Beamformed","IncoherentStokes","CoherentStokes","Trigger"]
        locationkeys = ["Observation.DataProducts.Output_%s.locations" % p for p in outputnames]

        storagenodes = set()

        for k in locationkeys:
          if k not in self:
            continue

          locations = self.getStringVector(k)
          hostnames = [x.split(":")[0] for x in locations]
          storagenodes.update( set(hostnames) )

        return list(storagenodes)

    def postRead(self):
        """ Distill values for our internal variables from the parset. """

        # partition
        partition = self.distillPartition()
        if partition:
          self.setPartition( partition )

        # storage nodes
        storagenodes = self.distillStorageNodes() or []
        self.setStorageNodes( storagenodes )

        # stations
        stationStr = self.distillStations()
        if stationStr:
          stationList = Stations.parse( stationStr )
          self.setStations( stationList )

    def addMissingKeys(self):
        """ Sets some default values which SAS does not yet contain. """

        # meta data
        self.setdefault("Observation.ObserverName","unknown")
        self.setdefault("Observation.ProjectName","unknown")

        self.setdefault("Observation.DataProducts.Output_Trigger.enabled",False)

        if 'OLAP.PPF' in self:
          if not self['OLAP.PPF']:
            self["Observation.channelsPerSubband"] = 1
        else:  
          self['OLAP.PPF'] = int(self["Observation.channelsPerSubband"]) > 1

        for k in ["OLAP.CNProc_CoherentStokes.channelsPerSubband", "OLAP.CNProc_IncoherentStokes.channelsPerSubband"]:
          if k not in self or int(self[k]) == 0:
            self[k] = self["Observation.channelsPerSubband"]
        self.setdefault('Observation.DataProducts.Output_Filtered.namemask','L${OBSID}_SB${SUBBAND}.filtered')
        self.setdefault('Observation.DataProducts.Output_Beamformed.namemask','L${OBSID}_SAP${SAP}_B${BEAM}_S${STOKES}_P${PART}_bf.raw')
        self.setdefault('Observation.DataProducts.Output_Correlated.namemask','L${OBSID}_SB${SUBBAND}_uv.MS')
        self.setdefault('Observation.DataProducts.Output_CoherentStokes.namemask','L${OBSID}_SAP${SAP}_B${BEAM}_S${STOKES}_P${PART}_bf.raw')
        self.setdefault('Observation.DataProducts.Output_IncoherentStokes.namemask','L${OBSID}_SB${SUBBAND}_bf.incoherentstokes')
        self.setdefault('Observation.DataProducts.Output_Trigger.namemask','L${OBSID}_SAP${SAP}_B${BEAM}_S${STOKES}_P${PART}_bf.trigger')
	self.setdefault('OLAP.dispersionMeasure', 0);

        self.setdefault('Observation.DataProducts.Output_Filtered.dirmask','L${YEAR}_${OBSID}')
        self.setdefault('Observation.DataProducts.Output_Beamformed.dirmask','L${YEAR}_${OBSID}')
        self.setdefault('Observation.DataProducts.Output_Correlated.dirmask','L${YEAR}_${OBSID}')
        self.setdefault('Observation.DataProducts.Output_CoherentStokes.dirmask','L${YEAR}_${OBSID}')
        self.setdefault('Observation.DataProducts.Output_IncoherentStokes.dirmask','L${YEAR}_${OBSID}')
        self.setdefault('Observation.DataProducts.Output_Trigger.dirmask','L${YEAR}_${OBSID}')

        # default beamlet settings, derived from subbandlist, for development
	if "Observation.subbandList" in self:
	  nrSubbands = len(self.getInt32Vector("Observation.subbandList"))
        else:
          nrSubbands = NRRSPBOARDS*NRBOARBEAMLETS

        slots = int(self["Observation.nrSlotsInFrame"])  

        self.setdefault("Observation.subbandList",  [151+s for s in xrange(nrSubbands)])  
	self.setdefault("Observation.beamList",     [0     for s in xrange(nrSubbands)])
	self.setdefault("Observation.rspBoardList", [s//slots for s in xrange(nrSubbands)])
	self.setdefault("Observation.rspSlotList",  [s%slots  for s in xrange(nrSubbands)])

        # convert pencil rings and fly's eye to more coordinates
        for b in count():
          if "Observation.Beam[%s].angle1" % (b,) not in self:
            break
          self.setdefault("Observation.Beam[%s].nrTabRings" % (b,),0)
          self.setdefault("Observation.Beam[%s].TabRingSize" % (b,),0.0)

          self.setdefault("Observation.Beam[%s].nrTabRings" % (b,), 0);
          self.setdefault("Observation.Beam[%s].TabRingSize" % (b,), 0);

          dirtype = self["Observation.Beam[%s].directionType" % (b,)]  
          dm = int(self.get("OLAP.dispersionMeasure",0))

          nrrings = int(self["Observation.Beam[%s].nrTabRings" % (b,)]) 
          width   = float(self["Observation.Beam[%s].TabRingSize" % (b,)]) 
          ringcoordinates = RingCoordinates( nrrings, width )
          ringset = [
            { "angle1": angle1,
              "angle2": angle2,
              "directionType": dirtype,
              "dispersionMeasure": dm,
              "stationList": [],
              "specificationType": "ring",
            } for (angle1,angle2) in ringcoordinates.coordinates()
          ]

          flyseyeset = []

          if self.getBool("OLAP.PencilInfo.flysEye"):
	    allStationNames = [st.getName() for st in self.stations]

            for s in allStationNames:
              flyseyeset.append(
                { "angle1": 0,
                  "angle2": 0,
                  "directionType": dirtype,
                  "dispersionMeasure": dm,
                  "stationList": [s],
                  "specificationType": "flyseye",
                }
              )

          manualset = []

          for m in count():
            if "Observation.Beam[%s].TiedArrayBeam[%s].angle1" % (b,m) not in self:
              break

            manualset.append(
              { "angle1": self["Observation.Beam[%s].TiedArrayBeam[%s].angle1" % (b,m)],
                "angle2": self["Observation.Beam[%s].TiedArrayBeam[%s].angle2" % (b,m)],
                "directionType": self["Observation.Beam[%s].TiedArrayBeam[%s].directionType" % (b,m)],
                "dispersionMeasure": self["Observation.Beam[%s].TiedArrayBeam[%s].dispersionMeasure" % (b,m)],
                "stationList": [],
                "specificationType": "manual",
              }
            )

          # first define the rings, then the manual beams (which thus get shifted in number!)
          allsets = ringset + flyseyeset + manualset
          for m,s in enumerate(allsets):
            prefix = "Observation.Beam[%s].TiedArrayBeam[%s]" % (b,m)

            for k,v in s.iteritems():
              self["%s.%s" % (prefix,k)] = v

          self["Observation.Beam[%s].nrTiedArrayBeams" % (b,)] = len(allsets)    

          if self.getBool("Observation.DataProducts.Output_Beamformed.enabled"):
            if NRCVSTOKES == 4:
              self["OLAP.CNProc_CoherentStokes.which"] = "XXYY"
            else: 
              self["OLAP.CNProc_CoherentStokes.which"] = "XY"


    def convertDepricatedKeys(self):
        """ Converts some new keys to old ones to help old CEP code cope with new SAS code or vice-versa. """

        convertmap = [
          # ("old","new")
          ("OLAP.Stokes.which", "OLAP.CNProc_CoherentStokes.which"),
          ("OLAP.Stokes.which", "OLAP.CNProc_IncoherentStokes.which"),

          ("OLAP.Stokes.integrationSteps", "OLAP.CNProc_CoherentStokes.timeIntegrationFactor"),
          ("OLAP.Stokes.integrationSteps", "OLAP.CNProc_IncoherentStokes.timeIntegrationFactor"),

          ("OLAP.Stokes.channelsPerSubband", "OLAP.CNProc_CoherentStokes.channelsPerSubband"),
          ("OLAP.Stokes.channelsPerSubband", "OLAP.CNProc_IncoherentStokes.channelsPerSubband"),

          ("OLAP.PencilInfo.ringSize", "Observation.Beam[0].TabRingSize"),
          ("OLAP.PencilInfo.nrRings",  "Observation.Beam[0].nrTabRings"),
        ]

        for (k,v) in convertmap:
          if k in self and v not in self:
            self[v] = self[k]

        # convert pencil beams (assign them to station beam 0)
        pbkeys = [ "angle1", "angle2", "directionType", "dispersionMeasure" ]
        pbdefaults = {
          "angle1": 0.0,
          "angle2": 0.0,
          "directionType": "J2000",
          "dispersionMeasure": 0,
        }  

        old_prefix = "OLAP.Pencil"
        new_prefix = "Observation.Beam[0].TiedArrayBeam"
        new_nr     = "Observation.Beam[0].nrTiedArrayBeams"
        
        for b in count():
          if "%s[%s].%s" % (old_prefix,b,pbkeys[0]) not in self:
            break

          for k in pbkeys:
            old_key = "%s[%s].%s" % (old_prefix,b,k)
            new_key = "%s[%s].%s" % (new_prefix,b,k)

            if new_key not in self:
              if old_key in self:
                self[new_key] = self[old_key]
              else:
                self[new_key] = pbdefaults[k]

        self.setdefault(new_nr,b)

    def convertSASkeys(self):
        """ Convert keys generated by SAS to those used by OLAP. """

        def delIfEmpty( k ):
          if k in self and not self[k]:
            del self[k]

        # SAS cannot omit keys, so assume that empty keys means 'use default'
        delIfEmpty( "OLAP.CNProc.phaseOnePsets" )
        delIfEmpty( "OLAP.CNProc.phaseTwoPsets" )
        delIfEmpty( "OLAP.CNProc.phaseThreePsets" )

        # SAS specifies beams differently
        if "Observation.subbandList" not in self:
          # convert beam configuration
          allSubbands = {}

	  for b in count():
            if "Observation.Beam[%s].angle1" % (b,) not in self:
              break

	    beamSubbands = self.getInt32Vector("Observation.Beam[%s].subbandList" % (b,)) # the actual subband number (0..511)
            beamBeamlets = self.getInt32Vector("Observation.Beam[%s].beamletList" % (b,)) # the bebamlet index (0..247)

            assert len(beamSubbands) == len(beamBeamlets), "Beam %d has %d subbands but %d beamlets defined" % (b,len(beamSubbands),len(beamBeamlets))

            for subband,beamlet in zip(beamSubbands,beamBeamlets):
              assert beamlet not in allSubbands, "Beam %d and %d both use beamlet %d" % (allSubbands[beamlet]["beam"],b,beamlet)

              allSubbands[beamlet] = {
                "beam":     b,
                "subband":  subband,
                "beamlet":  beamlet,

                # assign rsp board and slot according to beamlet id
                "rspboard": beamlet // NRBOARBEAMLETS,
                "rspslot":  beamlet % NRBOARBEAMLETS,
              }


          # order subbands according to beamlet id, for more human-friendly reading
          sortedSubbands = [allSubbands[x] for x in sorted( allSubbands )]

          # populate OLAP lists
	  self["Observation.subbandList"]  = [s["subband"] for s in sortedSubbands]
	  self["Observation.beamList"]     = [s["beam"] for s in sortedSubbands]
	  self["Observation.rspBoardList"] = [s["rspboard"] for s in sortedSubbands]
	  self["Observation.rspSlotList"]  = [s["rspslot"] for s in sortedSubbands]

    def addStorageKeys(self):
	self["OLAP.Storage.userName"] = getpass.getuser()
	self["OLAP.Storage.sshIdentityFile"]  = "%s/.ssh/id_rsa" % (os.environ["HOME"],)
	self["OLAP.Storage.msWriter"] = Locations.resolvePath( Locations.files["storage"], self )
	self["OLAP.Storage.parsetFilename"] = self.filename

        self.setdefault("OLAP.Storage.AntennaSetsConf",  "${STORAGE_CONFIGDIR}/AntennaSets.conf");
        self.setdefault("OLAP.Storage.AntennaFieldsDir", "${STORAGE_CONFIGDIR}/StaticMetaData");
        self.setdefault("OLAP.Storage.HBADeltasDir",     "${STORAGE_CONFIGDIR}/StaticMetaData");

        for p in ["OLAP.Storage.AntennaSetsConf",
                  "OLAP.Storage.AntennaFieldsDir",
                  "OLAP.Storage.HBADeltasDir"]:
          self[p] = Locations.resolvePath( self[p], self )          

    def preWrite(self):
        """ Derive some final keys and finalise any parameters necessary
	    before writing the parset to disk. """

        self.convertSASkeys();
        self.convertDepricatedKeys();
        self.addMissingKeys();
	self.addStorageKeys();

        # Versioning info
        self["OLAP.BeamsAreTransposed"] = True

	# TODO: we use self.setdefault, but this can create inconsistencies if we
	# set one value but not the other in a pair of interdependent parameters.
	# This should possibly be detected in the check routine, but it seems
	# sloppy to let it pass through here unnoticed.

	# tied-array beam forming
        tiedArrayStationList = []
	tabList = []
	beamFormedStations = []

        for index in count():
          if "Observation.Beamformer[%s].stationList" % (index,) not in self:
            break

	  curlist = self.getString('Observation.Beamformer[%s].stationList' % (index,))

          # remove any initial or trailing "
	  curlist = curlist.strip('"').rstrip('"')

          # transform , to +
	  curlist = curlist.replace(',','+')

	  tiedArrayStationList.append(curlist)

          # extract the individual stations
	  beamFormedStations += curlist.split('+')
	
	if index > 0:
	  # tied-array beamforming will occur

	  # add remaining stations to the list
	  allStationNames = [st.getName() for st in self.stations]
	  tiedArrayStationList += filter( lambda st: st not in beamFormedStations, allStationNames )

          # create a list of indices for all the stations, which by definition occur in
	  # the tiedArrayStationList
	  def findTabStation( s ):
	    for nr,tab in enumerate(tiedArrayStationList):
	      for st in tab.split('+'):
	        if s.getName() == st:
	          return nr

	  tabList = map( findTabStation, self.stations )
	   
	self.setdefault('OLAP.tiedArrayStationNames', tiedArrayStationList)
	self.setdefault('OLAP.CNProc.tabList', tabList)

	# input flow configuration
	for station in self.stations:
	  self.setdefault('PIC.Core.Station.%s.RSP.ports' % (station.name,), station.inputs)
	  
          stationName = station.name.split("_")[0] # remove specific antenna or array name (_hba0 etc) if present
          self.setdefault("PIC.Core.%s.position" % (stationName,), self["PIC.Core.%s.phaseCenter" % (stationName,)])

	for pset in xrange(len(self.psets)):
	  self.setdefault('PIC.Core.IONProc.%s[%s].inputs' % (self.partition,pset), [
	    "%s/RSP%s" % (station.name,rsp) for station in self.stations
	                                      if station.getPsetIndex(self.partition) == pset
	                                    for rsp in xrange(len(station.inputs))] )


	# output flow configuration
        self['OLAP.storageStationNames'] = [s.name for s in self.stations]

        self.setdefault('OLAP.OLAP_Conn.IONProc_Storage_Transport','TCP');
        self.setdefault('OLAP.OLAP_Conn.IONProc_CNProc_Transport','FCNP');

	# subband configuration
	if "Observation.subbandList" in self:
	  nrSubbands = len(self.getInt32Vector("Observation.subbandList"))
        else:
          nrSubbands = 248

        for nrBeams in count():
          if "Observation.Beam[%s].angle1" % (nrBeams,) not in self:
            break

          self.setdefault("Observation.Beam[%s].target" % (nrBeams,),"")
          self.setdefault("Observation.Beam[%s].directionType" % (nrBeams,),"J2000")

        self.setdefault('Observation.nrBeams', nrBeams)

	# Pset configuration
	self['OLAP.CNProc.partition'] = self.partition
        self['OLAP.IONProc.psetList'] = self.psets

        self.setdefault('OLAP.Storage.subbandsPerPart', nrSubbands);
	if nrSubbands > 0:
          self['OLAP.Storage.partsPerStokes'] = int( math.ceil( 1.0 * nrSubbands / int(self["OLAP.Storage.subbandsPerPart"]) ) )
        else:
          self['OLAP.Storage.partsPerStokes'] = 0

	nrPsets = len(self.psets)
	nrStorageNodes = self.getNrUsedStorageNodes()
        nrBeamFiles = self.getNrBeamFiles()

        # set storage hostnames
        self["OLAP.Storage.hosts"] = self.storagenodes[:]

	self.setdefault('OLAP.nrPsets', nrPsets)
	self.setdefault('OLAP.CNProc.phaseOnePsets', [s.getPsetIndex(self.partition) for s in self.stations])
	self.setdefault('OLAP.CNProc.phaseTwoPsets', range(nrPsets))
        if self.phaseThreeExists():
	  self.setdefault('OLAP.CNProc.phaseThreePsets', self['OLAP.CNProc.phaseTwoPsets'])
        else:  
	  self.setdefault('OLAP.CNProc.phaseThreePsets', [])

        if 'OLAP.CNProc.usedCoresInPset' in self:
          cores = self.getInt32Vector("OLAP.CNProc.usedCoresInPset")
        else:
          cores = range(64)

        self.setdefault('OLAP.CNProc.phaseOneTwoCores',cores)  
        self.setdefault('OLAP.CNProc.phaseThreeCores',cores)

        # what will be stored where?
        # outputSubbandPsets may well be set before finalize()
	subbandsPerPset = int( math.ceil(1.0 * nrSubbands / max( 1, len(self["OLAP.CNProc.phaseTwoPsets"]) ) ) )
        beamsPerPset = int( math.ceil(1.0 * nrBeamFiles / max( 1, len(self["OLAP.CNProc.phaseThreePsets"]) ) ) )

        def _sn( sb, sb_pset0, nr_sb ):
          if nrStorageNodes <= 1:
            return 0

          if PERFORMANCE_TEST:
            if sb < sb_pset0:
              return sb % (nrStorageNodes - 1)
            else:
              return nrStorageNodes - 1
          else:
            return sb//int(math.ceil(1.0 * nr_sb/nrStorageNodes));

        self.setdefault('OLAP.storageNodeList',[_sn(i,subbandsPerPset,nrSubbands) for i in xrange(nrSubbands)])

	self.setdefault('OLAP.PencilInfo.storageNodeList',[_sn(i,beamsPerPset,nrBeamFiles) for i in xrange(nrBeamFiles)])

        self.setdefault('OLAP.Storage.targetDirectory','/data1')

        # generate filenames to produce - phase 2
        nodelist = self.getInt32Vector( "OLAP.storageNodeList" );
        products = ["Filtered","Correlated","IncoherentStokes"]
        outputkeys = ["Filtered","Correlated","IncoherentStokes"]

        for p,o in zip(products,outputkeys):
          outputkey    = "Observation.DataProducts.Output_%s.enabled" % (o,)
          if not self.getBool(outputkey):
            continue

          maskkey      = "Observation.DataProducts.Output_%s.namemask" % p
          mask         = self["OLAP.Storage.targetDirectory"] + "/" + self["Observation.DataProducts.Output_%s.dirmask" % p] + "/" + self[maskkey]
          locationkey  = "Observation.DataProducts.Output_%s.locations" % p
          filenameskey = "Observation.DataProducts.Output_%s.filenames" % p

          if locationkey in self and filenameskey in self:
            continue

          paths = [ self.parseMask( mask, subband = i ) for i in xrange(nrSubbands) ]
          filenames = map( os.path.basename, paths )
          dirnames = map( os.path.dirname, paths )

          if self.storagenodes:
            locations = [ "%s:%s" % (self.storagenodes[nodelist[i]], dirnames[i]) for i in xrange(nrSubbands) ]
          else:
            locations = [ "" for i in xrange(nrSubbands) ]

          self.setdefault( locationkey, locations )
          self.setdefault( filenameskey, filenames )

        # generate filenames to produce - phase 3
        nodelist = self.getInt32Vector( "OLAP.PencilInfo.storageNodeList" );
        products = ["Beamformed","CoherentStokes","Trigger"]
        outputkeys = ["Beamformed","CoherentStokes","Trigger"]

        for p,o in zip(products,outputkeys):
          outputkey    = "Observation.DataProducts.Output_%s.enabled" % (o,)
          if not self.getBool(outputkey):
            continue

          maskkey      = "Observation.DataProducts.Output_%s.namemask" % p
          mask         = self["OLAP.Storage.targetDirectory"] + "/" + self["Observation.DataProducts.Output_%s.dirmask" % p] + "/" + self[maskkey]
          locationkey  = "Observation.DataProducts.Output_%s.locations" % p
          filenameskey = "Observation.DataProducts.Output_%s.filenames" % p

          if locationkey in self and filenameskey in self:
            continue

          # python iterates over last 'for' first!
          # this is the order generated by the IO nodes (see IONProc/src/Job.cc)
          paths = [ self.parseMask( mask, sap = sap, beam = b, stokes = s, part = p ) for sap in xrange(self.getNrSAPs()) for b in xrange(self.getNrBeams(sap)) for s in xrange(self.getNrCoherentStokes()) for p in xrange(self.getNrParts(sap))]
          filenames = map( os.path.basename, paths )
          dirnames = map( os.path.dirname, paths )

          if self.storagenodes:
            locations = [ "%s:%s" % (self.storagenodes[nodelist[i]], dirnames[i]) for i in xrange(self.getNrBeamFiles())]
          else:
            locations = [ "" for i in xrange(nrSubbands) ]

          self.setdefault( locationkey, locations )
          self.setdefault( filenameskey, filenames )

	# calculation configuration

        # integration times of CNProc and IONProc, based on self.integrationtime
        # maximum amount of time CNProc can integrate due to memory constraints
        if self.phaseThreeExists():
          maxCnIntegrationTime = 0.33
        else:
          maxCnIntegrationTime = 1.2

        # (minimal) number of times the IONProc will have to integrate
        integrationtime = float( self["OLAP.Correlator.integrationTime"] )
        ionIntegrationSteps = int(math.ceil(integrationtime / maxCnIntegrationTime))
        self.setdefault('OLAP.IONProc.integrationSteps', ionIntegrationSteps)

        # the amount of time CNProc will integrate, translated into samples
        cnIntegrationTime = integrationtime / int(self["OLAP.IONProc.integrationSteps"])
        nrSamplesPerSecond = int(self['Observation.sampleClock']) * 1e6 / 1024 / int(self['Observation.channelsPerSubband'])

        def gcd( a, b ):
          while b > 0:
            a, b = b, a % b
          return a

        def lcm( a, b ):
          return a * b / gcd(a, b)

        def lcmlist( l ):
          return reduce(lcm, l, 1)

        def roundTo( x, y ):
          """ Round x to a multiple of y. """
          return max(int(round(x/y))*y,y)

        cnIntegrationSteps = max(1, roundTo(nrSamplesPerSecond * cnIntegrationTime, lcmlist( [
          16,
          int(self["OLAP.CNProc_CoherentStokes.timeIntegrationFactor"]),
          int(self["OLAP.CNProc_IncoherentStokes.timeIntegrationFactor"]),
          int(self.get("OLAP.CNProc.dedispersionFFTsize",1)),
          ])))

        self.setdefault('OLAP.CNProc.integrationSteps', cnIntegrationSteps)

        if self.getBool( "OLAP.realTime" ):
          earliest_start = time.time() + 15 # allow a 15-second overhead
          if timestamp(parse(self["Observation.startTime"])) < earliest_start < timestamp(parse(self["Observation.stopTime"])):
            # remove the start of the observation that we missed
            warn("The start time of the observation has already passed. Moving it from %s to %s." % (self["Observation.startTime"],format(earliest_start)))
            self["Observation.startTime"] = format(earliest_start)

    def setStations(self,stations):
	""" Set the array of stations to use (used internally). """

        def name( s ):
          try:
            return s.name
          except:
            return s
	
	self.stations = sorted( stations, cmp=lambda x,y: cmp(name(x),name(y)) )

    def forceStations(self,stations):
	""" Override the set of stations to use (from the command line). """

	self.setStations(stations)

        def name( s ):
          try:
            return s.name
          except:
            return s

        self['OLAP.storageStationNames'] = map( name, self.stations )
        del self['Observation.VirtualInstrument.stationList']
        #del self['Observation.antennaSet']
        
    def setPartition(self,partition):
	""" Define the partition to use. """

	assert partition in PartitionPsets, "Partition %s unknown. Run LOFAR/Partitions.py to get a list of valid partitions." % (partition,)
	
	self.partition = partition
	self.psets = PartitionPsets[partition]

    def setStorageNodes(self,storagenodes):
	""" Define the list of storage nodes to use. """

        # do not resolve host names, since the resolve depends on the need (i.e. NIC needed)
	self.storagenodes = sorted(storagenodes)

        # OLAP needs IP addresses from the backend
        self["OLAP.Storage.hosts"] = self.storagenodes[:]

    def setObsID(self,obsid):
        self.setdefault("Observation.ObsID", obsid)

    def getObsID(self):
        if "Observation.ObsID" not in self:
          return None

        return int(self["Observation.ObsID"])

    def getNrUsedStorageNodes(self):
        return len(self.storagenodes)

    def disableCNProc(self):
        self["OLAP.OLAP_Conn.IONProc_CNProc_Transport"] = "NULL"

    def disableIONProc(self):
        self["OLAP.OLAP_Conn.IONProc_Storage_Transport"] = "NULL"
        self["OLAP.OLAP_Conn.IONProc_CNProc_Transport"] = "NULL"

    def disableStorage(self):
        self["OLAP.OLAP_Conn.IONProc_Storage_Transport"] = "NULL"
        self.setStorageNodes([])

    def parseMask( self, mask, sap = 0, subband = 0, beam = 0, stokes = 0, part = 0 ):
      """ Fills a mask. """

      assert "Observation.ObsID" in self, "Observation ID not generated yet."

      # obtain settings
      date = parse( self["Observation.startTime"] ).timetuple()

      # fill in the mask
      datenames = [ "YEAR", "MONTH", "DAY", "HOURS", "MINUTES", "SECONDS" ] # same order as in time tuple
      for index,d in enumerate(datenames):
        mask = mask.replace( "${%s}" % d, "%02d" % (date[index],) )

      mask = mask.replace( "${OBSID}", "%05d" % (self.getObsID(),) )
      mask = mask.replace( "${MSNUMBER}", "%05d" % (self.getObsID(),) )
      mask = mask.replace( "${SUBBAND}", "%03d" % (subband,) )
      mask = mask.replace( "${SAP}", "%03d" % (sap,) )
      mask = mask.replace( "${PART}", "%03d" % (part,) )
      mask = mask.replace( "${BEAM}", "%03d" % (beam,) )
      mask = mask.replace( "${STOKES}", "%01d" % (stokes,) )

      return mask

    def setStartStopTime( self, starttime, stoptime ):
      start = timestamp( parse( starttime ) )
      stop  = timestamp( parse( stoptime ) )

      self["Observation.startTime"] = format( start )
      self["Observation.stopTime"] = format( stop )

    def setStartRunTime( self, starttime, duration ):
      start = timestamp( parse( starttime ) )
      stop  = start + parseDuration( duration ) 

      self["Observation.startTime"] = format( start )
      self["Observation.stopTime"] = format( stop )

    def setClock( self, mhz ):
      self['Observation.sampleClock'] = int( mhz )

    def setIntegrationTime( self, integrationTime ):
      self["OLAP.Correlator.integrationTime"] = integrationTime

      # make sure these values will be recalculated in finalise()
      del self['OLAP.IONProc.integrationSteps']
      del self['OLAP.CNProc.integrationSteps']

    def getNrSAPs( self ):
      return int(self["Observation.nrBeams"])

    def getNrSubbands( self, sap ):
      return sum([1 for s in self.getInt32Vector("Observation.beamList") if s == sap])

    def getNrParts( self, sap ):
      return int(math.ceil(1.0 * self.getNrSubbands(sap) / int(self["OLAP.Storage.subbandsPerPart"])))

    def getNrBeams( self, sap ):
      return self["Observation.Beam[%u].nrTiedArrayBeams" % (sap,)]

    def getNrMergedStations( self ):
      tabList = self["OLAP.CNProc.tabList"]

      if not tabList:
        return len(self.stations)

      return max(tabList) + 1  

    def getNrCoherentStokes( self ):  
      if self.getBool("Observation.DataProducts.Output_Beamformed.enabled"):
        return len(self["OLAP.CNProc_CoherentStokes.which"])
      elif self.getBool("Observation.DataProducts.Output_Trigger.enabled"):
        return len(self["OLAP.CNProc_CoherentStokes.which"]) # todo: recombine Xi+Xr and Yi+Yr
      elif self.getBool("Observation.DataProducts.Output_CoherentStokes.enabled"):
        return len(self["OLAP.CNProc_CoherentStokes.which"])
      else:
        return 0

    def getNrBeamFiles( self ):
      return sum([self.getNrBeams(sap) * self.getNrCoherentStokes() * self.getNrParts(sap) for sap in xrange(self.getNrSAPs())])

    def phaseThreeExists( self ):  
      # NO support for mixing with Observation.mode and Observation.outputIncoherentStokesI
      output_keys = [
        "Observation.DataProducts.Output_Beamformed.enabled",
        "Observation.DataProducts.Output_CoherentStokes.enabled",
        "Observation.DataProducts.Output_Trigger.enabled",
      ]

      for k in output_keys:
        if k in self and self.getBool(k):
          return True

      return False

    def phaseThreePsetDisjunct( self ):
      phase1 = set(self.getInt32Vector("OLAP.CNProc.phaseOnePsets"))
      phase2 = set(self.getInt32Vector("OLAP.CNProc.phaseTwoPsets"))
      phase3 = set(self.getInt32Vector("OLAP.CNProc.phaseThreePsets"))

      return len(phase1.intersection(phase3)) == 0 and len(phase2.intersection(phase3)) == 0

    def phaseThreeCoreDisjunct( self ):
      phase12 = set(self.getInt32Vector("OLAP.CNProc.phaseOneTwoCores"))
      phase3 = set(self.getInt32Vector("OLAP.CNProc.phaseThreeCores"))

      return len(phase12.intersection(phase3)) == 0

    def phaseTwoThreePsetEqual( self ):
      phase2 = self.getInt32Vector("OLAP.CNProc.phaseTwoPsets")
      phase3 = self.getInt32Vector("OLAP.CNProc.phaseThreePsets")

      return phase2 == phase3

    def phaseOneTwoThreeCoreEqual( self ):
      phase12 = self.getInt32Vector("OLAP.CNProc.phaseOneTwoCores")
      phase3 = self.getInt32Vector("OLAP.CNProc.phaseThreeCores")

      return phase12 == phase3

    def outputPrefixes( self ):
      return [
        "Observation.DataProducts.Output_Filtered",
        "Observation.DataProducts.Output_Correlated",
        "Observation.DataProducts.Output_Beamformed",
        "Observation.DataProducts.Output_CoherentStokes",
        "Observation.DataProducts.Output_IncoherentStokes",
        "Observation.DataProducts.Output_Trigger",
      ]

    def getNrOutputs( self ):
      # NO support for mixing with Observation.mode and Observation.outputIncoherentStokesI
      output_keys = [ "%s.enabled" % (p,) for p in self.outputPrefixes() ]

      return sum( (1 for k in output_keys if k in self and self.getBool(k)) )

    def check( self ):
      """ Check the Parset configuration for inconsistencies. """

      def getBool(k):
        """ A getBool() routine with False as a default value. """
        return k in self and self.getBool(k)

      try:  
        assert self.getNrOutputs() > 0, "No data output selected."
        assert len(self.stations) > 0, "No stations selected."
        assert len(self.getInt32Vector("Observation.subbandList")) > 0, "No subbands selected."
        assert len(self.getInt32Vector("Observation.subbandList")) <= NRRSPBOARDS*NRBOARBEAMLETS, "More than %d subbands selected." % (NRRSPBOARDS*NRBOARBEAMLETS,)

        # phase 2 and 3 are either disjunct or equal
        assert self.phaseThreePsetDisjunct() or self.phaseTwoThreePsetEqual(), "Phase 2 and 3 should use either disjunct or the same psets."
        assert self.phaseThreeCoreDisjunct() or self.phaseOneTwoThreeCoreEqual(), "Phase 1+2 and 3 should use either disjunct or the same cores."
        assert not (self.phaseThreePsetDisjunct() and self.phaseThreeCoreDisjunct()), "Phase 3 should use either disjunct psets or cores."

        # verify psets used
        nrPsets = len(self.psets)
        for k in [
          "OLAP.CNProc.phaseOnePsets",
          "OLAP.CNProc.phaseTwoPsets",
          "OLAP.CNProc.phaseThreePsets",
        ]:
          psets = self.getInt32Vector( k )
          for p in psets:
            assert p < nrPsets, "Use of pset %d requested in key %s, but only psets [0..%d] are available" % (p,k,nrPsets-1)

        # no both bf complex voltages and stokes
        assert not (getBool("Observation.DataProducts.Output_Beamformed.enabled") and getBool("Observation.DataProducts.Output_CoherentStokes.enabled")), "Cannot output both complex voltages and coherent stokes."

        # restrictions on #samples and integration in beam forming modes
        if self.getBool("Observation.DataProducts.Output_Beamformed.enabled") or self.getBool("Observation.DataProducts.Output_CoherentStokes.enabled"):
          # beamforming needs a multiple of 16 samples
          assert int(self["OLAP.CNProc.integrationSteps"]) % 16 == 0, "OLAP.CNProc.integrationSteps should be dividable by 16"

          assert int(self["OLAP.CNProc.integrationSteps"]) % int(self["OLAP.CNProc_CoherentStokes.timeIntegrationFactor"]) == 0, "OLAP.CNProc.integrationSteps should be dividable by OLAP.CNProc_CoherentStokes.timeIntegrationFactor"
          assert int(self["OLAP.CNProc.integrationSteps"]) % int(self["OLAP.CNProc_IncoherentStokes.timeIntegrationFactor"]) == 0, "OLAP.CNProc.integrationSteps should be dividable by OLAP.CNProc_IncoherentStokes.timeIntegrationFactor"
          if not self.phaseThreePsetDisjunct() and not self.phaseThreeCoreDisjunct():
            assert self.getNrBeamFiles() <= len(self.getInt32Vector("Observation.subbandList")), "Cannot create more files than there are subbands."

          # create at least 1 beam
          #assert self.getNrBeams( True ) > 0, "Beam forming requested, but no beams defined. Add at least one beam."

        if self.getBool("Observation.DataProducts.Output_CoherentStokes.enabled"):
          assert int(self["OLAP.CNProc.integrationSteps"]) >= 4, "OLAP.CNProc.integrationSteps should be at least 4 if coherent stokes are requested"

        assert int(self["OLAP.CNProc_CoherentStokes.channelsPerSubband"]) <= int(self["Observation.channelsPerSubband"]), "Coherent Stokes should have the same number or fewer channels than specified for the full observation."
        assert int(self["Observation.channelsPerSubband"]) % int(self["OLAP.CNProc_CoherentStokes.channelsPerSubband"]) == 0, "Coherent Stokes channels should be a whole fraction of the total number of channels."

        assert int(self["OLAP.CNProc_IncoherentStokes.channelsPerSubband"]) <= int(self["Observation.channelsPerSubband"]), "Incoherent Stokes should have the same number or fewer channels than specified for the full observation."
        assert int(self["Observation.channelsPerSubband"]) % int(self["OLAP.CNProc_IncoherentStokes.channelsPerSubband"]) == 0, "Incoherent Stokes channels should be a whole fraction of the total number of channels."

        # verify start/stop times
        assert self["Observation.startTime"] < self["Observation.stopTime"], "Start time (%s) must be before stop time (%s)" % (self["Observation.startTime"],self["Observation.stopTime"])

        if self.getBool( "OLAP.realTime" ):
          assert timestamp(parse(self["Observation.startTime"])) > time.time(), "Observation.realTime is set, so start time (%s) should be later than now (%s)" % (self["Observation.startTime"],format(time.time()))

        # verify stations
        for s in self.stations:
          stationName = s.name.split("_")[0] # remove specific antenna or array name (_hba0 etc) if present
          assert "PIC.Core.%s.phaseCenter" % (stationName,) in self, "Phase center of station '%s' not present in parset." % (stationName,)
      except AssertionError,e:
        error(e);

        self["OLAP.IONProc.parsetError"] = "%s" % (e,)
        return False
      else:
        self["OLAP.IONProc.parsetError"] = ""
        return True

if __name__ == "__main__":
  from optparse import OptionParser,OptionGroup
  import sys

  # parse the command line
  parser = OptionParser( "usage: %prog [options] parset [parset ...]" )

  # parse arguments
  (options, args) = parser.parse_args()
  
  if not args:
    parser.print_help()
    sys.exit(0)

  parset = Parset()

  for files in args:
    parset.readFile( files )

  parset.postRead()
  parset.preWrite()
  parset.writeFile( "-" )  

  sys.exit(0)

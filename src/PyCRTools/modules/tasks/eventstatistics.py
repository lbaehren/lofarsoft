"""

Task: Event Statistics for LORA/LOFAR
=================================

.. moduleauthor:: Name of the module author <email address of the module author>
"""


import pycrtools as cr
import pycrtools.tasks as tasks
import os
import time
import numpy as np
import sys
from pycrtools import xmldict

cr.tasks.__raiseTaskDeprecationWarning(__name__)

def GetInformationFromFile(topdir, events):

    eventdirs = cr.listFiles([os.path.join(topdir, event) for event in events])

    for eventdir in eventdirs:

        print "Processing event in directory:", eventdir

        par = {}
        antid = {0: [], 1: []}
        signal = {0: [], 1: []}
        positions = {0: [], 1: []}
        rms = {0: [], 1: []}
        spectral_power = {0: [], 1: []}
        res = {}

        datadirs = cr.listFiles(os.path.join(os.path.join(eventdir, "pol?"), "*"))
        overallstatus = False
        for datadir in datadirs:
            if not os.path.isfile(os.path.join(datadir, "results.xml")):
                continue
            # print "Processing data results directory:",datadir

            try:
                res = xmldict.load(os.path.join(datadir, "results.xml"))
                # res=res["results"]
            except Exception:
                continue
                print "File skipped, NaN found!"

            try:
                status = res["status"]
            except Exception:
                status = "OK assumed"  # the status has not be propagated for old files

            if status == "OK":
                overallstatus = True

            antid[res["polarization"]].extend(res["antennas"][key] for key in res["antennas"].keys())
            positions[res["polarization"]].extend(res["antenna_positions_array_XYZ_m"])

            # signal[res["polarization"]].extend(res[plot_parameter])
            rms[res["polarization"]].extend(res["timeseries_power_rms"])
            spectral_power[res["polarization"]].extend(res["antennas_spectral_power"])

        if res == {}:
            print "No results file found"
            continue
            # sys.exit(0)

        # print "Number of dipoles found:",ndipoles

        timesec = res["TIME"]
        timestr = time.strftime("%Y%m%dT%H%M%S", time.gmtime(timesec))

        timens = str(res["SAMPLE_INTERVAL"] * res["SAMPLE_NUMBER"] * 1000)[0:3]
        time_stamp = timestr + "." + timens + "Z"

        par["event_id"] = time_stamp

        par["antenna_set"] = res["ANTENNA_SET"]

        lorcore = cr.hArray(res["pulse_core_lora"])
        Dim = lorcore.shape()
        if 2 in Dim:
            par["loracore"] = cr.hArray([lorcore[0], lorcore[1], 0.0])
        else:
            if 3 in Dim:
                par["loracore"] = lorcore
            else:
                print "Warning, Coreposition of LORA seems to contain the wrong number of coordinates"

        par["status"] = overallstatus
        par["loradirection"] = cr.hArray(res["pulse_direction_lora"])
        par["loraenergy"] = res["pulse_energy_lora"]
        par["rms"] = rms
        par["antid"] = antid
        par["time"] = timesec
        par["spectral_power"] = spectral_power
    return par


class eventstatistics(tasks.Task):

    """
    This task will make some plots for event statitsics. Information about how to run will be added.
    """

    parameters = dict(
        topdir=dict(default="/data/VHECR/LORAtriggered/results", doc="provide topdir", unit=""),
        eventslist=dict(default=lambda self: self.events if isinstance(self.events, list) else [self.events], doc="list with event names to process (directories in topdir)", unit=""),
        events=dict(default=["VHECR_LORA-20110612T231913.199Z"], doc="Event directories in topdir - either as list or as single string", unit=""),
        results=dict(default=lambda self: GetInformationFromFile(self.topdir, self.eventslist), doc="Provide task with topdirectory", unit=""),
        antennaquery=dict(default=['002000002', '005008071']),
        direction_lora=dict(default=lambda self: self.results["loradirection"], doc="hArray of shower direction [az, el] as provided by Lora. Az is eastwards from north and El is up from horizon.", unit="degrees"),
        rms=dict(default=lambda self: self.results["rms"]),
        antid=dict(default=lambda self: self.results["antid"]),
        time=dict(default=lambda self: self.results["time"]),
        status=dict(default=lambda self: self.results["status"]),
        spectral_power=dict(default=lambda self: self.results["spectral_power"])
        # to add other quantities
        )

    def init(self):
        pass

    def call(self):
        pass

    def run(self):

        """
        Making all sorts of things.

        """

        rms_0 = self.rms[0]
        rms_1 = self.rms[1]

        antid_0 = self.antid[0]
        antid_1 = self.antid[1]

        spectral_power_0 = self.spectral_power[0]
        spectral_power_1 = self.spectral_power[1]

        # print "Search:", self.antennaquery
        # print "In: ", antid_0

        try:
            station_0 = antid_0.index(self.antennaquery[0])
            value = rms_0[station_0]
            power = spectral_power_0[station_0]

        except Exception:
            value = -100
            power = -100
            print "Station not found"

        return value, self.time, self.direction_lora, self.status, power

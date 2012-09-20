#! /usr/bin/env python
"""
Add a datafile to the CR database
"""
import os
import time
from optparse import OptionParser

class DataExtractor(object):
    """Class to extract specific data from an hdf5 cr datafile.
    """

    def __init__(self, filename=""):
        """Initialisation of the DataExtractor object.

        **Properties**

        ==========  ==========================================
        Parameter   Description
        ==========  ==========================================
        *filename*  filename of the VHECR data file.
        ==========  ==========================================
        """
        self._filename = filename
        self._datafile = None

        self.open()


    def open(self):
        """Open a VHECR data file."""
        if not self._datafile:
            if "" != self._filename:
                try:
                    self._datafile = cr.open(self._filename)
                except:
                    self._datafile = None
                    print "WARNING: problem when opening file {0}".format(self._filename)
            else:
                raise ValueError("No filename provided!")


    def isOpen(self):
        """Check if the datafile is open."""
        if self._datafile:
            result = True
        else:
            result = False

        return result


    @property
    def timestamp(self):
        """Timestamp of the event."""
        result = 0

        if self.isOpen():
            seconds = self._datafile["TIME"][0]
            subsec  = self._datafile["SAMPLE_NUMBER"][0] * self._datafile["SAMPLE_INTERVAL"][0]
            result = int(seconds) # + round(subsec + 0.5))
        else:
            raise ValueError("No open datafile")

        return result


    @property
    def stationnames(self):
        """List of station names."""
        result = []

        if self.isOpen():
            result = list(set(self._datafile["STATION_NAME"]))

        return result


    @property
    def antennaset(self):
        """Name of the used antennaset in the datafile."""
        result = ""

        if self.isOpen():
            result = self._datafile["ANTENNA_SET"]

        return result


    def resultsfile(self, pol=0):
        """Name of the resultsfile where the pipeline results are stored.

        This filename is derived from metadata in the datafile.

        **Properties**

        =========  ===================================================
        Parameter  Datafile
        =========  ===================================================
        *pol*      polarization direction.
        =========  ===================================================
        """
        result = ""

        if self.isOpen():
            # The filename of the resultsfile is created/derived in the same way as in cr_event.py
            root_pathname = "VHECR_LORA-"

            # Timestamp
            time_s      = self._datafile["TIME"][0]
            time_s_str  = time.strftime("%Y%m%dT%H%M%S",time.gmtime(time_s))
            time_ms     = int(self._datafile["SAMPLE_INTERVAL"][0]*self._datafile["SAMPLE_NUMBER"][0]*1000)
            time_ms_str = str(time_ms).zfill(3)
            time_stamp  = time_s_str+"."+time_ms_str+"Z"

            polname = "pol%s" %(pol)

            stationname = self.stationnames[0]

            rel_pathname = os.path.join(root_pathname+time_stamp, polname, stationname)
            rel_filename = os.path.join(rel_pathname, "results.xml")

            result = rel_filename

        return result


    def summary(self):
        """Summary of the DataExtractor object."""
        linewidth = 80

        print "="*linewidth
        print "  Summary of the DataExtractor object."
        print "="*linewidth

        print "  %-40s : %s" %("Filename", self._filename)

        print "="*linewidth



def populate(db_filename, data_filename):
    """Populate database with information from the specified datafile.

    **Properties**

    ===============  ====================================
    Parameter        Description
    ===============  ====================================
    *db_filename*    Filename of the database.
    *data_filename*  Filename of the data file.
    ===============  ====================================
    """

    # Attribute validation
    if "" == db_filename:
        raise ValueError("No database filename provided.")

    dbManager = crdb.CRDatabase(filename=db_filename,
                                datapath=options.datapath,
                                resultspath=options.resultspath,
                                lorapath=options.lorapath)
    db = dbManager.db

    dx = None
    filename_full = options.datapath + "/" + data_filename

    # Find file information
    datafileIDs = dbManager.getDatafileIDs(filename=data_filename)
    if not datafileIDs:
        print "Processing %s..." %(filename_full)
        dx = DataExtractor(filename_full)
        if not dx.isOpen():
            # Skip to next file when unable to open datafile
            print "WARNING: Unable to open '{0}', skipping to next file.".format(filename_full)
            return

        timestamp = dx.timestamp

        # Create datafile entry
        d = crdb.Datafile(db)
        d.filename = data_filename
        d.write(recursive=False, parameters=False)

        # Find event information
        eventIDs = dbManager.getEventIDs(timestamp=timestamp)
        if not eventIDs:
            e = crdb.Event(db)
            e.timestamp = timestamp
            e.write(recursive=False, parameters=False)
        else:
            eventID = eventIDs[0]
            e = crdb.Event(db, id=eventID)

        # Add datafile to event
        e.addDatafile(d)

        # Add LORA data to event parameters
        process_lora_data(e, dbManager.settings.lorapath)
    else:
        # Return if datafile information is already available
        return

    # Find station information
    # - Station names
    for stationname in dx.stationnames:
        stationIDs = dbManager.getStationIDs(datafileID=d.id, stationname=stationname)
        if not stationIDs:
            # Create stations
            s = crdb.Station(db)
            s.stationname = stationname
            s.write(recursive=False, parameters=False)
            d.addStation(s)

            # Create polarizations
            for pol_direction in ["0","1","xyz"]:
                p = crdb.Polarization(db)
                p.antennaset = dx.antennaset
                p.direction = "%s" %(pol_direction)
                p.resultsfile = dx.resultsfile(pol_direction)
                p.write()

                s.addPolarization(p)
                if options.parameters:
                    # If results.xml file exists add results to properties.
                    results_filename = os.path.join(dbManager.settings.resultspath,
                                                    p.resultsfile)
                    if os.path.isfile(results_filename):
                        print "      Processing results file %s..." %(results_filename)
                        parameters = xmldict.load(results_filename)
                        for key in parameters.keys():
                            p[key] = parameters[key]
                        p.write(recursive=False, parameters=True)
                    else:
                        print("Results file {0} does not exist...".format(results_filename))


def update(db_filename):
    """Update the database.

    **Properties**

    ===============  ====================================
    Parameter        Description
    ===============  ====================================
    *db_filename*    Filename of the database.
    ===============  ====================================
    """
    # Attribute validation
    if "" == db_filename:
        raise ValueError("No database filename provided.")

    dbManager = crdb.CRDatabase(filename=db_filename,
                                datapath=options.datapath,
                                resultspath=options.resultspath,
                                lorapath=options.lorapath)
    db = dbManager.db

    eventIDs = dbManager.getEventIDs()

    resultspath = dbManager.settings.resultspath
    print "Resultspath: ", resultspath

    for eID in eventIDs:
        e = dbManager.getEvent(eID)
        print "Event: ", e.timestamp
        for d in e.datafiles:
            print "  Datafile: ", d.filename
            for s in d.stations:
                print "    Station: ", s.stationname
                for p in s.polarization.values():
                    # If results.xml file exists add results to properties.
                    results_filename = os.path.join(resultspath, p.resultsfile)
                    if os.path.isfile(results_filename) and options.parameters:
                        print "      Processing results file %s..." %(results_filename)
                        parameters = xmldict.load(results_filename)
                        for key in parameters.keys:
                            p[key] = parameters[key]
                        p.write(recursive=False, parameters=True)
                    else:
                        print("Results file {0} does not exist...".format(results_filename))

        # Add LORA data to event parameters
        process_lora_data(e, dbManager.settings.lorapath)


def process_lora_data(event, lorapath):
    """Add LORA specific information to the event information in the database.

    **Properties**

    ============  =======================================================
    Parameter     Description
    ============  =======================================================
    *event*       event object in which to store the LORA information.
    *lorapath*    Path where the LORA information files are found.
    ============  =======================================================
    """

    print "Adding LORA parameters..."
    lora_data = lora.loraInfo(event.timestamp, lorapath)
    if lora_data:
        for key in lora_data.keys():
            lora_key = "lora_" + key
            event[lora_key] = lora_data[key]
        event.write(recursive=False, parameters=True)
    else:
        print("Empty lora_data set...")


def parseOptions():
    """Parse options from the command line."""

    usage = "usage: %prog [options] <databasefile> <datafile>"
    parser = OptionParser(usage=usage)

    # parser.add_option("-a", "--archive", type="str", dest="archivepath", default="", help="directory where the archive can be found.")
    parser.add_option("-d", "--datapath", type="str", dest="datapath", default="", help="directory where the datafiles can be found.")
    parser.add_option("-r", "--resultspath", type="str", dest="resultspath", default="", help="directory where the results files are written.")
    parser.add_option("-l", "--lorapath", type="str", dest="lorapath", default="", help="directory where the lora files are written.")
    parser.add_option("-u", "--update", action="store_true", dest="update", default=False, help="Update instead of populating database.")
    parser.add_option("-s", "--hdf5", type="str", dest="datafile", default="", help="name of a single hdf5 file that needs to be registered in the database.")
    parser.add_option("-p", "--parameters", action="store_true", dest="parameters", default=False, help="Enable storing polarization parameter information if available [default=False].")

    (options, args) = parser.parse_args()

    if len(args) != 2:
        parser.error("Incorrect number of arguments.")

    options.basepath = os.path.dirname(os.path.abspath(args[0]))

    print "datapath = %s" %(options.datapath)
    print "resultspath = %s" %(options.resultspath)
    print "lorapath = %s" %(options.lorapath)
    print "datafile = %s" %(options.datafile)

    return (options, args)


(options, args) = parseOptions()

if __name__ == '__main__':
    print "Add a datafile to the CR database..."

    import pycrtools as cr
    from pycrtools import crdatabase as crdb
    from pycrtools import xmldict
    from pycrtools import lora

    db_filename = args[0]
    data_filename = args[1]

    if options.update:
        print "Updating database %s..." %(db_filename)
        update(db_filename)
    else:
        print "Populating database %s with %s..." %(db_filename, data_filename)
        populate(db_filename, data_filename)


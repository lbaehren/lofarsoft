"""Populate the CR Database with datafile and event information.

This is required to process the datafiles in the pipeline.

:Author: Martin van den Akker <martinva@astro.ru.nl>
"""

import os
from optparse import OptionParser
import time

class CRDatabasePopulator(object):

    def __init__(self, db_filename="", options=None):
        """Initialisation of the CRDatabasePopulator object.

        **Properties**

        =============  =========================================================
        Parameter      Description
        =============  =========================================================
        *db_filename*  filename of the database file.
        *options*      command line parameters.
        =============  =========================================================
        """
        self._db_filename = db_filename
        self._db = None
        self._options = options

        # Attribute validation
        if "" == self._db_filename:
            raise ValueError("No database filename provided.")

        if not self._options:
            raise ValueError("No options provided.")

        self.dbManager = crdb.CRDatabase(filename=self._db_filename,
                                         datapath=options.datapath,
                                         resultspath=options.resultspath,
                                         lorapath=options.lorapath)

        self.doWrite = False

        if self.dbManager:
            self._db = self.dbManager.db
            self.settings = self.dbManager.settings
        else:
            raise ValueError("Unable to set CRDatabase.")


    def populate(self):
        """Populate the database with datafile and event information."""

        # Build list of filenames
        datapath = self.settings.datapath

        if "" != options.datafile:
            filename_list = [options.datafile]
        else:
            print "  . Building list of files to process..."
            filename_list = os.listdir(datapath)

            # - Filter filename_list for appropriate files
            #   - includefilter (.h5 files)
            includefilter = crdb.Filter(self._db, "INCLUDE")
            includefilter.add("h5")
            print "    - Running include filter..."
            filename_list = filter(lambda f: includefilter.execute(f), filename_list)

            #   - excludefilter (no 'test' or 'bkp' files)
            excludefilter = crdb.Filter(self._db, "EXCLUDE")
            excludefilter.add("test")
            excludefilter.add("bkp")
            print "    - Running excludefilter..."
            for i in range(1,99):
                excludefilter.add("R%03d" %(i))
            filename_list = filter(lambda f: not excludefilter.execute(f), filename_list)

            # - File filter
            if "" != options.filefilter:
                filefilter = crdb.Filter(self._db, "FILE")
                print "options.filefilter = %s" %(options.filefilter)
                filefilter.add(options.filefilter)
                filename_list = filter(lambda f: filefilter.execute(f), filename_list)
                filefilter.delete(options.filefilter)

        # Loop over all filenames in the filelist
        print "  . Looping over all files"
        for filename in filename_list:
            dx = None
            filename_full = datapath + "/" + filename

            # Find file information
            datafileIDs = self.dbManager.getDatafileIDs(filename=filename)
            if not datafileIDs:
                print "Processing %s ..." %(filename_full)
                dx = DataExtractor(filename_full)
                if not dx.isOpen():
                    # Skip to next file when unable to open datafile
                    print "WARNING: Unable to open '{0}', skipping to next file.".format(filename_full)
                    continue

                timestamp = dx.timestamp

                d = crdb.Datafile(self._db)
                d.filename = filename
                d.write(recursive=False, parameters=False)

                # Find event information
                eventIDs = self.dbManager.getEventIDs(timestamp=timestamp)
                if not eventIDs:
                    e = crdb.Event(self._db)
                    e.timestamp = timestamp
                    e.write(recursive=False, parameters=False)
                else:
                    eventID = eventIDs[0]
                    e = crdb.Event(self._db, id=eventID)

                # Add datafile to event
                e.addDatafile(d)

                # Add LORA data to event parameters
                self.process_lora_data(e)

            else:
                continue

            # Find station information
            # - Station names
            for stationname in dx.stationnames:
                stationIDs = self.dbManager.getStationIDs(datafileID=d.id, stationname=stationname)
                if not stationIDs:
                    # Create stations
                    s = crdb.Station(self._db)
                    s.stationname = stationname
                    s.write(recursive=False, parameters=False)
                    d.addStation(s)

                    # Create polarizations
                    for pol_direction in ["0","1","xyz"]:
                        p = crdb.Polarization(self._db)
                        p.antennaset = dx.antennaset
                        p.direction = "%s" %(pol_direction)
                        p.resultsfile = dx.resultsfile(pol_direction)

                        p.write()
                        s.addPolarization(p)
                        if options.parameters:
                            # If results.xml file exists add results to properties.
                            results_filename = os.path.join(self.dbManager.settings.resultspath,
                                                            p.resultsfile)
                            if os.path.isfile(results_filename):
                                print "      Processing results file %s..." %(results_filename)
                                parameters = xmldict.load(results_filename)
                                for key in parameters.keys():
                                    p[key] = parameters[key]
                                p.write(recursive=False, parameters=True)
                            else:
                                print("Results file {0} does not exist...".format(results_filename))


    def update(self):
        """Update already existing database entries."""
        eventIDs = self.dbManager.getEventIDs()

        resultspath = self.dbManager.settings.resultspath
        print "Resultspath: ", resultspath

        for eID in eventIDs:
            e = self.dbManager.getEvent(eID)
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

            # Add LORA parameters
            self.process_lora_data(e)


    def process_lora_data(self, event):
        print "Adding LORA parameters..."
        lora_data = lora.loraInfo(event.timestamp, self.settings.lorapath)
        if lora_data:
            for key in lora_data.keys():
                lora_key = "lora_" + key
                event[lora_key] = lora_data[key]
            event.write(recursive=False, parameters=True)
        else:
            print("Empty lora_data set...")


    def populate_nodb(self):
        """Populate the database with datafile and event information."""

        # Build list of filenames
        datapath = self.settings.datapath
        filename_list = os.listdir(datapath)

        # - Filter filename_list for appropriate files

        #   - includefilter (.h5 files)
        includefilter = crdb.Filter(self._db, "INCLUDE")
        includefilter.add("h5")
        filename_list = filter(lambda f: includefilter.execute(f), filename_list)

        #   - excludefilter (no 'test' or 'bkp' files)
        excludefilter = crdb.Filter(self._db, "EXCLUDE")
        excludefilter.add("test")
        excludefilter.add("bkp")
        filename_list = filter(lambda f: not excludefilter.execute(f), filename_list)

        # - File filter
        if "" != options.filefilter:
            filefilter = crdb.Filter(self._db, "FILE")
            print "options.filefilter = %s" %(options.filefilter)
            filefilter.add(options.filefilter)
            filename_list = filter(lambda f: filefilter.execute(f), filename_list)
            filefilter.delete(options.filefilter)

        # Loop over all filenames in the filelist
        for filename in filename_list:
            dx = None
            filename_full = datapath + "/" + filename

            # Find file information
            datafileIDs = self.dbManager.getDatafileIDs(filename=filename)
            if not datafileIDs:
                print "Processing %s ..." %(filename_full)
                dx = DataExtractor(filename_full)
                if not dx.isOpen():
                    # Skip to next file when unable to open datafile
                    print "WARNING: Unable to open '{0}', skipping to next file.".format(filename_full)
                    continue

                timestamp = dx.timestamp

                datafile = crdb.Datafile(self._db)
                datafile.filename = filename
                if self.doWrite:
                    datafile.write(recursive=False)

                # Find event information
                eventIDs = self.dbManager.getEventIDs(timestamp=timestamp)
                if not eventIDs:
                    event = crdb.Event(self._db)
                    event.timestamp = timestamp
                    if self.doWrite:
                        event.write(recursive=False)
                else:
                    eventID = eventIDs[0]
                    event = crdb.Event(self._db, id=eventID)

                # Add datafile to event
                if self.doWrite:
                    event.addDatafile(datafile)
                    event.write(recursive=False)
            else:
                continue

            # Find station information
            # - Station names
            for stationname in dx.stationnames:
                stationIDs = self.dbManager.getStationIDs(datafileID=datafile.id, stationname=stationname)
                if not stationIDs:
                    # Create stations
                    station = crdb.Station(self._db)
                    station.stationname = stationname
                    if self.doWrite:
                        station.write(recursive=False)
                        datafile.addStation(station)

                    # Create polarizations
                    for pol_direction in ["0","1","xyz"]:
                        p = crdb.Polarization(self._db)
                        p.antennaset = dx.antennaset
                        p.direction = "%s" %(pol_direction)
                        p.resultsfile = dx.resultsfile(pol_direction)

                        if self.doWrite:
                            p.write()   # Write to db and get ID
                            station.addPolarization(p)


    def summary(self):
        """Summary of the CRDatabasePopulator object."""
        linewidth = 80

        print "="*linewidth
        print "  Summary of the CRDatabasePopulator object."
        print "="*linewidth

        print "  %-40s : %s" %("datapath", self.settings.datapath)
        print "  %-40s : %s" %("resultspath", self.settings.resultspath)
        print "  %-40s : %s" %("lorapath", self.settings.lorapath)

        print "="*linewidth


class DataExtractor(object):

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

            polname = "pol%d" %(pol)

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



def parseOptions():
    """Parse options from the command line."""

    usage = "usage: %prog [options] databasefile"
    parser = OptionParser(usage=usage)

    # parser.add_option("-a", "--archive", type="str", dest="archivepath", default="", help="directory where the archive can be found.")
    parser.add_option("-d", "--datapath", type="str", dest="datapath", default="", help="directory where the datafiles can be found.")
    parser.add_option("-f", "--filefilter", type="str", dest="filefilter", default="", help="filter to select which files are processed.")
    parser.add_option("-r", "--resultspath", type="str", dest="resultspath", default="", help="directory where the results files are written.")
    parser.add_option("-l", "--lorapath", type="str", dest="lorapath", default="", help="directory where the lora files are written.")
    parser.add_option("-u", "--update", action="store_true", dest="update", default=False, help="Update instead of populating database.")
    parser.add_option("-s", "--hdf5", type="str", dest="datafile", default="", help="name of a single hdf5 file that needs to be registered in the database.")
    parser.add_option("-p", "--parameters", action="store_true", dest="parameters", default=False, help="Enable storing polarization parameter information if available [default=False].")

    (options, args) = parser.parse_args()

    if len(args) != 1:
        parser.error("Incorrect number of arguments.")

    options.basepath = os.path.dirname(os.path.abspath(args[0]))

    print "datapath = %s" %(options.datapath)
    print "resultspath = %s" %(options.resultspath)
    print "lorapath = %s" %(options.lorapath)
    print "filefilter = %s" %(options.filefilter)
    print "datafile = %s" %(options.datafile)

    return (options, args)


def test(options, args):
    # Populate the database
    db0_name = args[0]+"0.db"
    db1_name = args[0]+"1.db"

    dbp0 = CRDatabasePopulator(db_filename=db0_name, options=options)
    dbp1 = CRDatabasePopulator(db_filename=db1_name, options=options)

    t0 = time.time()
    dbp0.populate()
    print "t : ",(time.time() - t0)
    dbp0.dbManager.summary()

    print "="*80

    t0 = time.time()
    dbp1.populate_nodb()
    print "t : ",(time.time() - t0)
    dbp1.dbManager.summary()


if __name__ == '__main__':
    (options, args) = parseOptions()

    # Load all packages after processing the commandline
    import pycrtools as cr
    from pycrtools import crdatabase as crdb
    from pycrtools import xmldict
    from pycrtools import lora

    #test(options, args)

    db_name = args[0]
    dbp = CRDatabasePopulator(db_filename=db_name, options=options)

    t0 = time.time()

    if options.update:
        print "Updating database %s..." %(db_name)
        dbp.update()
    else:
        print "Populating database %s..." %(db_name)
        dbp.populate()

    print "time needed: ", (time.time()-t0)


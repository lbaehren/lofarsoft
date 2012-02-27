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
                                         resultspath=options.resultspath)

        if self.dbManager:
            self._db = self.dbManager.db
            self.settings = self.dbManager.settings
        else:
            raise ValueError("Unable to set CRDatabase.")


    def populate(self):
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
        if options.filefilter != "":
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
                    continue

                timestamp = dx.timestamp

                datafile = crdb.Datafile(self._db)
                datafile.filename = filename
                datafile.write(recursive=False)

                # Find event information
                eventIDs = self.dbManager.getEventIDs(timestamp=timestamp)
                if not eventIDs:
                    event = crdb.Event(self._db)
                    event.timestamp = timestamp
                    event.write(recursive=False)
                else:
                    eventID = eventIDs[0]
                    event = crdb.Event(self._db, id=eventID)

                # Add datafile to event
                event.addDatafile(datafile)
                event.write()

            datafileIDs = self.dbManager.getDatafileIDs(filename=filename)
            if datafileIDs:
                datafileID = datafileIDs[0]
                if not dx:
                    dx = DataExtractor(filename_full)
                else:
                    # Already open
                    pass

                datafile = crdb.Datafile(self._db, id=datafileID)

                # Find station information
                # - Station names
                for stationname in dx.stationnames:
                    stationIDs = self.dbManager.getStationIDs(datafileID=datafile.id, stationname=stationname)
                    if not stationIDs:
                        # Create stations
                        station = crdb.Station(self._db)
                        station.stationname = stationname
                        station.write()
                        datafile.addStation(station)

                        # Create polarisations
                        for pol_direction in [0,1]:
                            p = crdb.Polarisation(self._db)
                            p.antennaset = dx.antennaset
                            p.direction = "%d" %(pol_direction)
                            p.resultsfile = dx.resultsfile(pol_direction)
                            print "Resultsfile: ", p.resultsfile
                            print "Resultspath: ", os.path.join(self.settings.resultspath, p.resultsfile)
                            p.write()   # Write to db and get ID
                            station.addPolarisation(p)


    def summary(self):
        """Summary of the CRDatabasePopulator object."""
        linewidth = 80

        print "="*linewidth
        print "  Summary of the CRDatabasePopulator object."
        print "="*linewidth

        print "  %-40s : %s" %("datapath", self.settings.datapath)
        print "  %-40s : %s" %("resultspath", self.settings.resultspath)

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
            result = int(seconds + round(subsec + 0.5))
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

        **Properties**

        =========  ===================================================
        Parameter  Datafile
        =========  ===================================================
        *pol*      polarisation direction.
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
    parser.add_option("-r", "--resultspath", type="str", dest="resultspath", default="", help="directory where the results files are written.")
    parser.add_option("-f", "--filefilter", type="str", dest="filefilter", default="", help="filter to select which files are processed.")

    (options, args) = parser.parse_args()

    if len(args) != 1:
        parser.error("Incorrect number of arguments.")

    options.basepath = os.path.dirname(os.path.abspath(args[0]))

    print "datapath = %s" %(options.datapath)
    print "resultspath = %s" %(options.resultspath)
    print "filefilter = %s" %(options.filefilter)

    return (options, args)


if __name__ == '__main__':
    (options, args) = parseOptions()

    # Load all packages after processing the commandline
    import pycrtools as cr
    from pycrtools import crdatabase as crdb

    # Populate the database
    dbp = CRDatabasePopulator(db_filename=args[0], options=options)
    dbp.populate()

    # Give a summary of the database
    dbp.dbManager.summary()

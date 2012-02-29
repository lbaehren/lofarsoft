"""
Interface classes to communicate with the Cosmic Ray Pipeline Database.

:Author: Martin van den Akker <martinva@astro.ru.nl>
"""


from pycrtools.io import database as db
import os


class CRDatabase(object):
    """Functionality to let the VHECR pipeline communicate with an SQL database."""

    def __init__(self, filename=":memory:", datapath="", resultspath=""):
        """Initialisation of the CRDatabase object.

        **Properties**

        ============= =====================================
        Parameter     Description
        ============= =====================================
        *filename*    filename of the database.
        *datapath*    path where the datafiles are stored.
        *resultspath* path where the results are stored.
        ============= =====================================

        If *inputpath* is an empty string the ''data'' directory of
        the directory in which the database file is located is used.

        If *outputpath* is an empty string the ''results'' directory of
        the directory in which the database file is located is used.
        """

        # Full filename of the datapath
        self.filename = os.path.realpath(filename)

        # Database object
        self.db = db.Database(self.filename)

        # Initialize database structure
        self.db.open()
        self.__createTables()

        # Settings
        self.settings = Settings(self.db)

        # Path settings
        self.basepath = os.path.dirname(self.filename)

        # Location of the datapath
        if "" != datapath:
            self.settings.datapath = os.path.realpath(datapath)
        elif "" == self.settings.datapath:
            self.settings.datapath = self.basepath + "/data"

        # Location of the resultspath
        if "" != resultspath:
            self.settings.resultspath = os.path.realpath(resultspath)
        elif "" == self.settings.resultspath:
            self.settings.resultspath = self.basepath + "/results"


    def __createTables(self):
        """Create the pipeline database tables if they not already exist."""

        if self.db:
            # Event table
            sql = "CREATE TABLE IF NOT EXISTS main.events (eventID INTEGER PRIMARY KEY, timestamp INTEGER, status TEXT)"
            self.db.execute(sql)

            # Event properties table
            sql = "CREATE TABLE IF NOT EXISTS main.eventproperties (propertyID INTEGER PRIMARY KEY, eventID INTEGER NOT NULL, key TEXT, value TEXT)"
            self.db.execute(sql)

            # Datafile table
            sql = "CREATE TABLE IF NOT EXISTS main.datafiles (datafileID INTEGER PRIMARY KEY, filename TEXT UNIQUE, status TEXT)"
            self.db.execute(sql)

            # Stations table
            sql = "CREATE TABLE IF NOT EXISTS main.stations (stationID INTEGER PRIMARY KEY, stationname TEXT, status TEXT)"
            self.db.execute(sql)

            # Polarisations table
            sql = "CREATE TABLE IF NOT EXISTS main.polarisations (polarisationID INTEGER PRIMARY KEY, antennaset TEXT, direction TEXT, status TEXT, resultsfile TEXT)"
            self.db.execute(sql)

            # Polarisation properties table
            sql = "CREATE TABLE IF NOT EXISTS main.polarisationproperties (propertyID INTEGER PRIMARY KEY, polarisationID INTEGER NOT NULL, key TEXT, value TEXT)"
            self.db.execute(sql)

            # event_datafile table (linking events to datafiles)
            sql = "CREATE TABLE IF NOT EXISTS main.event_datafile (eventID INTEGER NOT NULL, datafileID INTEGER NOT NULL UNIQUE)"
            self.db.execute(sql)

            # datafile_station table (linking datafiles to stations)
            sql = "CREATE TABLE IF NOT EXISTS main.datafile_station (datafileID INTEGER NOT NULL, stationID INTEGER NOT NULL UNIQUE)"
            self.db.execute(sql)

            # station_polarisation table (linking stations to polarisations)
            sql = "CREATE TABLE IF NOT EXISTS main.station_polarisation (stationID INTEGER NOT NULL, polarisationID INTEGER NOT NULL UNIQUE)"
            self.db.execute(sql)

            # settings table
            sql = """
            CREATE TABLE IF NOT EXISTS main.settings (key TEXT NOT NULL UNIQUE, value TEXT);
            INSERT OR IGNORE INTO main.settings (key, value) VALUES ('datapath', '');
            INSERT OR IGNORE INTO main.settings (key, value) VALUES ('resultspath', '');
            """
            self.db.executescript(sql)

            # Status table
            sql = """
            CREATE TABLE IF NOT EXISTS main.status (statusID INTEGER UNIQUE, status TEXT NOT NULL);
            INSERT OR IGNORE INTO main.status (statusID, status) VALUES (-1, 'NEW');
            INSERT OR IGNORE INTO main.status (statusID, status) VALUES ( 0, 'UNKNOWN');
            INSERT OR IGNORE INTO main.status (statusID, status) VALUES ( 1, 'PROCESSED');
            """
            self.db.executescript(sql)

            # Filters table
            sql = "CREATE TABLE IF NOT EXISTS main.filters (name TEXT, filter TEXT NOT NULL UNIQUE);"
            self.db.execute(sql)

        else:
            raise ValueError("Unable to read from database: no database was set.")


    def __resetTables(self):
        """Reset all tables and create a clean table structure."""

        if self.db:
            # Remove all tables.
            self.db.execute("DROP TABLE IF EXISTS main.events")
            self.db.execute("DROP TABLE IF EXISTS main.eventproperties")
            self.db.execute("DROP TABLE IF EXISTS main.datafiles")
            self.db.execute("DROP TABLE IF EXISTS main.stations")
            self.db.execute("DROP TABLE IF EXISTS main.polarisations")
            self.db.execute("DROP TABLE IF EXISTS main.polarisationproperties")
            self.db.execute("DROP TABLE IF EXISTS main.event_datafile")
            self.db.execute("DROP TABLE IF EXISTS main.datafile_station")
            self.db.execute("DROP TABLE IF EXISTS main.station_polarisation")
            self.db.execute("DROP TABLE IF EXISTS main.filters")
            self.db.execute("DROP TABLE IF EXISTS main.status")

            # Database needs to be closed to properly drop the tables before creating them again.
            self.db.close()
            self.db.open()

            # Create all tables.
            self.__createTables()
        else:
            raise ValueError("Unable to read from database: no database was set.")


    def getEventIDs(self, timestamp=None, timestamp_start=None, timestamp_end=None, status=None):
        """Return a list of eventIDs satifying the values of this
        functions arguments.

        **Properties**

        =================  ==============================================================
        Parameter          Description
        =================  ==============================================================
        *timestamp*        timestamp of the event is equal to this value.
        *timestamp_start*  timestamp of the event is larger than this value.
        *timestamp_end*    timestamp of the event is smaller than this value.
        *status*           status of the event.
        =================  ==============================================================

        The *timestamp*, *timestamp_start* and *timestamp_end*
        parameters are integers representing the unix-time of the
        timestamp.  If *timestamp* is provided, than *timestamp_start*
        and *timestamp_end* are ignored.  If no arguments are given
        all eventIDs are selected.

        When multiple arguments are provided, all returned eventIDs
        satisfy all argument values.
        """
        result = []

        if self.db:
            # Processing selection criteria
            sql_selection = []
            if timestamp:
                sql_selection.append("timestamp={0}".format(int(timestamp)))
            else:
                if timestamp_start:
                    sql_selection.append("timestamp>{0}".format(int(timestamp_start)))
                if timestamp_end:
                    sql_selection.append("timestamp<{0}".format(int(timestamp_end)))
            if status:
                sql_selection.append("status='{0}'".format(str(status)))

            # Building SQL expression
            sql = "SELECT eventID FROM main.events"
            if len(sql_selection) > 0:
                sql += " WHERE "
                for i in range(len(sql_selection)):
                    sql += sql_selection[i]
                    if i < len(sql_selection) - 1:
                        sql += " AND "

            # Extracting eventIDs
            records = self.db.select(sql)
            for record in records:
                result.append(record[0])
        else:
            raise ValueError("Unable to read from database: no database was set.")

        return result


    def getDatafileIDs(self, filename=None, status=None):
        """Return a list of datafileIDs satifying the values of this
        functions arguments.

        **Properties**

        =================  ==============================================================
        Parameter          Description
        =================  ==============================================================
        *filename*         name of the datafile.
        *status*           status of the datafile.
        =================  ==============================================================

        If no arguments are given all datafileIDs are selected. When
        multiple arguments are provided, all returned datafileIDs satisfy
        all argument values.
        """
        result = []

        if self.db:
            # Processing selection criteria
            sql_selection = []
            if filename:
                sql_selection.append("filename='{0}'".format(str(filename)))
            if status:
                sql_selection.append("status='{0}'".format(str(status)))

            # Building SQL expression
            sql = "SELECT datafileID FROM main.datafiles"
            if len(sql_selection) > 0:
                sql += " WHERE "
                for i in range(len(sql_selection)):
                    sql += sql_selection[i]
                    if i < len(sql_selection) - 1:
                        sql += " AND "

            # Extracting datafileIDs
            records = self.db.select(sql)
            for record in records:
                result.append(record[0])
            result = [record[0] for record in self.db.select(sql)]
        else:
            raise ValueError("Unable to read from database: no database was set.")

        return result


    def getStationIDs(self, eventID=None, datafileID=None, stationname=None, status=None):
        """Return a list of stationIDs that satisfy the values of the
        provided arguments of this method.

        **Properties**

        =================  ==============================================================
        Parameter          Description
        =================  ==============================================================
        *eventID*          id of the event.
        *datafileID*       id of the datafile.
        *stationname*      name of the station.
        *status*           status of the station.
        =================  ==============================================================

        If no arguments are given all stationIDs are selected. When
        multiple arguments are provided, the returned stationIDs satisfy
        all argument values that are provided.
        """
        results = []
        records = []

        if self.db:
            sql_fields = ""
            sql_table = ""
            sql_selection = []
            if eventID:                 # EventID
                sql_fields = "ed.eventID, s.stationID "
                sql_table = "event_datafile AS ed INNER JOIN (datafile_station AS ds INNER JOIN stations AS s ON s.stationID=dsstationID) ON ds.datafileID=ed.datafileID"
                sql_selection.append("ed.eventID={0}".format(eventID))
            elif datafileID:            # DatafileID
                sql_fields = "s.stationID"
                sql_table = "datafile_station AS ds INNER JOIN stations AS s ON ds.stationID=s.stationID"
                sql_selection.append("ds.datafileID={0}".format(datafileID))
            # Other selections
            if stationname:
                sql_selection.append("s.stationname='{0}'".format(stationname))
            if status:
                sql_selection.append("s.status='{0}'".format(status))

            sql = "SELECT {0} FROM {1}".format(sql_fields, sql_table)
            if len(sql_selection) > 0:
                sql += " WHERE "
                for i in range(len(sql_selection)):
                    sql += sql_selection[i]
                    if i < len(sql_selection) - 1:
                        sql += " AND "

            records = self.db.select(sql)

        if records:
            results = [r[0] for r in records]

        return results


    def summary(self):
        """Summary of the CRDatabase object."""
        linewidth = 80

        print "="*linewidth
        print "  Summary of the CRDatabase object."
        print "="*linewidth

        if self.db:
            dbstatus = "opened"
        else:
            dbstatus = "closed"
        print "  %-40s : '%s'" %("Database", dbstatus)

        print "  %-40s : '%s'" %("Filename", self.filename)
        print "  %-40s : '%s'" %("Base path", self.basepath)
        print "  %-40s : '%s'" %("Data path", self.settings.datapath)
        print "  %-40s : '%s'" %("Results path", self.settings.resultspath)

        print "-"*linewidth

        # Events
        if self.db:
            sql = "SELECT COUNT(eventID) AS nevents FROM main.events"
            n_events = self.db.select(sql)[0][0]
            print "  %-40s : %d" %("Nr. of events", n_events)

        # Datafiles
        if self.db:
            sql = "SELECT COUNT(datafileID) AS ndatafiles FROM main.datafiles"
            n_datafiles = self.db.select(sql)[0][0]
            print "  %-40s : %d" %("Nr. of datafiles", n_datafiles)

        # Stations
        if self.db:
            sql = "SELECT COUNT(stationID) AS nstations FROM main.stations"
            n_stations = self.db.select(sql)[0][0]
            print "  %-40s : %d" %("Nr. of stations", n_stations)

        # Polarisations
        if self.db:
            sql = "SELECT COUNT(polarisationID) AS npolarisations FROM main.polarisations"
            n_polarisations = self.db.select(sql)[0][0]
            print "  %-40s : %d" %("Nr. of polarisations", n_polarisations)

        print "-"*linewidth

        # Filters
        if self.db:
            # Number of filter groups
            sql = "SELECT COUNT(DISTINCT name) FROM main.filters"
            result = self.db.select(sql)
            n_filtergroups = result[0][0]
            print "  %-40s : %d" %("Nr. of filter groups", n_filtergroups)

            # Number of filters
            sql = "SELECT COUNT(name) FROM main.filters"
            result = self.db.select(sql)
            n_filters = result[0][0]
            print "  %-40s : %d" %("Nr. of filters", n_filters)

        print "="*linewidth



class Settings(object):
    """Global settings of the CR database."""

    def __init__(self, db=None):
        """Initialisation of the settings.

        **Properties**

        =========  ===================================================
        Parameter  Description
        =========  ===================================================
        *db*       database object to which to link the settings to.
        =========  ===================================================
        """
        if db:
            self._db = db
        else:
            raise ValueError("Unable to set database: no database was provided.")


    @property
    def datapath(self):
        """Get the value of the datapath variable as set in the database."""
        result = None

        if self._db:
            sql = "SELECT value FROM main.settings WHERE key='datapath'"
            result = str(self._db.select(sql)[0][0])
        else:
            raise ValueError("Unable to read from database: no database was set.")

        return result


    @datapath.setter
    def datapath(self, value):
        """Set the value of the datapath variable in the database.

        **Properties**

        =========  ===========================================
        Parameter  Description
        =========  ===========================================
        *value*    new value of the *datapath* variable.
        =========  ===========================================
        """
        if self._db:
            sql = "UPDATE main.settings SET value='{1}' WHERE key='{0}'".format('datapath', str(value))
            self._db.execute(sql)
        else:
            raise ValueError("Unable to read from database: no database was set.")


    @property
    def resultspath(self):
        """Get the value of the resultspath variable as set in the database."""
        result = None

        if self._db:
            sql = "SELECT value FROM main.settings WHERE key='resultspath'"
            result = str(self._db.select(sql)[0][0])
        else:
            raise ValueError("Unable to read from database: no database was set.")

        return result


    @resultspath.setter
    def resultspath(self, value):
        """Set the value of the resultspath variable in the database.

        **Properties**

        =========  ===========================================
        Parameter  Description
        =========  ===========================================
        *value*    new value of the *resultspath* variable.
        =========  ===========================================
        """
        if self._db:
            sql = "UPDATE main.settings SET value='{1}' WHERE key='{0}'".format('resultspath',str(value))
            self._db.execute(sql)
        else:
            raise ValueError("Unable to read from database: no database was set.")


    def summary(self):
        """Summary of the Settings object."""
        linewidth = 80

        print "="*linewidth
        print "  Summary of the Settings object."
        print "="*linewidth

        print "  %-40s : %s" %("datapath", self.datapath)
        print "  %-40s : %s" %("resultspath", self.resultspath)



class Event(object):
    """CR event information."""

    def __init__(self, db=None, id=0):
        """Initialisation of the Event object.

        **Properties**

        ========== ===========================================
        Parameter  Description
        ========== ===========================================
        *db*       database to which to link this event to.
        *id*       id of the new event.
        ========== ===========================================
        """
        self._db = db
        self._id = id

        # Set default values
        self.timestamp = 0
        self.status = "NEW"
        self.datafiles = []
        self.property = {}

        self.settings = Settings(db)

        # Initialize attributes
        if self.inDatabase():           # Read from database
            self.read()


    def __repr__(self):
        return "EventID=%d   timestampe=%d   status='%s'" %(self._id, self.timestamp, self.status)


    def read(self):
        """Read event information from the database."""
        if self._db:
            if self.inDatabase():
                # Read attributes
                sql = "SELECT eventID, timestamp, status FROM main.events WHERE eventID={0}".format(int(self._id))
                records = self._db.select(sql)
                if len(records) == 1:
                    self._id = int(records[0][0])
                    self.timestamp = int(records[0][1])
                    self.status = str(records[0][2])
                elif len(records) == 0:
                    raise ValueError("No records found for eventID={0}".format(self._id))
                else:
                    raise ValueError("Multiple records found for eventID={0}".format(self._id))

                # Read datafiles
                self.datafiles = []
                sql = "SELECT datafileID FROM main.event_datafile WHERE eventID={0}".format(self._id)
                records = self._db.select(sql)
                for record in records:
                    datafileID = int(record[0])
                    datafile = Datafile(self._db, id=datafileID)
                    datafile.event = self
                    self.datafiles.append(datafile)

                # Reading properties
                sql = "SELECT key, value FROM main.eventproperties WHERE eventID={0}".format(self._id)
                records = self._db.select(sql)
                for record in records:
                    self.property[str(record[0])] = record[1]
            else:
                print "WARNING: This event (id={0}) is not available in the database.".format(self._id)
        else:
            raise ValueError("Unable to read from database: no database was set.")


    def write(self, recursive=True):
        """Write event information to the database.

        **Properties**

        ===========  =================================================================
        Parameter    Description
        ===========  =================================================================
        *recursive*  if *True* write all underlying datastructures (datafiles, etc.)
        ===========  =================================================================
        """
        if self._db:
            # Writing attributes
            if self.inDatabase():       # Update values
                sql = "UPDATE main.events SET timestamp={1}, status='{2}' WHERE eventID={0}".format(self._id, int(self.timestamp), str(self.status))
                self._db.execute(sql)
            else:                       # Create new record
                if self._id == 0:
                    sql = "INSERT INTO main.events (timestamp, status) VALUES ({0}, '{1}')".format(int(self.timestamp), str(self.status))
                else:
                    sql = "INSERT INTO main.events (eventID, timestamp, status) VALUES ({0}, {1}, '{2}')".format(self._id, int(self.timestamp), str(self.status))
                self._id = self._db.insert(sql)

            # Writing datafile information
            for datafile in self.datafiles:
                datafileID = datafile.id
                if recursive:
                    datafile.write()

                sql = "SELECT COUNT(eventID) FROM main.event_datafile WHERE datafileID={0}".format(datafileID)
                result = self._db.select(sql)[0][0]
                if result == 0:
                    sql = "INSERT INTO main.event_datafile (eventID, datafileID) VALUES ({0}, {1})".format(self._id, datafileID)
                    self._db.insert(sql)

            # Writing properties
            sql = "SELECT key FROM main.eventproperties WHERE eventID={0}".format(self._id)
            db_keys = [record[0] for record in self._db.select(sql)]
            py_keys = [key for key in self.property]

            # - Insert/update properties
            for key in py_keys:
                if key in db_keys:
                    sql = "UPDATE main.eventproperties SET value='{2}' WHERE eventID={0} AND key='{1}'".format(self._id, str(key), str(self.property[key]))
                else:
                    sql = "INSERT INTO main.eventproperties (eventID, key, value) VALUES ({0}, '{1}', '{2}')".format(self._id, str(key), str(self.property[key]))
                self._db.execute(sql)

            # - delete unused properties
            for key in db_keys:
                if not key in py_keys:
                    sql = "DELETE FROM main.eventproperties WHERE eventID={0} AND key='{1}'".format(self._id, str(key))
                    self._db.execute(sql)

        else:
            raise ValueError("Unable to read from database: no database was set.")


    def inDatabase(self):
        """Check if event information is available in the database.

        This method checks if the event information of this object,
        uniquely identified by the *id* and the *timestamp*, is
        already in the database.

        If the event information is in the database *True* is
        returned, *False* otherwise.
        """
        result = False

        if self._db:
            sql = "SELECT eventID FROM main.events WHERE eventID={0}".format(int(self._id))
            records = self._db.select(sql)
            if len(records) > 0:
                result = True
        else:
            raise ValueError("Unable to read from database: no database was set.")

        return result


    @property
    def id(self):
        """Return the ID of the object as it is identified in the database."""
        return self._id


    def addDatafile(self, datafile=None):
        """Add a datafile object to this event.

        This adds a datafile object to the current event object and
        updates the database.

        **Properties**

        ===========  =====================================================
        Parameter    Description
        ===========  =====================================================
        *datafile*   datafile object to be linked to the current event.
        ===========  =====================================================

        Returns *True* if adding the datafile object went
        successfully, *False* otherwise.
        """
        result = False

        if datafile:
            # Update object
            datafileID = datafile.id

            # Check for duplicate
            isNew = True
            for d in self.datafiles:
                if d.id == datafileID:
                    isNew = False
                    break
            if isNew:
                self.datafiles.append(datafile)

            # Update database
            if self._db:
                # Add datafile object to database if it does not yet exist.
                if not datafile.inDatabase():
                    datafile.write()
                # Update the linking table.
                sql = "SELECT eventID FROM main.event_datafile WHERE eventID={0} AND datafileID={1}".format(self._id, datafileID)
                if len(self._db.select(sql)) == 0:
                    sql = "INSERT INTO main.event_datafile (eventID, datafileID) VALUES ({0}, {1})".format(self._id, datafileID)
                    self._db.insert(sql)
                    result = True
            else:
                raise ValueError("Unable to write to database: no database was set.")

        return result


    def removeDatafile(self, datafileID=0):
        """Remove datafile object with id= *datafileID* from this event.

        This removes the datafile information object from this event
        object and updates the database. Note that the datafile info
        is not deleted from the database: only the link between the
        event and the datafile objects is deleted.

        **Properties**

        ============  =============================================================
        Parameter     Description
        ============  =============================================================
        *datafileID*  id of the datafile that needs to be removed from this event.
        ============  =============================================================

        Returns *True* if removing the datafile object went
        successfully, *False* otherwise.
        """
        result = False

        if datafileID > 0:
            # Update object
            for d in self.datafiles:
                if d.id == datafileID:
                    self.datafiles.remove(d)

            # Update database
            if self._db:
                sql = "DELETE FROM main.event_datafile WHERE eventID={0} AND datafileID={1}".format(self._id, datafileID)
                self._db.execute(sql)
            else:
                raise ValueError("Unable to write to database: no database was set.")
        else:
            print "WARNING: invalid datafileID!"

        return result


    def summary(self):
        """Summary of the Event object."""
        linewidth = 80

        print "="*linewidth
        print "  Summary of the Event object."
        print "="*linewidth

        # Event info
        print "  %-40s : %d" %("ID", self._id)
        print "  %-40s : %s" %("Timestamp", self.timestamp)
        print "  %-40s : %s" %("Status", self.status)

        # Datafiles
        n_datafiles = len(self.datafiles)
        print "  %-40s : %d" %("Nr. of datafiles", n_datafiles)
        if n_datafiles > 0:
            print "  Datafiles:"
            for datafile in self.datafiles:
                print "    %-6d - %s" %(datafile.id, datafile.filename)

        # Properties
        if len(self.property) > 0:
            print "  Properties:"
            for key in self.property.keys():
                print "    %-38s : %s" %(key, self.property[key])

        print "="*linewidth



class Datafile(object):
    """CR datafile information."""

    def __init__(self, db=None, id=0):
        """Initialisation of Datafile object.

        **Properties**

        ========= ===========================================
        Parameter Description
        ========= ===========================================
        *db*      database to which to link this datafile to.
        *id*      id of the new datafile.
        ========= ===========================================
        """
        self._db = db
        self._id = id

        self.filename = ""
        self.status = "NEW"
        self.stations = []

        self.settings = Settings(db)

        # Initialize attributes
        if self.inDatabase():
            self.read()


    def __repr__(self):
        return "DatafileID=%d   filename='%s'   status='%s'" %(self._id, self.filename, self.status)


    def read(self):
        """Read datafile information from the database."""
        if self._db:
            if self.inDatabase():
                # Read attributes
                sql = "SELECT datafileID, filename, status FROM main.datafiles WHERE datafileID={0}".format(int(self._id))
                records = self._db.select(sql)
                if len(records) == 1:
                    self._id = int(records[0][0])
                    self.filename = str(records[0][1])
                    self.status = str(records[0][2])
                elif len(records) == 0:
                    raise ValueError("No records found for datafileID={0}".format(self._id))
                else:
                    raise ValueError("Multiple records found for datafileID={0}".format(self._id))

                # Read station information
                self.stations = []
                sql = "SELECT stationID FROM main.datafile_station WHERE datafileID={0}".format(self._id)
                records = self._db.select(sql)
                for record in records:
                    stationID = int(record[0])
                    station = Station(self._db, id=stationID)
                    station.datafile = self
                    self.stations.append(station)

            else:
                print "WARNING: This datafile (id={0}) is not available in the database.".format(self._id)
        else:
            raise ValueError("Unable to read from database: no database was set.")


    def write(self, recursive=True):
        """Write datafile information to the database.

        **Properties**

        ===========  =================================================================
        Parameter    Description
        ===========  =================================================================
        *recursive*  if *True* write all underlying datastructures (stations, etc.)
        ===========  =================================================================
        """
        if self._db:
            # Write attributes
            if self.inDatabase():
                sql = "UPDATE main.datafiles SET filename='{1}', status='{2}' WHERE datafileID={0}".format(self._id, str(self.filename), str(self.status))
                self._db.execute(sql)
            else:
                # Check uniqueness (filename)
                sql = "SELECT datafileID FROM main.datafiles WHERE filename='{0}'".format(self.filename)
                if len(self._db.select(sql)) > 0:
                    print "ERROR: Filename is not uniquely defined"
                    return
                # Add to database
                if self._id == 0:
                    sql = "INSERT INTO main.datafiles (filename, status) VALUES ('{0}', '{1}')".format(str(self.filename), str(self.status))
                else:
                    sql = "INSERT INTO main.datafiles (datafileID, filename, status) VALUES ({0}, '{1}', '{2}')".format(self._id, str(self.filename), str(self.status))
                self._id = self._db.insert(sql)

            # Write station information
            for station in self.stations:
                stationID = station.id
                if recursive:
                    station.write()

                sql = "SELECT COUNT(datafileID) FROM main.datafile_station WHERE stationID={0}".format(stationID)
                result = self._db.select(sql)[0][0]
                if result == 0:
                    sql = "INSERT INTO main.datafile_station (datafileID, stationID) VALUES ({0}, {1})".format(self._id, stationID)
                    self._db.insert(sql)

        else:
            raise ValueError("Unable to read from database: no database was set.")


    def inDatabase(self):
        """Check if the datafile information is available in the database.

        This method checks if the datafile information of this object,
        uniquely identified by the *id* and the *filename*, is in the
        database.

        If the datafile information is in the database *True* is
        returned, *False* otherwise.
        """
        result = False

        if self._db:
            sql = "SELECT datafileID FROM main.datafiles WHERE datafileID={0}".format(int(self._id))
            records = self._db.select(sql)
            if len(records) > 0:
                result = True
        else:
            raise ValueError("Unable to read from database: no database was set.")

        return result


    @property
    def id(self):
        """Return the ID of the object as it is identified in the database."""
        return self._id


    def addStation(self, station=None):
        """Add a station object to this event.

        This adds a station object to the list of stations of the
        current datafile object and updates the database.

        **Properties**

        =========  ================================================================================
        Parameter  Description
        =========  ================================================================================
        *station*  station object to be added to the list of stations of this datafile object.
        =========  ================================================================================

        Returns *True* if adding the station object went successfully,
        *False* otherwise.
        """
        result = False

        if station:
            # Update object
            stationID = station.id

            # Check for duplicate
            isNew = True
            for s in self.stations:
                if s.id == stationID:
                    isNew = False
                    break
            if isNew:
                self.stations.append(station)

            # Update database
            if self._db:
                # Add station object to database if it does not yet exist.
                if not station.inDatabase():
                    station.write()
                # Update linking table
                sql = "SELECT datafileID FROM main.datafile_station WHERE datafileID={0} AND stationID={1}".format(self._id, stationID)
                if len(self._db.select(sql)) == 0:
                    sql = "INSERT INTO main.datafile_station (datafileID, stationID) VALUES ({0}, {1})".format(self._id, stationID)
                    self._db.insert(sql)
                    result = True
            else:
                raise ValueError("Unable to read from database: no database was set.")

        return result


    def removeStation(self, stationID=0):
        """Remove station object with id= *stationID* from this datafile.

        This removes the station information object from this datafile
        object and updates the database. Note that the station info
        is not deleted from the database: only the link between the
        datafile and the station objects is deleted.

        **Properties**

        ============  ===============================================================
        Parameter     Description
        ============  ===============================================================
        *stationID*   id of the station that needs to be removed from this datafile.
        ============  ===============================================================

        Returns *True* if removing the station object went
        successfully, *False* otherwise.
        """
        result = False

        if stationID > 0:
            # Update object
            for s in self.stations:
                if s.id == stationID:
                    self.stations.remove(s)

            # Update database
            if self._db:
                sql = "DELETE FROM main.datafile_station WHERE datafileID={0} AND stationID={1}".format(self._id, stationID)
                self._db.execute(sql)
            else:
                raise ValueError("Unable to write to database: no database was set.")
        else:
            print "WARNING: invalid stationID!"

        return result


    def summary(self):
        """Summary of the Datafile object."""
        linewidth = 80

        print "="*linewidth
        print "  Summary of the Datafile object."
        print "="*linewidth

        # Datafile info
        print "  %-40s : %s" %("ID", self._id)
        print "  %-40s : %s" %("Filename", self.filename)
        print "  %-40s : %s" %("Status", self.status)
        # print "  %-40s : %s" %("Timestamp", self.timestamp)
        # print "  %-40s : %s" %("Results path", self.resultsfile)

        # Stations
        n_stations = len(self.stations)
        print "  %-40s : %d" %("Number of stations", n_stations)
        if n_stations > 0:
            print "  Stations:"
            for station in self.stations:
                print "  %-6d - %s" %(station.id, station.stationname)

        print "="*linewidth



class Station(object):
    """CR station information"""

    def __init__(self, db=None, id=0):
        """Initialisation of Station object.

        **Properties**

        ========= ===========================================
        Parameter Description
        ========= ===========================================
        *db*      database to which to link this datafile to.
        *id*      id of the new station.
        ========= ===========================================
        """
        self._db = db
        self._id = id

        self.stationname = ""
        self.status = "NEW"
        self.polarisations = []

        self.settings = Settings(db)

        if self.inDatabase():
            self.read()


    def __repr__(self):
        return "StationID=%d   stationname='%s'   status='%s'" %(self._id, self.stationname, self.status)


    def read(self):
        """Read station information from the database."""
        if self._db:
            if self.inDatabase():
                # Read attributes
                sql = "SELECT stationID, stationname, status FROM main.stations WHERE stationID={0}".format(int(self._id))
                records = self._db.select(sql)
                if len(records) == 1:
                    self._id = int(records[0][0])
                    self.stationname = str(records[0][1])
                    self.status = str(records[0][2])
                elif len(records) == 0:
                    raise ValueError("No records found for stationID={0}".format(self._id))
                else:
                    raise ValueError("Multiple records found for stationID={0}".format(self._id))

                # Read polarisation information
                self.polarisations = []
                sql = "SELECT polarisationID FROM main.station_polarisation WHERE stationID={0}".format(self._id)
                records = self._db.select(sql)
                for record in records:
                    polarisationID = int(record[0])
                    polarisation = Polarisation(self._db, id=polarisationID)
                    polarisation.station = self
                    self.polarisations.append(polarisation)

            else:
                print "WARNING: This station (id={0}) is not available in the database.".format(self._id)
        else:
            raise ValueError("Unable to read from database: no database was set.")


    def write(self, recursive=True):
        """Write station information to the database.

        **Properties**

        ===========  =================================================================
        Parameter    Description
        ===========  =================================================================
        *recursive*  if *True* write all underlying datastructures (polarisations)
        ===========  =================================================================
        """
        if self._db:
            # Write attributes
            if self.inDatabase():
                sql = "UPDATE main.stations SET stationname='{1}', status='{2}' WHERE stationID={0}".format(self._id, str(self.stationname), str(self.status))
                self._db.execute(sql)
            else:
                if self._id == 0:
                    sql = "INSERT INTO main.stations (stationname, status) VALUES ('{0}', '{1}')".format(str(self.stationname), str(self.status))
                else:
                    sql = "INSERT INTO main.stations (stationID, stationname, status) VALUES ({0}, '{1}', '{2}')".format(self._id, str(self.stationname), str(self.status))
                self._id = self._db.insert(sql)

            # Write polarisation information
            for polarisation in self.polarisations:
                polarisationID = polarisation.id
                if recursive:
                    polarisation.write()

                sql = "SELECT COUNT(stationID) FROM main.station_polarisation WHERE polarisationID={0}".format(polarisationID)
                result = self._db.select(sql)[0][0]
                if result == 0:
                    sql = "INSERT INTO main.station_polarisation (stationID, polarisationID) VALUES ({0}, {1})".format(self._id, polarisationID)
                    self._db.insert(sql)

        else:
            raise ValueError("Unable to read from database: no database was set.")


    def inDatabase(self):
        """Check if the station information is available in the database.

        This method checks if the station information of this object,
        uniquely identified by *id*, is in the database.

        If the station information is in the database *True* is
        returned, *False* otherwise.
        """
        result = False

        if self._db:
            sql = "SELECT stationID FROM main.stations WHERE stationID={0}".format(int(self._id))
            records = self._db.select(sql)
            if len(records) > 0:
                result = True
        else:
            raise ValueError("Unable to read from database: no database was set.")

        return result


    @property
    def id(self):
        """Return the ID of the object as it is identified in the database."""
        return self._id


    def addPolarisation(self, polarisation=None):
        """Add a polarisation object to this station.

        This adds a polarisation object to the current station object and
        updates the database.

        **Properties**

        ===============  =========================================================
        Parameter        Description
        ===============  =========================================================
        *polarisation*   polarisation object to be linked to the current station.
        ===============  =========================================================

        Returns *True* if adding the polarisation object went
        successfully, *False* otherwise.
        """
        result = False

        if polarisation:
            # Update object
            polarisationID = polarisation.id

            # Check for duplicate
            isNew = True
            for p in self.polarisations:
                if p.id == polarisationID:
                    isNew = False
                    break
            if isNew:
                self.polarisations.append(polarisation)

            # Update database
            if self._db:
                # Add polarisation object to database if it does not yet exist.
                if not polarisation.inDatabase():
                    polarisation.write()
                # Update linking table
                sql = "SELECT stationID FROM main.station_polarisation WHERE stationID={0} AND polarisationID={1}".format(self._id, polarisationID)
                if len(self._db.select(sql)) == 0:
                    sql = "INSERT INTO main.station_polarisation (stationID, polarisationID) VALUES ({0}, {1})".format(self._id, polarisationID)
                    self._db.insert(sql)
                    result = True
            else:
                raise ValueError("Unable to write to database: no database was set.")

        return result


    def removePolarisation(self, polarisationID=0):
        """Remove polarisation object with id= *polarisationID* from
        this station.

        This removes the polarisation information object from this station
        object and updates the database. Note that the polarisation info
        is not deleted from the database: only the link between the
        station and the polarisation objects is deleted.

        **Properties**

        ================  ===================================================================
        Parameter         Description
        ================  ===================================================================
        *polarisationID*  id of the polarisation that needs to be removed from this station.
        ================  ===================================================================

        Returns *True* if removing the polarisation object went
        successfully, *False* otherwise.
        """
        result = False

        if polarisationID > 0:
            # Update object
            for p in self.polarisations:
                if p.id == polarisationID:
                    self.polarisations.remove(p)

            # Update database
            if self._db:
                sql = "DELETE FROM main.station_polarisation WHERE stationID={0} AND polarisationID={1}".format(self._id, polarisationID)
                self._db.execute(sql)
            else:
                raise ValueError("Unable to write to database: no database was set.")
        else:
            print "WARNING: invalid polarisationID!"

        return result


    def summary(self):
        """Summary of the Station object."""
        linewidth = 80

        print "="*linewidth
        print "  Summary of the Station object."
        print "="*linewidth

        # Station information
        print "  %-40s : %d" %("ID", self._id)
        print "  %-40s : %s" %("Station name", self.stationname)
        print "  %-40s : %s" %("Status", self.status)

        # Polarisations
        n_polarisations = len(self.polarisations)
        print "  %-40s : %d" %("Nr. of polarisations",n_polarisations)
        if n_polarisations > 0:
            print "Polarisations:"
            for polarisation in self.polarisations:
                print "  %-6d - %s" %(polarisation.id, polarisation.resultsfile)
            pass

        print "="*linewidth



class Polarisation(object):
    """CR polarisation information."""

    def __init__(self, db=None, id=0):
        """Initialisation of the Polarisation object.

        **Properties**

        ========== ===========================================
        Parameter  Description
        ========== ===========================================
        *db*       database to which to link this event to.
        *id*       id of the new polarisation.
        ========== ===========================================
        """
        self._db = db
        self._id = id

        self.antennaset = ""
        self.direction = ""
        self.status = "NEW"
        self.resultsfile = ""
        self.property = {}

        self.settings = Settings(db)

        if self.inDatabase():
            self.read()


    def __repr__(self):
        return "polarisationID=%d   results='%s'   status='%s'" %(self._id, self.resultsfile, self.status)


    def read(self):
        """Read polarisation information from the database."""
        if self._db:
            if self.inDatabase():
                # Read attributes
                sql = "SELECT polarisationID, antennaset, direction, status, resultsfile FROM main.polarisations WHERE polarisationID={0}".format(int(self._id))
                records = self._db.select(sql)
                if len(records) == 1:
                    self._id = int(records[0][0])
                    self.antennaset = str(records[0][1])
                    self.direction = str(records[0][2])
                    self.status = str(records[0][3])
                    self.resultsfile = str(records[0][4])
                elif len(records) == 0:
                    raise ValueError("No records found for eventID={0}".format(self._id))
                else:
                    raise ValueError("Multiple records found for eventID={0}".format(self._id))

                # Read properties
                sql = "SELECT key, value FROM main.polarisationproperties WHERE polarisationID={0}".format(self._id)
                records = self._db.select(sql)
                for record in records:
                    self.property[str(record[0])] = record[1]

            else:
                print "WARNING: This polarisation (id={0}) is not available in the database.".format(self._id)

        else:
            raise ValueError("Unable to read from database: no database was set.")


    def write(self):
        """Write polarisation information to the database."""
        if self._db:
            # Write attributes
            if self.inDatabase():
                sql = "UPDATE main.polarisations SET antennaset='{1}', direction='{2}', status='{3}', resultsfile='{4}' WHERE polarisationID={0}".format(self._id, str(self.antennaset), str(self.direction), str(self.status), str(self.resultsfile))
                self._db.execute(sql)
            else:
                if self._id == 0:
                    sql = "INSERT INTO main.polarisations (antennaset, direction, status, resultsfile) VALUES ('{0}', '{1}', '{2}', '{3}')".format(str(self.antennaset), str(self.direction), str(self.status), str(self.resultsfile))
                else:
                    sql = "INSERT INTO main.polarisations (polarisationID, antennaset, direction, status, resultsfile) VALUES ({0}, '{1}', '{2}', '{3}', '{4}')".format(self._id, str(self.antennaset), str(self.direction), str(self.status), str(self.resultsfile))
                self._id = self._db.insert(sql)

            # Writing properties
            sql = "SELECT key FROM main.polarisationproperties WHERE polarisationID={0}".format(self._id)
            db_keys = [record[0] for record in self._db.select(sql)]
            py_keys = [key for key in self.property]

            # - Insert/update properties
            for key in py_keys:
                if key in db_keys:
                    sql = "UPDATE main.polarisationproperties SET value='{2}' WHERE polarisationID={0} AND key='{1}'".format(self._id, str(key), str(self.property[key]))
                else:
                    sql = "INSERT INTO main.polarisationproperties (polarisationID, key, value) VALUES ({0}, '{1}', '{2}')".format(self._id, str(key), str(self.property[key]))
                self._db.execute(sql)

            # - delete unused properties
            for key in db_keys:
                if not key in py_keys:
                    sql = "DELETE FROM main.polarisationproperties WHERE polarisationID={0} AND key='{1}'".format(self._id, str(key))
                    self._db.execute(sql)

        else:
            raise ValueError("Unable to read from database: no database was set.")


    def inDatabase(self):
        """Check if the polarisation information is available in the
        database.

        This method checks if the polarisation information of this
        object, uniquely identified by *id*, is in the database.

        If the polarisation information is in the database *True* is
        returned, *False* otherwise.
        """
        result = False

        if self._db:
            sql = "SELECT polarisationID FROM main.polarisations WHERE polarisationID={0}".format(int(self._id))
            records = self._db.select(sql)
            if len(records) > 0:
                result = True
        else:
            raise ValueError("Unable to read from database: no database was set.")

        return result


    @property
    def id(self):
        """Return the ID of the object as it is identified in the database."""
        return self._id


    def summary(self):
        """Summary of the Polarisation object."""
        linewidth = 80

        print "="*linewidth
        print "  Summary of the Polarisation object."
        print "="*linewidth

        # Polarisation information
        print "  %-40s : %d" %("ID", self._id)
        print "  %-40s : %s" %("Antennaset", self.antennaset)
        print "  %-40s : %s" %("Direction", self.direction)
        print "  %-40s : %s" %("Status", self.status)
        print "  %-40s : %s" %("Results file", self.resultsfile)

        # Properties
        if len(self.property) > 0:
            print "  Properties:"
            for key in self.property.keys():
                print "    %-38s : %s" %(key, self.property[key])

        print "="*linewidth



class Filter(object):
    """Filter"""

    def __init__(self, db=None, name="DEFAULT"):
        """Initialisation of Filter object.

        **Properties**

        ========== ==============================================
        Parameter  Description
        ========== ==============================================
        *db*       database in which the filters are stored.
        *name*     name of the filter.
        ========== ==============================================
        """
        self._db = db
        self.name = name
        self.filters = []


    def __repr__(self):
        return "name='%s'" %(self.name)


    def read(self):
        """Read filter from the database."""
        self.filters = []

        if self._db:
            sql = "SELECT filter FROM main.filters WHERE name='{0}'".format(self.name)
            for record in self._db.select(sql):
                self.filters.append(str(record[0]))
        else:
            raise ValueError("Unable to read from database: no database was set.")

        return self.filters


    def write(self):
        """Write filter to database."""
        if self._db:
            for filtervalue in self.filters:
                # Check if filtervalue already exists
                sql = "SELECT filter FROM main.filters WHERE name='{0}' AND filter='{1}'".format(self.name, filtervalue)
                n = len(self._db.select(sql))
                # Only write new filters
                if n == 0:
                    sql = "INSERT INTO main.filters (name, filter) VALUES ('{0}', '{1}');".format(self.name, filtervalue)
                    self._db.insert(sql)
        else:
            raise ValueError("Unable to read from database: no database was set.")


    def add(self, filtervalue=""):
        """Add a filtervalue to the database.

        **Properties**

        ===============  ========================
        Parameter        Description
        ===============  ========================
        *filtervalue*    content of the filter.
        ===============  ========================
        """
        # Write filtervalue to database
        if self._db:
            # Check if filtervalue already exists
            sql = "SELECT filter FROM main.filters WHERE name='{0}' AND filter='{1}'".format(self.name, filtervalue)
            n = len(self._db.select(sql))
            # Only write new filters
            if n == 0:
                sql = "INSERT INTO main.filters (name, filter) VALUES ('{0}', '{1}');".format(self.name, filtervalue)
                self._db.insert(sql)
        else:
            raise ValueError("Unable to read from database: no database was set.")

        # Add filtervalue to object
        if not filtervalue in self.filters:
            self.filters.append(filtervalue)


    def delete(self, filtervalue=""):
        """Delete a filtervalue from the database.

        **Properties**

        ===============  ========================
        Parameter        Description
        ===============  ========================
        *filtervalue*    content of the filter.
        ===============  ========================
        """
        if self._db:
            sql = "DELETE FROM main.filters WHERE name='{0}' AND filter='{1}';".format(self.name, filtervalue)
            self._db.execute(sql)
        else:
            raise ValueError("Unable to read from database: no database was set.")

        self.filters.remove(filtervalue)


    def execute(self, string=""):
        """Execute the filtering of a string.

        **Properties**

        ===========  =======================================================
        Parameter    Description
        ===========  =======================================================
        *string*     String with which the filters are tested.
        ===========  =======================================================

        Returns *True* if at least one subfilter matches a sunstring
        of *string*, *False* otherwise.
        """
        result = False

        if len(string) > 0:
            for f in self.filters:
                if f in string:
                    return True
        else:
            raise ValueError("Unable to check empty string.")

        return result


    def summary(self):
        """Summary of the filter object."""
        linewidth = 80

        print "="*linewidth
        print "  Summary of the Filter object."
        print "="*linewidth

        print "  %-40s : %s" %("Filter name", self.name)
        print "  %-40s : %d" %("Nr. of filters", len(self.filters))

        if len(self.filters) > 0:
            for f in self.filters:
                print "    %s" %(f)

        print "="*linewidth


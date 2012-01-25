from pycrtools.io import database as db
import os

## =============================================================================
##
##  Classes
##
## =============================================================================

class PipelineDatabase:
    """Functionality to let the VHECR pipeline communicate with an SQL database."""

    def __init__(self, filename=":memory:", datapath="", resultspath=""):
        """Initialisation of the PipelineDatabase object.

        **Properties**

        ============= =====================================
        Parameter     Description
        ============= =====================================
        *filename*    filename of the database
        *inputpath*   path where the datafiles are stored.
        *outputpath*  path where the results are stored.
        ============= =====================================

        If *inputpath* is an empty string the ''data'' directory of
        the directory in which the database file is located is used.

        If *outputpath* is an empty string the ''results'' directory of
        the directory in which the database file is located is used.
        """
        # Database object
        self._db = db.Database(filename)

        # Full filename of the database (i.e. including path)
        self._filename = os.path.realpath(filename)

        # base path, i.e. location of database file
        self.base_path = os.path.dirname(self._filename)

        # Location of the data path
        if (len(datapath) > 0):
            self.data_path = datapath
        else:
            self.data_path = self.base_path + "/data"

        # Location of the results path
        if (len(resultspath) > 0):
            self.results_path = resultspath
        else:
            self.results_path = self.base_path + "/results"

        # Create database structure
        self.__createTables()


    def __createTables(self):
        """Create the pipeline database tables if they not already exist."""
        # Check Event table
        sql = "CREATE TABLE IF NOT EXISTS main.events (eventID INTEGER PRIMARY KEY NOT NULL UNIQUE, status TEXT, timestamp TEXT, resultspath TEXT);"
        self._db.execute(sql)

        # Check EventProperties table
        sql = "CREATE TABLE IF NOT EXISTS main.eventproperties (propertyID TEXT NOT NULL UNIQUE, eventID INTEGER NOT NULL, key TEXT, value TEXT, type TEXT);"
        self._db.execute(sql)

        # Check Datafile table
        sql = "CREATE TABLE IF NOT EXISTS main.datafiles (datafileID TEXT PRIMARY KEY NOT NULL UNIQUE, status TEXT, timestamp TEXT, resultspath TEXT);"
        self._db.execute(sql)

        # Stations table
        sql = "CREATE TABLE IF NOT EXISTS main.stations (stationID INTEGER PRIMARY KEY NOT NULL UNIQUE, name TEXT, status TEXT)"
        self._db.execute(sql)

        # Polarisations table
        sql = "CREATE TABLE IF NOT EXISTS main.polarisations (polarisationID INTEGER PRIMARY KEY NOT NULL UNIQUE, status TEXT, pol_type INTEGER, pol_dir INTEGER, resultspath TEXT)"
        self._db.execute(sql)

        # PolarisationProperties
        sql = "CREATE TABLE IF NOT EXISTS main.polarisationproperties (propertyID TEXT NOT NULL UNIQUE, polarisationID INTEGER NOT NULL, key TEXT, value TEXT, type TEXT);"
        self._db.execute(sql)

        # Check Event_datafile table (linking Events and Datafiles tables)
        sql = "CREATE TABLE IF NOT EXISTS main.event_datafile (eventID INTEGER NOT NULL, datafileID TEXT NOT NULL UNIQUE);"
        self._db.execute(sql)

        # Datafile_station table (linking Datafiles and Stations tables)
        sql = "CREATE TABLE IF NOT EXISTS main.datafile_station (datafileID TEXT NOT NULL, stationID INTEGER NOT NULL)"
        self._db.execute(sql)

        # Station_polarisation table (linking Stations and Polarisations tables)
        sql = "CREATE TABLE IF NOT EXISTS main.station_polarisation (stationID INTEGER NOT NULL, polarisationID INTEGER NOT NULL)"
        self._db.execute(sql)

        # Check Filter table
        sql = "CREATE TABLE IF NOT EXISTS main.filters (name TEXT, filter TEXT NOT NULL UNIQUE);"
        self._db.execute(sql)


    def __reset(self):
        """Reset all tables and create a clean table structure."""
        # Remove all tables.
        self._db.execute("DROP TABLE IF EXISTS main.events")
        self._db.execute("DROP TABLE IF EXISTS main.eventproperties")
        self._db.execute("DROP TABLE IF EXISTS main.datafiles")
        self._db.execute("DROP TABLE IF EXISTS main.stations")
        self._db.execute("DROP TABLE IF EXISTS main.polarisations")
        self._db.execute("DROP TABLE IF EXISTS main.polarisationproperties")
        self._db.execute("DROP TABLE IF EXISTS main.event_datafile")
        self._db.execute("DROP TABLE IF EXISTS main.datafile_station")
        self._db.execute("DROP TABLE IF EXISTS main.station_polarisation")
        self._db.execute("DROP TABLE IF EXISTS main.filters")

        # Database needs to be closed to properly drop the tables before creating them again.
        self._db.close()
        self._db.open()

        # Create all tables.
        self.__createTables()



    def hasEvent(self, id=0):
        """Check if there is an event with the corresponding *id* in
        the database.

        **Properties**

        ========== ================================
        Parameter  Description
        ========== ================================
        *id*       id of the event to check.
        ========== ================================

        Returns *True* if an event with *id* exists, *False* otherwise.
        """
        result = False
        records = []

        if (id != 0):
            sql = "SELECT eventID FROM main.events WHERE eventID={0}".format(id)
            records = self._db.select(sql)

        if len(records) > 0:
            result = True

        return result


    def createEvent(self, id=0):
        """Add a new event to the database.

        **Properties**

        ========== ================================
        Parameter  Description
        ========== ================================
        *id*       id of the created event.
        ========== ================================

        Returns a new Event object with the specified *id*.  If the
        event with this *id* already exists in the database, the
        already existing event is returned.
        """
        event = Event(self._db, id=id)

        if not self.hasEvent(id):
            event.write()
        else:
            raise ValueError("Event with id=%d already exists." %(id))

        return event


    def readEventIDs(self, status=''):
        """Read datafile IDs from database that satisfy the provided status.

        **Properties**

        ========== ================================
        Parameter  Description
        ========== ================================
        *status*   status of the requested events.
        ========== ================================

        Returns a list of eventIDs. If the status is empty then all
        eventIDs are returned.
        """
        eventIDs = []

        if (status != ''):
            sql = "SELECT eventID FROM main.events WHERE status='{0}'".format(status)
        else:
            sql = "SELECT eventID FROM main.events"
        records = self._db.select(sql)

        for record in records:
            eventIDs.append(int(record[0]))

        return eventIDs


    def readEvent(self, id=0):
        """Read the event that satisfies the provided *id*.

        **Properties**

        ========== ================================
        Parameter  Description
        ========== ================================
        *id*       id of the requested event.
        ========== ================================

        Returns an Event object with the specified *id*.
        """
        event = Event(db=self._db, id=id)

        if self.hasEvent(id):
            event.read()
        else:
            raise ValueError("Event with id=%d does not exist." %(id))

        return event


    def deleteEvent(self, id=0):
        """Delete the event with the specified *id* from the database.

        **Properties**

        ========== ================================
        Parameter  Description
        ========== ================================
        *id*       id of the event to delete.
        ========== ================================
        """
        if self.hasEvent(id):
            sql = "DELETE FROM main.events WHERE eventID={0}".format(id)
            self._db.execute(sql)
        else:
            raise ValueError("Event with id=%d does not exist." %(id))



    def hasDatafile(self, id=''):
        """Check if there is a VHECR datafile with the corresponding
        *id* in the database.

        **Properties**

        ========== ================================
        Parameter  Description
        ========== ================================
        *id*       id of the datafile to check.
        ========== ================================

        Returns *True* if a VHECR datafile with *id* exists, *False*
        otherwise.
        """
        result = False
        records = []

        if (id != ''):
            sql = "SELECT datafileID FROM main.datafiles WHERE datafileID='{0}'".format(id)
            records = self._db.select(sql)

        if len(records) > 0:
            result = True

        return result


    def createDatafile(self, id=''):
        """Add new VHECR datafile information to the database.

        **Properties**

        ========== ================================
        Parameter  Description
        ========== ================================
        *id*       id of the created datafile.
        ========== ================================

        Returns a new Datafile object with the specified *id*.  If an
        entry with this *id* already exists in the database, the
        already existing VHECR datafile information is returned.
        """
        datafile = Datafile(self._db, id=id)

        if not self.hasDatafile(id):
            datafile.write()
        else:
            raise ValueError("Datafile with id='%s' already exists." %(id))

        return datafile


    def readDatafileIDs(self, status=''):
        """Read datafile IDs from database that satisfy the provided
        *status*.

        **Properties**

        ========== ==================================
        Parameter  Description
        ========== ==================================
        *status*   status of the requested datafile.
        ========== ==================================

        If *status* is empty or omitted, all datafileIDs are selected.
        """
        datafileIDs = []

        if (status != ''):
            sql = "SELECT datafileID FROM main.datafiles WHERE status='{0}'".format(status)
        else:
            sql = "SELECT datafileID FROM main.datafiles"
        records = self._db.select(sql)

        for record in records:
            datafileIDs.append(str(record[0]))

        return datafileIDs


    def readDatafile(self, id=''):
        """Read the VHECR datafile information that satisfies the
        provided *id*.

        **Properties**

        ========== ================================
        Parameter  Description
        ========== ================================
        *id*       id of the requested datafile.
        ========== ================================

        Returns a Datafile object for the provided *id*.
        """
        datafile = Datafile(db=self._db, id=id)

        if self.hasDatafile(id=id):
            datafile.read()
        else:
            raise ValueError("Datafile with id='%s' does not exist." %(id))

        return datafile


    def deleteDatafile(self, id=''):
        """Delete the VHECR datafile information for the specified
        *id* from the database.

        **Properties**

        ========== ================================
        Parameter  Description
        ========== ================================
        *id*       id of the datafile to delete.
        ========== ================================
        """
        if self.hasDatafile(id):
            sql = "DELETE FROM main.datafiles WHERE datafileID='{0}'".format(id)
            self._db.execute(sql)
        else:
            raise ValueError("Datafile with id='%s' does not exist." %(id))



    def hasStation(self, id=0):
        """Check if the VHECR station information with the
        corresponding *id* is in the database.

        **Properties**

        ========== ================================
        Parameter  Description
        ========== ================================
        *id*       id of the station.
        ========== ================================

        Returns *True* if VHECR station information with *id* exists,
        *False* otherwise.
        """
        result = False
        records = []

        if (id != 0):
            sql = "SELECT stationID FROM main.stations WHERE stationID={0}".format(id)
            records = self._db.select(sql)

        if len(records) > 0:
            result = True

        return result


    def createStation(self, id=0):
        """Add new VHECR information for a station.

        **Properties**

        ========== ================================
        Parameter  Description
        ========== ================================
        *id*       id of the station.
        ========== ================================

        Returns a new Station object with the specified *id*. If a
        station entry with this *id* already exists in the database,
        the already existing station information is returned.
        """
        station = Station(db=self._db, id=id)

        if not self.hasStation(id=id):
            station.write()
        else:
            raise ValueError("Station with id=%d does not exist." %(id))

        return station


    def readStationIDs(self, status=''):
        """Read stationIDs from the database that satisfy the provided
        *status*.

        **Properties**

        ========== ==================================
        Parameter  Description
        ========== ==================================
        *status*   status of the requested station.
        ========== ==================================

        If *status* is empty or omitted, all stationIDs are selected.
        """
        stationIDs = []

        if (status != ''):
            sql = "SELECT stationID FROM main.stations WHERE status='{0}'".format(status)
        else:
            sql = "SELECT stationID FROM main.stations"
        records = self._db.select(sql)

        for record in records:
            stationIDs.append(int(record[0]))

        return stationIDs


    def readStation(self, id=0):
        """Read the VHECR information for a single station that
        satisfies the provided *id*.

        **Properties**

        ========== ================================
        Parameter  Description
        ========== ================================
        *id*       id of the station.
        ========== ================================

        Return a Station object for the provided *id*.
        """
        station = Station(db=self._db, id=id)

        if self.hasStation(id=id):
            station.read()
        else:
            raise ValueError("Station with id=%d does not exist." %(id))

        return station


    def deleteStation(self, id=0):
        """Delete the VHECR station information for the specified
        *id* from the database.

        **Properties**

        ========== ================================
        Parameter  Description
        ========== ================================
        *id*       id of the station.
        ========== ================================
        """
        if self.hasStation(id=id):
            sql = "DELETE FROM main.stations WHERE stationID={0}".format(id)
            self._db.execute(sql)
        else:
            raise ValueError("Station with id=%d does not exist." %(id))



    def hasPolarisation(self, id=0):
        """Check if the VHECR polarisation information with the
        corresponding *id* is in the database.

        **Properties**

        ========== ================================
        Parameter  Description
        ========== ================================
        *id*       id of the polarisation.
        ========== ================================

        Returns *True* if VHECR polarisation information with *id*
        exists, *False* otherwise.
        """
        result = False
        records = []

        if (id != 0):
            sql = "SELECT polarisationID FROM main.polarisations WHERE polarisationID={0}".format(id)
            records = self._db.select(sql)
        else:
            raise ValueError("Invalid ID")

        if len(records) > 0:
            result = True

        return result


    def createPolarisation(self, id=0):
        """Add new VHECR polarisation information to the database.

        **Properties**

        ========== ================================
        Parameter  Description
        ========== ================================
        *id*       id of the polarisation.
        ========== ================================

        Returns a new VHECR Polarisation information object with the
        specified *id*.  If an entry with this *id* already exists in
        the database, the already existing VHECR polarisation
        information is returned.
        """
        polarisation = Polarisation(db=self._db, id=id)

        if not self.hasPolarisation(id=id):
            polarisation.write()
        else:
            raise ValueError("Polarisation with id=%d does not exist." %(id))

        return polarisation


    def readPolarisationIDs(self, status=''):
        """Read polarisationIDs from the database that satisfy the
        provided *status*.

        **Properties**

        ========== =======================================
        Parameter  Description
        ========== =======================================
        *status*   status of the requested polarisation.
        ========== =======================================

        If *status* is empty or omitted, all polarisationIDs are
        selected.
        """
        polarisationIDs = []

        if (status != ''):
            sql = "SELECT polarisationID FROM main.polarisations WHERE status={0}".format(status)
        else:
            sql = "SELECT polarisationID FROM main.polarisations"
        records = self._db.select(sql)

        for record in records:
            polarisationIDs.append(int(record[0]))

        return polarisationIDs


    def readPolarisation(self, id=0):
        """Read VHECR information for this polarisation that satisfies
        the provided *id*.

        **Properties**

        ========== ================================
        Parameter  Description
        ========== ================================
        *id*       id of the polarisation.
        ========== ================================

        Return a Polarisation object for the provided *id*.
        """
        polarisation = Polarisation(db=self._db, id=id)

        if self.hasPolarisation(id=id):
            polarisation.read()
        else:
            raise ValueError("Polarisation with id=%d does not exist." %(id))

        return polarisation


    def deletePolarisation(self, id=0):
        """Delete the VHECR polarisation information for the specified
        *id* from the database.

        **Properties**

        ========== ================================
        Parameter  Description
        ========== ================================
        *id*       id of the polarisation.
        ========== ================================
        """
        if self.hasPolarisation(id=id):
            sql = "DELETE FROM main.polarisations WHERE polarisationID={0}".format(id)
            self._db.execute(sql)
        else:
            raise ValueError("Polarisation with id=%d does not exist." %(id))



    def hasFilter(self, name=''):
        """Check if there is a filter with the corresponding *name* in
        the database.

        **Properties**

        ========== ================================
        Parameter  Description
        ========== ================================
        *name*     name of the datafile to check.
        ========== ================================

        Returns *True* if a filter with *name* exists, *False*
        otherwise.
        """
        result = False
        records = []

        if (name != ''):
            sql = "SELECT datafileID FROM main.filters WHERE name='{0}'".format(name)
            records = self_db.select(sql)

        if len(records) > 0:
            result = True

        return result


    def readFilter(self, name=''):
        """Read filter from database that satisfy the provided name.

        **Properties**

        ========== ==============================
        Parameter  Description
        ========== ==============================
        *name*     name of the filter.
        ========== ==============================
        """
        f = Filter(self._db, name)
        f.read()

        return f



    def validate(self):
        """Validation of the database.

        Returns *True* if no inconsistencies are found, *False* otherwise.
        """
        result = True

        # Run over all datafiles:
        datafileIDs = self.readDatafileIDs()
        for datafileID in datafileIDs:
            datafile = self.readDatafile(datafileID)

            # does the datafile have a status
            if (datafile.status == ''):
                print "{0}: Empty status".format(datafileID)
                datafile.setStatus("UNPROCESSED")
                result = False

            # does the datafile have a timestamp
            if (datafile.timestamp == 0):
                print "{0}: Empty timestamp".format(datafileID)
                result = False

            # does the datafile have a resultspath
            if (datafile.resultspath == ''):
                print "{0}: Empty resultspath".format(datafileID)
                if (datafile.status == "PROCESSED"):
                    result = False

            # check if corresponding events are available

            # Check if datafiles are linked to exactly one event
            sql = "SELECT eventID FROM main.event_datafile WHERE datafileID='{0}'".format(datafileID)
            n_records = len(self._db.select(sql))
            if n_records != 1:
                print "Warning: {0}: Instead of 1 this datafile has {1} associated events".format(datafileID, n_records)
                result = False

        # Run over all events:
        eventIDs = self.readEventIDs()
        for eventID in eventIDs:
            event = self.readEvent(eventID)

            # - does the event have a valid status
            if (event.status == ''):
                print "{0}: Empty status".format(eventID)
                result = False
            pass

        # Check for duplicate filters

        return result


    def summary(self):
        """Summary of the PipelineDatabase object."""
        linewidth = 80

        print "="*linewidth
        print "  Summary of the PipelineDatabase object."
        print "="*linewidth

        print "  %-40s : '%s'" %("Filename", self._filename)
        print "  %-40s : '%s'" %("Base path", self.basePath)
        print "  %-40s : '%s'" %("Data path", self.dataPath)
        print "  %-40s : '%s'" %("Results path", self.resultsPath)

        print "-"*linewidth

        # Events
        print "  %-40s : %d" %("Nr. of events", len(self.readEventIDs()))

        print "-"*linewidth

        # Datafiles
        print "  %-40s : %d" %("Nr. of datafiles", len(self.readDatafileIDs()))

        # Filters


        print "="*linewidth



class Event:
    """VHECR event information."""

    def __init__(self, db=None, id=0):
        """Initialisation of Event object.

        **Properties**

        ========== ===========================================
        Parameter  Description
        ========== ===========================================
        *db*       database to which to link this event to.
        *id*       id of the new event.
        ========== ===========================================
        """
        # Reference to database object containing event information.
        self._db = db

        self._id = id
        self._status = "UNDEFINED"
        self._timestamp = "UNDEFINED"

        self._archivename = "UNDEFINED"
        self._archivepath = "UNDEFINED"
        self._datafilepath = "UNDEFINED"
        self._resultspath = "UNDEFINED"

        self._datafiles = []
        self._properties = {}


    def __hasRecord(self):
        """Check if the database already has a record for this object
        with the corresponding *id*.
        """
        result = False

        if self._db:
            sql = "SELECT * FROM main.events WHERE eventID={0}".format(self._id)
            records = self._db.select(sql)
            if len(records) > 0:
                result = True

        return result


    def __getPropertyID(self, key=''):
        """Return the property *id* for a specific key.

        **Properties**

        ========== ===============================
        Parameter  Description
        ========== ===============================
        *key*      key name of the property.
        ========== ===============================
        """
        return "{0}|{1}".format(self._id, key)



    def read(self):
        """Read event information from the database."""
        if self._db:
            # read from event table
            sql = "SELECT eventID, status, timestamp, resultspath FROM main.events WHERE eventID={0}".format(self._id)
            records = self._db.select(sql)
            if len(records) == 1:
                self._id, self._status, self._timestamp, self._resultspath = records[0]
            elif len(records) == 0:
                raise ValueError("No records found for eventID={0}".format(self._id))
            else:
                raise ValueError("Multiple records found for eventID={0}".format(self._id))

            # read datafiles from list of datafileIDs
            sql = "SELECT datafileID FROM main.event_datafile WHERE eventID={0}".format(self._id)
            datafileIDs = []
            records = self._db.select(sql)
            for record in records:
                datafileID = str(record[0])
                datafile = Datafile(self._db, id=datafileID)
                self._datafiles.append(datafile.read())

            # read from eventproperties table
            sql = "SELECT key, value, type FROM main.eventproperties WHERE eventID={0}".format(self._id)
            records = self._db.select(sql)
            for record in records:
                self._properties[record[0]] = record[1]

        else:
            raise ValueError("Unable to read from database: no database was set.")


    def write(self, recursive=True):
        """Write event information to the database.

        **Properties**

        ============  ================================================================================
        Parameter     Description
        ============  ================================================================================
        *recursive*   Logical value if all underlying objects should be written to the database.
        ============  ================================================================================
        """
        if self._db:
            # write to event table
            if not self.__hasRecord():
                sql = "INSERT INTO main.events (eventID, status, timestamp, resultspath) VALUES ({0}, '{1}', '{2}', '{3}')".format(self._id, self._status, self._timestamp, self._resultspath)
            else:
                sql = "UPDATE main.events SET status='{1}', timestamp='{2}', resultspath='{3}' WHERE eventID={0}".format(self._id, self._status, self._timestamp, self._resultspath)
            self._db.insert(sql)

            # write to event_datafile table
            for datafile in self._datafiles:
                sql = "SELECT eventID FROM main.event_datafile WHERE datafileID='{0}'".format(datafile.getID())
                if (len(self._db.select(sql)) == 0):
                    sql = "INSERT INTO main.event_datafile (eventID, datafileID) VALUES ({0}, '{1}')".format(self._id, datafile.getID())
                    self._db.execute(sql)

            # write datafile information
            if recursive:
                for datafile in self._datafiles:
                    datafile.write(recursive=recursive)

            # write to eventproperties table
            for key in self._properties.keys():
                self.setProperty(key, self._properties[key])
        else:
            raise ValueError("Unable to write to database: no database was set.")



    def getID(self):
        """get the ID of this event."""
        return self._id


    def getStatus(self):
        """Get the status of the event."""
        return self._status


    def setStatus(self, status=''):
        """Set the status of the event and update the database.

        **Properties**

        ========== ================================
        Parameter  Description
        ========== ================================
        *status*   new value of the status
        ========== ================================
        """
        # Set the status in the object.
        self._status = status

        # Update the database.
        if self._db:
            if not self.__hasRecord():
                self.write()
            else:
                sql = "UPDATE main.events SET status='{0}' WHERE eventID={1}".format(self._status, self._id)
                self._db.execute(sql)
        else:
            raise ValueError("Unable to write to database: no database was set.")


    def getTimestamp(self):
        """Get the timestamp of the event."""
        return self._timestamp


    def setTimestamp(self, timestamp=''):
        """Set the timestamp of the event and update the database.

        **Properties**

        ============= ================================
        Parameter     Description
        ============= ================================
        *timestamp*   new value of the timestamp
        ============= ================================
        """
        # Set the timestamp in the object.
        self._timestamp = timestamp

        # Update the database.
        if self._db:
            if not self.__hasRecord():
                self.write()
            else:
                sql = "UPDATE main.events SET timestamp='{0}' WHERE eventID={1}".format(self._timestamp, self._id)
                self._db.execute(sql)
        else:
            raise ValueError("Unable to write to database: no database was set.")


    def addDatafile(datafile=None):
        """Add a VHECR datafile information object to the list of
        datafile information objects for this event object.

        **Properties**

        ===========  ==================================================
        Parameter    Description
        ===========  ==================================================
        *datafile*   VHECR datafile information object.
        ===========  ==================================================
        """
        if datafile:
            isUnique = True
            datafileID = datafile.getID()

            # Check if datafile is already in the list
            for d in self.datafiles:
                if d.getID() == datafileID:
                    isUnique = False
                    break
        else:
            isUnique = False

        if isUnique:
            # Update list
            self.datafiles.append(datafile)

            # Update database
            if self._db:
                sql = "SELECT eventID FROM main.event_datafile WHERE datafileID='{0}'".format(datafile.getID())
                if (len(self._db.select(sql)) == 0):
                    sql = "INSERT INTO main.event_datafile (eventID, datafileID) VALUES ({0}, '{1}')".format(self._id, datafile.getID())
                    self._db.insert(sql)
            else:
                raise ValueError("Unable to write to database: no database was set.")


    def getDatafiles(self):
        """Return a list of datafile objects."""
        return self._datafiles


    def getResultsPath(self):
        """Return the path where the results files are located."""
        return self._resultspath


    def getDatafilePath(self):
        """Return the path where the datafiles are located."""
        return self._datafilepath



    def getProperty(self, key=''):
        """Get a property of the event matching the *key*.

        **Properties**

        ========== ================================
        Parameter  Description
        ========== ================================
        *key*      key name of the property.
        ========== ================================
        """
        try:
            return self._properties[key]
        except:
            raise ValueError("Unknown key value")


    def setProperty(self, key='', value=''):
        """Set a property of the event and update the database.

        **Properties**

        ========= =================================
        Parameter Description
        ========= =================================
        *key*     key name of the property.
        *value*   value of the property.
        ========= =================================
        """
        if self.hasProperty(key):
            sql = "UPDATE main.eventproperties SET value='{1}' WHERE propertyID='{0}'".format(self.__getPropertyID(key), str(value))
        else:
            sql = "INSERT INTO main.eventproperties (propertyID, eventID, key, value) VALUES ('{0}', {1}, '{2}', '{3}')".format(self.__getPropertyID(key), self._id, key, str(value))
        self._db.execute(sql)

        self._properties[key] = value


    def deleteProperty(self, key=''):
        """Delete an event property.

        **Properties**

        ========= ===============================
        Parameter Description
        ========= ===============================
        *key*     key name of the property.
        ========= ===============================
        """
        if self._db:
            sql = "DELETE FROM main.eventproperties WHERE propertyID='{0}'".format(self.__getPropertyID(key))
            self._db.execute(sql)

        del self._properties[key]


    def hasProperty(self, key=''):
        """Check if the property with the provided *key* is available.

        **Properties**


        ========= ===============================
        Parameter Description
        ========= ===============================
        *key*     key name of the property.
        ========= ===============================
        """
        result = False
        records = []

        if self._db:
            sql = "SELECT * FROM main.eventproperties WHERE propertyID='{0}'".format(self.__getPropertyID(key))
            records = self._db.select(sql)

        if len(records) > 0:
            result = True

        return result



    def summary(self):
        """Summary of the Event object."""
        linewidth = 80

        print "="*linewidth
        print "  Summary of the Event object."
        print "="*linewidth

        # Event info
        print "  %-40s : %d" %("ID", self._id)
        print "  %-40s : %s" %("Status", self._status)
        print "  %-40s : %s" %("Timestamp", self._timestamp)
        print "  %-40s : %s" %("Results path", self._resultspath)

        # Datafiles
        print "  %-40s : %d" %("Nr. of datafiles", len(self._datafiles))
        for datafile in self._datafiles:
            print "    %-38s", datafile.getFilename()

        # Properties
        if len(self._properties) > 0:
            print "  Properties:"
            for key in self._properties.keys():
                print "    %-38s : %s" %(key, self._properties[key])

        print "="*linewidth



class Datafile:
    """VHECR datafile information."""

    def __init__(self, db=None, id=''):
        """Initialisation of Datafile object.

        **Properties**

        ========= ===========================================
        Parameter Description
        ========= ===========================================
        *db*      database to which to link this datafile to.
        *id*      id of the new datafile.
        ========= ===========================================
        """
        # Reference to database object containing datafile information.
        self._db = db

        self._id = id
        self._filename = ""
        self._status = "UNDEFINED"
        self._timestamp = "UNDEFINED"
        self._resultspath = "UNDEFINED"
        self._stations = []


    def __hasRecord(self):
        """Check if the database already has a record for this object
        with the corresponding *id*.
        """
        result = False

        if self._db:
            sql = "SELECT * FROM main.datafiles WHERE datafileID='{0}'".format(self._id)
            records = self._db.select(sql)
            if len(records) > 0:
                result = True

        return result



    def read(self):
        """Read datafile information from the database."""
        if self._db:
            # read from datafiles table
            sql = "SELECT datafileID, status, timestamp, resultspath FROM main.datafiles WHERE datafileID='{0}'".format(self._id)
            records = self._db.select(sql)
            if len(records) == 1:
                self._id, self._status, self._timestamp, self._resultspath = records[0]
            elif len(records) == 0:
                raise ValueError("No records found for datafileID={0}".format(self._id))
            else:
                raise ValueError("Multiple records found for datafileID={0}".format(self._id))

            # read from datafile_station table
            sql = "SELECT stationID FROM main.datafile_station WHERE datafileID={0}".format(self._id)
            stationIDs = []
            records = self._db.select(sql)
            for record in records:
                stationID = record[0]
                station = Station(self._db, id=stationID)
                self._stations.append(station.read())

        else:
            raise ValueError("Unable to read from database: no database was set.")


    def write(self, recursive=True):
        """Write datafile information to the database.

        **Properties**

        ============  ================================================================================
        Parameter     Description
        ============  ================================================================================
        *recursive*   logical value if all underlying objects should be written to the database.
        ============  ================================================================================
        """
        if self._db:
            # write to datafile table
            if not self.__hasRecord():
                sql = "INSERT INTO main.datafiles (datafileID, status, timestamp, resultspath) VALUES ('{0}', '{1}', '{2}', '{3}')".format(self._id, self._status, self._timestamp, self._resultspath)
            else:
                sql = "UPDATE main.datafiles SET status='{1}', timestamp='{2}', resultspath='{3}' WHERE datafileID='{0}'".format(self._id, self._status, self._timestamp, self._resultspath)
            self._db.insert(sql)

            # write to datafile_station table
            for station in self._stations:
                sql = "SELECT stationID FROM main.datafile_station WHERE stationID='{0}'".format(station.getID())
                if (len(self._db.select(sql)) == 0):
                    sql = "INSERT INTO main.datafile_station (datafileID, stationID) VALUES ({0}, '{1}')".format(self._id, station.getID())
                    self._db.execute(sql)

            # write to stations table
            if recursive:
                for station in self._stations:
                    station.write()

        else:
            raise ValueError("Unable to write to database: no database was set.")



    def getID(self):
        """get the ID of this Datafile object."""
        return self._id


    def getFilename(self):
        """Get the filename of the datafile."""
        return self._filename


    def getStatus(self):
        """Get the status of the datafile."""
        return self._status


    def setStatus(self, status=''):
        """Set the status of the datafile and update the database.

        **Properties**

        ========== ===========================================
        Parameter  Description
        ========== ===========================================
        *status*   new value of the status
        ========== ===========================================
        """
        self._status = status

        if self._db:
            if not self.__hasRecord():
                self.write()
            else:
                sql = "UPDATE main.datafiles SET status='{0}' WHERE datafileID='{1}'".format(self._status, self._id)
                self._db.execute(sql)
        else:
            raise ValueError("Unable to write to database: no database was set.")


    def addStation(self, station):
        """Add a VHECR station information object to the list of
        station information objects in this datafile object.

        **Properties**

        =========  ==================================
        Parameter  Description
        =========  ==================================
        *station*  VHECR station information object.
        =========  ==================================
        """
        if station:
            isUnique = True
            stationID = station.getID()

            # Check if station is already in the list
            for s in self.stations:
                if s.getID() == stationID:
                    isUnique = False
                    break
        else:
            isUnique = False
            (stationID)
        if isUnique:
            # Update list
            self._stations.append(station)
            # Update database
            if self._db:
                sql = "SELECT datafileID FROM main.datafile_station WHERE stationID='{0}'".format(station.getID())
                if len(self._db.select(sql)) == 0:
                    sql = "INSERT INTO main.datafile_station (datafileID, stationID) VALUES ('{0}', {1})".format(self._id, station.getID())
                    self._db.insert(sql)
            else:
                raise ValueError("Unable to write to database: no database was set.")


    def getStations(self):
        """Get a list of stations in this datafile."""
        return self._stations



    def summary(self):
        """Summary of the Datafile object."""
        linewidth = 80

        print "="*linewidth
        print "  Summary of the Datafile object."
        print "="*linewidth

        # Datafile info
        print "  %-40s : %s" %("ID", self._id)
        print "  %-40s : %s" %("Filename", self._filename)
        print "  %-40s : %s" %("Status", self._status)
        print "  %-40s : %s" %("Timestamp", self._timestamp)
        print "  %-40s : %s" %("Results path", self._resultspath)

        print "  %-40s : %d" %("Number of stations", len(self._stations))

        print "="*linewidth



class Station:
    """VHECR information for a single LOFAR station."""

    def __init__(self, db=None, id=0):
        """Initialisation of Station object.

        **Properties**

        =========  ===================================================
        Parameter  Description
        =========  ===================================================
        *db*       database to which to link this station object to.
        *id*       id of the new station object.
        =========  ===================================================
        """
        self._db = db
        self._id = id
        self._name = "UNDEFINED"
        self._status = "UNDEFINED"
        self._polarisations = []


    def __hasRecord(self):
        """Check if the database already has a record for this object
        with the corresponding *id*.
        """
        result = False

        if self._db:
            sql = "SELECT * FROM main.stations WHERE stationID='{0}'".format(self._id)
            records = self._db.select(sql)
            if len(records) > 0:
                result = True

        return result



    def read(self):
        """Read station information from the database."""
        if self._db:
            # read from stations table
            sql = "SELECT stationID, name, status FROM main.stations WHERE stationID={0}".format(self._id)
            records = self._db.select(sql)
            if 1 == len(records):
                self._id, self._name, self._status = records[0]
            elif 0 == len(records):
                raise ValueError("No records found for stationID={0}".format(self._id))
            else:
                raise ValueError("Multiple records found for stationID={0}".format(self._id))

            # read from station_polarisation table
            sql = "SELECT polarisationID FROM main.station_polarisation WHERE stationID={0}".format(self._id)
            polarisationIDs = []
            records = self._db.select(sql)
            for record in records:
                polarisationID = record[0]
                polarisation = Polarisation(self._db, id=stationID)
                self._polarisations.append(polarisation.read())

        else:
            raise ValueError("Unable to write to database: no database was set.")


    def write(self, recursive=True):
        """Write station information to the database.

        **Properties**

        ============  ================================================================================
        Parameter     Description
        ============  ================================================================================
        *recursive*   Logical value if all underlying objects should be written to the database.
        ============  ================================================================================
        """
        if self._db:
            # write to stations table
            if not self.__hasRecord():
                sql = "INSERT INTO main.stations (stationID, name, status) VALUES ({0}, '{1}', '{2}')".format(self._id, self._name, self._status)
            else:
                sql = "UPDATE main.stations SET name='{1}', status='{2}' WHERE stationID={0}".format(self._id, self._name, self._status)
            self._db.insert(sql)

            # write to station_polarisation table
            for polarisation in self._polarisations:
                sql = "SELECT polarisationID FROM main.station_polarisation WHERE polarisationID={0}".format(polarisation.getID())
                if (len(self._db.select(sql)) == 0):
                    sql = "INSERT INTO main.station_polarisation (stationID, polarisationID) VALUES ({0}, {1})".format(self._id, polarisation.getID())
                    self._db.execute(sql)

            # write to polarisation table
            if recursive:
                for polarisation in self._polarisations:
                    polarisation.write()

        else:
            raise ValueError("Unable to write to database: no database was set.")



    def getID(self):
        """Get the ID of the station."""
        return self._id


    def getName(self):
        """Get the name of the station."""
        return self._name


    def getStatus(self):
        """Get the status of the station."""
        return self._status


    def setStatus(self, status=''):
        """Set the status of the VHECR for this station and update the
        database.

        **Properties**

        ========== ===========================================
        Parameter  Description
        ========== ===========================================
        *status*   new value of the status
        ========== ===========================================
        """
        self._status = status

        if self._db:
            if not self.__hasRecord():
                self.write()
            else:
                sql = "UPDATE main.stations SET status='{0}' WHERE stationID={1}".format(self._status, self._id)
                self._db.execute(sql)
        else:
            raise ValueError("Unable to write to database: no database was set.")


    def addPolarisation(self, polarisation):
        """Add a VHECR polarisation information object to the list of
        polarisation information objects for this station object.

        **Properties**

        ==============  ========================================
        Parameter       Description
        ==============  ========================================
        *polarisation*  VHECR polaristaion information object.
        ==============  ========================================
        """
        if polarisation:
            isUnique = True
            polarisationID = polarisation.getID()

            # check if polarisation is already existing in list
            for p in self._polarisations:
                if p.getID() == polarisationID:
                    isUnique = False
                    break
        else:
            isUnique = True

        if isUnique:
            # Update list
            self._polarisations.append(polarisation)

            # Update database
            if self._db:
                sql = "SELECT stationID FROM main.station_polarisation WHERE polarisationID={0}".format(polarisation.getID())
                if (len(self._db.select(sql)) == 0):
                    sql = "INSERT INTO main.station_polarisation (stationID, polarisationID) VALUES ({0}, {1})".format(self._id, polarisation.getID())
                    self._db.insert(sql)
            else:
                raise ValueError("Unable to write to database: no database was set.")


    def getPolarisations(self):
        """Get a list of polarisations for this station object."""
        return self._polarisations



    def summary(self):
        """Summary of the Station object."""
        linewidth = 80

        print "="*linewidth
        print "  Summary of the Station object."
        print "="*linewidth

        # Datafile info
        print "  %-40s : %s" %("ID", self._id)
        print "  %-40s : %s" %("Name", self._name)
        print "  %-40s : %s" %("Status", self._status)

        print "  %-40s : %d" %("Number of polarisations", len(self._polarisations))

        print "="*linewidth



class Polarisation:
    """VHECR information for a single polarisation in a station."""

    def __init__(self, db=None, id=0):
        """Initialisation of the Polarisation object.

        **Properties**

        =========  ===================================================
        Parameter  Description
        =========  ===================================================
        *db*       database to which to link this polarisation object to.
        *id*       id of the new polarisation object.
        =========  ===================================================
        """
        self._db = db

        self._id = id
        self._status = "UNDEFINED"
        self._pol_type = "UNDEFINED"
        self._pol_direction = "UNDEFINED"
        self._resultspath = "UNDEFINED"

        self._properties = {}


    def __hasRecord(self):
        """Check if the database already has a record for this object
        with the corresponding *id*.
        """
        result = False

        if self._db:
            sql = "SELECT * FROM main.polarisations WHERE polarisationID={0}".format(self._id)
            records = self._db.select(sql)
            if len(records) > 0:
                result = True

        return result


    def __getPropertyID(self, key=''):
        """Return the property *id* for a specific key.

        **Properties**

        ========== ===============================
        Parameter  Description
        ========== ===============================
        *key*      key name of the property.
        ========== ===============================
        """
        return self._id + "|" + key



    def read(self):
        """Read station polarisation information from the database."""
        if self._db:
            # read to polarisation table
            sql = "SELECT polarisationID, status, pol_type, pol_dir, resultspath FROM main.polarisations WHERE polarisationID={0}".format(self._id)
            records = self._db.select(sql)
            if 1 == len(records):
                self._id, self._status, self._pol_type, self._pol_direction = records[0]
            elif 0 == len(records):
                raise ValueError("No records found for polarisationID={0}".format(self._id))
            else:
                raise ValueError("Multiple records found for polarisationID={0}".format(self._id))

            # read polarisationproperties table
            sql = "SELECT key, value, type FROM main.polarisationproperties WHERE polarisationID={0}".format(self._id)
            records = self._db.select(sql)
            for record in records:
                self._properties[record[0]] = record[1]

        else:
            raise ValueError("Unable to read from database: no database was set.")


    def write(self):
        """Write station polarisation information to the database."""
        if self._db:
            # write to polarisation table
            if not self.__hasRecord():
                sql = "INSERT INTO main.polarisations (polarisationID, status, pol_type, pol_dir) VALUES ({0}, '{1}', {2}, {3})".format(self._id, self._status, self._pol_type, self._pol_dir)
            else:
                sql = "UPDATE main.polarisations SET status='{1}', pol_type={2}, pol_dir={3} WHERE polarisationID={0}".format(self._id, self._status, self._pol_type, self._pol_dir)
            self._db.insert(sql)

            # write polarisationproperties table
            for key in self._properties.keys():
                self.setProperty(key, self._properties[key])

        else:
            raise ValueError("Unable to read from database: no database was set.")



    def getID(self):
        """Get the ID of this polarisation."""
        return self._id


    def getStatus(self):
        """Get the status of the polarisation object."""
        return self._status


    def setStatus(self, status=''):
        """Set the status of the polarisation and update the database.

        **Properties**

        ========== ===========================================
        Parameter  Description
        ========== ===========================================
        *status*   new value of the status
        ========== ===========================================
        """
        self._status = status

        # write status to database
        if self._db:
            if not self.__hasRecord():
                self.write()
            else:
                sql = "UPDATE main.polarisations SET status='{0}' WHERE polarisationID={1}".format(self._status, self._id)
                self._db.execute(sql)
        else:
            raise ValueError("Unable to read from database: no database was set.")


    def getPolType(self):
        """Get the type of polarisation."""
        return self._pol_type


    def getPolDirection(self):
        """Get the direction of the polarisation."""
        return self._pol_direction


    def getResultsPath(self):
        """Get the path where the results are stored."""
        return self._resultspath



    def hasProperty(self, key=''):
        """Check if the property with the provided *key* is available.

        **Properties**

        ========== ===============================
        Parameter  Description
        ========== ===============================
        *key*      key name of the property.
        ========== ===============================

        Returns *True* if a property with *key* exists, *False*
        otherwise.
        """
        result = False
        records = []

        if self._db:
            sql = "SELECT * FROM main.polarisationproperties WHERE propertyID='{0}'".format(self.__getPropertyID(key))
            records = self._db.select(sql)

        if len(records) > 0:
            result = True

        return result


    def setProperty(self, key='', value=''):
        """Set a property and update the database.

        **Properties**

        =========  =================================
        Parameter  Description
        =========  =================================
        *key*      key name of the property.
        *value*    value of the property.
        =========  =================================
        """
        if self.hasProperty(key):
            sql = "UPDATE main.polarisationproperties SET value='{1}' WHERE propertyID='{0}'".format(self.__getPropertyID(key), str(value))
        else:
            sql = "INSERT INTO main.polarisationproperties (propertyID, polarisationID, key, value) VALUES ('{0}', '{1}', '{2}', '{3}')".format(self.__getPropertyID(key), self._id, key, str(value))
        self._db.execute(sql)

        self._properties[key] = value


    def deleteProperty(self, key=''):
        """Delete a datafile property.

        **Properties**

        ========== ===============================
        Parameter  Description
        ========== ===============================
        *key*      key name of the property.
        ========== ===============================
        """
        if self._db:
            sql = "DELETE FROM main.polarisationproperties WHERE propertyID='{0}'".format(self.__getPropertyID(key))
            self._db.execute(sql)

        del self._properties[key]



    def summary(self):
        """Summary of the Polarisation object."""
        linewidth = 80

        print "="*linewidth
        print "  Summary of the Polarisation object."
        print "="*linewidth

        # Datafile info
        print "  %-40s : %s" %("ID", self._id)
        print "  %-40s : %s" %("Status", self._status)
        print "  %-40s : %s" %("Polarisation type", self._pol_type)
        print "  %-40s : %s" %("Polarisation direction", self._pol_direction)
        print "  %-40s : %s" %("Results path", self._resultspath)

        # Properties
        if len(self._properties) > 0:
            print "  Properties:"
            for key in self._properties.keys():
                print "    %-38s : %s" %(key, self._properties[key])

        print "="*linewidth



class Filter:
    """Container class for filters.

    These filters are stored in the pipeline database.
    """

    def __init__(self, db=None, name='DEFAULT'):
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


    def read(self):
        """Read filter from the database."""
        self.filters = []

        if self._db:
            sql = "SELECT filter FROM main.filters WHERE name='{0}'".format(self.name)
            for record in self._db.select(sql):
                self.filters.append(str(record[0]))
        else:
            raise ValueError("No database set.")

        return self.filters


    def write(self):
        """Write filters to the database."""
        if self._db:
            for filtervalue in self.filters:
                # Only add filter if it not already exists
                sql = "SELECT * FROM main.filters WHERE name='{0}' AND filter='{1}'".format(self.name, filtervalue)
                n = len(self._db.select(sql))
                if n == 0:
                    sql = "INSERT INTO main.filters (name, filter) VALUES ('{0}', '{1}');".format(self.name, filtervalue)
                    self._db.insert(sql)
        else:
            raise ValueError("No database set.")


    def add(self, filtervalue=''):
        """Add a filter to the database.

        **Properties**

        =============== ========================
        Parameter       Description
        =============== ========================
        *filtervalue*   content of the filter.
        =============== ========================
        """
        if self._db:
            # Only add filter if it not already exists
            sql = "SELECT * FROM main.filters WHERE name='{0}' AND filter='{1}'".format(self.name, filtervalue)
            n = len(self._db.select(sql))
            if n == 0:
                sql = "INSERT INTO main.filters (name, filter) VALUES ('{0}', '{1}');".format(self.name, filtervalue)
                self._db.execute(sql)
        else:
            raise ValueError("No database set.")

        if not filtervalue in self.filters:
            self.filters.append(filtervalue)


    def delete(self, filtervalue=''):
        """Delete a filtervalue from the database.

        **Properties**

        =============== ========================
        Parameter       Description
        =============== ========================
        *filtervalue*   content of the filter.
        =============== ========================
        """
        if self._db:
            sql = "DELETE FROM main.filters WHERE name='{0}' AND filter='{1}'".format(self.name, filtervalue)
            self._db.execute(sql)
        else:
            raise ValueError("No database set.")

        self.filters.remove(filtervalue)


    def check(self, string=''):
        """Check if at least one of the filters corresponds to a
        substring of *string*.

        **Properties**

        ========= ====================================================
        Parameter Description
        ========= ====================================================
        *string*  the string that is tested against the filters.
        ========= ====================================================

        Returns ``True`` if at least one of the filters is equal to a
        substring of *string*, ``False`` otherwise.
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

from pycrtools.io import database as db
import os

## =============================================================================
##
##  Classes
##
## =============================================================================

class PipelineDatabase:
    """Functionality to let the VHECR pipeline communicate with an SQL database."""

    def __init__(self, filename=":memory:", inputpath="", outputpath=""):
        """Initialisation of the PipelineDatabase object.

        **Parameters**

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
        self.basePath = os.path.dirname(self._filename)

        # Location of the data path
        if (len(inputpath) > 0):
            self.dataPath = inputpath
        else:
            self.dataPath = self.basePath + "/data"

        # Location of the results path
        if (len(outputpath) > 0):
            self.resultsPath = outputpath
        else:
            self.resultsPath = self.basePath + "/results"

        # Create database structure
        self.createTables()


    def createTables(self):
        """Create the pipeline database."""
        # Check Event table
        sql = "CREATE TABLE IF NOT EXISTS main.events (eventID INTEGER PRIMARY KEY NOT NULL UNIQUE, status TEXT, timestamp TEXT, resultspath TEXT);"
        self._db.execute(sql)

        # Check EventProperties table
        sql = "CREATE TABLE IF NOT EXISTS main.eventproperties (propertyID TEXT NOT NULL UNIQUE, eventID INTEGER NOT NULL, key TEXT, value TEXT, type TEXT);"
        self._db.execute(sql)

        # Check Datafile table
        sql = "CREATE TABLE IF NOT EXISTS main.datafiles (datafileID TEXT PRIMARY KEY NOT NULL UNIQUE, status TEXT, timestamp TEXT, resultspath TEXT);"
        self._db.execute(sql)

        # Check DatafileProperties table
        sql = "CREATE TABLE IF NOT EXISTS main.datafileproperties (propertyID TEXT NOT NULL UNIQUE, datafileID TEXT NOT NULL, key TEXT, value TEXT, 'type' TEXT);"
        self._db.execute(sql)

        # Check Event_Datafile table (linking Event and Datafile tables)
        sql = "CREATE TABLE IF NOT EXISTS main.event_datafile (eventID INTEGER NOT NULL, datafileID TEXT NOT NULL UNIQUE);"
        self._db.execute(sql)

        # Check Filter table
        sql = "CREATE TABLE IF NOT EXISTS main.filters (name TEXT, filter TEXT NOT NULL UNIQUE);"
        self._db.execute(sql)


    def reset(self):
        """Reset all tables and create a clean table structure"""
        self._db.execute("DROP TABLE IF EXISTS main.events")
        self._db.execute("DROP TABLE IF EXISTS main.eventproperties")
        self._db.execute("DROP TABLE IF EXISTS main.datafiles")
        self._db.execute("DROP TABLE IF EXISTS main.datafileproperties")
        self._db.execute("DROP TABLE IF EXISTS main.event_datafile")
        self._db.execute("DROP TABLE IF EXISTS main.filters")

        # Database needs to be closed to properly drop the tables before creating them again.
        self._db.close()
        self._db.open()

        self.createTables()


    def readEventIDs(self, status=''):
        """Read datafile IDs from database that satisfy the provided status.

        **Properties**

        ========== ================================
        *status*   status of the requested event.
        ========== ================================

        If the status is empty then all eventIDs are selected.
        """
        eventIDs = []

        if (status != ''):
            sql = "SELECT eventID FROM main.events WHERE status='{0}'".format(status)
        else:
            sql = "SELECT eventID FROM main.events"
        records = self._db.select(sql)

        for record in records:
            eventIDs.append(str(record[0]))

        return eventIDs


    def readEvent(self, id=0):
        """Read the event that satisfies the provided id.

        **Properties**

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


    def createEvent(self, id=0):
        """Add a new event to the database.

        **Properties**

        ========== ================================
        *id*       id of the created event.
        ========== ================================

        Returns a new Event object with the specified *id*.  If the
        event already existed in the database, the already existing
        event is returned.
        """
        event = Event(self._db, id=id)

        if not self.hasEvent(id):
            event.write()
        else:
            raise ValueError("Event with id=%d already exists." %(id))

        return event


    def deleteEvent(self, id=0):
        """Delete the event with the specified id from the database.

        **Properties**

        ========== ================================
        *id*       id of the event to delete.
        ========== ================================
        """
        if self.hasEvent(id):
            sql = "DELETE FROM main.events WHERE eventID={0}".format(id)
            self._db.execute(sql)
        else:
            raise ValueError("Event with id=%d does not exist." %(id))


    def hasEvent(self, id=0):
        """Check if there is an event with the corresponding *id* in
        the database.

        **Properties**

        ========== ================================
        *id*       id of the event to check.
        ========== ================================
        """
        result = False
        records = []

        if (id != 0):
            sql = "SELECT eventID FROM main.events WHERE eventID={0}".format(id)
            records = self._db.select(sql)

        if len(records) > 0:
            result = True

        return result


    def readDatafileIDs(self, status=''):
        """Read datafile IDs from database that satisfy the provided status.

        **Properties**

        ========== ==================================
        *status*   status of the requested datafile.
        ========== ==================================

        If the status is empty all datafileIDs are selected.
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
        """Read the datafile that satisfies the provided id.

        **Properties**

        ========== ================================
        *id*       id of the requested datafile.
        ========== ================================
        """
        datafile = Datafile(db=self._db, id=id)

        if self.hasDatafile(id):
            datafile.read()
        else:
            raise ValueError("Datafile with id='%s' does not exist." %(id))

        return datafile


    def createDatafile(self, id=''):
        """Add a new datafile to the database.

        **Properties**

        ========== ================================
        *id*       id of the created datafile.
        ========== ================================
        """
        datafile = Datafile(self._db, id=id)

        if not self.hasDatafile(id):
            datafile.write()
        else:
            raise ValueError("Datafile with id='%s' already exists." %(id))

        return datafile


    def deleteDatafile(self, id=''):
        """Delete the datafile with the specified id from the database.

        **Properties**

        ========== ================================
        *id*       id of the datafile to delete.
        ========== ================================
        """
        if self.hasDatafile(id):
            sql = "DELETE FROM main.datafiles WHERE datafileID='{0}'".format(id)
            self._db.execute(sql)
        else:
            raise ValueError("Datafile with id='%s' does not exist." %(id))


    def hasDatafile(self, id=''):
        """Check if there is a datafile with the corresponding *id* in
        the database.

        **Properties**

        ========== ================================
        *id*       id of the datafile to check.
        ========== ================================
        """
        result = False
        records = []

        if (id != ''):
            sql = "SELECT datafileID FROM main.datafiles WHERE datafileID='{0}'".format(id)
            records = self._db.select(sql)

        if len(records) > 0:
            result = True

        return result


    def readFilter(self, name=''):
        """Read filter from database that satisfy the provided name.

        **Properties**

        ======== ==============================
        *name*   name of the filter.
        ======== ==============================
        """
        f = Filter(self._db, name)
        f.read()

        return f


    def hasFilter(self, name=''):
        """Check if there is a filter with the corresponding *name* in
        the database.

        **Properties**

        ========== ================================
        *name*     name of the datafile to check.
        ========== ================================
        """
        result = False
        records = []

        if (name != ''):
            sql = "SELECT datafileID FROM main.filters WHERE name='{0}'".format(name)
            records = self_db.select(sql)

        if len(records) > 0:
            result = True

        return result


    def check(self):
        """Consistency check of the database.

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
    """Container class for VHECR event information."""

    def __init__(self, db=None, id=0):
        """Initialisation of Event object.

        **Properties**

        ====== ===========================================
        *db*   database to which to link this event to.
        *id*   id of the new event.
        ====== ===========================================
        """
        # Reference to database object containing event information.
        self._db = db

        self._id = id
        self.status = "UNDEFINED"
        self.timestamp = "UNDEFINED"
        self.resultspath = "UNDEFINED"
        self.datafileIDs = []
        self.properties = {}


    def hasRecord(self):
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


    def read(self):
        """Read event information from the database."""
        if self._db:
            # read from event table
            sql = "SELECT eventID, status, timestamp, resultspath FROM main.events WHERE eventID={0}".format(self._id)
            records = self._db.select(sql)
            if len(records) == 1:
                self._id, self.status, self.timestamp, self.resultspath = records[0]
            elif len(records) == 0:
                raise ValueError("No records found for eventID={0}".format(self._id))
            else:
                raise ValueError("Multiple records found for eventID={0}".format(self._id))

            # read from event_datafile table
            sql = "SELECT datafileID FROM main.event_datafile WHERE eventID={0}".format(self._id)
            records = self._db.select(sql)
            for record in records:
                self.datafileIDs.append(record[0]) # For the moment only append datafileIDs

            # read from eventproperties table
            sql = "SELECT key, value, type FROM main.eventproperties WHERE eventID={0}".format(self._id)
            records = self._db.select(sql)
            for record in records:
                self.properties[record[0]] = record[1]
        else:
            raise ValueError("Unable to read from database: no database was set.")


    def write(self):
        """Write event information to the database."""
        if self._db:
            # write to event table
            if not self.hasRecord():
                sql = "INSERT INTO main.events (eventID, status, timestamp, resultspath) VALUES ({0}, '{1}', '{2}', '{3}')".format(self._id, self.status, self.timestamp, self.resultspath)
            else:
                sql = "UPDATE main.events SET status='{1}', timestamp='{2}', resultspath='{3}' WHERE eventID={0}".format(self._id, self.status, self.timestamp, self.resultspath)
            self._db.insert(sql)

            # write to event_datafile table
            for datafileID in self.datafileIDs:
                sql = "SELECT eventID FROM main.event_datafile WHERE datafileID='{0}'".format(datafileID)
                if (len(self._db.select(sql)) == 0):
                    sql = "INSERT INTO main.event_datafile (eventID, datafileID) VALUES ({0}, '{1}')".format(self._id, datafileID)
                    self._db.execute(sql)

            # write to eventproperties table
            for key in self.properties.keys():
                self.setProperty(key, self.properties[key])
        else:
            raise ValueError("Unable to write to database: no database was set.")


    def setStatus(self, status=''):
        """Set the status of the event and update the database.

        **Properties**

        ========== ================================
        *status*   new value of the status
        ========== ================================
        """
        self.status = status

        if self._db:
            if not self.hasRecord():
                sql = "INSERT INTO main.events (eventID, status, timestamp, resultspath) VALUES ({0}, '{1}', '{2}', '{3}')".format(self._id, self.status, self.timestamp, self.resultspath)
            else:
                sql = "UPDATE main.events SET status='{0}' WHERE eventID={1}".format(self.status, self._id)
            self._db.execute(sql)


    def setProperty(self, key='', value=''):
        """Set a property of the event and update the database.

        **Properties**

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

        self.properties[key] = value


    def deleteProperty(self, key=''):
        """Delete an event property.

        **Properties**

        ======= ===============================
        *key*   key name of the property.
        ======= ===============================
        """
        if self._db:
            sql = "DELETE FROM main.eventproperties WHERE propertyID='{0}'".format(self.__getPropertyID(key))
            self._db.execute(sql)

        del self.properties[key]


    def hasProperty(self, key=''):
        """Check if the property with the provided *key* is available.

        **Properties**

        ======= ===============================
        *key*   key name of the property.
        ======= ===============================
        """
        result = False
        records = []

        if self._db:
            sql = "SELECT * FROM main.eventproperties WHERE propertyID='{0}'".format(self.__getPropertyID(key))
            records = self._db.select(sql)

        if len(records) > 0:
            result = True

        return result


    def __getPropertyID(self, key=''):
        """Return the property id for a specific key.

        **Properties**

        ======= ===============================
        *key*   key name of the property.
        ======= ===============================
        """
        return "{0}|{1}".format(self._id, key)


    def getID(self):
        """get the ID of this event."""
        return self._id


    def summary(self):
        """Summary of the Event object."""
        linewidth = 80

        print "="*linewidth
        print "  Summary of the Event object."
        print "="*linewidth

        # Event info
        print "  %-40s : %d" %("ID", self._id)
        print "  %-40s : %s" %("Status", self.status)
        print "  %-40s : %s" %("Timestamp", self.timestamp)
        print "  %-40s : %s" %("Results path", self.resultspath)

        # Datafiles
        print "  %-40s : %d" %("Nr. of datafiles", len(self.datafileIDs))
        for datafileID in self.datafileIDs:
            print "    %s" %(datafileID)

        # Properties
        if len(self.properties) > 0:
            print "  Properties:"
            for key in self.properties.keys():
                print "    %-38s : %s" %(key, self.properties[key])

        print "="*linewidth



class Datafile:
    """Container class for VHECR datafile information."""

    def __init__(self, db=None, id=''):
        """Initialisation of Datafile object.

        **Properties**

        ====== ===========================================
        *db*   database to which to link this datafile to.
        *id*   id of the new datafile.
        ====== ===========================================
        """
        # Reference to database object containing datafile information.
        self._db = db

        self._id = id
        self.status = "UNDEFINED"
        self.timestamp = "UNDEFINED"
        self.resultspath = "UNDEFINED"
        self.properties = {}


    def hasRecord(self):
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
            # read from datafile table
            sql = "SELECT datafileID, status, timestamp, resultspath FROM main.datafiles WHERE datafileID='{0}'".format(self._id)
            records = self._db.select(sql)
            if len(records) == 1:
                self._id, self.status, self.timestamp, self.resultspath = records[0]
            elif len(records) == 0:
                raise ValueError("No records found for datafileID={0}".format(self._id))
            else:
                raise ValueError("Multiple records found for datafileID={0}".format(self._id))

            # read from datafileproperties table
            sql = "SELECT key, value FROM main.datafileproperties WHERE datafileID='{0}'".format(self._id)
            records = self._db.select(sql)
            for record in records:
                self.properties[record[0]] = record[1]
        else:
            raise ValueError("Unable to read from database: no database was set.")


    def write(self):
        """Write datafile information to the database."""
        if self._db:
            # write to datafile table
            if not self.hasRecord():
                sql = "INSERT INTO main.datafiles (datafileID, status, timestamp, resultspath) VALUES ('{0}', '{1}', '{2}', '{3}')".format(self._id, self.status, self.timestamp, self.resultspath)
            else:
                sql = "UPDATE main.datafiles SET status='{1}', timestamp='{2}', resultspath='{3}' WHERE datafileID='{0}'".format(self._id, self.status, self.timestamp, self.resultspath)
            self._db.insert(sql)

            # write to datafileproperties table
            for key in self.properties.keys():
                self.setProperty(key, self.properties[key])
        else:
            raise ValueError("Unable to write to database: no database was set.")


    def setStatus(self, status=''):
        """Set the status of the datafile and update the database.

        **Properties**

        ========== ===========================================
        *status*   new value of the status
        ========== ===========================================
        """
        self.status = status

        if self._db:
            sql = "UPDATE main.datafiles SET status='{0}' WHERE datafileID='{1}'".format(self.status, self._id)
            self._db.execute(sql)


    def setProperty(self, key='', value=''):
        """Set a property and update the database.

        **Properties**

        ========= =================================
        *key*     key name of the property.
        *value*   value of the property.
        ========= =================================
        """
        if self.hasProperty(key):
            sql = "UPDATE main.datafileproperties SET value='{1}' WHERE propertyID='{0}'".format(self.__getPropertyID(key), str(value))
        else:
            sql = "INSERT INTO main.datafileproperties (propertyID, datafileID, key, value) VALUES ('{0}', '{1}', '{2}', '{3}')".format(self.__getPropertyID(key), self._id, key, str(value))
        self._db.execute(sql)

        self.properties[key] = value


    def deleteProperty(self, key=''):
        """Delete a datafile property.

        **Properties**

        ======= ===============================
        *key*   key name of the property.
        ======= ===============================
        """
        if self._db:
            sql = "DELETE FROM main.datafileproperties WHERE propertyID='{0}'".format(self.__getPropertyID(key))
            self._db.execute(sql)

        del self.properties[key]


    def hasProperty(self, key=''):
        """Check if the property with the provided *key* is available.

        **Properties**

        ======= ===============================
        *key*   key name of the property.
        ======= ===============================
        """
        result = False
        records = []

        if self._db:
            sql = "SELECT * FROM main.datafileproperties WHERE propertyID='{0}'".format(self.__getPropertyID(key))
            records = self._db.select(sql)

        if len(records) > 0:
            result = True

        return result


    def __getPropertyID(self, key=''):
        """Return the property id for a specific key.

        **Properties**

        ======= ===============================
        *key*   key name of the property.
        ======= ===============================
        """
        return self._id + "|" + key


    def getID(self):
        """get the ID of this datafile."""
        return self._id


    def summary(self):
        """Summary of the Datafile object."""
        linewidth = 80

        print "="*linewidth
        print "  Summary of the Datafile object."
        print "="*linewidth

        # Datafile info
        print "  %-40s : %s" %("ID", self._id)
        print "  %-40s : %s" %("Status", self.status)
        print "  %-40s : %s" %("Timestamp", self.timestamp)
        print "  %-40s : %s" %("Results path", self.resultspath)

        # Properties
        if len(self.properties) > 0:
            print "  Properties:"
            for key in self.properties.keys():
                print "    %-38s : %s" %(key, self.properties[key])

        print "="*linewidth



class Filter:
    """Container class for filters.

    These filters are stored in the pipeline database.
    """

    def __init__(self, db=None, name='DEFAULT'):
        """Initialisation of filter object.

        **Properties**

        ======== ==============================================
        *db*     database to which to link this datafile to.
        *name*   name of the filter.
        ======== ==============================================
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
        """Detele a filtervalue from the database.

        **Properties**

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
        """Check if at least one of the filters satisfy a substring of
        *string*.

        **Parameters**

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


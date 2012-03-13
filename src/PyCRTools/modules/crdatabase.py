"""
Interface classes to communicate with the Cosmic Ray Pipeline Database.

:Author: Martin van den Akker <martinva@astro.ru.nl>
"""

from pycrtools.io import database as db
import os
import re
import pickle


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
            self.settings.datapath = os.path.join(self.basepath, "data")

        # Location of the resultspath
        if "" != resultspath:
            self.settings.resultspath = os.path.realpath(resultspath)
        elif "" == self.settings.resultspath:
            self.settings.resultspath = os.path.join(self.basepath, "results")


    def __createTables(self):
        """Create the pipeline database tables if they not already exist."""

        if self.db:
            # Event table
            sql = "CREATE TABLE IF NOT EXISTS main.events (eventID INTEGER PRIMARY KEY, timestamp INTEGER, status TEXT)"
            self.db.execute(sql)

            # Event parameters table
            sql = "CREATE TABLE IF NOT EXISTS main.eventparameters (parameterID INTEGER PRIMARY KEY, eventID INTEGER NOT NULL, key TEXT, value TEXT)"
            self.db.execute(sql)

            # Datafile table
            sql = "CREATE TABLE IF NOT EXISTS main.datafiles (datafileID INTEGER PRIMARY KEY, filename TEXT UNIQUE, status TEXT)"
            self.db.execute(sql)

            # Datafile parameters table
            sql = "CREATE TABLE IF NOT EXISTS main.datafileparameters (parameterID INTEGER PRIMARY KEY, datafileID INTEGER NOT NULL, key TEXT, value TEXT)"
            self.db.execute(sql)

            # Stations table
            sql = "CREATE TABLE IF NOT EXISTS main.stations (stationID INTEGER PRIMARY KEY, stationname TEXT, status TEXT)"
            self.db.execute(sql)

            # Station parameters table
            sql = "CREATE TABLE IF NOT EXISTS main.stationparameters (parameterID INTEGER PRIMARY KEY, stationID INTEGER NOT NULL, key TEXT, value TEXT)"
            self.db.execute(sql)

            # Polarizations table
            sql = "CREATE TABLE IF NOT EXISTS main.polarizations (polarizationID INTEGER PRIMARY KEY, antennaset TEXT, direction TEXT, status TEXT, resultsfile TEXT)"
            self.db.execute(sql)

            # Polarization parameters table
            sql = "CREATE TABLE IF NOT EXISTS main.polarizationparameters (parameterID INTEGER PRIMARY KEY, polarizationID INTEGER NOT NULL, key TEXT, value TEXT)"
            self.db.execute(sql)

            # event_datafile table (linking events to datafiles)
            sql = "CREATE TABLE IF NOT EXISTS main.event_datafile (eventID INTEGER NOT NULL, datafileID INTEGER NOT NULL UNIQUE)"
            self.db.execute(sql)

            # datafile_station table (linking datafiles to stations)
            sql = "CREATE TABLE IF NOT EXISTS main.datafile_station (datafileID INTEGER NOT NULL, stationID INTEGER NOT NULL UNIQUE)"
            self.db.execute(sql)

            # station_polarization table (linking stations to polarizations)
            sql = "CREATE TABLE IF NOT EXISTS main.station_polarization (stationID INTEGER NOT NULL, polarizationID INTEGER NOT NULL UNIQUE)"
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
            self.db.execute("DROP TABLE IF EXISTS main.settings")
            self.db.execute("DROP TABLE IF EXISTS main.events")
            self.db.execute("DROP TABLE IF EXISTS main.eventparameters")
            self.db.execute("DROP TABLE IF EXISTS main.datafiles")
            self.db.execute("DROP TABLE IF EXISTS main.datafileparameters")
            self.db.execute("DROP TABLE IF EXISTS main.stations")
            self.db.execute("DROP TABLE IF EXISTS main.stationparameters")
            self.db.execute("DROP TABLE IF EXISTS main.polarizations")
            self.db.execute("DROP TABLE IF EXISTS main.polarizationparameters")
            self.db.execute("DROP TABLE IF EXISTS main.event_datafile")
            self.db.execute("DROP TABLE IF EXISTS main.datafile_station")
            self.db.execute("DROP TABLE IF EXISTS main.station_polarization")
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
            # Construct SQL table
            sql_fields = "e.eventID"
            sql_table = "events AS e"
            sql_selection_list = []

            # Construct selection
            if timestamp:
                sql_selection_list.append("e.timestamp={0}".format(int(timestamp)))
            else:
                if timestamp_start:
                    sql_selection_list.append("e.timestamp>{0}".format(int(timestamp_start)))
                if timestamp_end:
                    sql_selection_list.append("e.timestamp<{0}".format(int(timestamp_end)))
            if status:
                sql_selection_list.append("e.status='{0}'".format(str(status).upper()))

            sql_selection = ""
            if sql_selection_list:
                n_sql_selection_list = len(sql_selection_list)
                for i in range(n_sql_selection_list):
                    sql_selection += sql_selection_list[i]
                    if i < (n_sql_selection_list - 1):
                        sql_selection += " AND "

            #  Building SQL expression
            sql = "SELECT {0} FROM {1}".format(sql_fields, sql_table)
            if sql_selection_list:
                sql += " WHERE {0}".format(sql_selection)

            # Extracting eventIDs
            result = [record[0] for record in self.db.select(sql)]
        else:
            raise ValueError("Unable to read from database: no database was set.")

        return result


    def getDatafileIDs(self, eventID=None, filename=None, status=None):
        """Return a list of datafileIDs satifying the values of this
        functions arguments.

        **Properties**

        =================  ==============================================================
        Parameter          Description
        =================  ==============================================================
        *eventID*          id of the event.
        *filename*         name of the datafile.
        *status*           status of the datafile.
        =================  ==============================================================

        If no arguments are given all datafileIDs are selected. When
        multiple arguments are provided, all returned datafileIDs satisfy
        all argument values.
        """
        result = []

        if self.db:
            # Construct SQL table
            sql_fields = "d.datafileID"
            sql_table_d = "datafiles AS d"
            sql_table_e = "event_datafile AS ed INNER JOIN " + sql_table_d + " ON (ed.datafileID=d.datafileID)"
            sql_table = sql_table_d     # Default sql_table value
            sql_selection_list = []
            if eventID:
                sql_table = sql_table_e
                sql_selection_list.append("ed.eventID={0}".format(int(eventID)))

            # Construct SQL selection
            if filename:
                sql_selection_list.append("d.filename='{0}'".format(str(filename)))
            if status:
                sql_selection_list.append("d.status='{0}'".format(str(status.upper())))

            sql_selection = ""
            if sql_selection_list:
                n_sql_selection_list = len(sql_selection_list)
                for i in range(n_sql_selection_list):
                    sql_selection += sql_selection_list[i]
                    if i < (n_sql_selection_list - 1):
                        sql_selection += " AND "

            # Constructing SQL expression
            sql = "SELECT {0} FROM {1}".format(sql_fields, sql_table)
            if sql_selection_list:
                sql += " WHERE {0}".format(sql_selection)

            # Extracting datafileIDs
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
        result = []

        if self.db:
            # Construct SQL table
            sql_fields = "s.stationID"
            sql_table_s = "stations AS s"
            sql_table_d = "datafile_station AS ds INNER JOIN " + sql_table_s + " ON (ds.stationID=s.stationID)"
            sql_table_e = "event_datafile AS ed INNER JOIN " + sql_table_d + " AND (ed.datafileID=ds.datafileID)"
            sql_table = sql_table_s     # Default sql_table value
            sql_selection_list = []
            if datafileID:
                sql_table = sql_table_d
                sql_selection_list.append("ds.datafileID={0}".format(int(datafileID)))
            if eventID:
                sql_table = sql_table_e
                sql_selection_list.append("ed.eventID={0}".format(int(eventID)))

            # Construct selection
            if stationname:
                sql_selection_list.append("s.stationname='{0}'".format(str(stationname)))
            if status:
                sql_selection_list.append("s.status='{0}'".format(str(status.upper())))

            sql_selection = ""
            if sql_selection_list:
                n_sql_selection_list = len(sql_selection_list)
                for i in range(n_sql_selection_list):
                    sql_selection += sql_selection_list[i]
                    if i < (n_sql_selection_list - 1):
                        sql_selection += " AND "

            # Construct SQL statement
            sql = "SELECT {0} FROM {1}".format(sql_fields, sql_table)
            if sql_selection_list:
                sql += " WHERE {0}".format(sql_selection)

            result = [record[0] for record in self.db.select(sql)]
        else:
            raise ValueError("Unable to read from database: no database was set.")

        return result


    def getPolarizationIDs(self, eventID=None, datafileID=None, stationID=None, antennaset=None, status=None):
        """Return a list of polarizationIDs that satisfy the values of the
        provided arguments of this method.

        **Properties**

        =================  ==============================================================
        Parameter          Description
        =================  ==============================================================
        *eventID*          id of the event.
        *datafileID*       id of the datafile.
        *stationID*        id of the station.
        *antennaset*       type of antennaset.
        *status*           status of the polarization.
        =================  ==============================================================

        If no arguments are given all polarizationIDs are selected. When
        multiple arguments are provided, the returned polarizationIDs satisfy
        all argument values that are provided.
        """
        result = []

        if self.db:
            # Construct SQL table
            sql_fields = "p.polarizationID"
            sql_table_p = "polarizations AS p"
            sql_table_s = "station_polarization AS sp INNER JOIN " + sql_table_p + " ON (sp.polarizationID=p.polarizationID)"
            sql_table_d = "datafile_station AS ds INNER JOIN " + sql_table_s + " AND (ds.stationID=sp.stationID)"
            sql_table_e = "event_datafile AS ed INNER JOIN " + sql_table_d + " AND (ed.datafileID=ds.datafileID)"
            sql_table = sql_table_p     # Default sql_table value
            sql_selection_list = []
            if stationID:
                sql_table = sql_table_s
                sql_selection_list.append("sp.stationID={0}".format(int(stationID)))
            if datafileID:
                sql_table = sql_table_d
                sql_selection_list.append("ds.datafileID={0}".format(int(datafileID)))
            if eventID:
                sql_table = sql_table_e
                sql_selection_list.append("ed.eventID={0}".format(int(eventID)))

            # Construct selection
            if antennaset:
                sql_selection_list.append("p.antennaset='{0}'".format(str(antennaset.upper())))
            if status:
                sql_selection_list.append("p.status='{0}'".format(str(status.upper())))

            sql_selection = ""
            if sql_selection_list:
                n_sql_selection_list = len(sql_selection_list)
                for i in range(n_sql_selection_list):
                    sql_selection += sql_selection_list[i]
                    if i < (n_sql_selection_list - 1):
                        sql_selection += " AND "

            # Construct SQL statement
            sql = "SELECT {0} FROM {1}".format(sql_fields, sql_table)
            if sql_selection_list:
                sql += " WHERE {0}".format(sql_selection)

            result = [record[0] for record in self.db.select(sql)]

        else:
            raise ValueError("Unable to read from database: no database was set.")

        return result


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

        # Polarizations
        if self.db:
            sql = "SELECT COUNT(polarizationID) AS npolarizations FROM main.polarizations"
            n_polarizations = self.db.select(sql)[0][0]
            print "  %-40s : %d" %("Nr. of polarizations", n_polarizations)

        print "-"*linewidth

        # Filters
        if self.db:
            # Number of filter groups
            sql = "SELECT COUNT(DISTINCT name) FROM main.filters"
            records = self.db.select(sql)
            n_filtergroups = records[0][0]
            print "  %-40s : %d" %("Nr. of filter groups", n_filtergroups)

            # Number of filters
            sql = "SELECT COUNT(name) FROM main.filters"
            records = self.db.select(sql)
            n_filters = records[0][0]
            print "  %-40s : %d" %("Nr. of filters", n_filters)

        print "="*linewidth



class Settings(object):
    """Global settings of the CR database.

    This container class contains the following settings from the database:

    * *datapath*: basepath from which the datafiles can be found.
    * *resultspath*: basepath from which the results can be found.

    """

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



class _Parameter(object):
    """General functionality for optional information of an information object."""

    def __init__(self, parent=None):
        """Initialisation of the _Parameter object.

        **Properties**

        ==============  ===============================================================================
        Parameter       Description
        ==============  ===============================================================================
        *parent*        parent object.
        *objectname*    name of the parent object, this is used to acces the correct database tables.
        ==============  ===============================================================================
        """
        self._parent = parent
        self._parent_type = parent.__class__.__name__.lower()
        self._parameter = {}

        # Check if the parent is of the correct type ()
        if self._parent_type in ['event','datafile','station','polarization']:
            self._tablename = "main." + self._parent_type + "parameters"
            self._idlabel = self._parent_type + "ID"
        else:
            raise TypeError("Parent type does not match any parameter table in the database.")

        self._db = self._parent._db
        self._id = self._parent._id


    def __repr__(self):
        """Representation of the parameter object."""
        return self._parameter.__repr__()


    def __getitem__(self, key):
        """Get the value of the parameter *key*."""
        self._parameter.setdefault(key, self.db_read(key))
#        self._parameter[key] = self.db_read(key)
        return self._parameter[key]


    def __setitem__(self, key, value):
        """Set the value of the parameter *key*."""
        self._parameter[key] = value
        self.db_write(key, value)


    def __delitem__(self, key):
        """Delete the entry *key* from the list of parameters."""
        self._parameter.__delitem__(key)
        self.db_delete(key)


    def read(self):
        """Read all parameters key names from the database."""
        for key in self.keys():
            self._parameter.setdefault(key, self.db_read(key))


    def write(self):
        """Write all parameters to the database."""
        if self._db:
            sql = "SELECT key FROM {0} WHERE {1}={2}".format(self._tablename, self._idlabel, self._id)
            db_keys = [record[0] for record in self._db.select(sql)]
            py_keys = [key for key in self._parameter]

            # - Insert/update parameters
            for key in py_keys:
                self.db_write(key, self._parameter[key])

            # - Delete unused parameters
            for key in db_keys:
                if not key in py_keys:
                    sql = "DELETE FROM {0} WHERE {1}={2} AND key='{3}'".format(self._tablename, self._idlabel, self._id, str(key))
                    self._db.execute(sql)
        else:
            raise ValueError("Unable to read from database: no database was set.")


    def keys(self):
        """List of parameter keys."""
        keys = self._parameter.keys()
        if not keys:
            # Read from database
            if self._db:
                sql = "SELECT key FROM {0} WHERE {1}={2}".format(self._tablename, self._idlabel, self._id)
                records = self._db.select(sql)
                keys = [str(r[0]) for r in records]

        return keys


    def db_read(self, key):
        """Read value for parameter *key* from the database.

        **Properties**

        =========  ===================================
        Parameter  Description
        =========  ===================================
        *key*      name of the parameter key.
        =========  ===================================
        """
        value = None

        if self._db:
            sql = "SELECT value FROM {0} WHERE {1}={2} AND key='{3}'".format(self._tablename, self._idlabel, self._id, key)
            records = self._db.select(sql)
            if records:
                value = self.unpickle_parameter(records[0][0])
            else:
                raise ValueError("Invalid key name")
        else:
            raise ValueError("Unable to read from database: no database was set.")

        return value


    def db_write(self, key, value):
        """Write value for parameter *key* to the database.

        **Properties**

        =========  ============================================
        Parameter  Description
        =========  ============================================
        *key*      name of the parameter key.
        *value*    value of the parameter with keyword *key*.
        =========  ============================================
        """
        if self._db:
            if self._id != 0:
                sql = "SELECT parameterID FROM {0} WHERE {1}={2} AND key='{3}'".format(self._tablename, self._idlabel, self._id, key)
                records = self._db.select(sql)
                if records:
                    sql = "UPDATE {0} SET value='{2}' WHERE parameterID={1}".format(self._tablename, int(records[0][0]), self.pickle_parameter(value))
                else:
                    sql = "INSERT INTO {0} ({1}, key, value) VALUES ({2}, '{3}', '{4}')".format(self._tablename, self._idlabel, self._id, key, self.pickle_parameter(value))
                self._db.execute(sql)
        else:
            raise ValueError("Unable to read from database: no database was set.")


    def db_delete(self, key):
        """Delete a parameter entry from the database.

        **Properties**

        =========  ==================================================
        Parameter  Description
        =========  ==================================================
        *key*      Name of the key of the parameter to be deleted.
        =========  ==================================================
        """
        if self._db:
            sql = "DELETE FROM {0} WHERE {1}={2} AND key='{3}'".format(self._tablename, self._idlabel, self._id, key)
            self._db.execute(sql)
        else:
            raise ValueError("Unable to read from database: no database was set.")


    def pickle_parameter(self, py_parameter):
        """Return the parameter as a database friendly pickle
        representation.

        **Properties**

        ==============  ======================================================
        Parameter       Description
        ==============  ======================================================
        *py_parameter*  Python representation of the value of the parameter.
        ==============  ======================================================
        """
        return re.sub("'", '"', pickle.dumps(py_parameter))


    def unpickle_parameter(self, db_parameter):
        """Return the parameter value from parsing a database friendly
        representation of the parameter.

        **Properties**

        ==============  ================================================
        Parameter       Description
        ==============  ================================================
        *db_parameter*  Database representation of the parameter value.
        ==============  ================================================
        """
        return pickle.loads(re.sub('"', "'", str(db_parameter)))


    def summary(self):
        """Summary of the parameters."""
        keys = self.keys()

        if keys:
            print "  Parameters:"
            keys.sort()
            for key in keys:
                print "    %-18s : %s" %(key, self.__getitem__(key))



class Event(object):
    """CR event information.

    This object contains the following information of an event:

    * *timestamp*:  the timestamp of the event in UTC stored as the number of seconds after 1 January 1970.
    * *status*: the status of the event.
    * *datafiles*: a list of datafile information objects (:class:`Datafile`) associated with this event.
    * *parameter*: a dictionary of optional parameters with additional information for this specific event.
    """

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
        self.parameter = _Parameter(parent=self)

        self.settings = Settings(db)

        # Initialize attributes
        if self.inDatabase():           # Read from database
            self.read()


    def __repr__(self):
        return "EventID=%d   timestampe=%d   status='%s'" %(self._id, self.timestamp, self.status.upper())


    def __getitem__(self, key):
        """Get parameter value for *key*."""
        return self.parameter[key]


    def __setitem__(self, key, value):
        """Set parameter value for *key*."""
        self.parameter[key] = value


    def __delitem__(self, key):
        """Delete parameter value for *key*."""
        self.parameter.__delitem__(key)


    def read(self):
        """Read event information from the database."""
        if self._db:
            if self.inDatabase():
                # Read attributes
                sql = "SELECT eventID, timestamp, status FROM main.events WHERE eventID={0}".format(int(self._id))
                records = self._db.select(sql)
                if 1 == len(records):
                    self._id = int(records[0][0])
                    self.timestamp = int(records[0][1])
                    self.status = str(records[0][2]).upper()
                elif 0 == len(records):
                    raise ValueError("No records found for eventID={0}".format(self._id))
                else:
                    raise ValueError("Multiple records found for eventID={0}".format(self._id))

                self.datafiles = []
                # Read datafiles
                sql = "SELECT datafileID FROM main.event_datafile WHERE eventID={0}".format(self._id)
                records = self._db.select(sql)
                for record in records:
                    datafileID = int(record[0])
                    datafile = Datafile(self._db, id=datafileID)
                    datafile.event = self
                    self.datafiles.append(datafile)

                # Reading parameters - Done by _Parameter class
                # self.parameter.read()
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
                sql = "UPDATE main.events SET timestamp={1}, status='{2}' WHERE eventID={0}".format(self._id, int(self.timestamp), str(self.status.upper()))
                self._db.execute(sql)
            else:                       # Create new record
                if 0 == self._id:
                    sql = "INSERT INTO main.events (timestamp, status) VALUES ({0}, '{1}')".format(int(self.timestamp), str(self.status.upper()))
                else:
                    sql = "INSERT INTO main.events (eventID, timestamp, status) VALUES ({0}, {1}, '{2}')".format(self._id, int(self.timestamp), str(self.status.upper()))
                self._id = self._db.insert(sql)

            # Writing datafile information
            for datafile in self.datafiles:
                datafileID = datafile.id
                if recursive:
                    datafile.write()

                sql = "SELECT COUNT(eventID) FROM main.event_datafile WHERE datafileID={0}".format(datafileID)
                result = self._db.select(sql)[0][0]
                if 0 == result:
                    sql = "INSERT INTO main.event_datafile (eventID, datafileID) VALUES ({0}, {1})".format(self._id, datafileID)
                    self._db.insert(sql)

            # Writing parameters
            self.parameter.write()

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
            datafile.event = self       # Reference to parent (event)

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
                    datafile.write(recursive=False)
                # Update the linking table.
                sql = "SELECT eventID FROM main.event_datafile WHERE eventID={0} AND datafileID={1}".format(self._id, datafileID)
                if 0 == len(self._db.select(sql)):
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
        if self.datafiles:
            print "  Datafiles:"
            for datafile in self.datafiles:
                print "    %-6d - %s" %(datafile.id, datafile.filename)

        # Parameters
        self.parameter.summary()

        print "="*linewidth



class Datafile(object):
    """CR datafile information.

    This object contains the following information of a datafile:

    * *filename*: the filename of the datafile.
    * *status*: the status of the datafile.
    * *stations*: a list of station information objects (:class:`Station`) that are stored in the datafile.

    """

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
        self.parameter = _Parameter(parent=self)

        self.settings = Settings(db)

        # Initialize attributes
        if self.inDatabase():
            self.read()


    def __repr__(self):
        return "DatafileID=%d   filename='%s'   status='%s'" %(self._id, self.filename, self.status.upper())


    def __getitem__(self, key):
        """Get parameter value for *key*."""
        return self.parameter[key]


    def __setitem__(self, key, value):
        """Set parameter value for *key*."""
        self.parameter[key] = value


    def __delitem__(self, key):
        """Delete parameter value for *key*."""
        self.parameter.__delitem__(key)


    def read(self):
        """Read datafile information from the database."""
        if self._db:
            if self.inDatabase():
                # Read attributes
                sql = "SELECT datafileID, filename, status FROM main.datafiles WHERE datafileID={0}".format(int(self._id))
                records = self._db.select(sql)
                if 1 == len(records):
                    self._id = int(records[0][0])
                    self.filename = str(records[0][1])
                    self.status = str(records[0][2]).upper()
                elif 0 == len(records):
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

                # Read parameters - Done by _Parameter class
                # self.parameter.read()
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
                sql = "UPDATE main.datafiles SET filename='{1}', status='{2}' WHERE datafileID={0}".format(self._id, str(self.filename), str(self.status.upper()))
                self._db.execute(sql)
            else:
                # Check uniqueness (filename)
                sql = "SELECT datafileID FROM main.datafiles WHERE filename='{0}'".format(self.filename)
                if len(self._db.select(sql)) > 0:
                    print "ERROR: Filename is not uniquely defined"
                    return
                # Add to database
                if 0 == self._id:
                    sql = "INSERT INTO main.datafiles (filename, status) VALUES ('{0}', '{1}')".format(str(self.filename), str(self.status.upper()))
                else:
                    sql = "INSERT INTO main.datafiles (datafileID, filename, status) VALUES ({0}, '{1}', '{2}')".format(self._id, str(self.filename), str(self.status.upper()))
                self._id = self._db.insert(sql)

            # Write station information
            for station in self.stations:
                stationID = station.id
                if recursive:
                    station.write()

                sql = "SELECT COUNT(datafileID) FROM main.datafile_station WHERE stationID={0}".format(stationID)
                result = self._db.select(sql)[0][0]
                if 0 == result:
                    sql = "INSERT INTO main.datafile_station (datafileID, stationID) VALUES ({0}, {1})".format(self._id, stationID)
                    self._db.insert(sql)

            # Write parameter information
            self.parameter.write()

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
            station.datafile = self     #  Reference to parent (datafile)

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
                    station.write(recursive=False)
                # Update linking table
                sql = "SELECT datafileID FROM main.datafile_station WHERE datafileID={0} AND stationID={1}".format(self._id, stationID)
                if 0 == len(self._db.select(sql)):
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

        # Stations
        n_stations = len(self.stations)
        print "  %-40s : %d" %("Number of stations", n_stations)
        if self.stations:
            print "  Stations:"
            for station in self.stations:
                print "  %-6d - %s" %(station.id, station.stationname)

        # Parameters
        self.parameter.summary()

        print "="*linewidth



class Station(object):
    """CR station information.

    This object contains the following information of a station:

    * *stationname*: the name of the station, e.g. CS001, RS203.
    * *status*: the status of the station.
    * *polarizations*: a list of polarization information objects (:class:`Polarization`) stored in this station.

    """

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
        self.polarizations = []
        self.parameter = _Parameter(parent=self)

        self.settings = Settings(db)

        if self.inDatabase():
            self.read()


    def __repr__(self):
        return "StationID=%d   stationname='%s'   status='%s'" %(self._id, self.stationname, self.status.upper())


    def __getitem__(self, key):
        """Get parameter value for *key*."""
        return self.parameter[key]


    def __setitem__(self, key, value):
        """Set parameter value for *key*."""
        self.parameter[key] = value


    def __delitem__(self, key):
        """Delete parameter value for *key*."""
        self.parameter.__delitem__(key)


    def read(self):
        """Read station information from the database."""
        if self._db:
            if self.inDatabase():
                # Read attributes
                sql = "SELECT stationID, stationname, status FROM main.stations WHERE stationID={0}".format(int(self._id))
                records = self._db.select(sql)
                if 1 == len(records):
                    self._id = int(records[0][0])
                    self.stationname = str(records[0][1])
                    self.status = str(records[0][2]).upper()
                elif 0 == len(records):
                    raise ValueError("No records found for stationID={0}".format(self._id))
                else:
                    raise ValueError("Multiple records found for stationID={0}".format(self._id))

                # Read polarization information
                self.polarizations = []
                sql = "SELECT polarizationID FROM main.station_polarization WHERE stationID={0}".format(self._id)
                records = self._db.select(sql)
                for record in records:
                    polarizationID = int(record[0])
                    polarization = Polarization(self._db, id=polarizationID)
                    polarization.station = self
                    self.polarizations.append(polarization)

                # Read parameter information - Done by _Parameter class
                # self.parameter.read()
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
        *recursive*  if *True* write all underlying datastructures (polarizations)
        ===========  =================================================================
        """
        if self._db:
            # Write attributes
            if self.inDatabase():
                sql = "UPDATE main.stations SET stationname='{1}', status='{2}' WHERE stationID={0}".format(self._id, str(self.stationname), str(self.status.upper()))
                self._db.execute(sql)
            else:
                if 0 == self._id:
                    sql = "INSERT INTO main.stations (stationname, status) VALUES ('{0}', '{1}')".format(str(self.stationname), str(self.status.upper()))
                else:
                    sql = "INSERT INTO main.stations (stationID, stationname, status) VALUES ({0}, '{1}', '{2}')".format(self._id, str(self.stationname), str(self.status.upper()))
                self._id = self._db.insert(sql)

            # Write polarization information
            for polarization in self.polarizations:
                polarizationID = polarization.id
                if recursive:
                    polarization.write()

                sql = "SELECT COUNT(stationID) FROM main.station_polarization WHERE polarizationID={0}".format(polarizationID)
                result = self._db.select(sql)[0][0]
                if 0 == result:
                    sql = "INSERT INTO main.station_polarization (stationID, polarizationID) VALUES ({0}, {1})".format(self._id, polarizationID)
                    self._db.insert(sql)

            # Write parameter information
            self.parameter.write()

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


    def addPolarization(self, polarization=None):
        """Add a polarization object to this station.

        This adds a polarization object to the current station object and
        updates the database.

        **Properties**

        ===============  =========================================================
        Parameter        Description
        ===============  =========================================================
        *polarization*   polarization object to be linked to the current station.
        ===============  =========================================================

        Returns *True* if adding the polarization object went
        successfully, *False* otherwise.
        """
        result = False

        if polarization:
            # Update object
            polarizationID = polarization.id
            polarization.station = self # Reference to parent (station)

            # Check for duplicate
            isNew = True
            for p in self.polarizations:
                if p.id == polarizationID:
                    isNew = False
                    break
            if isNew:
                self.polarizations.append(polarization)

            # Update database
            if self._db:
                # Add polarization object to database if it does not yet exist.
                if not polarization.inDatabase():
                    polarization.write(recursive=False)
                # Update linking table
                sql = "SELECT stationID FROM main.station_polarization WHERE stationID={0} AND polarizationID={1}".format(self._id, polarizationID)
                if 0 == len(self._db.select(sql)):
                    sql = "INSERT INTO main.station_polarization (stationID, polarizationID) VALUES ({0}, {1})".format(self._id, polarizationID)
                    self._db.insert(sql)
                    result = True
            else:
                raise ValueError("Unable to write to database: no database was set.")

        return result


    def removePolarization(self, polarizationID=0):
        """Remove polarization object with id= *polarizationID* from
        this station.

        This removes the polarization information object from this station
        object and updates the database. Note that the polarization info
        is not deleted from the database: only the link between the
        station and the polarization objects is deleted.

        **Properties**

        ================  ===================================================================
        Parameter         Description
        ================  ===================================================================
        *polarizationID*  id of the polarization that needs to be removed from this station.
        ================  ===================================================================

        Returns *True* if removing the polarization object went
        successfully, *False* otherwise.
        """
        result = False

        if polarizationID > 0:
            # Update object
            for p in self.polarizations:
                if p.id == polarizationID:
                    self.polarizations.remove(p)

            # Update database
            if self._db:
                sql = "DELETE FROM main.station_polarization WHERE stationID={0} AND polarizationID={1}".format(self._id, polarizationID)
                self._db.execute(sql)
            else:
                raise ValueError("Unable to write to database: no database was set.")
        else:
            print "WARNING: invalid polarizationID!"

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

        # Polarizations
        n_polarizations = len(self.polarizations)
        print "  %-40s : %d" %("Nr. of polarizations",n_polarizations)
        if self.polarizations:
            print "Polarizations:"
            for polarization in self.polarizations:
                print "  %-6d - %s" %(polarization.id, polarization.resultsfile)
            pass

        # Parameters
        self.parameter.summary()

        print "="*linewidth



class Polarization(object):
    """CR polarization information.

    This object contains the following information of a polarization:

    * *antennaset*: the name of the antennaset (e.g. `LBA_OUTER`, `LBA_INNER`, etc.).
    * *direction*: the direction of the polarization component.
    * *status*: the status of the polarization.
    * *resultsfile*: the name of the resultsfile, where all the results from the pipeline are stored.
    * *parameter*: a dictionary of optional parameters. This contains also resulting information from the pipeline.

    """

    def __init__(self, db=None, id=0):
        """Initialisation of the Polarization object.

        **Properties**

        ========== ===========================================
        Parameter  Description
        ========== ===========================================
        *db*       database to which to link this event to.
        *id*       id of the new polarization.
        ========== ===========================================
        """
        self._db = db
        self._id = id

        self.antennaset = ""
        self.direction = ""
        self.status = "NEW"
        self.resultsfile = ""
        self.parameter = _Parameter(parent=self)

        self.settings = Settings(db)

        if self.inDatabase():
            self.read()


    def __repr__(self):
        return "PolarizationID=%d   results='%s'   status='%s'" %(self._id, self.resultsfile, self.status.upper())


    def __getitem__(self, key):
        """Get parameter value for *key*."""
        return self.parameter[key]


    def __setitem__(self, key, value):
        """Set parameter value for *key*."""
        self.parameter[key] = value


    def __delitem__(self, key):
        """Delete parameter value for *key*."""
        self.parameter.__delitem__(key)


    def read(self):
        """Read polarization information from the database."""
        if self._db:
            if self.inDatabase():
                # Read attributes
                sql = "SELECT polarizationID, antennaset, direction, status, resultsfile FROM main.polarizations WHERE polarizationID={0}".format(int(self._id))
                records = self._db.select(sql)
                if 1 == len(records):
                    self._id = int(records[0][0])
                    self.antennaset = str(records[0][1]).upper()
                    self.direction = str(records[0][2])
                    self.status = str(records[0][3]).upper()
                    self.resultsfile = str(records[0][4])
                elif 0 == len(records):
                    raise ValueError("No records found for eventID={0}".format(self._id))
                else:
                    raise ValueError("Multiple records found for eventID={0}".format(self._id))

                # Read parameters - Done by _Parameter class
                # self.parameter.read()

            else:
                print "WARNING: This polarization (id={0}) is not available in the database.".format(self._id)

        else:
            raise ValueError("Unable to read from database: no database was set.")


    def write(self):
        """Write polarization information to the database."""
        if self._db:
            # Write attributes
            if self.inDatabase():
                sql = "UPDATE main.polarizations SET antennaset='{1}', direction='{2}', status='{3}', resultsfile='{4}' WHERE polarizationID={0}".format(self._id, str(self.antennaset.upper()), str(self.direction), str(self.status.upper()), str(self.resultsfile))
                self._db.execute(sql)
            else:
                if 0 == self._id:
                    sql = "INSERT INTO main.polarizations (antennaset, direction, status, resultsfile) VALUES ('{0}', '{1}', '{2}', '{3}')".format(str(self.antennaset.upper()), str(self.direction), str(self.status.upper()), str(self.resultsfile))
                else:
                    sql = "INSERT INTO main.polarizations (polarizationID, antennaset, direction, status, resultsfile) VALUES ({0}, '{1}', '{2}', '{3}', '{4}')".format(self._id, str(self.antennaset.upper()), str(self.direction), str(self.status.upper()), str(self.resultsfile))
                self._id = self._db.insert(sql)

            # Write parameters
            self.parameter.write()

        else:
            raise ValueError("Unable to read from database: no database was set.")


    def inDatabase(self):
        """Check if the polarization information is available in the
        database.

        This method checks if the polarization information of this
        object, uniquely identified by *id*, is in the database.

        If the polarization information is in the database *True* is
        returned, *False* otherwise.
        """
        result = False

        if self._db:
            sql = "SELECT polarizationID FROM main.polarizations WHERE polarizationID={0}".format(int(self._id))
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
        """Summary of the Polarization object."""
        linewidth = 80

        print "="*linewidth
        print "  Summary of the Polarization object."
        print "="*linewidth

        # Polarization information
        print "  %-40s : %d" %("ID", self._id)
        print "  %-40s : %s" %("Antennaset", self.antennaset)
        print "  %-40s : %s" %("Direction", self.direction)
        print "  %-40s : %s" %("Status", self.status)
        print "  %-40s : %s" %("Results file", self.resultsfile)

        # Parameters
        self.parameter.summary()

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
            self.filters = [str(record[0]) for record in self._db.select(sql)]
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
                if 0 == n:
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
            if 0 == n:
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


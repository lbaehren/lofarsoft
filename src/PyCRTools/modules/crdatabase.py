"""
Interface classes to communicate with the Cosmic Ray Pipeline Database.

:Author: Martin van den Akker <m.vandenakker@astro.ru.nl>
"""

from pycrtools.io import database as db
import os
import re
import pickle
import time

debug_mode = False

class CRDatabase(object):
    """Functionality to let the VHECR pipeline communicate with an SQL database."""

    def __init__(self, filename=":memory:", datapath="", resultspath="", lorapath=""):
        """Initialisation of the CRDatabase object.

        **Properties**

        ============= ===============================================
        Parameter     Description
        ============= ===============================================
        *filename*    filename of the database.
        *datapath*    path where the datafiles are stored.
        *resultspath* path where the results are stored.
        *lorapath*    path where the LORA information is stored.
        ============= ===============================================

        If *datapath* is an empty string the ''data'' directory of
        the directory in which the database file is located is used.

        If *resultspath* is an empty string the ''results'' directory of
        the directory in which the database file is located is used.
        """

        # Full filename of the datapath
        self.filename = os.path.realpath(filename)

        # Database object
        self.db = db.Database(self.filename)

        # Initialize database structure
        self.db.open()
        self.__createDatabase()

        # Settings
        self.settings = Settings(self.db)

        # Database version applied in this module
        self.db_required_version = 4
        self.__updateDatabase()

        # Path settings
        self.basepath = os.path.dirname(self.filename)
        datapath_DEFAULT = os.path.join(self.basepath, "data")
        resultspath_DEFAULT = os.path.join(self.basepath, "results")
        lorapath_DEFAULT = os.path.join(self.basepath, "LORA/data")

        # Location of the datapath
        if "" != datapath:
            self.settings.datapath = os.path.realpath(datapath)
        elif "" == self.settings.datapath:
            self.settings.datapath = datapath_DEFAULT

        # Location of the resultspath
        if "" != resultspath:
            self.settings.resultspath = os.path.realpath(resultspath)
        elif "" == self.settings.resultspath:
            self.settings.resultspath = resultspath_DEFAULT

        # Location of the lora path
        if "" != lorapath:
            self.settings.lorapath = os.path.realpath(lorapath)
        elif "" == self.settings.lorapath:
            self.settings.lorapath =lorapath_DEFAULT

        # Name of the file that locks the database file
        self.lockfilename = os.path.join(os.path.dirname(filename), "." + os.path.basename(filename) + ".lock")


    def __createDatabase(self):
        """***DO NOT UPDATE TABLE DEFINITIONS IN THIS METHOD***.

        Create the pipeline database tables if they not already
        exist. Extensions of or changes to the database tables must not be
        applied in this method, these need to be done in a special
        __updateTables method.
        """

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
            INSERT OR IGNORE INTO main.settings (key, value) VALUES ('lorapath', '');
            INSERT OR IGNORE INTO main.settings (key, value) VALUES ('db_version', '0');
            """
            self.db.executescript(sql)

        else:
            raise ValueError("Unable to read from database: no database was set.")


    def __updateDatabase(self):
        self.__updateDatabase_v0_to_v1()
        self.__updateDatabase_v1_to_v2()
        self.__updateDatabase_v2_to_v3()
        self.__updateDatabase_v3_to_v4()


    def addParameterName(self, grouptype, parametername):
        """Add a parameter with name *parametername* to the table
        *grouptype*.

        **Properties**

        ===============  =======================================================
        Parameter        Description
        ===============  =======================================================
        *grouptype*      Type of the group to whcih to add the parameter:
                         'Event', 'Datafile', 'Station', or 'Polarization'.
        *parametername*  Name of the parameter.
        ===============  =======================================================

        If a column with the name *parametername* already exists it
        will be skipped, otherwise it will be created.
        """
        if not grouptype in ["event", "datafile", "station", "polarization"]:
            raise ValueError("Invalid grouptype, should be 'event', 'datafile', 'station' or 'polarization'")

        tablename = grouptype.lower()+"parameters"
        columnname = parametername.lower()

        if not self.isLocked():
            # Find list of parameter names
            parameternames = []
            sql = "pragma table_info({0});".format(tablename)
            if debug_mode: print "SQL: table info: ",sql
            records = self.db.select(sql)
            if records:
                parameternames = [str(r[1]) for r in records[1:]]

            if parameternames:
                if columnname in parameternames:
                    return

            # Add parametername to table
            sql = "ALTER TABLE {0} ADD COLUMN {1} TEXT;".format(tablename, columnname)
            if debug_mode: print "SQL: Adding column: ",sql
            self.db.execute(sql)
        else:
            raise ValueError("DATABASE IS LOCKED: Unable to add a parametername {0} to the database".format(columnname))


    def __convertParameterTable_v0_to_v1(self, tablename="", parameter_keys=[]):

        if not tablename in ["event", "datafile", "station", "polarization"]:
            raise ValueError("Invalid tablename: should be 'event', 'datafile', 'station' or 'polarization'")

        # Create new parameters table
        if debug_mode: print "    Creating new table..." # DEBUG

        columns_string = ""
        for key in parameter_keys:
            columns_string += ", {0} TEXT".format(key)

        sql_list = []
        sql_list.append("CREATE TABLE IF NOT EXISTS main.{0}parameters_new ({0}ID INTEGER PRIMARY KEY {1});".format(tablename, columns_string))
        self.db.executelist(sql_list)

        # Transfer information from old to new parameter table
        if debug_mode: print "    Transfering data to new table..." # DEBUG
        if debug_mode: print "      . retrieving data from db..."   # DEBUG
        id_list = [r[0] for r in self.db.select("SELECT DISTINCT {0}ID FROM main.{0}parameters;".format(tablename))]
        records = self.db.select("SELECT {0}ID, lower(key), value FROM {0}parameters ORDER BY {0}ID;".format(tablename))

        if debug_mode: print "      . extracting parameter keys and values..." # DEBUG
        transfer_keys = {}
        transfer_values = {}
        for _id in id_list:
            transfer_keys[_id] = "{0}ID".format(tablename)
            transfer_values[_id] = "{0}".format(_id)
        for r in records:
            par_id = r[0]
            k = r[1]
            v = r[2]
            if (k in parameter_keys) and (v):
                transfer_keys[par_id] += ", " + k
                transfer_values[par_id] += ", '" + v + "'"

        if debug_mode: print "      . building sql statements..." # DEBUG
        sql_list = []
        for _id in id_list:
            sql_list.append("INSERT INTO {0}parameters_new ({1}) VALUES ({2});".format(tablename, transfer_keys[_id], transfer_values[_id]))
        if debug_mode: print "      . executing sql statements..." # DEBUG
        self.db.executelist(sql_list)

        # Rename new polarizationparameters table
        if debug_mode: print "    Renaming new table (replacing old table)..." # DEBUG
        sql_list = []
        sql_list.append("DROP TABLE main.{0}parameters;".format(tablename))
        sql_list.append("ALTER TABLE main.{0}parameters_new RENAME TO {0}parameters;".format(tablename))
        self.db.executelist(sql_list)


    def __updateDatabase_v0_to_v1(self):
        """Update database from version 0 to version 1.

        The database is only updated if the version of the database is
        0 and the required version is larger than 0.

        List of changes:
        - Change PolarizationParameters table from row based to column based.
        - Change StationParameters table from row based to column based.
        - Change DatafileParameters table from row based to column based.
        - Change EventParameters table from row based to column based.
        """
        db_version_pre = 0
        db_version_post = 1


        if ((self.settings.db_version == db_version_pre) and
            (self.db_required_version >= db_version_post)):

            print "Upgrading database to version {0}...".format(db_version_post)

            # ______________________________________________________________________________
            #                                                 Update polarization parameters
            print "  Updating polarization parameters..." # DEBUG
            new_parameter_keys = ["antenna_positions_array_xyz_m",
                                  "antenna_positions_itrf_m",
                                  "antenna_positions_station_xyz_m",
                                  "antenna_set",
                                  "antennas",
                                  "antennas_final_cable_delays",
                                  "antennas_flagged_delays",
                                  "antennas_residual_cable_delays",
                                  "antennas_spectral_power",
                                  "antennas_timeseries_npeaks",
                                  "antennas_timeseries_rms",
                                  "antennas_with_peaks",
                                  "antennas_with_strong_pulses",
                                  "bad_antennas",
                                  "block",
                                  "blocksize",
                                  "data_length",
                                  "data_length_ms",
                                  "delay_quality_error",
                                  "dipole_calibration_delay_applied",
                                  "dirty_channels",
                                  "filedir",
                                  "filename",
                                  "frequency_range",
                                  "nantennas_with_strong_pulses",
                                  "ndipoles",
                                  "nof_dipole_datasets",
                                  "npeaks_found",
                                  "nyquist_zone",
                                  "pipeline_version",
                                  "plotfiles",
                                  "polarization",
                                  "pulse_core_lora",
                                  "pulse_coreuncertainties_lora",
                                  "pulse_direction",
                                  "pulse_direction_delta_delays_final",
                                  "pulse_direction_delta_delays_start",
                                  "pulse_direction_lora",
                                  "pulse_direction_planewave",
                                  "pulse_end_sample",
                                  "pulse_energy_lora",
                                  "pulse_height",
                                  "pulse_height_incoherent",
                                  "pulse_height_rms",
                                  "pulse_location",
                                  "pulse_moliere_lora",
                                  "pulse_normalized_height",
                                  "pulse_start_sample",
                                  "pulse_time_ms",
                                  "pulses_absolute_arrivaltime",
                                  "pulses_maxima_x",
                                  "pulses_maxima_y",
                                  "pulses_power_snr",
                                  "pulses_refant",
                                  "pulses_sigma",
                                  "pulses_strength",
                                  "pulses_timelags_ns",
                                  "sample_frequency",
                                  "sample_interval",
                                  "sample_number",
                                  "station_antennas_homogeneity_factor",
                                  "station_spectral_power",
                                  "station_timeseries_npeaks",
                                  "station_timeseries_rms",
                                  "status",
                                  "svn_revision",
                                  "telescope",
                                  "time",
                                  "timeseries_power_mean",
                                  "timeseries_power_rms",
                                  "timeseries_raw_rms",
                                  "timeseries_rms"]


            # Create new polarizationparameters table
            self.__convertParameterTable_v0_to_v1("polarization", new_parameter_keys)

            # ______________________________________________________________________________
            #                                                      Update station parameters
            print "  Updating station parameters..."
            new_parameter_keys = []

            self.__convertParameterTable_v0_to_v1("station", new_parameter_keys)

            # ______________________________________________________________________________
            #                                                     Update datafile parameters
            print "  Updating datafile parameters..."
            new_parameter_keys = []

            self.__convertParameterTable_v0_to_v1("datafile", new_parameter_keys)

            # ______________________________________________________________________________
            #                                                        Update event parameters
            print "  Updating event parameters..."
            new_parameter_keys = ["lora_energy_ev",
                                  "lora_energy",
                                  "lora_core_x",
                                  "lora_core_y",
                                  "lora_coreuncertainties",
                                  "lora_elevation",
                                  "lora_utc_time_secs",
                                  "lora_coree_x",
                                  "lora_coree_y",
                                  "lora_core",
                                  "lora_direction",
                                  "lora_10_nsec",
                                  "lora_moliere_rad_m",
                                  "lora_time",
                                  "lora_detectorid",
                                  "lora_moliere",
                                  "lora_azimuth",
                                  "lora_particle_density__m2",
                                  "lora_nsecs",
                                  "lora_posz",
                                  "lora_posx",
                                  "lora_posy"]

            self.__convertParameterTable_v0_to_v1("event", new_parameter_keys)

            # ______________________________________________________________________________
            #                                                 Update database version number
            print "  Updating database version number..." # DEBUG

            self.db.executescript("UPDATE main.settings SET value='{0}' WHERE key='db_version';\n".format(db_version_post))


    def __updateDatabase_v1_to_v2(self):
        """Update database from version 1 to version 2.

        The database is only updated if the version of the database is
        1 and the required version is larger than the database version.

        List of changes:
        - Added locking of database.
        """
        db_version_pre = 1
        db_version_post = 2

        if ((self.settings.db_version == db_version_pre) and
            (self.db_required_version >= db_version_post)):

            print "Upgrading database to version {0}...".format(db_version_post)

            # Add lock setting.
            print "  Updating database locking..."
            self.db.executescript("INSERT OR IGNORE INTO main.settings (key, value) VALUES ('locked', '1');")

            # Upgrade the database version number.
            print "  Updating database version number..." # DEBUG
            self.db.execute("UPDATE main.settings SET value='{0}' WHERE key='db_version';\n".format(db_version_post))


    def __updateDatabase_v2_to_v3(self):
        """Update database from version 2 to version 3.

        The database is only updated if the version of the database is
        2 and the required version is larger than the database version.

        List of changes:
        - Added locking of database.
        """
        db_version_pre = 2
        db_version_post = 3

        if ((self.settings.db_version == db_version_pre) and
            (self.db_required_version >= db_version_post)):

            print "Upgrading database to version {0}...".format(db_version_post)

            # Add parameters for cr_physics pipeline
            print "  Adding additional parameters for cr_physics pipeline..."
            sql = ""
            sql += "ALTER TABLE {0}parameters ADD COLUMN {1} TEXT;".format("event", "plotfiles")
            sql += "ALTER TABLE {0}parameters ADD COLUMN {1} TEXT;".format("event", "crp_average_direction")
            sql += "ALTER TABLE {0}parameters ADD COLUMN {1} TEXT;".format("station", "crp_pulse_direction")
            sql += "ALTER TABLE {0}parameters ADD COLUMN {1} TEXT;".format("polarization", "crp_itrf_antenna_positions")
            sql += "ALTER TABLE {0}parameters ADD COLUMN {1} TEXT;".format("polarization", "crp_pulse_delays")
            sql += "ALTER TABLE {0}parameters ADD COLUMN {1} TEXT;".format("polarization", "crp_pulse_peak_amplitude")
            sql += "ALTER TABLE {0}parameters ADD COLUMN {1} TEXT;".format("polarization", "crp_rms")
            sql += "ALTER TABLE {0}parameters ADD COLUMN {1} TEXT;".format("polarization", "crp_stokes")
            sql += "ALTER TABLE {0}parameters ADD COLUMN {1} TEXT;".format("polarization", "crp_polarization_angle")
            self.db.executescript(sql)

            # Upgrade the database version number.
            print "  Updating database version number..." # DEBUG
            self.db.execute("UPDATE main.settings SET value='{0}' WHERE key='db_version';\n".format(db_version_post))


    def __updateDatabase_v3_to_v4(self):
        """Update database from version 3 to version 4.

        The database is only updated if the version of the database is
        3 and the required version is larger than the database version.

        List of changes:
        - Added new lora keywords to event parameters:
          - lora_datafile
          - lora_ldf
        """
        db_version_pre = 3
        db_version_post = 4

        if ((self.settings.db_version == db_version_pre) and
            (self.db_required_version >= db_version_post)):

            print "Upgrading database to version {0}...".format(db_version_post)

            # Add parameters for cr_physics pipeline
            print "  Adding additional parameters for cr_physics pipeline..."
            sql_list= []
            sql_list.append("ALTER TABLE {0}parameters ADD COLUMN {1} TEXT;".format("event", "lora_datafile"))
            sql_list.append("ALTER TABLE {0}parameters ADD COLUMN {1} TEXT;".format("event", "lora_ldf"))

            # Upgrade the database version number.
            print "  Updating database version number..." # DEBUG
            sql_list.append("UPDATE main.settings SET value='{0}' WHERE key='db_version';".format(db_version_post))
            self.db.executelist(sql_list)


    def getEventIDs(self, timestamp=None, timestamp_start=None, timestamp_end=None, status=None, datafile_name=None, order="e.timestamp"):
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
        *datafile_name*    name of a datafile associated to an event.
        *order*            database fields that are used to sort the returned records.
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
            sql_table_e  = "events AS e"
            sql_table_ed = "event_datafile AS ed INNER JOIN " + sql_table_e + " ON (ed.eventID=e.eventID)"
            sql_table_d  = "datafiles AS d INNER JOIN " + sql_table_ed + "AND (d.datafileID=ed.datafileID)"
            sql_table = sql_table_e
            sql_order = order
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
            if datafile_name:
                sql_table = sql_table_d
                sql_selection_list.append("d.filename='{0}'".format(str(datafile_name)))

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
            if len(sql_order) > 0:
                sql += " ORDER BY {0}".format(sql_order)

            # Extracting eventIDs
            result = [record[0] for record in self.db.select(sql)]
        else:
            raise ValueError("Unable to read from database: no database was set.")

        return result


    def getEvent(self, id=0):
        """Get the event with a specific *id*.

        **Properties**

        =========  ====================================
        Parameter  Description
        =========  ====================================
        *id*       id of the requested event.
        =========  ====================================
        """
        if (id > 0):
            return Event(self.db, id=id)
        else:
            return None


    def deleteEvent(self, id=0):
        """Delete an event and its underlying information objects from
        the database.

        **Properties**

        =========  ====================================
        Parameter  Description
        =========  ====================================
        *id*       id of the requested event.
        =========  ====================================
        """
        # TODO: CRDatabase.deleteEvent() - Add implementation

        sql_list = []

        # TODO: CRDatabase.deleteEvent() - Check existence of eventID
        # TODO: CRDatabase.deleteEvent() - Remove Event with eventID
        # TODO: CRDatabase.deleteEvent() - Remove EventParameters with EventID
        # TODO: CRDatabase.deleteEvent() - Get list of DatafileIDs
        # TODO: CRDatabase.deleteEvent() - Remove Datafiles

        # TEST: CRDatabase.deleteEvent() - Execute the list of SQL statements.
        if self.db and sql_list:
            self.db.executelist(sql_list)
        else:
            raise ValueError("Unable to read from database: no database was set.")

        raise NotImplementedError("Functionality needs to be implemented.")


    def __getDeleteEventSql(self, id=0):
        """Get a list of SQL statements to delete the event
        information from the database.

        **Properties**

        =========  ====================================
        Parameter  Description
        =========  ====================================
        *id*       id of the requested event.
        =========  ====================================
        """
        # TODO: CRDatabase.__getDeleteEventSql() - Add implementation

        raise NotImplementedError("Functionality needs to be implemented.")


    def getDatafileIDs(self, eventID=None, filename=None, status=None, order=""):
        """Return a list of datafileIDs satifying the values of this
        functions arguments.

        **Properties**

        =================  ==============================================================
        Parameter          Description
        =================  ==============================================================
        *eventID*          id of the event.
        *filename*         name of the datafile.
        *status*           status of the datafile.
        *order*            database fields that are used to sort the returned records.
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
            sql_order = order
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
            if len(sql_order) > 0:
                sql += " ORDER BY {0}".format(sql_order)

            # Extracting datafileIDs
            result = [record[0] for record in self.db.select(sql)]
        else:
            raise ValueError("Unable to read from database: no database was set.")

        return result


    def getDatafile(self, id=0):
        """Get the Datafile with a specific *id*.

        **Properties**

        =========  ====================================
        Parameter  Description
        =========  ====================================
        *id*       id of the requested datafile.
        =========  ====================================
        """
        if (id > 0):
            return Datafile(self.db, id=id)
        else:
            return none


    def deleteDatafile(self, id=0):
        """Delete a datafile and its underlying information objects from
        the database.

        **Properties**

        =========  ========================================================
        Parameter  Description
        =========  ========================================================
        *id*       id of the requested datafile entry in the database.
        =========  ========================================================
        """
        # TODO: CRDatabase.deleteDatafile() - Add implementation

        sql_list = []

        # TODO: CRDatabase.deleteDatafile() - Check existence of datafileID
        # TODO: CRDatabase.deleteDatafile() - Remove Datafile with datafileID
        # TODO: CRDatabase.deleteDatafile() - Remove DatafileParameters with datafileID
        # TODO: CRDatabase.deleteDatafile() - Get list of StationIDs
        # TODO: CRDatabase.deleteDatafile() - Remove Stations

        # TEST: CRDatabase.deleteDatafile() - Execute the list of SQL statements.
        if self.db and sql_list:
            self.db.executelist(sql_list)
        else:
            raise ValueError("Unable to read from database: no database was set.")

        raise NotImplementedError("Functionality needs to be implemented.")


    def __getDeleteDatafileSql(self, id=0):
        """Get a list of SQL statements to delete the datafile
        information from the database.

        **Properties**

        =========  ====================================
        Parameter  Description
        =========  ====================================
        *id*       id of the requested event.
        =========  ====================================
        """
        # TODO: CRDatabase.__getDeleteDatafileSql() - Add implementation

        sql_list = []

        return sql_list

        raise NotImplementedError("Functionality needs to be implemented.")


    def getStationIDs(self, eventID=None, datafileID=None, stationname=None, status=None, order="s.stationname"):
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
        *order*            database fields that are used to sort the returned records.
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
            sql_order = order
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
            if len(sql_order) > 0:
                sql += " ORDER BY {0}".format(sql_order)

            result = [record[0] for record in self.db.select(sql)]
        else:
            raise ValueError("Unable to read from database: no database was set.")

        return result


    def getStation(self, id=0):
        """Get the Station with a specific *id*.

        **Properties**

        =========  ====================================
        Parameter  Description
        =========  ====================================
        *id*       id of the requested station.
        =========  ====================================
        """
        if (id > 0):
            return Station(self.db, id=id)
        else:
            return none


    def deleteStation(self, id=0):
        """Delete a station and its underlying information objects from
        the database.

        **Properties**

        =========  ========================================================
        Parameter  Description
        =========  ========================================================
        *id*       id of the requested station entry in the database.
        =========  ========================================================
        """
        # TODO: CRDatabase.deleteStation() - Add implementation

        sql_list = []

        # TODO: CRDatabase.deleteStation() - Check existence of stationID
        # TODO: CRDatabase.deleteStation() - Remove Station with stationID
        # TODO: CRDatabase.deleteStation() - Remove StationParameters with stationID
        # TODO: CRDatabase.deleteStation() - Get list of PolarizationIDs
        # TODO: CRDatabase.deleteStation() - Remove Polarizations

        # TEST: CRDatabase.deleteStation() - Execute the list of SQL statements.
        if self.db and sql_list:
            self.db.executelist(sql_list)
        else:
            raise ValueError("Unable to read from database: no database was set.")

        raise NotImplementedError("Functionality needs to be implemented.")


    def __getDeleteStationSql(self, id=0):
        """Get a list of SQL statements to delete the station
        information from the database.

        **Properties**

        =========  ====================================
        Parameter  Description
        =========  ====================================
        *id*       id of the requested event.
        =========  ====================================
        """
        # TODO: CRDatabase.__getDeleteStationSql() - Add implementation

        sql_list = []

        return sql_list

        raise NotImplementedError("Functionality needs to be implemented.")


    def getPolarizationIDs(self, eventID=None, datafileID=None, stationID=None, antennaset=None, status=None, order=""):
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
        *order*            database fields that are used to sort the returned records.
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
            sql_order = order
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
            if len(sql_order) > 0:
                sql += " ORDER BY {0}".format(sql_order)

            result = [record[0] for record in self.db.select(sql)]

        else:
            raise ValueError("Unable to read from database: no database was set.")

        return result


    def getPolarization(self, id=0):
        """Get the Polarization with a specific *id*.

        **Properties**

        =========  ====================================
        Parameter  Description
        =========  ====================================
        *id*       id of the requested polarization.
        =========  ====================================
        """
        if (id > 0):
            return Polarization(self.db, id=id)
        else:
            return none


    def deletePolarization(self, id=0):
        """Delete a polarization and its underlying information objects from
        the database.

        **Properties**

        =========  ========================================================
        Parameter  Description
        =========  ========================================================
        *id*       id of the requested polarization entry in the database.
        =========  ========================================================
        """
        # TODO: CRDatabase.deletePolarization() - Add implementation

        sql_list = []

        # TODO: CRDatabase.deletePolarization() - Remove Polarization with polarizationID
        # TODO: CRDatabase.deletePolarization() - Remove Polarization with polarizationID
        # TODO: CRDatabase.deletePolarization() - Remove PolarizationParameters with polarizationID

        # TEST: CRDatabase.deletePolarization() - Execute the list of SQL statements.
        if self.db and sql_list:
            self.db.executelist(sql_list)
        else:
            raise ValueError("Unable to read from database: no database was set.")

        raise NotImplementedError("Functionality needs to be implemented.")


    def __getDeletePolarizationSql(self, id=0):
        """Get a list of SQL statements to delete the polarization
        information from the database.

        **Properties**

        =========  ====================================
        Parameter  Description
        =========  ====================================
        *id*       id of the requested event.
        =========  ====================================
        """
        # TODO: CRDatabase.__getDeletePolarizationSql() - Add implementation

        sql_list = []

        return sql_list

        raise NotImplementedError("Functionality needs to be implemented.")


    def isLocked(self):
        """Check if the database is locked."""
        result = True

        if self.db:
            sql = "SELECT value FROM main.settings WHERE key='locked'"
            if (int(self.db.select(sql)[0][0]) == 0):
                if os.path.exists(self.lockfilename):
                    sql = "UPDATE main.settings SET value='{1}' WHERE key='{0}'".format('locked', str(1))
                    self.db.execute(sql)
                    result = True
                else:
                    result = False

        if debug_mode: print "isLocked(): ",result # DEBUG

        return result


    def unlock(self):
        """Unlock the database to be able to modify it. An unlocked database cannot be locked again!
        """
        if self.isLocked():
            if not os.path.exists(self.lockfilename):
                print "UNLOCKING THE DATABASE..."
                print "WARNING: An unlocked database cannot be locked again!"

                sql = "UPDATE main.settings SET value='{1}' WHERE key='{0}'".format('locked', str(0))
                self.db.execute(sql)
            else:
                print "Unable to unlock the database."
        else:
           print "Database is already unlocked."


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
        print "  %-40s : %d" %("Version", self.settings.db_version)

        if self.isLocked():
            db_locked_status = "locked"
        else:
            db_locked_status = "unlocked"
        print "  %-40s : %s" %("Is database locked", db_locked_status)

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

        self._datapath = None
        self._resultspath = None
        self._lorapath = None
        #self._db_version = None


    @property
    def datapath(self):
        """Get the value of the *datapath* variable as set in the database."""
        result = None

        if self._datapath:
            result = self._datapath
        else:
            if self._db:
                sql = "SELECT value FROM main.settings WHERE key='datapath'"
                result = str(self._db.select(sql)[0][0])
                self._datapath = result
            else:
                raise ValueError("Unable to read from database: no database was set.")

        return result


    @datapath.setter
    def datapath(self, value):
        """Set the value of the *datapath* variable in the database.

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
            self._datapath = str(value)
        else:
            raise ValueError("Unable to read from database: no database was set.")


    @property
    def resultspath(self):
        """Get the value of the *resultspath* variable as set in the database."""
        result = None

        if self._resultspath:
            result = self._resultspath
        else:
            if self._db:
                sql = "SELECT value FROM main.settings WHERE key='resultspath'"
                result = str(self._db.select(sql)[0][0])
                self._resultspath = result
            else:
                raise ValueError("Unable to read from database: no database was set.")

        return result


    @resultspath.setter
    def resultspath(self, value):
        """Set the value of the *resultspath* variable in the database.

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
            self._resultspath = str(value)
        else:
            raise ValueError("Unable to read from database: no database was set.")


    @property
    def lorapath(self):
        """Get the value of the *lorapath* variable as set in the database."""
        result = None

        if self._lorapath:
            result = self._lorapath
        else:
            if self._db:
                sql = "SELECT value FROM main.settings WHERE key='lorapath'"
                result = str(self._db.select(sql)[0][0])
                self._lorapath = result
            else:
                raise ValueError("Unable to read from database: no database was set.")

        return result


    @lorapath.setter
    def lorapath(self, value):
        """Set the value of the *lorapath* variable in the database.

        **Properties**

        =========  ===========================================
        Parameter  Description
        =========  ===========================================
        *value*    new value of the *lorapath* variable.
        =========  ===========================================
        """
        if self._db:
            sql = "UPDATE main.settings SET value='{1}' WHERE key='{0}'".format('lorapath',str(value))
            self._db.execute(sql)
            self._lorapath = value
        else:
            raise ValueError("Unable to read from database: no database was set.")


    @property
    def db_version(self):
        """Get the value of the *db_version* variable as set in the database."""
        result = 0

        if self._db:
            sql = "SELECT value FROM settings WHERE key='db_version'"
            result = int(self._db.select(sql)[0][0])
            self._db_version = result   # Not used at the moment.
        else:
            raise ValueError("Unable to read from database: no database was set.")

        return result


    def summary(self):
        """Summary of the Settings object."""
        linewidth = 80

        print "="*linewidth
        print "  Summary of the Settings object."
        print "="*linewidth

        print "  %-40s : %s" %("datapath", self.datapath)
        print "  %-40s : %s" %("resultspath", self.resultspath)
        print "  %-40s : %s" %("lorapath", self.lorapath)
        print "  %-40s : %d" %("db version", self.db_version)



class BaseParameter(object):

    def __init__(self, parent):
        """"""
        self._parent = parent
        self._parent_type = parent.__class__.__name__.lower()
        self._parameter = {}

        # Check if the parent is of the correct type ()
        if self._parent_type in ['event','datafile','station','polarization']:
            self._tablename = self._parent_type + "parameters"
            self._idlabel = self._parent_type + "ID"
        else:
            raise TypeError("Parent type does not match any parameter table in the database.")

        self._keys = self.keys()


    @property
    def _id(self):
        """Return the ID of the parent object."""
        return self._parent._id


    @property
    def _db(self):
        """Return the db reference of the parent object."""
        return self._parent._db


    def __repr__(self):
        """Representation of the parameter object."""
        return self._parameter.__repr__()


    def __getitem__(self, key):
        """Get the value of the parameter *key*."""
        return self._parameter[key]


    def __setitem__(self, key, value):
        """Set the value of the parameter *key*."""
        self._parameter[key] = value


    def __delitem__(self, key):
        """Empty the entry *key* from the list of parameters."""
        self._parameter[key] = None


    def keys(self):
        """Return a list of valid parameter keys."""
        self._keys = self._parameter.keys()

        if not self._keys:
            sql = "pragma table_info({0});".format(self._tablename)
            if debug_mode: print sql    # DEBUG
            records = self._db.select(sql)
            if records:
                self._keys = [str(r[1]) for r in records[1:]]

        return self._keys


    def read(self):
        """Read parameters from the database."""
        sql = "SELECT * FROM {0} WHERE {1}={2}".format(self._tablename, self._idlabel, self._id)
        if debug_mode: print sql        # DEBUG
        records = self._db.select(sql)
        if records:
            values = [v for v in records[0][1:]]
            for i in range(len(values)):
                key = self._keys[i]
                value = values[i]
                if value:
                    self._parameter[key] = self.unpickle_parameter(str(value))


    def write(self):
        """Write parameters to the database."""
        sql_keys = "{0}".format(self._idlabel)
        sql_values = "{0}".format(self._id)
        for key in self._parameter.keys():
            sql_keys += ", {0}".format(key)
            sql_values += ", '{0}'".format(self.pickle_parameter(self._parameter[key]))
        sql = "INSERT OR REPLACE INTO {0} ({1}) VALUES ({2});".format(self._tablename, sql_keys, sql_values)
        self._db.insert(sql)


    def __getUpdateSql(self):
        """Get a list of SQL statements to update the parameter
        information in the database.
        """
        sql_list = []

        if len(self._parameter) > 0:
            sql_keys = "{0}".format(self._idlabel)
            sql_values = "{0}".format(self._id)
            for key in self._parameter.keys():
                sql_keys += ", {0}".format(key)
                sql_values += ", '{0}'".format(self.pickle_parameter(self._parameter[key]))
            sql = "INSERT OR REPLACE INTO {0} ({1}) VALUES ({2});".format(self._tablename, sql_keys, sql_values)

            sql_list.append(sql)

        return sql_list


    def getDeleteSql(self):
        """Get a list of SQL statements to delete the parameter
        information from the database."""
        # TODO: BaseParameter.getDeleteSql() - Add implementation

        raise NotImplementedError("Functionality needs to be implemented.")


    def pickle_parameter(self, value):
        """Return the parameter as a database friendly pickle
        representation.

        **Properties**

        ==============  ======================================================
        Parameter       Description
        ==============  ======================================================
        *value*         Python representation of the value of the parameter.
        ==============  ======================================================
        """
        return re.sub("'", '"', pickle.dumps(value))


    def unpickle_parameter(self, value):
        """Return the parameter value from parsing a database friendly
        representation of the parameter.

        **Properties**

        ==============  ================================================
        Parameter       Description
        ==============  ================================================
        *value*         Database representation of the parameter value.
        ==============  ================================================
        """
        return pickle.loads(re.sub('"', "'", str(value)))


    def summary(self, parent_class_name="BaseParameter"):
        """Summary of the parameter object."""
        linewidth = 80

        print "=" * linewidth
        print "  Summary of the {0} object.".format(parent_class_name)
        print "=" * linewidth

        print "  %-40s : %d" %("Number of parameters", len(self._parameter))

        print "="*linewidth

        for key in self._parameter.keys():
            print "    %-38s : %s" %(key, self._parameter[key])

        print "="*linewidth



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
        self.status = "NEW" # Allowed values: NEW, CR_FOUND, CR_NOT_FOUND, CR_ANALYZED, CR_NOT_ANALYZED
        self.datafiles = []
        self.stations = []
        self.parameter = EventParameter(parent=self)

        self.settings = Settings(db)

        self.inDatabase()

        # Initialize attributes
        if self._inDatabase:           # Read from database
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
            if self._inDatabase:
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

                # Read datafiles
                self.datafiles = []
                sql = "SELECT datafileID FROM main.event_datafile WHERE eventID={0}".format(self._id)
                records = self._db.select(sql)
                for record in records:
                    datafileID = int(record[0])
                    datafile = Datafile(self._db, id=datafileID)
                    datafile.event = self
                    self.datafiles.append(datafile)

                # Read stations
                for d in self.datafiles:
                    # Station objects are already created by creating Datafile objects (see above)
                    self.stations.extend(d.stations)
                    for s in d.stations:
                        s.event = self

                # Reading parameters
                self.parameter.read()
            else:
                print "WARNING: This event (id={0}) is not available in the database.".format(self._id)
        else:
            raise ValueError("Unable to read from database: no database was set.")


    def write(self, recursive=True, parameters=True):
        """Write event information to the database.

        **Properties**

        ============  =================================================================
        Parameter     Description
        ============  =================================================================
        *recursive*   if *True* write all underlying datastructures (datafiles, etc.)
        *parameters*  if *True* write all parameters
        ============  =================================================================
        """
        if self._db:
            # Writing attributes
            if self._inDatabase:        # Update values
                sql = "UPDATE main.events SET timestamp={1}, status='{2}' WHERE eventID={0}".format(self._id, int(self.timestamp), str(self.status.upper()))
                self._db.execute(sql)
            else:                       # Create new record
                if 0 == self._id:
                    sql = "INSERT INTO main.events (timestamp, status) VALUES ({0}, '{1}')".format(int(self.timestamp), str(self.status.upper()))
                else:
                    sql = "INSERT INTO main.events (eventID, timestamp, status) VALUES ({0}, {1}, '{2}')".format(self._id, int(self.timestamp), str(self.status.upper()))
                self._id = self._db.insert(sql)
                self._inDatabase = True

            # Writing datafile information
            if recursive:
                for datafile in self.datafiles:
                    datafile.write(recursive=True)

            # Writing parameters
            if parameters:
                self.parameter.write()

        else:
            raise ValueError("Unable to read from database: no database was set.")


    def update(self, recursive=True, parameters=True):
        """Update event information to the database in a faster buffered way.

        **Properties**

        ============  =================================================================
        Parameter     Description
        ============  =================================================================
        *recursive*   if *True* write all underlying datastructures (datafiles, etc.)
        *parameters*  if *True* write all parameters
        ============  =================================================================
        """
        if self._db:
            sql_list = self.__getUpdateSql(recursive, parameters)

            self._db.executelist(sql_list)
        else:
            raise ValueError("Unable to read from database: no database was set.")


    def __getUpdateSql(self, recursive=True, parameters=True):
        """Get a list of SQL statements to update the event
        information in the database.

        **Properties**

        ============  =================================================================
        Parameter     Description
        ============  =================================================================
        *recursive*   if *True* write all underlying datastructures (datafiles, etc.)
        *parameters*  if *True* write all parameters
        ============  =================================================================
        """
        sql_list = []

        if self._db:
            if self._inDatabase:
                # Add the event information
                sql = "UPDATE main.events SET timestamp={1}, status='{2}' WHERE eventID={0}".format(self._id, int(self.timestamp), str(self.status.upper()))
                sql_list.append(sql)

                # Add the event parameters
                if parameters:
                    sql_sublist = self.parameter.__getUpdateSql()
                    sql_list += sql_sublist

                # Add underlying structures (datafiles)
                if recursive:
                    for datafile in self.datafiles:
                        sql_sublist = datafile.__getUpdateSql(recursive, parameters)
                        sql_list += sql_sublist
            else:
                raise ValueError("No records found for EventID={0}".format(self._id))
        else:
            raise ValueError("Unable to read from database: no database was set.")

        return sql_list


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

        self._inDatabase = result

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
            if 0 == datafile.id:
                datafile.write(recursive=False, parameters=False)
            datafile.event = self       # Reference to parent (event)

            # Check for duplicate
            isNew = True
            for d in self.datafiles:
                if d.id == datafile.id:
                    isNew = False
                    break
            if isNew:
                self.datafiles.append(datafile)

            # Update database
            if self._db:
                if not self._inDatabase:
                    self.write(recursive=False, parameters=False)

                # Update the linking table.
                sql = "SELECT eventID FROM main.event_datafile WHERE eventID={0} AND datafileID={1}".format(self._id, datafile.id)
                if 0 == len(self._db.select(sql)):
                    sql = "INSERT INTO main.event_datafile (eventID, datafileID) VALUES ({0}, {1})".format(self._id, datafile.id)
                    self._db.insert(sql)
                    result = True
            else:
                raise ValueError("Unable to write to database: no database was set.")

        return result


    def removeDatafile(self, datafile=None, datafileID=0):
        """Remove datafile object with id= *datafileID* from this event.

        This removes the datafile information object from this event
        object and updates the database. Note that the datafile info
        is not deleted from the database: only the link between the
        event and the datafile objects is deleted.

        **Properties**

        ============  =============================================================
        Parameter     Description
        ============  =============================================================
        *datafile*    datafile that needs to be removed from this event.
        *datafileID*  id of the datafile that needs to be removed from this event.
        ============  =============================================================

        If *datafile* is not provided, *datafileID* will be used to remove
        it from the list of datafiles.

        Returns *True* if removing the datafile object went
        successfully, *False* otherwise.
        """
        result = False

        if datafile:
            datafileID = datafile.id

        if datafileID > 0:
            # Update object
            for d in self.datafiles:
                if d.id == datafileID:
                    self.datafiles.remove(d)

            # Update database
            if self._db:
                if self._id:
                    sql = "DELETE FROM main.event_datafile WHERE eventID={0} AND datafileID={1}".format(self._id, datafileID)
                    self._db.execute(sql)
            else:
                raise ValueError("Unable to write to database: no database was set.")
        else:
            print "WARNING: invalid datafileID!"

        return result


    def is_cr_found(self):
        """Check if a polarization with a CR detection is found within
        this event.

        Returns *True* if a polarization with a CR detection
        (polarization.status=='OK') is found, *False* otherwise.
        """
        result = False

        if self._db:
            sql = """
            SELECT p.status FROM
            event_datafile AS ed
            INNER JOIN datafile_station AS ds ON (ed.datafileID=ds.datafileID)
            INNER JOIN station_polarization as sp ON (ds.stationID=sp.stationID)
            INNER JOIN polarizations as p ON (sp.polarizationID=p.polarizationID)
            WHERE (p.status='OK' AND ed.eventID={0});""".format(self._id)
            records = self._db.select(sql)

            if records:
                result = True

        return result


    def summary(self, showParameters=False):
        """Summary of the Event object.

        **Properties**

        =================  ==========================================================
        Parameter          Description
        =================  ==========================================================
        *showParameters*   Flag to enable/disable (default disabled) the display of
                           all parameters associated with this information object.
        =================  ==========================================================
        """
        linewidth = 80

        print "="*linewidth
        print "  Summary of the Event object."
        print "="*linewidth

        # Event info
        print "  %-40s : %d" %("ID", self._id)
        print "  %-40s : %s (%s s)" %("Timestamp", time.strftime("%Y-%m-%d %H:%M:%S", time.gmtime(int(self.timestamp))), self.timestamp)
        print "  %-40s : %s" %("Status", self.status)

        # Datafiles
        n_datafiles = len(self.datafiles)
        print "  %-40s : %d" %("Nr. of datafiles", n_datafiles)
        if self.datafiles:
            print "  Datafiles:"
            for datafile in self.datafiles:
                print "    %-6d - %s" %(datafile.id, datafile.filename)

        # Parameters
        if showParameters:
            self.parameter.summary()

        print "="*linewidth



class EventParameter(BaseParameter):

    # Class variables
    key_list = []
    table_name = "eventparameters"
    id_name = "eventID"

    def __init__(self, parent=None):
        """Initialization of the EventParameter object."""
        BaseParameter.__init__(self, parent)

        EventParameter.key_list = self.keys


    def summary(self):
        """Summary of the EventParameter object."""
        BaseParameter.summary(self, "EventParameter")



class Datafile(object):
    """CR datafile information.

    This object contains the following information of a datafile:

    * *filename*: the filename of the datafile.
    * *status*: the status of the datafile.
    * *stations*: a list of station information objects (:class:`Station`) that are stored in the datafile.
    * *parameter*: a dictionary of optional parameters with additional information for this specific datafile.
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
        self.parameter = DatafileParameter(parent=self)

        self.settings = Settings(db)

        self.inDatabase()

        # Initialize attributes
        if self._inDatabase:
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
            if self._inDatabase:
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

                # Read parameters
                self.parameter.read()
            else:
                print "WARNING: This datafile (id={0}) is not available in the database.".format(self._id)
        else:
            raise ValueError("Unable to read from database: no database was set.")


    def write(self, recursive=True, parameters=True):
        """Write datafile information to the database.

        **Properties**

        ============  =================================================================
        Parameter     Description
        ============  =================================================================
        *recursive*   if *True* write all underlying datastructures (stations, etc.)
        *parameters*  if *True* write all parameters
        ============  =================================================================
        """
        if self._db:
            # Write attributes
            if self._inDatabase:
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
                self._inDatabase = True

            # Write station information
            if recursive:
                for station in self.stations:
                    station.write(recursive=True)

            # Write parameter information
            if parameters:
                self.parameter.write()

        else:
            raise ValueError("Unable to read from database: no database was set.")


    def update(self, recursive=True, parameters=True):
        """Update datafile information to the database in a faster buffered way.

        **Properties**

        ============  =================================================================
        Parameter     Description
        ============  =================================================================
        *recursive*   if *True* write all underlying datastructures (stations, etc.)
        *parameters*  if *True* write all parameters
        ============  =================================================================
        """
        if self._db:
            sql_list = self.__getUpdateSql(recursive, parameters)

            self._db.executelist(sql_list)
        else:
            raise ValueError("Unable to read from database: no database was set.")


    def __getUpdateSql(self, recursive=True, parameters=True):
        """Get a list of SQL statements to update the datafile
        information in the database.

        **Properties**

        ============  =================================================================
        Parameter     Description
        ============  =================================================================
        *recursive*   if *True* write all underlying datastructures (stations, etc.)
        *parameters*  if *True* write all parameters
        ============  =================================================================
        """
        sql_list = []

        if self._db:
            if self._inDatabase:
                # Add the datafile information
                sql = "UPDATE main.datafiles SET filename='{1}', status='{2}' WHERE datafileID={0}".format(self._id, str(self.filename), str(self.status.upper()))
                sql_list.append(sql)

                # Add the datafile parameters
                if parameters:
                    sql_sublist = self.parameter.__getUpdateSql()
                    sql_list += sql_sublist

                # Add underlying structures (stations)
                if recursive:
                    for station in self.stations:
                        sql_sublist = station.__getUpdateSql(recursive, parameters)
                        sql_list += sql_sublist
            else:
                raise ValueError("No records found for datafileID={0}".format(self._id))
        else:
            raise NotImplementedError("Functionality needs to be implemented.")

        return sql_list


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

        self._inDatabase = result

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
            if 0 == station.id:
                station.write(recursive=False, parameters=False)
            station.datafile = self     #  Reference to parent (datafile)

            # Check for duplicate
            isNew = True
            for s in self.stations:
                if s.id == station.id:
                    isNew = False
                    break
            if isNew:
                self.stations.append(station)

            # Update database
            if self._db:
                if not self._inDatabase:
                    self.write(recursive=False, parameters=False)

                # Update linking table
                if self._id != 0:
                    sql = "SELECT datafileID FROM main.datafile_station WHERE datafileID={0} AND stationID={1}".format(self._id, station.id)
                    if 0 == len(self._db.select(sql)):
                        sql = "INSERT INTO main.datafile_station (datafileID, stationID) VALUES ({0}, {1})".format(self._id, station.id)
                        self._db.insert(sql)
                        result = True
            else:
                raise ValueError("Unable to read from database: no database was set.")

        return result


    def removeStation(self, station=None, stationID=0):
        """Remove station object with id= *stationID* from this datafile.

        This removes the station information object from this datafile
        object and updates the database. Note that the station info
        is not deleted from the database: only the link between the
        datafile and the station objects is deleted.

        **Properties**

        ============  ===============================================================
        Parameter     Description
        ============  ===============================================================
        *station*     station that needs to be removed from this datafile.
        *stationID*   id of the station that needs to be removed from this datafile.
        ============  ===============================================================

        If *station* is not provided, *stationID* will be used to remove
        it from the list of stations.

        Returns *True* if removing the station object went
        successfully, *False* otherwise.
        """
        result = False

        if station:
            stationID = station.id

        if stationID > 0:
            # Update object
            for s in self.stations:
                if s.id == stationID:
                    self.stations.remove(s)

            # Update database
            if self._db:
                if self._id != 0:
                    sql = "DELETE FROM main.datafile_station WHERE datafileID={0} AND stationID={1}".format(self._id, stationID)
                    self._db.execute(sql)
            else:
                raise ValueError("Unable to write to database: no database was set.")
        else:
            print "WARNING: invalid stationID!"

        return result


    def summary(self, showParameters=False):
        """Summary of the Datafile object.

        **Properties**

        =================  ==========================================================
        Parameter          Description
        =================  ==========================================================
        *showParameters*   Flag to enable/disable (default disabled) the display of
                           all parameters associated with this information object.
        =================  ==========================================================
        """
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
        if showParameters:
            self.parameter.summary()

        print "="*linewidth



class DatafileParameter(BaseParameter):

    # Class variables
    key_list = []
    table_name = "datafileparameters"
    id_name = "datafileID"

    def __init__(self, parent=None):
        """Initialization of the DatafileParameter object."""
        BaseParameter.__init__(self, parent)

        DatafileParameter.key_list = self.keys


    def summary(self):
        """Summary of the DatafileParameter object."""
        BaseParameter.summary(self, "DatafileParameter")



class Station(object):
    """CR station information.

    This object contains the following information of a station:

    * *stationname*: the name of the station, e.g. CS001, RS203.
    * *status*: the status of the station.
    * *polarizations*: a list of polarization information objects (:class:`Polarization`) stored in this station.
    * *parameter*: a dictionary of optional parameters with additional information for this specific station.
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
        self.polarization = {}
        self.parameter = StationParameter(parent=self)

        self.settings = Settings(db)

        self.inDatabase()

        if self._inDatabase:
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
            if self._inDatabase:
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
                self.polarization = {}
                sql = "SELECT polarizationID FROM main.station_polarization WHERE stationID={0}".format(self._id)
                records = self._db.select(sql)
                for record in records:
                    polarizationID = int(record[0])
                    polarization = Polarization(self._db, id=polarizationID)
                    polarization.station = self
                    key = str(polarization.direction)
                    self.polarization[key] = polarization

                # Read parameter information
                self.parameter.read()
            else:
                print "WARNING: This station (id={0}) is not available in the database.".format(self._id)
        else:
            raise ValueError("Unable to read from database: no database was set.")


    def write(self, recursive=True, parameters=True):
        """Write station information to the database.

        **Properties**

        ============  =================================================================
        Parameter     Description
        ============  =================================================================
        *recursive*   if *True* write all underlying datastructures (polarizations)
        *parameters*  if *True* write all parameters
        ============  =================================================================
        """
        if self._db:
            # Write attributes
            if self._inDatabase:
                sql = "UPDATE main.stations SET stationname='{1}', status='{2}' WHERE stationID={0}".format(self._id, str(self.stationname), str(self.status.upper()))
                self._db.execute(sql)
            else:
                if 0 == self._id:
                    sql = "INSERT INTO main.stations (stationname, status) VALUES ('{0}', '{1}')".format(str(self.stationname), str(self.status.upper()))
                else:
                    sql = "INSERT INTO main.stations (stationID, stationname, status) VALUES ({0}, '{1}', '{2}')".format(self._id, str(self.stationname), str(self.status.upper()))
                self._id = self._db.insert(sql)
                self._inDatabase = True

            # Write polarization information
            if recursive:
                for key in self.polarization.keys():
                    self.polarization[key].write()

            # Write parameter information
            if parameters:
                self.parameter.write()

        else:
            raise ValueError("Unable to read from database: no database was set.")


    def update(self, recursive=True, parameters=True):
        """Update station information to the database in a faster buffered way.

        **Properties**

        ============  =================================================================
        Parameter     Description
        ============  =================================================================
        *recursive*   if *True* write all underlying datastructures (polarizations, etc.)
        *parameters*  if *True* write all parameters
        ============  =================================================================
        """
        if self._db:
            sql_list = self.__getUpdateSql(recursive, parameters)

            self._db.executelist(sql_list)
        else:
            raise ValueError("Unable to read from database: no database was set.")


    def __getUpdateSql(self, recursive=True, parameters=True):
        """Get a list of SQL statements to update the station
        information in the database.

        **Properties**

        ============  =================================================================
        Parameter     Description
        ============  =================================================================
        *recursive*   if *True* write all underlying datastructures (polarizations, etc.)
        *parameters*  if *True* write all parameters
        ============  =================================================================
        """
        sql_list = []

        if self._db:
            if self._inDatabase:
                # Add the station information
                sql = "UPDATE main.stations SET stationname='{1}', status='{2}' WHERE stationID={0}".format(self._id, str(self.stationname), str(self.status.upper()))
                sql_list.append(sql)

                # Add the station parameters
                if parameters:
                    sql_sublist = self.parameter.__getUpdateSql()
                    sql_list += sql_sublist

                # Add underlying structures (polarizations)
                if recursive:
                    for polarization in self.polarizations:
                        sql_sublist = polarization.__getUpdateSql(parameters)
                        sql_list += sql_sublist
            else:
                raise ValueError("No records found for stationID={0}".format(self._id))
        else:
            raise ValueError("Unable to read from database: no database was set.")

        return sql_list


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

        self._inDatabase = result

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
            if 0 == polarization.id:
                polarization.write(recursive=False, parameters=False)
            polarization.station = self # Reference to parent (station)
            key = polarization.direction
            if 0 == len(key):
                key = "empty"
            self.polarization[key] = polarization

            # Update database
            if self._db:
                if not self._inDatabase:
                    self.write(recursive=False, parameters=False)

                # Update linking table
                sql = "SELECT stationID FROM main.station_polarization WHERE stationID={0} AND polarizationID={1}".format(self.id, polarization.id)
                if 0 == len(self._db.select(sql)):
                    sql = "INSERT INTO main.station_polarization (stationID, polarizationID) VALUES ({0}, {1})".format(self.id, polarization.id)
                    self._db.insert(sql)
                    result = True
            else:
                raise ValueError("Unable to write to database: no database was set.")

        return result


    def removePolarization(self, polarization=None, key=None, id=0):
        """Remove polarization object with id= *polarizationID* from
        this station.

        This removes the polarization information object from this station
        object and updates the database. Note that the polarization info
        is not deleted from the database: only the link between the
        station and the polarization objects is deleted.

        **Properties**

        ================  ===========================================================================
        Parameter         Description
        ================  ===========================================================================
        *polarization*    polarization that needs to be removed from this station.
        *id*              id of the polarization that needs to be removed from this station.
        *key*             key name of the polarization, this is equal to the polarization direction.
        ================  ===========================================================================

        Only one of the parameters is used to identify the
        polarization that needs to be removed.  The order of
        evaluating the parameters to identify the polarization is
        *polarization*, *id*, *key*

        Returns *True* if removing the polarization object went
        successfully, *False* otherwise.
        """
        result = False
        pol_id = id
        pol_key = key

        if polarization:
            pol_id = polarization.id
            pol_key = polarization.direction
        elif id > 0:
            pol_key = None
            for p in self.polarization:
                if p.id == pol_id:
                    pol_key = p.direction
        elif pol_key in self.polarization.keys():
            id = self.polarization[pol_key].id
        else:
            print "WARNING: unable to determine polarization!"


        if pol_key:
            # Update object
            self.polarization.pop(pol_key)

            # Update database
            if self._db:
                if self.id != 0:
                    sql = "DELETE FROM main.station_polarization WHERE stationID={0} AND polarizationID={1}".format(self.id, pol_id)
                    self._db.execute(sql)
                    result = True
            else:
                raise ValueError("Unable to write to database: no database was set.")

        return result


    def summary(self, showParameters=False):
        """Summary of the Station object.

        **Properties**

        =================  ==========================================================
        Parameter          Description
        =================  ==========================================================
        *showParameters*   Flag to enable/disable (default disabled) the display of
                           all parameters associated with this information object.
        =================  ==========================================================
        """
        linewidth = 80

        print "="*linewidth
        print "  Summary of the Station object."
        print "="*linewidth

        # Station information
        print "  %-40s : %d" %("ID", self._id)
        print "  %-40s : %s" %("Station name", self.stationname)
        print "  %-40s : %s" %("Status", self.status)

        # Polarizations
        n_polarizations = len(self.polarization)
        print "  %-40s : %d" %("Nr. of polarizations",n_polarizations)
        if self.polarization:
            print "Polarizations:"
            for key in self.polarization:
                print "  %-6d - %s - %s" %(self.polarization[key].id, key, self.polarization[key].resultsfile)
            pass

        # Parameters
        if showParameters:
            self.parameter.summary()

        print "="*linewidth



class StationParameter(BaseParameter):

    # Class variables
    key_list = []
    table_name = "stationparameters"
    id_name = "stationID"

    def __init__(self, parent=None):
        """Initialization of the StationParameter object."""
        BaseParameter.__init__(self, parent)

        StationParameter.key_list = self.keys


    def summary(self):
        """Summary of the StationParameter object."""
        BaseParameter.summary(self, "StationParameter")



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
        self.parameter = PolarizationParameter(parent=self)

        self.settings = Settings(db)

        self._inDatabase = self.inDatabase()

        if self._inDatabase:
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
            if self._inDatabase:
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

                # Read parameters
                self.parameter.read()

            else:
                print "WARNING: This polarization (id={0}) is not available in the database.".format(self._id)

        else:
            raise ValueError("Unable to read from database: no database was set.")


    def write(self, recursive=True, parameters=True):
        """Write polarization information to the database.

        **Properties**

        ============  =================================================================
        Parameter     Description
        ============  =================================================================
        *recursive*   if *True* write all underlying data structures.
        *parameters*  if *True* write all parameters.
        ============  =================================================================
        """
        if self._db:
            # Write attributes
            if self._inDatabase:
                sql = "UPDATE main.polarizations SET antennaset='{1}', direction='{2}', status='{3}', resultsfile='{4}' WHERE polarizationID={0}".format(self._id, str(self.antennaset.upper()), str(self.direction), str(self.status.upper()), str(self.resultsfile))
                self._db.execute(sql)
            else:
                if 0 == self._id:
                    sql = "INSERT INTO main.polarizations (antennaset, direction, status, resultsfile) VALUES ('{0}', '{1}', '{2}', '{3}')".format(str(self.antennaset.upper()), str(self.direction), str(self.status.upper()), str(self.resultsfile))
                else:
                    sql = "INSERT INTO main.polarizations (polarizationID, antennaset, direction, status, resultsfile) VALUES ({0}, '{1}', '{2}', '{3}', '{4}')".format(self._id, str(self.antennaset.upper()), str(self.direction), str(self.status.upper()), str(self.resultsfile))
                self._id = self._db.insert(sql)
                self._inDatabase = True

            # Write parameters
            if parameters:
                self.parameter.write()

        else:
            raise ValueError("Unable to read from database: no database was set.")


    def update(self, parameters=True):
        """Update polarization information to the database in a faster buffered way.

        **Properties**

        ============  =================================================================
        Parameter     Description
        ============  =================================================================
        *parameters*  if *True* write all parameters
        ============  =================================================================
        """
        if self._db:
            sql_list = self.__getUpdateSql(parameters)

            self._db.executelist(sql_list)
        else:
            raise ValueError("Unable to read from database: no database was set.")


    def __getUpdateSql(self, parameters=True):
        """Get a list of SQL statements to update the polarization
        information in the database.

        **Properties**

        ============  =================================================================
        Parameter     Description
        ============  =================================================================
        *parameters*  if *True* write all parameters
        ============  =================================================================
        """
        sql_list = []

        if self._db:
            if self._inDatabase:
                # Add the station information
                sql = "UPDATE main.polarizations SET antennaset='{1}', direction='{2}', status='{3}', resultsfile='{4}' WHERE polarizationID={0}".format(self._id, str(self.antennaset.upper()), str(self.direction), str(self.status.upper()), str(self.resultsfile))
                sql_list.append(sql)

                # Add the polarization parameters
                if parameters:
                    sql_sublist = self.parameter.__getUpdateSql()
                    sql_list += sql_sublist
            else:
                raise ValueError("No records found for polarizationID={0}".format(self._id))
        else:
            raise ValueError("Unable to read from database: no database was set.")

        return sql_list


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

        self._inDatabase = result

        return result


    @property
    def id(self):
        """Return the ID of the object as it is identified in the database."""
        return self._id


    def summary(self, showParameters=False):
        """Summary of the Polarization object.

        **Properties**

        =================  ==========================================================
        Parameter          Description
        =================  ==========================================================
        *showParameters*   Flag to enable/disable (default disabled) the display of
                           all parameters associated with this information object.
        =================  ==========================================================
        """
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
        if showParameters:
            self.parameter.summary()
        else:
            print "  %-40s : %d" %("# Parameters", len(self.parameter.keys()))

        print "="*linewidth



class PolarizationParameter(BaseParameter):

    # Class variables
    key_list = []
    table_name = "polarizationparameters"
    id_name = "polarizationID"

    def __init__(self, parent=None):
        """Initialization of the PolarizationParameter object."""
        BaseParameter.__init__(self, parent)

        PolarizationParameter.key_list = self.keys


    def summary(self):
        """Summary of the PolarizationParameter object."""
        BaseParameter.summary(self, "PolarizationParameter")


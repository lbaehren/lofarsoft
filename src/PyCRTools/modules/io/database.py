"""
Provide a simple database interface to access sqlite3 and PostgreSQL databases.

If the database object is initialized with a host variable, the
database object assumes a PostgreSQL database connection.
"""

import sqlite3

try:
    import psycopg2
    have_psycopg2 = True
except ImportError:
    have_psycopg2 = False


class Database(object):
    """Class to handle all python communication with a database."""

    def __init__(self, filename=":memory:", host=None, user=None, password=None, dbname=None, autoclose=True):
        """Initialisation of the Database class.

        **Properties**

        ============ =======================================================
        Parameter    Description
        ============ =======================================================
        *filename*   Filename of the database (1).
        *host*       Hostname of the database server (2).
        *user*       Username to access the database server (2).
        *password*   Password to access the database server (2).
        *dbname*     Name of the database on the database server (2).
        *autoclose*  Automatically close the connection to the database
                     after a transaction [default=True].
        ============ =======================================================
        1) For SQLite database
        2) For PostgreSQL database

        If the database object is initialized with a host variable, the
        database object assumes a PostgreSQL database connection.

        When the filename is ``:memory:`` the database is written to
        and read from memory.
        """

        self._filename = filename
        self._host = host
        self._user = user
        self._password = password
        self._dbname = dbname
        self._dbtype = "unknown"
        self._autoclose = True

        self._timeout = 60

        self._db = None

    def open(self):
        """Open the database.
        """

        if self._db:
            self._db.close()

        if self._host:
            if have_psycopg2:
                # Open PostgreSQL database
                self._db = psycopg2.connect(host=self._host, user=self._user, password=self._password, dbname=self._dbname)
                self._dbtype = "postgresql"
            else:
                raise ImportError("Unable to import 'psycopg2' module which is needed to connect to the PostgreSQL server.")
        else:
            # Open SQLite database
            self._db = sqlite3.connect(self._filename, timeout=self._timeout)
            self._dbtype = "sqlite3"

    def close(self):
        """Close the database."""
        if self._db:
            self._db.close()
            self._db = None

    def insert(self, sql=""):
        """Insert a new record into the database and return the new primary key.

        **Properties**

        ==========  =======================================================
        Parameter   Description
        ==========  =======================================================
        *sql*       SQL statement to execute.
        ==========  =======================================================
        """
        if not self._db:
            self.open()

        newID = 0
        cursor = self._db.cursor()

        try:
            cursor.execute(sql)
            newID = cursor.lastrowid

            self._db.commit()
        except Exception as e:
            print "Error when executing SQL statement:"
            print sql
            print e
            print "rolling back transaction."

            self._db.rollback()

        cursor.close()

        if self._autoclose:
            self.close()

        return newID

    def select(self, sql=""):
        """Select records from the database.

        **Properties**

        =========  =======================================================
        Parameter  Description
        =========  =======================================================
        *sql*      SQL statement to execute.
        =========  =======================================================
        """
        if not self._db:
            self.open()

        cursor = self._db.cursor()

        cursor.execute(sql)
        records = cursor.fetchall()
        cursor.close()

        if self._autoclose:
            self.close()

        return records

    def selectone(self, sql=""):
        """Select one record from the database.

        **Properties**

        =========  =======================================================
        Parameter  Description
        =========  =======================================================
        *sql*      SQL statement to execute.
        =========  =======================================================
        """
        if not self._db:
            self.open()

        cursor = self._db.cursor()

        cursor.execute(sql)
        records = cursor.fetchone()
        cursor.close()

        if self._autoclose:
            self.close()

        return records

    def tableExists(self, table):
        """Select records from the database.

        **Properties**

        =========  =======================================================
        Parameter  Description
        =========  =======================================================
        *table*    Check if table exists
        =========  =======================================================
        """
        if not self._db:
            self.open()

        cursor = self._db.cursor()

        if self._dbtype == "postgresql":
            cursor.execute("select exists(select * from information_schema.tables where table_name='%s')" % (table,))
            exists = cursor.fetchone()[0]
        elif self._dbtype == "sqlite3":
            cursor.execute("SELECT EXISTS(SELECT name FROM sqlite_master WHERE type='table' AND name='%s');" % (table,))
            exists = cursor.fetchone()[0]
        else:
            raise ValueError("Unsupported database type")

        cursor.close()

        if self._autoclose:
            self.close()

        return exists

    def execute(self, sql=""):
        """Execute an sql statement

        **Properties**

        =========  =======================================================
        Parameter  Description
        =========  =======================================================
        *sql*      SQL statement to execute.
        =========  =======================================================
        """

        if not self._db:
            self.open()

        cursor = self._db.cursor()

        try:
            cursor.execute(sql)

            self._db.commit()
        except Exception as e:
            print "Error when executing SQL statement:"
            print sql
            print e
            print "rolling back transaction."

            self._db.rollback()

        cursor.close()

        if self._autoclose:
            self.close()

    def executelist(self, sql_list=[], blocksize=128, verbose=False):
        """Execute a list of SQL statements.

        **Properties**

        ===========  ======================================================================
        Parameter    Description
        ===========  ======================================================================
        *sql_list*   List of SQL commands to process.
        *blocksize*  Maximum size of a block os SQL commands that gets processed
                     in one go. Longer lists are split in multiple blocks [default=128].
        *verbose*    Boolean that [default=False].
        ===========  ======================================================================
        """

        if not self._db:
            self.open()

        cursor = self._db.cursor()

        for sql in sql_list:
            try:
                cursor.execute(sql)

                self._db.commit()
            except Exception as e:
                print "Error when executing SQL statement:"
                print sql
                print e
                print "rolling back transaction."

                self._db.rollback()

        cursor.close()

        if self._autoclose:
            self.close()

    def writescript(self, filename, sql_script):
        """Write SQL script to a file.

        **Properties**

        ============  =======================================================
        Parameter     Description
        ============  =======================================================
        *filename*    Name of the file to which to write the sql_script.
        *sql_script*  Script with SQL statements.
        ============  =======================================================

        """
        sql_file = open(filename, 'w')

        sql_file.write(sql_script)

        if self._autoclose:
            sql_file.close()

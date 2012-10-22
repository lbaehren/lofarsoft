"""
Provide a simple database access interface using sqlite3.
"""

import sqlite3

class Database(object):
    """Class to handle all python communication with an SQLite database."""

    def __init__(self, filename=":memory:"):
        """Initialisation of the Database class.

        **Properties**

        ============ =======================================================
        Parameter    Description
        ============ =======================================================
        *filename*   Filename of the database.
        ============ =======================================================

        When the filename is ``:memory:`` the database is written to
        and read from memory.
        """
        self._filename = filename
        self._timeout = 60
        self._isolation_level = "DEFERRED"

        self._db = None


    def open(self, filename="", isolation_level=None):
        """Open an SQLite database.

        **Properties**

        ================== =======================================================
        Parameter          Description
        ================== =======================================================
        *filename*         Filename of the database.
        *isolation_level*  Type of connection
        ================== =======================================================

        When the filename is ``:memory:`` the database is written to
        and read from memory.
        """
        if self._db:
            self._db.close()

        if filename != "":
            self._filename = filename

        if not isolation_level:
            isolation_level = self._isolation_level

        self._db = sqlite3.connect(self._filename, timeout=self._timeout, isolation_level=isolation_level)


    def close(self):
        """Close the SQLite database."""
        if self._db:
            self._db.close()


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

        with self._db:
            cursor.execute(sql)
            newID = cursor.lastrowid

        cursor.close()

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
            self.open(isolation_level="IMMEDIATE")

        cursor = self._db.cursor()

        cursor.execute(sql)
        records = cursor.fetchall()
        cursor.close()

        return records


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
            self.open(isolation_level=None)

        cursor = self._db.cursor()

        with self._db:
            cursor.execute(sql)

        cursor.close()


    def executescript(self, sql=""):
        """Execute a series of SQL statements.

        **Properties**

        ==========  =======================================================
        Parameter   Description
        ==========  =======================================================
        *sql*       SQL statement to execute.
        ==========  =======================================================
        """
        if not self._db:
            self.open()

        cursor = self._db.cursor()

        cursor.executescript(sql)
        self._db.commit()

        cursor.close()


    def executelist(self, sql_list=[], blocksize=128, verbose=False):
        i_block = 1
        n_block = 1

        if not self._db:
            self.open()

        cursor = self._db.cursor()

        if sql_list:
            sql_block = "BEGIN TRANSACTION;\n"
            for sql_statement in sql_list:
                sql_block += sql_statement + "\n"
                i_block += 1
                if i_block > blocksize:
                    sql_block += "COMMIT;"
                    if verbose:
                        print "commiting sql_block ", n_block, "(",n_block*blocksize,")"
                        print sql_block
                    cursor.executescript(sql_block)
                    self._db.commit()
                    sql_block = "BEGIN TRANSACTION;\n"
                    i_block = 1
                    n_block += 1

            sql_block += "COMMIT;"
            cursor.executescript(sql_block)
            self._db.commit()

        cursor.close()


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

        sql_file.close()


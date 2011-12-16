"""
Provide a simple database access interface using sqlite3.
"""

import sqlite3

class Database:
    """Class to handle all python communication with an SQLite database."""

    def __init__(self, filename=":memory:"):
        """Initialisation of the Database class.

        **Parameters**

        ============ =========================
        *filename*   Filename of the database.
        ============ =========================

        When the *filename* is ``:memory:`` (by default) the database is
        written to and read from memory.
        """
        self._filename = filename
        self._db = sqlite3.connect(self._filename)


    def open(self, filename=":memory:"):
        """Open an SQLite database.

        **Parameters**

        ============ =========================
        *filename*   Filename of the database.
        ============ =========================

        When the *filename* is ``:memory:`` (default) the database is
        written to and read from memory.
        """
        self._db.close()

        if filename != "":
            self._filename = filename

        self._db = sqlite3.connect(self._filename)


    def close(self):
        """Close the SQLite database."""
        if self._db:
            self._db.close()


    def insert(self, sql=""):
        """Insert a new record into the database.

        **Parameters**

        ====== ============================
        *sql*  SQL statement to execute.
        ====== ============================

        Returns the ID of the last row inserted.
        """
        if not self._db:
            self.open()

        newID = 0
        cursor = self._db.cursor()

        cursor.execute(sql)
        newID = cursor.lastrowid
        self._db.commit()
        cursor.close()

        return newID


    def select(self, sql=""):
        """Select records from the database.

        **Parameters**

        ====== ============================
        *sql*  SQL statement to execute.
        ====== ============================

        Returns a list of selected records.
        """
        if not self._db:
            self.open()

        cursor = self._db.cursor()

        cursor.execute(sql)
        records = cursor.fetchall()
        cursor.close()

        return records


    def execute(self, sql=""):
        """Execute an SQL statement.

        **Parameters**

        ====== ===========================
        *sql*  SQL statement to execute.
        ====== ===========================
        """
        if not self._db:
            self.open()

        cursor = self._db.cursor()

        cursor.execute(sql)
        self._db.commit()
        cursor.close()


    def executescript(self, sql=""):
        """Execute a series of SQL statements.

        **Parameters**

        ====== ===========================
        *sql*  SQL statement to execute.
        ====== ===========================
        """
        if not self._db:
            self.open()

        cursor = self._db.cursor()

        cursor.executescript(sql)
        self._db.commit()
        cursor.close()


    def hasTable(self, tablename):
        """Check if a specific table exists.

        **Parameters**

        ============ ==========================================
        *tablename*  Name of the table to check for existence.
        ============ ==========================================

        Returns ``True`` if *tablename* exists, ``False`` otherwise.
        """
        result = False

        sql = "SELECT name FROM sqlite_master WHERE type='table' AND name='%s';" %(tablename)
        result = len(self.select(sql)) > 0

        return result



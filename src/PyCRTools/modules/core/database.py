"""
Provide simple database access interface using sqlite3.
"""

import sqlite3

class Database:
    """Class to handle all python communication with a SQLite database."""

    def __init__(self, filename=":memory:"):
        """Initialisation of the Database class."""
        self._filename = filename
        self._db = sqlite3.connect(self._filename)


    def open(self, filename=""):
        """Open an SQLite database."""
        self._db.close()

        if filename != "":
            self._filename = filename

        self._db = sqlite.connect(self._filename)


    def close(self):
        """Close the SQLite database."""
        if self._db:
            self._db.close()


    def insert(self, sql=""):
        """Insert a new record into the database and return the new primary key"""
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
        """Select records from the database."""
        if not self._db:
            self.open()

        cursor = self._db.cursor()

        cursor.execute(sql)
        records = cursor.fetchall()
        cursor.close()

        return records


    def execute(self, sql=""):
        """Execute an sql statement"""
        if not self._db:
            self.open()

        cursor = self._db.cursor()

        cursor.execute(sql)
        self._db.commit()
        cursor.close()


    def executescript(self, sql=""):
        """Execute a series of SQL statements."""
        if not self._db:
            self.open()

        cursor = self._db.cursor()

        cursor.executescript(sql)
        self._db.commit()
        cursor.close()


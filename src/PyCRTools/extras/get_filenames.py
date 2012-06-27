#! /usr/bin/env python

import sqlite3
from optparse import OptionParser

# Parse commandline options
parser = OptionParser()
parser.add_option("-d", "--database", default="cr.db", help = "filename of database")

(options, args) = parser.parse_args()

# Connect to database
con = sqlite3.connect(options.database)

with con:

    # Get cursor on database
    cur = con.cursor()

    cur.execute("SELECT filename FROM datafiles")

    for e in cur.fetchall():
        print e[0]


#! /usr/bin/env python

import sqlite3
from optparse import OptionParser

# Parse commandline options
parser = OptionParser()
parser.add_option("-d", "--database", default="cr.db", help = "filename of database")

(options, args) = parser.parse_args()

# Connect to database
con = sqlite3.connect(options.database)

files_in_db = []
with con:

    # Get cursor on database
    cur = con.cursor()

    cur.execute("SELECT filename FROM datafiles")

    for e in cur.fetchall():
        files_in_db.append(e[0])

# Print all filenames given as command line arguments that not in the database
for f in args:
    if f not in files_in_db:
        print f


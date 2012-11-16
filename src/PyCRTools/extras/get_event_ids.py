#! /usr/bin/env python

import sqlite3
from optparse import OptionParser

# Parse commandline options
parser = OptionParser()
parser.add_option("-d", "--database", default="cr.db", help = "filename of database")
parser.add_option("-s", "--status", default="", help = "event status")
parser.add_option("-a", "--alt-status", action="store_true", default=False, help = "use alt status")

(options, args) = parser.parse_args()

# Connect to database
con = sqlite3.connect(options.database)

with con:

    # Get cursor on database
    cur = con.cursor()

    if options.status != "":
        if options.alt_status:
            cur.execute("SELECT eventID FROM events WHERE alt_status=?", (options.status, ))
        else:
            cur.execute("SELECT eventID FROM events WHERE status=?", (options.status, ))
    else:
        cur.execute("SELECT eventID FROM events")

    for e in cur.fetchall():
        print e[0]


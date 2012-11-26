"""
"""

import sys
import sqlite3

try:
    import psycopg2
    have_psycopg2 = True
except ImportError:
    have_psycopg2 = False

from optparse import OptionParser

# Parse commandline options
parser = OptionParser()
parser.add_option("-e", "--every-event", action="store_true", default=False, help = "apply action to all events in database")
parser.add_option("-s", "--event-status", default = "NEW", help = "update event status to given value")
parser.add_option("-o", "--old-event-status", default = "NEW", help = "update all events with this status to new status")
parser.add_option("-a", "--alt-status", action = "store_true", default = False, help = "use alternate status for all actions.")
parser.add_option("-d", "--database", default = "crdb.sqlite", help = "filename of database")
parser.add_option("--host", default=None, help="PostgreSQL host.")
parser.add_option("--user", default=None, help="PostgreSQL user.")
parser.add_option("--password", default=None, help="PostgreSQL password.")
parser.add_option("--dbname", default=None, help="PostgreSQL dbname.")

(options, args) = parser.parse_args()

# Connect to database
if have_psycopg2 and options.host:
    # Open PostgreSQL database
    print "Opening connection to PostgreSQL database"
    con = psycopg2.connect(host=options.host, user=options.user, password=options.password, dbname=options.dbname)
else:
    # Open SQLite database
    print "Opening Sqlite database"
    con = sqlite3.connect(options.database, timeout=60.0)

with con:

    # Get cursor on database
    cur = con.cursor()

    if options.every_event:
        cur.execute("SELECT eventID FROM events")

        events = [e[0] for e in cur.fetchall()]
    elif options.old_event_status != "NEW":
        if options.alt_status:
            cur.execute("SELECT eventID FROM events WHERE alt_status=?", (options.old_event_status, ))
        else:
            cur.execute("SELECT eventID FROM events WHERE status=?", (options.old_event_status, ))

        events = [e[0] for e in cur.fetchall()]
    else:
        events = args


    if options.alt_status:
        for eid in events:
            print "setting alt_status of event", eid, "to", options.event_status
            cur.execute('UPDATE events SET alt_status=? WHERE eventID=?', (options.event_status, eid))
    else:
        for eid in events:
            print "setting status of event", eid, "to", options.event_status
            cur.execute('UPDATE events SET status=? WHERE eventID=?', (options.event_status, eid))


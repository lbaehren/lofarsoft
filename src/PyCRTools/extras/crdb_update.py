"""
"""

import sys
import sqlite3

from optparse import OptionParser

# Parse commandline options
parser = OptionParser()
parser.add_option("-e", "--every-event", action="store_true", default=False, help = "apply action to all events in database")
parser.add_option("-s", "--event-status", default = "NEW", help = "update event status to given value")
parser.add_option("-o", "--old-event-status", default = "NEW", help = "update all events with this status to new status")
parser.add_option("-d", "--database", default = "cr.db", help = "filename of database")
parser.add_option("-a", "--alt-status", action = "store_true", default = False, help = "use alternate status for all actions.")

(options, args) = parser.parse_args()

# Connect to database
con = sqlite3.connect(options.database)

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
            print "setting alt_status of event", eid, "to", options.event_alt_status
            cur.execute('UPDATE events SET alt_status=? WHERE eventID=?', (options.event_alt_status, eid))
    else:
        for eid in events:
            print "setting status of event", eid, "to", options.event_status
            cur.execute('UPDATE events SET status=? WHERE eventID=?', (options.event_status, eid))


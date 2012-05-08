"""
"""

import sys
import sqlite3

from optparse import OptionParser

# Parse commandline options
parser = OptionParser()
parser.add_option("-a", "--all-events", action="store_true", default=False, help = "apply action to all events in database")
parser.add_option("-s", "--event-status", default = "NEW", help = "update event status to given value")
parser.add_option("-d", "--database", default = "cr.db", help = "filename of database")

(options, args) = parser.parse_args()

# Connect to database
con = sqlite3.connect(options.database)

with con:

    # Get cursor on database
    cur = con.cursor()

    if options.event_status and options.event_status not in ["NEW", "CR_FOUND", "CR_NOT_FOUND", "CR_ANALYZED", "CR_NOT_ANALYZED"]:
        print parser.print_help()
        sys.exit(1)
    
    if options.all_events:
        cur.execute("SELECT eventID FROM events")

        events = [e[0] for e in cur.fetchall()]
    else:
        events = args


    for eid in events:
        print "setting status of event", eid, "to", options.event_status
        cur.execute('UPDATE events SET status=? WHERE eventID=?', (options.event_status, eid))


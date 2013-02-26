#! /usr/bin/env python

import sqlite3

try:
    import psycopg2
    have_psycopg2 = True
except ImportError:
    have_psycopg2 = False

from optparse import OptionParser

# Parse commandline options
parser = OptionParser()
parser.add_option("-d", "--database", default=None, help = "filename of database")
parser.add_option("-s", "--status", default="", help = "event status")
parser.add_option("-a", "--alt-status", action="store_true", default=False, help = "use alt status")
parser.add_option("--host", default=None, help="PostgreSQL host.")
parser.add_option("--user", default=None, help="PostgreSQL user.")
parser.add_option("--password", default=None, help="PostgreSQL password.")
parser.add_option("--dbname", default=None, help="PostgreSQL dbname.")
(options, args) = parser.parse_args()

# Connect to database
if have_psycopg2 and options.host:
    # Open PostgreSQL database
    conn = psycopg2.connect(host=options.host, user=options.user, password=options.password, dbname=options.dbname)
else:
    # Open SQLite database
    conn = sqlite3.connect(options.database, timeout=60.0)

# Get cursor on database
cur = conn.cursor()

if options.alt_status:
    sql = "SELECT COUNT(eventID) FROM events WHERE (alt_status='{0}')".format(options.status)
    cur.execute(sql)
    
    print cur.fetchone()[0]
    
else:
    sql = "SELECT COUNT(eventID) FROM events WHERE (status='{0}')".format(options.status)
    cur.execute(sql)
    
    print cur.fetchone()[0]


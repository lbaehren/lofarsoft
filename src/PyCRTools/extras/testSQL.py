#! /usr/bin/env python

import sqlite3
from optparse import OptionParser
import pickle
import numpy as np
import re

# Parse commandline options
parser = OptionParser()
parser.add_option("-d", "--database", default="cr.db", help = "filename of database")

(options, args) = parser.parse_args()

# Connect to database
con = sqlite3.connect(options.database)

with con:

    # Get cursor on database
    cur = con.cursor()

    # get all event IDs
    eventIDs = []
    cur.execute("SELECT eventID FROM events")

    for e in cur.fetchall():
        thisID = e[0]
        eventIDs.append(thisID)
        print thisID

    for id in eventIDs:
#        sql = "SELECT filename FROM datafiles WHERE eventID={0};".format(id)
        sql = "SELECT datafileID FROM event_datafile WHERE eventID={0};".format(id)
        cur.execute(sql)
        datafileIDs = []
        for e in cur.fetchall():
            datafileIDs.append(e[0])

        print 'Datafiles for eventID %d' % id
        for datafileID in datafileIDs:
            sql = "SELECT filename FROM datafiles WHERE datafileID={0};".format(datafileID)
            cur.execute(sql)
            print cur.fetchone()[0]

#sql = "SELECT stationID from datafile_station INNER JOIN event_datafile ON datafile_station.datafileID=event_datafile.datafileID AND event_datafile.eventID=95228015"
# lijst van station ids voor dit event
# dan uit station ids: crp_pulse_delay
        sql = "SELECT crp_pulse_delay FROM stationparameters INNER JOIN datafile_station INNER JOIN event_datafile ON datafile_station.datafileID=event_datafile.datafileID AND stationparameters.stationID=datafile_station.stationID AND event_datafile.eventID={0};".format(id)

        cur.execute(sql)
        for e in cur.fetchall():
            a = pickle.loads(re.sub('"', "'", str(e[0])))
            print a

#pickle.loads(re.sub('"', "'", str(value)))


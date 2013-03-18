"""
Updating all events or one event in the database with new data from LORA. Needed if the LORA reconstruction changes or a problem with one event occured.
Usage:

python update_loradata.py

"""

import sys
import sqlite3
from pycrtools import lora

try:
    import psycopg2
    have_psycopg2 = True
except ImportError:
    have_psycopg2 = False

from optparse import OptionParser
import cPickle as pickle
import re

# Parse commandline options
parser = OptionParser()
parser.add_option("-e", "--every_event", default=False, help = "update information for all events")
parser.add_option("-i", "--event_id", default='', help = "update information of this event only")
parser.add_option("--host", default='coma00.science.ru.nl', help="PostgreSQL host.")
parser.add_option("--user", default='crdb', help="PostgreSQL user.")
parser.add_option("--password", default='crdb', help="PostgreSQL password.")
parser.add_option("--dbname", default='crdb', help="PostgreSQL dbname.")
parser.add_option("--lorapath", default='/vol/astro/lofar/vhecr/lora_triggered/LORA', help="LORA data.")
parser.add_option("--key", default="all", help="Updat all LORA data on coma, otherwise specify key")

(options, args) = parser.parse_args()

def unpickle_parameter(db_parameter):
        return pickle.loads(re.sub('"', "'", str(db_parameter)))
        
def pickle_parameter(value):
        return re.sub("'", '"', pickle.dumps(value))
        
# -------------------------------------
# Check whether one or all keys need to be updated
# -------------------------------------------
if options.key == "all":
    keys = ['UTC_Time(secs)', 'nsecs', 'Core(X)', 'Core(Y)', 'Elevation', 'Azimuth', 'Energy(eV)', 'CoreE(X)', 'CoreE(Y)', 'Moliere_rad(m)', 'ElevaErr', 'AziErr', 'EnergyErr(eV)', 'Ne', 'NeErr','CorCoef_XY']
else:
    keys = [options.key] 
       
times = []
eventids = []


# Get access to the database
conn = psycopg2.connect(host=options.host, user=options.user, password=options.password, dbname=options.dbname)

cur = conn.cursor()

if options.every_event:
    cur.execute("""SELECT ep.lora_utc_time_secs, e.eventID FROM
            events AS e
            INNER JOIN eventparameters AS ep ON (e.eventID=ep.eventID)
            """)

    for i in cur.fetchall():
        if i[0] != None: 
           f = unpickle_parameter(i[0])
           e = i[1]
           times.append(int(f))
           eventids.append(e)
else:
    cur.execute("""SELECT ep.lora_utc_time_secs, e.eventID FROM
            events AS e
            INNER JOIN eventparameters AS ep ON (e.eventID=ep.eventID)
             WHERE ((e.eventID='{0}'))""".format(str(options.event_id)))
            """)
    for i in cur.fetchall():
        if i[0] != None: 
           f = unpickle_parameter(i[0])
           e = i[1]
           times.append(int(f))
           eventids.append(e)    
    
    

for i in xrange(len(times)):
    loradata = lora.loraInfo(times[i], options.lorapath)
    for key in keys:
        lora_key = "lora_" + key
        try:
            (l,a) = lora_key.lower(), pickle_parameter(loradata[key])
            cur.execute("UPDATE eventparameters SET {0}='{1}' WHERE eventID='{2}'".format(l,a,eventids[i]))
        except:
            print "Excepted", eventids[i], key
            
conn.commit()
cur.close()
conn.close()









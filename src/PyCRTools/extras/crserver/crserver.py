"""
Serve CR events dynamically from database.
"""

import re
import math
import cPickle as pickle
import sqlite3
import SocketServer
import SimpleHTTPServer

try:
    import psycopg2
    have_psycopg2 = True
except ImportError:
    have_psycopg2 = False

from datetime import datetime
from optparse import OptionParser
from xml.etree.cElementTree import ElementTree, Element, SubElement, ProcessingInstruction
from StringIO import StringIO

# Parse commandline options
parser = OptionParser()
parser.add_option("-p", "--port", default = 8000, type = "int", help="port to bind to")
parser.add_option("-n", "--hostname", default = "localhost", help = "hostname or IP of server")
parser.add_option("-d", "--database", default = "crdb.sqlite", help = "filename of database")
parser.add_option("--host", default=None, help="PostgreSQL host.")
parser.add_option("--user", default=None, help="PostgreSQL user.")
parser.add_option("--password", default=None, help="PostgreSQL password.")
parser.add_option("--dbname", default=None, help="PostgreSQL dbname.")

(options, args) = parser.parse_args()

def good_lora_reconstruction(core_x, core_y, moliere, elevation):
    """
    """

    quality = False

    try:
        if math.sqrt(core_x**2 + core_y**2) < 150:
            if moliere < 100 and moliere > 20:
                if elevation > 55:
                    quality = True
    except:
        pass

    return quality

def unpickle_parameter(db_parameter):
    """Return the parameter value from parsing a database friendly
    representation of the parameter.

    **Properties**

    ==============  ================================================
    Parameter       Description
    ==============  ================================================
    *db_parameter*  Database representation of the parameter value.
    ==============  ================================================
    """

    if db_parameter is None:
        return ""
    else:
        return pickle.loads(re.sub('"', "'", str(db_parameter)))

def event_header(cursor, eventID, station=None, polarization=None, datafile=None):
    """
    """

    header = Element("header")

    # Get event header
    sql = "SELECT timestamp, status, alt_status, statusmessage, alt_statusmessage FROM events WHERE eventID={0}".format(eventID)
    cursor.execute(sql)

    e = cursor.fetchone()
    SubElement(header, "id").text = str(eventID)
    SubElement(header, "timestamp").text = str(datetime.utcfromtimestamp(e[0]))
    SubElement(header, "status").text = str(e[1])
    SubElement(header, "alt_status").text = str(e[2])
    SubElement(header, "statusmessage").text = str(e[3])
    SubElement(header, "alt_statusmessage").text = str(e[4])

    if station:
        s = SubElement(header, "station")
        SubElement(s, "name").text = str(station)

        sql = """SELECT s.status, s.alt_status, s.statusmessage, s.alt_statusmessage FROM
        event_datafile AS ed INNER JOIN datafile_station AS ds ON (ed.datafileID=ds.datafileID)
        INNER JOIN stations AS s ON (ds.stationID=s.stationID)
        WHERE (eventID={0} AND stationName='{1}')
        """.format(eventID, station)
        cursor.execute(sql)

        e = cursor.fetchone()

        SubElement(s, "status").text = e[0]
        SubElement(s, "alt_status").text = e[1]
        SubElement(s, "statusmessage").text = e[2]
        SubElement(s, "alt_statusmessage").text = e[3]

    if polarization:
        p = SubElement(header, "polarization")
        SubElement(p, "name").text = str(polarization)

        sql = """SELECT p.status, p.alt_status, p.statusmessage, p.alt_statusmessage FROM
        event_datafile AS ed INNER JOIN datafile_station AS ds ON (ed.datafileID=ds.datafileID)
        INNER JOIN stations AS s ON (ds.stationID=s.stationID)
        INNER JOIN station_polarization AS sp ON (ds.stationID=sp.stationID)
        INNER JOIN polarizations AS p ON (sp.polarizationID=p.polarizationID)
        WHERE (ed.eventID={0} AND s.stationname='{1}' AND p.direction='{2}')""".format(eventID, station, polarization)
        cursor.execute(sql)

        e = cursor.fetchone()

        SubElement(p, "status").text = e[0]
        SubElement(p, "alt_status").text = e[1]
        SubElement(p, "statusmessage").text = e[2]
        SubElement(p, "alt_statusmessage").text = e[3]

    # Get all stations in event
    sql = """SELECT s.stationname, s.status, s.alt_status FROM
    event_datafile AS ed INNER JOIN datafile_station AS ds ON (ed.datafileID=ds.datafileID)
    INNER JOIN stations AS s ON (ds.stationID=s.stationID)
    WHERE eventID={0}
    """.format(eventID)
    cursor.execute(sql)

    stations = SubElement(header, "stations")
    for e in cursor.fetchall():
        s = SubElement(stations, "station")
        SubElement(s, "name").text = e[0]
        if e[0] == station:
            s.set("current", "true")
        SubElement(s, "status").text = e[1]
        SubElement(s, "alt_status").text = e[2]

        # Get all polarizations for this station
        sql = """SELECT p.direction, p.status, p.alt_status FROM
        event_datafile AS ed INNER JOIN datafile_station AS ds ON (ed.datafileID=ds.datafileID)
        INNER JOIN stations AS s ON (ds.stationID=s.stationID)
        INNER JOIN station_polarization AS sp ON (ds.stationID=sp.stationID)
        INNER JOIN polarizations AS p ON (sp.polarizationID=p.polarizationID)
        WHERE (ed.eventID={0} AND s.stationname='{1}')""".format(eventID, e[0])
        cursor.execute(sql)

        polarizations = SubElement(s, "polarizations")
        for k in cursor.fetchall():
            p = SubElement(polarizations, "polarization")
            SubElement(p, "name").text = k[0]
            SubElement(p, "status").text = k[1]
            SubElement(p, "alt_status").text = k[2]

    # Get all datafiles in event
    sql = """SELECT filename FROM
    event_datafile AS ed INNER JOIN datafiles AS df ON (ed.datafileID=df.datafileID)
    WHERE ed.eventID={0}""".format(eventID)
    cursor.execute(sql)

    datafiles = SubElement(header, "datafiles")
    for e in cursor.fetchall():
        d = SubElement(datafiles, "datafile")
        SubElement(d, "name").text = e[0]

    return header

def events_handler():
    """Handle summary of events.
    """

    # Connect to database
    if have_psycopg2 and options.host:
        # Open PostgreSQL database
        print "Opening connection to PostgreSQL database"
        conn = psycopg2.connect(host=options.host, user=options.user, password=options.password, dbname=options.dbname)
    else:
        # Open SQLite database
        print "Opening Sqlite database"
        conn = sqlite3.connect(options.database, timeout=60.0)

    # Create cursor
    c = conn.cursor()

    # Fetch all event IDs
    c.execute("""SELECT e.eventID, e.timestamp, e.status, e.alt_status, lora_energy, lora_core_x, lora_core_y, lora_azimuth, lora_elevation, lora_moliere FROM
    events AS e LEFT JOIN eventparameters AS ep ON (e.eventID=ep.eventID)""")

    # Generate empty XML
    elements = Element("elements")

    for e in c.fetchall():

        event = SubElement(elements, "event")

        SubElement(event, "id").text = str(e[0])
        SubElement(event, "timestamp").text = str(datetime.utcfromtimestamp(e[1]))
        SubElement(event, "status").text = str(e[2])
        SubElement(event, "alt_status").text = str(e[3])

        lora = SubElement(event, "lora")

        energy = unpickle_parameter(e[4])
        core_x = unpickle_parameter(e[5])
        core_y = unpickle_parameter(e[6])
        azimuth = unpickle_parameter(e[7])
        elevation = unpickle_parameter(e[8])
        moliere = unpickle_parameter(e[9])

        if good_lora_reconstruction(core_x, core_y, moliere, elevation):
            lora.attrib['good_reconstruction'] = "true"

        SubElement(lora, "energy").text = str(energy)
        SubElement(lora, "core_x").text = str(core_x)
        SubElement(lora, "core_y").text = str(core_y)
        SubElement(lora, "azimuth").text = str(azimuth)
        SubElement(lora, "elevation").text = str(elevation)
        SubElement(lora, "moliere").text = str(moliere)

    # Open string file descriptor for output
    f = StringIO()

    # Write header information
    f.write('<?xml version="1.0" ?>')
    f.write('<?xml-stylesheet type="text/xsl" href="/layout/events.xsl"?>')

    # Write XML DOM to string file descriptor
    ElementTree(elements).write(f)

    return f.getvalue()

def event_handler(eventID):
    """Handle a single event.
    """

    # Connect to database
    if have_psycopg2 and options.host:
        # Open PostgreSQL database
        print "Opening connection to PostgreSQL database"
        conn = psycopg2.connect(host=options.host, user=options.user, password=options.password, dbname=options.dbname)
    else:
        # Open SQLite database
        print "Opening Sqlite database"
        conn = sqlite3.connect(options.database, timeout=60.0)

    # Create cursor
    c = conn.cursor()

    # Generate empty XML
    elements = Element("elements")

    # Add event header
    elements.append(event_header(c, eventID))

    # Fetch event parameter keys
    parameters = SubElement(elements, "parameters")
    figures = SubElement(elements, "figures")

    if options.host:
        # PostgreSQL
        c.execute("SELECT column_name FROM information_schema.columns WHERE table_name ='eventparameters'")

        keys = [str(e[0]) for e in c.fetchall()[1:]]
    else:
        # Sqlite
        c.execute("PRAGMA table_info(eventparameters)")

        keys = [str(e[1]) for e in c.fetchall()[1:]]

    # Fetch event parameter values
    sql = "SELECT * FROM eventparameters WHERE eventID={0}".format(eventID)
    c.execute(sql)

    v = c.fetchone()
    if v is not None and len(v) > 1:

        values = [unpickle_parameter(e) for e in v[1:]]

        for e in zip(keys, values):

            parameter = SubElement(parameters, "parameter")

            SubElement(parameter, "key").text = e[0]
            SubElement(parameter, "value").text = str(e[1])

    # Fetch event level figures
    sql = "SELECT plotfiles, crp_plotfiles FROM eventparameters WHERE eventID={0}".format(eventID)
    c.execute(sql)

    v = c.fetchone()
    if v is not None and len(v) > 1:

        values = [unpickle_parameter(e) for e in v]

        for e in zip(keys, values):

            for p in e[1]:
                figure = SubElement(figures, "figure")
                SubElement(figure, "path").text = "/results"+str(p).split("results")[1]

    sql = "SELECT lora_ldf FROM eventparameters WHERE eventID={0}".format(eventID)
    c.execute(sql)
    v = c.fetchone()

    if v is not None:
        print v, unpickle_parameter(v)
        figure = SubElement(figures, "figure")
        SubElement(figure, "path").text = "/LORA"+str(unpickle_parameter(v)).split("results")[1]

    # Open string file descriptor for output
    f = StringIO()

    # Write header information
    f.write('<?xml version="1.0" ?>')
    f.write('<?xml-stylesheet type="text/xsl" href="/layout/event.xsl"?>')

    # Write XML DOM to string file descriptor
    ElementTree(elements).write(f)

    return f.getvalue()

def station_handler(eventID, station_name):
    """Handle a single event.
    """

    # Connect to database
    if have_psycopg2 and options.host:
        # Open PostgreSQL database
        print "Opening connection to PostgreSQL database"
        conn = psycopg2.connect(host=options.host, user=options.user, password=options.password, dbname=options.dbname)
    else:
        # Open SQLite database
        print "Opening Sqlite database"
        conn = sqlite3.connect(options.database, timeout=60.0)

    # Create cursor
    c = conn.cursor()

    # Generate empty XML
    elements = Element("elements")

    # Add event header
    header = event_header(c, eventID, station=station_name)
    elements.append(header)

    # Get all polarizations for this station
    sql = """SELECT p.direction, p.status, p.alt_status FROM
    event_datafile AS ed INNER JOIN datafile_station AS ds ON (ed.datafileID=ds.datafileID)
    INNER JOIN stations AS s ON (ds.stationID=s.stationID)
    INNER JOIN station_polarization AS sp ON (ds.stationID=sp.stationID)
    INNER JOIN polarizations AS p ON (sp.polarizationID=p.polarizationID)
    WHERE (ed.eventID={0} AND s.stationname='{1}')""".format(eventID, station_name)
    c.execute(sql)

    polarizations = SubElement(header, "polarizations")
    for e in c.fetchall():
        s = SubElement(polarizations, "polarization")
        SubElement(s, "name").text = e[0]
        SubElement(s, "status").text = e[1]
        SubElement(s, "alt_status").text = e[1]

    # Fetch all station parameters
    parameters = SubElement(elements, "parameters")
    figures = SubElement(elements, "figures")

    if options.host:
        # PostgreSQL
        c.execute("SELECT column_name FROM information_schema.columns WHERE table_name ='stationparameters'")

        keys = [str(e[0]) for e in c.fetchall()[1:]]
    else:
        # Sqlite
        c.execute("PRAGMA table_info(stationparameters)")

        keys = [str(e[1]) for e in c.fetchall()[1:]]

    # Fetch all station parameter values
    sql = """SELECT sp.* FROM
    event_datafile AS ed INNER JOIN datafile_station AS ds ON (ed.datafileID=ds.datafileID)
    INNER JOIN stations AS s ON (ds.stationID=s.stationID)
    INNER JOIN stationparameters AS sp ON (s.stationID=sp.stationID)
    WHERE (ed.eventID={0} AND s.stationname='{1}')""".format(eventID, station_name)
    c.execute(sql)

    v = c.fetchone()
    if v is not None and len(v) > 1:

        values = [unpickle_parameter(e) for e in v[1:]]

        for e in zip(keys, values):

            parameter = SubElement(parameters, "parameter")

            SubElement(parameter, "key").text = e[0]
            SubElement(parameter, "value").text = str(e[1])

    # Fetch station level figures
    sql = """SELECT sp.plotfiles, sp.crp_plotfiles FROM
    event_datafile AS ed INNER JOIN datafile_station AS ds ON (ed.datafileID=ds.datafileID)
    INNER JOIN stations AS s ON (ds.stationID=s.stationID)
    INNER JOIN stationparameters AS sp ON (s.stationID=sp.stationID)
    WHERE (ed.eventID={0} AND s.stationname='{1}')""".format(eventID, station_name)
    c.execute(sql)

    v = c.fetchone()
    if v is not None and len(v) > 1:

        values = [unpickle_parameter(e) for e in v]

        for e in zip(keys, values):

            for p in e[1]:
                figure = SubElement(figures, "figure")
                SubElement(figure, "path").text = "/results"+str(p).split("results")[1]

    # Open string file descriptor for output
    f = StringIO()

    # Write header information
    f.write('<?xml version="1.0" ?>')
    f.write('<?xml-stylesheet type="text/xsl" href="/layout/station.xsl"?>')

    # Write XML DOM to string file descriptor
    ElementTree(elements).write(f)

    return f.getvalue()

def polarization_handler(eventID, station_name, polarization_direction):
    """Handle a single event.
    """

    # Connect to database
    if have_psycopg2 and options.host:
        # Open PostgreSQL database
        print "Opening connection to PostgreSQL database"
        conn = psycopg2.connect(host=options.host, user=options.user, password=options.password, dbname=options.dbname)
    else:
        # Open SQLite database
        print "Opening Sqlite database"
        conn = sqlite3.connect(options.database, timeout=60.0)

    # Create cursor
    c = conn.cursor()

    # Generate empty XML
    elements = Element("elements")

    # Add event header
    header = event_header(c, eventID, station=station_name, polarization=polarization_direction)
    elements.append(header)

    # Get all polarizations for this station
    sql = """SELECT direction FROM
    event_datafile AS ed INNER JOIN datafile_station AS ds ON (ed.datafileID=ds.datafileID)
    INNER JOIN stations AS s ON (ds.stationID=s.stationID)
    INNER JOIN station_polarization AS sp ON (ds.stationID=sp.stationID)
    INNER JOIN polarizations AS p ON (sp.polarizationID=p.polarizationID)
    WHERE (ed.eventID={0} AND s.stationname='{1}')""".format(eventID, station_name)
    c.execute(sql)

    polarizations = SubElement(header, "polarizations")
    for e in c.fetchall():
        s = SubElement(polarizations, "polarization")
        SubElement(s, "name").text = e[0]
        if e[0] == polarization_direction:
            s.set("current", "true")

    # Fetch polarization parameter keys
    parameters = SubElement(elements, "parameters")
    figures = SubElement(elements, "figures")

    if options.host:
        # PostgreSQL
        c.execute("SELECT column_name FROM information_schema.columns WHERE table_name ='polarizationparameters'")

        keys = [str(e[0]) for e in c.fetchall()[1:]]
    else:
        # Sqlite
        c.execute("PRAGMA table_info(polarizationparameters)")

        keys = [str(e[1]) for e in c.fetchall()[1:]]

    # Fetch polarization parameter values
    sql = """SELECT pp.* FROM
    event_datafile AS ed INNER JOIN datafile_station AS ds ON (ed.datafileID=ds.datafileID)
    INNER JOIN stations AS s ON (ds.stationID=s.stationID)
    INNER JOIN station_polarization AS sp ON (ds.stationID=sp.stationID)
    INNER JOIN polarizations AS p ON (sp.polarizationID=p.polarizationID)
    INNER JOIN polarizationparameters AS pp ON (p.polarizationID=pp.polarizationID)
    WHERE (ed.eventID={0} AND s.stationname='{1}' AND p.direction='{2}')
    """.format(eventID, station_name, polarization_direction)
    c.execute(sql)

    v = c.fetchone()
    if v is not None and len(v) > 1:

        values = [unpickle_parameter(e) for e in v[1:]]

        for e in zip(keys, values):

            parameter = SubElement(parameters, "parameter")

            SubElement(parameter, "key").text = e[0]
            SubElement(parameter, "value").text = str(e[1])

    # Fetch polarization level figures
    sql = """SELECT pp.plotfiles, pp.crp_plotfiles FROM
    event_datafile AS ed INNER JOIN datafile_station AS ds ON (ed.datafileID=ds.datafileID)
    INNER JOIN stations AS s ON (ds.stationID=s.stationID)
    INNER JOIN station_polarization AS sp ON (ds.stationID=sp.stationID)
    INNER JOIN polarizations AS p ON (sp.polarizationID=p.polarizationID)
    INNER JOIN polarizationparameters AS pp ON (p.polarizationID=pp.polarizationID)
    WHERE (ed.eventID={0} AND s.stationname='{1}' AND p.direction='{2}')
    """.format(eventID, station_name, polarization_direction)
    c.execute(sql)

    v = c.fetchone()
    if v is not None and len(v) > 1:

        values = [unpickle_parameter(e) for e in v]

        for e in zip(keys, values):

            for p in e[1]:
                figure = SubElement(figures, "figure")
                SubElement(figure, "path").text = "/results"+str(p).split("results")[1]

    # Open string file descriptor for output
    f = StringIO()

    # Write header information
    f.write('<?xml version="1.0" ?>')
    f.write('<?xml-stylesheet type="text/xsl" href="/layout/polarization.xsl"?>')

    # Write XML DOM to string file descriptor
    ElementTree(elements).write(f)

    return f.getvalue()

class CustomHandler(SimpleHTTPServer.SimpleHTTPRequestHandler):

    def do_GET(self):

        try:

            if self.path == '/events':

                s = events_handler()

                self.send_response(200)
                self.send_header('Content-type','text/xml')
                self.end_headers()
                self.wfile.write(s)

            elif re.match(r'/events/[0-9]+$', self.path):

                m = re.match(r'/events/([0-9]+)$', self.path)

                s = event_handler(int(m.group(1)))

                self.send_response(200)
                self.send_header('Content-type','text/xml')
                self.end_headers()
                self.wfile.write(s)

            elif re.match(r'/events/([0-9]+)/([A-Z][A-Z][0-9][0-9][0-9])$', self.path):

                m = re.match(r'/events/([0-9]+)/([A-Z][A-Z][0-9][0-9][0-9])$', self.path)

                s = station_handler(int(m.group(1)), m.group(2))

                self.send_response(200)
                self.send_header('Content-type','text/xml')
                self.end_headers()
                self.wfile.write(s)

            elif re.match(r'/events/([0-9]+)/([A-Z][A-Z][0-9][0-9][0-9])/(\w+)$', self.path):

                m = re.match(r'/events/([0-9]+)/([A-Z][A-Z][0-9][0-9][0-9])/(\w+)$', self.path)

                s = polarization_handler(int(m.group(1)), m.group(2), m.group(3))

                self.send_response(200)
                self.send_header('Content-type','text/xml')
                self.end_headers()
                self.wfile.write(s)

            else:
                # serve files, and directory listings by following self.path from
                # current working directory
                SimpleHTTPServer.SimpleHTTPRequestHandler.do_GET(self)

        except Exception as e:

            s = "<html><head><title>Error</title></head><body><h1>Error</h1>"+str(e)+"</body></html>"

            self.send_response(200)
            self.send_header('Content-type','text/html')
            self.end_headers()
            self.wfile.write(s)

httpd = SocketServer.ThreadingTCPServer((options.hostname, options.port),CustomHandler)

print "serving at port", options.port
httpd.serve_forever()


"""
Serve CR events dynamically from database.
"""

import re
import cPickle as pickle
import sqlite3
import SocketServer
import SimpleHTTPServer

from datetime import datetime
from optparse import OptionParser
from xml.etree.cElementTree import ElementTree, Element, SubElement, ProcessingInstruction
from StringIO import StringIO

# Parse commandline options
parser = OptionParser()
parser.add_option("-p", "--port", default = 8000, type = "int", help="port to bind to")
parser.add_option("-n", "--hostname", default = "localhost", help = "hostname or IP of server")
parser.add_option("-d", "--database", default = "cr.db", help = "filename of database")

(options, args) = parser.parse_args()

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
    cursor.execute("SELECT timestamp, status FROM events WHERE eventID=?", (eventID, ))

    e = cursor.fetchone()
    SubElement(header, "id").text = str(eventID)
    SubElement(header, "timestamp").text = str(datetime.utcfromtimestamp(e[0]))
    SubElement(header, "status").text = str(e[1])
    
    if station:
        s = SubElement(header, "station")
        SubElement(s, "name").text = str(station)

        cursor.execute("""SELECT status FROM
        event_datafile AS ed INNER JOIN datafile_station AS ds ON (ed.datafileID=ds.datafileID)
        INNER JOIN stations AS s ON (ds.stationID=s.stationID)
        WHERE (eventID=? AND stationName=?)
        """, (eventID, station))

        e = cursor.fetchone()

        SubElement(s, "status").text = e[0]

    if polarization:
        p = SubElement(header, "polarization")
        SubElement(p, "name").text = str(polarization)

        cursor.execute("""SELECT p.status FROM 
        event_datafile AS ed INNER JOIN datafile_station AS ds ON (ed.datafileID=ds.datafileID)
        INNER JOIN stations AS s ON (ds.stationID=s.stationID)
        INNER JOIN station_polarization AS sp ON (ds.stationID=sp.stationID)
        INNER JOIN polarizations AS p ON (sp.polarizationID=p.polarizationID)
        WHERE (ed.eventID=? AND s.stationName=? AND p.direction=?)""", (eventID, station, polarization))

        e = cursor.fetchone()

        SubElement(p, "status").text = e[0]

    # Get all stations in event
    cursor.execute("""SELECT stationname, status FROM
    event_datafile AS ed INNER JOIN datafile_station AS ds ON (ed.datafileID=ds.datafileID)
    INNER JOIN stations AS s ON (ds.stationID=s.stationID)
    WHERE eventID=?
    """, (eventID, ))

    stations = SubElement(header, "stations")
    for e in cursor.fetchall():
        s = SubElement(stations, "station")
        SubElement(s, "name").text = e[0]
        if e[0] == station:
            s.set("current", "true")
        SubElement(s, "status").text = e[1]

        # Get all polarizations for this station
        cursor.execute("""SELECT p.direction, p.status FROM 
        event_datafile AS ed INNER JOIN datafile_station AS ds ON (ed.datafileID=ds.datafileID)
        INNER JOIN stations AS s ON (ds.stationID=s.stationID)
        INNER JOIN station_polarization AS sp ON (ds.stationID=sp.stationID)
        INNER JOIN polarizations AS p ON (sp.polarizationID=p.polarizationID)
        WHERE (ed.eventID=? AND s.stationName=?)""", (eventID, e[0]))

        polarizations = SubElement(s, "polarizations")
        for k in cursor.fetchall():
            p = SubElement(polarizations, "polarization")
            SubElement(p, "name").text = k[0]
            SubElement(p, "status").text = k[1]

    # Get all datafiles in event
    cursor.execute("""SELECT filename FROM
    event_datafile AS ed INNER JOIN datafiles AS df ON (ed.datafileID=df.datafileID)
    WHERE ed.eventID=?""", (eventID, ))

    datafiles = SubElement(header, "datafiles")
    for e in cursor.fetchall():
        d = SubElement(datafiles, "datafile")
        SubElement(d, "name").text = e[0]

    return header

def events_handler():
    """Handle summary of events.
    """

    # Connect to database
    conn = sqlite3.connect(options.database, timeout=60.0)

    # Create cursor
    c = conn.cursor()

    # Fetch all event IDs
    c.execute("SELECT eventID, timestamp, status FROM events")

    # Generate empty XML
    elements = Element("elements")

    for e in c.fetchall():

        event = SubElement(elements, "event")

        SubElement(event, "id").text = str(e[0])
        SubElement(event, "timestamp").text = str(datetime.utcfromtimestamp(e[1]))
        SubElement(event, "status").text = str(e[2])

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
    conn = sqlite3.connect(options.database)

    # Create cursor
    c = conn.cursor()

    # Generate empty XML
    elements = Element("elements")

    # Add event header
    elements.append(event_header(c, eventID))

    # Fetch event parameter keys 
    c.execute("PRAGMA table_info(eventparameters)")

    keys = [str(e[1]) for e in c.fetchall()[1:]]

    # Fetch event parameter values
    c.execute("SELECT * FROM eventparameters WHERE eventID=?", (eventID, ))

    values = [unpickle_parameter(e) for e in c.fetchone()[1:]]

    parameters = SubElement(elements, "parameters")
    figures = SubElement(elements, "figures")
    for e in zip(keys, values):

        parameter = SubElement(parameters, "parameter")

        if e[0] == "plotfiles":
            for p in e[1]:
                figure = SubElement(figures, "figure")
                SubElement(figure, "path").text = str(p)
        else:
            SubElement(parameter, "key").text = e[0]
            SubElement(parameter, "value").text = str(e[1])

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
    conn = sqlite3.connect(options.database)

    # Create cursor
    c = conn.cursor()

    # Generate empty XML
    elements = Element("elements")

    # Add event header
    header = event_header(c, eventID, station=station_name)
    elements.append(header)

    # Get all polarizations for this station
    c.execute("""SELECT p.direction, p.status FROM 
    event_datafile AS ed INNER JOIN datafile_station AS ds ON (ed.datafileID=ds.datafileID)
    INNER JOIN stations AS s ON (ds.stationID=s.stationID)
    INNER JOIN station_polarization AS sp ON (ds.stationID=sp.stationID)
    INNER JOIN polarizations AS p ON (sp.polarizationID=p.polarizationID)
    WHERE (ed.eventID=? AND s.stationName=?)""", (eventID, station_name))

    polarizations = SubElement(header, "polarizations")
    for e in c.fetchall():
        s = SubElement(polarizations, "polarization")
        SubElement(s, "name").text = e[0]
        SubElement(s, "status").text = e[1]

    # Fetch all station parameters
    c.execute("PRAGMA table_info(stationparameters)")

    keys = [str(e[1]) for e in c.fetchall()[1:]]

    # Fetch all station parameter values
    c.execute("""
    SELECT sp.* FROM 
    event_datafile AS ed INNER JOIN datafile_station AS ds ON (ed.datafileID=ds.datafileID)
    INNER JOIN stations AS s ON (ds.stationID=s.stationID)
    INNER JOIN stationparameters AS sp ON (s.stationID=sp.stationID)
    WHERE (ed.eventID=? AND s.stationname=?)
    """, (eventID, station_name))

    values = [unpickle_parameter(e) for e in c.fetchone()[1:]]

    parameters = SubElement(elements, "parameters")
    figures = SubElement(elements, "figures")
    for e in zip(keys, values):

        parameter = SubElement(parameters, "parameter")

        if e[0] == "plotfiles":
            for p in e[1]:
                figure = SubElement(figures, "figure")
                SubElement(figure, "path").text = str(p)
        else:
            SubElement(parameter, "key").text = e[0]
            SubElement(parameter, "value").text = str(e[1])

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
    conn = sqlite3.connect(options.database)

    # Create cursor
    c = conn.cursor()

    # Generate empty XML
    elements = Element("elements")

    # Add event header
    header = event_header(c, eventID, station=station_name, polarization=polarization_direction)
    elements.append(header)

    # Get all polarizations for this station
    c.execute("""SELECT direction FROM 
    event_datafile AS ed INNER JOIN datafile_station AS ds ON (ed.datafileID=ds.datafileID)
    INNER JOIN stations AS s ON (ds.stationID=s.stationID)
    INNER JOIN station_polarization AS sp ON (ds.stationID=sp.stationID)
    INNER JOIN polarizations AS p ON (sp.polarizationID=p.polarizationID)
    WHERE (ed.eventID=? AND s.stationName=?)""", (eventID, station_name))

    polarizations = SubElement(header, "polarizations")
    for e in c.fetchall():
        s = SubElement(polarizations, "polarization")
        SubElement(s, "name").text = e[0]
        if e[0] == polarization_direction:
            s.set("current", "true")

    # Fetch polarization parameter keys 
    c.execute("PRAGMA table_info(polarizationparameters)")

    keys = [str(e[1]) for e in c.fetchall()[1:]]

    # Fetch polarization parameter values
    c.execute("""
    SELECT pp.* FROM 
    event_datafile AS ed INNER JOIN datafile_station AS ds ON (ed.datafileID=ds.datafileID)
    INNER JOIN stations AS s ON (ds.stationID=s.stationID)
    INNER JOIN station_polarization AS sp ON (ds.stationID=sp.stationID)
    INNER JOIN polarizations AS p ON (sp.polarizationID=p.polarizationID)
    INNER JOIN polarizationparameters AS pp ON (p.polarizationID=pp.polarizationID)
    WHERE (ed.eventID=? AND s.stationname=? AND p.direction=?)
    """, (eventID, station_name, polarization_direction))

    values = [unpickle_parameter(e) for e in c.fetchone()[1:]]

    parameters = SubElement(elements, "parameters")
    figures = SubElement(elements, "figures")
    print keys, values
    for e in zip(keys, values):

        parameter = SubElement(parameters, "parameter")

        if e[0] == "plotfiles":
            for p in e[1]:
                figure = SubElement(figures, "figure")
                SubElement(figure, "path").text = str(p)
        else:
            SubElement(parameter, "key").text = e[0]
            SubElement(parameter, "value").text = str(e[1])

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


import matplotlib
matplotlib.use("Agg")

import glob
import datetime
import pytmf
import pycrtools as cr
import numpy as np
import matplotlib.pyplot as plt

from optparse import OptionParser

parser = OptionParser()
parser.add_option("-a", "--antenna", help="Antenna number.", default=0)
parser.add_option("-s", "--station", help="Station name (CS002, ...).", default="CS002")
parser.add_option("-d", "--directory", help="Directory containing data.", default="/Volumes/Data/VHECR/LORAtriggered/data/")
parser.add_option("-n", "--number", help="Block number", default=0)
parser.add_option("-b", "--blocksize", help="Block size", default=2**16)
parser.add_option("-f", "--filename", help="Filename for output", default="rms.txt")

(options, args) = parser.parse_args()

filenames = glob.iglob(options.directory+"*"+options.station+"*R000_tbb.h5")

timeseries_data = cr.hArray(float, options.blocksize, fill=0)

# Longitude of LOFAR
L = pytmf.deg2rad(6.869837540)

with open(options.filename, "w") as output:

    for filename in filenames:
    
        print filename
    
        try:
            f = cr.open(filename, options.blocksize)
    
            f.setAntennaSelection([options.antenna])
    
            f.getTimeseriesData(timeseries_data, options.number)
    
            # Get timestamp
            t = datetime.datetime.utcfromtimestamp(f["TIME"][0])
    
            # Calculate JD(UT1)
            ut = pytmf.gregorian2jd(t.year, t.month, float(t.day) + ((float(t.hour) + float(t.minute) / 60. + float(t.second) / 3600.) / 24.))
    
            # Calculate JD(TT)
            dtt = pytmf.delta_tt_utc(pytmf.date2jd(t.year, t.month, float(t.day) + ((float(t.hour) + float(t.minute) / 60. + float(t.second) / 3600.) / 24.)))
            tt = pytmf.gregorian2jd(t.year, t.month, float(t.day) + ((float(t.hour) + float(t.minute) / 60. + (float(t.second) + dtt / 3600.)) / 24.));
    
            # Calculate Local Apparant Sidereal Time
            theta_L = pytmf.rad2circle(pytmf.last(ut, tt, L));
    
            output.write("{0} {1:.6f} {2:.6f}\n".format(f["TIME"][0], theta_L, timeseries_data.rms().val()))
        except:
            pass


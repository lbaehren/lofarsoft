#! /usr/bin/env python

from __future__ import with_statement
from __future__ import division

"""

Doc string

"""

__version__ = '0.2'

import sys
import os
import warnings
import logging
import numpy
from datetime import datetime
from argparse import ArgumentParser
#import imgstats
#import matplotlib as mpl
#from matplotlib.backends.backend_agg import FigureCanvasAgg
from pyrap import tables as pt


SPEED_OF_LIGHT = 299792458.0
MJD1970_SECONDS = 3506716800.0  # seconds between MJD 0 and Unix time 0 (1970-1-1)
TRACE = 5
XINFO = 15
SEVERE = 35



def deg2sex(deg, seconds=True, asstring=False, precision=1, sign=True,
            factor=1, stringsep=":"):
    # This rather lengthly routine was copied from a library with various
    # other utility functions. Including the whole library felt a bit
    # like overkill though
    """Return sexasegimal notation for decimal degrees, or an equivalent tuple"""

    if isinstance(deg, (list, tuple, set)):
        return type(coord)([
            deg2sex(d, seconds=seconds, asstring=asstring,
                    precision=precision, sign=sign, factor=factor,
                    stringsep=stringsep) for d in deg])
    _sign = -1 if deg < 0 else 1
    signformat = "-" if _sign == -1 else ("+" if sign else "")
    absdeg = abs(deg/factor)
    h, m = divmod(absdeg, 1)
    m, s = divmod(m*60, 1)
    s *= 60
    h = int(h)
    ndigits = 3+precision
    if precision == 0:
        ndigits -= 1
    if len(stringsep) == 1:
        stringseph, stringsepm = stringsep, stringsep
    elif len(stringsep) == 2:
        stringseph, stringsepm = stringsep[0], stringsep[1]
    if not seconds:
        m += s/60.
        if _sign < 0:
            formatstring = "-%%02d%s%%0%d.%df" % (
                stringseph, ndigits, precision)
        else:
            if sign:
                formatstring = "%%+03d%s%%0%d.%df" % (
                    stringseph, ndigits, precision)
            else:
                formatstring = "%%02d%s%%0%d.%df" % (
                    stringseph, ndigits, precision)
        sex = formatstring % (abs(h), abs(m))
        if asstring:
            return sex
        else:
            if _sign < 0:
                h = -h
            return (h, m)
    else:
        m = int(m)
        if _sign < 0:
            formatstring = "-%%02d%s%%02d%s%%0%d.%df" % (
                stringseph, stringsepm, ndigits, precision)
        else:
            if sign:
                formatstring = "%%+03d%s%%02d%s%%0%d.%df" % (
                stringseph, stringsepm, ndigits, precision)
            else:
                formatstring = "%%02d%s%%02d%s%%0%d.%df" % (
                    stringseph, stringsepm, ndigits, precision)
        sex = formatstring % (abs(h), abs(m), abs(s))
        if asstring:
            return sex
        else:
            if _sign < 0:
                h = -h
            return (h, m, s)



def setup_logger(conffile=None, verboseness=3):
    # some extra debug levels
    levels = [logging.CRITICAL, logging.ERROR, SEVERE,
              logging.WARNING, logging.INFO, XINFO, logging.DEBUG, TRACE]    
    logprop_filename = "msinfo.log_prop"
    if conffile:
        logging.config.fileConfig(conffile)
    elif os.path.exists(logprop_filename):
        logging.config.fileConfig(logprop_filename)
    elif os.path.exists(os.path.join(
            os.environ['HOME'], logprop_filename)):
        logging.config.fileConfig(os.path.join(
                os.environ['HOME'], logprop_filename))
    else:
        logger = logging.getLogger("msinfo")
        handler = logging.StreamHandler()
        formatter = logging.Formatter("%(message)s")
        handler.setFormatter(formatter)
        logger.addHandler(handler)
        logger.setLevel(levels[verboseness+1])
    

class Info(object):

    def __init__(self, **kwargs):
        self.parse_options(nargs=[])
        setup_logger(verboseness=self.verboseness)
        self.logger = logging.getLogger("msinfo")

    def run(self):
        for self.filename in self.filenames:
            self.incorrect = False
            try:
                if (os.path.exists(
                        os.path.join(self.filename, "table.f0data")) and
                    os.path.exists(
                        os.path.join(self.filename, "table.f0meta"))):
                    self.logger.warn("  WARNING: "
                        "this looks like a Measurement Set that "
                        "is not properly formatted\n   (eg, straight from "
                        "the correlator).\n   Output may be somewhat "
                        "incorrect, in particular the end time (and thus "
                        "the duration).\n")
                    self.incorrect = True
                self.ms = pt.table(self.filename, ack=False)
                if (self.get_summary() != 0):
                    return 0
                self.print_summary()
                self.print_antennas()
                self.print_details()
            except IOError, msg:
                self.logger.warn(str(msg))
            else:
                pass
                self.ms.close()
        return 0

    def parse_options(self, args=None, nargs=[], usage_message=''):
        parser = ArgumentParser(description="Show information about one or "
                                "more measurenent sets", prog="msinfo",
                                version="%(prog)s " + __version__)
        parser.add_argument("filename", nargs='+')
        parser.add_argument("-V", "--verbose", action='store_true', 
                            default=False, help="Show more output")
        parser.add_argument("-f", "--force", action='store_true',
                            default=False, help="Force running on large files")
        args = parser.parse_args()
        self.args = args
        self.filenames = args.filename
        self.verboseness = 4 if args.verbose else 3

    def get_antennas(self):
        antennas = {}
        ant_table = pt.table(os.path.join(self.filename, "ANTENNA"), ack=False)
        antennas['position'] = ant_table.getcol("POSITION")
        antennas['name'] = ant_table.getcol("NAME")
        antennas['count'] = len(antennas['name'])
        return antennas
    
    def get_frequencies(self):
        table = pt.table(os.path.join(self.filename, 'SPECTRAL_WINDOW'), ack=False)
        frequencies = table.getcol('CHAN_FREQ')[0]
        nchannels = len(frequencies)
        channel_width = table.getcol('CHAN_WIDTH')[0][0]
        return {'frequencies': frequencies, 'nchannels': nchannels,
                'width': channel_width}

    def get_summary(self):
        self.number_vis = self.ms.nrows()
        if self.number_vis > 500000 and not self.args.force:
            self.logger.warn("  WARNING: "
                            "large data file. Processing might take a while.\n"
                             "  Use -f/--force if you really want to run "
                             "msinfo on this file\n")
            return 1

        self.number_flaggedvis = self.ms.query('any(FLAG)').nrows()
        #self.number_vis  = len(self.number_vis)
        #self.number_flaggedvis = int(len(numpy.where(self.ms.getcol('FLAG')==True)[0])/factor)
        self.integration_time = self.ms.getcol('INTERVAL')[0]
        subtables = self.ms.keywordnames()
        for subtable in ('POLARIZATION', 'OBSERVATION', 'FIELD', 
                         'SPECTRAL_WINDOW'):
            if subtable not in subtables:
                self.logger.warn("Subtable %s missing from MS" % subtable)
                sys.exit()
        frequencies = self.get_frequencies()
        antennas = self.get_antennas()
        polarization = {'count': pt.table(
                os.path.join(self.filename, 'POLARIZATION'), ack=False).getcol(
                'NUM_CORR')[0]}
        #times = self.ms.getcol('TIME')
        #times = (times[0]-MJD1970_SECONDS, times[-1]-MJD1970_SECONDS)
        #times = map(datetime.fromtimestamp, times)
        # Don't use this one: is wrong on MS originating straight from the correlator
        times = pt.table(
            os.path.join(self.filename, 'OBSERVATION'), ack=False).getcol(
            'TIME_RANGE')[0]
        if self.incorrect:
            times[1] = self.ms.getcol('TIME')[-1]
        times -= MJD1970_SECONDS
        duration = (times[-1] - times[0] + self.integration_time)/3600.
        times = (datetime.fromtimestamp(times[0]), datetime.fromtimestamp(times[1]))
        times = {'time': times, 'duration': duration}
        fieldnames = {'fieldnames': pt.table(
                os.path.join(self.filename, 'FIELD'), ack=False).getcol('NAME')}
        phasedir = pt.table(
            os.path.join(self.filename, 'FIELD'), ack=False).getcol('PHASE_DIR')
        phasedir = phasedir[0][0]/numpy.pi*180
        if phasedir[0] < 0:
            phasedir[0] += 360.0
        phasedir = (deg2sex(phasedir[0], asstring=True, factor=15),
                    deg2sex(phasedir[1], asstring=True))

        phases = {'direction': phasedir}
        self.summary = {
            'frequencies': frequencies,
            'polarization': polarization,
            'fieldnames': fieldnames,
            'times': times,
            'phases': phases,
            'antennas': antennas
            }
        return 0

    def get_antenntas(self):
        self.summar['antennas'] = {}
        self.summar['antennas']['position'] = pt.table(
            os.path.join(self.filename, "ANTENNA"), ack=False).getcol("POSITION")
        self.summary['antennas']['name'] = pt.table(
            os.path.join(self.filename, "ANTENNA"), ack=False).getcol("NAME")
        self.summary['antennas']['count'] = len(self.antennas['name'])

    
    def print_summary(self):
        self.logger.info("""
        
    \tSummary of UV data for %s
    
      # of visibilities:       %d
      # of flagged vis.:       %d
      Phase center:            %s, %s
      Frequency range (MHz):   %.3f  --  %.3f
      Wavelength range (m):    %.3f  --  %.3f
      Observation time span:   %s  --  %s
      Duration (hrs):          %.2f
      time bin / integration:  %.5f
      # of channels:           %d
      channel width (KHz):     %.1f
      # of polarizations:      %d
      # of antennas:           %d
    """ % (self.filename, self.number_vis, self.number_flaggedvis,
           self.summary['phases']['direction'][0], 
           self.summary['phases']['direction'][1],
           min(self.summary['frequencies']['frequencies'] -
               self.summary['frequencies']['width']/2.)/1e6,
           (max(self.summary['frequencies']['frequencies']) +
            self.summary['frequencies']['width']/2.)/1e6,
           SPEED_OF_LIGHT/min(self.summary['frequencies']['frequencies']),
           SPEED_OF_LIGHT/(max(self.summary['frequencies']['frequencies'])+
                           self.summary['frequencies']['width']),
           self.summary['times']['time'][0].strftime("%Y-%m-%dT%H:%M:%S"), 
           self.summary['times']['time'][1].strftime("%Y-%m-%dT%H:%M:%S"), 
           self.summary['times']['duration'],
           self.integration_time,
           self.summary['frequencies']['nchannels'],
           self.summary['frequencies']['width']/1e3,
           self.summary['polarization']['count'],
           self.summary['antennas']['count']))

    def print_antennas(self):
        # CENTER = (3.82682E6, 4.60987E5, 5.06472E6)
        antennas = self.summary['antennas']
        r = numpy.sqrt(antennas['position'][:,0]**2 + 
                       antennas['position'][:,1]**2 + 
                       antennas['position'][:,2]**2)
        latitude = 180*numpy.arctan2(antennas['position'][:,1], 
                                     antennas['position'][:,0])/numpy.pi
        longitude = 90 - 180*numpy.arccos(antennas['position'][:,2]/r)/numpy.pi
        self.logger.log(XINFO, "Antennas & their GPS positions (longitude, latitude):")
        for name, rr, pphi, ttheta in zip(antennas['name'], r, 
                                          latitude, longitude):
            self.logger.log(XINFO, "  %s:  %10.6f  %10.6f" % (name, pphi, ttheta))
        self.logger.log(XINFO, "")

    def print_details(self):
        colnames = self.ms.colnames()
        self.logger.log(XINFO, "Available data columns:")
        for name in ['DATA', 'CORRECTED_DATA', 'MODEL_DATA']:
            if name in colnames:
                self.logger.log(XINFO, "  %s" % name)
        # Need to fix this: includes auto-correlations
        # Also, only print out the number of *unflagged* visibilities
        # (or both: flagged & unflagged visibilities)
        #self.logger.log(XINFO, "There are %d visibilities across %d baselines" % (
        #        len(self.ms.getcol('DATA')), baselines_table.nrows()))
    

def main():
    info = Info()
    info.run()


if __name__ == '__main__':
    sys.exit(main())

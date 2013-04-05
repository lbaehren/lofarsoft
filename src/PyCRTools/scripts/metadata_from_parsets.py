#! /usr/bin/env python

import re
import os
import subprocess
import datetime

nyquist_zone_map = {'LBA_10-80' : 1, 'LBA_30-80' : 1, 'HBA_110_190' : 2, 'HBA_170-230' : 3, 'HBA_210-250' : 3}

def get_obsid(filename):

    m = re.search('(L[0-9]+)_D', filename)

    assert m is not None

    return m.group(1)

def parse_parset(obsid):

    f = open(os.path.join(os.environ['LOFAR_PARSET_PATH'], obsid + '.parset'), 'r')

    parset = {}
    for line in f:

        # Observation.startTime
        m = re.search('Observation\.startTime = \'([0-9]{4})-([0-9]{2})-([0-9]{2}) ([0-9]{2}):([0-9]{2}):([0-9]{2})\'', line)
        if m is not None:
            parset['OBSERVATION_START_UTC'] = datetime.datetime(int(m.group(1)), int(m.group(2)), int(m.group(3)), int(m.group(4)), int(m.group(5)), int(m.group(6)))
        
        # Observation.stopTime
        m = re.search('Observation\.stopTime = \'([0-9]{4})-([0-9]{2})-([0-9]{2}) ([0-9]{2}):([0-9]{2}):([0-9]{2})\'', line)
        if m is not None:
            parset['OBSERVATION_END_UTC'] = datetime.datetime(int(m.group(1)), int(m.group(2)), int(m.group(3)), int(m.group(4)), int(m.group(5)), int(m.group(6)))

        # Observation.antennaSet
        m = re.search('Observation.antennaSet = ([A-Z_]+)', line)

        if m is not None:
            parset['ANTENNA_SET'] = m.group(1)

        # Observation.bandFilter
        m = re.search('Observation.bandFilter = ([A-Z0-9_]+)', line)

        if m is not None:
            parset['FILTER_SELECTION'] = m.group(1)
            parset['NYQUIST_ZONE'] = nyquist_zone_map[m.group(1)]

        # Observation.clockMode
        m = re.search('Observation.clockMode = <<Clock([0-9]{3})', line)

        if m is not None:
            parset['CLOCK_FREQUENCY'] = int(m.group(1))
            parset['CLOCK_FREQUENCY_UNIT'] = 'MHz'

    return parset
        
def write_metadata(filename, attributes={}):

    subprocess.call([os.path.join(os.environ['LOFARSOFT'], 'release/bin/tbbmd'), ['--{0} {1}'.format(a[0], a[1]) for a in attributes.items()]])

def timestamp_in_observation(filename, parset):

    m = re.search('D([0-9]{4})([0-9]{2})([0-9]{2})T([0-9]{2})([0-9]{2})([0-9]{2})\.([0-9]{3})', filename)

    assert m is not None

    timestamp = datetime.datetime(int(m.group(1)), int(m.group(2)), int(m.group(3)), int(m.group(4)), int(m.group(5)), int(m.group(6)), int(m.group(7)) * 1000)

    return (timestamp >= parset['OBSERVATION_START_UTC'] and timestamp <= parset['OBSERVATION_END_UTC'])

if __name__ == '__main__':

    from optparse import OptionParser
    
    parser = OptionParser()
    parser.add_option("-t", "--test", action="store_false", dest="apply", default=True, help="don't write anything to files, just print")
    
    (options, args) = parser.parse_args()

    for a in args:

        filename = a.split('/')[-1]

        parset = parse_parset(get_obsid(filename))

        if not timestamp_in_observation(filename, parset):
            print filename, parset
    

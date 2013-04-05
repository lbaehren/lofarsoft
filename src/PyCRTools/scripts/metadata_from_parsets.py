#! /usr/bin/env python

import re
import os
import subprocess
import datetime

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

        print filename, timestamp_in_observation(filename, parset)
    

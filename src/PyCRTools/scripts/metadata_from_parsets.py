#! /usr/bin/env python

import re
import os
import subprocess
import datetime

nyquist_zone_map = {'LBA_10_80' : 1, 'LBA_30_90' : 1, 'HBA_110_190' : 2, 'HBA_170_230' : 3, 'HBA_210_250' : 3}

def get_obstimes(filename):

    for files in os.listdir(os.environ['LOFAR_PARSET_PATH']):
        obstimes = {}
        if files.endswith(".parset"):
            f = open(os.path.join(os.environ['LOFAR_PARSET_PATH'], files), 'r')

            observation_time = {}
            for line in f:
                # Observation.startTime
                m = re.search('Observation\.startTime = \'([0-9]{4})-([0-9]{2})-([0-9]{2}) ([0-9]{2}):([0-9]{2}):([0-9]{2})\'', line)
                if m is not None:
                    observation_time['startUTC'] = datetime.datetime(int(m.group(1)), int(m.group(2)), int(m.group(3)), int(m.group(4)), int(m.group(5)), int(m.group(6)))
                
                # Observation.stopTime
                m = re.search('Observation\.stopTime = \'([0-9]{4})-([0-9]{2})-([0-9]{2}) ([0-9]{2}):([0-9]{2}):([0-9]{2})\'', line)
                if m is not None:
                    observation_time['endUTC'] = datetime.datetime(int(m.group(1)), int(m.group(2)), int(m.group(3)), int(m.group(4)), int(m.group(5)), int(m.group(6)))

                if len(observation_time) == 2:
                    # No need to read rest of the file
                    break

        obstimes[files] = observation_time

    return obstimes

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
            parset['startUTC'] = "{0}-{1}-{2}T{3}:{4}:{5}Z".format(m.group(1), m.group(2), m.group(3), m.group(4), m.group(5), m.group(6))
        
        # Observation.stopTime
        m = re.search('Observation\.stopTime = \'([0-9]{4})-([0-9]{2})-([0-9]{2}) ([0-9]{2}):([0-9]{2}):([0-9]{2})\'', line)
        if m is not None:
            parset['endUTC'] = "{0}-{1}-{2}T{3}:{4}:{5}Z".format(m.group(1), m.group(2), m.group(3), m.group(4), m.group(5), m.group(6))

        # Observation.antennaSet
        m = re.search('Observation.antennaSet = ([A-Z_]+)', line)

        if m is not None:
            parset['antennaSet'] = m.group(1)

        # Observation.bandFilter
        m = re.search('Observation.bandFilter = ([A-Z0-9_]+)', line)

        if m is not None:
            parset['filterSelection'] = m.group(1)

        # Observation.clockMode
        m = re.search('Observation.clockMode = <<Clock([0-9]{3})', line)

        if m is not None:
            parset['clockFrequency'] = int(m.group(1))
            parset['clockFrequencyUnit'] = 'MHz'

        parset['antennaPositionDir'] = os.path.join(os.environ['LOFARSOFT'], 'data/lofar/antennapositions/')
        parset['dipoleCalibrationDelayDir'] = os.path.join(os.environ['LOFARSOFT'], 'dipole_calibration_delay/')

    return parset
        
def write_metadata(filename, parset):

    subprocess.call([os.path.join(os.environ['LOFARSOFT'], 'release/bin/tbbmd'), ['--{0} {1}'.format(a[0], a[1]) for a in parset.items()]])

def timestamp_in_observation(filename, parset):

    m = re.search('D([0-9]{4})([0-9]{2})([0-9]{2})T([0-9]{2})([0-9]{2})([0-9]{2})\.([0-9]{3})', filename)

    assert m is not None

    timestamp = datetime.datetime(int(m.group(1)), int(m.group(2)), int(m.group(3)), int(m.group(4)), int(m.group(5)), int(m.group(6)), int(m.group(7)) * 1000)

    # Parse ISO timestamp
    temp = parset['startUTC']
    startUTC = datetime.datetime(int(temp[0:4]), int(temp[5:7]), int(temp[8:10]), int(temp[11:13]), int(temp[14:16]), int(temp[17:19]))
    temp = parset['endUTC']
    endUTC = datetime.datetime(int(temp[0:4]), int(temp[5:7]), int(temp[8:10]), int(temp[11:13]), int(temp[14:16]), int(temp[17:19]))

    return (timestamp >= startUTC and timestamp <= endUTC)

if __name__ == '__main__':

    from optparse import OptionParser
    
    parser = OptionParser()
    parser.add_option("-t", "--test", action="store_false", dest="apply", default=True, help="don't write anything to files, just print")
    
    (options, args) = parser.parse_args()

#    for a in args:
#
#        filename = a.split('/')[-1]
#
#        parset = parse_parset(get_obsid(filename))
#
#        if not timestamp_in_observation(filename, parset):
#            print filename, parset

    print get_obstimes()

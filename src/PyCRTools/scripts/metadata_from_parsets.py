#! /usr/bin/env python

import re
import os
import shutil
import subprocess
import datetime

nyquist_zone_map = {'LBA_10_80' : 1, 'LBA_30_90' : 1, 'HBA_110_190' : 2, 'HBA_170_230' : 3, 'HBA_210_250' : 3}

def get_obstimes():

    obstimes = {}

    for files in os.listdir(os.environ['LOFAR_PARSET_PATH']):
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

            if len(observation_time) != 2:
                print "error parsing parset", files
            else:
                obstimes[files] = observation_time

    return obstimes

def parset_from_filename(filename, obstimes):

    m = re.search('D([0-9]{4})([0-9]{2})([0-9]{2})T([0-9]{2})([0-9]{2})([0-9]{2})\.([0-9]{3})', filename)

    assert m is not None

    timestamp = datetime.datetime(int(m.group(1)), int(m.group(2)), int(m.group(3)), int(m.group(4)), int(m.group(5)), int(m.group(6)), int(m.group(7)) * 1000)

    for obst in sorted(obstimes.items()):
        
        if timestamp >= obst[1]['startUTC'] and timestamp <= obst[1]['endUTC']:

            return obst[0]

def get_obsid(filename):

    m = re.search('(L[0-9]+)_D', filename)

    assert m is not None

    return m.group(1)

def parse_parset(parset):

    f = open(os.path.join(os.environ['LOFAR_PARSET_PATH'], parset), 'r')

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

    command = os.path.join(os.environ['LOFARSOFT'], 'release/bin/tbbmd')

    for a in parset.items():
        command += " --{0} {1}".format(a[0], a[1])

    command += " {0}".format(filename)

    print command

    os.chmod(filename, 0777)

    subprocess.call(command, shell=True)

    os.chmod(filename, 0220)

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

def fixfile(filename, parset, path):

    if parset is None:
        print "Error cannot find parset for file", filename
        return
    
    new_filename = re.sub("L[0-9]+", parset.rstrip(".parset"), filename)

    shutil.copy(os.path.join(path, filename), os.path.join(path, 'backup', new_filename))

#    shutil.move(os.path.join(path, filename), os.path.join(path, new_filename))

    print filename, new_filename,
    write_metadata(os.path.join(path, 'backup', new_filename), parse_parset(parset))

if __name__ == '__main__':

    from optparse import OptionParser
    
    parser = OptionParser()
    parser.add_option("-t", "--test", action="store_false", dest="apply", default=True, help="don't write anything to files, just print")
    
    (options, args) = parser.parse_args()

    obstimes = get_obstimes()

    path = args[0]

    for filename in os.listdir(path):

        if filename.endswith('.h5'):

            parset = parset_from_filename(filename, obstimes)

            m = re.search('(L[0-9]+)', filename)

            if m is None:
                print "error, cannot find parset for", filename
                continue

            if m.group(1) + '.parset' != parset:

                fixfile(filename, parset, path)


import pycrtools as cr
import datetime
import subprocess
import sys

#filelist = listFiles("/Users/acorstanje/triggering/CR/caltestevent/*.h5")
#filelist = listFiles("/Users/acorstanje/triggering/CR/*.986Z*.h5")
if len(sys.argv) > 2:
    filefilter = sys.argv[1]
    caltable_dir = sys.argv[2]
    if caltable_dir[-1] != '/':
        caltable_dir += '/'
else:
    print 'Usage: python updateDipoleCalibration.py <filefilter> <caltable_dir>'
    exit()

filelist = cr.listFiles(filefilter)

print 'File list contains %d files' % len(filelist)
print 'First few names: '
if len(filelist) > 5:
    print filelist[0:5]
else:
    print filelist

caltable_date = datetime.datetime(2012, 6, 8)

for name in filelist:
    # get date
    datestring = name[1+name.rfind('D'):name.rfind('T')]

    year = int(datestring[0:4])
    month = int(datestring[4:6])
    day = int(datestring[6:8])

    if year < 2006 or year > 2020 or month > 12 or day > 31:
        print 'ERROR parsing file date!'
        continue

    thisDate = datetime.datetime(year, month, day)

    if thisDate >= caltable_date:
    # update caltable using tbbmd
#        print 'Doing file: %s' % name
        proc=subprocess.Popen(["tbbmd " + name + " --dipoleCalibrationDelayDir " + caltable_dir], shell=True)
        proc.wait()
    else:
        print 'SKIPPING file: %s' % name




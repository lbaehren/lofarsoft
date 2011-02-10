#! /usr/bin/env python
# A prototype Cosmic Ray pipeline based on PyCRtools. 
# Created by Arthur Corstanje, Jan 17, 2011. Email: a.corstanje@astro.ru.nl

import os
import sys
import time
import numpy as np
import matplotlib.pyplot as plt

import datacheck as dc
import rficlean as rf
import pulsefit as pf
import matching as match # possibly push this down to pulsefit?
# import beamformer as bf

antennaset = 'LBA_OUTER' # hack around missing info in data files

def writeDictOneLine(outfile, dict):
    for key in dict:
        if type(dict[key]) == np.float64:
            outfile.write('%s: %7.2f\n' % (str(key), dict[key]))
        else:
            outfile.write('%s: %s; ' % (str(key), ''.join(repr(dict[key]).strip('[]').split(','))))

def writeDict(outfile, dict):
    for key in dict:
#        print '%s %s %s' % (key, dict[key], type(dict[key]))
        if type(dict[key]) == dict: # if we have a dict inside our dict, print it on one line (and hope it's small)
            writeDictOneLine(outfile, dict[key])
        elif type(dict[key]) == np.float64:        
            outfile.write('%s: %7.2f\n' % (str(key), dict[key]))
        else:
            outfile.write('%s: %s\n' % (str(key), ''.join(repr(dict[key]).strip('[]').split(','))))
    outfile.write('\n')
    
def runAnalysis(files, triggers, outfilename, doPlot = False):
    """ Input: list of files to process, trigger info as read in by match.readtriggers(...), filename for results output
    """
    outfile = open(outfilename, mode='w')
    n = 0
    for file in files:
        n += 1
        print 'Processing file %d out of %d' % (n, len(files))
        result = dc.safeOpenFile(file, antennaset)
        #print result
        
        writeDict(outfile, result)
        if not result["success"]:
            continue
        crfile = result["file"]
        
#        crfile.set("blocksize", 32768)
#        import pdb; pdb.set_trace()
#        rf.cleanSpectrum(crfile)
        
        outfile.write('\n')
        # do quality check and rfi cleaning here
        result = dc.qualityCheck(crfile, doPlot = doPlot)
        flaggedList = result["flagged"]
        writeDict(outfile, result)
        if not result["success"]: 
            continue
        #print flaglist
        # find initial direction of incoming pulse, using trigger logs
        result = pf.triggerMessageFit(crfile, triggers, 'linearFit') 
        writeDict(outfile, result)
        if not result["success"]:
            continue
        triggerFitResult = result
        # now find the final direction based on all data, using initial direction as starting point
        try: # apparently it's dangerous...
          result = pf.fullDirectionFit(crfile, triggerFitResult, 4096, flaggedList = flaggedList, FarField = False, doPlot = doPlot)     
          writeDict(outfile, result)
          if not result["success"]:
              continue
          
        except (ZeroDivisionError, IndexError), msg:
          print 'EROR!'
          print msg
        bfEven = result["even"]["optBeam"]
        bfOdd = result["odd"]["optBeam"]
        
        bfEven.plot()
        raw_input("--- Plotted optimal beam for even antennas - press Enter to continue...")
        bfOdd.plot()
        raw_input("--- Plotted optimal beam for odd antennas - press Enter to continue...")
        
        outfile.flush()
    # end for
    outfile.close()


# get list of files to process
if len(sys.argv) > 2:
    datafiles = sys.argv[1] 
    triggerMessageFile = sys.argv[2]
elif len(sys.argv) > 1:
    datafiles = sys.argv[1]
    print 'Taking default trigger message file.'
    triggerMessageFile = '/Users/acorstanje/triggering/datarun_19-20okt/2010-10-19_.dat'
else:
    print 'No files given on command line, using a default set instead.'
    datafiles = '/Users/acorstanje/triggering/datarun_19-20okt/data/oneshot_level4_CS017_19okt_no-23.h5'
#    datafiles = '/Users/acorstanje/triggering/MACdatarun_2feb2011/automatic_obs_test-2feb-2-26.h5'
#    triggerMessageFile = '/Users/acorstanje/triggering/MACdatarun_2feb2011/2011-02-02_TRIGGER.dat'
    triggerMessageFile = '/Users/acorstanje/triggering/datarun_19-20okt/2010-10-19_TRIGGER_debugstripped.dat' #TRIGGER_debugstripped.dat'
    #datafiles = '/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/trigger-dumps-2010-07-07-cs006/*'
    #triggerMessageFile = '/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/2010-07-07-triggers/2010-07-07_TRIGGER-cs006.dat'

sortstring = 'sort -rn --field-separator="-" --key=18'
outfile = 'crPipelineResults.txt'
antennaset="LBA_OUTER"

fd = os.popen('ls '+ datafiles+' | ' + sortstring)
files = fd.readlines()
nofiles = len(files)
#------------------------------------------------------------------------
# read in trigger info
print "Number of files to process:", nofiles
fd.close()
print 'Reading triggers...'
triggers = match.readtriggers(triggerMessageFile)
print 'Trigger reading complete.'


runAnalysis(files, triggers, outfile, doPlot = True)
#fitergs = dict()




#       n_az = len(fitDataEven[2][0, 0])
#  n_el = len(fitDataOdd[2][1, 0])
#  az_max = fitDataEven[2][0].max()
#  az_min = fitDataEven[2][0].min()
#  el_max = fitDataEven[2][1].max()
#  el_min = fitDataEven[2][1].min()

#  pixarray = np.zeros( (n_el,n_az) ) 
#  pixarray = -1.0 * fitDataEven[3].T # of transpose?
#  plt.imshow(pixarray,cmap=plt.cm.hot,extent=(az_min, az_max, el_min, el_max) )



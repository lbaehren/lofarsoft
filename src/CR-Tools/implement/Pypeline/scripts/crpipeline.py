#! /usr/bin/env python
# A prototype Cosmic Ray pipeline based on PyCRtools. 
# Created by Arthur Corstanje, Jan 17, 2011. Email: a.corstanje@astro.ru.nl

import os
import sys
import time
import numpy as np

import datacheck as dc
import pulsefit as pf
import matching as match

antennaset = 'LBA_OUTER' # hack around missing info in data files

def writeDict(outfile, dict):
    for key in dict:
        outfile.write('%s: %s\n' % (str(key), ''.join(repr(dict[key]).strip('[]').split(','))))

def runAnalysis(files, trigs, outfilename):
    """ Input: list of files to process, trigger info as read in by match.readtriggers(...), filename for results output
    """
    outfile = open(outfilename, mode='w')
    n = 0
    for file in files:
        n += 1
        print 'Processing file %d out of %d' % (n, len(files))
        result = dc.safeOpenFile(file, antennaset)
        print result

        writeDict(outfile, result)
        if not result["success"]:
            continue
        crfile = result["file"]
        outfile.write('\n')
        # do quality check and rfi cleaning here
        
        # find direction of incoming pulse
        try:
          fitResult = pf.fullPulseFit(crfile, trigs, 1024)     
          outstring = ('bruteForce: Az:%7.2f, El:%7.2f'%fitResult['triggerFit'][0:2]+
                       ', Dist:%8.1f'%-1.+
                       ', mse:%8.3f'%fitResult['triggerFit'][2] +
                       ', nAnts:%u\n'%fitResult['triggerFit'][4])
          outfile.write(outstring)
          outstring = (' linearFit: Az:%7.2f, El:%7.2f'%fitResult['linearFit'][0:2]+
                       ', Dist:%8.1f'%-1.+
                       ', mse:%8.3f'%fitResult['linearFit'][2] +
                       ', nAnts:%u\n'%fitResult['linearFit'][4])
          outfile.write(outstring)
          evenResults = fitResult['fitEven']
          outstring = ('  Even-Fit: Az:%7.2f, El:%7.2f, Dist: xxx '%tuple(evenResults[0])+
                       ', Val:%8.0f, Iter: xxx' % evenResults[1] )#  + # 0 tot 2 voor brute, 1 to 3 for simplex
    #                   ', Flag:%u\n'%fitResult['fitEven'][4] ) 
          outfile.write(outstring)
          oddResults = fitResult['fitOdd']
          outstring = ('   Odd-Fit: Az:%7.2f, El:%7.2f, Dist: xxx '%tuple(oddResults[0])+
                       ', Val:%8.0f, Iter: xxx' % oddResults[1] ) # + 
    #                   ', Flag:%u\n'%fitResult['fitOdd'][4] )
          outfile.write(outstring)
          outfile.flush()
        except (ZeroDivisionError, IndexError):
          print 'EROR!'
    # end for
    outfile.close()



# get list of files to process
if len(sys.argv) > 2:
    datafiles = sys.argv[1] 
    triggerMessageFile = sys.argv[2]
elif len(sys.argv) > 1:
    datafiles = sys.argv[1]
    print 'Taking default trigger message file.'
    triggerMessageFile = '/Users/acorstanje/triggering/datarun_19-20okt/2010-10-19_TRIGGER.dat'
else:
    print 'No files given on command line, using a default set instead.'
    datafiles = '/Users/acorstanje/triggering/datarun_19-20okt/data/oneshot_level4_CS017_19okt_no-23*'
    triggerMessageFile = '/Users/acorstanje/triggering/datarun_19-20okt/2010-10-19_TRIGGER.dat'
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
trigs = match.readtriggers(triggerMessageFile)
print 'Trigger reading complete.'

runAnalysis(files, trigs, outfile)
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


#! /usr/bin/env python
# Ported to Pycrtools on Nov. 10, 2010

import os
#import pycr as cr
import numpy as np
import pulsefit as tp
import matching as match
import time
#------------------------------------------------------------------------
"""
(++) Parameters
---------------
"""
#-------
#datafiles = '/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/trigger-dumps-2010-07-07-cs003/*'
#datafiles = '/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/trigger-dumps-2010-07-07-cs005/*'
#datafiles = '/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/trigger-dumps-2010-07-07-cs006/*'
datafiles = '/Users/acorstanje/triggering/datarun_19-20okt/data/oneshot_level4_CS017_19okt_no-23*'

#-------
#triggerMessageFile = '/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/2010-07-07-triggers/2010-07-07_TRIGGER-cs003.dat'
#triggerMessageFile = '/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/2010-07-07-triggers/2010-07-07_TRIGGER-cs005.dat'
#triggerMessageFile = '/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/2010-07-07-triggers/2010-07-07_TRIGGER-cs006.dat'
triggerMessageFile = '/Users/acorstanje/triggering/datarun_19-20okt/2010-10-19_TRIGGER.dat'
#-------
sortstring = 'sort -rn --field-separator="-" --key=18'
outfile = 'test-triggerfits-out.txt'
antennaset="LBA_OUTER"
#------------------------------------------------------------------------

fd = os.popen('ls '+ datafiles+' | ' + sortstring)
files = fd.readlines()
nofiles = len(files)
print "Number of files to process:", nofiles
fd.close()
print 'Reading triggers...'
trigs = match.readtriggers(triggerMessageFile)
print 'Trigger reading complete.'
outfd = open(outfile, mode='w')
fitergs = dict()

for fileind in range(nofiles):
    print 'Processing file', fileind, 'out of', nofiles
    if os.path.exists(files[fileind].strip()) :
        try:
            fitergs[fileind] = tp.fullPulseFit(files[fileind].strip(),trigs,antennaset)


            outfd.write('\n------------\n')
            outfd.write(files[fileind])
            #dateString = time.strftime("%a, %d %b %Y %H:%M:%S +0000", time.gmtime(epoch))
            outstring = ('bruteForce: Az:%7.2f, El:%7.2f'%fitergs[fileind]['triggerFit'][0:2]+
                         ', Dist:%8.1f'%-1.+
                         ', mse:%8.3f'%fitergs[fileind]['triggerFit'][2] +
                         ', nAnts:%u\n'%fitergs[fileind]['triggerFit'][4])
            outfd.write(outstring)
            outstring = (' linearFit: Az:%7.2f, El:%7.2f'%fitergs[fileind]['linearFit'][0:2]+
                         ', Dist:%8.1f'%-1.+
                         ', mse:%8.3f'%fitergs[fileind]['linearFit'][2] +
                         ', nAnts:%u\n'%fitergs[fileind]['linearFit'][4])
            outfd.write(outstring)
            evenResults = fitergs[fileind]['fitEven']
            outstring = ('  Even-Fit: Az:%7.2f, El:%7.2f, Dist: xxx '%tuple(evenResults[0])+
                         ', Val:%8.0f, Iter:%4u' % (evenResults[1], len(evenResults[2])) )#  + # 0 tot 2 voor brute, 1 to 3 for simplex
#                   ', Flag:%u\n'%fitergs[fileind]['fitEven'][4] )
            outfd.write(outstring)
            oddResults = fitergs[fileind]['fitOdd']
            outstring = ('   Odd-Fit: Az:%7.2f, El:%7.2f, Dist: xxx '%tuple(oddResults[0])+
                         ', Val:%8.0f, Iter:%4u' % (oddResults[1], len(oddResults[2])) ) # +
#                   ', Flag:%u\n'%fitergs[fileind]['fitOdd'][4] )
            outfd.write(outstring)
            outfd.flush()
        except (ZeroDivisionError, IndexError):
            print 'Error!'
    else:
        print 'file \'',files[fileind].strip(),'\' does not exist!'
outfd.close()

#       n_az = len(fitDataEven[2][0, 0])
#  n_el = len(fitDataOdd[2][1, 0])
#  az_max = fitDataEven[2][0].max()
#  az_min = fitDataEven[2][0].min()
#  el_max = fitDataEven[2][1].max()
#  el_min = fitDataEven[2][1].min()

#  pixarray = np.zeros( (n_el,n_az) )
#  pixarray = -1.0 * fitDataEven[3].T # of transpose?
#  plt.imshow(pixarray,cmap=plt.cm.hot,extent=(az_min, az_max, el_min, el_max) )

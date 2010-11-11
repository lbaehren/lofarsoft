#! /usr/bin/env python
# Ported to Pycrtools on Nov. 10, 2010

import os
#import pycr as cr
import numpy as np
import triggered_pulsefitting as tp
import matching as match

#------------------------------------------------------------------------
"""
(++) Parameters
---------------
"""
#-------
#datafiles = '/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/trigger-dumps-2010-07-07-cs003/*'
#datafiles = '/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/trigger-dumps-2010-07-07-cs005/*'
#datafiles = '/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/trigger-dumps-2010-07-07-cs006/*'
datafiles = '/Users/acorstanje/triggering/datarun_19-20okt/data/oneshot_level4_CS017_19okt_no-*'

#-------
#triggerMessageFile = '/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/2010-07-07-triggers/2010-07-07_TRIGGER-cs003.dat'
#triggerMessageFile = '/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/2010-07-07-triggers/2010-07-07_TRIGGER-cs005.dat'
#triggerMessageFile = '/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/2010-07-07-triggers/2010-07-07_TRIGGER-cs006.dat'
triggerMessageFile = '/Users/acorstanje/triggering/datarun_19-20okt/2010-10-20_TRIGGER.dat'
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

"""
for fileind in range(nofiles):
  print 'Processing file', fileind, 'out of', nofiles
  if os.path.exists(files[fileind].strip()) :
    try:
      fitergs[fileind] = tp.fullPulseFit(files[fileind].strip(),trigs,antennaset)
      outfd.write('\n------------\n')
      outfd.write(files[fileind])
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
      outstring = ('  Even-Fit: Az:%7.2f, El:%7.2f, Dist:%8.1f'%tuple(fitergs[fileind]['fitEven'][0])+
                   ', Val:%8.0f, Iter:%4u'%fitergs[fileind]['fitEven'][1:3] +
                   ', Flag:%u\n'%fitergs[fileind]['fitEven'][4] ) 
      outfd.write(outstring)
      outstring = ('   Odd-Fit: Az:%7.2f, El:%7.2f, Dist:%8.1f'%tuple(fitergs[fileind]['fitOdd'][0])+
                   ', Val:%8.0f, Iter:%4u'%fitergs[fileind]['fitOdd'][1:3] + 
                   ', Flag:%u\n'%fitergs[fileind]['fitOdd'][4] )
      outfd.write(outstring)
      outfd.flush()
    except (ZeroDivisionError, IndexError):
      pass
  else:
    print 'file \'',files[fileind].strip(),'\' does not exist!'    
outfd.close()
"""
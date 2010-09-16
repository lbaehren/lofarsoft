#! /usr/bin/env python

import os
import pycr as cr
import numpy as np
import matplotlib.pyplot as plt

#------------------------------------------------------------------------
"""
(++) Parameters
---------------
"""
#-------
#datafiles = '/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/trigger-dumps-2010-07-07-cs003/*'
#datafiles = '/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/trigger-dumps-2010-07-07-cs005/*'
#datafiles = '/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/trigger-dumps-2010-07-07-cs006/*'
datafiles = '/home/driss/usg/data/lofar/trigger-2010-02-11/*h5'
#-------
sortstring = 'sort -rn --field-separator="-" --key=18'
outfile = 'test-adcstatistics-out.txt'
#------------------------------------------------------------------------

fd = os.popen('ls '+ datafiles+' | ' + sortstring)
files = fd.readlines()
nofiles = len(files)
print "Number of files to process:", nofiles
fd.close()

outfd = open(outfile, mode='w')
for fileind in range(nofiles):
  print 'Processing file', fileind, 'out of', nofiles
  filename = files[fileind].strip()
  if os.path.exists(filename) :
    try:
      dr = cr.open(filename,'LOFAR_TBB')
      #select the data before the triggered pulse
      # ***this implements a bias in the data!***
      dr.blocksize = 1024*63  
      dr.block = 0
      ants = dr["nofantennas"]
      blocksize = dr["blocksize"]
      cr_efield = np.empty((ants, blocksize))
      dr.read("Fx",cr_efield)
      (histo, bins) = np.histogram(cr_efield[0], bins=4096, range=(-2048.5,2047.5))
      plt.plot((bins[0:4096]+0.5),histo)
      print bins
      print histo
      raw_input("Plotted a histogram - press Enter to continue...")

      outfd.flush()
    except (ZeroDivisionError, IndexError):
      pass
  else:
    print 'file \'',filename,'\' does not exist!'    
outfd.close()

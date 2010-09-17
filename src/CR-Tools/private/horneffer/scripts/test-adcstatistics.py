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
datafiles = '/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/trigger-dumps-2010-07-07-cs003/*'
#datafiles = '/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/trigger-dumps-2010-07-07-cs005/*'
#datafiles = '/mnt/lofar/triggered-data/2010-07-07-CS003-CS005-CS006/trigger-dumps-2010-07-07-cs006/*'
#datafiles = '/home/driss/usg/data/lofar/trigger-2010-02-11/*h5'
#-------
sortstring = 'sort -rn --field-separator="-" --key=18'
outfile = 'test-adcstatistics-out.txt'
blocksize = 1024*63
normalize = True
blocks = [0]
maxants = 96
#------------------------------------------------------------------------

fd = os.popen('ls '+ datafiles+' | ' + sortstring)
files = fd.readlines()
nofiles = len(files)
print "Number of files to process:", nofiles
fd.close()

summedhist = np.zeros((maxants,4096))

for fileind in range(nofiles):
  print 'Processing file', fileind, 'out of', nofiles
  filename = files[fileind].strip()
  if os.path.exists(filename) :
    try:
      dr = cr.open(filename,'LOFAR_TBB')
      #select the data before the triggered pulse
      # ***this implements a bias in the data!***
      dr.blocksize = blocksize
      ants = dr["nofantennas"]
      blocksize = dr["blocksize"]
      cr_efield = np.empty((ants, blocksize))
      for blockno in blocks:
        dr.block = blockno
        dr.read("Fx",cr_efield)
        for antno in range(min(ants,maxants)):
          (histo, bins) = np.histogram(cr_efield[antno], bins=4096, range=(-2048.5,2047.5), normed=normalize)
          summedhist[antno] += histo
      
      #plt.plot((bins[0:4096]+0.5),histo)
      #print bins
      #print histo
      #raw_input("Plotted a histogram - press Enter to continue...")
    except (ZeroDivisionError, IndexError):
      pass
  else:
    print 'file \'',filename,'\' does not exist!'    
outfd = open(outfile, mode='w')
outfd.flush()
for binno in range(4096):
  outstring = '%5u '%(binno-2048)
  for antno in range(min(ants,maxants)):
    outstring += '%f '%(summedhist[antno,binno])
  outstring += '\n'
  outfd.write(outstring)
outfd.close()

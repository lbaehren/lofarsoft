
""" 

Given a list of assoc_id.s, make plots with op of psv, the spectrum
and also a spectrum with gsm points overplotted. this list of
assoc_ids can be made with awk, for example. 

The argument to this code is the gsm filename and a filename with the list of 
assoc_ids.

"""


import pylab as pl
import numpy as N
import os
import sys
import libs

################################################################
# Administration
################################################################

args = sys.argv
if len(args) != 3:
    print 'Usage : post_gsm gsmfilename idfilename'
    sys.exit()

gsmfilename = args[1]
idfilename = args[2]

if os.path.exists(gsmfilename): 
    print gsmfilename + 'doesnt exist'
    sys.exit()
if os.path.exists(idfilename): 
    print gsmfilename + 'doesnt exist'
    sys.exit()


################################################################


d = libs.readinfile(gsmfilename)

for id in d:
  print id







#! /usr/bin/env python

import sys
import pydal as dal
from pylab import *
	
# check usage
if len(sys.argv) != 7:
	print "Usage:"
	print "\tdata2ascii.py <msfile> <antenna1> <antenna2> " + \
	      "<sub-band> <channel> <output ascii filename>"
	print "\t<> required"
	print "\t[] optional"
	print "Writes data to ascii file.  Columns:  time (MJ seconds), xxr, xxi, xyr, xyi, yxr, yxi, yyr, yyi."
	sys.exit(1)

# open file
msds= dal.dalDataset()
if ( msds.open(sys.argv[1]) ):
	sys.exit(1)

# open table
tablename = "MAIN";
msds.setFilter( "TIME, DATA", \
	"ANTENNA1 = " + sys.argv[2] + " AND ANTENNA2 = " + sys.argv[3] + \
	" AND DATA_DESC_ID = " + sys.argv[4] )
maintable = msds.openTable( tablename );

# get times
time_col = maintable.getColumn("TIME")
time = time_col.data()

# get data
data_col = maintable.getColumn("DATA")
data = data_col.data()
nchannels = data.shape[1] # second element of the data shape i.e. (nrows,256,4) (after pydal change to numpy)

temp = sys.stdout
sys.stdout = open(sys.argv[6],'a')
for row in range(len(data)):
	print time[row],data[row,int(sys.argv[5]),0].real,data[row,int(sys.argv[5]),0].imag,data[row,int(sys.argv[5]),1].real,data[row,int(sys.argv[5]),1].imag,data[row,int(sys.argv[5]),2].real,data[row,int(sys.argv[5]),2].imag,data[row,int(sys.argv[5]),3].real,data[row,int(sys.argv[5]),3].imag

sys.stdout = temp

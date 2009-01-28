#! /usr/bin/env python

import sys
import pydal as dal
from pylab import *
	
# check usage
if len(sys.argv) != 7:
	print "Usage:"
	print "\tdata2ascii.py <msfile> <antenna1> <antenna2> " + \
	      "<sub-band> <channel or range> <output ascii filename>"
	print "\t<> required"
	print "\t[] optional"
	print "\tE.g., \t data2ascii.py SB3.MS 1 10 0 10:200 SB3.txt"
	print "\tWrites data to ascii file.  Columns:  time (MJ seconds), xxr, xxi, xyr, xyi, yxr, yxi, yyr, yyi."
	sys.exit(1)

# set channels to average
if len((sys.argv[5]).split(':')) > 1:
	channels = range(int((sys.argv[5]).split(':')[0]),int((sys.argv[5]).split(':')[1]))
else: channels = [int(sys.argv[5])]

# open file
msds= dal.dalDataset()
if ( True != msds.open(sys.argv[1]) ):
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

# average data
data_reduce = add.reduce(array=data[:,channels,:],axis=1)/len(channels)

temp = sys.stdout
sys.stdout = open(sys.argv[6],'a')
for row in range(len(data)):
	print time[row],data_reduce[row,0].real,data_reduce[row,0].imag,data_reduce[row,1].real,data_reduce[row,1].imag,data_reduce[row,2].real,data_reduce[row,2].imag,data_reduce[row,3].real,data_reduce[row,3].imag

sys.stdout = temp

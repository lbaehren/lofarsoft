#! /usr/bin/env python

import libpydal as dal
from pylab import *
import numarray
	
# check usage
if len(sys.argv) < 5:
	print "Usage:"
	print "\tbaseline_intensity.py <file> <antenna1> <antenna2> <sub-band> [channel]"
	print "\t<> required"
	print "\t[] optional"
	print ""
	sys.exit(1)

# open file
msds= dal.dalDataset(sys.argv[1], "MSCASA")

# open table
tablename = "MAIN";
filter_string = "SELECT TIME, DATA from $1 where " \
  + "ANTENNA1 = 1 AND ANTENNA2 = 2 AND DATA_DESC_ID = " + sys.argv[4]
maintable = msds.openFilteredTable( tablename, filter_string );

# get times
time_col = maintable.getColumn("TIME")
time_data = time_col.data()
time = time_data.get()

# get data
data_col = maintable.getColumn("DATA")
data_data = data_col.data()
data = data_data.get()
nchannels = data.shape[1] # second element of the data shape i.e. (4,256,nrows)

# if the optional channel argument is present
#  plot for this channel
if (6 == len(sys.argv)):
	# plot intensity of given channel vs. time
	plot( time, abs(data[0][int(sys.argv[5])]), "." )
	title("Time vs. Amplitude, Baseline " + \
	  sys.argv[2] + '-' + sys.argv[3] + ", Sub-band(" + sys.argv[4] + ") " + \
	  " Channel(" + sys.argv[5] + ")\n" + sys.argv[1] )

# otherwise, plot all channels
elif (5 == len(sys.argv)):
	# plot intensity of each channel vs. time
	for channel in range(data.shape[1]):
		plot( time, abs(data[0][channel]), "." )
	title("Time vs. Amplitude, Baseline " + \
	  sys.argv[2] + '-' + sys.argv[3] + ", Sub-band(" + sys.argv[4] + ") " + \
	  str(data.shape[1]) + " channels" + '\n' + sys.argv[1] )

xlabel("time")
ylabel("intensity")
gca().set_yscale("log")
show()

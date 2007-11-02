#! /usr/bin/env python

import sys
import pydal as dal
from pylab import *
from scipy import *

# check usage
if len(sys.argv) < 6 or len(sys.argv) > 9:
	print "Usage:"
	print "\tclosure_phase.py <file> <antenna1> <antenna2> <antenna3> " + \
	      "<sub-band> ['channel' or 'time'] [number of channel/time] " + \
	      "[polarization (0-6)]"
	print "\t<> required"
	print "\tE.g., closure_phase.py /lifs003/SB3.MS 1 5 10 0 channel 10 6"
	print "\tChannel and Time in relative bin coordinates;"
	print "\tPolarization 0-3 plots them individually, 4 plots xx and xy, 5 plots xx and yx, 6 plots xx and yy"
	print "\t-1 means plot all.  Default plots the closure phase of the xx for all channels as function of time."
	print "\tNote that antenna numbers are zero-based and must be given in order from lowest to highest."
	print ""
	sys.exit(1)

# other customizations
data_name = "DATA"

# set default values
if len(sys.argv) < 7:  quantity_plot = 'channel'
else: quantity_plot = sys.argv[6]
if len(sys.argv) < 8:  range_plot = -1
else: range_plot = int(sys.argv[7])
if len(sys.argv) < 9:  pol = 0
else:	pol = int(sys.argv[8])
if pol > 3:
	pol2 = pol-3
	pol = 0
	print "plotting pols %i and %i" % (pol, pol2)
else:
	pol2 = 0
	print "plotting pol %i" % (pol)

# open file
msds= dal.dalDataset()
if ( msds.open(sys.argv[1]) ):
	sys.exit(1)

# open tables
tablename = "MAIN";
msds.setFilter( "TIME," + data_name, \
	"ANTENNA1 = " + sys.argv[2] + " AND ANTENNA2 = " + sys.argv[3] + \
	" AND DATA_DESC_ID = " + sys.argv[5] )
table12 = msds.openTable( tablename );

msds.setFilter( "TIME," + data_name, \
	"ANTENNA1 = " + sys.argv[3] + " AND ANTENNA2 = " + sys.argv[4] + \
	" AND DATA_DESC_ID = " + sys.argv[5] )
table23 = msds.openTable( tablename );

msds.setFilter( "TIME," + data_name, \
	"ANTENNA1 = " + sys.argv[2] + " AND ANTENNA2 = " + sys.argv[4] + \
	" AND DATA_DESC_ID = " + sys.argv[5] )
table13 = msds.openTable( tablename );

# get times
time_col = table12.getColumn("TIME")
time = time_col.data()

# get data
data_col12 = table12.getColumn(data_name)
data12 = data_col12.data()
data_col23 = table23.getColumn(data_name)
data23 = data_col23.data()
data_col13 = table13.getColumn(data_name)
data13 = data_col13.data()
nchannels = data12.shape[1] # second element of the data shape i.e. (4,256,nrows)

# calculate phases
if quantity_plot == 'channel':       
	phase12 = arctan2((data12[pol,:,:]).real,(data12[pol,:,:]).imag)
	phase23 = arctan2((data23[pol,:,:]).real,(data23[pol,:,:]).imag)
	phase13 = arctan2((data13[pol,:,:]).real,(data13[pol,:,:]).imag)
	if pol2:
		phase12_2 = arctan2((data12[pol2,:,:]).real,(data12[pol2,:,:]).imag)
		phase23_2 = arctan2((data23[pol2,:,:]).real,(data23[pol2,:,:]).imag)
		phase13_2 = arctan2((data13[pol2,:,:]).real,(data13[pol2,:,:]).imag)
elif quantity_plot == 'time':
	phase12 = arctan2((data12[pol,:,:]).real,(data12[pol,:,:]).imag)
        phase23 = arctan2((data23[pol,:,:]).real,(data23[pol,:,:]).imag)
        phase13 = arctan2((data13[pol,:,:]).real,(data13[pol,:,:]).imag)
	if pol2:
		phase12_2 = arctan2((data12[pol2,:,:]).real,(data12[pol2,:,:]).imag)
		phase23_2 = arctan2((data23[pol2,:,:]).real,(data23[pol2,:,:]).imag)
		phase13_2 = arctan2((data13[pol2,:,:]).real,(data13[pol2,:,:]).imag)

closure = (phase12 + phase23 - phase13)%2*pi-pi
if pol2: closure_2 = (phase12_2 + phase23_2 - phase13_2)%2*pi-pi
closure[where(closure > pi)] = closure[where(closure > pi)] - 2*pi
	
# if the optional channel argument is present
#  plot for this channel/time
if (range_plot != -1):
	if quantity_plot == 'channel':
		# plot data of given data vs. time
		plot( time, closure[range_plot,:], "," )
		if pol2: plot( time, closure_2[range_plot,:], "," )
		title("Time vs. Closure Phase, Antennas " + \
		      sys.argv[2] + '-' + sys.argv[3] + '-' + sys.argv[4] + ", Sub-band(" + sys.argv[5] +
		      ") " + " Channel(" + str(range_plot) + ")\n" + sys.argv[1] )
		xlabel("Time (s)")

	elif quantity_plot == 'time':
		# plot intensity of given data vs. channel
		plot( closure[:,range_plot], "," )
		if pol2: plot( closure_2[:,range_plot], "," )
		title("Channel vs. Closure Phase, Antennas " + \
		      sys.argv[2] + '-' + sys.argv[3] + '-' + sys.argv[4] + ", Sub-band(" + sys.argv[5] +
		      ") " + " Time(" + str(time[range_plot]) + ")\n" + sys.argv[1] )
		xlabel("Channel")

# otherwise, plot all channels/times
else:
	if quantity_plot == 'channel':
		# plot intensity of each channel vs. time
		for channel in range( nchannels ):
			plot( closure[channel,:], "," )
			if pol2:  plot( closure_2[channel,:], "," )
			title("Channel vs. Closure Phase, Antennas " + \
			      sys.argv[2] + '-' + sys.argv[3] + '-' + sys.argv[4] + ", Sub-band(" + sys.argv[5] +
			      ") " + " Channel(" + str(range_plot) + ")\n" + sys.argv[1] )
		xlabel("Time (s)")
	if quantity_plot == 'time':
		# plot intensity at each time vs. channel
		for t in range( len(time) ):
			plot( closure[:,t], "," )
			if pol2:  plot( closure_2[:,t], "," )
			title("Time vs. Closure Phase, Antennas " + \
			      sys.argv[2] + '-' + sys.argv[3] + '-' + sys.argv[4] + ", Sub-band(" + sys.argv[5] +
			      ") " + str(len(time)) + " times" + '\n' + sys.argv[1] )
		xlabel("Channel")

ylabel("Closure phase (rad)")

show()



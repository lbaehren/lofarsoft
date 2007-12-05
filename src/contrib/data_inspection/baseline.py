#! /usr/bin/env python

import sys
import pydal as dal
from pylab import *
from numpy import *
from scipy import *

# check usage
if len(sys.argv) < 5 or len(sys.argv) > 10:
	print "Usage:"
	print "\tbaseline.py <file> <antenna1> <antenna2> " + \
	      "<sub-band> ['channel' or 'time'] [number of channel/time] " + \
	      "['p'hase or 'a'mplitude] [polarization (0-3,4-6)] [plot range xmin,xmax,ymin,ymax (no spaces)]"
	print "\t<> required"
	print "\t E.g., baseline.py /lifs006/SB3.MS 1 10 0 channel 3 a 6 4.69e9,4.7e9,0,0.001"
	print "\tChannel and Time in relative bin coordinates;"
	print "\tPolarization 0-3 plots individually, 4 plots xx and xy, 5 plots xx and yx, 6 plots xx and yy."
	print "\t-1 means plot all.  Default plots the closure phase of the xx for all channels as function of time."
	print "Note that antenna numbers are zero-based and must be given in order from lowest to highest."
	print ""
	sys.exit(1)

# other customizations
data_name = "DATA"

# set default values
if len(sys.argv) < 6:  quantity_plot = 'channel'
else: quantity_plot = sys.argv[5]
if len(sys.argv) < 7:  range_plot = -1
else: range_plot = int(sys.argv[6])
if len(sys.argv) < 8:  data_plot = 'a'
else:	data_plot = sys.argv[7]
if len(sys.argv) < 9:  pol = 0
else:	pol = int(sys.argv[8])
if len(sys.argv) < 10: axis_range = 0
else:
	axis_range = []
	for i in [0,1,2,3]:  axis_range.append(float(sys.argv[9].split(',')[i]))

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

# open table
tablename = "MAIN";
msds.setFilter( "TIME," + data_name, \
	"ANTENNA1 = " + sys.argv[2] + " AND ANTENNA2 = " + sys.argv[3] + \
	" AND DATA_DESC_ID = " + sys.argv[4] )
maintable = msds.openTable( tablename );

# get times
time_col = maintable.getColumn("TIME")
time = time_col.data()
time = time/(24*3600)    # convert from MJD in seconds to days

print 'Start of data (MJD from 1Oct2007): ' + str(time[0]-54374)
print 'Fractional day is ' + str(24*(time[0]-int(time[0]))) + ' hrs'
print 'Total length of data (integrations): ' + str(len(time)) + ', (hrs): ' + str((time[-1]-time[0])*24)
#event_time = 54379+(06+(41/60.))/24.      # 071006, obs L3907, el=-50
#event_time = 54381+(21+(55/60.))/24.      # 071008, obs L3914, el=+8       ## look at SB1, ch34 for best rfi (38.6009216 MHz)
#event_time = 54383+(03+(41/60.))/24.      # 071010A, obs L3917, el=-60
#event_time = 54383+(20+(45/60.))/24.      # 071010B, obs L3940, el=+8, stronger
#event_time = 54383+(22+(20/60.))/24.      # 071010C, obs L3940, el=+72
#event_time = 54386+(12+(9/60.))/24.       # 071013, obs L3980, el=+37
event_time = 0

# get data
data_col = maintable.getColumn(data_name)
data = data_col.data()
nchannels = data.shape[1] # second element of the data shape i.e. (4,256,nrows)

# if the optional channel argument is present
#  plot for this channel/time
if (range_plot != -1):
	if quantity_plot == 'channel':
		# plot data of given data vs. time
		if data_plot == 'a':
			current_value = hypot((data[pol,range_plot,:]).real,(data[pol,range_plot,:]).imag)
			if pol2:  current_value2 = hypot((data[pol2,range_plot,:]).real,(data[pol2,range_plot,:]).imag)
                elif data_plot == 'p':
			current_value = arctan2((data[pol,range_plot,:]).imag,(data[pol,range_plot,:]).real)%2*pi-pi
			if pol2:  current_value2 = arctan2((data[pol2,range_plot,:]).imag,(data[pol2,range_plot,:]).real)%2*pi-pi
		plot( time, current_value, "," )
		if pol2:  plot( time, current_value2, "," )
		if axis_range:  axis(axis_range)
		title("Time vs. Amplitude, Baseline " + \
		      sys.argv[2] + '-' + sys.argv[3] + ", Sub-band(" + sys.argv[4] +
		      ") " + " Channel(" + str(range_plot) + ")\n" + sys.argv[1] )
		xlabel("Time (MJD)")
		if event_time:  plot( event_time*ones(2), array([min(current_value),max(current_value)]))

	elif quantity_plot == 'time':
		# plot intensity of given data vs. channel
		if data_plot == 'a':
			current_value = hypot(array((data[pol,:,range_plot]).real),array((data[pol,:,range_plot]).imag))
			if pol2:  current_value2 = hypot(array((data[pol2,:,range_plot]).real),array((data[pol2,:,range_plot]).imag))
                elif data_plot == 'p':
			current_value = arctan2(array((data[pol,:,range_plot]).imag),array((data[pol,:,range_plot]).real))%2*pi-pi
			if pol2: current_value2 = arctan2(array((data[pol2,:,range_plot]).imag),array((data[pol2,:,range_plot]).real))%2*pi-pi
		plot( current_value, "," )
		if pol2: plot( current_value2, "," )
		if axis_range:  axis(axis_range)
		title("Channel vs. Amplitude, Baseline " + \
		      sys.argv[2] + '-' + sys.argv[3] + ", Sub-band(" + sys.argv[4] +
		      ") " + " Time(" + str(time[range_plot]) + " MJD)\n" + sys.argv[1] )
		xlabel("Channel")

# otherwise, plot all channels/times
else:
	if quantity_plot == 'channel':
		current_value_min = 0.
		current_value_max = 0.
		# plot intensity of each channel vs. time
		for channel in range( nchannels ):
			if data_plot == 'a':
				current_value = hypot(array((data[pol,channel,:]).real),array((data[pol,channel,:]).imag))
				if pol2: current_value2 = hypot(array((data[pol2,channel,:]).real),array((data[pol2,channel,:]).imag))
				current_value_min = min(current_value_min,min(current_value))
				current_value_max = max(current_value_max,max(current_value))
                        elif data_plot == 'p':
				current_value = arctan2(array((data[pol,channel,:]).imag),array((data[pol,channel,:]).real))%2*pi-pi
				if pol2: current_value2 = arctan2(array((data[pol2,channel,:]).imag),array((data[pol2,channel,:]).real))%2*pi-pi
			plot( time, current_value, "," )
			if pol2: plot ( time, current_value2, ",")
			if axis_range:  axis(axis_range)
			title("Time vs. Amplitude, Baseline " + \
			      sys.argv[2] + '-' + sys.argv[3] + ", Sub-band(" + sys.argv[4] +
			      ") " + str(data.shape[1]) + " channels" + '\n' + sys.argv[1] )
		xlabel("Time (MJD)")
		if event_time:  plot( event_time*ones(2), array([current_value_min,current_value_max]))

	if quantity_plot == 'time':
		# plot intensity at each time vs. channel
		for t in range( len(time) ):
                        if data_plot == 'a':
				current_value = hypot(array((data[pol,:,t]).real),array((data[pol,:,t]).imag))
				if pol2: current_value2 = hypot(array((data[pol2,:,t]).real),array((data[pol2,:,t]).imag))
                        elif data_plot == 'p':
				current_value = arctan2(array((data[pol,:,t]).imag),array((data[pol,:,t]).real))%2*pi-pi
				if pol2: current_value2 = arctan2(array((data[pol2,:,t]).imag),array((data[pol2,:,t]).real))%2*pi-pi
			plot( current_value, "," )
			if pol2: plot( current_value2, ",")
			if axis_range:  axis(axis_range)
			title("Time vs. Amplitude, Baseline " + \
			      sys.argv[2] + '-' + sys.argv[3] + ", Sub-band(" + sys.argv[4] +
			      ") " + str(len(time)) + " times" + '\n' + sys.argv[1] )
		xlabel("Channel")

if data_plot == 'a':
	ylabel("Intensity")
elif data_plot == 'p':
	ylabel("Phase (rad)")
	
show()



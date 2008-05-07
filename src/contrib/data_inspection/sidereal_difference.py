#! /usr/bin/env python

import sys
import pydal as dal
from pylab import *
from numpy import *
from scipy import *

# check usage
if len(sys.argv) < 4 or len(sys.argv) > 10:
	print "Usage:"
	print "\tsidereal_difference.py <file> <antenna1> <antenna2> " + \
	      "[sub-band] [channel range] [start time] " + \
	      "['p'hase or 'a'mplitude] [polarization (0-3,4-6)] [plot range xmin,xmax,ymin,ymax (no spaces)]"
	print "\t<> required"
	print "\t E.g., sidereal_difference.py /lifs006/SB3.MS 1 10 0 3 0 a 6 4.69e9,4.7e9,0,0.001"
	print "\tSingle or range of channels for averaging (in relative bin coordinates; ranges colon-delimited);"
	print "\tTime in relative bin coordinates;"
	print "\tPolarization 0-3 plots individually, 4 plots xx and xy, 5 plots xx and yx, 6 plots xx and yy."
	print "\t-1 means plot all.  Default plots xx for all channels as function of time for subband 0."
	print "Note that antenna numbers are zero-based and must be given in order from lowest to highest."
	print ""
	sys.exit(1)

# other customizations
data_name = "DATA"
timecut = (23+56/60.)/24    # 1 sidereal day, 23h56m

# set default values
if len(sys.argv) < 5:  subband = str(0)
else: subband = sys.argv[4]
if len(sys.argv) < 6:  range_plot = [0]
elif len((sys.argv[5]).split(':')) > 1:
	range_plot = range(int((sys.argv[5]).split(':')[0]),int((sys.argv[5]).split(':')[1]))
else: range_plot = [int(sys.argv[5])]
if len(sys.argv) < 7:  start_time = 0
else:	start_time = int(sys.argv[6])
if len(sys.argv) < 8:  data_plot = 'a'
else:	data_plot = sys.argv[7]
if len(sys.argv) < 9:  pol = 0
else:	pol = int(sys.argv[8])
if len(sys.argv) < 10: axis_range = 0
else:
	axis_range = []
	for i in [0,1,2,3]:  axis_range.append(float(sys.argv[8].split(',')[i]))

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
	" AND DATA_DESC_ID = " + subband )
maintable = msds.openTable( tablename );

# get times
time_col = maintable.getColumn("TIME")
time = time_col.data()
time = array(time[start_time:]/(24*3600))    # convert from MJD in seconds to days

# get data
data_col = maintable.getColumn(data_name)
data = array(data_col.data()[start_time:,:,:])
print shape(data)
nchannels = data.shape[1] # second element of the data shape i.e. (nrows,256,4)

data_reduce = add.reduce(array=data[:,range_plot,:],axis=1)/len(range_plot)

# get filtered data and time (old way, creates possibly different length arrays)
#time_day1 = time[where((time-time[0]) <= timecut)]
#data_day1 = data[where((time-time[0]) <= timecut)[0],:,:]
#time_day2 = time[where(((time-time[0]) >= timecut) * ((time-time[0]) <= 2*timecut))]
#data_day2 = data[where(((time-time[0]) >= timecut) * ((time-time[0]) <= 2*timecut))[0],:,:]
# get filtered data and time (new way;  assumes all time bins are same size)
indices_day1 = where((time-time[0]) <= timecut)[0]
time_day1 = time[indices_day1]
data_day1 = data_reduce[indices_day1,:]
time_day2 = time[indices_day1+len(indices_day1)]
data_day2 = data_reduce[indices_day1+len(indices_day1),:]

print 'Start of data (MJD from 1Jan2000): ' + str(time[0]-51544)
print 'Start of data (MJD from 17Oct2007): ' + str(time[0]-54390) + '.  Fractional day is ' + str(24*(time[0]-int(time[0]))) + ' hrs'
print 'Total length of data (integrations): ' + str(len(time)+start_time) + ', Day 1 (hrs): ' + str((time_day1[-1]-time_day1[0])*24) + ', Day 2 (hrs): ' + str((time_day2[-1]-time_day2[0])*24)

# set plot label and title
if data_plot == 'a':
	titlestring = "Amplitude and Difference, Baseline " + \
	      sys.argv[2] + '-' + sys.argv[3] + ", Sub-band(" + subband + \
	      ") " + " Chan0(" + str(range_plot[0]) + ")\n nchan(" + str(len(range_plot)) + "), " + sys.argv[1] 
	ylabelstring = "Intensity"
elif data_plot == 'p':
	titlestring = "Phase and Difference, Baseline " + \
	      sys.argv[2] + '-' + sys.argv[3] + ", Sub-band(" + subband + \
	      ") " + " Chan0(" + str(range_plot[0]) + ")\n nchan(" + str(len(range_plot)) + "), " + sys.argv[1] 
	ylabelstring = "Phase (rad)"

# if the optional channel argument is present
#  plot for this channel/time
if (range_plot != -1):
	# plot data of given data vs. time
	if data_plot == 'a':
		current_value_day1 = hypot((data_day1[:,pol]).real,(data_day1[:,pol]).imag)
		if pol2:  current_value2_day1 = hypot((data_day1[:,pol2]).real,(data_day1[:,pol2]).imag)
		current_value_day2 = hypot((data_day2[:,pol]).real,(data_day2[:,pol]).imag)
		if pol2:  current_value2_day2 = hypot((data_day2[:,pol2]).real,(data_day2[:,pol2]).imag)
	elif data_plot == 'p':
		current_value_day1 = arctan2((data_day1[:,pol]).imag,(data_day1[:,pol]).real)%2*pi-pi
		if pol2:  current_value2_day1 = arctan2((data_day1[:,pol2]).imag,(data_day1[:,pol2]).real)%2*pi-pi
		current_value_day2 = arctan2((data_day2[:,pol]).imag,(data_day2[:,pol]).real)%2*pi-pi
		if pol2:  current_value2_day2 = arctan2((data_day2[:,pol2]).imag,(data_day2[:,pol2]).real)%2*pi-pi
	figure(1)
	subplot(211)
	plot( time_day1, current_value_day1, "," , time_day2-timecut, current_value_day2, "," )
	if pol2:  plot( time_day1, current_value2_day1, "," , time_day2-timecut, current_value2_day2, "," )
	if axis_range:  axis(axis_range)
	ylabel(ylabelstring)
	title(titlestring)
	subplot(212)
	plot( time_day1, current_value_day1-current_value_day2, "," )
	if pol2:  plot( time_day1, current_value2_day1-current_value2_day2, "," )
	ylabel("Day1-Day2 Difference")
	xlabel("Time (MJD)")

## This is broken after adding the data averaging ##
#
# otherwise, plot all channels/times
#else:
#	# plot intensity of each channel vs. time
#	for channel in range( nchannels ):
#		if data_plot == 'a':
#			current_value_day1 = hypot(array((data_day1[:,channel,pol]).real),array((data_day1[:,channel,pol]).imag))
#			if pol2: current_value2_day1 = hypot(array((data_day1[:,channel,pol2]).real),array((data_day1[:,channel,pol2]).imag))
#			current_value_day2 = hypot(array((data_day2[:,channel,pol]).real),array((data_day2[:,channel,pol]).imag))
#			if pol2: current_value2_day2 = hypot(array((data_day2[:,channel,pol2]).real),array((data_day2[:,channel,pol2]).imag))
#		elif data_plot == 'p':
#			current_value_day1 = arctan2(array((data_day1[:,channel,pol]).imag),array((data_day1[:,channel,pol]).real))%2*pi-pi
#			if pol2: current_value2_day1 = arctan2(array((data_day1[:,channel,pol2]).imag),array((data_day1[:,channel,pol2]).real))%2*pi-pi
#			current_value_day2 = arctan2(array((data_day2[:,channel,pol]).imag),array((data_day2[:,channel,pol]).real))%2*pi-pi
#			if pol2: current_value2_day2 = arctan2(array((data_day2[:,channel,pol2]).imag),array((data_day2[:,channel,pol2]).real))%2*pi-pi
#		figure(1)
#		subplot(211)
#		plot( time_day1, current_value_day1, "," , time_day2-timecut, current_value_day2, "," )
#		if pol2:  plot( time_day1, current_value2_day1, "," , time_day2-timecut, current_value2_day2, "," )
#		if axis_range:  axis(axis_range)
#		ylabel(ylabelstring)
#		title(titlestring)
#		subplot(212)
#		plot( time_day1, current_value_day1-current_value_day2, "," )
#		if pol2:  plot( time_day1, current_value2_day1-current_value2_day2, "," )
#		ylabel("Day1-Day2 Difference")
#		xlabel("Time (MJD)")

show()



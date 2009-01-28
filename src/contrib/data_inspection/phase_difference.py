#! /usr/bin/env python

import sys
import pydal as dal
from pylab import *
from scipy import *

# check usage
if len(sys.argv) < 6 or len(sys.argv) > 10:
	print "Usage:"
	print "\tphase_difference.py <file> <antenna1-1> <antenna1-2> <antenna2-1> <antenna2-2> " + \
	      "[sub-band] ['channel' or 'time'] [channel/time range] " + \
	      "[polarization (0-3,4-6)]"
	print "\t<> required"
	print "\t E.g., phase_difference.py /lifs003/SB3.MS 8 11 9 10 0 channel 100:110 6"
	print "\tSingle or range of channels and times for averaging (in relative bin coordinates; ranges colon-delimited);"
	print "\tPolarization 0-3 plots individually, 4 plots xx and xy, 5 plots xx and yx, 6 plots xx and yy."
	print "\t-1 means plot all.  Default plots the phase difference of the xx for all channels as function of time for subband 0."
	print "\tNote that antenna numbers are zero-based and numbers for each antenna pair must be given in order from lowest to highest."
	print ""
	sys.exit(1)

# other customizations
data_name = "DATA"

# set default values
if len(sys.argv) < 7:  subband = str(0)
else: subband = sys.argv[6]
if len(sys.argv) < 8:  quantity_plot = 'channel'
else: quantity_plot = sys.argv[7]
if len(sys.argv) < 9:  range_plot = [0]
elif len((sys.argv[8]).split(':')) > 1:
	range_plot = range(int((sys.argv[8]).split(':')[0]),int((sys.argv[8]).split(':')[1]))
else: range_plot = [int(sys.argv[8])]
if len(sys.argv) < 10:  pol = 0
else:	pol = int(sys.argv[9])
if pol > 3:
	pol2 = pol-3
	pol = 0
	print "plotting pols %i and %i" % (pol, pol2)
else:
	pol2 = 0
	print "plotting pol %i" % (pol)

# open file
msds= dal.dalDataset()
if ( True != msds.open(sys.argv[1]) ):
	sys.exit(1)

# open tables
tablename = "MAIN";
msds.setFilter( "TIME," + data_name, \
	"ANTENNA1 = " + sys.argv[2] + " AND ANTENNA2 = " + sys.argv[3] + \
	" AND DATA_DESC_ID = " + subband )
table12 = msds.openTable( tablename );

msds.setFilter( "TIME," + data_name, \
	"ANTENNA1 = " + sys.argv[4] + " AND ANTENNA2 = " + sys.argv[5] + \
	" AND DATA_DESC_ID = " + subband )
table34 = msds.openTable( tablename );

# get times
time_col = table12.getColumn("TIME")
time = time_col.data()
time = time/(24*3600)    # convert from MJD in seconds to days

# get data
data_col12 = table12.getColumn(data_name)
data12 = data_col12.data()
data_col34 = table34.getColumn(data_name)
data34 = data_col34.data()
nchannels = data12.shape[1] # second element of the data shape i.e. (nrows,256,4)

if quantity_plot == 'channel':
	data12_reduce = add.reduce(array=data12[:,range_plot,:],axis=1)/len(range_plot)
	data34_reduce = add.reduce(array=data34[:,range_plot,:],axis=1)/len(range_plot)
elif quantity_plot == 'time':
	data12_reduce = add.reduce(array=data12[range_plot,:,:],axis=0)/len(range_plot)
	data34_reduce = add.reduce(array=data34[range_plot,:,:],axis=0)/len(range_plot)

# calculate phases
phase12 = arctan2((data12_reduce[:,pol]).imag,(data12_reduce[:,pol]).real)
phase34 = arctan2((data34_reduce[:,pol]).imag,(data34_reduce[:,pol]).real)
if pol2:
	phase12_2 = arctan2((data12_reduce[:,pol2]).imag,(data12_reduce[:,pol2]).real)
	phase34_2 = arctan2((data34_reduce[:,pol2]).imag,(data34_reduce[:,pol2]).real)

difference = (phase12 - phase34)
# normalize range to -pi to +pi
difference[where(difference > pi)] = difference[where(difference > pi)] - 2*pi
difference[where(difference < -pi)] = difference[where(difference < -pi)] + 2*pi
if pol2:
	difference_2 = (phase12_2 - phase34_2)
	difference_2[where(difference_2 > pi)] = difference_2[where(difference_2 > pi)] - 2*pi
	difference_2[where(difference_2 < -pi)] = difference_2[where(difference_2 < -pi)] + 2*pi
	
figure(1)
# if the optional channel argument is present
#  plot for this channel/time
if (range_plot != -1):
	if quantity_plot == 'channel':
		# plot data of given data vs. time
		subplot(212)
                plot( time, difference, ',')
		if pol2: plot( time, difference_2, "," )
		xlabel("Time (MJD)")
		ylabel("Phase difference (rad)")
		subplot(211)
		plot( time, phase12, ',', time, phase34, ',')
		if pol2: plot( time, phase12_2, ',', time, phase34_2, ',')
		ylabel("Phase (rad)")
		title("Time vs. Phase Difference, Antennas " + \
		      sys.argv[2] + '-' + sys.argv[3] + '/' + sys.argv[4] + '-' + sys.argv[5] + ", Sub-band(" + subband +
		      ") " + " Chan0(" + str(range_plot[0]) + ")\n nchan(" + str(len(range_plot)) + "), " + sys.argv[1] )

	elif quantity_plot == 'time':
		# plot intensity of given data vs. channel
		plot( difference, "," )
		if pol2: plot( difference_2, "," )
		title("Channel vs. Phase Difference, Antennas " + \
		      sys.argv[2] + '-' + sys.argv[3] + '/' + sys.argv[4] + '-' + sys.argv[5] + ", Sub-band(" + subband +
		      ") " + " Time0(" + str(time[range_plot[0]]) + " MJD)\n dtime(" + str(len(range_plot)) + "), " + sys.argv[1] )
		xlabel("Channel")
		ylabel("Phase difference (rad)")

## This is broken after adding the data averaging ##
#
# otherwise, plot all channels/times
#else:
#	if quantity_plot == 'channel':
#		# plot intensity of each channel vs. time
#		for channel in range( nchannels ):
#			plot( difference[channel,:], "," )
#			if pol2:  plot( difference_2[channel,:], "," )
#			title("Channel vs. Phase Difference, Antennas " + \
#			      sys.argv[2] + '-' + sys.argv[3] + '/' + sys.argv[4] + '-' + sys.argv[5] + ", Sub-band(" + subband +
#			      ") " + " Channel(" + str(range_plot) + ")\n" + sys.argv[1] )
#		xlabel("Time (MJD)")
#		ylabel("Phase difference (rad)")
#	if quantity_plot == 'time':
#		# plot intensity at each time vs. channel
#		for t in range( len(time) ):
#			plot( difference[:,t], "," )
#			if pol2:  plot( difference_2[:,t], "," )
#			title("Time vs. Phase Difference, Antennas " + \
#			      sys.argv[2] + '-' + sys.argv[3] + '/' + sys.argv[4] + '-' + sys.argv[5] + ", Sub-band(" + subband +
#			      ") " + str(len(time)) + " times" + '\n' + sys.argv[1] )
#		xlabel("Channel")
#		ylabel("Phase difference (rad)")

show()



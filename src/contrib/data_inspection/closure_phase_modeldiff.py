#! /usr/bin/env python

import sys
import pydal as dal
from pylab import *
from scipy import *

# check usage
if len(sys.argv) < 5 or len(sys.argv) > 9:
	print "Usage:"
	print "\tclosure_phase_modeldiff.py <file> <antenna1> <antenna2> <antenna3> " + \
	      "[sub-band] ['channel' or 'time'] [channel/time range] " + \
	      "[polarization (0-6)]"
	print "\t<> required"
	print "\tE.g., closure_phase_modeldiff.py /lifs003/SB3.MS 1 5 10 0 channel 100:110 6"
	print "\tSingle or range of channels and times for averaging (in relative bin coordinates; ranges colon-delimited);"
	print "\tPolarization 0-3 plots them individually, 4 plots xx and xy, 5 plots xx and yx, 6 plots xx and yy"
	print "\t-1 means plot all.  Default plots the closure phase model difference for xx for all channels as function of time for subband 0."
	print "\tNote that antenna numbers are zero-based and must be given in order from lowest to highest."
	print ""
	sys.exit(1)

# other customizations
data_name1 = "DATA"
data_name2 = "MODEL_DATA"

# set default values
if len(sys.argv) < 6:  subband = str(0)
else: subband = sys.argv[5]
if len(sys.argv) < 7:  quantity_plot = 'channel'
else: quantity_plot = sys.argv[6]
if len(sys.argv) < 8:  range_plot = [0]
elif len((sys.argv[7]).split(':')) > 1:
	range_plot = range(int((sys.argv[7]).split(':')[0]),int((sys.argv[7]).split(':')[1]))
else: range_plot = [int(sys.argv[7])]
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
msds.setFilter( "TIME," + data_name1 + "," + data_name2, \
	"ANTENNA1 = " + sys.argv[2] + " AND ANTENNA2 = " + sys.argv[3] + \
	" AND DATA_DESC_ID = " + subband )
table12 = msds.openTable( tablename );

msds.setFilter( "TIME," + data_name1 + "," + data_name2, \
	"ANTENNA1 = " + sys.argv[3] + " AND ANTENNA2 = " + sys.argv[4] + \
	" AND DATA_DESC_ID = " + subband )
table23 = msds.openTable( tablename );

msds.setFilter( "TIME," + data_name1 + "," + data_name2, \
	"ANTENNA1 = " + sys.argv[2] + " AND ANTENNA2 = " + sys.argv[4] + \
	" AND DATA_DESC_ID = " + subband )
table13 = msds.openTable( tablename );

# get times
time_col = table12.getColumn("TIME")
time = time_col.data()
time = time/(24*3600)    # convert from MJD in seconds to days

# get data
data_col12_1 = table12.getColumn(data_name1)
data12_1 = data_col12_1.data()
data_col23_1 = table23.getColumn(data_name1)
data23_1 = data_col23_1.data()
data_col13_1 = table13.getColumn(data_name1)
data13_1 = data_col13_1.data()
data_col12_2 = table12.getColumn(data_name2)
data12_2 = data_col12_2.data()
data_col23_2 = table23.getColumn(data_name2)
data23_2 = data_col23_2.data()
data_col13_2 = table13.getColumn(data_name2)
data13_2 = data_col13_2.data()
nchannels = data12_1.shape[1] # second element of the data shape i.e. (nrows,256,4)

# calculate phases
if quantity_plot == 'channel':       
	data12_1_reduce = add.reduce(array=data12_1[:,range_plot,:],axis=1)/len(range_plot)
	data23_1_reduce = add.reduce(array=data23_1[:,range_plot,:],axis=1)/len(range_plot)
	data13_1_reduce = add.reduce(array=data13_1[:,range_plot,:],axis=1)/len(range_plot)
	data12_2_reduce = add.reduce(array=data12_2[:,range_plot,:],axis=1)/len(range_plot)
	data23_2_reduce = add.reduce(array=data23_2[:,range_plot,:],axis=1)/len(range_plot)
	data13_2_reduce = add.reduce(array=data13_2[:,range_plot,:],axis=1)/len(range_plot)
elif quantity_plot == 'time':
	data12_1_reduce = add.reduce(array=data12_1[range_plot,:,:],axis=0)/len(range_plot)
	data23_1_reduce = add.reduce(array=data23_1[range_plot,:,:],axis=0)/len(range_plot)
	data13_1_reduce = add.reduce(array=data13_1[range_plot,:,:],axis=0)/len(range_plot)
	data12_2_reduce = add.reduce(array=data12_2[range_plot,:,:],axis=0)/len(range_plot)
	data23_2_reduce = add.reduce(array=data23_2[range_plot,:,:],axis=0)/len(range_plot)
	data13_2_reduce = add.reduce(array=data13_2[range_plot,:,:],axis=0)/len(range_plot)

phase12_1 = arctan2((data12_1_reduce[:,pol]).imag,(data12_1_reduce[:,pol]).real)
phase23_1 = arctan2((data23_1_reduce[:,pol]).imag,(data23_1_reduce[:,pol]).real)
phase13_1 = arctan2((data13_1_reduce[:,pol]).imag,(data13_1_reduce[:,pol]).real)
phase12_2 = arctan2((data12_2_reduce[:,pol]).imag,(data12_2_reduce[:,pol]).real)
phase23_2 = arctan2((data23_2_reduce[:,pol]).imag,(data23_2_reduce[:,pol]).real)
phase13_2 = arctan2((data13_2_reduce[:,pol]).imag,(data13_2_reduce[:,pol]).real)
if pol2:
	phase12_1_2 = arctan2((data12_1_reduce[:,pol2]).imag,(data12_1_reduce[:,pol2]).real)
	phase23_1_2 = arctan2((data23_1_reduce[:,pol2]).imag,(data23_1_reduce[:,pol2]).real)
	phase13_1_2 = arctan2((data13_1_reduce[:,pol2]).imag,(data13_1_reduce[:,pol2]).real)
	phase12_2_2 = arctan2((data12_2_reduce[:,pol2]).imag,(data12_2_reduce[:,pol2]).real)
	phase23_2_2 = arctan2((data23_2_reduce[:,pol2]).imag,(data23_2_reduce[:,pol2]).real)
	phase13_2_2 = arctan2((data13_2_reduce[:,pol2]).imag,(data13_2_reduce[:,pol2]).real)

# closure stuff
closure_1 = (phase12_1 + phase23_1 - phase13_1)%2*pi-pi
closure_2 = (phase12_2 + phase23_2 - phase13_2)%2*pi-pi
if pol2:
	closure_1_2 = (phase12_1_2 + phase23_1_2 - phase13_1_2)%2*pi-pi
	closure_2_2 = (phase12_2_2 + phase23_2_2 - phase13_2_2)%2*pi-pi

closure_1[where(closure_1 > pi)] = closure_1[where(closure_1 > pi)] - 2*pi
closure_2[where(closure_2 > pi)] = closure_2[where(closure_2 > pi)] - 2*pi
closure_diff = closure_1 - closure_2
if pol2: closure_diff_2 = closure_1_2 - closure_2_2

# if the optional channel argument is present
#  plot for this channel/time
if (range_plot != -1):
	if quantity_plot == 'channel':
		# plot data of given data vs. time
		subplot(211)
		plot( time, closure_diff, "," )
		if pol2:
			plot( time, closure_diff_2, "," )
		subplot(212)
		plot( time, closure_1, ",", time, closure_2, ",")
		subplot(211)
		title("Time vs. Closure Phase Diff, Antennas " + \
		      sys.argv[2] + '-' + sys.argv[3] + '-' + sys.argv[4] + ", Sub-band(" + subband +
		      ") " + " Chan0(" + str(range_plot[0]) + ")\n nchan(" + str(len(range_plot)) + ") " + sys.argv[1] )
		xlabel("Time (MJD)")

	elif quantity_plot == 'time':
		# plot intensity of given data vs. channel
		subplot(211)
		plot( closure_diff, "," )
		if pol2: plot( closure_diff_2, "," )
		subplot(212)
		plot( closure_1, ",", closure_2, ",")
		subplot(211)
		title("Channel vs. Closure Phase Diff, Antennas " + \
		      sys.argv[2] + '-' + sys.argv[3] + '-' + sys.argv[4] + ", Sub-band(" + subband +
		      ") " + " Time(" + str(time[range_plot[0]]) + " MJD)\n dtime(" + str(len(range_plot)) + ") " + sys.argv[1] )
		xlabel("Channel")

## This is broken after adding the data averaging ##
#
# otherwise, plot all channels/times
#else:
#	if quantity_plot == 'channel':
#		# plot intensity of each channel vs. time
#		for channel in range( nchannels ):
#			plot( closure[channel,:], "," )
#			if pol2:  plot( closure_2[channel,:], "," )
#			title("Channel vs. Closure Phase, Antennas " + \
#			      sys.argv[2] + '-' + sys.argv[3] + '-' + sys.argv[4] + ", Sub-band(" + subband +
#			      ") " + " Channel(" + str(range_plot) + ")\n" + sys.argv[1] )
#		xlabel("Time (MJD)")
#	if quantity_plot == 'time':
#		# plot intensity at each time vs. channel
#		for t in range( len(time) ):
#			plot( closure[:,t], "," )
#			if pol2:  plot( closure_2[:,t], "," )
#			title("Time vs. Closure Phase, Antennas " + \
#			      sys.argv[2] + '-' + sys.argv[3] + '-' + sys.argv[4] + ", Sub-band(" + subband +
#			      ") " + str(len(time)) + " times" + '\n' + sys.argv[1] )
#		xlabel("Channel")

ylabel("Closure phase Diff (rad)")

show()



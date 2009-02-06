#! /usr/bin/env python

import sys
import pydal as dal
from pylab import *
from scipy import *

# check usage
if len(sys.argv) < 6 or len(sys.argv) > 10:
	print "Usage:"
	print "\tamplitude_difference.py <file> <antenna1-1> <antenna1-2> <antenna2-1> <antenna2-2> " + \
	      "[sub-band] ['channel' or 'time'] [channel/time range] " + \
	      "[polarization (0-3,4-6)]"
	print "\t<> required"
	print "\t E.g., amplitude_difference.py /lifs006/SB3.MS 1 3 8 13 0 channel 10:20 6"
	print "\tSingle or range of channels and times for averaging (in relative bin coordinates; ranges colon-delimited);"
	print "\tPolarization 0-3 plots individually, 4 plots xx and xy, 5 plots xx and yx, 6 plots xx and yy."
	print "\t-1 means plot all.  Default plots the amplitude difference of the xx for all channels as function of time for subband 0."
	print "\tNote that antenna numbers must be given in order from lowest to highest."
	print ""
	sys.exit(1)

# To do:  Add plot of elevation of Cas A, Cyg A, etc.

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
	print "ERROR: Could not open file: " + sys.argv[1]
	print "       Please check the file and try again."
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

# get data
data_col12 = table12.getColumn(data_name)
data12 = data_col12.data()
data_col34 = table34.getColumn(data_name)
data34 = data_col34.data()
nchannels = data12.shape[1] # second element of the data shape i.e. (nrows,256,4)

# calculate amplitudes
if quantity_plot == 'channel':
	data12_reduce = add.reduce(array=data12[:,range_plot,:],axis=1)/len(range_plot)
	data34_reduce = add.reduce(array=data34[:,range_plot,:],axis=1)/len(range_plot)
elif quantity_plot == 'time':
	data12_reduce = add.reduce(array=data12[range_plot,:,:],axis=0)/len(range_plot)
	data34_reduce = add.reduce(array=data34[range_plot,:,:],axis=0)/len(range_plot)

ampl12 = hypot((data12_reduce[:,pol]).real,(data12_reduce[:,pol]).imag)
ampl34 = hypot((data34_reduce[:,pol]).real,(data34_reduce[:,pol]).imag)
if pol2:
	ampl12_2 = hypot((data12_reduce[:,pol2]).real,(data12_reduce[:,pol2]).imag)
	ampl34_2 = hypot((data34_reduce[:,pol2]).real,(data34_reduce[:,pol2]).imag)

#ratio = ampl12/ampl34
#if pol2: ratio_2 = ampl12_2/ampl34_2
difference = (ampl12-ampl34)#/(ampl12+ampl34)
if pol2: difference_2 = (ampl12_2-ampl34_2)#/(ampl12_2+ampl34_2)

figure(1)
# if the optional channel argument is present
#  plot for this channel/time range average
if (range_plot != -1):
	if quantity_plot == 'channel':
		# plot data of given data vs. time
		subplot(211)
		plot( time, difference, ",")
		if pol2: plot( time, difference_2, "," )
		ylabel("Amplitude Difference")
		title("Time vs. Amplitude Difference, Baselines " + \
		      sys.argv[2] + '-' + sys.argv[3] + '/' + sys.argv[4] +  '-' + sys.argv[5] + ", Sub-band(" + subband +
		      ") " + " Chan0(" + str(range_plot[0]) + ")\n nchan(" + str(len(range_plot)) + "), " + sys.argv[1] )
		subplot(212)
		plot( time, ampl12, "," ,time, ampl34, ",")
		if pol2:  plot( time, ampl12_2, "," ,time, ampl34_2, ",")
		xlabel("Time (s)")
		ylabel("Amplitude")

	elif quantity_plot == 'time':
		# plot intensity of given data vs. channel
		subplot(211)
		plot( difference, "," )
		if pol2:  plot( difference_2, "," )
		title("Channel vs. Amplitude Difference, Baselines " + \
		      sys.argv[2] + '-' + sys.argv[3] + '/' + sys.argv[4] +  '-' + sys.argv[5] + ", Sub-band(" + subband +
		      ") " + " Time(" + str(time[range_plot[0]]) + ")\n dtime(" + str(len(range_plot)) + " ints), " + sys.argv[1] )
		subplot(212)
		plot( ampl12, "," , ampl34, ",")
		if pol2:  plot( ampl12_2, "," , ampl34_2, ",")
		xlabel("Channel")
		ylabel("Amplitude")

## This is broken after adding the data averaging ##
#
# otherwise, plot all channels/times
#else:
#	print 'Sorry, no subplots done in this case...'
#	if quantity_plot == 'channel':
#		# plot intensity of each channel vs. time
#		for channel in range( nchannels ):
#			plot( difference[channel,:], "," )
#			if pol2: plot( difference_2[channel,:], "," )
#			title("Time vs.Amplitude Difference , Baselines " + \
#			      sys.argv[2] + '-' + sys.argv[3] + '/' + sys.argv[4] +  '-' + sys.argv[5] + ", Sub-band(" + subband +
#			      ") " + str(data12.shape[1]) + " channels" + '\n' + sys.argv[1] )
#		xlabel("Time (s)")
#	if quantity_plot == 'time':
#		# plot intensity at each time vs. channel
#		for t in range( len(time) ):
#			plot( difference[:,t], "," )
#			if pol2:  plot( difference_2[:,t], "," )
#			title("Time vs. Amplitude Difference, Baselines " + \
#			      sys.argv[2] + '-' + sys.argv[3] +  '/' + sys.argv[4] +  '-' + sys.argv[5] + ", Sub-band(" + subband +
#			      ") " + str(len(time)) + " times" + '\n' + sys.argv[1] )
#		xlabel("Channel")

show()



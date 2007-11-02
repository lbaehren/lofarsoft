#! /usr/bin/env python

import sys
import pydal as dal
from pylab import *
from scipy import *

# check usage
if len(sys.argv) < 7 or len(sys.argv) > 10:
	print "Usage:"
	print "\tclosure_phase.py <file> <antenna1> <antenna2> <antenna3> <antenna4> " + \
	      "<sub-band> ['channel' or 'time'] [number of channel/time] " + \
	      "[polarization (0-3,4-6)]"
	print "\t<> required"
	print "\t E.g., closure_amplitude.py /lifs006/SB3.MS 1 3 8 13 0 channel 3 6"
	print "\tChannel and Time in relative bin coordinates;"
	print "\tPolarization 0-3 plots individually, 4 plots xx and xy, 5 plots xx and yx, 6 plots xx and yy."
	print "\t-1 means plot all.  Default plots the closure phase of the xx for all channels as function of time."
	print "\tNote that antenna numbers must be given in order from lowest to highest."
	print ""
	sys.exit(1)

# other customizations
data_name = "DATA"

# set default values
if len(sys.argv) < 8:  quantity_plot = 'channel'
else: quantity_plot = sys.argv[7]
if len(sys.argv) < 9:  range_plot = -1
else: range_plot = int(sys.argv[8])
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
if ( msds.open(sys.argv[1]) ):
	sys.exit(1)

# open tables
tablename = "MAIN";
msds.setFilter( "TIME," + data_name, \
	"ANTENNA1 = " + sys.argv[2] + " AND ANTENNA2 = " + sys.argv[3] + \
	" AND DATA_DESC_ID = " + sys.argv[6] )
table12 = msds.openTable( tablename );

msds.setFilter( "TIME," + data_name, \
	"ANTENNA1 = " + sys.argv[4] + " AND ANTENNA2 = " + sys.argv[5] + \
	" AND DATA_DESC_ID = " + sys.argv[6] )
table34 = msds.openTable( tablename );

msds.setFilter( "TIME," + data_name, \
	"ANTENNA1 = " + sys.argv[2] + " AND ANTENNA2 = " + sys.argv[4] + \
	" AND DATA_DESC_ID = " + sys.argv[6] )
table13 = msds.openTable( tablename );

msds.setFilter( "TIME," + data_name, \
	"ANTENNA1 = " + sys.argv[3] + " AND ANTENNA2 = " + sys.argv[5] + \
	" AND DATA_DESC_ID = " + sys.argv[6] )
table24 = msds.openTable( tablename );

# get times
time_col = table12.getColumn("TIME")
time = time_col.data()

# get data
data_col12 = table12.getColumn(data_name)
data12 = data_col12.data()
data_col34 = table34.getColumn(data_name)
data34 = data_col34.data()
data_col13 = table13.getColumn(data_name)
data13 = data_col13.data()
data_col24 = table24.getColumn(data_name)
data24 = data_col24.data()
nchannels = data12.shape[1] # second element of the data shape i.e. (4,256,nrows)

# calculate phases
if quantity_plot == 'channel':       
	ampl12 = hypot((data12[pol,:,:]).real,(data12[pol,:,:]).imag)
        ampl34 = hypot((data34[pol,:,:]).real,(data34[pol,:,:]).imag)
        ampl13 = hypot((data13[pol,:,:]).real,(data13[pol,:,:]).imag)
        ampl24 = hypot((data24[pol,:,:]).real,(data24[pol,:,:]).imag)
	if pol2:
		ampl12_2 = hypot((data12[pol2,:,:]).real,(data12[pol2,:,:]).imag)
		ampl34_2 = hypot((data34[pol2,:,:]).real,(data34[pol2,:,:]).imag)
		ampl13_2 = hypot((data13[pol2,:,:]).real,(data13[pol2,:,:]).imag)
		ampl24_2 = hypot((data24[pol2,:,:]).real,(data24[pol2,:,:]).imag)
elif quantity_plot == 'time':
	ampl12 = hypot((data12[pol,:,:]).real,(data12[pol,:,:]).imag)
        ampl34 = hypot((data34[pol,:,:]).real,(data34[pol,:,:]).imag)
        ampl13 = hypot((data13[pol,:,:]).real,(data13[pol,:,:]).imag)
        ampl24 = hypot((data24[pol,:,:]).real,(data24[pol,:,:]).imag)
	if pol2:
		ampl12_2 = hypot((data12[pol2,:,:]).real,(data12[pol2,:,:]).imag)
		ampl34_2 = hypot((data34[pol2,:,:]).real,(data34[pol2,:,:]).imag)
		ampl13_2 = hypot((data13[pol2,:,:]).real,(data13[pol2,:,:]).imag)
		ampl24_2 = hypot((data24[pol2,:,:]).real,(data24[pol2,:,:]).imag)

closure = (ampl12*ampl34)/(ampl13*ampl24)
if pol2: closure_2 = (ampl12_2*ampl34_2)/(ampl13_2*ampl24_2)

# if the optional channel argument is present
#  plot for this channel/time
if (range_plot != -1):
	if quantity_plot == 'channel':
		# plot data of given data vs. time
		plot( time, closure[range_plot,:], "," )
		if pol2: plot( time, closure_2[range_plot,:], "," )
		title("Time vs. Closure Amplitude, Baseline " + \
		      sys.argv[2] + '-' + sys.argv[3] + '-' + sys.argv[4] +  '-' + sys.argv[5] + ", Sub-band(" + sys.argv[6] +
		      ") " + " Channel(" + str(range_plot) + ")\n" + sys.argv[1] )
		xlabel("Time (s)")

	elif quantity_plot == 'time':
		# plot intensity of given data vs. channel
		plot( closure[:,range_plot], "," )
		if pol2:  plot( closure_2[:,range_plot], "," )
		title("Channel vs. Closure Amplitude, Baseline " + \
		      sys.argv[2] + '-' + sys.argv[3] + '-' + sys.argv[4] +  '-' + sys.argv[5] + ", Sub-band(" + sys.argv[6] +
		      ") " + " Time(" + str(time[range_plot]) + ")\n" + sys.argv[1] )
		xlabel("Channel")

# otherwise, plot all channels/times
else:
	if quantity_plot == 'channel':
		# plot intensity of each channel vs. time
		for channel in range( nchannels ):
			plot( closure[channel,:], "," )
			if pol2: plot( closure_2[channel,:], "," )
			title("Time vs. Closure Amplitude, Baseline " + \
			      sys.argv[2] + '-' + sys.argv[3] + '-' + sys.argv[4] +  '-' + sys.argv[5] + ", Sub-band(" + sys.argv[6] +
			      ") " + str(data12.shape[1]) + " channels" + '\n' + sys.argv[1] )
		xlabel("Time (s)")
	if quantity_plot == 'time':
		# plot intensity at each time vs. channel
		for t in range( len(time) ):
			plot( closure[:,t], "," )
			if pol2:  plot( closure_2[:,t], "," )
			title("Time vs. Closure Amplitude, Baseline " + \
			      sys.argv[2] + '-' + sys.argv[3] +  '-' + sys.argv[4] +  '-' + sys.argv[5] + ", Sub-band(" + sys.argv[6] +
			      ") " + str(len(time)) + " times" + '\n' + sys.argv[1] )
		xlabel("Channel")

ylabel("Closure Amplitude")
show()



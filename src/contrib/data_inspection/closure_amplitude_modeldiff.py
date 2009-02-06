#! /usr/bin/env python

import sys
import pydal as dal
from pylab import *
from scipy import *

# check usage
if len(sys.argv) < 6 or len(sys.argv) > 10:
	print "Usage:"
	print "\tclosure_phase_modeldiff.py <file> <antenna1> <antenna2> <antenna3> <antenna4> " + \
	      "[sub-band] ['channel' or 'time'] [channel/time range] " + \
	      "[polarization (0-3,4-6)]"
	print "\t<> required"
	print "\t E.g., closure_amplitude_modeldiff.py /lifs006/SB3.MS 1 3 8 13 0 channel 100:110 6"
	print "\tSingle or range of channels and times for averaging (in relative bin coordinates; ranges colon-delimited);"
	print "\tPolarization 0-3 plots individually, 4 plots xx and xy, 5 plots xx and yx, 6 plots xx and yy."
	print "\t-1 means plot all.  Default plots the closure phase of the xx for all channels as function of time for subband 0."
	print "\tNote that antenna numbers must be given in order from lowest to highest."
	print ""
	sys.exit(1)

# other customizations
data_name1 = "DATA"
data_name2 = "MODEL_DATA"

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
msds.setFilter( "TIME," + data_name1 + ',' + data_name2, \
	"ANTENNA1 = " + sys.argv[2] + " AND ANTENNA2 = " + sys.argv[3] + \
	" AND DATA_DESC_ID = " + subband )
table12 = msds.openTable( tablename );

msds.setFilter( "TIME," + data_name1 + ',' + data_name2, \
	"ANTENNA1 = " + sys.argv[4] + " AND ANTENNA2 = " + sys.argv[5] + \
	" AND DATA_DESC_ID = " + subband )
table34 = msds.openTable( tablename );

msds.setFilter( "TIME," + data_name1 + ',' + data_name2, \
	"ANTENNA1 = " + sys.argv[2] + " AND ANTENNA2 = " + sys.argv[4] + \
	" AND DATA_DESC_ID = " + subband )
table13 = msds.openTable( tablename );

msds.setFilter( "TIME," + data_name1 + ',' + data_name2, \
	"ANTENNA1 = " + sys.argv[3] + " AND ANTENNA2 = " + sys.argv[5] + \
	" AND DATA_DESC_ID = " + subband )
table24 = msds.openTable( tablename );

# get times
time_col = table12.getColumn("TIME")
time = time_col.data()
time = time/(24*3600)    # convert from MJD in seconds to days

# get data
data_col12_1 = table12.getColumn(data_name1)
data12_1 = data_col12_1.data()
data_col34_1 = table34.getColumn(data_name1)
data34_1 = data_col34_1.data()
data_col13_1 = table13.getColumn(data_name1)
data13_1 = data_col13_1.data()
data_col24_1 = table24.getColumn(data_name1)
data24_1 = data_col24_1.data()
data_col12_2 = table12.getColumn(data_name1)
data12_2 = data_col12_2.data()
data_col34_2 = table34.getColumn(data_name1)
data34_2 = data_col34_2.data()
data_col13_2 = table13.getColumn(data_name1)
data13_2 = data_col13_2.data()
data_col24_2 = table24.getColumn(data_name1)
data24_2 = data_col24_2.data()
nchannels = data12_1.shape[1] # second element of the data shape i.e. (nrows,256,4)

# calculate phases
if quantity_plot == 'channel':
	data12_reduce_1 = add.reduce(array=data12_1[:,range_plot,:],axis=1)/len(range_plot)
	data34_reduce_1 = add.reduce(array=data34_1[:,range_plot,:],axis=1)/len(range_plot)
	data13_reduce_1 = add.reduce(array=data13_1[:,range_plot,:],axis=1)/len(range_plot)
	data24_reduce_1 = add.reduce(array=data24_1[:,range_plot,:],axis=1)/len(range_plot)
	data12_reduce_2 = add.reduce(array=data12_2[:,range_plot,:],axis=1)/len(range_plot)
	data34_reduce_2 = add.reduce(array=data34_2[:,range_plot,:],axis=1)/len(range_plot)
	data13_reduce_2 = add.reduce(array=data13_2[:,range_plot,:],axis=1)/len(range_plot)
	data24_reduce_2 = add.reduce(array=data24_2[:,range_plot,:],axis=1)/len(range_plot)
elif quantity_plot == 'time':
	data12_reduce_1 = add.reduce(array=data12_1[range_plot,:,:],axis=0)/len(range_plot)
	data34_reduce_1 = add.reduce(array=data34_1[range_plot,:,:],axis=0)/len(range_plot)
	data13_reduce_1 = add.reduce(array=data13_1[range_plot,:,:],axis=0)/len(range_plot)
	data24_reduce_1 = add.reduce(array=data24_1[range_plot,:,:],axis=0)/len(range_plot)
	data12_reduce_2 = add.reduce(array=data12_2[range_plot,:,:],axis=0)/len(range_plot)
	data34_reduce_2 = add.reduce(array=data34_2[range_plot,:,:],axis=0)/len(range_plot)
	data13_reduce_2 = add.reduce(array=data13_2[range_plot,:,:],axis=0)/len(range_plot)
	data24_reduce_2 = add.reduce(array=data24_2[range_plot,:,:],axis=0)/len(range_plot)

ampl12_1 = hypot((data12_reduce_1[:,pol]).real,(data12_reduce_1[:,pol]).imag)
ampl34_1 = hypot((data34_reduce_1[:,pol]).real,(data34_reduce_1[:,pol]).imag)
ampl13_1 = hypot((data13_reduce_1[:,pol]).real,(data13_reduce_1[:,pol]).imag)
ampl24_1 = hypot((data24_reduce_1[:,pol]).real,(data24_reduce_1[:,pol]).imag)
ampl12_2 = hypot((data12_reduce_2[:,pol]).real,(data12_reduce_2[:,pol]).imag)
ampl34_2 = hypot((data34_reduce_2[:,pol]).real,(data34_reduce_2[:,pol]).imag)
ampl13_2 = hypot((data13_reduce_2[:,pol]).real,(data13_reduce_2[:,pol]).imag)
ampl24_2 = hypot((data24_reduce_2[:,pol]).real,(data24_reduce_2[:,pol]).imag)
if pol2:
	ampl12_1_2 = hypot((data12_reduce_1[:,pol2]).real,(data12_reduce_1[:,pol2]).imag)
	ampl34_1_2 = hypot((data34_reduce_1[:,pol2]).real,(data34_reduce_1[:,pol2]).imag)
	ampl13_1_2 = hypot((data13_reduce_1[:,pol2]).real,(data13_reduce_1[:,pol2]).imag)
	ampl24_1_2 = hypot((data24_reduce_1[:,pol2]).real,(data24_reduce_1[:,pol2]).imag)
	ampl12_2_2 = hypot((data12_reduce_2[:,pol2]).real,(data12_reduce_2[:,pol2]).imag)
	ampl34_2_2 = hypot((data34_reduce_2[:,pol2]).real,(data34_reduce_2[:,pol2]).imag)
	ampl13_2_2 = hypot((data13_reduce_2[:,pol2]).real,(data13_reduce_2[:,pol2]).imag)
	ampl24_2_2 = hypot((data24_reduce_2[:,pol2]).real,(data24_reduce_2[:,pol2]).imag)

closure_1 = (ampl12_1*ampl34_1)/(ampl13_1*ampl24_1)
closure_2 = (ampl12_2*ampl34_2)/(ampl13_2*ampl24_2)
if pol2:
	closure_1_2 = (ampl12_1_2*ampl34_1_2)/(ampl13_1_2*ampl24_1_2)
	closure_2_2 = (ampl12_2_2*ampl34_2_2)/(ampl13_2_2*ampl24_2_2)

closure_diff = closure_1 - closure_2
if pol2:  closure_diff_2 = closure_1_2 - closure_2_2

# if the optional channel argument is present
#  plot for this channel/time
if (range_plot != -1):
	if quantity_plot == 'channel':
		# plot data of given data vs. time
		subplot(211)
		plot( time, closure_diff, "," )
		if pol2: plot( time, closure_diff_2, "," )
		title("Time vs. Closure Amplitude, Baseline " + \
		      sys.argv[2] + '-' + sys.argv[3] + '-' + sys.argv[4] +  '-' + sys.argv[5] + ", Sub-band(" + subband +
		      ") " + " Chan0(" + str(range_plot[0]) + ")\n nchan(" + str(len(range_plot)) + "), " + sys.argv[1] )
		subplot(212)
		plot( time, closure_1, ",", time, closure_2, "," )
		if pol2: plot( time, closure_1_2, ",", time, closure_2_2, "," )
		xlabel("Time (MJD)")

	elif quantity_plot == 'time':
		# plot intensity of given data vs. channel
		subplot(211)
		plot( closure_diff, "," )
		if pol2:  plot( closure_diff_2, "," )
		title("Channel vs. Closure Amplitude, Baseline " + \
		      sys.argv[2] + '-' + sys.argv[3] + '-' + sys.argv[4] +  '-' + sys.argv[5] + ", Sub-band(" + subband +
		      ") " + " Time(" + str(time[range_plot[0]]) + " MJD)\n dtime(" + str(len(range_plot)) + "), " + sys.argv[1] )
		subplot(212)
		plot( closure_1, ",", closure_2, ",")
		xlabel("Channel")

## This is broken after adding the data averaging ##
#
# otherwise, plot all channels/times
#else:
#	if quantity_plot == 'channel':
#		# plot intensity of each channel vs. time
#		for channel in range( nchannels ):
#			plot( closure[channel,:], "," )
#			if pol2: plot( closure_2[channel,:], "," )
#			title("Time vs. Closure Amplitude, Baseline " + \
#			      sys.argv[2] + '-' + sys.argv[3] + '-' + sys.argv[4] +  '-' + sys.argv[5] + ", Sub-band(" + subband +
#			      ") " + str(data12.shape[1]) + " channels" + '\n' + sys.argv[1] )
#		xlabel("Time (MJD)")
#	if quantity_plot == 'time':
#		# plot intensity at each time vs. channel
#		for t in range( len(time) ):
#			plot( closure[:,t], "," )
#			if pol2:  plot( closure_2[:,t], "," )
#			title("Time vs. Closure Amplitude, Baseline " + \
#			      sys.argv[2] + '-' + sys.argv[3] +  '-' + sys.argv[4] +  '-' + sys.argv[5] + ", Sub-band(" + subband +
#			      ") " + str(len(time)) + " times" + '\n' + sys.argv[1] )
#		xlabel("Channel")

ylabel("Closure Amplitude")
show()



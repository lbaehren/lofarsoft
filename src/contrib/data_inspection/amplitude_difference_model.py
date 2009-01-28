#! /usr/bin/env python

import sys
import pydal as dal
from pylab import *
from scipy import *
import numpy

# check usage
if len(sys.argv) < 4 or len(sys.argv) > 8:
	print "Usage:"
	print "\tamplitude_difference_model.py <file> <antenna1-1> <antenna1-2> " + \
	      "[sub-band] ['channel' or 'time'] [channel/time range] " + \
	      "[polarization (0-3)]"
	print "\t<> required"
	print "\t E.g., amplitude_difference.py /lifs006/SB3.MS 1 13 0 channel 10:20 6"
	print "\tSingle or range of channels and times for averaging (in relative bin coordinates; ranges colon-delimited);"
	print "\tPolarization 0-3 plots individually, 4 plots xx and xy, 5 plots xx and yx, 6 plots xx and yy."
	print "\t-1 means plot all.  Default plots the amplitude difference between model and data, "
	print "for the xx for all channels as function of time for subband 0."
	print "\tNote that antenna numbers must be given in order from lowest to highest."
	print ""
	sys.exit(1)

# To do:  Add plot of elevation of Cas A, Cyg A, etc.

# other customizations
data_name1 = "DATA"
data_name2 = "CORRECTED_DATA"

# set default values
if len(sys.argv) < 5:  subband = str(0)
else: subband = sys.argv[4]
if len(sys.argv) < 6:  quantity_plot = 'channel'
else: quantity_plot = sys.argv[5]
if len(sys.argv) < 7:  range_plot = [0]
elif len((sys.argv[6]).split(':')) > 1:
	range_plot = range(int((sys.argv[6]).split(':')[0]),int((sys.argv[6]).split(':')[1]))
else: range_plot = [int(sys.argv[6])]
if len(sys.argv) < 8:  pol = 0
else:	pol = int(sys.argv[7])

# open file
msds= dal.dalDataset()
if ( True != msds.open(sys.argv[1]) ):
	sys.exit(1)

# open tables
tablename = "MAIN";
msds.setFilter( "TIME," + data_name1, \
	"ANTENNA1 = " + sys.argv[2] + " AND ANTENNA2 = " + sys.argv[3] + \
	" AND DATA_DESC_ID = " + subband )
table1 = msds.openTable( tablename );

msds.setFilter( "TIME," + data_name2, \
	"ANTENNA1 = " + sys.argv[2] + " AND ANTENNA2 = " + sys.argv[3] + \
	" AND DATA_DESC_ID = " + subband )
table2 = msds.openTable( tablename );

# get times
time_col = table1.getColumn("TIME")
time = time_col.data()

# get data
data_col1 = table1.getColumn(data_name1)
data1 = data_col1.data()
data_col2 = table2.getColumn(data_name2)
data2 = data_col2.data()
nchannels = data1.shape[1] # second element of the data shape i.e. (nrows,256,4)

# reduce data
if quantity_plot == 'channel':
	data1_reduce = add.reduce(array=data1[:,range_plot,:],axis=1)/len(range_plot)
	data2_reduce = add.reduce(array=data2[:,range_plot,:],axis=1)/len(range_plot)
elif quantity_plot == 'time':
	data1_reduce = add.reduce(array=data1[range_plot,:,:],axis=0)/len(range_plot)
	data2_reduce = add.reduce(array=data2[range_plot,:,:],axis=0)/len(range_plot)

# calculate normalized amplitudes
ampl1 = hypot((data1_reduce[:,pol]).real,(data1_reduce[:,pol]).imag)
ampl2 = hypot((data2_reduce[:,pol]).real,(data2_reduce[:,pol]).imag)
trimmean = where(ampl1-median(ampl1) < 3*std(ampl1-median(ampl1)))
ampl1 = (ampl1/mean(ampl1[trimmean]))
ampl2 = (ampl2/mean(ampl2[trimmean]))

#ratio = ampl1/ampl2
difference = (ampl1-ampl2)#/(ampl1+ampl2)
#print 'Mean of trimmed (ampl1,ampl2,diff) = ', mean(ampl1[trimmean]), mean(ampl2[trimmean]), mean(difference[trimmean])

figure(1)
# if the optional channel argument is present
#  plot for this channel/time range average
if (range_plot != -1):
	if quantity_plot == 'channel':
		# plot data of given data vs. time
		subplot(211)
#		plot( difference, ",")
		plot( time, difference, ",")
		ylabel("Amplitude Difference")
		title("Time vs. Amplitude Difference, Baselines " + \
		      sys.argv[2] + '-' + sys.argv[3] + ", Sub-band(" + subband +
		      ") " + " Chan0(" + str(range_plot[0]) + ")\n nchan(" + str(len(range_plot)) + "), " + sys.argv[1] )
		subplot(212)
#		plot( ampl1, "," , ampl2, ",")
		plot( time, ampl1, "," ,time, ampl2, ",")
		xlabel("Time (s)")
		ylabel("Amplitude")

	elif quantity_plot == 'time':
		# plot intensity of given data vs. channel
		subplot(211)
		plot( difference, "," )
		title("Channel vs. Amplitude Difference, Baselines " + \
		      sys.argv[2] + '-' + sys.argv[3] + ", Sub-band(" + subband +
		      ") " + " Time(" + str(time[range_plot[0]]) + ")\n dtime(" + str(len(range_plot)) + " ints), " + sys.argv[1] )
		subplot(212)
		plot( ampl1, "," , ampl2, ",")
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
#			title("Time vs.Amplitude Difference , Baselines " + \
#			      sys.argv[2] + '-' + sys.argv[3] + '/' + subband +  '-' + sys.argv[5] + ", Sub-band(" + sys.argv[6] +
#			      ") " + str(data12.shape[1]) + " channels" + '\n' + sys.argv[1] )
#		xlabel("Time (s)")
#	if quantity_plot == 'time':
#		# plot intensity at each time vs. channel
#		for t in range( len(time) ):
#			plot( difference[:,t], "," )
#			title("Time vs. Amplitude Difference, Baselines " + \
#			      sys.argv[2] + '-' + sys.argv[3] +  '/' + subband +  '-' + sys.argv[5] + ", Sub-band(" + sys.argv[6] +
#			      ") " + str(len(time)) + " times" + '\n' + sys.argv[1] )
#		xlabel("Channel")

show()



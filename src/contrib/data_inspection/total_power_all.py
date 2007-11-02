#! /usr/bin/env python

import sys
import pydal as dal
from pylab import *
from numpy import *
from scipy import *

def func(real,imag,plottype):
# is there a better way to do this?
	if plottype == 'a':
		return array(abs(real,imag))
	elif plottype == 'p':
		return array(arctan(real,imag))

# check usage
if len(sys.argv) < 2 or len(sys.argv) > 6:
	print "Usage:"
	print "\ttotal_power_all.py <file> [subband] [antenna] ['channel' or 'time'] [channel/time]"
	print "\t<> required"
	print "\tE.g., total_power_all.py /lifs006/SB3.MS 0 0 channel 0"
	print "\tDefault plots the total power for eight (internally specified) antennas in first subband in channel 0"
	print ""
	sys.exit(1)

# other customizations
data_name = "DATA"
#antenna_numbers = range(16)
antenna_numbers = [0,4,8,9,10,11,12,13]   # a nice subset of 8, including four from an outer station

# set default values
if len(sys.argv) < 3:  subband = 0
else: subband = int(sys.argv[2])
if len(sys.argv) < 4:  antenna = -1
else: antenna = int(sys.argv[3])
if len(sys.argv) < 5:  quantity_plot = 'channel'
else: quantity_plot = sys.argv[4]
if len(sys.argv) < 6: range_plot = 0
else: range_plot = int(sys.argv[5])

# open file
msds= dal.dalDataset()
if ( msds.open(sys.argv[1]) ):
	sys.exit(1)

# open table with filter set to get autocorrelations
tablename = "MAIN";
if antenna == -1:
	filter = "(ANTENNA1 = " + str(antenna_numbers[0]) + " AND ANTENNA2 = " + str(antenna_numbers[0]) + ")"
	for i in antenna_numbers[1:]:
		filter = filter + " OR (ANTENNA1 = " + str(i) + " AND ANTENNA2 = " + str(i) + ")"
	filter = filter + " AND DATA_DESC_ID = " + str(subband)
elif antenna != -1:
	filter = "ANTENNA1 = " + str(antenna) + " AND ANTENNA2 = " + str(antenna) + \
		" AND DATA_DESC_ID = " + str(subband)
msds.setFilter( "TIME,ANTENNA1,ANTENNA2," + data_name, filter)
maintable = msds.openTable( tablename );

# get times into numpy arrays
time_col = maintable.getColumn("TIME")
time = array(time_col.data())

# get antennas into numpy arrays
ant1_col = maintable.getColumn("ANTENNA1")
ant1 = array(ant1_col.data())
ant2_col = maintable.getColumn("ANTENNA2")
ant2 = array(ant2_col.data())

# get data into numpy arrays
data_col = maintable.getColumn(data_name)
data = array(data_col.data())
nchannels = data.shape[1] # second element of the data shape i.e. (4,256,nrows)

# set range of antennas to iterate over for plotting
if antenna == -1:
	antenna_range = antenna_numbers
elif antenna != -1:
	antenna_range = [antenna]

# open figure and set title
figure(1)
if quantity_plot == 'channel':
	title_string = "Time vs. Autocorrelation, Antenna=" + str(antenna) + ", Sub-band(" + str(subband) + "), " + " Channel(" + str(range_plot) + ")\n" + sys.argv[1]
elif quantity_plot == 'time':
	title_string = "Channel vs. Autocorrelation, Antenna=" + str(antenna) + ", Sub-band(" + str(subband) + "), " + " Time(" + str(time[range_plot]) + ")\n" + sys.argv[1] 

# loop over antennas, set data and plot each autocorrelation in different panel
for i in antenna_range:
	#  plot autocorrelations for one channel
	if quantity_plot == 'channel':
		# plot data only for one antenna in each subplot
		antenna_indices = where(ant1 == i)[0]
		curve0 = (hypot((data[0,range_plot,:]).real,(data[0,range_plot,:]).imag))[antenna_indices]
		curve1 = (hypot((data[1,range_plot,:]).real,(data[1,range_plot,:]).imag))[antenna_indices]
		curve2 = (hypot((data[2,range_plot,:]).real,(data[2,range_plot,:]).imag))[antenna_indices]
		curve3 = (hypot((data[3,range_plot,:]).real,(data[3,range_plot,:]).imag))[antenna_indices]
		# if multiple antennas, plot in two columns.  assumes even number of antennas
		if len(antenna_range) == 1:  ax = subplot(1,1,1)
		elif len(antenna_range) > 1:  ax = subplot(len(antenna_range)/2,2,antenna_range.index(i)+1)
		plot( time[antenna_indices], curve0, ",", time[antenna_indices], curve1, ",", time[antenna_indices], curve2, ",", time[antenna_indices], curve3, "," )

		# make axis labels and title look nice
		text(time[len(time)/2],mean(curve0)/2.,'ant'+str(i))
		if ax.is_last_row(): xlabel("Time (s)")
		if ax.is_first_col(): ylabel("Intensity")
		if ax.is_first_row() and ax.is_first_col(): title(title_string)
	#  plot autocorrelations for one time
	elif quantity_plot == 'time':
		antenna_data = data[:,:,where(ant1 == i)[0]]
		curve0 = hypot(array((antenna_data[0,:,range_plot]).real),array((antenna_data[0,:,range_plot]).imag))
		curve1 = hypot(array((antenna_data[1,:,range_plot]).real),array((antenna_data[1,:,range_plot]).imag))
		curve2 = hypot(array((antenna_data[2,:,range_plot]).real),array((antenna_data[2,:,range_plot]).imag))
		curve3 = hypot(array((antenna_data[3,:,range_plot]).real),array((antenna_data[3,:,range_plot]).imag))
		if len(antenna_range) == 1:  ax = subplot(1,1,1)
		elif len(antenna_range) > 1:  ax = subplot(len(antenna_range)/2,2,antenna_range.index(i)+1)
		plot( curve0, ",", curve1, ",", curve2, ",", curve3, "," )
		# make axis labels and title look nice
		text(len(curve0)/2.,mean(curve0)/2.,'ant'+str(i))
		if ax.is_last_row(): xlabel("Channel")
		if ax.is_first_col(): ylabel("Intensity")
		if ax.is_first_row() and ax.is_first_col(): title(title_string)

show()

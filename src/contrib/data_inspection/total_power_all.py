#! /usr/bin/env python
#
# Casey Law, 13nov07
# A data validation script for LOFAR/CS1

import sys
import pydal as dal
from pylab import *
from numpy import *
from scipy import *

# check usage
if len(sys.argv) < 2 or len(sys.argv) > 6:
	print "Usage:"
	print "\ttotal_power_all.py <file> [subband] [antenna] ['channel' or 'time'] [channel/time]"
	print "\t<> required"
	print "\tE.g., total_power_all.py /lifs006/SB3.MS 0 0 channel 0"
	print "\tDefault plots the total power for eight (internally specified) antennas in first subband in central channel"
	print ""
	sys.exit(1)

# other customizations
plots_per_page = 4      # probably needs to be even number
antenna_numbers = range(16)
data_name = "DATA"

# try to find MS name from input string.  later used to name output plots
if len((sys.argv[1]).split('/')[-1]) > 0: msname = (sys.argv[1]).split('/')[-1]
else: msname = (sys.argv[1]).split('/')[-2]   # in this case, last character is '/'

# set default values
if len(sys.argv) < 3:  subband = 0
else: subband = int(sys.argv[2])
if len(sys.argv) < 4:  antenna = -1
else: antenna = int(sys.argv[3])
if len(sys.argv) < 5:  quantity_plot = 'channel'
else: quantity_plot = sys.argv[4]
if len(sys.argv) < 6: range_plot = -1      # default value set later to nchannels/2
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
time = time/(24*3600)    # convert from MJD in seconds to days

# get antennas into numpy arrays
ant1_col = maintable.getColumn("ANTENNA1")
ant1 = array(ant1_col.data())
ant2_col = maintable.getColumn("ANTENNA2")
ant2 = array(ant2_col.data())

# get data into numpy arrays
data_col = maintable.getColumn(data_name)
data = array(data_col.data())
nchannels = data.shape[1]     # second element of the data shape i.e. (4,256,nrows)
if range_plot == -1: range_plot = nchannels/2     # set default to some middle channel

# set range of antennas to iterate over for plotting
if antenna == -1:
	antenna_range = antenna_numbers
elif antenna != -1:
	antenna_range = [antenna]

# set title
if quantity_plot == 'channel':
	title_string = "Time vs. Autocorrelation, Antenna=" + str(antenna) + ", Sub-band(" + str(subband) + "), " + " Channel(" + str(range_plot) + ")\n" + sys.argv[1]
	print 'Plot for ' + msname + ':  subband ' + str(subband) + ', channel ' + str(range_plot)
elif quantity_plot == 'time':
	title_string = "Channel vs. Autocorrelation, Antenna=" + str(antenna) + ", Sub-band(" + str(subband) + "), " + " Time(" + str(time[range_plot]) + " MJD)\n" + sys.argv[1] 
	print 'Plot for ' + msname + ':  subband ' + str(subband) + ', time ' + str(time[range_plot]) + ' MJD'

# loop over antennas, set data and plot each autocorrelation in different panel
for i in antenna_range:
	if antenna_range.index(i)%plots_per_page == 0:
		if i > 0:
			savefig('plot' + msname + '_' + str(antenna_range.index(i)/plots_per_page) + '.jpg')
			close()
			print 'saved figure as ' + 'plot' + msname + '_' + str(antenna_range.index(i)/plots_per_page) + '.jpg'
		fig = figure(antenna_range.index(i)/plots_per_page+1)
		axis('off')
		title(title_string)      # plot title on each new figure
	#  plot autocorrelations for one channel
	print 'Plotting antenna ' + str(i)
	if quantity_plot == 'channel':
		# plot data only for one antenna in each subplot
		antenna_indices = where(ant1 == i)[0]
		curve0 = (hypot((data[0,range_plot,:]).real,(data[0,range_plot,:]).imag))[antenna_indices]
		curve1 = (hypot((data[1,range_plot,:]).real,(data[1,range_plot,:]).imag))[antenna_indices]
		curve2 = (hypot((data[2,range_plot,:]).real,(data[2,range_plot,:]).imag))[antenna_indices]
		curve3 = (hypot((data[3,range_plot,:]).real,(data[3,range_plot,:]).imag))[antenna_indices]
		# if multiple antennas, plot in two columns.  assumes even number of antennas
		if len(antenna_range) == 1:  ax = fig.add_subplot(1,1,1)
		elif len(antenna_range) > 1:  ax = fig.add_subplot(plots_per_page/2,2,antenna_range.index(i)%plots_per_page+1)
		plot( time[antenna_indices], curve0, ",", label='xx')
		plot( time[antenna_indices], curve1, ",", label='xy')
		plot( time[antenna_indices], curve2, ",", label='yx')
		plot( time[antenna_indices], curve3, ",", label='yy')

		# make axis labels and title look nice
		text(time[len(time)/2],mean(curve0)/2.,'ant'+str(i))
		if ax.is_last_row(): xlabel("Time (MJD)")
		if ax.is_first_col(): ylabel("Intensity")
		if ax.is_first_row() and ax.is_first_col():
			legend(('xx','xy','yx','yy'),4,numpoints=4)
	#  plot autocorrelations for one time
	elif quantity_plot == 'time':
		antenna_data = data[:,:,where(ant1 == i)[0]]
		curve0 = hypot(array((antenna_data[0,:,range_plot]).real),array((antenna_data[0,:,range_plot]).imag))
		curve1 = hypot(array((antenna_data[1,:,range_plot]).real),array((antenna_data[1,:,range_plot]).imag))
		curve2 = hypot(array((antenna_data[2,:,range_plot]).real),array((antenna_data[2,:,range_plot]).imag))
		curve3 = hypot(array((antenna_data[3,:,range_plot]).real),array((antenna_data[3,:,range_plot]).imag))
		if len(antenna_range) == 1:  ax = fig.add_subplot(1,1,1)
		elif len(antenna_range) > 1:  ax = fig.add_subplot(plots_per_page/2,2,antenna_range.index(i)%plots_per_page+1)
		plot( curve0, ",", curve1, ",", curve2, ",", curve3, "," )
		# make axis labels and title look nice
		text(len(curve0)/2.,mean(curve0)/2.,'ant'+str(i))
		if ax.is_last_row(): xlabel("Channel")
		if ax.is_first_col(): ylabel("Intensity")
		if ax.is_first_row() and ax.is_first_col():
			legend(('xx','xy','yx','yy'),4,numpoints=4)

savefig('plot' + msname + '_' + str((antenna_range.index(i)+1)/plots_per_page) + '.jpg')
close()
print 'saved figure as ' + 'plot' + msname + '_' + str((antenna_range.index(i)+1)/plots_per_page) + '.jpg'

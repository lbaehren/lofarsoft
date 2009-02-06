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
	print "\thistogram_all.py <file> [subband] [antenna] ['channel' or 'time'] [channel/time]"
	print "\t<> required"
	print "\tE.g., histogram_all.py /lifs006/SB3.MS 0 0 channel 0"
	print "\tDefault plots the histogram of data amplitude for all antennas in first subband in central channel"
	print ""
	sys.exit(1)

# other customizations
nbins = 100
plots_per_page = 4      # probably needs to be even number
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
if ( True != msds.open(sys.argv[1]) ):
	print "ERROR: Could not open file: " + sys.argv[1]
	print "       Please check the file and try again."
	sys.exit(1)

# get antenna names
anttab = msds.openTable('ANTENNA')
namecol = anttab.getColumn('NAME')
name = namecol.data()
num_ants = len(name)
antenna_numbers = range(num_ants)

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
ant1 = ant1_col.data()
ant2_col = maintable.getColumn("ANTENNA2")
ant2 = ant2_col.data()

# get data into numpy arrays
data_col = maintable.getColumn(data_name)
data = data_col.data()
nchannels = data.shape[1]     # second element of the data shape i.e. (4,256,nrows)
if range_plot == -1 and quantity_plot == 'channel': range_plot = nchannels/2     # set default to some middle channel

# set range of antennas to iterate over for plotting
if antenna == -1:
	antenna_range = antenna_numbers
elif antenna != -1:
	antenna_range = [antenna]

# set title
if quantity_plot == 'channel':
	title_string = "Autocorrelation Histogram for 1 Channel, Antenna=" + str(antenna) + ", Sub-band(" + str(subband) + "), " + " Channel(" + str(range_plot) + ")\n" + sys.argv[1]
	print 'Plot for ' + msname + ':  subband ' + str(subband) + ', channel ' + str(range_plot)
elif quantity_plot == 'time':
	title_string = "Autocorrelation Histogram for 1 Time, Antenna=" + str(antenna) + ", Sub-band(" + str(subband) + "), " + " Time(" + str(time[range_plot]) + " MJD)\n" + sys.argv[1] 
	print 'Plot for ' + msname + ':  subband ' + str(subband) + ', time ' + str(time[range_plot]) + ' MJD'

# loop over antennas, set data and plot each autocorrelation in different panel
for i in antenna_range:
	if antenna_range.index(i)%plots_per_page == 0:
		if i > 0:
			savefig('vv' + msname + '_' + str(antenna_range.index(i)/plots_per_page) + 'hist.png')
			close()
			print 'saved figure as ' + 'vv' + msname + '_' + str(antenna_range.index(i)/plots_per_page) + 'hist.png'
		fig = figure(antenna_range.index(i)/plots_per_page+1)
		axis('off')
		title(title_string)      # plot title on each new figure
	#  plot autocorrelations for one channel
	print 'Plotting antenna ' + str(i)
	if quantity_plot == 'channel':
		# plot data only for one antenna in each subplot
		antenna_indices = where(ant1 == i)[0]
		# if multiple antennas, plot in two columns.  assumes even number of antennas
		if len(antenna_range) == 1:  ax = fig.add_subplot(1,1,1)
		elif len(antenna_range) > 1:  ax = fig.add_subplot(plots_per_page/2,2,antenna_range.index(i)%plots_per_page+1)
		pdf0,bins0,patches0 = hist((hypot((data[:,range_plot,0]).real,(data[:,range_plot,0]).imag)[antenna_indices]),bins=nbins,normed=0)
		pdf1,bins1,patches1 = hist((hypot((data[:,range_plot,1]).real,(data[:,range_plot,1]).imag)[antenna_indices]),bins=nbins,normed=0)
		pdf2,bins2,patches2 = hist((hypot((data[:,range_plot,2]).real,(data[:,range_plot,2]).imag)[antenna_indices]),bins=nbins,normed=0)
		pdf3,bins3,patches3 = hist((hypot((data[:,range_plot,3]).real,(data[:,range_plot,3]).imag)[antenna_indices]),bins=nbins,normed=0)
		cla()
		plot( bins0, pdf0, label='xx')
		plot( bins1, pdf1, label='xy')
		plot( bins2, pdf2, label='yx')
		plot( bins3, pdf3, label='yy')

		# make axis labels and title look nice
		text(0.5,0.8,'ant'+str(i),transform = ax.transAxes)
		if ax.is_last_row(): xlabel("Amplitude")
		if ax.is_first_col(): ylabel("Number")
		if ax.is_first_row() and ax.is_first_col():
			legend(('xx','xy','yx','yy'),1,numpoints=4)
	#  plot autocorrelations for one time
	elif quantity_plot == 'time':
		antenna_data = data[where(ant1 == i)[0],:,:]
		if len(antenna_range) == 1:  ax = fig.add_subplot(1,1,1)
		elif len(antenna_range) > 1:  ax = fig.add_subplot(plots_per_page/2,2,antenna_range.index(i)%plots_per_page+1)
		pdf0,bins0,patches0 = hist((hypot((antenna_data[range_plot,:,0]).real,(antenna_data[range_plot,:,0]).imag)),bins=nbins,normed=0)
		pdf1,bins1,patches1 = hist((hypot((antenna_data[range_plot,:,1]).real,(antenna_data[range_plot,:,1]).imag)),bins=nbins,normed=0)
		pdf2,bins2,patches2 = hist((hypot((antenna_data[range_plot,:,2]).real,(antenna_data[range_plot,:,2]).imag)),bins=nbins,normed=0)
		pdf3,bins3,patches3 = hist((hypot((antenna_data[range_plot,:,3]).real,(antenna_data[range_plot,:,3]).imag)),bins=nbins,normed=0)
		cla()
		plot( bins0, pdf0, label='xx')
		plot( bins1, pdf1, label='xy')
		plot( bins2, pdf2, label='yx')
		plot( bins3, pdf3, label='yy')
		# make axis labels and title look nice
		text(0.5,0.8,'ant'+str(i),transform = ax.transAxes)
		if ax.is_last_row(): xlabel("Amplitude")
		if ax.is_first_col(): ylabel("Number")
		if ax.is_first_row() and ax.is_first_col():
			legend(('xx','xy','yx','yy'),4,numpoints=4)

savefig('vv' + msname + '_' + str((antenna_range.index(i)+1)/plots_per_page) + 'hist.png')
close()
print 'saved figure as ' + 'vv' + msname + '_' + str((antenna_range.index(i)+1)/plots_per_page) + 'hist.png'
clf()

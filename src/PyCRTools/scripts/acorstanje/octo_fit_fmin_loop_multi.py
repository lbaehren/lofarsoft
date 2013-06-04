# Test script to get relative arrival times of pulses from the octocopter data with a loop over the blocks.
# Started by Arthur Corstanje, Oct 2012.
# Added by Maria Krause, April 2013.
# At the end, the script tries to fit the curve of the distance to each antenna with the fmin function.
# The file 'octocopter_function' needs to be in the same directory
import matplotlib
#matplotlib.use('Agg')
import pylab
pylab.ion()
import matplotlib.pyplot as plt
import pycrtools as cr
import numpy as np
np.set_printoptions(precision=20)
from numpy import *
import math

from pycrtools.tasks import crosscorrelateantennas, pulsecal
import os
from matplotlib import rc, rcParams
import scipy
from scipy import constants
from scipy.optimize import fmin

from pylab import *
latex_preamble = "\usepackage{cmbright}" # Use sans-serif (math)fonts
#latex_preamble = "\usepackage{cmbright}\Huge" # Use sans-serif (math)fonts
rcParams['text.usetex'] = True
rcParams['text.latex.preamble'] = latex_preamble
rcParams.update({'font.size': 30})

import sys
import os
sys.path.append(os.environ["LOFARSOFT"] + "/src/PyCRTools/scripts/")

import octocopter_function as of

def locateFirstMaximum(timeseries_cut_to_pulse, resampleFactor=64, debug=True):
    timeseries_resampled = cr.hArray(float, dimensions=(nofChannels, blocksize * resampleFactor))
    cr.hFFTWResample(timeseries_resampled[...], timeseries_cut_to_pulse[...])

    timeseries_resampled *= -1.0
    ampli = cr.hArray(timeseries_resampled[...].max())
    medianAmpli = ampli.median()[0]
    print 'Median amplitude = %4.1f' % medianAmpli
    thresholdcrossings = cr.hFindGreaterThan(timeseries_resampled[...], 0.2 * medianAmpli) # 0.2 * ampli
    maximumCutoutWidth = int(resampleFactor * 1.5)
    thisMaximumCutout = cr.hArray(float, dimensions=[nofChannels, maximumCutoutWidth])
    thisMaximumCutout[...].copy(timeseries_resampled[..., thresholdcrossings:thresholdcrossings+maximumCutoutWidth])
    timeOfMaximum = cr.hArray(float, [nofChannels])
    timeOfMaximum.copy(cr.hArray(thisMaximumCutout[...].maxpos()))
    timeOfMaximum += cr.hArray(thresholdcrossings) # NB. need to convert to hArray; adding vector to array doesn't work
    timeOfMaximum *= (5.0e-9 / resampleFactor)
    timeOfMaximum -= timeOfMaximum[0]

    if debug:
        #y = timeseries_cut_to_pulse.toNumpy()
        y = timeseries_resampled.toNumpy()
        plt.figure()
        plt.plot(y[52])
        plt.plot(y[0][1000:-1])
        #plt.plot(y[8][2000:-1])
        #plt.plot(y[8])
        #plt.plot(y[39])
        plt.figure()
        y = timeseries.toNumpy()
        plt.plot(y[52])
        plt.plot(y[0][1000:-1])
        #plt.plot(y[8][2000:-1])
        1/0

    return timeOfMaximum

"""The octocopter timing file"""
filefilter = '/Users/acorstanje/triggering/CR/octocopter_test/L65331_D20120919T094039.104Z_CS0[01][13]_R000_tbb.h5'
#filefilter = '/media/MyDrive/Masterarbeit/Calibration_data/Time_Measurements/Timing_Files_h5/data/L65336_D20120919T112033.709Z_CS003_R000_tbb.h5'
#filefilter = '/Users/acorstanje/triggering/CR/octocopter_test/L65331_D20120919T094039.104Z_CS003_R000_tbb.h5'

#Write all files into a list
#file_ar = os.listdir('/media/MyDrive/Masterarbeit/Calibration_data/Time_Measurements/Timing_Files_h5/data/')
#filenames = []
#for i in range(0,len(file_ar)):
#	filenames.append('/media/MyDrive/Masterarbeit/Calibration_data/Time_Measurements/Timing_Files_h5/data/{}'.format(file_ar[i]))

#filefilter = '/media/MyDrive/Masterarbeit/Calibration_data/Time_Measurements/Timing_Files_h5/data/L65343_D20120919T123204.519Z_CS004_R000_tbb.h5'
#L65325_D20120919T124336.486Z_CS002_R000_tbb.h5'
#L65331_D20120919T094039.104Z_CS003_R000_tbb.h5'
#L65333_D20120919T105520.902Z_CS002_R000_tbb.h5'
#L65336_D20120919T112033.709Z_CS002_R000_tbb.h5'
#L65343_D20120919T123204.519Z_CS003_R000_tbb.h5
#L65345_D20120919T120330.995Z_CS004_R000_tbb.h5

#filelist = cr.listFiles(filenames)
filelist = cr.listFiles(filefilter)
print filelist[0]
if len(filelist) == 1:
    filelist = filelist[0]
#print 'File is {}'.format(filelist)
blocksize = 2048
f = cr.open(filelist, blocksize = blocksize) # big blocks, then cut out pulses

f.setAntennaSelection('even')

###################################################################################
#	Get the position files of the antennas
###################################################################################
antennaSet = f['ANTENNA_SET'][0] if isinstance(f['ANTENNA_SET'], list) else f['ANTENNA_SET']
print 'The antenna set is %s' % antennaSet
"""The LBA or HBA antenna position file"""
if antennaSet=='LBA_OUTER':
	LBAfile = open(os.path.join(os.environ['LOFARSOFT'], 'data/lofar/antennapositions/LOFAR_ITRF_LBA_OUTER'),'r')
elif antennaSet=='LBA_INNER':
	LBAfile = open(os.path.join(os.environ['LOFARSOFT'], 'data/lofar/antennapositions/LOFAR_ITRF_LBA_INNER'),'r')
elif antennaSet=='LBA_X':
	LBAfile = open(os.path.join(os.environ['LOFARSOFT'], 'data/lofar/antennapositions/LOFAR_ITRF_LBA_X'),'r')
elif antennaSet=='LBA_Y':
	LBAfile = open(os.path.join(os.environ['LOFARSOFT'], 'data/lofar/antennapositions/LOFAR_ITRF_LBA_Y'),'r')
elif antennaSet=='HBA_DUAL':
	LBAfile = open(os.path.join(os.environ['LOFARSOFT'], 'data/lofar/antennapositions/LOFAR_ITRF_HBA_DUAL'),'r')
elif antennaSet=='HBA_JOINED':
	LBAfile = open(os.path.join(os.environ['LOFARSOFT'], 'data/lofar/antennapositions/LOFAR_ITRF_HBA_JOINED'),'r')
elif antennaSet=='HBA_ONE':
	LBAfile = open(os.path.join(os.environ['LOFARSOFT'], 'data/lofar/antennapositions/LOFAR_ITRF_HBA_ONE'),'r')
elif antennaSet=='HBA_ZERO':
	LBAfile = open(os.path.join(os.environ['LOFARSOFT'], 'data/lofar/antennapositions/LOFAR_ITRF_HBA_ZERO'),'r')

###################################################################################
#	Ask for which antenna station is used: CS002, CS003, CS004, CS005 or CS006
###################################################################################
station_number = int(raw_input('Which antenna station is used? Please specify: 2, 3, 4, 5 or 6! '))
print 'Thanks, you chosed %d.' %station_number
print ''
if (station_number == 2):
	x0_station = 3826577.10950		#east coordinate
	y0_station = 461022.90020		#north coordinate
	z0_station = 5064892.758
	antenna0_position = of.central_antenna_position(x0_station,y0_station,z0_station)
	east0_station = antenna0_position[0]
	north0_station = antenna0_position[1]
elif (station_number == 3):
	x0_station = 3826516.79150
	y0_station = 460930.01819
	z0_station = 5064946.42900
	antenna0_position = of.central_antenna_position(x0_station,y0_station,z0_station)
	east0_station = antenna0_position[0]
	north0_station = antenna0_position[1]
elif (station_number == 4):
	x0_station = 3826654.24051
	y0_station = 460939.52820
	z0_station = 5064842.39700
	antenna0_position = of.central_antenna_position(x0_station,y0_station,z0_station)
	east0_station = antenna0_position[0]
	north0_station = antenna0_position[1]
elif (station_number == 5):
	x0_station = 3826668.79350
	y0_station = 461069.50220
	z0_station = 5064819.72500
	antenna0_position = of.central_antenna_position(x0_station,y0_station,z0_station)
	east0_station = antenna0_position[0]
	north0_station = antenna0_position[1]
elif (station_number == 6):
	x0_station = 3826596.77349
	y0_station = 461145.13020
	z0_station = 5064866.95000
	antenna0_position = of.central_antenna_position(x0_station,y0_station,z0_station)
	east0_station = antenna0_position[0]
	north0_station = antenna0_position[1]

ID_array= []
X_array = []
Y_array = []
Z_array = []

for line in LBAfile:
	ID_array.append(str(line.split()[0]))
	X_array.append(float(line.split()[1]))
	Y_array.append(float(line.split()[2]))
	Z_array.append(float(line.split()[3]))
LBAfile.close()

f_sample = f['SAMPLE_FREQUENCY_VALUE']

# Get selected dipoles (station) and the observation time from the LOFAR HDF5 file
t_UTC = f['TIME']
ID = f['SELECTED_DIPOLES']

###################################################################################
#		The octocopter position files
###################################################################################

#octofile = open('/home/maria/Dokumente/Master_Thesis/Calibration_data/Time_Program/Octocopter_position_file.txt','r')
octofile = open('/Users/acorstanje/usg/src/PyCRTools/scripts/Octocopter_position_file.txt','r')
octofile.next() # Skip file header lines

Time_array = []
Lat_array = []
Lon_array = []
Ele_array = []
WGS_array = []

for line in octofile:
	Time_array.append(float(line.split()[0]))
 	Lat_array.append(float(line.split()[1]))
 	Lon_array.append(float(line.split()[2]))
 	Ele_array.append(float(line.split()[3]))
 	WGS_array.append(float(line.split()[4]))
octofile.close()
###################################################################################
#		Calculation of the geometrical time delay (octocopter at original position)
#
#		Conversion of ITRF to latitude/longitude to UTM coordinates of the antennas
###################################################################################
trafo_values = of.transformation(Time_array,t_UTC, Lat_array, Lon_array, Ele_array, WGS_array, ID_array, ID, X_array, Y_array, Z_array)

time_diff_exp = trafo_values[0]
north         = trafo_values[1]   #coordinates of antennas
east          = trafo_values[2]
height        = trafo_values[3]
northocto     = trafo_values[4]		#coordinates of octocopter
eastocto      = trafo_values[5]
hocto         = trafo_values[6]
north0        = trafo_values[7]		#coordinates of central station antenna
east0	      = trafo_values[8]
h0            = trafo_values[9]
distance      = trafo_values[10]

#selected_dipoles = [x for x in self.f["DIPOLE_NAMES"] if int(x) % 2 == self.pol]

# Read in calibration delays
try:
    cabledelays = cr.hArray(f["DIPOLE_CALIBRATION_DELAY"])
except Exception:
    raise StationSkipped("do not have DIPOLE_CALIBRATION_DELAY value")

nofChannels = f["NOF_SELECTED_DATASETS"]

subsampleOffsetsPerStation = f["SUBSAMPLE_CLOCK_OFFSET"] # numpy array

stationList = f["STATION_LIST"]
stationStartIndex = f["STATION_STARTINDEX"]
nofStations = len(stationStartIndex) - 1
subsampleOffsets = np.zeros(len(cabledelays))
for i in range(nofStations):
    start = stationStartIndex[i]
    end = stationStartIndex[i+1]
    subsampleOffsets[start:end] = subsampleOffsetsPerStation[i] # get them per antenna, makes it easy to correct

# ...Timeseries analysis starts here...
timeseries = f.empty("TIMESERIES_DATA")

nofblocks = 10
thisBlock = 0
lofar_delay_allblocks = np.zeros( (nofblocks, nofChannels) )
shift = 0
datablocknr = 0

while thisBlock < nofblocks:										#Loop over all blocks in that file
    datablocknr += int(shift / 2048)
    shift %= 2048
    print 'Doing block: %d, datablock = %d, shift = %d' % (thisBlock, datablocknr, shift)
    f.getTimeseriesData(timeseries, datablocknr, shift)
    #timeseries_cut_to_pulse[...].copy(timeseries[..., start:end])
    #timeseries_cut_to_pulse.square()

    timeOfMaximum = locateFirstMaximum(timeseries, debug=False if thisBlock == 0 else False)
    # Apply cable delays
    timeOfMaximum -= cabledelays
    timeOfMaximum -= cr.hArray(subsampleOffsets.tolist()) # sign...?

    if thisBlock == 0:
        #	Plot the original pulse
        y = timeseries.toNumpy()
        plt.figure(figsize=(12,8))
        ax = subplot(111)
        plt.subplots_adjust(left=0.12, right=0.98,top=0.96,bottom=0.11)
        #plt.xlabel('Sample Number')
        plt.xlabel('Time [$\mu s$]')
        plt.ylabel('Voltage [ADC]')
        #plt.title('Original Pulse')
        sample_time = []
        for i in range(0,len(y[2])):
                a = i
                time = a*5.0e-9
                sample_time.append(time)

        sample_time = np.array(sample_time)*1e6
        plt.xlim(3,8)
        plt.ylim(-2100,2100)
        plt.plot(np.array(sample_time),y[0], label = 'Antenna 0')
        plt.plot(np.array(sample_time),y[50], label = 'Antenna 48')
        plt.legend(prop={'size':10})
        leg = plt.gca().get_legend()
        ltext  = leg.get_texts()
        plt.setp(ltext, fontsize=28)
        plt.figure()

        # now cross correlate all channels in full_timeseries, get relative times
#        crosscorr = cr.trerun('CrossCorrelateAntennas', "crosscorr", timeseries_data=timeseries, oversamplefactor=16)

#        y = crosscorr.crosscorr_data.toNumpy()
#        plt.plot(y[0]) # will have maximum at 16 * blocksize / 2 (8192)
#        plt.figure()
#        plt.plot(y[2]) # will have a shifted maximum
#        plt.title('Shifted Maximum')

        #	And determine the relative offsets between them
#        sample_interval = 5.0e-9
#        maxima_cc = cr.trerun('FitMaxima', "Lags", crosscorr.crosscorr_data, doplot = True, plotend=5, sampleinterval = sample_interval / crosscorr.oversamplefactor, peak_width = 11, splineorder = 3, refant = 0)

    # Measured time delays from LOFAR system
    #maxima_cc.lags *= 1e9
#    maxima_cc.lags.copy(1e9 * timeOfMaximum.vec())
#    thisLofarDelay = cr.hArray(maxima_cc.lags).toNumpy()
    thisLofarDelay = 1e9 * timeOfMaximum.toNumpy()
    lofar_delay_allblocks[thisBlock] = thisLofarDelay

    thisBlock += 1
    shift += 1600
    # loop

#maxlags = maxima_cc.maxx
#maxlags= cr.hArray(maxlags).toNumpy()

plt.figure()
plt.title('Lofar delay all blocks')
for i in range(nofblocks):
    plt.plot(lofar_delay_allblocks[i], 'bo')

# Take the average over all pulses processed. Times from LOFAR system with all antennas (raw times): lofar_delay
lofar_delay = np.mean(lofar_delay_allblocks, axis=0)

###################################################################################
#		Fit the distance curve with fmin
###################################################################################

def sq_error_minimizer(p, x0,y0,z0,x1,y1,z1, times_from_lofar):
	# Quality function for simplex search
	A, B, C = p
	A0 = northocto
	B0 = eastocto
	C0 = hocto

# test starting at 0.0
	A += A0
	B += B0
	C += C0

	distance_to_center = (np.sqrt( (A-x0)**2+(B-y0)**2+(C-z0)**2))/(constants.c*1e-9)
	time_error = ((np.sqrt( (A-x1)**2+(B-y1)**2+(C-z1)**2))/(constants.c*1e-9) - distance_to_center)
	time_error -= times_from_lofar
	time_error -= np.median(time_error)
	#for i in range(0,len(time_error)):
	#	oneSampleShifts = int(round(time_error[i] / 5.)) # assuming time_error in nanoseconds
	#	time_error[i] -= 5.0 * oneSampleShifts
	if np.abs(A-A0)>10. or np.abs(B-B0)>10. or np.abs(C-C0)>10.:
		sq_error = 1.0e6
	# subtract 5-ns offsets NB! Assuming they are not data outliers but really one-sample shifts in time!
	else: # need to minimize square error (curve fit does this automatically)
		sq_error = (time_error)**2
		sq_error[np.argwhere(sq_error > 2.0**2)] = 2.0**2
		sq_error = sum(sq_error)
 	print 'Evaluated direction: A = %f, B = %f, C = %f, sq_error = %f' % (A, B, C, sq_error)
	#print time_error
	#print '   '
	#print sq_error
	return sq_error # return this value as this needs to be minimized

# Take away antennas with huge time delays (outliers)
exclude_list = []
# for multi-TBB, remove outliers per station, i.e. > +/- 200 ns from station median
stationStartIndex = f["STATION_STARTINDEX"]
for i in range(nofStations):
    start = stationStartIndex[i]
    end = stationStartIndex[i+1]
    thisStationMedian = np.median(lofar_delay[start:end])
    for j in range(start, end):
        if abs(lofar_delay[j] - thisStationMedian) > 200:
            exclude_list.append(j)

#for i in range(0,len(lofar_delay)):
#	if lofar_delay[i]<-200 or lofar_delay[i]>200:
#		exclude_list.append(i)

#if len(exclude_list) > 0:
#    lofar_delay[np.array(exclude_list)] = np.float('nan')
#lofar_exclude_delay = lofar_delay
#LOFAR times from the system without the outliers
lofar_exclude_delay = np.delete(lofar_delay,exclude_list)
north1 = np.delete(north,exclude_list)						#antenna positions excluding outlier antennas
east1 = np.delete(east,exclude_list)
height1 = np.delete(height,exclude_list)
#(north1, east1, height1) = (np.array(north), np.array(east), np.array(height))
print lofar_exclude_delay

#startPosition = [northocto+8., eastocto-8., hocto+9.9] # from octocopter GPS
#startPosition = [-1.1, -1.95, -2.9]
startPosition = [0.0, 0.0, 0.0]
print 'Optimum start'
#terror = sq_error_minimizer(startPosition, north0, east0, h0, north1, east1, height1, lofar_exclude_delay)
#optimum = scipy.optimize.fmin(sq_error_minimizer, startPosition, args=(north0, east0, h0, north1, east1, height1, lofar_exclude_delay),xtol=1e-5, ftol=1e-5)

# HACK no-fitting
optimum = [0.0, 0.0, 0.0]
# end hack

print east
print 'Octocopter position'
print array(startPosition)

print 'Fit values: N = %f, E = %f, h = %f' % (optimum[0], optimum[1], optimum[2])
A = optimum[0] + northocto
B = optimum[1] + eastocto
C = optimum[2] + hocto
x0 = north0
y0 = east0
z0 = h0
x1 = north1
y1 = east1
z1 = height1
distance_to_center = (np.sqrt( (A-x0)**2+(B-y0)**2+(C-z0)**2))/(constants.c*1e-9)
fit = ((np.sqrt( (A-x1)**2+(B-y1)**2+(C-z1)**2))/(constants.c*1e-9) - distance_to_center)

print 'Optimum-original'
print A-northocto, B-eastocto, C-hocto

#lofar_exclude_delay -= np.median(lofar_exclude_delay)

east1 = np.array(east1)-east0_station
east1 = list(east1)
north1 = np.array(north1)-north0_station
north1 = list(north1)

plt.figure()
plt.rcParams['xtick.major.pad']='11'
ax = subplot(111)
plt.subplots_adjust(left=0.15, right=1.03, top=0.9, bottom=0.1)
c = scatter(east1, north1, c=(fit-lofar_exclude_delay), marker='o',s=100,lw=0)
cbar=plt.colorbar()
cbar.set_label('Time delay [ns]')
locs,labels = xticks()
xticks(locs, map(lambda x: "%.0f" % x, locs))
xlabel('UTM East [m]')
locs,labels = yticks()
yticks(locs, map(lambda x: "%.0f" % x, locs))
ylabel('UTM North [m]')
ax.set_title('Fit-Lofar delay without outliers')

plt.figure()
plt.rcParams['xtick.major.pad']='11'
ax = subplot(111)
plt.subplots_adjust(left=0.15, right=1.03, top=0.9, bottom=0.1)
c = scatter(east1, north1, c=fit, marker='o',s=100,lw=0)
cbar=plt.colorbar()
cbar.set_label('Time delay [ns]')
locs,labels = xticks()
xticks(locs, map(lambda x: "%.0f" % x, locs))
xlabel('UTM East [m]')
locs,labels = yticks()
yticks(locs, map(lambda x: "%.0f" % x, locs))
ylabel('UTM North [m]')
ax.set_title('Times from fit')

print '-----------------------------------------------------------------------------'
print 'The fitted measured times'
print '     '
print fit

new_time_exp = np.delete(time_diff_exp,exclude_list)		#new time_diff_exp without the antennas with outliers in lofar_delay
#new_time_exp = np.array(time_diff_exp)
#new_time_exp[np.array(exclude_list)] = np.float('nan')

fit_fct = lofar_exclude_delay - fit
fit_fct -= np.median(fit_fct)
print '-----------------------------------------------------------------------------'
print 'The time differences, fitted, excluding outliers'
print '     '
print fit_fct

plt.figure()
plt.plot(fit_fct, 'b-o',linewidth=2)
#plt.plot(lofar_exclude_delay,'g-o')
#plt.plot(new_time_exp,'r-o')
#plt.plot(fit,'b-o')
#plt.plot(np.abs(lofar_exclude_delay-new_time_exp),'m-')
diff = lofar_exclude_delay - new_time_exp
diff -= np.median(diff)
#plt.plot(diff, 'm-')
plt.xlabel('Antenna number')
plt.ylabel('Time [ns]')
plt.grid(True,linewidth=3)
#plt.title('Fitted time delays')
#plt.legend(('Fit- (LOFAR delay)', 'Lofar delay', 'Expected diff', 'Fit', 'Meas-Exp'),'lower left', ncol=3,  shadow=True)
#leg = plt.gca().get_legend()
#ltext  = leg.get_texts()
#plt.setp(ltext, fontsize=12)

np.set_printoptions(precision=8)
print '-----------------------------------------------------------------------------'
print 'The expected time delays from the octocopter'
print '     '
print time_diff_exp

east = np.array(east)-east0_station
east = list(east)
north = np.array(north)-north0_station
north = list(north)

plt.figure(figsize=(11.1,9))
plt.rcParams['xtick.major.pad']='11'
ax = subplot(111, aspect='equal')
nlabels=(-50,-40,-30,-20,-10,0,10,20,30,40,50)
ax.set_yticks(nlabels)
labels=ax.set_yticklabels(nlabels)
plt.subplots_adjust(left=-1.3, right=1.2, top=0.97, bottom=0.12)
c = scatter(east, north, c=time_diff_exp,marker='o',s=250,lw=0, vmin = -75, vmax=190)
#c.set_alpha(0.75)
cbar=plt.colorbar(pad=0.01)
cbar.set_label('Time delay [ns]')
locs,labels = xticks()
xticks(locs, map(lambda x: "%.0f" % x, locs))
xlabel('Distance [m]')
locs,labels = yticks()
yticks(locs, map(lambda x: "%.0f" % x, locs))
ylabel('Distance [m]')
plt.xlim(-45,45)
plt.ylim(-45,45)
#ax.set_title('Time delay between stations with \n octocopter at original position')

print '-----------------------------------------------------------------------------'
print 'The measured time delays (LOFAR) without the outliers'
print '     '
print lofar_exclude_delay
plt.figure(figsize=(11.1,9))
plt.rcParams['xtick.major.pad']='11'
ax = subplot(111, aspect='equal')
nlabels=(-50,-40,-30,-20,-10,0,10,20,30,40,50)
ax.set_yticks(nlabels)
labels=ax.set_yticklabels(nlabels)
plt.subplots_adjust(left=-1.3, right=1.2, top=0.97, bottom=0.12)
c = scatter(east1, north1, c=lofar_exclude_delay, marker='o',s=250,lw=0, vmin=-75, vmax=190)
cbar=plt.colorbar(pad=0.01)
cbar.set_label('Time delay [ns]')
locs,labels = xticks()
xticks(locs, map(lambda x: "%.0f" % x, locs))
xlabel('Distance [m]')
locs,labels = yticks()
yticks(locs, map(lambda x: "%.0f" % x, locs))
ylabel('Distance [m]')
plt.xlim(-45,45)
plt.ylim(-45,45)

#ax.set_title('Time delay of the LOFAR system between stations')

print '-----------------------------------------------------------------------------'
print 'The time differences, not fitted, with outliers'
print '     '
diff_delay = time_diff_exp - lofar_delay #cr.hArray(maxima_cc.lags).toNumpy()
diff_delay -= np.median(diff_delay)
print diff_delay

# Get inter-station clock offsets remaining
interStationDelays = []
for i in range(nofStations):
    start = stationStartIndex[i]
    end = stationStartIndex[i+1]
    thisStationMedian = np.median(diff_delay[start:end])
    if i == 0:
        diff_delay -= thisStationMedian
        thisStationMedian = 0.0
    interStationDelays.append(thisStationMedian)

print ' --- Inter-station delays ---'
i = 0
for station in f["STATION_LIST"]:
    print station + ': %3.3f' % interStationDelays[i]
    i += 1

plt.figure()
plt.plot(time_diff_exp, 'ro')
plt.plot(lofar_delay, 'bo')
plt.plot(diff_delay, 'go')
plt.xlabel('Antenna number')
plt.ylabel('Time difference expected - measured [ns]')
plt.title('Delays')
plt.legend(('Expected times', 'Measured times (LOFAR)', 'Time difference'),'lower left', ncol=3,  shadow=True)
leg = plt.gca().get_legend()
ltext  = leg.get_texts()
plt.setp(ltext, fontsize=12)
plt.grid(True)

plt.figure()
plt.rcParams['xtick.major.pad']='11'
ax = subplot(111)
plt.subplots_adjust(left=0.15, right=1.03, top=0.9, bottom=0.1)
c = scatter(east, north, c=diff_delay, marker='o',s=100,lw=0, vmin=-7, vmax=20)
cbar=plt.colorbar()
cbar.set_label('Time delay [ns]')
locs,labels = xticks()
xticks(locs, map(lambda x: "%.0f" % x, locs))
xlabel('UTM East [m]')
locs,labels = yticks()
yticks(locs, map(lambda x: "%.0f" % x, locs))
ylabel('UTM North [m]')
ax.set_title('Time difference between expected and measured (not fitted)')

#plt.figure()
#plt.plot(distance, diff_delay, 'k.')
#plt.xlabel('Distance to central station antenna [m]')
#plt.ylabel('Time [ns]')
#plt.title('Delays')
#plt.grid(True)

#plt.savefig()
# plot lags, plot flagged lags from a k-sigma criterion on the crosscorr maximum

#plt.figure()
#cr.hArray(maxima_cc.lags).plot()

#plt.show()


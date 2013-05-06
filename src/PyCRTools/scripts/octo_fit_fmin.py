# Test script to get relative arrival times of pulses from the octocopter data.
# Started by Arthur Corstanje, Oct 2012.
# Added by Maria Krause, April 2013.
# At the end, the script tries to fit the curve of the distance to each antenna with the fmin function.
# The file 'octocopter_position' and 'octocopter_function' needs to be in the same directory
import matplotlib
#matplotlib.use('Agg')
#import pylab
#pylab.ion()
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
rcParams.update({'font.size': 24})

import sys
import os
sys.path.append(os.environ["LOFARSOFT"] + "/src/PyCRTools/scripts/")

#import octocopter_position as op
import octocopter_function as of

def locateFirstMaximum(timeseries_cut_to_pulse, resampleFactor=64, debug=True):
    timeseries_resampled = cr.hArray(float, dimensions=(nofChannels, blocksize * resampleFactor))
    cr.hFFTWResample(timeseries_resampled[...], timeseries_cut_to_pulse[...])

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
        plt.plot(y[3])
        plt.plot(y[2][1000:-1])
        #plt.plot(y[8][2000:-1])
        #plt.plot(y[8])
        #plt.plot(y[39])
        plt.figure()
        y = timeseries.toNumpy()
        plt.plot(y[3])
        plt.plot(y[2][1000:-1])
        #plt.plot(y[8][2000:-1])

    return timeOfMaximum

"""The octocopter timing file"""
filefilter = '/Volumes/WDdata/octocopter/L65331_D20120919T094039.104Z_CS003_R000_tbb.h5'
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
print f['ANTENNA_SET']
"""The LBA or HBA antenna position file"""
if f['ANTENNA_SET']=='LBA_OUTER':
	LBAfile = open(os.path.join(os.environ['LOFARSOFT'], 'data/lofar/antennapositions/LOFAR_ITRF_LBA_OUTER'),'r')
elif f['ANTENNA_SET']=='LBA_INNER':
	LBAfile = open(os.path.join(os.environ['LOFARSOFT'], 'data/lofar/antennapositions/LOFAR_ITRF_LBA_INNER'),'r')
elif f['ANTENNA_SET']=='LBA_X':
	LBAfile = open(os.path.join(os.environ['LOFARSOFT'], 'data/lofar/antennapositions/LOFAR_ITRF_LBA_X'),'r')
elif f['ANTENNA_SET']=='LBA_Y':
	LBAfile = open(os.path.join(os.environ['LOFARSOFT'], 'data/lofar/antennapositions/LOFAR_ITRF_LBA_Y'),'r')
elif f['ANTENNA_SET']=='HBA_DUAL':
	LBAfile = open(os.path.join(os.environ['LOFARSOFT'], 'data/lofar/antennapositions/LOFAR_ITRF_HBA_DUAL'),'r')
elif f['ANTENNA_SET']=='HBA_JOINED':
	LBAfile = open(os.path.join(os.environ['LOFARSOFT'], 'data/lofar/antennapositions/LOFAR_ITRF_HBA_JOINED'),'r')
elif f['ANTENNA_SET']=='HBA_ONE':
	LBAfile = open(os.path.join(os.environ['LOFARSOFT'], 'data/lofar/antennapositions/LOFAR_ITRF_HBA_ONE'),'r')
elif f['ANTENNA_SET']=='HBA_ZERO':
	LBAfile = open(os.path.join(os.environ['LOFARSOFT'], 'data/lofar/antennapositions/LOFAR_ITRF_HBA_ZERO'),'r')

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

###################################################################################
#		The octocopter position files
###################################################################################
#octofile = open('/home/maria/Dokumente/Master_Thesis/Calibration_data/Time_Program/Octocopter_position_file.txt','r')
f_sample = f['SAMPLE_FREQUENCY_VALUE']

# Get selected dipoles (station) and the observation time from the LOFAR HDF5 file
t_UTC = f['TIME']
ID = f['SELECTED_DIPOLES']

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
east0	        = trafo_values[8]
h0            = trafo_values[9]
distance      = trafo_values[10]

#selected_dipoles = [x for x in self.f["DIPOLE_NAMES"] if int(x) % 2 == self.pol]

# Read in calibration delays
try:
    cabledelays = cr.hArray(f["DIPOLE_CALIBRATION_DELAY"])
except Exception:
    raise StationSkipped("do not have DIPOLE_CALIBRATION_DELAY value")

nofChannels = f["NOF_SELECTED_DATASETS"]

# ...Timeseries analysis starts here...
timeseries = f.empty("TIMESERIES_DATA")

nofblocks = 10
thisBlock = 0
lofar_delay_allblocks = np.zeros( (nofblocks, nofChannels) )
shift = 0
datablocknr = 0
while thisBlock < nofblocks:
    datablocknr += int(shift / 2048)
    shift %= 2048
    print 'Doing block: %d, datablock = %d, shift = %d' % (thisBlock, datablocknr, shift)
    f.getTimeseriesData(timeseries, datablocknr, shift)
    #timeseries_cut_to_pulse[...].copy(timeseries[..., start:end])
    #timeseries_cut_to_pulse.square()

    timeOfMaximum = locateFirstMaximum(timeseries, debug=False)
    # Apply cable delays
    timeOfMaximum -= cabledelays

    if thisBlock == 0:
        #	Plot the original pulse
        y = timeseries.toNumpy()
        plt.figure()
        plt.plot(y[2])
        plt.plot(y[4])
        plt.figure()

        # now cross correlate all channels in full_timeseries, get relative times
        crosscorr = cr.trerun('CrossCorrelateAntennas', "crosscorr", timeseries_data=timeseries, oversamplefactor=16)

        y = crosscorr.crosscorr_data.toNumpy()
        plt.plot(y[0]) # will have maximum at 16 * blocksize / 2 (8192)
        plt.figure()
        plt.plot(y[2]) # will have a shifted maximum
        plt.title('Shifted Maximum')

        #	And determine the relative offsets between them
        sample_interval = 5.0e-9
        maxima_cc = cr.trerun('FitMaxima', "Lags", crosscorr.crosscorr_data, doplot = True, plotend=10, sampleinterval = sample_interval / crosscorr.oversamplefactor, peak_width = 11, splineorder = 3, refant = 0)

    # Measured time delays from LOFAR system
    #maxima_cc.lags *= 1e9
    maxima_cc.lags.copy(1e9 * timeOfMaximum.vec())
#    thisLofarDelay = cr.hArray(maxima_cc.lags).toNumpy()
    thisLofarDelay = 1e9 * timeOfMaximum.toNumpy()
    lofar_delay_allblocks[thisBlock] = thisLofarDelay

    thisBlock += 1
    shift += 1600
    # loop

#maxlags = maxima_cc.maxx
#maxlags= cr.hArray(maxlags).toNumpy()

plt.figure()
for i in range(nofblocks):
    plt.plot(lofar_delay_allblocks[i], 'bo')

# Take the average over all pulses processed.
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
	distance_to_center = (np.sqrt( (A-x0)**2+(B-y0)**2+(C-z0)**2))/(constants.c*1e-9)
	time_error = ((np.sqrt( (A-x1)**2+(B-y1)**2+(C-z1)**2))/(constants.c*1e-9) - distance_to_center)
	time_error -= times_from_lofar
	time_error -= np.median(time_error)
	#for i in range(0,len(time_error)):
	#	oneSampleShifts = int(round(time_error[i] / 5.)) # assuming time_error in nanoseconds
	#	time_error[i] -= 5.0 * oneSampleShifts
	if np.abs(A-A0)>10. or np.abs(B-B0)>10. or np.abs(C-C0)>1.:
		sq_error = 1.0e6
	# subtract 5-ns offsets NB! Assuming they are not data outliers but really one-sample shifts in time!
	else: # need to minimize square error (curve fit does this automatically)
		sq_error = (time_error)**2
		sq_error[np.argwhere(sq_error > 2.0**2)] = 2.0**2
		sq_error = sum(sq_error)
 	print 'Evaluated direction: A = %f, B = %f, C = %f, sq_error = %f' % (A, B, C, sq_error)
	#print time_error
	#print '   '
	print sq_error
	return sq_error # return this value as this needs to be minimized

#	Take away antennas with huge time delays (outliers)
exclude_list = []
for i in range(0,len(lofar_delay)):
	if lofar_delay[i]<-200 or lofar_delay[i]>200:
		exclude_list.append(i)

y_delay = np.delete(lofar_delay,exclude_list)
north1 = np.delete(north,exclude_list)						#antenna positions excluding outlier antennas
east1 = np.delete(east,exclude_list)
height1 = np.delete(height,exclude_list)
print y_delay

startPosition = [northocto, eastocto, hocto] # from octocopter GPS
print 'Optimum start'
#terror = sq_error_minimizer(startPosition, north0, east0, h0, north1, east1, height1, y_delay)
optimum = scipy.optimize.fmin(sq_error_minimizer, startPosition, args=(north0, east0, h0, north1, east1, height1, y_delay),xtol=1e-5, ftol=1e-5)
print east
print 'Octocopter position'
print array(startPosition)

print 'Fit values: N = %f, E = %f, h = %f' % (optimum[0], optimum[1], optimum[2])
A = optimum[0]
B = optimum[1]
C = optimum[2]
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

plt.figure()
plt.rcParams['xtick.major.pad']='11'
ax = subplot(111)
plt.subplots_adjust(left=0.15, right=1.03, top=0.9, bottom=0.1)
c = scatter(east1, north1, c=(fit-y_delay), marker='o',s=100,lw=0)
cbar=plt.colorbar()
cbar.set_label('Time delay [ns]')
locs,labels = xticks()
xticks(locs, map(lambda x: "%.0f" % x, locs))
xlabel('UTM East [m]')
locs,labels = yticks()
yticks(locs, map(lambda x: "%.0f" % x, locs))
ylabel('UTM North [m]')
ax.set_title('Fit-Lofar delay')

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
fit_fct = y_delay - fit
fit_fct -= np.median(fit_fct)
print '-----------------------------------------------------------------------------'
print 'The time differences, fitted, excluding outliers'
print '     '
print fit_fct

plt.figure()
plt.plot(fit_fct, 'k-o')
plt.plot(y_delay,'g-o')
plt.plot(new_time_exp,'r-o')
plt.plot(fit,'b-o')
#plt.plot(np.abs(y_delay-new_time_exp),'m-')
diff = y_delay - new_time_exp
diff -= np.median(diff)
plt.plot(diff, 'm-')
plt.xlabel('Antenna number')
plt.ylabel('Time [ns]')
plt.title('Fitted time delays')
plt.legend(('Fit- (LOFAR delay)', 'Lofar delay', 'Expected diff', 'Fit', 'Meas-Exp'),'lower left', ncol=3,  shadow=True)
leg = plt.gca().get_legend()
ltext  = leg.get_texts()
plt.setp(ltext, fontsize=12)
plt.grid(True)

np.set_printoptions(precision=8)
print '-----------------------------------------------------------------------------'
print 'The expected time delays from the octocopter'
print '     '
print time_diff_exp

plt.figure()
plt.rcParams['xtick.major.pad']='11'
ax = subplot(111)
plt.subplots_adjust(left=0.15, right=1.03, top=0.9, bottom=0.1)
c = scatter(east, north, c=time_diff_exp,marker='o',s=100,lw=0, vmin = -175, vmax=40)
#c.set_alpha(0.75)
cbar=plt.colorbar()
cbar.set_label('Time delay [ns]')
locs,labels = xticks()
xticks(locs, map(lambda x: "%.0f" % x, locs))
xlabel('UTM East [m]')
locs,labels = yticks()
yticks(locs, map(lambda x: "%.0f" % x, locs))
ylabel('UTM North [m]')
ax.set_title('Time delay between stations with \n octocopter at original position')

print '-----------------------------------------------------------------------------'
print 'The measured time delays (LOFAR)'
print '     '
print lofar_delay
plt.figure()
plt.rcParams['xtick.major.pad']='11'
ax = subplot(111)
plt.subplots_adjust(left=0.15, right=1.03, top=0.9, bottom=0.1)
c = scatter(east, north, c=lofar_delay, marker='o',s=100,lw=0, vmin=-170, vmax=40)
cbar=plt.colorbar()
cbar.set_label('Time delay [ns]')
locs,labels = xticks()
xticks(locs, map(lambda x: "%.0f" % x, locs))
xlabel('UTM East [m]')
locs,labels = yticks()
yticks(locs, map(lambda x: "%.0f" % x, locs))
ylabel('UTM North [m]')
ax.set_title('Time delay of the LOFAR system between stations')

print '-----------------------------------------------------------------------------'
print 'The time differences, not fitted'
print '     '
diff_delay = time_diff_exp - cr.hArray(maxima_cc.lags).toNumpy()
diff_delay -= np.median(diff_delay)
print diff_delay
plt.figure()
plt.plot(time_diff_exp, 'ro')
plt.plot(cr.hArray(maxima_cc.lags).toNumpy(), 'bo')
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


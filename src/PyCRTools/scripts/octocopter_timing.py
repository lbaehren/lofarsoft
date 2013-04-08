# Test script to get relative arrival times of pulses from the octocopter data.
# Started by Arthur Corstanje, Oct 2012.

import pycrtools as cr
import numpy as np
import math
import matplotlib.pyplot as plt
from pycrtools.tasks import crosscorrelateantennas, pulsecal
import os
from matplotlib import rc, rcParams
from scipy import constants
from pylab import *
latex_preamble = "\usepackage{cmbright}" # Use sans-serif (math)fonts
#latex_preamble = "\usepackage{cmbright}\Huge" # Use sans-serif (math)fonts
rcParams['text.usetex'] = True
rcParams['text.latex.preamble'] = latex_preamble
rcParams.update({'font.size': 24})

Rearth = 6373000


"""Conversion from latitude and longitude to UTM"""
#LatLong- UTM conversion.h
#definitions for lat/long to UTM and UTM to lat/lng conversions

_deg2rad = np.pi / 180.0
_rad2deg = 180.0 / np.pi

_EquatorialRadius = 2
_eccentricitySquared = 3

_ellipsoid = [
#  id, Ellipsoid name, Equatorial Radius, square of eccentricity
# first once is a placeholder only, To allow array indices to match id numbers
	[ -1, "Placeholder", 0, 0],
	[ 1, "Airy", 6377563, 0.00667054],
	[ 2, "Australian National", 6378160, 0.006694542],
	[ 3, "Bessel 1841", 6377397, 0.006674372],
	[ 4, "Bessel 1841 (Nambia] ", 6377484, 0.006674372],
	[ 5, "Clarke 1866", 6378206, 0.006768658],
	[ 6, "Clarke 1880", 6378249, 0.006803511],
	[ 7, "Everest", 6377276, 0.006637847],
	[ 8, "Fischer 1960 (Mercury] ", 6378166, 0.006693422],
	[ 9, "Fischer 1968", 6378150, 0.006693422],
	[ 10, "GRS 1967", 6378160, 0.006694605],
	[ 11, "GRS 1980", 6378137, 0.00669438],
	[ 12, "Helmert 1906", 6378200, 0.006693422],
	[ 13, "Hough", 6378270, 0.00672267],
	[ 14, "International", 6378388, 0.00672267],
	[ 15, "Krassovsky", 6378245, 0.006693422],
	[ 16, "Modified Airy", 6377340, 0.00667054],
	[ 17, "Modified Everest", 6377304, 0.006637847],
	[ 18, "Modified Fischer 1960", 6378155, 0.006693422],
	[ 19, "South American 1969", 6378160, 0.006694542],
	[ 20, "WGS 60", 6378165, 0.006693422],
	[ 21, "WGS 66", 6378145, 0.006694542],
	[ 22, "WGS-72", 6378135, 0.006694318],
	[ 23, "WGS-84", 6378137, 0.00669438]
]

#Reference ellipsoids derived from Peter H. Dana's website-
#http://www.utexas.edu/depts/grg/gcraft/notes/datum/elist.html
#Department of Geography, University of Texas at Austin
#Internet: pdana@mail.utexas.edu
#3/22/95

#Source
#Defense Mapping Agency. 1987b. DMA Technical Report: Supplement to Department of Defense World Geodetic System
#1984 Technical Report. Part I and II. Washington, DC: Defense Mapping Agency

#def LLtoUTM(int ReferenceEllipsoid, const double Lat, const double Long,
#			 double &UTMNorthing, double &UTMEasting, char* UTMZone)

def LLtoUTM(ReferenceEllipsoid, Lat, Long):
#converts lat/long to UTM coords.  Equations from USGS Bulletin 1532
#East Longitudes are positive, West longitudes are negative.
#North latitudes are positive, South latitudes are negative
#Lat and Long are in decimal degrees
#Written by Chuck Gantz- chuck.gantz@globalstar.com

    a = _ellipsoid[ReferenceEllipsoid][_EquatorialRadius]
    eccSquared = _ellipsoid[ReferenceEllipsoid][_eccentricitySquared]
    k0 = 0.9996

#Make sure the longitude is between -180.00 .. 179.9
    LongTemp = (Long+180)-int((Long+180)/360)*360-180 # -180.00 .. 179.9

    LatRad = Lat*_deg2rad
    LongRad = LongTemp*_deg2rad

    ZoneNumber = int((LongTemp + 180)/6) + 1

    if Lat >= 56.0 and Lat < 64.0 and LongTemp >= 3.0 and LongTemp < 12.0:
        ZoneNumber = 32

    # Special zones for Svalbard
    if Lat >= 72.0 and Lat < 84.0:
        if  LongTemp >= 0.0  and LongTemp <  9.0:ZoneNumber = 31
        elif LongTemp >= 9.0  and LongTemp < 21.0: ZoneNumber = 33
        elif LongTemp >= 21.0 and LongTemp < 33.0: ZoneNumber = 35
        elif LongTemp >= 33.0 and LongTemp < 42.0: ZoneNumber = 37

    LongOrigin = (ZoneNumber - 1)*6 - 180 + 3 #+3 puts origin in middle of zone
    LongOriginRad = LongOrigin * _deg2rad

    #compute the UTM Zone from the latitude and longitude
    UTMZone = "%d%c" % (ZoneNumber, _UTMLetterDesignator(Lat))

    eccPrime1 = (eccSquared)/(1-eccSquared)
    N = a/np.sqrt(1-eccSquared*np.sin(LatRad)*np.sin(LatRad))
    T = np.tan(LatRad)*np.tan(LatRad)
    C = eccPrime1*np.cos(LatRad)*np.cos(LatRad)
    A = np.cos(LatRad)*(LongRad-LongOriginRad)

    M = a*((1
            - eccSquared/4
            - 3*eccSquared*eccSquared/64
            - 5*eccSquared*eccSquared*eccSquared/256)*LatRad
           - (3*eccSquared/8
              + 3*eccSquared*eccSquared/32
              + 45*eccSquared*eccSquared*eccSquared/1024)*np.sin(2*LatRad)
           + (15*eccSquared*eccSquared/256 + 45*eccSquared*eccSquared*eccSquared/1024)*np.sin(4*LatRad)
           - (35*eccSquared*eccSquared*eccSquared/3072)*np.sin(6*LatRad))

    UTMEasting = (k0*N*(A+(1-T+C)*A*A*A/6
                        + (5-18*T+T*T+72*C-58*eccPrime1)*A*A*A*A*A/120)
                  + 500000.0)

    UTMNorthing = (k0*(M+N*np.tan(LatRad)*(A*A/2+(5-T+9*C+4*C*C)*A*A*A*A/24
                                        + (61
                                           -58*T
                                           +T*T
                                           +600*C
                                           -330*eccPrime1)*A*A*A*A*A*A/720)))

    if Lat < 0:
        UTMNorthing = UTMNorthing + 10000000.0; #10000000 meter offset for southern hemisphere
    return (UTMZone, UTMEasting, UTMNorthing)


def _UTMLetterDesignator(Lat):
#This routine determines the correct UTM letter designator for the given latitude
#returns 'Z' if latitude is outside the UTM limits of 84N to 80S
#Written by Chuck Gantz- chuck.gantz@globalstar.com

    if 84 >= Lat >= 72: return 'X'
    elif 72 > Lat >= 64: return 'W'
    elif 64 > Lat >= 56: return 'V'
    elif 56 > Lat >= 48: return 'U'
    elif 48 > Lat >= 40: return 'T'
    elif 40 > Lat >= 32: return 'S'
    elif 32 > Lat >= 24: return 'R'
    elif 24 > Lat >= 16: return 'Q'
    elif 16 > Lat >= 8: return 'P'
    elif  8 > Lat >= 0: return 'N'
    elif  0 > Lat >= -8: return 'M'
    elif -8> Lat >= -16: return 'L'
    elif -16 > Lat >= -24: return 'K'
    elif -24 > Lat >= -32: return 'J'
    elif -32 > Lat >= -40: return 'H'
    elif -40 > Lat >= -48: return 'G'
    elif -48 > Lat >= -56: return 'F'
    elif -56 > Lat >= -64: return 'E'
    elif -64 > Lat >= -72: return 'D'
    elif -72 > Lat >= -80: return 'C'
    else: return 'Z'	# if the Latitude is outside the UTM limits

"""The octocopter position file"""
#openfile = open('/home/maria/Dokumente/Master_Thesis/Calibration_data/Time_Program/Octocopter_position_file.txt','r')
openfile = open('/Users/acorstanje/usg/src/PyCRTools/scripts/Octocopter_position_file.txt','r')
openfile.next() # Skip file header lines

Time_array = []
Lat_array = []
Lon_array = []
Ele_array = []
WGS_array = []

a = 6378137					#Semi-major axis
b = 6356752.314245				#Semi-minor axis
fc = 0.0033528106718309896			#Flattening
e1 = 2*fc-fc*fc					#First eccentricity  e*e
e = np.sqrt(e1)
e2= (a*a-b*b)/(b*b)				#Second eccentricity e'*e'

for line in openfile:
	Time_array.append(float(line.split()[0]))
 	Lat_array.append(float(line.split()[1]))
 	Lon_array.append(float(line.split()[2]))
 	Ele_array.append(float(line.split()[3]))
 	WGS_array.append(float(line.split()[4]))
openfile.close()

"""Convert latitude/longitude to UTM"""
x_octo = np.zeros((len(Lat_array)))
y_octo = np.zeros((len(Lat_array)))
z_octo = np.zeros((len(Lat_array)))
u = np.zeros((len(Lat_array)))
for i in range(0,len(Lat_array)):
	u[i] = a/(np.sqrt(1-e1* math.pow(np.sin(math.radians(Lat_array[i])),2) ) )
	x_octo[i] = (u[i]+WGS_array[i]) * np.cos(math.radians(Lat_array[i])) * np.cos(math.radians(Lon_array[i]))
	y_octo[i] = (u[i]+WGS_array[i]) * np.cos(math.radians(Lat_array[i])) * np.sin(math.radians(Lon_array[i]))
	z_octo[i] = ((1-e1)*u[i]+WGS_array[i]) * np.sin(math.radians(Lat_array[i]))+Ele_array[i]

"""The octocopter timing file"""
#filefilter = '/Volumes/WDdata/octocopter/L65331_D20120919T094039.104Z_CS003_R000_tbb.h5'
filefilter = '/Users/acorstanje/triggering/CR/octocopter_test/L65331_D20120919T094039.104Z_CS003_R000_tbb.h5'

#Write all files into a list
#file_ar = os.listdir('/media/MyDrive/Masterarbeit/Calibration_data/Time_Measurements/Timing_Files_h5/data/')
#filenames = []
#for i in range(0,len(file_ar)):
#	filenames.append('/media/MyDrive/Masterarbeit/Calibration_data/Time_Measurements/Timing_Files_h5/data/{}'.format(file_ar[i]))

#filefilter = '/media/MyDrive/Masterarbeit/Calibration_data/Time_Measurements/Timing_Files_h5/data/L65343_D20120919T123204.519Z_CS002_R000_tbb.h5'

#filelist = cr.listFiles(filenames)
filelist = cr.listFiles(filefilter)
print filelist[0]
if len(filelist) == 1:
    filelist = filelist[0]
#print 'File is {}'.format(filelist)
f = cr.open(filelist, blocksize = 16384) # big blocks, then cut out pulses

f.setAntennaSelection('even')
#selected_dipoles = [x for x in self.f["DIPOLE_NAMES"] if int(x) % 2 == self.pol]


"""Start the analysis"""
#assert f["ANTENNA_SET"] == 'LBA_OUTER'

# cheap pulse finder; get pulse time from crossing level ADC = 500
timeseries = f["TIMESERIES_DATA"]

# Create FFTW plans
fftwplan = cr.FFTWPlanManyDftR2c(f["BLOCKSIZE"], 1, 1, 1, 1, 1, cr.fftw_flags.ESTIMATE)
ifftwplan = cr.FFTWPlanManyDftC2r(f["BLOCKSIZE"], 1, 1, 1, 1, 1, cr.fftw_flags.ESTIMATE)

# Read FFT data (without Hanning window)
fft_data = f.empty("FFT_DATA")

# Get corresponding frequencies
frequencies = cr.hArray(f["FREQUENCY_DATA"])

# Calculate FFT
cr.hFFTWExecutePlan(fft_data[...], timeseries[...], fftwplan)

# Normalize spectrum
fft_data /= f["BLOCKSIZE"]

# Reject DC component
fft_data[..., 0] = 0.0

# Also reject 1st harmonic (gives a lot of spurious power with Hanning window)
fft_data[..., 1] = 0.0

# Apply calibration delays
try:
    cabledelays = cr.hArray(f["DIPOLE_CALIBRATION_DELAY"])
except Exception:
    raise StationSkipped("do not have DIPOLE_CALIBRATION_DELAY value")

weights = cr.hArray(complex, dimensions=fft_data, name="Complex Weights")
phases = cr.hArray(float, dimensions=fft_data, name="Phases", xvalues=frequencies)

cr.hDelayToPhase(phases, frequencies, cabledelays)
cr.hPhaseToComplex(weights, phases)

fft_data.mul(weights)

# Go back to time domain
cr.hFFTWExecutePlan(timeseries[...], fft_data[...], ifftwplan)

#######################################################################################################################

#print f['DIPOLE_NAMES']
#print f['ANTENNA_POSITION_ITRF']

threshold = 500
firstPulsePosition = timeseries[...].findgreaterthan(threshold) # returns first position where y > 500
firstPulsePosition = cr.hArray(firstPulsePosition).toNumpy()		# convert array to numpy array
#print firstPulsePosition

noPulseCount = len(np.where(firstPulsePosition < 0)[0])
validPulses = firstPulsePosition[np.where(firstPulsePosition > 0)]
#print firstPulsePosition
first = min(validPulses)
last = max(validPulses)
avg = int(np.average(validPulses))
#print 'Pulse position first / last / avg: %d, %d, %d' % (first, last, avg)

nofChannels = f["NOF_SELECTED_DATASETS"]
#print 'No of channels = %d' % (nofChannels)

# Cut out a block around the pulse, store in new hArray
cutoutLength = 1024
start = avg - cutoutLength / 2
end = avg + cutoutLength / 2
if (end < last) or (start > first):
    raise ValueError('Blocksize too small for all pulses to fit in it!')

timeseries_cut_to_pulse = cr.hArray(float, [nofChannels, cutoutLength])
timeseries_cut_to_pulse[...].copy(timeseries[..., start:end])
#timeseries_cut_to_pulse.square()

y = timeseries_cut_to_pulse.toNumpy()
plt.figure()
plt.plot(y[2])
plt.plot(y[3])
plt.figure()
# may want to subtract the pulse offsets here; requires bookkeeping.
#timeseries_data_cut_to_pulse_offsets=cr.hArray(int,ndipoles,fill=(maxima_power.maxx+0.5-timeseries_data_cut_pulse_width/2))
#timeseries_data_cut_to_pulse[...].copy(pulse.timeseries_data_cut[..., timeseries_data_cut_to_pulse_offsets:timeseries_data_cut_to_pulse_offsets+timeseries_data_cut_pulse_width])

# now cross correlate all channels in full_timeseries, get relative times
crosscorr = cr.trerun('CrossCorrelateAntennas', "crosscorr", timeseries_data=timeseries_cut_to_pulse, oversamplefactor=16)

y = crosscorr.crosscorr_data.toNumpy()
plt.plot(y[0]) # will have maximum at 16 * blocksize / 2 (8192)
plt.figure()
plt.plot(y[2]) # will have a shifted maximum
plt.title('Shifted Maximum')

#And determine the relative offsets between them
sample_interval = 5.0e-9
maxima_cc = cr.trerun('FitMaxima', "Lags", crosscorr.crosscorr_data, doplot = True, plotend=10, sampleinterval = sample_interval / crosscorr.oversamplefactor, peak_width = 11, splineorder = 3, refant = 0)

#print startTimes
maxima_cc.lags *= 1e9
print 'Lags in Maxima in ns'
#print maxima_cc.lags
lofar_delay = maxima_cc.lags
lofar_delay = cr.hArray(lofar_delay).toNumpy()
print lofar_delay

#print maxima_cc.lags
maxlags = maxima_cc.maxx
maxlags= cr.hArray(maxlags).toNumpy()

f_sample = f['SAMPLE_FREQUENCY_VALUE']

"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""
Calculate the time delay measured with the octocopter
"""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

"""Get the position files of the antennas"""
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

"""Get antenna position, selected dipoles (station) and the observation time from the LOFAR HDF5 file"""
t_UTC = f['TIME']
#print f['ANTENNA_POSITION']
ID = f['SELECTED_DIPOLES']

"""Convert ITRF coordinates to latitude/ longitude followed by a conversion to UTM coordinates and
Calculate the time delay with the specific octocopter positions above the array (50m above central antenna of station) """
lon = np.zeros((len(X_array)))
lat = np.zeros((len(X_array)))
h_ant = np.zeros((len(X_array)))
east_ant = np.zeros((len(X_array)))
north_ant = np.zeros((len(X_array)))

t_exp = []					#expected time
time_diff = []			#Time difference with respect to central antenna
east = []						#UTM East
north = []					#UTM North
height = []					#Height

for j in range(0,len(ID_array)):
	for k in range(0,len(ID)):
		if ID_array[j][3:]=='000000' and ID_array[j]==ID[k]:
			lon0 = np.arctan(Y_array[j]/X_array[j])*_rad2deg
			p = np.sqrt(X_array[j]*X_array[j] + Y_array[j]*Y_array[j])
			r = np.sqrt(p*p + Z_array[j]*Z_array[j])
			mu = np.arctan(Z_array[j]/p * ( (1-fc)+e1*a/r ))*_rad2deg
			lat0 = np.arctan( (Z_array[j]*(1-fc)+e1*a*( pow(np.sin(mu*_deg2rad),3) ))/ ((1-fc)*(p-e1*a*pow(np.cos(mu*_deg2rad),3))) ) *_rad2deg
			h0 = p*np.cos(lat0*_deg2rad)+Z_array[j]*np.sin(lat0*_deg2rad)-a*np.sqrt(1-e1*pow(np.sin(lat0*_deg2rad),2))
			(z, east0, north0) = LLtoUTM(23, lat0, lon0)
			pos_east0 = east0-east0
			pos_north0 = north0-north0
			pos_height0 = h0-h0+50
			dist0 = np.sqrt( pow(pos_east0,2) + pow(pos_north0,2) + pow(pos_height0,2))
		if ID_array[j]==ID[k]:
			"""Calculate the time delay of octocopter to all antennas"""
			lon[j] = np.arctan(Y_array[j]/X_array[j])*_rad2deg
			p = np.sqrt(X_array[j]*X_array[j] + Y_array[j]*Y_array[j])
			r = np.sqrt(p*p + Z_array[j]*Z_array[j])
			mu = np.arctan(Z_array[j]/p * ( (1-fc)+e1*a/r ))*_rad2deg
			lat[j] = np.arctan( (Z_array[j]*(1-fc)+e1*a*( pow(np.sin(mu*_deg2rad),3) ))/ ((1-fc)*(p-e1*a*pow(np.cos(mu*_deg2rad),3))) ) *_rad2deg
			h_ant[j] = p*np.cos(lat[j]*_deg2rad)+Z_array[j]*np.sin(lat[j]*_deg2rad)-a*np.sqrt(1-e1*pow(np.sin(lat[j]*_deg2rad),2))
			(z, east_ant[j], north_ant[j]) = LLtoUTM(23, lat[j], lon[j])
			pos_east = east_ant[j]-east0
			pos_north = north_ant[j]-north0
			pos_height = h_ant[j]-h0+50
			distance = np.sqrt( pow(pos_east,2) + pow(pos_north,2) + pow(pos_height,2))
			texp = (distance-dist0)/(constants.c*1e-9)
			tdiff = np.abs(texp)
			time_diff.append(tdiff)
			east.append(east_ant[j])
			north.append(north_ant[j])
			height.append(h_ant[j])

print '-----------------------------------------------------------------------------'
print 'The theoretical/geometrical time delays'
print time_diff

#!/usr/bin/python
plt.figure()
plt.rcParams['xtick.major.pad']='11'
ax = subplot(111)
plt.subplots_adjust(left=0.15, right=1.03, top=0.9, bottom=0.1)
c = scatter(east, north, c=time_diff,marker='o',s=100,lw=0)
cbar=plt.colorbar()
cbar.set_label('Time delay [ns]')
locs,labels = xticks()
xticks(locs, map(lambda x: "%.0f" % x, locs))
xlabel('UTM East [m]')
locs,labels = yticks()
yticks(locs, map(lambda x: "%.0f" % x, locs))
ylabel('UTM North [m]')
ax.set_title('Time delay between stations with \n octocopter at 50 m above central antenna')


"""Calculate the time delay with the original octocopter positions above the array"""
t_exp = []					#expected time
time_diff = []
east = []
north = []
height = []

for i in range(0,len(Time_array)):
	if Time_array[i]==t_UTC[0]:
		"""Octocopter position"""
		(zocto, eastocto, northocto) = LLtoUTM(23, Lat_array[i], Lon_array[i])
		hocto = Ele_array[i]+WGS_array[i]
		for j in range(0,len(ID_array)):
			for k in range(0,len(ID)):
				if ID_array[j][3:]=='000000' and ID_array[j]==ID[k]:
					#print eastocto, northocto, hocto
					"""Central antenna position"""
					lon0 = np.arctan(Y_array[j]/X_array[j])*_rad2deg
					p = np.sqrt(X_array[j]*X_array[j] + Y_array[j]*Y_array[j])
					r = np.sqrt(p*p + Z_array[j]*Z_array[k])
					mu = np.arctan(Z_array[j]/p * ( (1-fc)+e1*a/r ))*_rad2deg
					lat0 = np.arctan( (Z_array[j]*(1-fc)+e1*a*( pow(np.sin(mu*_deg2rad),3) ))/ ((1-fc)*(p-e1*a*pow(np.cos(mu*_deg2rad),3))) ) *_rad2deg
					h0 = p*np.cos(lat0*_deg2rad)+Z_array[j]*np.sin(lat0*_deg2rad)-a*np.sqrt(1-e1*pow(np.sin(lat0*_deg2rad),2))
					(z, east0, north0) = LLtoUTM(23, lat0, lon0)
					pos_east0 = east0-eastocto
					pos_north0 = north0-northocto
					pos_height0 = h0-hocto
					dist0 = np.sqrt( pow(pos_east0,2) + pow(pos_north0,2) + pow(pos_height0,2))
					texp0 = dist0/(constants.c*1e-9)
				"""Each antenna position"""
				if ID_array[j]==ID[k]:
					#print east0, north0, h0, eastocto, northocto, hocto
					lon[j] = np.arctan(Y_array[j]/X_array[j])*_rad2deg
					p = np.sqrt(X_array[j]*X_array[j] + Y_array[j]*Y_array[j])
					r = np.sqrt(p*p + Z_array[j]*Z_array[j])
					mu = np.arctan(Z_array[j]/p * ( (1-fc)+e1*a/r ))*_rad2deg
					lat[j] = np.arctan( (Z_array[j]*(1-fc)+e1*a*( pow(np.sin(mu*_deg2rad),3) ))/ ((1-fc)*(p-e1*a*pow(np.cos(mu*_deg2rad),3))) ) *_rad2deg
					h_ant[j] = p*np.cos(lat[j]*_deg2rad)+Z_array[j]*np.sin(lat[j]*_deg2rad)-a*np.sqrt(1-e1*pow(np.sin(lat[j]*_deg2rad),2))
					(z, east_ant[j], north_ant[j]) = LLtoUTM(23, lat[j], lon[j])
					#print east_ant[j], north_ant[j], h_ant[j], eastocto, northocto, hocto
					pos_east = east_ant[j]-eastocto
					pos_north = north_ant[j]-northocto
					pos_height = h_ant[j]-hocto
					#print pos_east, pos_north, pos_height, pos_east0, pos_north0, pos_height0
					dist = np.sqrt( pow(pos_east,2) + pow(pos_north,2) + pow(pos_height,2))
					texp = dist/(constants.c*1e-9)
					tdiff = texp-texp0
					time_diff.append(tdiff)
					east.append(east_ant[j])
					north.append(north_ant[j])
					height.append(h_ant[j])

print '-----------------------------------------------------------------------------'
print 'The expected time delays'
print time_diff

#!/usr/bin/python
plt.figure()
plt.rcParams['xtick.major.pad']='11'
ax = subplot(111)
plt.subplots_adjust(left=0.15, right=1.03, top=0.9, bottom=0.1)
c = scatter(east, north, c=time_diff, vmin=-50, vmax=50,marker='o',s=100,lw=0)
#c.set_alpha(0.75)
cbar=plt.colorbar()
cbar.set_label('Time delay [ns]')
locs,labels = xticks()
xticks(locs, map(lambda x: "%.0f" % x, locs))
xlabel('UTM East [m]')
#xticks(locs, map(lambda x: "%.4f$^\circ$" % x, locs))
# yticks
locs,labels = yticks()
yticks(locs, map(lambda x: "%.0f" % x, locs))
ylabel('UTM North [m]')
ax.set_title('Time delay between stations with \n octocopter at original position')
#yticks(locs, map(lambda x: "%.4f$^\circ$" % x, locs))

plt.figure()
plt.plot(time_diff, maxima_cc.lags,'.')
plt.xlabel('Time delay from octocopter position [ns]')
plt.ylabel('Time delay from LOFAR system [ns]')
plt.title('Time differences')
plt.grid(True)

plt.figure()

y = time_diff - cr.hArray(maxima_cc.lags).toNumpy()
y -= np.median(y)
plt.plot(y, '.')
plt.xlabel('Antenna number')
plt.ylabel('Time difference expected - measured [ns]')
plt.title('Expected vs measured delays')
plt.grid(True)

#plt.savefig()
# plot lags, plot flagged lags from a k-sigma criterion on the crosscorr maximum

#plt.figure()
#cr.hArray(maxima_cc.lags).plot()

plt.show()
"""
sdev = crosscorr.crosscorr_data[...].stddev()
maxima = crosscorr.crosscorr_data[...].max() # need to look at positive maximum only!

ksigma = hArray(maxima / sdev)
k = ksigma.toNumpy()

x = np.where(k < 4.5) # bad indices
print 'Number of BAD channels: ', len(x[0])
if len(x[0]) > 0:
    y = hArray(maxima_cc.lags).toNumpy()
    yy = y[x]
    plt.scatter(x, yy)
    plt.title('Delays from cross correlations\nBlue dots represent flagged delays (no clear maximum)')

plt.figure()
hArray(ksigma).plot()
plt.title('Strength (in sigmas) of the crosscorrelation maximum')
"""

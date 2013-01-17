'''
This script is to test Beamformer.py

'''

import pycrtools as cr
from pytmf import *
from pycrtools import metadata
import math
import numpy as np
import pdb;# pdb.set_trace()


def strdate2jd(strdate):
    '''
    Transforms a date on string form ('2012-01-25 21:11:54') to julian day.
    '''
    import re
#   from pytmf import *

    date_all = [int(a) for a in re.findall(r'\w+',strdate)]
    date = [date_all[0],date_all[1], float(date_all[2])+date_all[3]/24.+date_all[4]/(24*60.)+date_all[5]/(24.*60*60)]
    utc = date2jd(date[0],date[1],date[2])
    return utc

pulsar = 1
altair = 0 #If you are running this program in altair.

if pulsar:
    cr.tload('BeamFormer')  #Using this since it seems the program runs faster when the task is preloaded (if other task is "loaded" then it runs slower).
    cr.treset()

    if altair:
#        f1=cr.open('/data/FRATS/data/L43784_D20120125T211154.887Z_CS002_R000_tbb.h5')
#        filefilter = '/data/FRATS/data/L43784_D20120125T211154.887Z_CS002_R000_tbb.h5'
#        f1=cr.open('/data/FRATS/data/L43784_D20120125T211154.871Z_CS006_R000_tbb.h5')
#        filefilter = '/data/FRATS/data/L43784_D20120125T211154.871Z_CS006_R000_tbb.h5'
#        f1=cr.open('/data/FRATS/data/L43784_D20120125T211154.867Z_CS003_R000_tbb.h5')
#        filefilter = '/data/FRATS/data/L43784_D20120125T211154.867Z_CS003_R000_tbb.h5'
#        f1=cr.open('/data/FRATS/data/L43784_D20120125T211154.887Z_CS004_R000_tbb.h5')
#        filefilter = '/data/FRATS/data/L43784_D20120125T211154.887Z_CS004_R000_tbb.h5'
#        f1=cr.open('/data/FRATS/data/L43784_D20120125T211154.866Z_CS005_R000_tbb.h5')
#        filefilter = '/data/FRATS/data/L43784_D20120125T211154.866Z_CS005_R000_tbb.h5'
        f1=cr.open('/data/FRATS/data/L43784_D20120125T211154.887Z_CS007_R000_tbb.h5')
        filefilter = '/data/FRATS/data/L43784_D20120125T211154.887Z_CS007_R000_tbb.h5'
        output_dir= '/Users/eenriquez/RESEARCH/Pulsars/Results/'
        f2=cr.open('/data/FRATS/data/L43784_D20120125T211154.887Z_CS007_R000_tbb.h5')

    else:
        f1=cr.open('~/RESEARCH/Pulsars/Data/L43784_D20120125T211154.887Z_CS002_R000_tbb.h5')
        filefilter = '~/RESEARCH/Pulsars/Data/L43784_D20120125T211154.887Z_CS002_R000_tbb.h5'
#        f1=cr.open('~/RESEARCH/Pulsars/Data/L43784_D20120125T211154.871Z_CS006_R000_tbb.h5')
#        filefilter = '~/RESEARCH/Pulsars/Data/L43784_D20120125T211154.871Z_CS006_R000_tbb.h5'
        output_dir= '~/RESEARCH/Pulsars/Results/'
        f2=cr.open('~/RESEARCH/Pulsars/Data/L43784_D20120125T211154.887Z_CS002_R000_tbb.h5')

#PSR B0329+54 -- Pulsar    0.71452 sec
#Wikipidea Values
    #alpha = hms2rad(3, 32, 59.368); # Right assention
    #delta = dms2rad(54, 34, 43.57); # Declination
#Simbad values
    alpha = hms2rad(03,32,59.37 ); # Right assention
    delta = dms2rad(54,34,44.9); # Declination

    phi = deg2rad(52.915122495)  #(LOFAR Superterp)
    L = deg2rad(6.869837540)

    utc = strdate2jd(f1['TIME_HR'][0])

    # Make input and output arrays for conversion
    equatorial = cr.hArray([alpha, delta])
    horizontal = equatorial.new()

    # Convert all coordinates in the input array
    # assumes difference between UTC and UT is 0 (hence ut1_utc=0.)
    cr.hEquatorial2Horizontal(horizontal, equatorial, utc, 0., L, phi)
    #horizontal2 = radec2azel(alpha,delta,utc,0.,L,phi)

    if horizontal[0] < 0:  horizontal[0] += 2.*math.pi  #Need to check the definitions used for positive azimut angles.

    azel = [horizontal[0],horizontal[1]]
    pdb.set_trace()
    NyquistZone = 2
    pointings=[{'az': azel[0], 'el': azel[1]}]
    maxchunklen = 2**20
    blocklen = 2**14
    dohanning =True

else:
    f1=cr.open('~/RESEARCH/VHECR/Data/L28348_D20110612T231913.199Z_CS002_R000_tbb.h5')
    filefilter = '~/RESEARCH/VHECR/Data/L28348_D20110612T231913.199Z_CS002_R000_tbb.h5'
#    f1=cr.open('~/RESEARCH/VHECR/Data/L35018_D20111120T014205.590Z_CS004_R000_tbb.h5')
#    filefilter = '~/RESEARCH/VHECR/Data/L35018_D20111120T014205.590Z_CS004_R000_tbb.h5'
    output_dir= '~/RESEARCH/VHECR/Results/'

#    azel_CR =[-46.9,45.4]
    azel_CR =[231.9,63.1]

    azel = [a*cr.deg for a in azel_CR]
    NyquistZone = 1
    pointings=[{'el': azel[1]+0*cr.deg, 'az': azel[0]}]#,{'el': azel[1]-5*cr.deg, 'az': azel[0]},{'el': azel[1], 'az': azel[0]-5*cr.deg}]
#    FarField = True ##This changes the output peak alot (False=nopeak)
    maxchunklen = 2**16*5
    blocklen = 2**10
    dohanning =False

#------------------------------
#Set basic beamformer parameters.
maxnchunks = 50
randomize_peaks = False
FarField = True
qualitycheck = False
plotspec = True
doplot = False
filenames = [filefilter]
nantennas_start = 0
nantennas_stride = 2
maxnantennas = f1['NOF_DIPOLE_DATASETS']
detail_name = '.pol1.multi_station'
single_station = True

#------------------------------
#Additional calibration values.

cal_delays=dict(zip(f1["DIPOLE_NAMES"],f1["DIPOLE_CALIBRATION_DELAY"]))

if pulsar:
    antenna_positions= cr.hArray(float,[96,3],f2['ANTENNA_POSITION_ITRF'])
antenna_set = f1['ANTENNA_SET']
channel = f1["CHANNEL_ID"]
sample_interval = f1["SAMPLE_INTERVAL"][0]

if pulsar:
    phase_center = cr.hArray(antenna_positions[nantennas_start].vec())

    antenna_positions= dict(zip(f1["DIPOLE_NAMES"],antenna_positions))
#tpar antenna_positions= dict(zip(f1["CHANNEL_ID"],f1.getITRFAntennaPositions()))
#clock_delay = metadata.getClockCorrection(f2['STATION_NAME'][0],f2['ANTENNA_SET']

#------------------------------
#Multi station starting times.
if pulsar:
    t0 = max(f2['SAMPLE_NUMBER'])*f2['SAMPLE_INTERVAL'][0]+f2['CLOCK_OFFSET'][0]
    t1 = max(f1['SAMPLE_NUMBER'])*f1['SAMPLE_INTERVAL'][0]+f1['CLOCK_OFFSET'][0]

#calculate delta samples, t0-f1[0][0]
    sample_offset = int((t0-t1)/f1['SAMPLE_INTERVAL'][0])

#----------------------------------
bm = cr.trun("BeamFormer",filefilter=filefilter,filenames = filenames,antenna_set='HBA1',output_dir=output_dir,pointings=pointings,FarField=FarField,NyquistZone=NyquistZone,cal_delays=cal_delays,randomize_peaks=randomize_peaks,qualitycheck=qualitycheck,plotspec=plotspec,doplot=doplot,nantennas_start = nantennas_start, nantennas_stride = nantennas_stride,maxnantennas = maxnantennas,maxchunklen=maxchunklen,blocklen=blocklen,dohanning=dohanning,detail_name=detail_name,single_station=single_station,test_beam_per_antenna=True)

#antenna_positions=antenna_positions ,,phase_center=phase_center,,sample_offset=sample_offset,,maxnchunks=maxnchunks)
#-------------------------------------------------------------------------------------

#Run for Beamformer Pulsar case
'''
from pycrtools import *
tpar filenames=['~/RESEARCH/Pulsars/Data/L43784_D20120125T211154.887Z_CS002_R000_tbb.h5']
tpar maxchunklen=2**20
tpar blocklen=2**14
tpar detail_name='.h5.pol1.multi_station'
dynspec,cleandynspec=Task.dyncalc(tbin=16,clean=True,save_file=True)

tpar NyquistZone=2
bm=hArrayRead('L43784_D20120125T211154.887Z_CS002_R000_tbb.h5.beams.pcr')
dmin=10e2;dmax=10e4
dynspec=Task.dyncalc(tbin=16*4)
Task.dynplot(plot_cleanspec=True,dmin=4,dmax=6,cmin=-.001,cmax=.007,from_file=True)
cr.plt.clf(); cr.plt.imshow(np.transpose(np.log(cleandynspec)),origin='lower',aspect='auto',vmin=-8,vmax=-2,extent=[0,1.024,100,200])
'''

#Run for Beamformer CR case
'''
from pycrtools import *
tpar filenames=['~/RESEARCH/VHECR/Data/L28348_D20110612T231913.199Z_CS002_R000_tbb.h5']
tpar maxchunklen=2**16*5
tpar blocklen=2**10
tpar detail_name='.pol0'
tpar NyquistZone=1
#dmin=-10e1;dmax=10e1
bm=hArrayRead('L35018_D20111120T014205.590Z_CS004_R000_tbb.h5.beams.pcr')'''


#Run for StationBeamformer
'''
from pycrtools import *
tload 'StationBeamformer'
tpar filename='L43784_D20120125T211154.887Z_CS002_R000_tbb.h5'
tpar blocksize=2**14
tpar antennas='even'
tpar coordinates=(pytmf.hms2deg(3, 32, 59.37),pytmf.dms2deg(54,34 , 44.9))
Task.init()
Task.run()
'''


#-------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------
#________________
#NOTE:
#If a file doesn't have calibration delay values use (check all pars first):
#tbbmd --antennaPositionDir /Users/emilio/lofarsoft/data/lofar/antennapositions/ --dipoleCalibrationDelayDir /Users/emilio/lofarsoft/data/lofar/dipole_calibration_delay/ --telescope LOFAR --projectID LEA036 --projectTitle FRATS --projectPI S.terVeen --projectCoI P.Schellart --projectContact s.terveen@astro.ru.nl --observationID unknown --clockFrequency 200. --clockFrequencyUnit "MHz" --antennaSet "HBA_DUAL" --filterSelection="HBA_110_190" --target 'PSR B0329+54' L43784_D20120125T211154.887Z_CS002_R000_tbb.h5

'''
This script is to test Beamformer.py

'''

import pycrtools as cr
from pytmf import *
from pycrtools import metadata
import math
import numpy as np

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
altair = 1 # if you are running this program in altair.

if pulsar:
    cr.tload('BeamFormer')  #Using this since it seems the program runs faster when the task is preloaded (if other task is "loaded" then it runs slower).
    cr.treset()

    if altair:
        f1=cr.open('/data/FRATS/data/L43784_D20120125T211154.887Z_CS002_R000_tbb.h5')
        filefilter = '/data/FRATS/data/L43784_D20120125T211154.887Z_CS002_R000_tbb.h5'
    else:    
        f1=cr.open('~/RESEARCH/Pulsars/Data/L43784_D20120125T211154.887Z_CS002_R000_tbb.h5')
        filefilter = '~/RESEARCH/Pulsars/Data/L43784_D20120125T211154.887Z_CS002_R000_tbb.h5'

#PSR B0329+54 -- Pulsar    
#Wikipidea Values
    #alpha = hms2rad(3, 32, 59.368); # Right assention
    #delta = dms2rad(54, 34, 43.57); # Declination        
#Simbad values
    alpha = hms2rad(03,32,59.37 ); # Right assention 
    delta = dms2rad(54,34,44.9); # Declination    
    
    phi = deg2rad(52.92)  #(LOFAR Superterp)
    L = deg2rad(6.87)
    
    utc = strdate2jd(f1['TIME_HR'][0])
    
    # Make input and output arrays for conversion
    equatorial = cr.hArray([alpha, delta])
    horizontal = equatorial.new()
    
    # Convert all coordinates in the input array
    # assumes difference between UTC and UT is 0 (hence ut1_utc=0.)
    cr.hEquatorial2Horizontal(horizontal, equatorial, utc, 0., L, phi)
    
    if horizontal[0] < 0:  horizontal[0] += 2.*math.pi  #Need to check the definitions used for positive azimut angles.
    
    azel = [horizontal[0],horizontal[1]]
    NyquistZone = 2
    pointings=[{'el': azel[1], 'az': azel[0]}]
    maxchunklen = 2**20
    blocklen = 2**14
    dohanning =False


else:
    f1=cr.open('~/RESEARCH/VHECR/Data/L35018_D20111120T014205.590Z_CS004_R000_tbb.h5')
    filefilter = '~/RESEARCH/VHECR/Data/L35018_D20111120T014205.590Z_CS004_R000_tbb.h5'
    azel_CR =[-46.9,45.4]
    azel = [a*cr.deg for a in azel_CR]
    NyquistZone = 1
    pointings=[{'el': azel[1], 'az': azel[0]}]#,{'el': azel[1]-5*cr.deg, 'az': azel[0]},{'el': azel[1], 'az': azel[0]-5*cr.deg}]
#    FarField = True ## what does this do?? ...since it changes the output peak alot (False=nopeak)
    maxchunklen = 2**16*5
    blocklen = 2**10
    dohanning =False

#------------------------------
antenna_positions=f1['ANTENNA_POSITIONS']
antenna_set = f1['ANTENNA_SET']
channel = f1["CHANNEL_ID"]
sample_interval = f1["SAMPLE_INTERVAL"][0]
phase_center = cr.hArray(antenna_positions[0].vec())
phase_center = phase_center

#tpar antenna_positions= dict(zip(f1["CHANNEL_ID"],f1.getITRFAntennaPositions()))
antenna_positions= dict(zip(f1["DIPOLE_NAMES"],antenna_positions))
cal_delays=dict(zip(f1["DIPOLE_NAMES"],f1["DIPOLE_CALIBRATION_DELAY"]))


#------------------------------
randomize_peaks = False
FarField = True
qualitycheck = False
plotspec = True
doplot = True
filenames = [filefilter]
nantennas_start = 1
nantennas_stride = 2
maxnantennas = f1['NOF_DIPOLE_DATASETS']
detail_name = '.test'
#----------------------------------

bm = cr.trun("BeamFormer",filefilter=filefilter,filenames = filenames,pointings=pointings,FarField=FarField,NyquistZone=NyquistZone,cal_delays=cal_delays,antenna_positions=antenna_positions,phase_center=phase_center,randomize_peaks=randomize_peaks,qualitycheck=qualitycheck,plotspec=plotspec,doplot=doplot,nantennas_start = nantennas_start, nantennas_stride = nantennas_stride,maxnantennas = maxnantennas,maxchunklen=maxchunklen,blocklen=blocklen,dohanning=dohanning,detail_name=detail_name)

#-------------------------------------------------------------------------------------
#from pycrtools import *

#tpar filenames=['~/RESEARCH/Pulsars/Data/L43784_D20120125T211154.887Z_CS002_R000_tbb.h5']
#tpar NyquistZone=2
#bm=hArrayRead('L43784_D20120125T211154.887Z_CS002_R000_tbb.h5.beams.pcr')
#tpar maxchunklen=2**20
#tpar blocklen=2**14
#dmin=10e2;dmax=10e4


'''tpar filenames=['~/RESEARCH/VHECR/Data/L35018_D20111120T014205.590Z_CS004_R000_tbb.h5']
tpar NyquistZone=1
tpar maxchunklen=2**16*5
tpar blocklen=2**10
#dmin=-10e1;dmax=10e1
bm=hArrayRead('L35018_D20111120T014205.590Z_CS004_R000_tbb.h5.beams.pcr')'''

#-------------------------------------------------------------------------------------
#-------------------------------------------------------------------------------------
#________________
#NOTE:
#If a file doesn't have calibration delay values use (check all pars first):
#tbbmd --antennaPositionDir /Users/emilio/lofarsoft/data/lofar/antennapositions/ --dipoleCalibrationDelayDir /Users/emilio/lofarsoft/data/lofar/dipole_calibration_delay/ --telescope LOFAR --projectID LEA036 --projectTitle FRATS --projectPI S.terVeen --projectCoI P.Schellart --projectContact s.terveen@astro.ru.nl --observationID unknown --clockFrequency 200. --clockFrequencyUnit "MHz" --antennaSet "HBA_DUAL" --filterSelection="HBA_110_190" --target 'PSR B0329+54' L43784_D20120125T211154.887Z_CS002_R000_tbb.h5

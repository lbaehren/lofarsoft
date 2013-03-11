#! /usr/bin/env python
"""
**Usage:**
execfile(PYP+"pipelines/frats_event.py")

This is the basic LOFAR FRATS event processing script, including RFI excision and beamforming.
To be added: gain calibration, pulse finding?

**Parameters:**
======================= =================================================================
*fname*                 filename of raw data to be processed.

*polarization*          either 0 or 1 for selecting even or odd antennas

*inner_tiles*           If True then it uses the inner 4 tiles of a HBA station, then substation is True.

*substation*            If True, then using HBA substations (HBA0 and HBA1)

*station_centered*      True if need phase center at (sub)station center, or False at LOFAR center.

*blocklen*              Block lenght, number of samples in powers of two.

======================= =================================================================

For more help on more parameters run ``frats_event.py --help``.

Uses beamformer in simple-calibrated TBB data.

Example::

python frats_event.py filename_tbb.h5

Note:
It needs environment variables: $BF_PARSETS, $FRATS_ANALYSIS, and $FRATS_ANALYSIS/trigger_info/
    In Coma:
        $BF_PARSETS   = /vol/astro/lofar/frats/bf/parsets
        $FRATS_ANALYSIS = /vol/astro/lofar/frats/tbb/analysis
    In CEP1:
        $BF_PARSETS   = /globalhome/lofarsystem/log/
        $FRATS_ANALYSIS = /staging1/frats/tbb/analysis/

.. moduleauthor:: J. Emilio Enriquez <e.enriquez 'at' astro.ru.nl>

Revision History:
V1.0 created by E. Enriquez, Feb 2013
"""

import matplotlib
matplotlib.use("Agg")

from optparse import OptionParser
import pycrtools as cr
from pytmf import *
import numpy as np
from pycrtools import tools
from pycrtools import bfdata as bf
from pycrtools.tasks import beamformer, findrfi
import os; import sys; import glob
import pdb;# pdb.set_trace()
#------------------------------------------------------

def make_list(option, opt_str, value, parser):
    setattr(parser.values, option.dest, value.split(','))

#------------------------------------------------------
#Command line options
#------------------------------------------------------
parser = OptionParser()

parser.add_option("-u","--substation",action="store_false",default=True,help="If True, then using HBA substations (HBA0 and HBA1).")
parser.add_option("-b","--blocklen",type="float",default=2**14,help="Block lenght")
parser.add_option("-i","--inner_tiles",action="store_true",default=False,help="If True then it uses the inner 4 tiles of a HBA station, then substation is True.")
parser.add_option("-s","--station_centered",action="store_true",default=False,help="True if need phase center at (sub)station center, or False at LOFAR center.")
parser.add_option("-p","--polarization",type="int",default=0,help="Polarization, 0 for even, 1 for odd antennas.")
parser.add_option("-f","--flag_antenna",type="str",action='callback',default=None,callback=make_list,help="List of antenna numbers to flag.")

(options, args) = parser.parse_args()

#----------------------------------------------------
if not parser.get_prog_name()=="frats_event.py":
    #   Program was run from within python
    substation = 1
    inner_tiles = 0
    polarization = 1
    station_centered = False
    blocklen = 2**14
else:
    substation = options.substation
    inner_tiles = options.inner_tiles
    polarization = options.polarization
    station_centered = options.station_centered
    blocklen = options.blocklen
    flag_antenna = options.flag_antenna

#----------------------------------------------------
#Open given TBB file.

fname = args[0]
file = cr.open(fname)

#----------------------------------------------------
#General parameters.

FRATS_ANALYSIS=os.environ["FRATS_ANALYSIS"].rstrip('/')+'/'
Obs_ID = fname.split('/')[-1].split('_')[0]
try:
    Trigger_info_file = glob.glob(FRATS_ANALYSIS+'trigger_info/'+Obs_ID+'*.npy')
except:
    raise IOError('No such file: ' + FRATS_ANALYSIS+'trigger_info/'+Obs_ID+'*.npy')

triggger_info = np.load(Trigger_info_file[0])

outdir = FRATS_ANALYSIS+Obs_ID

# Create output directory, if not already present.
if not os.path.isdir(outdir) or not os.path.isdir(outdir+'/beam.results/'):
    os.mkdirs(outdir+'/beam.results/')

outdir = FRATS_ANALYSIS+Obs_ID+'/beam.results/'

if station_centered:
    extra=''
else:
    extra='.LOFAR_centered'

if inner_tiles:
    substation = 1
    antenna_list = [17,19,29,31]
    antenna_list2 = list(cr.hArray(int,(4),antenna_list)+48)
    file['SELECTED_DIPOLES']=antenna_list+antenna_list2
else:
    poli=['even','odd']
    file['SELECTED_DIPOLES']=poli[polarization]

if flag_antenna:
    if flag_antenna[0] == '-1':
        file['SELECTED_DIPOLES'] = file['SELECTED_DIPOLES_INDEX'][:-1]
    else:
        antenna_list = np.array(file['SELECTED_DIPOLES_INDEX'])
        for flag in flag_antenna:
            antenna_list=antenna_list[np.arange(len(antenna_list))[(antenna_list!=int(flag))]]
        file['SELECTED_DIPOLES'] = list(antenna_list)

if substation:
    detail_name = '.pol%i_HBA0%s'%(polarization,extra)  #HBA0
    detail_name2 = '.pol%i_HBA1%s'%(polarization,extra)   #HBA1
else:
    detail_name = '.pol%i%s'%(polarization,extra)
    antenna_list = cr.hArray(int,file['NOF_DIPOLE_DATASETS'],file['SELECTED_DIPOLES_INDEX'])

file['BLOCKSIZE'] = blocklen

nantennas_start = polarization
utc = tools.strdate2jd(file['TIME_HR'][0])

beamnr = int(triggger_info['beam'])
dm = float(triggger_info['DM'])

#----------------------------------------------------
#Calibration
#----------------------------------------------------
#RFI excission.
# Find RFI and bad antennas

nobl=100  # Number of blocks
sbl=0    # Start block
rfi = cr.trun("FindRFI", f=file,nofblocks=nobl,verbose=False,startblock=sbl,freq_range=(110, 190),save_plots=True,plot_prefix=FRATS_ANALYSIS+Obs_ID+'/FindRFI_sbl_'+str(sbl)+'_nobl_'+str(nobl)+'_st_'+file['STATION_NAME'][0]+'_pol_'+str(polarization)+'_')

if rfi:
    file['SELECTED_DIPOLES'] = rfi.good_antennas
    rfi_channels = rfi.dirty_channels

if substation:
    index = cr.asval(cr.hFindGreaterThan(cr.hArray(file['SELECTED_DIPOLES_INDEX']),49))
    antenna_list = file['SELECTED_DIPOLES_INDEX'][0:index]
    antenna_list2 = file['SELECTED_DIPOLES_INDEX'][index:]

#----------------------------------------------------
#Calibrate station times. First step: Rounding the sample_number used to the nearest whole block since second start, including CLOCK_OFFSET.

sample_offset = cr.hArray(int,1,max(file['SAMPLE_NUMBER'])+int(file['CLOCK_OFFSET'][0]/file['SAMPLE_INTERVAL'][0]))
cr.hModulus(sample_offset, blocklen)

sample_offset = cr.asval(blocklen - sample_offset + int(file['CLOCK_OFFSET'][0]/file['SAMPLE_INTERVAL'][0]))

#----------------------------------------------------
#Beamform
#----------------------------------------------------
#Get pointing and phase center info.

BF_PARSETS=os.environ["BF_PARSETS"].rstrip('/')+'/'

fullparsetname=BF_PARSETS+Obs_ID+'.parset'
par=bf.get_parameters_new(fullparsetname,useFilename=True)
alpha = par['beam'][beamnr]['RA'] # Right assention
delta = par['beam'][beamnr]['DEC'] # Declination

phi = deg2rad(52.915122495) #(LOFAR Superterp)
L = deg2rad(6.869837540)

#----------------------------------------------------
# Make input and output arrays for conversion
equatorial = cr.hArray([alpha, delta])
horizontal = equatorial.new()

#----------------------------------------------------
# Convert all coordinates in the input array
# assumes difference between UTC and UT is 0 (hence ut1_utc=0.)
cr.hEquatorial2Horizontal(horizontal, equatorial, utc, 0., L, phi)

if horizontal[0] < 0:
    horizontal[0] += 2. * np.pi # Need to check the definitions used for positive azimut angles.

azel = [horizontal[0],horizontal[1]]
pointings = [{'az': azel[0], 'el': azel[1]}]
cal_delays = dict(zip(file["DIPOLE_NAMES"],file["DIPOLE_CALIBRATION_DELAY"]))

#----------------------------------------------------
if substation:
    antenna_set = 'HBA0'
    bm = cr.trun("BeamFormer", filenames = [fname,], output_dir = outdir, pointings = pointings, cal_delays = cal_delays, blocklen = blocklen,detail_name=detail_name,sample_offset=sample_offset,station_centered=station_centered,antenna_set=antenna_set,antenna_list=list(antenna_list),rfi_channels=rfi_channels)
    antenna_set = 'HBA1'
    bm2 = cr.trun("BeamFormer", filenames = [fname,], output_dir = outdir, pointings = pointings, cal_delays = cal_delays, blocklen = blocklen,detail_name=detail_name2,sample_offset=sample_offset,station_centered=station_centered,antenna_set=antenna_set,antenna_list=list(antenna_list2),rfi_channels=rfi_channels)
else:
    bm = cr.trun("BeamFormer", filenames = [fname,], output_dir = outdir, pointings = pointings, cal_delays = cal_delays, blocklen = blocklen,detail_name=detail_name,sample_offset=sample_offset,station_centered=station_centered,antenna_list=list(antenna_list),rfi_channels=rfi_channels)

#----------------------------------------------------


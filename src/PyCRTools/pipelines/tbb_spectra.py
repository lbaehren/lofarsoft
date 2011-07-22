#! /usr/bin/env python

"""
Pipeline script to calculate average and dynamic spectra from all TBB files.

Usage:
  tbb_spectra.py --filefilter 'datarate-2011-07-10-*.h5' --antennas_start=4 --plot_zoom_x1=87 --plot_zoom_x2=95

  cd /data/smilde
  run $LOFARSOFT/src/PyCRTools/pipelines/tbb_spectra.py  --filefilter '/data/sandertv/smilde/*.h5' --plot_zoom_x1=87 --plot_zoom_x2=95 --antennas_start=4 --output_dir=/data/smilde --maxnchunks=26

For quick testing: 
   run $PYP/pipelines/tbb_spectra.py --filefilter 'datarate-2011-07-10-*.h5' --plot_zoom_x1=87 --plot_zoom_x2=95 --antennas_start=4 --maxnchunks=4 --nfiles=2 --antennas_end=6
"""

from pycrtools import *
from optparse import OptionParser

#Define some default parameters
par = dict(
    t_int=0.002,
    delta_nu=1000,
    doplot=True,
    addantennas=False,
    calc_dynspec=True,
    write_html=True,
    lofarmode="LBA_OUTER",
    randomize_peaks=False,
    filefilter="*.h5",
    rmsrange_min=0.5,
    rmsrange_max=50
    )

additional_input_parameters=dict(
    nfiles=-1
    ) 

#Load task
avspec=tasks.averagespectrum.AverageSpectrum(pardict=par)

#Initialize command line option parser
usage = "usage: %prog [options] --filefilter='datafile-*.h5' --plot_zoom_x=(87,95) --antennas_start=4 ..."
parser = OptionParser(usage=usage)

input_parameters=avspec.ws.getInputParameters()  #.union(additional_input_parameters)
additional_input_parameter_names=additional_input_parameters.keys()

for parameter in input_parameters:
    val=avspec[parameter]
    print parameter,val
    if type(val)==bool or val==None:
        action="false" if val else "true"
        parser.add_option("--"+parameter,default=val,action="store_" +action,
                   help=avspec.ws.parameter_properties[parameter]["doc"]+" (will be set to "+action+"if provided)")
    else:
        parser.add_option("--"+parameter,default=val,type=typename(val),
                   help=avspec.ws.parameter_properties[parameter]["doc"])
        

[parser.add_option("--"+parameter,default=value,type=typename(value),help=avspec.ws.parameter_properties[parameter]["doc"])
 for parameter,value in additional_input_parameters.items()]

(options, args) = parser.parse_args()

#Create keywoard dict for all input parameters, include additional ones only if the have non-default values
kwargs={}
for parameter in input_parameters:
    kwargs[parameter]=getattr(options,parameter)

for parameter in additional_input_parameter_names:
    if not getattr(options,parameter)==additional_input_parameters[parameter]:
        kwargs[parameter]=getattr(options,parameter)

#Run task
avspec(**kwargs)



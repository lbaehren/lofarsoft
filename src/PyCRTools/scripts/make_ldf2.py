import pycrtools as cr
from pycrtools import lora
import os
import sys


# try:
#     topdir = sys.argv[1]
#     events = sys.argv[2]
# except IndexError:
#   raise SystemExit("Usage of this script: run make_ldf.py topdir events\n\n topdir: where to find files\n events: list of event ids")    

#execfile PYP+'/scripts/make_ldf2.py'
topdir='~/LOFAR/work/results'
events=["VHECR_LORA-20110716T094509.665Z"]
eventdirs=cr.listFiles([os.path.join(topdir,event) for event in events])

for eventdir in eventdirs:
    print "Processing event in directory:",eventdir
    par={}
    antid={0:[],1:[]}
    signal={0:[],1:[]}
    positions={0:[],1:[]}
    positions2={0:[],1:[]}
    ndipoles={0:0,1:0}

#    datadirs=cr.listFiles(os.path.join(os.path.join(eventdir,"pol0"),"*"))
    datadirs=cr.listFiles(os.path.join(os.path.join(eventdir,"pol?"),"*"))
    for datadir in datadirs:
        print "    Processing data results directory:",datadir
        res={}
        execfile(os.path.join(datadir,"results.py"),res)
        res=res["results"]
        antid[res["polarization"]].extend([int(v) for v in res["antennas"]])
        positions2[res["polarization"]].extend(res["antenna_positions_ITRF_m"])
        signal[res["polarization"]].extend(res["pulses_maxima_y"])
        ndipoles[res["polarization"]]+=res["ndipoles"]


    print "Number of dipoles found:",ndipoles
    
    par["loracore"]=cr.hArray(res["pulse_core_lora"])
    par["loradirection"]=cr.hArray(res["pulse_direction_lora"])
    par["loraenergy"]=res["pulse_energy_lora"]
    ##ATTENTION: You cannot assume that you always have an identical list of antennas in both polarisations!!

    par["positions"]=cr.metadata.get("AntennaPositions",antid[0],res["ANTENNA_SET"],return_as_hArray=True)
    par["positions2"]=cr.hArray(float,[ndipoles[0],3],positions2[0],name="Antenna Positions",units="m")
    #par["signalsNS"]=cr.hArray(signal[1])
    par["signalsNS"]=cr.hArray(signal[0])
    par["signalsEW"]=cr.hArray(signal[0])

    cr.plt.clf()
    cr.plt.subplot(1,2,1)
    layout=cr.trun("PlotAntennaLayout",pardict=par,positions=par["positions"],colors=par["signalsEW"],sizes=par["signalsEW"],sizes_min=0,title="Pulse Strength Power",plotlegend=False,newfigure=False,plot_clf=False)
    cr.plt.subplot(1,2,2)
    ldf=cr.trun("ldf",pardict=par)

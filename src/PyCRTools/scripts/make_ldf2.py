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
#events=["VHECR_LORA-20110716T094509.665Z"]
events=["VHECR_LORA-20110714T174749.986Z"]

eventdirs=cr.listFiles([os.path.join(topdir,event) for event in events])

for eventdir in eventdirs:
    print "Processing event in directory:",eventdir
    par={}
    antid={0:[],1:[]}
    signal={0:[],1:[]}
    positions={0:[],1:[]}
 #   positions2={0:[],1:[]}
    ndipoles={0:0,1:0}
    pol=0

    datadirs=cr.listFiles(os.path.join(os.path.join(eventdir,"pol?"),"*"))
#    datadirs.remove('/Users/falcke/LOFAR/work/results/VHECR_LORA-20110716T094509.665Z/pol1/005')
    for datadir in datadirs:
        print "    Processing data results directory:",datadir
        res={}
        execfile(os.path.join(datadir,"results.py"),res)
        res=res["results"]
        antid[res["polarization"]].extend([int(v) for v in res["antennas"]])
        positions[res["polarization"]].extend(res["antenna_positions_array_XYZ_m"])
        signal[res["polarization"]].extend(res["pulses_strength"])
        ndipoles[res["polarization"]]+=res["ndipoles"]


    print "Number of dipoles found:",ndipoles

    corepos=cr.hArray(res["pulse_core_lora"])
    par["loracore"]=corepos
    par["loradirection"]=cr.hArray(res["pulse_direction_lora"])
    par["loraenergy"]=res["pulse_energy_lora"]
    ##ATTENTION: You cannot assume that you always have an identical list of antennas in both polarisations!!


#    par["positions"]=cr.metadata.get("AntennaPositions",antid[pol],res["ANTENNA_SET"],return_as_hArray=True)
#    par["positions2"]=cr.hArray(float,[ndipoles[pol],3],positions2[pol],name="Antenna Positions",units="m")
    par["positions"]=cr.hArray(float,[ndipoles[pol],3],fill=positions[pol],name="Antenna Positions",units="m")
    #par["signalsNS"]=cr.hArray(signal[1])
    #par["signalsEW"]=cr.hArray(signal[0])
    par["signalsNS"]=cr.hArray(signal[pol])*100000.
    par["signalsEW"]=cr.hArray(signal[pol])*100000.

    corepos3D=hArray(float,3,fill=corepos)
    corepos3D[2]=par["positions"][0,2]

    cr.plt.clf()
    cr.plt.subplot(1,3,1)
    layout=cr.trun("PlotAntennaLayout",pardict=par,positions=par["positions"],colors=par["signalsEW"],sizes=par["signalsEW"],sizes_min=0,title="Pulse Strength Power",plotlegend=False,newfigure=False,plot_clf=False)
    cr.plt.plot(corepos[0],corepos[1],marker="x",color="blue",markersize=20,markeredgewidth=4)
    cr.plt.subplot(1,3,2)
    ldf=cr.trun("ldf",pardict=par)
    cr.plt.subplot(1,3,3)
    dist=hArray(float,par["positions"].shape()[0])
    dist.vectorseparation(corepos3D,par["positions"])
    par["signalsEW"].plot(xvalues=dist,marker="o",linestyle='None',clf=False,logplot="y")

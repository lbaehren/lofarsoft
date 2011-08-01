import pycrtools as cr
from pycrtools import lora
import os
import sys


try:
    topdir = sys.argv[1]
    candiates = sys.argv[2]
except IndexError:
  raise SystemExit("Usage of this script: run make_ldf.py topdir candidates which\n\n topdir: where to find files\n candidates: list of event ids\n which: specify index of candidates, if candiates > 1\n ")    


try:
    nr = sys.argv[3] 
except IndexError:
    nr = 0   

print nr
    
#topdir='/Volumes/Data/sandertv/VHECR/LORAtriggered/results/'
#topdir='/Volumes/Data/VHECR/LORAtriggered/results/'
#candidates=["VHECR_LORA-20110714T174749.986Z","VHECR_LORA-20110716T094509.665Z","VHECR_LORA-20110716T025339.597Z"]
#nr=1

datadirs=[topdir+d[0:-8] for d in os.listdir(topdir) if ".dir" in d and candidates[nr] in d]


par={}
antid=[]
signal={}
signal[0]=[]
signal[1]=[]

for datadir in datadirs:
        for pol in ["pol0.dir/","pol1.dir/"]:
		res={}
		execfile(datadir+pol+"/results.py",res)
		res=res["results"]
		if res["polarization"]==0:
			antid.extend([int(v) for v in res["antennas"]])
		signal[res["polarization"]].extend(res["pulses_maxima_y"])
		print datadir,res["pulse_start_sample"]

antset=res["ANTENNA_SET"]
if "TIME" not in res.keys():
    assert False
#res["TIME"]=1307920753 if "event-1" in res["FILENAME"] else None

lorainfo=lora.loraInfo(res["TIME"])
for k in ["core","direction","energy"]:
    par["lora"+k]=lorainfo[k]

positions=cr.metadata.get("AntennaPositions",antid,antset,return_as_hArray=True)
par["positions"]=positions
par["signalsNS"]=cr.hArray(signal[1])
par["signalsEW"]=cr.hArray(signal[0])

ldf=cr.trun("ldf",pardict=par)

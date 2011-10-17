nogui=True

import pycrtools as cr
import os


#datalocation="/data/VHECR/LORAtriggered/results"
datalocation = "/Users/annanelles/Uni/LOFAR/data/GOOD"
dirList=os.listdir(datalocation)

for fname in dirList:
    if "VHECR" in fname:
        Ev = [str(fname)]
        ldf=cr.trun("ldf",topdir=datalocation,events=Ev, plot_xmax=300,draw_global = False,Draw3D = False,save_images=True)
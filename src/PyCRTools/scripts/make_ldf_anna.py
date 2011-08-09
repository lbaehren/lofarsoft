import pycrtools as cr
from pycrtools import lora
import os
import sys


cr.plt.clf()

#default is altair data structure for topdir

ldf=cr.trun("ldf",topdir="/Users/annanelles/Uni/LOFAR/data",events=["VHECR_LORA-20110714T174749.986Z"])

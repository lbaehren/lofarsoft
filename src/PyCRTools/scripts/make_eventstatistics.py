import pycrtools as cr
from pycrtools import lora
import os
import sys

path = "/Volumes/MyBook/LOFAR_DATA/CR_RUN2"

Ev=['VHECR_LORA-20120120T184023.858Z']

statistics=cr.trun("eventstatistics",topdir=path,events=Ev)
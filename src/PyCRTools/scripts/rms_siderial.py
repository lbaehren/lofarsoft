import glob
import pycrtools as cr
import numpy as np
import matplotlib.pyplot as plt

from optparse import OptionParser

parser = OptionParser()
parser.add_option("-a", "--antenna", help="Antenna number.", default=0)
parser.add_option("-s", "--station", help="Station name (CS002, ...).", default="CS002")

(options, args) = parser.parse_args()

for filename in glob.iglob(options.directory+"*"+options.station+"*.h5"):
    print filename


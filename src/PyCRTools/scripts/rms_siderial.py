import pycrtools as cr
import numpy as np
import matplotlib.pyplot as plt

from optparse import OptionParser

parser = OptionParser()
parser.add_option("-a", "--antenna", help="Antenna calculate RMS from.")

(options, args) = parser.parse_args()

for file in args:
    print file


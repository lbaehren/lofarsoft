#!/usr/bin/env python
import numpy as np
import matplotlib.pyplot as plt
import pycrtools as cr
from pycrtools import lora
from optparse import OptionParser


def main():
    usage = "usage: %prog [options] arg"
    parser = OptionParser(usage)
    parser.add_option("-v", "--verbose", action="store_true", dest="verbose")
    parser.add_option("-s", "--lora-seconds", type="int")
    parser.add_option("-n", "--lora-nanoseconds", type="int")
    parser.add_option("-a", "--antenna", action="append", type=int)
    parser.add_option("--blocksize", type="int", default=2**16)
    parser.add_option("--before", type="int", default=0)
    parser.add_option("--after", type="int", default=0)
    parser.add_option("--block", type="int", default=None)
    parser.add_option("--output", type="string", default=None)

    (options, args) = parser.parse_args()

    if len(args) != 1:
        parser.error("incorrect number of arguments")

    filename = args[0]

    makeplot(filename,options.lora_seconds,options.lora_nanoseconds,options.block,options.blocksize,options.antenna,options.output)

def makeplot(filename,lora_seconds=0,lora_nanoseconds=0,blocksize=2**16,block=None,antenna=None,output=None):
    if not filename.endswith(".h5"):
        parser.error("not an HDF5 file")

    print filename

    f = cr.open(filename, blocksize)

    if antenna:
        f.setAntennaSelection(antenna)

    data = f.empty("TIMESERIES_DATA")

    print f["TIME"][0]
    if not block:
        block = lora.loraTimestampToBlocknumber(lora_seconds, lora_nanoseconds, f["TIME"][0], f["SAMPLE_NUMBER"][0], blocksize = blocksize)
    else:
        block=block
    
    print block
    f.getTimeseriesData(data, block)


    data[...].plot()
    if not output:
        plt.show()
    else:
        plt.savefig(output)

if __name__ == "__main__":
    main()


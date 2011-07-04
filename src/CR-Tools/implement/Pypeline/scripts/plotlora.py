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

    (options, args) = parser.parse_args()

    if len(args) != 1:
        parser.error("incorrect number of arguments")

    filename = args[0]

    if not filename.endswith(".h5"):
        parser.error("not an HDF5 file")

    f = cr.open(parser, options.blocksize)

    if options.antenna:
        f.setAntennaSelection(options.antenna)

    data = f.empty("TIMESERIES_DATA")

    print f["TIME"][0]
    block = lora.loraTimestampToBlocknumber(options.lora_seconds, options.lora_nanoseconds, f["TIME"][0], f["SAMPLE_NUMBER"][0], blocksize = options.blocksize)
    
    f.getTimeSeriesData(data, block)

    data = data.toNumpy()

    plt.plot(data)
    plt.show()

if __name__ == "__main__":
    main()


#!/usr/bin/env python

"""Program for ploting a spectrum from beam formed HDF5 files."""

import numpy


def rebin(vector, n):
    """
    Sum up n neighboring elements and return a new vector of length/n.
    Prerequisite: the length of vector/n has to be integer.
    """
    if (len(vector) % n):
        print "Length of 'vector' has to be an integer multiple of 'n'=%d!" % n
        return None
    newvector = numpy.reshape(vector, (len(vector)/n, n))
    return numpy.add.reduce(newvector, 1)





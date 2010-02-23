#! /usr/bin/env python

import sys
import string
import numpy

## Check Python version ____________________________________

def check_pyversion():
        version = string.split(string.split(sys.version)[0], ".")
        if map(int, version) < [2, 5, 0]:
                print "\nSorry: Python version must be at least 2.5\n"
                sys.exit(-1)

import pycr as cr

## Run the tests ___________________________________________

## Test construction

print "\n[1] Testing SkymapQuantity() ...\n"
quant = cr.SkymapQuantity()
quant.summary()

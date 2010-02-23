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

print "\n[1] Testing TimeFreq() ...\n"
ts1 = cr.TimeFreq()
ts1.summary()

print "\n[2] Testing TimeFreq(uint) ...\n"
ts2 = cr.TimeFreq(1024)
ts2.summary()

print "\n[3] Testing TimeFreq(uint,double,uint) ...\n"
ts3 = cr.TimeFreq(1024,200E06,2)
ts3.summary()

print "\n[4] Testing TimeFreq(uint,double,uint,double) ...\n"
ts4 = cr.TimeFreq(1024,200E06,2,-0.1)
ts4.summary()

## Test parameter access

ts5 = cr.TimeFreq()

print "\n[5] Testing setBlocksize ...\n"
ts5.setBlocksize(2048)
ts5.summary()

print "\n[6] Testing setSampleFrequency ...\n"
ts5.setSampleFrequency(160,"MHz")
ts5.summary()

print "\n[7] Testing setNyquistZone ...\n"
ts5.setNyquistZone(3)
ts5.summary()

print "\n[8] Testing setReferenceTime ...\n"
ts5.setReferenceTime(-0.123)
ts5.summary()

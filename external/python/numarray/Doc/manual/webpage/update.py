#!/usr/bin/env python

NUMARRAYDIR = '/home/groups/n/nu/numpy/htdocs/numarray/numarray-cvs'
PYTHONDIR = '/home/groups/n/nu/numpy/htdocs/numarray/python-cvs/dist/src/Doc'

CVS = 'cvs'
CVSOPT = ['-q', 'up', '-A', '-P', '-d']
MAKE = 'make'
MAKEOPT = ['dvi', 'html', 'dvi']

import os, sys

# update cvs sources
os.chdir(NUMARRAYDIR)
os.spawnvp(os.P_WAIT, CVS, [CVS] + CVSOPT)
os.chdir(PYTHONDIR)
os.spawnvp(os.P_WAIT, CVS, [CVS] + CVSOPT)

# build new docs
os.chdir(os.path.join(NUMARRAYDIR, 'Doc', 'manual'))
os.spawnvp(os.P_WAIT, MAKE, [MAKE] + MAKEOPT)



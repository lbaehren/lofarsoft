#! /usr/bin/env python 

from pycrtools import *
import os
import numpy as np
import time

dimensions=2
nrofelements=2**10
nrofrepeats=1000
func_np=np.sin
func_hf=hSin
#func_np=np.sqrt
#func_hf=hSqrt

if dimensions==2:
    x=hArray(float,[nrofelements,nrofelements])
    x.setHistory(False)
    hRandom(x,0,1)
    y=np.array(x.vec())
    y.resize(nrofelements,nrofelements)

if dimensions==1:
    x=hArray(float,[nrofelements])
    x.setHistory(False)
    hRandom(x,0,1)
    y=np.array(x.vec())
    
t0=time.time()
for i in range(nrofrepeats):
    func_hf(x)
t1=time.time()
th=t1-t0
print "Performing ",nrofrepeats," on a hArray of ",nrofelements," takes ",th," seconds." 

t0=time.time()
for i in range(nrofrepeats):
    func_np(y,y)
t1=time.time()
tnp=t1-t0
print "Performing ",nrofrepeats," on a numpy array of ",nrofelements," takes ",tnp," seconds." 
print "hArray takes ",th/tnp," as long."






import numpy as N
import compute as comp
import matplotlib
from matplotlib import pylab as pl
from math import exp

n=41
m=67
g=N.zeros(n*m).reshape(n,m)
oc = (0.0,0.0)

for i in range(n):
    for j in range(m):
        g[i,j]=exp(-0.5*((i-n/2-oc[0])*(i-n/2-oc[0])+(j-m/2-oc[1])*(j-m/2-oc[1]))/2./3./4.)

gs = comp.imageshift(g, (2.5, 5.5))

pl.clf()
pl.contour(g,20)
pl.contour(gs,20)


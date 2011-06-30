
import numpy as N
import functions as func
import pylab as pl

n = 1000
subn = 10
rnd = (N.random.random(subn)+0.5)*10000.0

x = N.arange(n)
y = 22.3 + 2.3*x 
err = (N.random.random(n/subn)-0.5)*rnd[0] 
for i in range(subn-1):
  err = N.append(err, (N.random.random(n/subn)-0.5)*rnd[i+1])
y = y + err

p, ep= func.fit_mask_1d(x, y, N.ones(len(x)), N.zeros(len(x), bool), func.poly, order=1)
print p, ep

x1 = []
y1 = []

for i in range(0,1000,100):
  xav = N.arange(i, i+100)
  x1.append(N.mean(xav))
  y1.append(N.mean(y[N.array(xav)]))
x1 = N.array(x1)
y1 = N.array(y1)

p1, ep1= func.fit_mask_1d(x1, y1, N.ones(len(x1)), N.zeros(len(x1), bool), func.poly, order=1)
print p1, ep1

pl.subplot(2,1,1)
pl.plot(x,y,'*-r')
pl.plot(x,p[0]+p[1]*x,'-r')
pl.subplot(2,1,2)
pl.plot(x1,y1,'*-g')
pl.plot(x1,p1[0]+p1[1]*x1,'-g')



from numpy import *
from scipy import interpolate
import matplotlib
import matplotlib.pylab as plt

x,y = mgrid[-1:1:20j,-1:1:20j]
z = (x+y)*exp(-6.0*(x*x+y*y))
z = 1.0*sin(8.2*x+0.1)+1.2*cos(5.7*y+0.3)


plt.figure()
plt.pcolor(x,y,z)
plt.colorbar()
plt.title("Sparsely sampled function.")
plt.show()

xp=x.reshape(400)
yp=y.reshape(400)
zp=z.reshape(400)

xnew,ynew = mgrid[-1:1:70j,-1:1:70j]
zt = 1.0*sin(8.2*xnew+0.1)+1.2*cos(5.7*ynew+0.3)

tck = interpolate.bisplrep(xp,yp,zp,s=0)
znew = interpolate.bisplev(xnew[:,0],ynew[0,:],tck)

zres=zt-znew

plt.figure()
plt.pcolor(xnew,ynew,zres)
plt.colorbar()
plt.title("Interpolated function.")
plt.show()



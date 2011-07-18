import matplotlib as mpl
import matplotlib.pyplot as plt
import pycrtools as cr
import numpy as np

blocksize = 100
a = cr.hArray(float, blocksize)
b = a.new()
c = a.new()
d = a.new()

cr.hGetHanningFilter(a)
cr.hGetHanningFilter(b,0.4)
cr.hGetHanningFilter(c,0.5, 40)
cr.hGetHanningFilter(d,0.5, 10, 20, 40)

plt.close()

plt.title(r"Testing Hanning functions")
plt.xlabel("Index")
plt.ylabel("Value ")

plt.plot(a.toNumpy(),label=r"a: default ($\alpha=0.5, \beta=0$)")
plt.plot(b.toNumpy(),label=r"b: $\alpha=0.4$")
plt.plot(c.toNumpy(),label=r"c: $\beta=40$")
plt.plot(d.toNumpy(),label=r"d: $\beta_{\rm rise}=20, \beta_{\rm fall}=40$")

plt.legend(loc='lower center')

plt.show()
